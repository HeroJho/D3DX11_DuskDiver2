#include "stdafx.h"
#include "..\Public\Level_Stage3.h"
#include "Monster_Manager.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Level_Loading.h"

#include "UI_Manager.h"
#include "StreetStore.h"
#include "NPCIdle.h"
#include "NPCWalk.h"
#include "DataManager.h"
#include "Player_Manager.h"
#include "Camera_Manager.h"
#include "Crack_Manager.h"
#include "Buildings.h"
#include "Stage3Ad.h"
#include "Stage3Door.h"
#include "Stage3Cars.h"
#include "Stage3Bus.h"

#include "MetalTrashbinBody.h"
#include "CrystalBody.h"

#include "Skybox.h"
#include "UI.h"

CLevel_Stage3::CLevel_Stage3(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Stage3::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	Level_Start();
	//GI->PlaySound_Linear(L"BGM_Day.wav", SD_BGM, 0.3f, true, 0.4f);

	return S_OK;
}


void CLevel_Stage3::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


	CRM->Tick(fTimeDelta);

}

HRESULT CLevel_Stage3::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CLevel_Stage3::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
#pragma region 정적배경
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage3"), LEVEL_STAGE3, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage3Glass"), LEVEL_STAGE3, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage3Water"), LEVEL_STAGE3, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage3Trees"), LEVEL_STAGE3, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage3Terrain"), LEVEL_STAGE3, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("AzitLight"), LEVEL_STAGE3, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("AzitProp"), LEVEL_STAGE3, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage3Event1"), LEVEL_STAGE3, pLayerTag)))
		return E_FAIL;


	if (FAILED(GI->Add_GameObjectToLayer(TEXT("AzitEvent"), LEVEL_STAGE3, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("AzitEvent_1"), LEVEL_STAGE3, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("AzitEvent_2"), LEVEL_STAGE3, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("AzitEvent_3"), LEVEL_STAGE3, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("AzitEvent_4"), LEVEL_STAGE3, pLayerTag)))
		return E_FAIL;


	
	CMetalTrashbinBody::METALTRASHBINDESC MetalTrashbinDesc;
	for (_uint i = 0; 10 > i; ++i)
	{
		MetalTrashbinDesc.iTrashbinIndex = i;
		if (FAILED(GI->Add_GameObjectToLayer(TEXT("MetalTrashbinBody"), LEVEL_STAGE3, pLayerTag, &MetalTrashbinDesc)))
			return E_FAIL;
	}
	CCrystalBody::BODYDESC BodyDesc;
	BodyDesc.vPosition = { 9.17f,0.f,202.1f,1.f };
	BodyDesc.vRadianAngle = 1.f;
	BodyDesc.bClue = false;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE3, pLayerTag, &BodyDesc)))
		return E_FAIL;
	BodyDesc.vPosition = { 11.75f,0.f,121.02f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE3, pLayerTag, &BodyDesc)))
		return E_FAIL;
	// BodyDesc.bClue = true;
	BodyDesc.vPosition = { 82.89f,0.f,151.69f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE3, pLayerTag, &BodyDesc)))
		return E_FAIL;


	// 옥상
	BodyDesc.bClue = true;
	BodyDesc.vPosition = { 56.9f, 33.143f, 86.902f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE3, pLayerTag, &BodyDesc)))
		return E_FAIL;
	BodyDesc.bClue = false;
	BodyDesc.vPosition = { 89.764f,33.143f,125.649f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE3, pLayerTag, &BodyDesc)))
		return E_FAIL;
	BodyDesc.bClue = false;
	BodyDesc.vPosition = { 79.142f,33.143f,91.264f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE3, pLayerTag, &BodyDesc)))
		return E_FAIL;
	BodyDesc.bClue = false;
	BodyDesc.vPosition = { 81.022f,33.143f,72.869f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE3, pLayerTag, &BodyDesc)))
		return E_FAIL;



	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPC_Line00"), LEVEL_STAGE3, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPC_Line01"), LEVEL_STAGE3, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPC_Line02"), LEVEL_STAGE3, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPC_Line03"), LEVEL_STAGE3, pLayerTag)))
		return E_FAIL;	
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPC_Line04"), LEVEL_STAGE3, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPC_Line05"), LEVEL_STAGE3, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPC_Line06"), LEVEL_STAGE3, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPC_Line07"), LEVEL_STAGE3, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPC_Line08"), LEVEL_STAGE3, pLayerTag)))
		return E_FAIL;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage3Sound"), LEVEL_STAGE3, pLayerTag)))
		return E_FAIL;


	CBuildings::BDDESC BDDesc;
	for (_uint i = 0; 6 > i; ++i)
	{
		BDDesc.iIndex = (_uint)(i * 0.5f);
		BDDesc.iModelType = i % 2;
		if (FAILED(GI->Add_GameObjectToLayer(TEXT("Buildings"), LEVEL_STAGE3, pLayerTag, &BDDesc)))
			return E_FAIL;
	}

	// 전광판
	CStage3Ad::ADDESC ADDesc;
	ADDesc.iADType = CStage3Ad::TYPE_UV ;
	ADDesc.iADIndex = 0;
	ADDesc.iPassIndex = 16;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage3AD"), LEVEL_STAGE3, pLayerTag, &ADDesc)))
		return E_FAIL;

	for (_uint i = 0; 2 > i; ++i)
	{
		ADDesc.iADType = CStage3Ad::TYPE_ACC;
		ADDesc.iADIndex = i;
		ADDesc.iPassIndex = 17;
		if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage3AD"), LEVEL_STAGE3, pLayerTag, &ADDesc)))
			return E_FAIL;
	}

	// 아지트문
	CStage3Door::DOORDESC DoorDesc;
	for (_uint i = 0; 2 > i; ++i)
	{
		DoorDesc.iDoorIndex = i;
		DoorDesc.fMinZ = 1.5f * i - 1.5f;
		DoorDesc.fMaxZ = 1.5f * i;
		if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage3Door"), LEVEL_STAGE3, pLayerTag, &DoorDesc)))
			return E_FAIL;
		if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage3DoorGlass"), LEVEL_STAGE3, pLayerTag, &DoorDesc)))
			return E_FAIL;
	}

	// 길거리 차들
	CStage3Cars::CARSDESC CarsDesc;
	for (_uint i = 0; 4 > i; ++i)
	{
		CarsDesc.iLaneIndex = i;
		if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage3Cars"), LEVEL_STAGE3, pLayerTag, &CarsDesc)))
			return E_FAIL;
	}
	// 버스
	CStage3Bus::BUSDESC BusDesc;
	for (_uint i = 0; 2 > i; ++i)
	{
		BusDesc.iLaneIndex = 5 * i;
		if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage3Bus"), LEVEL_STAGE3, pLayerTag, &BusDesc)))
			return E_FAIL;
	}

	// 노점상
	for (_uint i = 0; 6 > i; ++i)
	{
		CStreetStore::STREETSTOREDESC StreetStoreDesc;
		StreetStoreDesc.iStoreIndex = i;
		if (FAILED(GI->Add_GameObjectToLayer(TEXT("StreetStore"), LEVEL_STAGE3, pLayerTag, &StreetStoreDesc)))
			return E_FAIL;
	}


	CSkybox::SKYDESC SkyDesc;
	SkyDesc.iSkyIndex = 1;
	SkyDesc.bDisolve = false;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Skybox"), LEVEL_STAGE3, pLayerTag, &SkyDesc)))
		return E_FAIL;
	SkyDesc.iSkyIndex = 0;
	SkyDesc.bDisolve = true;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Skybox"), LEVEL_STAGE3, pLayerTag, &SkyDesc)))
		return E_FAIL;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("RedWindow"), LEVEL_STAGE3, pLayerTag)))
		return E_FAIL;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("UpPotal"), LEVEL_STAGE3, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DownPotal"), LEVEL_STAGE3, pLayerTag)))
		return E_FAIL;

#pragma endregion 정적배경

	CUI::UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));

	UiInfoDesc.fSizeX = 322.f * 0.65f;
	UiInfoDesc.fSizeY = 322.f * 0.65f;
	UiInfoDesc.fX = 1160.f;
	UiInfoDesc.fY = 205.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_MiniMapBase"), LEVEL_STAGE3, TEXT("Layer_UI"), &UiInfoDesc)))
		return E_FAIL;


	UiInfoDesc.fSizeX = 451.f - 0.8f;
	UiInfoDesc.fSizeY = 624.f - 0.8f;
	UiInfoDesc.fX = 640.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_MiniMinimap"), LEVEL_STAGE3, TEXT("Layer_UI"), &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 192.f * 0.65f;
	UiInfoDesc.fSizeY = 66.f * 0.65f;
	UiInfoDesc.fX = 1155.f;
	UiInfoDesc.fY = 132.6f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_MiniMapUpside"), LEVEL_STAGE3, TEXT("Layer_UI"), &UiInfoDesc)))
		return E_FAIL;

	//UiInfoDesc.fSizeX = 534.f * 0.8f;
	//UiInfoDesc.fSizeY = 89.f * 0.8f;
	//UiInfoDesc.fX = 640.f;
	//UiInfoDesc.fY = 120.f;
	//UiInfoDesc.pDesc = nullptr;

	//if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_MapName1"), LEVEL_STAGE2, TEXT("Layer_UI"), &UiInfoDesc)))
	//	return E_FAIL;



	
	
#pragma region 동적배경

#pragma region NPCIdle
	/*CNPCIdle::NPCIDLEDESC NPCIdleDesc;
	NPCIdleDesc.iNPCIndex = 0;
	NPCIdleDesc.iIdleIndex = 3;
	NPCIdleDesc.iModelIndex = 13;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPCIdle"), LEVEL_STAGE3, pLayerTag, &NPCIdleDesc)))
	return E_FAIL;
	NPCIdleDesc.iNPCIndex = 1;
	NPCIdleDesc.iIdleIndex = 3;
	NPCIdleDesc.iModelIndex = 13;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPCIdle"), LEVEL_STAGE3, pLayerTag, &NPCIdleDesc)))
	return E_FAIL;
	NPCIdleDesc.iNPCIndex = 2;
	NPCIdleDesc.iIdleIndex = 1;
	NPCIdleDesc.iModelIndex = 13;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPCIdle"), LEVEL_STAGE3, pLayerTag, &NPCIdleDesc)))
	return E_FAIL;
	NPCIdleDesc.iNPCIndex = 3;
	NPCIdleDesc.iIdleIndex = 3;
	NPCIdleDesc.iModelIndex = 13;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPCIdle"), LEVEL_STAGE3, pLayerTag, &NPCIdleDesc)))
	return E_FAIL;
	NPCIdleDesc.iNPCIndex = 4;
	NPCIdleDesc.iIdleIndex = 1;
	NPCIdleDesc.iModelIndex = 13;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPCIdle"), LEVEL_STAGE3, pLayerTag, &NPCIdleDesc)))
	return E_FAIL;
	NPCIdleDesc.iNPCIndex = 5;
	NPCIdleDesc.iIdleIndex = 1;
	NPCIdleDesc.iModelIndex = 11;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPCIdle"), LEVEL_STAGE3, pLayerTag, &NPCIdleDesc)))
	return E_FAIL;*/
#pragma endregion NPCIdle
	
#pragma region NPCWalk
		// NPCWalk
	CNPCWalk::NPCWALKDESC NPCWalkDesc;
	NPCWalkDesc.iModelIndex = 1;
	NPCWalkDesc.iRouteIndex = 4;
	NPCWalkDesc.iStartNodeIndex = 0;
	NPCWalkDesc.vPosition = { 46.f,0.f,180.5f,1.f };
	//NPCWalkDesc.vPosition = { 0.f,0.f,0.f,1.f }; // 디버깅용
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPCWalk"), LEVEL_STAGE3, pLayerTag, &NPCWalkDesc)))
		return E_FAIL;
	NPCWalkDesc.iModelIndex = 2;
	NPCWalkDesc.iRouteIndex = 1;
	NPCWalkDesc.iStartNodeIndex = 1;
	NPCWalkDesc.vPosition = { 48.f,0.f,135.f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPCWalk"), LEVEL_STAGE3, pLayerTag, &NPCWalkDesc)))
		return E_FAIL;
	NPCWalkDesc.iModelIndex = 3;
	NPCWalkDesc.iRouteIndex = 1;
	NPCWalkDesc.iStartNodeIndex = 10;
	NPCWalkDesc.vPosition = { 10.5f,0.f,116.5f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPCWalk"), LEVEL_STAGE3, pLayerTag, &NPCWalkDesc)))
		return E_FAIL;
	NPCWalkDesc.iModelIndex = 4;
	NPCWalkDesc.iRouteIndex = 2;
	NPCWalkDesc.iStartNodeIndex = 0;
	NPCWalkDesc.vPosition = { 10.f,0.f,48.5f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPCWalk"), LEVEL_STAGE3, pLayerTag, &NPCWalkDesc)))
		return E_FAIL;
	NPCWalkDesc.iModelIndex = 5;
	NPCWalkDesc.iRouteIndex = 3;
	NPCWalkDesc.iStartNodeIndex = 4;
	NPCWalkDesc.vPosition = { -16.f,0.f,142.f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPCWalk"), LEVEL_STAGE3, pLayerTag, &NPCWalkDesc)))
		return E_FAIL;
	NPCWalkDesc.iModelIndex = 6;
	NPCWalkDesc.iRouteIndex = 3;
	NPCWalkDesc.iStartNodeIndex = 1;
	NPCWalkDesc.vPosition = { 9.f,0.f,113.f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPCWalk"), LEVEL_STAGE3, pLayerTag, &NPCWalkDesc)))
		return E_FAIL;
	NPCWalkDesc.iModelIndex = 7;
	NPCWalkDesc.iRouteIndex = 1;
	NPCWalkDesc.iStartNodeIndex = 8;
	NPCWalkDesc.vPosition = { -15.f,0.f,152.5f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPCWalk"), LEVEL_STAGE3, pLayerTag, &NPCWalkDesc)))
		return E_FAIL;
	NPCWalkDesc.iModelIndex = 8;
	NPCWalkDesc.iRouteIndex = 2;
	NPCWalkDesc.iStartNodeIndex = 4;
	NPCWalkDesc.vPosition = { -48.5f,0.f,111.f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPCWalk"), LEVEL_STAGE3, pLayerTag, &NPCWalkDesc)))
		return E_FAIL;
	NPCWalkDesc.iModelIndex = 9;
	NPCWalkDesc.iRouteIndex = 2;
	NPCWalkDesc.iStartNodeIndex = 12;
	NPCWalkDesc.vPosition = { 32.5f,0.f,179.f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPCWalk"), LEVEL_STAGE3, pLayerTag, &NPCWalkDesc)))
		return E_FAIL;
	NPCWalkDesc.iModelIndex = 10;
	NPCWalkDesc.iRouteIndex = 1;
	NPCWalkDesc.iStartNodeIndex = 11;
	NPCWalkDesc.vPosition = { 28.f,0.f,88.5f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPCWalk"), LEVEL_STAGE3, pLayerTag, &NPCWalkDesc)))
		return E_FAIL;
	NPCWalkDesc.iModelIndex = 12;
	NPCWalkDesc.iRouteIndex = 4;
	NPCWalkDesc.iStartNodeIndex = 4;
	NPCWalkDesc.vPosition = { 90.f,0.f,187.f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPCWalk"), LEVEL_STAGE3, pLayerTag, &NPCWalkDesc)))
		return E_FAIL;
	NPCWalkDesc.iModelIndex = 5;
	NPCWalkDesc.iRouteIndex = 5;
	NPCWalkDesc.iStartNodeIndex = 0;
	NPCWalkDesc.vPosition = { -0.5f,0.f,-8.f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPCWalk"), LEVEL_STAGE3, pLayerTag, &NPCWalkDesc)))
		return E_FAIL;
	NPCWalkDesc.iModelIndex = 10;
	NPCWalkDesc.iRouteIndex = 5;
	NPCWalkDesc.iStartNodeIndex = 4;
	NPCWalkDesc.vPosition = { 25.5f,0.f,6.5f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPCWalk"), LEVEL_STAGE3, pLayerTag, &NPCWalkDesc)))
		return E_FAIL;
#pragma endregion NPCWalk

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPCPartTime"), LEVEL_STAGE3, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPCZengbo"), LEVEL_STAGE3, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPCDog"), LEVEL_STAGE3, pLayerTag)))
		return E_FAIL;
	
#pragma endregion 동적배경

	
	return S_OK;
}

HRESULT CLevel_Stage3::Ready_Lights()
{
	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(-1.f, -1.f, -1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.9f, 0.9f, 0.9f, 1.f);
	LightDesc.vSpecular = _float4(0.5f, 0.5f, 0.5f, 1.f);

	if (FAILED(GI->Add_Light(LightDesc)))
		return E_FAIL;

	

	_float4x4 ShadowMatrix;
	_vector		vLightEye = XMVectorSet(62.416f, 145.776f, -21.242f, 1.f);
	_vector		vLightAt = XMVectorSet(60.451f, 138.411f, -16.838f, 1.f);
	_vector		vLightUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMStoreFloat4x4(&ShadowMatrix, XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp));

	GI->Add_ShadowLightViewMatrix(LEVEL_STAGE3, ShadowMatrix);


	return S_OK;
}

void CLevel_Stage3::Level_Start()
{ 
	DM->Load_PhysObjData(2);
 	DM->Load_LightData(3);
	DM->Load_ByInstancing(0);
	PM->Change_Navigation(3);

	UM->Set_QuestPointOff();

	UM->Set_FrontUI(false);
	CRM->Play_CutScene(2, true, false, nullptr);
	
	GI->StopSound(SD_MAP);
}

CLevel_Stage3 * CLevel_Stage3::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Stage3*		pInstance = new CLevel_Stage3(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_Stage3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Stage3::Free()
{
	__super::Free();

}


