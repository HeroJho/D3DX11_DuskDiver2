#include "stdafx.h"
#include "..\Public\Level_Stage2.h"

#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"
#include "Level_Loading.h"
#include "DataManager.h"
#include "Player_Manager.h"
#include "Crack_Manager.h"
#include "UI.h"
#include "Camera_Manager.h"

CLevel_Stage2::CLevel_Stage2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Stage2::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	Level_Start();

	//GI->PlaySound_Linear(L"SubWay.wav", SD_BGM, 0.3f, true, 0.4f);
	

	return S_OK;
}

void CLevel_Stage2::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CRM->Tick(fTimeDelta);

}

HRESULT CLevel_Stage2::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Stage2_Level"));

	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Lights()
{
	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 1.f);

	if (FAILED(GI->Add_Light(LightDesc)))
		return E_FAIL;


	_float4x4 ShadowMatrix;
	_vector		vLightEye = XMVectorSet(-4.867f, 31.684f, -5.233f, 1.f);
	_vector		vLightAt = XMVectorSet(-13.368f, 1.624f, -4.197f, 1.f);
	_vector		vLightUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMStoreFloat4x4(&ShadowMatrix, XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp));

	GI->Add_ShadowLightViewMatrix(LEVEL_STAGE2, ShadowMatrix);

	return S_OK;
}

HRESULT CLevel_Stage2::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	CUI::UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));

	UiInfoDesc.fSizeX = 322.f * 0.65f;
	UiInfoDesc.fSizeY = 322.f * 0.65f;
	UiInfoDesc.fX = 1160.f;
	UiInfoDesc.fY = 205.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_MiniMapBase"), LEVEL_STAGE2, TEXT("Layer_UI"), &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 451.f - 0.8f;
	UiInfoDesc.fSizeY = 624.f - 0.8f;
	UiInfoDesc.fX = 640.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_MiniMinimap"), LEVEL_STAGE2, TEXT("Layer_UI"), &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 192.f * 0.65f;
	UiInfoDesc.fSizeY = 66.f * 0.65f;
	UiInfoDesc.fX = 1155.f;
	UiInfoDesc.fY = 140.6f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_MiniMapUpside"), LEVEL_STAGE2, TEXT("Layer_UI"), &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 534.f * 0.8f;
	UiInfoDesc.fSizeY = 89.f * 0.8f;
	UiInfoDesc.fX = 640.f;
	UiInfoDesc.fY = 120.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_MapName"), LEVEL_STAGE2, TEXT("Layer_UI"), &UiInfoDesc)))
		return E_FAIL;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage2"), LEVEL_STAGE2, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage2Glass"), LEVEL_STAGE2, pLayerTag)))
		return E_FAIL;

	//if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage2Event2"), LEVEL_STAGE2, pLayerTag)))
	//	return E_FAIL;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage2Player04"), LEVEL_STAGE2, TEXT("Layer_Stage2Event"))))
		return E_FAIL;

	//if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage2Sound"), LEVEL_STAGE2, pLayerTag)))
	//	return E_FAIL;


	return S_OK;
}

void CLevel_Stage2::Level_Start()
{	
	// CRM->Play_CutScene(0, true, true, nullptr);

	// DM->Load_PhysObjData(1);
	DM->Load_LightData(1);
	// DM->Load_ByInstancing(2);
	PM->Change_Navigation(2);
	UM->Set_FrontUI(false);
	UM->Set_Stage2Start(true);
	UM->Set_QuestPointOff();
	
	CRKM->Create_Crack(_float4(-12.6f, 0.f, -17.22f, 1.f), 4.f, 1, TM->Get_CurLevel()); // Stage2ÀÇ ±Õ¿­ À§Ä¡

	GI->StopSound(SD_MAP);
}

CLevel_Stage2 * CLevel_Stage2::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Stage2*		pInstance = new CLevel_Stage2(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_Stage2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Stage2::Free()
{
	__super::Free();

}


