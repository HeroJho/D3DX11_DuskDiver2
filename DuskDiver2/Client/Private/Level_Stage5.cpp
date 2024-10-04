#include "stdafx.h"
#include "..\Public\Level_Stage5.h"

#include "GameInstance.h"
#include "ToolManager.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "Creture.h"
#include "Level_Loading.h"
#include "DataManager.h"
#include "UI.h"
#include "Camera_Manager.h"

CLevel_Stage5::CLevel_Stage5(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Stage5::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	Level_Start();
	//GI->PlaySound_Linear(L"BGM_BossLast.wav", SD_BGM, 0.3f, true, 0.4f);

	return S_OK;
}

void CLevel_Stage5::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


	CRM->Tick(fTimeDelta);


	//if (CK_DOWN(DIK_F7))
	//{
	//	CRM->Get_Cam()->Set_CamPos(360.f, 20.f);
	//	PM->Set_SceneIdle(PM->Get_SelectPlayer());
	//	PM->Set_SceneLook(PM->Get_SelectPlayer(), _float4(0.249f, -28.738f, 166.402, 1.f));
	//}


}

HRESULT CLevel_Stage5::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Stage5_Level"));

	return S_OK;
}

void CLevel_Stage5::Level_Start()
{
	DM->Load_PhysObjData(6);
	PM->Change_Navigation(5);
	_float4 vPos = { 0.619f, -32.962f, -158.067f, 1.f };
	PM->Set_ScenePos(PM->Get_SelectPlayer(), vPos);
	UM->Set_QuestPointOff();
	//UM->MoveBlind(false);

	// TM->Set_StartBossCutScene(true);
	UM->Set_TalkIndex(91);
	UM->Set_FrontUI(false);

	CRM->Get_Cam()->Set_CamPos(360.f, 20.f);
	PM->Set_SceneIdle(PM->Get_SelectPlayer());
	PM->Set_SceneLook(PM->Get_SelectPlayer(), _float4(0.249f, -28.738f, 166.402, 1.f));

	GI->StopSound(SD_MAP);
}

HRESULT CLevel_Stage5::Ready_Lights()
{
	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(-1.f, -1.f, -1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.7f, 0.7f, 0.7f, 1.f);
	LightDesc.vSpecular = _float4(0.7f, 0.7f, 0.7f, 1.f);

	if (FAILED(GI->Add_Light(LightDesc)))
		return E_FAIL;



	_float4x4 ShadowMatrix;
	_vector		vLightEye = XMVectorSet(62.416f, 145.776f, -21.242f, 1.f);
	_vector		vLightAt = XMVectorSet(60.451f, 138.411f, -16.838f, 1.f);
	_vector		vLightUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMStoreFloat4x4(&ShadowMatrix, XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp));

	GI->Add_ShadowLightViewMatrix(LEVEL_STAGE5, ShadowMatrix);


	return S_OK;
}

HRESULT CLevel_Stage5::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage5"), LEVEL_STAGE5, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage5Light"), LEVEL_STAGE5, pLayerTag)))
		return E_FAIL;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage5Event1"), LEVEL_STAGE5, pLayerTag)))
		return E_FAIL;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("LastBlock"), LEVEL_STAGE5, pLayerTag)))
		return E_FAIL;


	GI->Add_GameObjectToLayer(L"BeastMap", TM->Get_CurLevel(), L"Layer_Stage5");
	GI->Add_GameObjectToLayer(L"BeastMap2", TM->Get_CurLevel(), L"Layer_Stage5");
	GI->Add_GameObjectToLayer(L"BeastMap3", TM->Get_CurLevel(), L"Layer_Stage5");


	return S_OK;
}

HRESULT CLevel_Stage5::Ready_Layer_Monster(const _tchar * pLayerTag)
{
	CCreture::CREATEINFO _tInfo;
	ZeroMemory(&_tInfo, sizeof(CCreture::CREATEINFO));

	_tInfo.iNaviNum = 5;
	_tInfo.vPosition = { -0.307f, 0.665f, -17.077f, 1.f };
	_tInfo.vDir = { 0.f, 0.f, 1.f };
	_tInfo.iIndex = 0;


	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Beast"), LEVEL_STAGE5, pLayerTag, &_tInfo)))
		return E_FAIL;

	_tInfo.iNaviNum = 5;
	_tInfo.vPosition = { -0.307f, 0.665f, -17.077f, 1.f };
	_tInfo.iIndex = 1;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("BeastPillar"), LEVEL_STAGE5, pLayerTag, &_tInfo)))
		return E_FAIL;

	CUI::UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));

	UiInfoDesc.fSizeX = 560.f;
	UiInfoDesc.fSizeY = 45.f;
	UiInfoDesc.fX = 680.f;
	UiInfoDesc.fY = 660.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_MarvelTimeBackground"), LEVEL_STAGE5, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 540.f;
	UiInfoDesc.fSizeY = 35.f;
	UiInfoDesc.fX = 680.f;
	UiInfoDesc.fY = 660.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_MarvelTime"), LEVEL_STAGE5, pLayerTag, &UiInfoDesc)))
		return E_FAIL;



	return S_OK;
}

CLevel_Stage5 * CLevel_Stage5::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Stage5*		pInstance = new CLevel_Stage5(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_Stage5"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Stage5::Free()
{
	__super::Free();

}


