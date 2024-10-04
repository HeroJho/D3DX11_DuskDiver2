#include "../Default/stdafx.h"
#include "..\Public\DataManager.h"
#include "GameInstance.h"

#include "ToolManager.h"
#include "PxObj.h"
#include "ModelObj_Instance.h"
#include "NPC_T.h"
#include "Camera_Manager.h"
#include "MarkCube.h"
#include "LookCube_HR.h"


IMPLEMENT_SINGLETON(CDataManager)

CDataManager::CDataManager()
{
}

HRESULT CDataManager::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	if (FAILED(LoadModelPass()))
		return E_FAIL;

	if (FAILED(LoadTexturePass()))
		return E_FAIL;

	return S_OK;
}



HRESULT CDataManager::LoadModelPass()
{
	// ../Bin/Resources/Meshes/Anim/모델명
	// ../Bin/Resources/Meshes/NoneAnim/모델명

	// 경로를 다 읽어온다.
	WIN32_FIND_DATA data;

	HANDLE hFind = FindFirstFile(TEXT("../Bin/Resources/Meshes/NonAnim//*"), &data);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do {
			// TC -> C
			char temp[MAX_PATH];
			CToolManager::Get_Instance()->TCtoC(data.cFileName, temp);

			m_NonAnimFilePaths.push_back(temp);

		} while (FindNextFile(hFind, &data) != 0);

		FindClose(hFind);
	}

	m_NonAnimFilePaths.pop_front();
	m_NonAnimFilePaths.pop_front();

	hFind = FindFirstFile(TEXT("../Bin/Resources/Meshes/Anim//*"), &data);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do {
			// TC -> C
			char temp[MAX_PATH];
			CToolManager::Get_Instance()->TCtoC(data.cFileName, temp);

			m_AnimFilePaths.push_back(temp);

		} while (FindNextFile(hFind, &data) != 0);

		FindClose(hFind);
	}

	m_AnimFilePaths.pop_front();
	m_AnimFilePaths.pop_front();

	hFind = FindFirstFile(TEXT("../Bin/Resources/Meshes/Bone//*"), &data);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do {
			// TC -> C
			char temp[MAX_PATH];
			CToolManager::Get_Instance()->TCtoC(data.cFileName, temp);

			m_BoneFilePaths.push_back(temp);

		} while (FindNextFile(hFind, &data) != 0);

		FindClose(hFind);
	}

	m_BoneFilePaths.pop_front();
	m_BoneFilePaths.pop_front();

	return S_OK;
}

HRESULT CDataManager::LoadTexturePass()
{
	WIN32_FIND_DATA data;

	HANDLE hFind = FindFirstFile(TEXT("../Bin/Resources/Particle//*"), &data);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do {
			// TC -> C
			char temp[MAX_PATH];
			CToolManager::Get_Instance()->TCtoC(data.cFileName, temp);

			m_TexturePaths.push_back(temp);

		} while (FindNextFile(hFind, &data) != 0);

		FindClose(hFind);
	}

	m_TexturePaths.pop_front();
	m_TexturePaths.pop_front();

	return S_OK;
}


HRESULT CDataManager::ReadSceneData(char * pFileName, DATA_HEROSCENE* ReadScene, DATA_TYPE eTYPE)
{
	char cPullName[MAX_PATH];
	if (DATA_ANIM == eTYPE)
		strcpy_s(cPullName, "../Data/Bin_Model/Anim/");
	else if (DATA_NOEANIM == eTYPE)
		strcpy_s(cPullName, "../Data/Bin_Model/NonAnim/");
	else if (DATA_BONE == eTYPE)
		strcpy_s(cPullName, "../Data/Bin_Model/Bone/");

	strcat_s(cPullName, pFileName);


	ZeroMemory(ReadScene, sizeof(DATA_HEROSCENE));
	std::ifstream ifs(cPullName, ios::in | ios::binary);

	if (!ifs)
		return E_FAIL;

	// Node
	int iNodeCount = 0;
	ifs.read((char*)&iNodeCount, sizeof(int));
	ReadScene->iNodeCount = iNodeCount;
	ReadScene->pHeroNodes = new DATA_HERONODE[iNodeCount];
	for (int i = 0; i < iNodeCount; ++i)
	{
		DATA_HERONODE* pHeroNode = &ReadScene->pHeroNodes[i];
		ifs.read((char*)pHeroNode, sizeof(DATA_HERONODE));
	}

	// Material
	int iMaterialCount = 0;
	ifs.read((char*)&iMaterialCount, sizeof(int));
	ReadScene->iMaterialCount = iMaterialCount;
	ReadScene->pHeroMaterial = new DATA_HEROMATERIAL[iMaterialCount];
	for (int i = 0; i < iMaterialCount; ++i)
	{
		DATA_HEROMATERIAL* pHeroMarterial = &ReadScene->pHeroMaterial[i];
		ifs.read((char*)pHeroMarterial, sizeof(DATA_HEROMATERIAL));
	}

	// MashNode
	int iMashCount = 0;
	ifs.read((char*)&iMashCount, sizeof(int));
	ReadScene->iMeshCount = iMashCount;
	ReadScene->pHeroMesh = new DATA_HEROMETH[iMashCount];
	for (int i = 0; i < iMashCount; ++i)
	{
		DATA_HEROMETH* pHeroMash = &ReadScene->pHeroMesh[i];

		ifs.read((char*)&pHeroMash->cName, sizeof(char)*MAX_PATH);
		ifs.read((char*)&pHeroMash->iMaterialIndex, sizeof(int));

		ifs.read((char*)&pHeroMash->NumVertices, sizeof(int));

		int iIsAnim = 0;
		ifs.read((char*)&iIsAnim, sizeof(int));
		if (iIsAnim)
		{
			pHeroMash->pAnimVertices = new VTXANIMMODEL[pHeroMash->NumVertices];
			pHeroMash->pNonAnimVertices = nullptr;
			for (int j = 0; j < pHeroMash->NumVertices; ++j)
			{
				VTXANIMMODEL* VtxAniModel = &pHeroMash->pAnimVertices[j];
				ifs.read((char*)VtxAniModel, sizeof(VTXANIMMODEL));
			}
		}
		else
		{
			pHeroMash->pNonAnimVertices = new VTXMODEL[pHeroMash->NumVertices];
			pHeroMash->pAnimVertices = nullptr;
			for (int j = 0; j < pHeroMash->NumVertices; ++j)
			{
				VTXMODEL* VtxNonAniModel = &pHeroMash->pNonAnimVertices[j];
				ifs.read((char*)VtxNonAniModel, sizeof(VTXMODEL));
			}
		}

		pHeroMash->iNumPrimitives = 0;
		ifs.read((char*)&pHeroMash->iNumPrimitives, sizeof(int));
		pHeroMash->pIndices = new FACEINDICES32[pHeroMash->iNumPrimitives];
		for (int j = 0; j < pHeroMash->iNumPrimitives; ++j)
		{
			FACEINDICES32* Indices32 = &pHeroMash->pIndices[j];
			ifs.read((char*)Indices32, sizeof(FACEINDICES32));
		}


		pHeroMash->iNumBones = 0;
		ifs.read((char*)&pHeroMash->iNumBones, sizeof(int));
		pHeroMash->pBones = new DATA_HEROBONE[pHeroMash->iNumBones];
		for (int j = 0; j < pHeroMash->iNumBones; ++j)
		{
			DATA_HEROBONE* bon = &pHeroMash->pBones[j];
			ifs.read((char*)bon, sizeof(DATA_HEROBONE));
		}
	}


	// Animation
	int iNumAnimations = 0;
	ifs.read((char*)&iNumAnimations, sizeof(int));
	ReadScene->iNumAnimations = iNumAnimations;
	ReadScene->pHeroAnim = new DATA_HEROANIM[iNumAnimations];

	for (int i = 0; i < iNumAnimations; ++i)
	{
		ifs.read((char*)&ReadScene->pHeroAnim[i].iNumChannels, sizeof(int));
		ifs.read((char*)&ReadScene->pHeroAnim[i].fDuration, sizeof(float));
		ifs.read((char*)&ReadScene->pHeroAnim[i].fTickPerSecond, sizeof(float));
		ifs.read((char*)&ReadScene->pHeroAnim[i].bLoop, sizeof(bool));
		ifs.read((char*)&ReadScene->pHeroAnim[i].szName, sizeof(char) * MAX_PATH);

		ReadScene->pHeroAnim[i].pHeroChannel = new DATA_HEROCHANNEL[ReadScene->pHeroAnim[i].iNumChannels];
		for (int j = 0; j < ReadScene->pHeroAnim[i].iNumChannels; ++j)
		{
			DATA_HEROCHANNEL* pChannel = &ReadScene->pHeroAnim[i].pHeroChannel[j];
			ifs.read((char*)&pChannel->szName, sizeof(char)*MAX_PATH);
			ifs.read((char*)&pChannel->iNumKeyFrames, sizeof(int));

			pChannel->pKeyFrames = new KEYFRAME[pChannel->iNumKeyFrames];
			for (int k = 0; k < pChannel->iNumKeyFrames; ++k)
			{
				KEYFRAME* pKeyFrame = &pChannel->pKeyFrames[k];
				ifs.read((char*)pKeyFrame, sizeof(KEYFRAME));
			}
		}

		ifs.read((char*)&ReadScene->pHeroAnim[i].iNumTimeLimit, sizeof(int));
		if (0 < ReadScene->pHeroAnim[i].iNumTimeLimit)
		{
			ReadScene->pHeroAnim[i].pTimeLimits = new float[ReadScene->pHeroAnim[i].iNumTimeLimit];
			for (int j = 0; j < ReadScene->pHeroAnim[i].iNumTimeLimit; ++j)
			{
				ifs.read((char*)&ReadScene->pHeroAnim[i].pTimeLimits[j], sizeof(float));
			}
		}
		else
		{
			ReadScene->pHeroAnim[i].pTimeLimits = nullptr;
		}

	}


	ifs.close();
	return S_OK;
}
HRESULT CDataManager::Create_Try_BinModel(const _tchar * pModelName, LEVEL eLEVEL, DATA_TYPE eTYPE, _fmatrix mPivot)
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	//// 원본 체크
	//if (FAILED(pGameInstance->Check_Prototype(eLEVEL, pModelName)))
	//{
	//	RELEASE_INSTANCE(CGameInstance);
	//	return S_OK;
	//}

	TCHAR* pModelNameTemp = CToolManager::Get_Instance()->Get_ManagedTChar();
	lstrcpy(pModelNameTemp, pModelName);

	char cTempName[MAX_PATH];
	CToolManager::Get_Instance()->TCtoC(pModelName, cTempName);

	char* tPath = CToolManager::Get_Instance()->Get_ManagedChar();
	if (DATA_ANIM == eTYPE)
		strcpy(tPath, "../Bin/Resources/Meshes/Anim/");
	else if (DATA_NOEANIM == eTYPE)
		strcpy(tPath, "../Bin/Resources/Meshes/NonAnim/");
	else if(DATA_BONE == eTYPE)
		strcpy(tPath, "../Bin/Resources/Meshes/Bone/");

	strcat(tPath, cTempName);
	strcat(tPath, "/");

	char* tFileName = CToolManager::Get_Instance()->Get_ManagedChar();
	strcpy(tFileName, cTempName);
	char TempName[MAX_PATH];
	strcpy(TempName, tFileName);
	strcat(tFileName, ".fbx");


	// Bin 체크 And Load
	DATA_HEROSCENE* Scene = new DATA_HEROSCENE;
	ZeroMemory(Scene, sizeof(DATA_HEROSCENE));
	_bool bIsBin = true;
	if (FAILED(CDataManager::Get_Instance()->ReadSceneData(TempName, Scene, eTYPE)))
	{
		bIsBin = false;
		Safe_Delete(Scene);
	}
	

	_matrix PivotMatrix = mPivot;
	if (!strcmp("SM_Metro_Trashbin1", cTempName) ||
		!strcmp("SM_Metro_Trashbin2", cTempName) ||
		!strcmp("SM_Metro_Trashbin3", cTempName))
		PivotMatrix = XMMatrixScaling(10.f, 10.f, 10.f);
	else if (!strcmp("SM_PR_IronBoxA", cTempName)	||
			 !strcmp("SM_PR_PalletA", cTempName))
		PivotMatrix = XMMatrixScaling(2.f, 2.f, 2.f);
	else if(!strcmp("SM_PR_RoadblockF", cTempName) ||
			!strcmp("SM_PR_CabinetC", cTempName))
		PivotMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	else if(!strcmp("SM_PR_BoxAe", cTempName) ||
			!strcmp("SM_PR_BoxAf", cTempName))
		PivotMatrix = XMMatrixScaling(3.f, 3.f, 3.f);
	else if (!strcmp("SM_PR_ParkbenchC", cTempName) ||
		!strcmp("SM_PR_GuideSignA", cTempName) || 
		!strcmp("SM_PR_benchA", cTempName) )
		PivotMatrix = XMMatrixScaling(0.4f, 0.4f, 0.4f);
	else if (!strcmp("SM_PR_ClimaxArtB", cTempName))
		PivotMatrix = XMMatrixScaling(0.25f, 0.25f, 0.25f);
	else if (!strcmp("SM_PR_PotplantA", cTempName) ||
		!strcmp("SM_PR_PotplantB", cTempName) ||
		!strcmp("SM_PR_PotplantC", cTempName) || 
		!strcmp("SM_PR_PotplantD", cTempName))
		PivotMatrix = XMMatrixScaling(0.75f, 0.75f, 0.75f);



	CModel::TYPE etype = CModel::TYPE_END;
	if (DATA_ANIM == eTYPE)
		etype = CModel::TYPE_ANIM;
	else if (DATA_NOEANIM == eTYPE)
		etype = CModel::TYPE_NONANIM;
	else if (DATA_BONE == eTYPE)
		etype = CModel::TYPE_BONE;


	// 원본 생성
	if (bIsBin)
	{
		if (FAILED(pGameInstance->Add_Prototype(eLEVEL, pModelNameTemp,
			CModel::Bin_Create(m_pDevice, m_pContext, Scene, etype, tPath, tFileName, PivotMatrix))))
		{
			// 뭔가 파일 경로가 잘 못 됨.
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}
	}
	else
	{

		if (FAILED(pGameInstance->Add_Prototype(eLEVEL, pModelNameTemp,
			CModel::Create(m_pDevice, m_pContext, etype, tPath, tFileName, PivotMatrix))))
		{
			// 뭔가 파일 경로가 잘 못 됨.
			RELEASE_INSTANCE(CGameInstance);
			return E_FAIL;
		}

	}


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CDataManager::Create_Try_BinModel_All()
{
	_matrix mMatrix = XMMatrixIdentity();
	mMatrix = XMMatrixRotationAxis(_vector{ 0.f,1.f,0.f }, XMConvertToRadians(180.f));

	for (auto& str : m_AnimFilePaths)
	{
		TCHAR sTemp[MAX_PATH];
		TM->CtoTC(str.data(), sTemp);
		if (FAILED(Create_Try_BinModel(sTemp, LEVEL_STATIC, CDataManager::DATA_ANIM, mMatrix)))
			return E_FAIL;
	}


	for (auto& str : m_NonAnimFilePaths)
	{
		TCHAR sTemp[MAX_PATH];
		TM->CtoTC(str.data(), sTemp);
		if (FAILED(Create_Try_BinModel(sTemp, LEVEL_STATIC, CDataManager::DATA_NOEANIM, mMatrix)))
			return E_FAIL;
	}


	for (auto& str : m_BoneFilePaths)
	{
		TCHAR sTemp[MAX_PATH];
		TM->CtoTC(str.data(), sTemp);
		if (FAILED(Create_Try_BinModel(sTemp, LEVEL_STATIC, CDataManager::DATA_BONE, mMatrix)))
			return E_FAIL;
	}


	return S_OK;
}

HRESULT CDataManager::SaveSceneData(DATA_HEROSCENE * pScene, char* cModelName, DATA_TYPE eTYPE)
{
	char cPullName[MAX_PATH];
	if (eTYPE == DATA_ANIM)
		strcpy_s(cPullName, "../Data/Bin_Model/Anim/");
	else if (eTYPE == DATA_NOEANIM)
		strcpy_s(cPullName, "../Data/Bin_Model/NonAnim/");
	else if(eTYPE == DATA_BONE)
		strcpy_s(cPullName, "../Data/Bin_Model/Bone/");


	strcat_s(cPullName, cModelName);

	std::ofstream ofs(cPullName, ios::out | ios::binary);

	if (!ofs)
		return E_FAIL;

	// HierarchyNode
	ofs.write((char*)&pScene->iNodeCount, sizeof(int));
	for (int i = 0; i < pScene->iNodeCount; ++i)
	{
		DATA_HERONODE Node = pScene->pHeroNodes[i];
		ofs.write((char*)&Node, sizeof(DATA_HERONODE));
	}
	// Material
	ofs.write((char*)&pScene->iMaterialCount, sizeof(int));
	for (int i = 0; i < pScene->iMaterialCount; ++i)
	{
		DATA_HEROMATERIAL pMaterial = pScene->pHeroMaterial[i];
		ofs.write((char*)&pMaterial, sizeof(DATA_HEROMATERIAL));
	}

	// MashNode
	ofs.write((char*)&pScene->iMeshCount, sizeof(int));
	for (int i = 0; i < pScene->iMeshCount; ++i)
	{
		DATA_HEROMETH pMash = pScene->pHeroMesh[i];

		ofs.write((char*)&pMash.cName, sizeof(char)*MAX_PATH);
		ofs.write((char*)&pMash.iMaterialIndex, sizeof(int));


		ofs.write((char*)&pMash.NumVertices, sizeof(int));
		int iIsAnim = 0;
		if (nullptr != pMash.pAnimVertices)
		{
			iIsAnim = 1;
			ofs.write((char*)&iIsAnim, sizeof(int));

			for (int j = 0; j < pMash.NumVertices; ++j)
			{
				VTXANIMMODEL VtxAniModel = pMash.pAnimVertices[j];
				ofs.write((char*)&VtxAniModel, sizeof(VTXANIMMODEL));
			}
		}
		else if (nullptr != pMash.pNonAnimVertices)
		{
			iIsAnim = 0;
			ofs.write((char*)&iIsAnim, sizeof(int));

			for (int j = 0; j < pMash.NumVertices; ++j)
			{
				VTXMODEL VtxNoneAniModel = pMash.pNonAnimVertices[j];
				ofs.write((char*)&VtxNoneAniModel, sizeof(VTXMODEL));
			}
		}


		ofs.write((char*)&pMash.iNumPrimitives, sizeof(int));
		for (int j = 0; j < pMash.iNumPrimitives; ++j)
		{
			FACEINDICES32 Indices32 = pMash.pIndices[j];
			ofs.write((char*)&Indices32, sizeof(FACEINDICES32));
		}

		ofs.write((char*)&pMash.iNumBones, sizeof(int));
		for (int j = 0; j < pMash.iNumBones; ++j)
		{
			DATA_HEROBONE bon = pMash.pBones[j];
			ofs.write((char*)&bon, sizeof(DATA_HEROBONE));
		}
	}


	// Animation
	ofs.write((char*)&pScene->iNumAnimations, sizeof(int));
	for (int i = 0; i < pScene->iNumAnimations; ++i)
	{
		DATA_HEROANIM Anim = pScene->pHeroAnim[i];
		ofs.write((char*)&Anim.iNumChannels, sizeof(int));
		ofs.write((char*)&Anim.fDuration, sizeof(float));
		ofs.write((char*)&Anim.fTickPerSecond, sizeof(float));
		ofs.write((char*)&Anim.bLoop, sizeof(bool));
		ofs.write((char*)&Anim.szName, sizeof(char)*MAX_PATH);

		for (int j = 0; j < Anim.iNumChannels; ++j)
		{
			ofs.write((char*)&Anim.pHeroChannel[j].szName, sizeof(char)*MAX_PATH);
			ofs.write((char*)&Anim.pHeroChannel[j].iNumKeyFrames, sizeof(int));

			for (int k = 0; k < Anim.pHeroChannel[j].iNumKeyFrames; ++k)
			{
				ofs.write((char*)&Anim.pHeroChannel[j].pKeyFrames[k], sizeof(KEYFRAME));
			}

		}

		ofs.write((char*)&Anim.iNumTimeLimit, sizeof(int));
		for (int j = 0; j < Anim.iNumTimeLimit; ++j)
		{
			ofs.write((char*)&Anim.pTimeLimits[j], sizeof(float));
		}
		
	}



	ofs.close();


	return S_OK;
}
CModel* CDataManager::Create_Model_Without_Bin(const _tchar* pModelName, LEVEL eLEVEL, DATA_TYPE eTYPE)
{
	TCHAR* pModelNameTemp = CToolManager::Get_Instance()->Get_ManagedTChar();
	lstrcpy(pModelNameTemp, pModelName);

	char cTempName[MAX_PATH];
	CToolManager::Get_Instance()->TCtoC(pModelName, cTempName);

	char* tPath = CToolManager::Get_Instance()->Get_ManagedChar();
	if (DATA_ANIM == eTYPE)
		strcpy(tPath, "../Bin/Resources/Meshes/Anim/");
	else if (DATA_NOEANIM == eTYPE)
		strcpy(tPath, "../Bin/Resources/Meshes/NonAnim/");
	else if (DATA_BONE == eTYPE)
		strcpy(tPath, "../Bin/Resources/Meshes/Bone/");


	strcat(tPath, cTempName);
	strcat(tPath, "/");

	char* tFileName = CToolManager::Get_Instance()->Get_ManagedChar();
	strcpy(tFileName, cTempName);
	char TempName[MAX_PATH];
	strcpy(TempName, tFileName);
	strcat(tFileName, ".fbx");


	// Bin 체크 And Load
	DATA_HEROSCENE* Scene = new DATA_HEROSCENE;
	ZeroMemory(Scene, sizeof(DATA_HEROSCENE));
	_bool bIsBin = true;
	if (FAILED(CDataManager::Get_Instance()->ReadSceneData(TempName, Scene, eTYPE)))
		bIsBin = false;

	


	Safe_Delete_Array(Scene->pHeroNodes);
	Safe_Delete_Array(Scene->pHeroMaterial);

	for (_int i = 0; i < Scene->iMeshCount; ++i)
	{
		Safe_Delete_Array(Scene->pHeroMesh[i].pAnimVertices);
		Safe_Delete_Array(Scene->pHeroMesh[i].pNonAnimVertices);
		Safe_Delete_Array(Scene->pHeroMesh[i].pIndices);
		Safe_Delete_Array(Scene->pHeroMesh[i].pBones);
	}
	Safe_Delete_Array(Scene->pHeroMesh);


	for (_int i = 0; i < Scene->iNumAnimations; ++i)
	{
		for (_int j = 0; j < Scene->pHeroAnim[i].iNumChannels; ++j)
		{
			Safe_Delete_Array(Scene->pHeroAnim[i].pHeroChannel[j].pKeyFrames);
		}
		Safe_Delete_Array(Scene->pHeroAnim[i].pHeroChannel);

		Safe_Delete_Array(Scene->pHeroAnim[i].pTimeLimits);
	}
	Safe_Delete_Array(Scene->pHeroAnim);

	Safe_Delete(Scene);

	_matrix PivotMatrix;
	PivotMatrix = XMMatrixScaling(1.f, 1.f, 1.f) * XMMatrixRotationY(XMConvertToRadians(180.0f));

	CModel::TYPE etype = CModel::TYPE_END;
	if (DATA_ANIM == eTYPE)
		etype = CModel::TYPE_ANIM;
	else if (DATA_BONE == eTYPE)
		etype = CModel::TYPE_BONE;
	else if(DATA_NOEANIM == eTYPE)
		etype = CModel::TYPE_NONANIM;

	if (bIsBin)
		return nullptr;

	return CModel::Create(m_pDevice, m_pContext, etype, tPath, tFileName, PivotMatrix);
}

HRESULT CDataManager::Conv_Bin_Model(CModel * pModel, char* cModelName, DATA_TYPE eTYPE)
{
	DATA_HEROSCENE Scene;
	ZeroMemory(&Scene, sizeof(DATA_HEROSCENE));

	pModel->Get_HierarchyNodeData(&Scene);
	pModel->Get_MaterialData(&Scene);
	pModel->Get_MeshData(&Scene);
	pModel->Get_AnimData(&Scene);

	SaveSceneData(&Scene, cModelName, eTYPE);

	Safe_Delete_Array(Scene.pHeroNodes);
	Safe_Delete_Array(Scene.pHeroMaterial);

	for (_int i = 0; i < Scene.iMeshCount; ++i)
	{
		Safe_Delete_Array(Scene.pHeroMesh[i].pBones);
	}
	Safe_Delete_Array(Scene.pHeroMesh);


	for (_int i = 0; i < Scene.iNumAnimations; ++i)
	{
		for (_int j = 0; j < Scene.pHeroAnim[i].iNumChannels; ++j)
		{
			Safe_Delete_Array(Scene.pHeroAnim[i].pHeroChannel[j].pKeyFrames);
		}
		Safe_Delete_Array(Scene.pHeroAnim[i].pHeroChannel);

		Safe_Delete_Array(Scene.pHeroAnim[i].pTimeLimits);

	}

	Safe_Delete_Array(Scene.pHeroAnim);

	return S_OK;
}

HRESULT CDataManager::Conv_Bin_Model_All()
{

	for (auto& str : m_AnimFilePaths)
	{
		// 로드한다.
		TCHAR sTemp[MAX_PATH];
		CToolManager::Get_Instance()->CtoTC(str.data(), sTemp);
		CModel* pModel = Create_Model_Without_Bin(sTemp, LEVEL_TOOL, CDataManager::DATA_ANIM);
		// 이미 있다면 넘긴다.
		if (nullptr == pModel)
			continue;

		// 변환한다.
		char cTemp[MAX_PATH];
		strcpy(cTemp, str.data());
		CModel* pCloneModel = (CModel*)pModel->Clone();
		Conv_Bin_Model(pCloneModel, cTemp, CDataManager::DATA_ANIM);

		Safe_Release(pModel);
		Safe_Release(pCloneModel);
	}


	for (auto& str : m_NonAnimFilePaths)
	{
		// 로드한다.
		TCHAR sTemp[MAX_PATH];
		CToolManager::Get_Instance()->CtoTC(str.data(), sTemp);
		CModel* pModel = Create_Model_Without_Bin(sTemp, LEVEL_TOOL, CDataManager::DATA_NOEANIM);
		// 이미 있다면 넘긴다.
		if (nullptr == pModel)
			continue;

		// 변환한다.
		char cTemp[MAX_PATH];
		strcpy(cTemp, str.data());
		CModel* pCloneModel = (CModel*)pModel->Clone();
		Conv_Bin_Model(pCloneModel, cTemp, CDataManager::DATA_NOEANIM);

		Safe_Release(pModel);
		Safe_Release(pCloneModel);
	}


	for (auto& str : m_BoneFilePaths)
	{
		// 로드한다.
		TCHAR sTemp[MAX_PATH];
		CToolManager::Get_Instance()->CtoTC(str.data(), sTemp);
		CModel* pModel = Create_Model_Without_Bin(sTemp, LEVEL_TOOL, CDataManager::DATA_BONE);
		// 이미 있다면 넘긴다.
		if (nullptr == pModel)
			continue;

		// 변환한다.
		char cTemp[MAX_PATH];
		strcpy(cTemp, str.data());
		CModel* pCloneModel = (CModel*)pModel->Clone();
		Conv_Bin_Model(pCloneModel, cTemp, CDataManager::DATA_BONE);

		Safe_Release(pModel);
		Safe_Release(pCloneModel);
	}

	MSG_BOX(TEXT("Success Binary!"));

	return S_OK;
}






void CDataManager::Load_PhysObjData(_int iLevelIndex)
{
	Reset_MapRigidStatic();

	char cPullName[MAX_PATH];
	char cName[MAX_PATH];
	string ID = to_string(iLevelIndex);

	strcpy_s(cName, "Phys_");
	strcat_s(cName, ID.data());
	strcpy_s(cPullName, "../Data/PhysObjData/");
	strcat_s(cPullName, cName);

	std::ifstream ifs(cPullName, ios::in | ios::binary);

	if (!ifs)
		return;



	_int iID = 0;
	_int iSize = 0;
	ifs.read((char*)&iID, sizeof(int));
	ifs.read((char*)&iSize, sizeof(int));

	for (_int i = 0; i < iSize; ++i)
	{
		char sName[MAX_PATH];
		_float3 vPos, vRot;


		CPxObj::PXOBJDESC PxDesc;


		ifs.read((char*)sName, sizeof(char)*MAX_PATH);
		TM->CtoTC(sName, PxDesc.sModelName);
		ifs.read((char*)&PxDesc.RigidDesc.vStartPos, sizeof(_float3));
		ifs.read((char*)&PxDesc.RigidDesc.vRotation, sizeof(_float3));

		ifs.read((char*)&PxDesc.RigidDesc.vPos, sizeof(_float3));
		ifs.read((char*)&PxDesc.RigidDesc.vExtense, sizeof(_float3));

		ifs.read((char*)&PxDesc.RigidDesc.fWeight, sizeof(_float));
		ifs.read((char*)&PxDesc.RigidDesc.fAngleDump, sizeof(_float));
		ifs.read((char*)&PxDesc.RigidDesc.fStaticFriction, sizeof(_float));
		ifs.read((char*)&PxDesc.RigidDesc.fDynamicFriction, sizeof(_float));
		ifs.read((char*)&PxDesc.RigidDesc.fRestitution, sizeof(_float));
		ifs.read((char*)&PxDesc.RigidDesc.fMaxVel, sizeof(_float));


		Create_PxObj_Load(PxDesc);
	}

	ifs.close();
	return;
}

void CDataManager::Reset_MapRigidStatic()
{
	for (auto& pStatics : m_pMapRigidStatics)
		GI->Remove_Actor(pStatics);
	m_pMapRigidStatics.clear();
}

void CDataManager::Load_ByInstancing(_int iLevelIndex)
{
	// m_pNPC_T.clear();

	char cPullName[MAX_PATH];
	char cName[MAX_PATH];
	string ID = to_string(iLevelIndex);

	strcpy_s(cName, "NPC_");
	strcat_s(cName, ID.data());
	strcpy_s(cPullName, "../Data/PhysObjData/");
	strcat_s(cPullName, cName);

	std::ifstream ifs(cPullName, ios::in | ios::binary);

	if (!ifs)
		return;



	map<wstring, list<CNPC_T::NPCIDLEDESC>> TempDescs;

	_int iID = 0;
	_int iSize = 0;
	ifs.read((char*)&iID, sizeof(int));
	ifs.read((char*)&iSize, sizeof(int));

	for (_int i = 0; i < iSize; ++i)
	{
		CNPC_T::NPCIDLEDESC Desc;

		ifs.read((char*)& Desc, sizeof(CNPC_T::NPCIDLEDESC));


		// 이름에 맞게 분리
		auto& iter = TempDescs.find(Desc.sModelName);
		if (iter == TempDescs.end())
		{
			list<CNPC_T::NPCIDLEDESC> tempList;
			tempList.push_back(Desc);
			TempDescs.insert({ Desc.sModelName, tempList });
		}
		else
			(*iter).second.push_back(Desc);

	}

	// 이름 안에서도 애니메이션 인덱스로 정렬
	for (auto& Fair : TempDescs)
	{
		Fair.second.sort([](CNPC_T::NPCIDLEDESC pSour, CNPC_T::NPCIDLEDESC pDest)
		{
			return pSour.iAnimIndex > pDest.iAnimIndex;
		});


		vector<list<VTXMODELINSTANCE>> LocalDatas;

		list<CNPC_T::NPCIDLEDESC>::iterator iter = Fair.second.begin();
		vector<_int> iAnimIndexs;

		_uint iPreIndex = 1000;
		_int iCount = -1;
		for (; iter != Fair.second.end(); ++iter)
		{
			VTXMODELINSTANCE Data;

			if (iPreIndex != (*iter).iAnimIndex)
			{
				++iCount;
				list<VTXMODELINSTANCE> tTemp;
				LocalDatas.push_back(tTemp);
				iPreIndex = (*iter).iAnimIndex;
				iAnimIndexs.push_back((*iter).iAnimIndex);
			}

			_matrix mRot = XMMatrixRotationX(XMConvertToRadians((*iter).vRot.x));
			mRot *= XMMatrixRotationY(XMConvertToRadians((*iter).vRot.y + 180.f));
			mRot *= XMMatrixRotationZ(XMConvertToRadians((*iter).vRot.z));

			_float4x4 mRrot; XMStoreFloat4x4(&mRrot, mRot);

			Data.vRight = (_float4)&mRrot._11;
			Data.vUp = (_float4)&mRrot._21;
			Data.vLook = (_float4)&mRrot._31;

			Data.vPosition.x = (*iter).vPos.x;
			Data.vPosition.y = (*iter).vPos.y;
			Data.vPosition.z = (*iter).vPos.z;
			Data.vPosition.w = 1.f;
			Data.fDisolveRatio = GI->Get_FloatRandom(0.5f, 1.f);

			LocalDatas[iCount].push_back(Data);

		}


		_int iAnimIndexCount = 0;
		for (auto& LocalData : LocalDatas)
		{
			_uint iIncCount = (_uint)LocalData.size();
			VTXMODELINSTANCE* pData = new VTXMODELINSTANCE[iIncCount];
			_int iAnimIndex = iAnimIndexs[iAnimIndexCount];

			_int iCount = 0;
			_int AnimIndex = 0;
			for (auto& Data : LocalData)
			{
				pData[iCount] = Data;
				++iCount;
			}

			DATA_HEROSCENE* Scene = new DATA_HEROSCENE;
			ZeroMemory(Scene, sizeof(DATA_HEROSCENE));

			char sTemp[MAX_PATH];
			TM->TCtoC(Fair.first.data(), sTemp);
			CDataManager::Get_Instance()->ReadSceneData(sTemp, Scene, CDataManager::DATA_ANIM);

			wstring TempInName = Fair.first.data();
			TempInName.append(L"_Instance");
			TempInName.append(to_wstring(iAnimIndex));
			TempInName.append(to_wstring(iLevelIndex));
			
			string Path = "../Bin/Resources/Meshes/Anim/";
			Path.append(sTemp);
			Path.append("/");

			string Name = sTemp;
			Name.append(".fbx");

			TCHAR* pTcharTemp = TM->Get_ManagedTChar();
			lstrcpy(pTcharTemp, TempInName.data());
			if (FAILED(GI->Add_Prototype(LEVEL_STATIC, pTcharTemp,
				CModel_Instance::Bin_Create(m_pDevice, m_pContext, Scene, Path.data(), Name.data(), iIncCount, pData))))
				return;


			CModelObj_Instance::MODELINSTANCEDESC MIDesc;
			lstrcpy(MIDesc.sModelName, TempInName.data());
			MIDesc.iAnimIndex = iAnimIndex;

			LEVEL eLevel = TM->Get_CurLevel();
			if (FAILED(GI->Add_GameObjectToLayer(TEXT("ModelObj_Instance"), eLevel, TEXT("ModelInstance"), &MIDesc)))
				return;

			Safe_Delete_Array(pData);

			++iAnimIndexCount;
		}


	}


	ifs.close();
}

void CDataManager::Create_PxObj_Load(CPxObj::PXOBJDESC PxDesc)
{
	if (lstrcmp(PxDesc.sModelName, TEXT("MonsterBullet")) && lstrcmp(PxDesc.sModelName, TEXT("MonsterMissile")) && lstrcmp(PxDesc.sModelName, TEXT("BossMissile")))
	{
		CGameObject* pObj = nullptr;
		LEVEL eCulLevel = (LEVEL)TM->Get_CurLevel();
		GI->Add_GameObjectToLayer(TEXT("PxObj"), eCulLevel, TEXT("Layer_PxTool"), &PxDesc);
	}
	else
	{
		PxRigidStatic* pStatic = GI->Create_Static(PxDesc.RigidDesc.vStartPos, PxDesc.RigidDesc.vRotation, PxDesc.RigidDesc.vExtense, 0);
		m_pMapRigidStatics.push_back(pStatic);
	}


}




void CDataManager::Load_LightData(_int iLevelIndex)
{
	GI->Reset_Light();

	char cPullName[MAX_PATH];
	char cName[MAX_PATH];
	string ID = to_string(iLevelIndex);

	strcpy_s(cName, "Light_");
	strcat_s(cName, ID.data());
	strcpy_s(cPullName, "../Data/LightData/");
	strcat_s(cPullName, cName);

	std::ifstream ifs(cPullName, ios::in | ios::binary);

	if (!ifs)
		return;



	_int iID = 0;
	_int iSize = 0;
	ifs.read((char*)&iID, sizeof(int));
	ifs.read((char*)&iSize, sizeof(int));

	for (_int i = 0; i < iSize; ++i)
	{
		LIGHTDESC Desc;

		Desc.eType = LIGHTDESC::TYPE_POINT;
		ifs.read((char*)&Desc.vPosition, sizeof(_float4));
		ifs.read((char*)&Desc.fRange, sizeof(_float));

		ifs.read((char*)&Desc.vDiffuse, sizeof(_float4));
		ifs.read((char*)&Desc.vAmbient, sizeof(_float4));
		ifs.read((char*)&Desc.vSpecular, sizeof(_float4));

		GI->Add_Light(Desc);
	}

	ifs.close();

	return;
}



HRESULT CDataManager::Save_Cam(_int iCamID)
{
	char cPullName[MAX_PATH];
	char cName[MAX_PATH];
	string ID = to_string(iCamID);

	strcpy_s(cName, "Cam_");
	strcat_s(cName, ID.data());
	strcpy_s(cPullName, "../Data/Cam/");
	strcat_s(cPullName, cName);


	std::ofstream ofs(cPullName, ios::out | ios::binary);

	if (!ofs)
		return E_FAIL;


	const list<CMarkCube*>* MarkCubes = CRM->Get_CutSceneTool()->Get_MarkCubes();
	_uint iMarkCubeNum = (_uint)MarkCubes->size();

	ofs.write((char*)&iCamID, sizeof(int));
	ofs.write((char*)&iMarkCubeNum, sizeof(int));

	for (auto& pMarkCube : *MarkCubes)
	{
		CMarkCube::CAMDATA Data = pMarkCube->Get_SaveDATA();
		ofs.write((char*)&Data, sizeof(CMarkCube::CAMDATA));
	}


	const list<CLookCube_HR*>* LookCubes = CRM->Get_CutSceneTool()->Get_LookCubes();
	_uint iLookCubes = (_uint)LookCubes->size();

	ofs.write((char*)&iLookCubes, sizeof(int));

	for (auto& pLookCube : *LookCubes)
	{
		CLookCube_HR::CAMDATA Data = pLookCube->Get_SaveDATA();
		ofs.write((char*)&Data, sizeof(CLookCube_HR::CAMDATA));
	}

	ofs.close();

	return S_OK;
}

CDataManager::DATA_CAMS* CDataManager::Load_Cam(_int iCamID)
{
	char cPullName[MAX_PATH];
	char cName[MAX_PATH];
	string ID = to_string(iCamID);

	strcpy_s(cName, "Cam_");
	strcat_s(cName, ID.data());
	strcpy_s(cPullName, "../Data/Cam/");
	strcat_s(cPullName, cName);

	std::ifstream ifs(cPullName, ios::in | ios::binary);

	if (!ifs)
		return nullptr;



	DATA_CAMS* pDatas = new DATA_CAMS;
	ZeroMemory(pDatas, sizeof(DATA_CAMS));

	ifs.read((char*)&pDatas->iID, sizeof(int));
	ifs.read((char*)&pDatas->iPosNum, sizeof(int));

	pDatas->pPosDatas = new CAMDATA[pDatas->iPosNum];

	for (_uint i = 0; i < pDatas->iPosNum; ++i)
	{
		CAMDATA Data;
		ifs.read((char*)&Data, sizeof(CAMDATA));
		pDatas->pPosDatas[i] = Data;
	}



	ifs.read((char*)&pDatas->iLookNum, sizeof(int));

	pDatas->pLookDatas = new CAMDATA[pDatas->iLookNum];

	for (_uint i = 0; i < pDatas->iLookNum; ++i)
	{
		CAMDATA Data;
		ifs.read((char*)&Data, sizeof(CAMDATA));
		pDatas->pLookDatas[i] = Data;
	}

	ifs.close();

	return pDatas;
}





void CDataManager::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
