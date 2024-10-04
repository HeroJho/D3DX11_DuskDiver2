#include "stdafx.h"
#include "..\Public\AnimMap_Tool.h"
#include "GameInstance.h"

#include "ToolManager.h"
#include "DataManager.h"
#include "ImGui_Manager.h"
#include "ModelObj_Instance.h"


CAnimMap_Tool::CAnimMap_Tool(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CAnimMap_Tool::CAnimMap_Tool(const CAnimMap_Tool & rhs)
	: CGameObject(rhs)
{
}

HRESULT CAnimMap_Tool::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAnimMap_Tool::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_AnimMap_Tool";

	Make_ImGui();

	TM->Set_AnimMpa_Tool(this);


	return S_OK;
}

void CAnimMap_Tool::Tick(_float fTimeDelta)
{
	if (TK_PRESS(DIK_LSHIFT))
		m_fSpeed = 10.f;

	if (TK_PRESS(DIK_LEFT))
		m_pTransformCom->Go_Dir(XMVectorSet(-1.f, 0.f, 0.f, 0.f), m_fSpeed, fTimeDelta);
	else if (TK_PRESS(DIK_RIGHT))
		m_pTransformCom->Go_Dir(XMVectorSet(1.f, 0.f, 0.f, 0.f), m_fSpeed, fTimeDelta);
	else if (TK_PRESS(DIK_UP))
	{
		if (TK_PRESS(DIK_RSHIFT))
			m_pTransformCom->Go_Dir(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fSpeed, fTimeDelta);
		else
			m_pTransformCom->Go_Dir(XMVectorSet(0.f, 0.f, 1.f, 0.f), m_fSpeed, fTimeDelta);
	}
	else if (TK_PRESS(DIK_DOWN))
	{
		if (TK_PRESS(DIK_RSHIFT))
			m_pTransformCom->Go_Dir(XMVectorSet(0.f, -1.f, 0.f, 0.f), m_fSpeed, fTimeDelta);
		else
			m_pTransformCom->Go_Dir(XMVectorSet(0.f, 0.f, -1.f, 0.f), m_fSpeed, fTimeDelta);
	}


}

void CAnimMap_Tool::LateTick(_float fTimeDelta)
{

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	Render_Col(m_pRendererCom);

}

HRESULT CAnimMap_Tool::Render()
{


	return S_OK;
}






void CAnimMap_Tool::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	const list<string>* pNames = DM->Get_AnimFileNames();
	ImVec2 vSize{ 150.f, 200.f };
	if (ImGui::BeginListBox("Layers", vSize))
	{
		for (auto& Name : *pNames)
		{
			ImVec2 vSize{ 100.f, 10.f };
			bool isSelected = !strcmp(m_sSelectedModel, Name.data());
			if (ImGui::Selectable(Name.data(), isSelected, 0, vSize))
				strcpy(m_sSelectedModel, Name.data());

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndListBox();
	}



	if (ImGui::Button("Create"))
		Create_PxObj();


	ImGui::InputInt("Save_ID", &m_iSaveID);
	if (ImGui::Button("Save"))
		Save_Data(m_iSaveID);
	ImGui::SameLine();
	if (ImGui::Button("Load"))
		Load_Data(m_iSaveID);

	if (ImGui::Button("Test"))
		Load_ByInstancing(m_iSaveID);

	ImGui::End();
}





void CAnimMap_Tool::Save_Data(_int iLevelIndex)
{
	char cPullName[MAX_PATH];
	char cName[MAX_PATH];
	string ID = to_string(iLevelIndex);

	strcpy_s(cName, "NPC_");
	strcat_s(cName, ID.data());
	strcpy_s(cPullName, "../Data/PhysObjData/");
	strcat_s(cPullName, cName);


	std::ofstream ofs(cPullName, ios::out | ios::binary);

	if (!ofs)
		return;



	_int iNumObj = (_int)m_pNPC_T.size();



	ofs.write((char*)&iLevelIndex, sizeof(int));		// ID
	ofs.write((char*)&iNumObj, sizeof(int));			// NumObj


	for (auto& pNPC : m_pNPC_T)
	{
		CNPC_T::NPCIDLEDESC Desc = pNPC->Get_NPCDesc();
		ofs.write((char*)&Desc, sizeof(CNPC_T::NPCIDLEDESC));
	}


	ofs.close();

	return;
}

void CAnimMap_Tool::Load_Data(_int iLevelIndex)
{
	for (auto& pNPC : m_pNPC_T)
		pNPC->Set_Dead();
	m_pNPC_T.clear();

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



	_int iID = 0;
	_int iSize = 0;
	ifs.read((char*)&iID, sizeof(int));
	ifs.read((char*)&iSize, sizeof(int));

	for (_int i = 0; i < iSize; ++i)
	{
		CNPC_T::NPCIDLEDESC Desc;

		ifs.read((char*)& Desc, sizeof(CNPC_T::NPCIDLEDESC));


		Create_PxObj_Load(Desc);
	}


}

void CAnimMap_Tool::Load_ByInstancing(_int iLevelIndex)
{

	m_pNPC_T.clear();

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
			TempDescs.insert({Desc.sModelName, tempList});
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
			Data.vUp	= (_float4)&mRrot._21;
			Data.vLook	= (_float4)&mRrot._31;

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


}





void CAnimMap_Tool::Create_PxObj()
{
	CGameObject* pObj = nullptr;
	LEVEL eCulLevel = (LEVEL)GI->Get_Cur_LevelIndex();

	CNPC_T::NPCIDLEDESC NpcDesc;
	XMStoreFloat3(&NpcDesc.vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	NpcDesc.vRot = _float3(0.f, 0.f, 0.f);
	NpcDesc.vScale = _float3(1.f, 1.f, 1.f);
	NpcDesc.iAnimIndex = 1;
	TCHAR TTemp[MAX_PATH];
	TM->CtoTC(m_sSelectedModel, TTemp);
	lstrcpy(NpcDesc.sModelName, TTemp);


	if (FAILED(GI->Add_Get_GameObjectToLayer(TEXT("NPC_T"), eCulLevel, TEXT("Layer_NPC"), &NpcDesc, &pObj)))
		return;


	CNPC_T* pPxObj = (CNPC_T*)pObj;
	m_pNPC_T.push_back(pPxObj);
}

void CAnimMap_Tool::Create_PxObj_Load(CNPC_T::NPCIDLEDESC PxDesc)
{
	CGameObject* pObj = nullptr;
	LEVEL eCulLevel = (LEVEL)GI->Get_Cur_LevelIndex();


	if (FAILED(GI->Add_Get_GameObjectToLayer(TEXT("NPC_T"), eCulLevel, TEXT("Layer_NPC"), &PxDesc, &pObj)))
		return;


	CNPC_T* pPxObj = (CNPC_T*)pObj;
	m_pNPC_T.push_back(pPxObj);
}





void CAnimMap_Tool::Delete_Obj(string sImGuiID)
{
	for (list<class CNPC_T*>::iterator iter = m_pNPC_T.begin(); iter != m_pNPC_T.end();)
	{
		if ((*iter)->Get_ImGuiTag() == sImGuiID)
		{
			(*iter)->Set_Dead();
			m_pNPC_T.erase(iter);
			return;
		}
		else
			++iter;

	}
}





HRESULT CAnimMap_Tool::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;


	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "Tool");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}





CAnimMap_Tool * CAnimMap_Tool::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CAnimMap_Tool*		pInstance = new CAnimMap_Tool(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimMap_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CAnimMap_Tool::Clone(void * pArg)
{
	CAnimMap_Tool*		pInstance = new CAnimMap_Tool(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CAnimMap_Tool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CAnimMap_Tool::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);


}
