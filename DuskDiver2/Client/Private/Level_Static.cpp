#include "stdafx.h"
#include "..\Public\Level_Static.h"
#include "UI.h"
#include "GameInstance.h"
#include "Level_Loading.h"
#include "Camera_Manager.h"

CLevel_Static::CLevel_Static(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Static::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;


	CRM->Init();


	if (FAILED(Ready_Layer_Static(TEXT("Layer_Static"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Static::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	GI->Reserve_Level((_uint)LEVEL_LOGO);

}

HRESULT CLevel_Static::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Static_Level"));

	return S_OK;
}


HRESULT CLevel_Static::Ready_Layer_Static(const _tchar * pLayerTag)
{

	CCamera::CAMERADESC			CameraDesc;

	CameraDesc.vEye = _float4(0.f, 5.f, -15.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.2f;
	CameraDesc.fFar = 600.0f;

	CameraDesc.TransformDesc.fSpeedPerSec = 25.f;
	CameraDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Camera_Main"), LEVEL_STATIC, pLayerTag, &CameraDesc)))
		return E_FAIL;


	/*
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Model_Tool"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;

	*/



	 //if (FAILED(GI->Add_GameObjectToLayer(TEXT("CutScene_Tool"), LEVEL_STATIC, pLayerTag)))
	 //	return E_FAIL;
	// if (FAILED(GI->Add_GameObjectToLayer(TEXT("AnimMap_Tool"), LEVEL_STATIC, pLayerTag)))
	// 	return E_FAIL; 
	 //if (FAILED(GI->Add_GameObjectToLayer(TEXT("CutScene_Tool"), LEVEL_STATIC, pLayerTag)))
	 //	return E_FAIL;
	
		
	//if (FAILED(GI->Add_GameObjectToLayer(TEXT("PointLight_Tool"), LEVEL_STATIC, pLayerTag)))
	//	return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("AnimMap_Tool"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Px_Tool"), LEVEL_STATIC, pLayerTag)))
		return E_FAIL;
	//if (FAILED(GI->Add_GameObjectToLayer(TEXT("Shader_Tool"), LEVEL_STATIC, pLayerTag)))
	//	return E_FAIL;
	//if (FAILED(GI->Add_GameObjectToLayer(TEXT("PointLight_Tool"), LEVEL_STATIC, pLayerTag)))
	//	return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Static::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CUI::UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));

	UiInfoDesc.fSizeX = 50.f;
	UiInfoDesc.fSizeY = 50.f;
	UiInfoDesc.fX = 640.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_ActiveQuest"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 1280.f;
	UiInfoDesc.fSizeY = 720.f;
	UiInfoDesc.fX = 640.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_EscBase"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 1280.f;
	UiInfoDesc.fSizeY = 1440.f;
	UiInfoDesc.fX = 640.f;
	UiInfoDesc.fY = 1440.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Blind"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;


	UiInfoDesc.fSizeX = 1280.f / 3.f * 2.f;
	UiInfoDesc.fSizeY = 256.f / 3.f * 2.f;
	UiInfoDesc.fX = 640.f;
	UiInfoDesc.fY = 630.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_GolemName"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 1280.f / 3.f * 2.f;
	UiInfoDesc.fSizeY = 256.f / 3.f * 2.f;
	UiInfoDesc.fX = 640.f;
	UiInfoDesc.fY = 630.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_RiddleName"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 1280.f / 3.f * 2.f;
	UiInfoDesc.fSizeY = 256.f / 3.f * 2.f;
	UiInfoDesc.fX = 640.f;
	UiInfoDesc.fY = 630.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_KumaName"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 1280.f / 3.f * 2.f;
	UiInfoDesc.fSizeY = 256.f / 3.f * 2.f;
	UiInfoDesc.fX = 640.f;
	UiInfoDesc.fY = 630.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_ChaosName"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 1280.f;
	UiInfoDesc.fSizeY = 310.f;
	UiInfoDesc.fX = 640.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_BossBase"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;



	UiInfoDesc.fSizeX = 736.f * 0.7f;
	UiInfoDesc.fSizeY = 701.f * 0.7f;
	UiInfoDesc.fX = 640.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_SkillTextBasic"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 1280.f;
	UiInfoDesc.fSizeY = 720.f;
	UiInfoDesc.fX = 640.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_MenuBackGround"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;


	UiInfoDesc.fSizeX = 1280.f;
	UiInfoDesc.fSizeY = 720.f;
	UiInfoDesc.fX = 1920.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_LevelUpBase"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;



	UiInfoDesc.fSizeX = 1280.f;
	UiInfoDesc.fSizeY = 720.f;
	UiInfoDesc.fX = 1920.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_InventoryBase"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 1280.f;
	UiInfoDesc.fSizeY = 720.f;
	UiInfoDesc.fX = 1920.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_EquipMentPriviousBase"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 1280.f;
	UiInfoDesc.fSizeY = 720.f;
	UiInfoDesc.fX = 1920.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_StatusBase"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 1280.f;
	UiInfoDesc.fSizeY = 720.f;
	UiInfoDesc.fX = 1920.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_EquipmentBase"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;


	//UiInfoDesc.fSizeX = 322.f * 0.65f;
	//UiInfoDesc.fSizeY = 322.f * 0.65f;
	//UiInfoDesc.fX = 1150.f;
	//UiInfoDesc.fY = 225.f;
	//UiInfoDesc.pDesc = nullptr;

	//if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_MinimapBackground"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
	//	return E_FAIL;

	UiInfoDesc.fSizeX = 1280.f * 0.6f;
	UiInfoDesc.fSizeY = 800.f * 0.6f;
	UiInfoDesc.fX = g_iWinSizeX * 0.5f;
	UiInfoDesc.fY = g_iWinSizeY * 0.5f;
	UiInfoDesc.pDesc = nullptr;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Clue1"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 1280.f * 0.6f;
	UiInfoDesc.fSizeY = 800.f * 0.6f;
	UiInfoDesc.fX = g_iWinSizeX * 0.5f;
	UiInfoDesc.fY = g_iWinSizeY * 0.5f;
	UiInfoDesc.pDesc = nullptr;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Clue2"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;


	UiInfoDesc.fSizeX = 396.9f;
	UiInfoDesc.fSizeY = 128.8f;
	UiInfoDesc.fX = 193.f;
	UiInfoDesc.fY = 68.1f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Player_Status01"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Player_Status03"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Player_Status04"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 281.6f;
	UiInfoDesc.fSizeY = 281.6f;
	UiInfoDesc.fX = 150.f;
	UiInfoDesc.fY = 348.44f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Burst_BackGround"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 218.4f;
	UiInfoDesc.fSizeY = 196.7f;
	UiInfoDesc.fX = 130.f;
	UiInfoDesc.fY = 623.f;
	//UiInfoDesc.fY = 598.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Cross"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	/*UiInfoDesc.fSizeX = 171.6f;
	UiInfoDesc.fSizeY = 154.55f;*/
	UiInfoDesc.fSizeX = 187.2f;
	UiInfoDesc.fSizeY = 168.6f;
	UiInfoDesc.fX = 1139.3f;
	UiInfoDesc.fY = 583.7f;
	//UiInfoDesc.fY = 598.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Cross_Item"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 168.f;
	UiInfoDesc.fSizeY = 38.4f;
	UiInfoDesc.fX = 145.f;
	UiInfoDesc.fY = 223.499420f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Hit"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 263.4f;
	UiInfoDesc.fSizeY = 67.8f;
	UiInfoDesc.fX = 1186.f;
	UiInfoDesc.fY = 74.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Task_BackGround"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 64.f;
	UiInfoDesc.fSizeY = 25.6f;
	UiInfoDesc.fX = 1207.84f;
	UiInfoDesc.fY = 693.17f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Dash_Info"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 297.5f;
	UiInfoDesc.fSizeY = 164.5f;
	UiInfoDesc.fX = 640.f;
	UiInfoDesc.fY = 623.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Coin_BackGround"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;


	UiInfoDesc.fSizeX = 1280.f;
	UiInfoDesc.fSizeY = 720.f;
	UiInfoDesc.fX = 640.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_StoryQuest_BackGround1"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 1280.f;
	UiInfoDesc.fSizeY = 720.f;
	UiInfoDesc.fX = 640.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_PartyOn_BackGround2"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 1280.f;
	UiInfoDesc.fSizeY = 720.f;
	UiInfoDesc.fX = 640.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_ShopBase"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	//UiInfoDesc.fSizeX = 1280.f;
	//UiInfoDesc.fSizeY = 720.f;
	//UiInfoDesc.fX = 640.f;
	//UiInfoDesc.fY = 360.f;
	//UiInfoDesc.pDesc = nullptr;

	//if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_DDStart"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
	//	return E_FAIL;

	// Äù½ºÆ® ´À³¦Ç¥
	

	//UiInfoDesc.fSizeX = 451.f;
	//UiInfoDesc.fSizeY = 624.f;
	//UiInfoDesc.fX = 640.f;
	//UiInfoDesc.fY = 360.f;
	//UiInfoDesc.pDesc = nullptr;

	//if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_TutorialMinimap"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
	//	return E_FAIL;



	UiInfoDesc.fSizeX = 860.f * 0.7f;
	UiInfoDesc.fSizeY = 464.f * 0.7f;
	UiInfoDesc.fX = 540.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_StreetVendor1"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 860.f * 0.7f;
	UiInfoDesc.fSizeY = 464.f * 0.7f;
	UiInfoDesc.fX = 540.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_StreetVendor2"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 860.f * 0.7f;
	UiInfoDesc.fSizeY = 464.f * 0.7f;
	UiInfoDesc.fX = 540.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_StreetVendor3"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 860.f * 0.7f;
	UiInfoDesc.fSizeY = 464.f * 0.7f;
	UiInfoDesc.fX = 540.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_StreetVendor4"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	//UiInfoDesc.fSizeX = 860.f * 0.7f;
	//UiInfoDesc.fSizeY = 464.f * 0.7f;
	//UiInfoDesc.fX = 540.f;
	//UiInfoDesc.fY = 360.f;
	//UiInfoDesc.pDesc = nullptr;

	//if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_StreetVendor5"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
	//	return E_FAIL;

	UiInfoDesc.fSizeX = 860.f;
	UiInfoDesc.fSizeY = 484.f;
	UiInfoDesc.fX = 640.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_EquipmentBase"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	//UiInfoDesc.fSizeX = 320.f;
	//UiInfoDesc.fSizeY = 0.01f;
	//UiInfoDesc.fX = 200.f;
	//UiInfoDesc.fY = 160.f;
	//UiInfoDesc.pDesc = nullptr;

	//if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_BattleEndBase"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
	//	return E_FAIL;

	//UiInfoDesc.fSizeX = 216.4f;
	//UiInfoDesc.fSizeY = 58.4f;
	//UiInfoDesc.fX = 0.f;
	//UiInfoDesc.fY = 170.f;
	//UiInfoDesc.pDesc = nullptr;

	//if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_BattleEnd"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
	//	return E_FAIL;


	UiInfoDesc.fSizeX = 315.f * 0.4f;
	UiInfoDesc.fSizeY = 80.f * 0.4f;
	UiInfoDesc.fX = 740.f;
	UiInfoDesc.fY = 420.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_TalkButton"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 783.f * 0.7f;
	UiInfoDesc.fSizeY = 742.f * 0.7f;
	UiInfoDesc.fX = 640.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_PictureBurst"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;


	return S_OK;
}

CLevel_Static * CLevel_Static::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Static*		pInstance = new CLevel_Static(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_Static"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Static::Free()
{
	__super::Free();

}


