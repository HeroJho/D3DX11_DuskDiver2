#include "stdafx.h"
#include "..\Public\ToolManager.h"

#include "GameInstance.h"
#include "Player_Manager.h"
#include "Level_Loading.h"
#include "Monster_Manager.h"
#include "MiniGame_Yabawe.h"
#include "MiniGame_Block.h"
#include "BlockCuma.h"
#include "FunchCuma.h"
#include "Stage4Machine0.h"
#include "Camera_Manager.h"
#include "Camera_Main.h"

#include "Layer.h"


#include "Stage4Trigger_1.h"
#include "Stage4Trigger_2.h"
#include "Stage4Trigger_3.h"
#include "Stage4Trigger_4.h"
#include "Stage4Trigger_5.h"
#include "Stage4Trigger_6.h"
#include "Stage4Trigger_7.h"

#include "Bear_HP_Bar.h"

#include "EPotal.h"
#include "QPotal.h"


IMPLEMENT_SINGLETON(CToolManager)

CToolManager::CToolManager()
{

}

HRESULT CToolManager::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	if (nullptr == pDevice || nullptr == pContext)
		return E_FAIL;

	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(pDevice);
	Safe_AddRef(pContext);


	return S_OK;
}







//	두 포지션을 보간하는 함수
_vector CToolManager::Lerp_Pos(_fvector vPos, _fvector vTargetPos, 
	_float fTimeDelta)
{
	_vector vDisVec = vTargetPos - vPos;
	_float fDis = XMVectorGetX(XMVector3Length(vDisVec)) 
		* fTimeDelta;
	return XMVectorSetW(vPos + XMVector3Normalize(vDisVec) * fDis, 1.f);
}

_vector CToolManager::Lerp_PosByT(_fvector vPos, _fvector vTargetPos, 
	_float fT)
{
	_vector vDisVec = vTargetPos - vPos;
	return XMVectorSetW(vPos + vDisVec * fT, 1.f);
}

_vector CToolManager::Lerp_Look(_fvector vLook, _fvector vTargetLook, _float fT)
{
	_vector vNoLook = XMVector3Normalize(vLook);
	_vector vNoTargetLook = XMVector3Normalize(vTargetLook);

	_vector vLerpLook = vNoLook * (1.f-fT) + vNoTargetLook * fT;

	return XMVector3Normalize(vLerpLook);
}

void CToolManager::TCtoC(const TCHAR * pTC, char * pC)
{
	TCHAR pTCTemp[MAX_PATH];
	lstrcpy(pTCTemp, pTC);
	WideCharToMultiByte(CP_ACP, 0, pTCTemp, MAX_PATH, pC, MAX_PATH, NULL, NULL);
}
void CToolManager::CtoTC(const char * pC, TCHAR * pTC)
{
	char pCTemp[MAX_PATH];
	strcpy_s(pCTemp, pC);
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pCTemp, MAX_PATH, pTC, MAX_PATH);
}

_tchar* CToolManager::Get_ManagedTChar()
{
	_tchar* pTemp = new _tchar[MAX_PATH];
	ZeroMemory(pTemp, sizeof(_tchar) * MAX_PATH);
	m_ManagedTChar.push_back(pTemp);

	return pTemp;
}

char* CToolManager::Get_ManagedChar()
{
	char* pTemp = new char[MAX_PATH];
	ZeroMemory(pTemp, sizeof(char) * MAX_PATH);
	m_ManagedChar.push_back(pTemp);

	return pTemp;
}

void CToolManager::ClearManagedChar()
{
	for (auto& pTChar : m_ManagedTChar)
		Safe_Delete(pTChar);
	for (auto& pChar : m_ManagedChar)
		Safe_Delete(pChar);
}





// For. Math
_float3 CToolManager::GetBesierPos(_float3 vPos1, _float3 vPos2, _float fT)
{
	_float3 temp;
	XMStoreFloat3(&temp, (1 - fT)* XMLoadFloat3(&vPos1) + fT*XMLoadFloat3(&vPos2));
	return temp;
}

_float3 CToolManager::GetBesierPos(_float3 vPos1, _float3 vPos2, _float3 vPos3, _float fT)
{
	_float3 temp;
	XMStoreFloat3(&temp, (1 - fT)*(1 - fT)*XMLoadFloat3(&vPos1) + 2 * (1 - fT)*fT*XMLoadFloat3(&vPos2) + fT*fT*XMLoadFloat3(&vPos3));
	return temp;
}

_float3 CToolManager::GetBesierPos(_float3 vPos1, _float3 vPos2, 
	_float3 vPos3, _float3 vPos4, _float fT)
{
	_float3 temp;
	XMStoreFloat3(&temp, (1 - fT)*(1 - fT)*(1 - fT)*
		XMLoadFloat3(&vPos1) + 3 * (1 - fT)*(1 - fT)*fT* 
		XMLoadFloat3(&vPos2) + 3 * (1 - fT) *fT*fT*
		XMLoadFloat3(&vPos3) + fT*fT*fT*XMLoadFloat3(&vPos4));
	return temp;
}

_bool CToolManager::Get_Loaded(LEVEL eLevel)
{
	switch (eLevel)
	{
	case Client::LEVEL_LOAD:
		break;
	case Client::LEVEL_LOADING:
		break;
	case Client::LEVEL_LOGO:
		break;
	case Client::LEVEL_TEST:
		return m_bLoadTest;
		break;
	case Client::LEVEL_TOOL:
		return m_bLoadTool;
		break;
	case Client::LEVEL_STAGE1:
		return m_bLoadStage1;
		break;
	case Client::LEVEL_STAGE2:
		return m_bLoadStage2;
		break;
	case Client::LEVEL_STAGE3:
		return m_bLoadStage3;
		break;
	case Client::LEVEL_STAGE4:
		return m_bLoadStage4;
		break;
	case Client::LEVEL_STAGE5:
		return m_bLoadStage5;
		break;
	case Client::LEVEL_STATIC:
		break;
	}

	return false;
}

void CToolManager::Set_Loaded(LEVEL eLevel)
{
	switch (eLevel)
	{
	case Client::LEVEL_LOAD:
		break;
	case Client::LEVEL_LOADING:
		break;
	case Client::LEVEL_LOGO:
		break;
	case Client::LEVEL_TEST:
		m_bLoadTest = true;
		break;
	case Client::LEVEL_TOOL:
		m_bLoadTool = true;
		break;
	case Client::LEVEL_STAGE1:
		m_bLoadStage1 = true;
		break;
	case Client::LEVEL_STAGE2:
		m_bLoadStage1 = true;
		break;
	case Client::LEVEL_STAGE3:
		m_bLoadStage1 = true;
		break;
	case Client::LEVEL_STAGE4:
		m_bLoadStage1 = true;
		break;
	case Client::LEVEL_STAGE5:
		m_bLoadStage1 = true;
		break;
	case Client::LEVEL_STATIC:
		break;
	}
}

void CToolManager::Change_Level(LEVEL eLevel)
{
	GI->StopAll();
	GI->Reserve_Level((_uint)eLevel);
	m_eLevel = eLevel;
	IG->Reset_DebugData();

	MM->Reset_Monster();
	//PM->Release_Player();
}

void CToolManager::Start_MiniGame_Block_CutScene()
{
	CBlockCuma::BLOCKCMADESC DDesc;
	DDesc.vPos = _float3(0.f, 0.f, 0.f);
	DDesc.vRot = _float3(0.f, 0.f, 0.f);

	CGameObject* pObj = nullptr;

	GI->Add_Get_GameObjectToLayer(TEXT("BlockCuma"), Get_CurLevel(), L"Mini_Block_Cuma", &DDesc, &pObj);
	m_pCuma = (CBlockCuma*)pObj;
}

void CToolManager::Start_MiniGame_Block()
{
	// 쌓인 블럭들 삭제
	CLayer* pLayer = (GI->Get_Layers(TM->Get_CurLevel())->find(TEXT("Mini_Block")))->second;
	for (auto& pObj : *pLayer->Get_GameObjects())
		pObj->Set_Dead();
	

	CGameObject* pObj = nullptr;
	GI->Add_Get_GameObjectToLayer(TEXT("MiniGame_Block"), Get_CurLevel(), L"Layer_MiniGame", nullptr, &pObj);
	m_pMiniGame_Block = (CMiniGame_Block*)pObj;
}


void CToolManager::Start_MiniGame_Yabawe()
{
	CMiniGame_Yabawe::MINIGAMEYABADESC Desc;

	_matrix mMaxtrix;
	XMStoreFloat4x4(&Desc.mMatrix, XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(30.f)) * XMMatrixTranslation(-98.37f, -3.895f, 131.92f));
	//XMStoreFloat4x4(&Desc.mMatrix, XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(30.f)) * XMMatrixTranslation(3.f, 0.f, 0.f));

	CGameObject* pObj = nullptr;
	if (FAILED(GI->Add_Get_GameObjectToLayer(TEXT("MiniGame_Yabawe"), Get_CurLevel(), L"Layer_MiniGame", &Desc, &pObj)))
		return;
	m_pMiniGame_Yabawe = (CMiniGame_Yabawe*)pObj;
}

void CToolManager::End_MiniGame_Block()
{
	CLayer* pLayer = (GI->Get_Layers(TM->Get_CurLevel())->find(TEXT("Mini_Block")))->second;
	for (auto& pObj : *pLayer->Get_GameObjects())
		pObj->Set_Dead();

	CRM->Get_Cam()->End_Target();
}

void CToolManager::Start_MiniGame_Funch()
{
	CGameObject* pObj = nullptr;
	GI->Add_Get_GameObjectToLayer(TEXT("FunchCuma"), Get_CurLevel(), L"Mini_Funch", nullptr, &pObj);
	m_pFunchCuma = (CFunchCuma*)pObj;
	//GI->PlaySound_Linear(L"AirHockey.wav", SD_BGM, 0.3f, true, 0.5f);
	//GI->StopSound(SD_BGM);
	//GI->PlayBGM(L"AirHockey.wav", 0.3f);
}

void CToolManager::Create_MiniGame_Machine()
{
	CGameObject* pObj = nullptr;
	GI->Add_Get_GameObjectToLayer(TEXT("Stage4Machine0"), Get_CurLevel(), L"Mini_Funch", nullptr, &pObj);
	m_pFunchMachine = (CStage4Machine0*)pObj;
}

void CToolManager::End_MiniGame_Funch()
{
	CLayer* pLayer = (GI->Get_Layers(TM->Get_CurLevel())->find(TEXT("Mini_Funch")))->second;

	for (auto& pObj : *pLayer->Get_GameObjects())
		pObj->Set_Dead();

	CRM->Get_Cam()->End_Target();
}

void CToolManager::Start_CutScene_DG()
{
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DGCuma"), LEVEL_STAGE4, TEXT("Layer_CutScene"))))
		return;
}

void CToolManager::End_CutScene_DG()
{
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DGEndCuma"), LEVEL_STAGE4, TEXT("Layer_CutScene"))))
		return;

}

void CToolManager::Make_Potal()
{
	CGameObject* pObj = nullptr;
	if (FAILED(GI->Add_Get_GameObjectToLayer(TEXT("QPotal"), LEVEL_STAGE4, TEXT("Layer_Potal"), nullptr, &pObj)))
		return;

	m_pQPotal = (CQPotal*)pObj;
}

void CToolManager::Make_EPotal()
{
	CGameObject* pObj = nullptr;
	if (FAILED(GI->Add_Get_GameObjectToLayer(TEXT("EPotal"), LEVEL_STAGE4, TEXT("Layer_Potal"), nullptr, &pObj)))
		return;

	m_pEPotal = (CEPotal*)pObj;
}

void CToolManager::Make_CumaHp()
{

	CUI::UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));

	UiInfoDesc.fSizeX = 524.f;
	UiInfoDesc.fSizeY = 42.f;
	UiInfoDesc.fX = 650.5f;
	UiInfoDesc.fY = 76.9f;
	UiInfoDesc.pDesc = this;



	CGameObject* pObj = nullptr;
	if (FAILED(GI->Add_Get_GameObjectToLayer(TEXT("Prototype_GameObject_Bear_HP"), TM->Get_CurLevel(), TEXT("Layer_CumaHP"), &UiInfoDesc, &pObj)))
		return;
	m_pCumaHp = (CBear_HP*)pObj;

	UiInfoDesc.fSizeX = 467.f;
	UiInfoDesc.fY = 81.38f;
	if (FAILED(GI->Add_Get_GameObjectToLayer(TEXT("Prototype_GameObject_Bear_HP_Bar"), TM->Get_CurLevel(), TEXT("Layer_CumaHP"), &UiInfoDesc, &pObj)))
		return;
	m_pCumaHpBar = (CBear_HP_Bar*)pObj;

}

void CToolManager::Deal_CumaHp(_float fAt)
{
	m_pCumaHpBar->Deal_Hp(fAt);
}

void CToolManager::Make_Stage4Triggers()
{
	CGameObject* pObj = nullptr;
	if (FAILED(GI->Add_Get_GameObjectToLayer(TEXT("Stage4Trigger_1"), Get_CurLevel(), L"Layer_Trigger", nullptr, &pObj)))
		return;
	m_pStage4Trigger_1 = (CStage4Trigger_1*)pObj;

	pObj = nullptr;
	if (FAILED(GI->Add_Get_GameObjectToLayer(TEXT("Stage4Trigger_2"), Get_CurLevel(), L"Layer_Trigger", nullptr, &pObj)))
		return;
	m_pStage4Trigger_2 = (CStage4Trigger_2*)pObj;

	pObj = nullptr;
	if (FAILED(GI->Add_Get_GameObjectToLayer(TEXT("Stage4Trigger_3"), Get_CurLevel(), L"Layer_Trigger", nullptr, &pObj)))
		return;
	m_pStage4Trigger_3 = (CStage4Trigger_3*)pObj;

	pObj = nullptr;
	if (FAILED(GI->Add_Get_GameObjectToLayer(TEXT("Stage4Trigger_4"), Get_CurLevel(), L"Layer_Trigger", nullptr, &pObj)))
		return;
	m_pStage4Trigger_4 = (CStage4Trigger_4*)pObj;

	pObj = nullptr;
	if (FAILED(GI->Add_Get_GameObjectToLayer(TEXT("Stage4Trigger_5"), Get_CurLevel(), L"Layer_Trigger", nullptr, &pObj)))
		return;
	m_pStage4Trigger_5 = (CStage4Trigger_5*)pObj;

	pObj = nullptr;
	if (FAILED(GI->Add_Get_GameObjectToLayer(TEXT("Stage4Trigger_6"), Get_CurLevel(), L"Layer_Trigger", nullptr, &pObj)))
		return;
	m_pStage4Trigger_6 = (CStage4Trigger_6*)pObj;

	pObj = nullptr;
	if (FAILED(GI->Add_Get_GameObjectToLayer(TEXT("Stage4Trigger_7"), Get_CurLevel(), L"Layer_Trigger", nullptr, &pObj)))
		return;
	m_pStage4Trigger_7 = (CStage4Trigger_7*)pObj;

	
}

void CToolManager::Open_Stage4Trigger(_uint iIndex)
{
	switch (iIndex)
	{
	case 1:
		if (m_pStage4Trigger_1)
		{
			m_pStage4Trigger_1->Open();
			m_pStage4Trigger_1 = nullptr;
		}
		break;
	case 2:
		if (m_pStage4Trigger_2)
		{
			m_pStage4Trigger_2->Open();
			m_pStage4Trigger_2 = nullptr;
		}		
		break;
	case 3:
		if (m_pStage4Trigger_3)
		{
			m_pStage4Trigger_3->Open();
			m_pStage4Trigger_3 = nullptr;
		}		
		break;
	case 4:
		if (m_pStage4Trigger_4)
		{
			m_pStage4Trigger_4->Open();
			m_pStage4Trigger_4 = nullptr;
		}		
		break;
	case 5:
		if (m_pStage4Trigger_5)
		{
			m_pStage4Trigger_5->Open();
			m_pStage4Trigger_5 = nullptr;
		}		
		break;
	case 6:
		if (m_pStage4Trigger_6)
		{
			m_pStage4Trigger_6->Open();
			m_pStage4Trigger_6 = nullptr;
		}		
		break;
	case 7:
		if (m_pStage4Trigger_7)
		{
			m_pStage4Trigger_7->Open();
			m_pStage4Trigger_7 = nullptr;
		}
		break;
	default:
		break;
	}
}






void CToolManager::Free()
{


	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	ClearManagedChar();
}

