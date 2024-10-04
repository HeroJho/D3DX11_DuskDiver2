#include "stdafx.h"
#include "..\Public\Level_Test.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "ToolManager.h"
#include "Player_Manager.h"
#include "UI.h"
#include "Creture.h"
#include "UI_Manager.h"
#include "DataManager.h"
#include "Monster_Manager.h"
#include "Camera_Manager.h"
#include "Stage5.h"
#include "PxObj.h"
#include "Holo_1.h"
#include "Particle_Manager.h"
#include "EffectSpin.h"
#include "EffectShield.h"
#include "EffectShieldHit.h"
#include "EffectOctopusRange.h"
#include "NPC_T.h"

#include "Skybox.h"

#include "Camera_Manager.h"

#include "MiniGame_Block.h"

#include "CrystalBody.h"
#include "CrystalPart.h"
#include "Fissure.h"
#include "CrackStone.h"
#include "Holo_9.h"
#include "OctopusParticle.h"

CLevel_Test::CLevel_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Test::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	DM->Load_LightData(0);

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Dialog(TEXT("Layer_Dialog"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	//CCreture::CREATEINFO _tInfo;
	//ZeroMemory(&_tInfo, sizeof(CCreture::CREATEINFO));

	//_tInfo.iNaviNum = 1;
	//_tInfo.vPosition = { 0.f, 0.f, 20.f, 1.f };
	//_tInfo.vDir = { 0.f, 0.f, 1.f };
	//_tInfo.iIndex = 2;

	///*if (FAILED(GI->Add_GameObjectToLayer(TEXT("DisasterBeta"), LEVEL_TEST, L"a", &_tInfo)))
	//	return E_FAIL;*/
	//if (FAILED(GI->Add_GameObjectToLayer(TEXT("MechanicalOctopus"), LEVEL_TEST, L"a", &_tInfo)))
	//	return E_FAIL;

	PTM->LoadAll();

	DM->Load_PhysObjData(0);
	UM->Set_FrontUI(false);
	

	return S_OK;
}

void CLevel_Test::Tick(_float fTimeDelta)
{
	CRM->Tick(fTimeDelta);


	

	//if (CK_DOWN(DIK_F7))
	//{
	//	GI->PlaySound_Linear(L"BGM_Battle_LM.wav", SD_BGM, 0.1f, true, 0.1f);
	//}
	//else if (CK_DOWN(DIK_F8))
	//{
	//	GI->PlaySound_Linear(L"SubWay.wav", SD_BGM, 0.4f, true, 0.1f);
	//}

	if (CK_DOWN(DIK_HOME))
	{
		
		/*
		PM->Change_PlayerPos(PM->Get_SelectPlayer(), vPos);	*/	
		TM->Change_Level(LEVEL_STAGE5);
		
	}

	if (CK_DOWN(DIK_L))
		PM->Player01ChangeOff();

	__super::Tick(fTimeDelta);
}

HRESULT CLevel_Test::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;




	return S_OK;
}

HRESULT CLevel_Test::Ready_Lights()
{

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	// 주광원
	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(0.f, -1.f, -1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.5f, 0.5f, 0.5f, 1.f);
	LightDesc.vSpecular = _float4(0.3f, 0.3f, 0.3f, 1.f);

	if (FAILED(pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;



	_float4x4 ShadowMatrix;
	_vector		vLightEye = XMVectorSet(24.307f, 28.954f, -19.855f, 1.f);
	_vector		vLightAt = XMVectorSet(6.116f, 1.323f, -9.752f, 1.f);
	_vector		vLightUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMStoreFloat4x4(&ShadowMatrix, XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp));

	GI->Add_ShadowLightViewMatrix(LEVEL_TEST, ShadowMatrix);



	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


HRESULT CLevel_Test::Ready_Layer_BackGround(const _tchar * pLayerTag)
{

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage1"), LEVEL_TEST, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage1Light"), LEVEL_TEST, pLayerTag)))
		return E_FAIL;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage1Event3"), LEVEL_TEST, L"Layer_Event")))
		return E_FAIL;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("NPCTutorial"), LEVEL_TEST, pLayerTag)))
		return E_FAIL;

	CHolo_1::HOLODESC HoloDesc;
	HoloDesc.vPos = _float3(-1.2f, 2.3f, 29.1f);
	HoloDesc.vRot = _float3(90.f, 0.f, 0.f);
	HoloDesc.vScale = _float3(5.f, 1.f, 2.5f);
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("RealHolo_4"), LEVEL_TEST, pLayerTag, &HoloDesc)))
		return E_FAIL;

	HoloDesc.vPos = _float3(3.f, 2.2f, 28.7f);
	HoloDesc.vRot = _float3(90.f, 0.f, 0.f);
	HoloDesc.vScale = _float3(2.f, 3.f, 2.5f);
	CGameObject* pObj = nullptr;
	if (FAILED(GI->Add_Get_GameObjectToLayer(TEXT("Holo_9"), LEVEL_TEST, pLayerTag, &HoloDesc, &pObj)))
		return E_FAIL;
	((CHolo_9*)pObj)->Set_StartTime(5.f);

	return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_Player(const _tchar * pLayerTag)
{
	if (!TM->Get_PlayerCreate())
	{
		PM->Create_Players();
		TM->Set_PlayerCreate();
		CRM->Get_Cam()->Set_CamPos(0.f, 20.f);
	}
	return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CUI::UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));

	UiInfoDesc.fSizeX = g_iWinSizeX;
	UiInfoDesc.fSizeY = g_iWinSizeY;
	UiInfoDesc.fX = g_iWinSizeX * 0.5f;
	UiInfoDesc.fY = g_iWinSizeY * 0.5f;
	UiInfoDesc.pDesc = nullptr;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("BlockWin"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	
	CUI::UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));

	UiInfoDesc.fSizeX = 322.f * 0.65f;
	UiInfoDesc.fSizeY = 322.f * 0.65f;
	UiInfoDesc.fX = 1160.f;
	UiInfoDesc.fY = 205.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_MiniMapBase"), LEVEL_TEST, TEXT("Layer_UI"), &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 451.f - 0.8f;
	UiInfoDesc.fSizeY = 624.f - 0.8f;
	UiInfoDesc.fX = 640.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_MiniMinimap"), LEVEL_TEST, TEXT("Layer_UI"), &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 192.f * 0.65f;
	UiInfoDesc.fSizeY = 66.f * 0.65f;
	UiInfoDesc.fX = 1155.f;
	UiInfoDesc.fY = 133.6f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_MiniMapUpside"), LEVEL_TEST, TEXT("Layer_UI"), &UiInfoDesc)))
		return E_FAIL;

	CCreture::CREATEINFO _tInfo;
	ZeroMemory(&_tInfo, sizeof(CCreture::CREATEINFO));

	_tInfo.iNaviNum = 1;
	_tInfo.vPosition = { 0.f, 0.f, -17.53f, 1.f };
	_tInfo.vDir = { 0.f, 0.f, 1.f };
	_tInfo.iIndex = 0;

	
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("TrainingRobot"), LEVEL_TEST, pLayerTag, &_tInfo)))
		return E_FAIL;
		
	_tInfo.vPosition = { -4.5f, 0.f, -1.31f, 1.f };
	_tInfo.vDir = { 2.f, 0.f, -9.5f};
	_tInfo.iIndex = 1;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("TrainingRobot"), LEVEL_TEST, pLayerTag, &_tInfo)))
		return E_FAIL;

	_tInfo.vPosition = { 6.2f, 0.f, -3.01f, 1.f };
	_tInfo.vDir = { -4.f, 0.f,  -9.5f };
	_tInfo.iIndex = 2;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("TrainingRobot"), LEVEL_TEST, pLayerTag, &_tInfo)))
		return E_FAIL;




	MM->Set_StageMonsterCount(3);

	//!!!!!!!!!!!!!!테스트 여기서 이름 변경해서 쓰세요!!!!!!!!!!!!!!!!!
	//if (FAILED(GI->Add_GameObjectToLayer(TEXT("BombRobot"), LEVEL_TEST, pLayerTag, &_tInfo)))
	//	return E_FAIL;
	

	/*if (FAILED(GI->Add_GameObjectToLayer(TEXT("DefenceRobot"), LEVEL_TEST, pLayerTag, &_tInfo)))
		return E_FAIL;*/
	
	/*
	_tInfo.vPosition = { -5.f, 0.f, 5.f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DisasterBeta"), LEVEL_TEST, pLayerTag, &_tInfo)))
		return E_FAIL;

	_tInfo.vPosition = { 0.f, 0.f, 5.f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DisasterBoss"), LEVEL_TEST, pLayerTag, &_tInfo)))
		return E_FAIL;*/

	/*_tInfo.vPosition = { 5.f, 0.f,5.f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Golem"), LEVEL_TEST, pLayerTag, &_tInfo)))
		return E_FAIL;

	_tInfo.vPosition = { 5.f, 0.f,-5.f, 1.f };

	DisasterBeta

	 if (FAILED(GI->Add_GameObjectToLayer(TEXT("BombRobot"), LEVEL_TEST, pLayerTag, &_tInfo)))
	 	return E_FAIL;

	 _tInfo.vPosition = { 0.f, 0.f, 5.f, 1.f };

	 if (FAILED(GI->Add_GameObjectToLayer(TEXT("OldBombRobot"), LEVEL_TEST, pLayerTag, &_tInfo)))
		 return E_FAIL;

	_tInfo.vPosition = { 0.f, 0.f, 5.f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Beast"), LEVEL_TEST, pLayerTag, &_tInfo)))
		return E_FAIL;
		*/

	/*_tInfo.vPosition = { 0.f, 0.f, 5.f, 1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("MechanicalOctopus"), LEVEL_TEST, pLayerTag, &_tInfo)))
		return E_FAIL;*/

	/*_tInfo.vPosition = { 0.f, 0.f, 5.f, 1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DefenceRobot"), LEVEL_TEST, pLayerTag, &_tInfo)))
		return E_FAIL;*/

	return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_Dialog(const _tchar * pLayerTag)
{
	CUI::UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));

	UiInfoDesc.fSizeX = 1280.f;
	UiInfoDesc.fSizeY = 720.f;
	UiInfoDesc.fX = 640.f - 20.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_DialogTalk"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_DialogProfile"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fX = 640.f + 100.f;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_BattleDialog"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Test::Stage5_Debug(const _tchar * pLayerTag)
{
	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(-1.f, -1.f, -1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.2f, 0.2f, 0.2f, 1.f);
	LightDesc.vSpecular = _float4(0.5f, 0.5f, 0.5f, 1.f);

	if (FAILED(GI->Add_Light(LightDesc)))
		return E_FAIL;



	_float4x4 ShadowMatrix;
	_vector		vLightEye = XMVectorSet(62.416f, 145.776f, -21.242f, 1.f);
	_vector		vLightAt = XMVectorSet(60.451f, 138.411f, -16.838f, 1.f);
	_vector		vLightUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMStoreFloat4x4(&ShadowMatrix, XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp));

	GI->Add_ShadowLightViewMatrix(LEVEL_TEST, ShadowMatrix);

	CStage5::POS a;
	a.vPos = { 0.f,0.f,0.f,1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage5"), LEVEL_TEST, pLayerTag, &a)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage5Light"), LEVEL_TEST, pLayerTag)))
		return E_FAIL;
	
	CCreture::CREATEINFO _tInfo;
	ZeroMemory(&_tInfo, sizeof(CCreture::CREATEINFO));

	_tInfo.iNaviNum = 5;
	_tInfo.vPosition = { -0.307f, 0.665f, -17.077f, 1.f };
	_tInfo.vDir = { 0.f, 0.f, 1.f };
	_tInfo.iIndex = 0;


	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Beast"), LEVEL_TEST, pLayerTag, &_tInfo)))
		return E_FAIL;

	_tInfo.iNaviNum = 5;
	_tInfo.vPosition = { -0.307f, 0.665f, -17.077f, 1.f };
	_tInfo.iIndex = 1;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("BeastPillar"), LEVEL_TEST, pLayerTag, &_tInfo)))
		return E_FAIL;

	return S_OK;
}

CLevel_Test * CLevel_Test::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Test*		pInstance = new CLevel_Test(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_Test"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Test::Free()
{
	__super::Free();
}


