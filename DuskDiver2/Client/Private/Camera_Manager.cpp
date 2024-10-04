#include "stdafx.h"
#include "..\Public\Camera_Manager.h"
#include "GameInstance.h"

#include "ColorCube.h"
#include "MarkCube.h"
#include "LookCube_HR.h"
#include "ToolManager.h"
#include "UI_Manager.h"
#include "Monster_Manager.h"
#include "Crack_Manager.h"


IMPLEMENT_SINGLETON(CCamera_Manager)

CCamera_Manager::CCamera_Manager()
{
}


HRESULT CCamera_Manager::Add_CameraMain(CCamera_Main * pCameraMain)
{
	if (pCameraMain == nullptr)
		return E_FAIL;

	m_pCameraMain = pCameraMain;
	return S_OK;
}

void CCamera_Manager::Set_CamMode()
{
	m_pCameraMain->Set_CamMode();
}

CCamera_Main::CAMMODE CCamera_Manager::Get_CamMode()
{
	return m_pCameraMain->Get_CamMode();
}















HRESULT CCamera_Manager::Init()
{
	Load_Data();

	return S_OK;
}

void CCamera_Manager::Tick(_float fTimeDelta)
{
	_float fT = 1.f / 60.f;
	if(fT < fTimeDelta)
		fTimeDelta = fT;

	PlayCutScene(fTimeDelta);
}



void CCamera_Manager::Create_MarkCube(CDataManager::CAMDATA* pData)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CMarkCube::COLORCUBEDESC ColorCubeDesc;
	ZeroMemory(&ColorCubeDesc, sizeof(CMarkCube::COLORCUBEDESC));
	ColorCubeDesc.vColor = _float4(0.f, 1.f, 0.f, 1.f);
	// ColorCubeDesc.vPos = _float3(1.f, 1.f, 1.f);
	ColorCubeDesc.vPos = _float3(0.f, 0.f, 0.f);
	ColorCubeDesc.vScale = _float3(1.f, 1.f, 1.f);

	CGameObject* pTempObj = nullptr;

	
	if (FAILED(pGameInstance->Add_Get_GameObjectToLayer(TEXT("Prototype_GameObject_MarkCube"), TM->Get_CurLevel(), TEXT("Layer_CamTool"), &ColorCubeDesc, &pTempObj)))
		return;

	m_MarkCubes.push_back((CMarkCube*)pTempObj);


	Safe_AddRef(pTempObj);

	if (nullptr != pData)
	{
		((CMarkCube*)pTempObj)->Set_SaveDATA(pData);
	}

	Safe_Release(pGameInstance);

}

void CCamera_Manager::Create_LookCube(CDataManager::CAMDATA* pData)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	CLookCube_HR::COLORCUBEDESC ColorCubeDesc;
	ZeroMemory(&ColorCubeDesc, sizeof(CLookCube_HR::COLORCUBEDESC));
	ColorCubeDesc.vColor = _float4(0.f, 1.f, 1.f, 1.f);
	// ColorCubeDesc.vPos = _float3(1.f, 1.f, 1.f);
	ColorCubeDesc.vPos = _float3(0.f, 0.f, 0.f);
	ColorCubeDesc.vScale = _float3(1.f, 1.f, 1.f);

	CGameObject* pTempObj = nullptr;

	if (FAILED(pGameInstance->Add_Get_GameObjectToLayer(TEXT("Prototype_GameObject_LookCube"), TM->Get_CurLevel(), TEXT("Layer_CamTool"), &ColorCubeDesc, &pTempObj)))
		return;

	m_LookCubes.push_back((CLookCube_HR*)pTempObj);

	Safe_AddRef(pTempObj);

	if (nullptr != pData)
	{
		((CLookCube_HR*)pTempObj)->Set_SaveDATA(pData);
	}

	Safe_Release(pGameInstance);

}

void CCamera_Manager::Create_ChaseLookCube()
{
	//if (nullptr != m_pChaseLookCube)
	//{
	//	m_pChaseLookCube->Set_Dead();
	//	m_pChaseLookCube = nullptr;
	//}



	CColorCube::COLORCUBEDESC ColorCubeDesc;
	ZeroMemory(&ColorCubeDesc, sizeof(CColorCube::COLORCUBEDESC));
	ColorCubeDesc.vColor = _float4(0.5f, 0.5f, 0.5f, 1.f);
	ColorCubeDesc.vPos = _float3(0.f, 0.f, 0.f);
	ColorCubeDesc.vScale = _float3(1.f, 1.f, 1.f);

	CGameObject* pTempObj = nullptr;

	if (FAILED(GI->Add_Get_GameObjectToLayer(TEXT("Prototype_GameObject_ColorCube"), TM->Get_CurLevel(), TEXT("Layer_CamTool"), &ColorCubeDesc, &pTempObj)))
		return;

	m_pChaseLookCube = (CColorCube*)pTempObj;


}





void CCamera_Manager::Play_CutScene(_uint iIndex, _bool bAutoEnd, _bool bLerpBack, CTransform* pTran)
{
	m_bAutoEnd = bAutoEnd;
	m_iCurCutIndex = iIndex;
	m_bLerpBack = bLerpBack;

	CDataManager::DATA_CAMS* Data = m_Datas[iIndex];

	Clear_Data();

	// 절대 좌표
	if (nullptr == pTran)
	{
		for (_uint i = 0; i < Data->iPosNum; ++i)
		{
			Create_MarkCube(&Data->pPosDatas[i]);
		}

		for (_uint i = 0; i < Data->iLookNum; ++i)
		{
			Create_LookCube(&Data->pLookDatas[i]);
		}
	}
	// 상대 좌표
	else
	{
		_matrix mOtherWorld = pTran->Get_WorldMatrix();
		//mOtherWorld.r[0] = XMVectorSetW(XMVector3Normalize(mOtherWorld.r[0]), 0.f);
		//mOtherWorld.r[1] = XMVectorSetW(XMVector3Normalize(mOtherWorld.r[1]), 0.f);
		//mOtherWorld.r[2] = XMVectorSetW(XMVector3Normalize(mOtherWorld.r[2]), 0.f);

		for (_uint i = 0; i < Data->iPosNum; ++i)
		{
			CDataManager::CAMDATA CamData = Data->pPosDatas[i];
			_vector vPos = XMLoadFloat3(&CamData.vPos);
			vPos = XMVector3TransformCoord(vPos, mOtherWorld);
			
			XMStoreFloat3(&CamData.vPos, vPos);
			Create_MarkCube(&CamData);
		}

		for (_uint i = 0; i < Data->iLookNum; ++i)
		{
			CDataManager::CAMDATA CamData = Data->pLookDatas[i];
			_vector vPos = XMLoadFloat3(&CamData.vPos);
			vPos = XMVector3TransformCoord(vPos, mOtherWorld);
			XMStoreFloat3(&CamData.vPos, vPos);
			Create_LookCube(&CamData);
		}

	}



	Set_Start(true);

}

void CCamera_Manager::End_CutScene()
{
	m_bStart = false;
	//m_pCameraMain->Set_State(CCamera_Free::CAM_GAME);
	m_pCameraMain->Set_CutScene(false);
}



void CCamera_Manager::Set_Start(_bool bStart)
{
	m_bStart = bStart;
	m_fMarkSpeed = 0.f;
	m_fMarkTimeAcc = 0.f;
	m_fMarkT = 0.f;
	m_TempMarkCubes.clear();
	m_pTempBasiMarkCubes.clear();

	m_fLookSpeed = 0.f;
	m_fLookTimeAcc = 0.f;
	m_fLookT = 0.f;
	m_TempLookCubes.clear();
	m_pTempBasiLookCubes.clear();

	if (bStart)
	{
		m_TempMarkCubes = m_MarkCubes;
		MakeMarkPos();
		m_vMarkTempPos = CalculMarkBasi();
		m_pCameraMain->Set_Pos(m_vMarkTempPos);


		Create_ChaseLookCube();
		m_TempLookCubes = m_LookCubes;
		MakeLookPos();
		m_vLookTempPos = CalculLookBasi();
		m_pChaseLookCube->Set_Pos(m_vLookTempPos);

		// m_pCameraMain->Set_State(CCamera_Free::CAM_CUTSCENE);
		m_pCameraMain->Set_CutScene(true);
	}

}
void CCamera_Manager::PlayCutScene(_float fTimeDelta)
{
	if (!m_bStart)
		return;

	PlayMark(fTimeDelta);
	PlayLook(fTimeDelta);

}

void CCamera_Manager::PlayMark(_float fTimeDelta)
{

	// 도착 했냐
	if (1.f < m_fMarkT)
	{
		if (1 > m_TempMarkCubes.size() && m_bAutoEnd)
		{
			if (m_bLerpBack)
				m_pCameraMain->ResetPrePos();

			m_bStart = false;
			m_bLerpBack = false;


			m_pCameraMain->Set_CutScene(false);



			End_Cut();


			return;
		}
		else if (1 > m_TempMarkCubes.size() && !m_bAutoEnd)
			return;



		// 고른다. 
		m_pTempBasiMarkCubes.clear();
		MakeMarkPos();

		m_fMarkT = 0.f;
		m_fMarkTimeAcc = 0.f;

		m_vMarkTempPos = CalculMarkBasi();
		CRM->Get_Cam()->Set_Pos(m_vMarkTempPos);

		//m_fLookT = 2.f;
		//m_fLookTimeAcc = 0.f;


		return;
	}




	_vector vTempPos = XMLoadFloat3(&m_vMarkTempPos);
	vTempPos = XMVectorSetW(vTempPos, 1.f);

	if (1 == m_pTempBasiMarkCubes.size())
	{
		m_fMarkTimeAcc += fTimeDelta;
		if (m_fMarkSpeed < m_fMarkTimeAcc)
		{
			m_fMarkT = 2.f;
			m_fMarkTimeAcc = 0.f;
		}

	}
	else
	{
		if (m_pCameraMain->Move(vTempPos, m_fMarkSpeed, fTimeDelta, m_fMarkDestDis, true))
		{
			m_fMarkT += m_fMarkMoveSens;
			m_vMarkTempPos = CalculMarkBasi();
		}
	}

	_vector vPos = XMLoadFloat3(&m_pChaseLookCube->Get_Pos());
	vPos = XMVectorSetW(vPos, 1.f);
	m_pCameraMain->LookAt(vPos);


}
void CCamera_Manager::PlayLook(_float fTimeDelta)
{
	// 도착 했냐
	if (1.f < m_fLookT)
	{
		if (1 > m_TempLookCubes.size())
		{
			// m_bStart = false;
			return;
		}

		// 고른다. 
		m_pTempBasiLookCubes.clear();
		MakeLookPos();

		m_fLookT = 0.f;
		m_fLookTimeAcc = 0.f;

		m_vLookTempPos = CalculLookBasi();
		m_pChaseLookCube->Set_Pos(m_vLookTempPos);


		return;
	}




	_vector vTempPos = XMLoadFloat3(&m_vLookTempPos);
	vTempPos = XMVectorSetW(vTempPos, 1.f);

	if (1 == m_pTempBasiLookCubes.size())
	{
		m_fLookTimeAcc += fTimeDelta;
		if (m_fLookSpeed < m_fLookTimeAcc)
		{
			m_fLookT = 2.f;
			m_fLookTimeAcc = 0.f;
		}

	}
	else
	{
		if (m_pChaseLookCube->Move(vTempPos, m_fLookSpeed, fTimeDelta, m_fLookDestDis, true)) //5.f
		{
			m_fLookT += m_fLookMoveSens;
			m_vLookTempPos = CalculLookBasi();
		}
	}
}


void CCamera_Manager::MakeMarkPos()
{
	if (1 > m_TempMarkCubes.size())
		return;


	list<CMarkCube*>::iterator Cur_iter = m_TempMarkCubes.begin();
	list<CMarkCube*>::iterator Fro_iter = ++(m_TempMarkCubes.begin());

	for (; Cur_iter != m_TempMarkCubes.end(); ++Cur_iter)
	{

		if (1 == m_TempMarkCubes.size())
		{
			m_pTempBasiMarkCubes.push_back(*Cur_iter);
			break;
		}


		_int CurI = (*Cur_iter)->Get_LinkIndex();
		_int FroI = -1;
		if (Fro_iter != m_TempMarkCubes.end())
		{
			FroI = (*Fro_iter)->Get_LinkIndex();
			Fro_iter++;
		}



		if (CurI == FroI)
		{	// 연결 됐다.
			m_pTempBasiMarkCubes.push_back(*Cur_iter);
		}
		else
		{
			m_pTempBasiMarkCubes.push_back(*Cur_iter);
			break;
		}
	}


	for (_int i = 0; i < m_pTempBasiMarkCubes.size(); ++i)
		m_TempMarkCubes.pop_front();

	// 스피드를 설정한다.
	if (0 < m_pTempBasiMarkCubes.size())
	{
		CMarkCube* pTempCube = m_pTempBasiMarkCubes.front();
		m_fMarkSpeed = pTempCube->Get_SpeedTime();
		m_fMarkDestDis = pTempCube->Get_DestDis();
		m_fMarkMoveSens = pTempCube->Get_SensTime();
	}
}
void CCamera_Manager::MakeLookPos()
{
	if (1 > m_TempLookCubes.size())
		return;


	list<CLookCube_HR*>::iterator Cur_iter = m_TempLookCubes.begin();
	list<CLookCube_HR*>::iterator Fro_iter = ++(m_TempLookCubes.begin());

	for (; Cur_iter != m_TempLookCubes.end(); ++Cur_iter)
	{

		if (1 == m_TempLookCubes.size())
		{
			m_pTempBasiLookCubes.push_back(*Cur_iter);
			break;
		}


		_int CurI = (*Cur_iter)->Get_LinkIndex();
		_int FroI = -1;
		if (Fro_iter != m_TempLookCubes.end())
		{
			FroI = (*Fro_iter)->Get_LinkIndex();
			Fro_iter++;
		}



		if (CurI == FroI)
		{	// 연결 됐다.
			m_pTempBasiLookCubes.push_back(*Cur_iter);
		}
		else
		{
			m_pTempBasiLookCubes.push_back(*Cur_iter);
			break;
		}
	}


	for (_int i = 0; i < m_pTempBasiLookCubes.size(); ++i)
		m_TempLookCubes.pop_front();

	// 스피드를 설정한다.
	if (0 < m_pTempBasiLookCubes.size())
	{
		CLookCube_HR* pTempCube = m_pTempBasiLookCubes.front();
		m_fLookSpeed = pTempCube->Get_SpeedTime();
		m_fLookDestDis = pTempCube->Get_DestDis();
		m_fLookMoveSens = pTempCube->Get_SensTime();
	}
}
_float3 CCamera_Manager::CalculMarkBasi()
{
	_int iSize = (_int)m_pTempBasiMarkCubes.size();

	_float3 vPos1, vPos2, vPos3, vPos4;

	list<CMarkCube*>::iterator iter = m_pTempBasiMarkCubes.begin();

	switch (iSize)
	{
	case 1:
		vPos1 = (*iter)->Get_Pos();
		return vPos1;
		break;
	case 2:
		vPos1 = (*iter)->Get_Pos();
		vPos2 = (*(++iter))->Get_Pos();
		return CToolManager::Get_Instance()->GetBesierPos(vPos1, vPos2, m_fMarkT);
	case 3:
		vPos1 = (*iter)->Get_Pos();
		vPos2 = (*(++iter))->Get_Pos();
		vPos3 = (*(++iter))->Get_Pos();
		return CToolManager::Get_Instance()->GetBesierPos(vPos1, vPos2, vPos3, m_fMarkT);
	case 4:
		vPos1 = (*iter)->Get_Pos();
		vPos2 = (*(++iter))->Get_Pos();
		vPos3 = (*(++iter))->Get_Pos();
		vPos4 = (*(++iter))->Get_Pos();
		return CToolManager::Get_Instance()->GetBesierPos(vPos1, vPos2, vPos3, vPos4, m_fMarkT);
	}

	return _float3();
}
_float3 CCamera_Manager::CalculLookBasi()
{
	_int iSize = (_int)m_pTempBasiLookCubes.size();
	_float3 vPos1, vPos2, vPos3, vPos4;
	list<CLookCube_HR*>::iterator iter = m_pTempBasiLookCubes.begin();

	switch (iSize)
	{
	case 1:
		vPos1 = (*iter)->Get_Pos();
		return vPos1;
		break;
	case 2:
		vPos1 = (*iter)->Get_Pos();
		vPos2 = (*(++iter))->Get_Pos();
		return CToolManager::Get_Instance()->GetBesierPos(vPos1, vPos2, m_fLookT);
	case 3:
		vPos1 = (*iter)->Get_Pos();
		vPos2 = (*(++iter))->Get_Pos();
		vPos3 = (*(++iter))->Get_Pos();
		return CToolManager::Get_Instance()->GetBesierPos(vPos1, vPos2, vPos3, m_fLookT);
	case 4:
		vPos1 = (*iter)->Get_Pos();		// A
		vPos2 = (*(++iter))->Get_Pos();	// B
		vPos3 = (*(++iter))->Get_Pos();	// C
		vPos4 = (*(++iter))->Get_Pos(); // D
		return CToolManager::Get_Instance()->
			GetBesierPos(vPos1, vPos2, vPos3, vPos4, m_fLookT);
	}

	return _float3();
}










void CCamera_Manager::Free()
{

	for (auto& pCube : m_MarkCubes)
		Safe_Release(pCube);
	m_MarkCubes.clear();

	for (auto& pCube : m_LookCubes)
		Safe_Release(pCube);
	m_LookCubes.clear();

	for (auto& pData : m_Datas)
	{
		Safe_Delete_Array(pData->pPosDatas);
		Safe_Delete_Array(pData->pLookDatas);
		Safe_Delete(pData);
	}


	m_Datas.clear();
}



void CCamera_Manager::Load_Data()
{
	m_Datas.clear();
	CDataManager::DATA_CAMS* Data = nullptr;

	Data = CDataManager::Get_Instance()->Load_Cam(0);
	m_Datas.push_back(Data);
	Data = CDataManager::Get_Instance()->Load_Cam(1);
	m_Datas.push_back(Data);
	Data = CDataManager::Get_Instance()->Load_Cam(2);
	m_Datas.push_back(Data);
	Data = CDataManager::Get_Instance()->Load_Cam(3);
	m_Datas.push_back(Data);
	Data = CDataManager::Get_Instance()->Load_Cam(4);
	m_Datas.push_back(Data);
	Data = CDataManager::Get_Instance()->Load_Cam(5);
	m_Datas.push_back(Data);
	Data = CDataManager::Get_Instance()->Load_Cam(6); // 오락실
	m_Datas.push_back(Data);
	Data = CDataManager::Get_Instance()->Load_Cam(7);
	m_Datas.push_back(Data);
	Data = CDataManager::Get_Instance()->Load_Cam(8);
	m_Datas.push_back(Data);
	Data = CDataManager::Get_Instance()->Load_Cam(9);  // 오락실 첫 길라잡이 컷 신
	m_Datas.push_back(Data);
	Data = CDataManager::Get_Instance()->Load_Cam(10); // 유모 v스킬 컷 신
	m_Datas.push_back(Data);
	Data = CDataManager::Get_Instance()->Load_Cam(11); // 유모 B스킬 컷 신
	m_Datas.push_back(Data);
	Data = CDataManager::Get_Instance()->Load_Cam(12); // 리베다 스킬 컷
	m_Datas.push_back(Data);
	Data = CDataManager::Get_Instance()->Load_Cam(13); // DD 스킬 컷
	m_Datas.push_back(Data);
	Data = CDataManager::Get_Instance()->Load_Cam(14); // 슈퍼 변신 스킬 컷
	m_Datas.push_back(Data);

}

void CCamera_Manager::End_Cut()
{

	switch (m_iCurCutIndex)
	{
	case 0:
		Play_CutScene(1, true, false, nullptr);
		break;
	case 1:    // 지하철 끝
		UM->Set_TalkIndex(14);
		CRM->Get_Cam()->Set_CamPos(140.f, 15.f);
		break;
	case 2:
		Play_CutScene(3, true, true, nullptr);
		break;
	case 3:
		Play_CutScene(4, true, true, nullptr);
		break;
	case 4:
		Play_CutScene(5, true, false, nullptr);
		break;
	case 5: // 도시 끝
		CRM->Get_Cam()->Set_CamPos(90.f, 20.f);
		CRKM->Create_Crack(_float4(11.f, 0.f, -1.5f, 1.f), 4.f, 2, TM->Get_CurLevel());
		UM->Set_TalkIndex(28);
		break;
	case 6:
		Play_CutScene(7, true, true, nullptr);
		break;
	case 7:
		Play_CutScene(8, true, false, nullptr);
		break;
	case 8:  // 오락실 끝
		UM->Set_TalkIndex(63);
		break;
	default:
		break;
	}

}





void CCamera_Manager::Clear_PosData()
{
	for (auto& pCube : m_MarkCubes)
		pCube->Set_Dead();

	for (auto& pCube : m_MarkCubes)
		Safe_Release(pCube);

	m_MarkCubes.clear();
	m_TempMarkCubes.clear();
	m_pTempBasiMarkCubes.clear();


	m_bStart = false;
	m_fMarkSpeed = 0.f;
	m_fMarkTimeAcc = 0.f;
	m_fMarkT = 0.f;

	m_fLookSpeed = 0.f;
	m_fLookTimeAcc = 0.f;
	m_fLookT = 0.f;

}
void CCamera_Manager::Clear_LookData()
{
	for (auto& pCube : m_LookCubes)
		pCube->Set_Dead();

	for (auto& pCube : m_LookCubes)
		Safe_Release(pCube);

	m_LookCubes.clear();
	m_TempLookCubes.clear();
	m_pTempBasiLookCubes.clear();


	m_bStart = false;
	m_fMarkSpeed = 0.f;
	m_fMarkTimeAcc = 0.f;
	m_fMarkT = 0.f;

	m_fLookSpeed = 0.f;
	m_fLookTimeAcc = 0.f;
	m_fLookT = 0.f;
}
void CCamera_Manager::Clear_Data()
{
	Clear_PosData();
	Clear_LookData();
}
