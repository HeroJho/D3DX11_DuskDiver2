#include "stdafx.h"
#include "..\Public\Level_Stage4.h"

#include "GameInstance.h"
#include "ToolManager.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "UI.h"
#include "Creture.h"
#include "Level_Loading.h"
#include "DataManager.h"

#include "Stage4Star.h"
#include "CrystalFake.h"
#include "CrystalBody.h"
#include "DropItem.h"
#include "MiniGame_Yabawe.h"
#include "MiniGame_Block.h"
#include "Monster_Manager.h"
#include "Camera_Manager.h"
#include "Crack_Manager.h"

CLevel_Stage4::CLevel_Stage4(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Stage4::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;


	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;
	if (FAILED(Ready_Layer_ItemObj(TEXT("Layer_ItemObj"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_ArcadeObject(TEXT("Layer_Arcade"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	Level_Start();

	// GI->PlaySound_Linear(L"Arcade.wav", SD_BGM, 0.3f, true, 0.4f);
	//GI->PlayBGM(L"Arcade.wav", 0.2f);

	return S_OK;
}

void CLevel_Stage4::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);


	CRM->Tick(fTimeDelta);


	

	if (CK_DOWN(DIK_NUMPADENTER))
	{
		//TM->Deal_CumaHp(41500.f);
		TM->Deal_CumaHp(500.f);
		UM->Set_QuestPointOff();
	}



}

HRESULT CLevel_Stage4::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Stage4_Level"));

	return S_OK;
}

HRESULT CLevel_Stage4::Ready_Layer_BackGround(const _tchar * pLayerTag)
{
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage4"), LEVEL_STAGE4, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage4Roof"), LEVEL_STAGE4, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage4Face"), LEVEL_STAGE4, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage4Glass"), LEVEL_STAGE4, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage4Light"), LEVEL_STAGE4, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage4Escalator"), LEVEL_STAGE4, pLayerTag)))
		return E_FAIL;

	
	CStage4Star::STARDESC StarDesc;
	for (_uint i = 0; 7 > i; ++i)
	{
		StarDesc.iStarIndex = i;
		if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage4Star"), LEVEL_STAGE4, pLayerTag, &StarDesc)))
			return E_FAIL;
	}


	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage4Event1"), LEVEL_STAGE4, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage4Event2"), LEVEL_STAGE4, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage4Event3"), LEVEL_STAGE4, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage4Event4"), LEVEL_STAGE4, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage4Event5"), LEVEL_STAGE4, pLayerTag)))
		return E_FAIL;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("BallDownTrigger"), LEVEL_STAGE4, pLayerTag)))
		return E_FAIL;
	
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("MiniGame_Block_Triger"), LEVEL_STAGE4, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("MiniGame_Funch_Trigger"), LEVEL_STAGE4, pLayerTag)))
		return E_FAIL;
	
	
	TM->Create_MiniGame_Machine();
	
	return S_OK;
}

HRESULT CLevel_Stage4::Ready_Layer_ItemObj(const _tchar * pLayerTag)
{
#pragma region 아이템드랍박스
	CCrystalBody::BODYDESC BodyDesc;
	//좌측
	BodyDesc.vPosition = { 9.4f,7.8f,61.9f,1.f };
	BodyDesc.vRadianAngle = 1.f;
	BodyDesc.bClue = false;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	BodyDesc.vPosition = { -5.4f,7.8f,90.6f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	BodyDesc.vPosition = { -41.6f,15.6f,109.f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;

	BodyDesc.vPosition = { -72.3f,11.7f,90.f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;

	/*BodyDesc.vPosition = { 10.58f,7.82f,63.87f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;*/

	BodyDesc.vPosition = { -58.3f,11.74f,73.83f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	/*BodyDesc.vPosition = { -68.12f,11.74f,79.47f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;*/

	/*BodyDesc.vPosition = { -89.44f,7.84f,56.38f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;*/

	BodyDesc.vPosition = { -121.52f,0.f,70.88f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	/*BodyDesc.vPosition = { -122.47f,0.f,69.1f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;*/
	BodyDesc.vPosition = { -120.45f,0.f,105.77f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	BodyDesc.vPosition = { -122.03f,0.f,106.56f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	/*BodyDesc.vPosition = { -130.08f,0.f,64.f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;*/

	/*BodyDesc.vPosition = { -70.64f,27.32f,167.38f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	BodyDesc.vPosition = { -70.64f,27.32f,170.38f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	BodyDesc.vPosition = { -76.56f,27.32f,194.04f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;*/
	BodyDesc.vPosition = { -102.19f,27.32f,185.5f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;

	BodyDesc.vPosition = { -145.99f,31.28f,113.45f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	/*BodyDesc.vPosition = { -147.89f,31.28f,111.43f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;*/
	/*BodyDesc.vPosition = { -155.22f,31.28f,98.34f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;*/

	// 우측
	/*BodyDesc.vPosition = { 44.42f,0.f,96.38f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;*/
	BodyDesc.vPosition = { 45.58f,0.f,97.04f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	BodyDesc.vPosition = { 48.64f,0.f,108.57f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;

	BodyDesc.vPosition = { 97.36f,-7.83f,113.52f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	BodyDesc.vPosition = { 105.96f,-7.83f,128.f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	//BodyDesc.vPosition = { 95.89f,-7.83f,142.94f,1.f };
	//if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
	//	return E_FAIL;

	BodyDesc.vPosition = { 130.78f,-3.92f,129.f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	/*BodyDesc.vPosition = { 128.75f,-3.92f,131.36f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	BodyDesc.vPosition = { 124.64f,-3.92f,152.33f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;*/

	BodyDesc.vPosition = { 110.44f,3.9f,182.01f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	/*BodyDesc.vPosition = { 111.41f,3.9f,183.47f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	BodyDesc.vPosition = { 113.08f,3.9f,183.37f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	BodyDesc.vPosition = { 116.28f,3.9f,189.6f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;*/
	BodyDesc.vPosition = { 112.91f,3.9f,196.29f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	BodyDesc.vPosition = { 106.29f,3.9f,207.76f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	/*BodyDesc.vPosition = { 77.31f,3.9f,194.45f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalBody"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;*/
#pragma endregion 아이템드랍박스
	
#pragma region 가짜박스
	CCrystalFake::FAKEDESC FakeDesc;
	//우측
	BodyDesc.vPosition = { -6.87f,7.82f,62.48f,1.f };
	//BodyDesc.vRadianAngle = 1.f;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalFake"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	/*BodyDesc.vPosition = { 94.98f,-7.82f,113.74f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalFake"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;*/
	BodyDesc.vPosition = { 98.59f,-7.82f,142.7f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalFake"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	BodyDesc.vPosition = { 127.71f,-23.41f,188.89f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalFake"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	/*BodyDesc.vPosition = { 89.15f,3.9f,173.81f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalFake"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;*/
	BodyDesc.vPosition = { 95.53f,-15.64f,174.2f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalFake"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	BodyDesc.vPosition = { 154.84f,-23.4f,169.88f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalFake"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	BodyDesc.vPosition = { 114.35f,3.9f,193.99f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalFake"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;

	//좌측
	BodyDesc.vPosition = { -39.6f,15.64f,107.66f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalFake"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	/*BodyDesc.vPosition = { -49.86f,15.64f,101.3f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalFake"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	BodyDesc.vPosition = { -66.19f,11.74f,78.3f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalFake"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;*/
	BodyDesc.vPosition = { -74.4f,11.74f,89.28f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalFake"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;

	/*BodyDesc.vPosition = { -91.49f,7.84f,57.36f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalFake"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;*/
	BodyDesc.vPosition = { -112.39f,0.f,96.4f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalFake"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	BodyDesc.vPosition = { -137.21f,0.f,87.41f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalFake"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	BodyDesc.vPosition = { -80.28f,23.46f,113.56f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalFake"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	/*BodyDesc.vPosition = { -82.16f,23.46f,113.56f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalFake"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;
	BodyDesc.vPosition = { -92.57f,23.46f,143.22f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CrystalFake"), LEVEL_STAGE4, pLayerTag, &BodyDesc)))
		return E_FAIL;*/
	
#pragma endregion 가짜박스

	return S_OK;
}


HRESULT CLevel_Stage4::Ready_Layer_ArcadeObject(const _tchar * pLayerTag)
{
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("YMTest"), LEVEL_STAGE4, pLayerTag)))
		return E_FAIL; // 하키

	XMVECTOR Pos = { 1.344f, 35.16f + 21.f, 137.75f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CraneScene0"), LEVEL_STAGE4, pLayerTag, &Pos)))
		return E_FAIL;

	Pos = { 1.344f, 35.16f + 11.58f, 137.75f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CraneScene1"), LEVEL_STAGE4, pLayerTag, &Pos)))
		return E_FAIL;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CraneScene2"), LEVEL_STAGE4, pLayerTag, &Pos)))
		return E_FAIL;

	Pos = { 1.344f, 35.16f + 11.58f, 137.75f, 0.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CraneScene2"), LEVEL_STAGE4, pLayerTag, &Pos)))
		return E_FAIL;

	Pos = { 1.344f,35.16f + 6.5f, 137.75f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CapsuleScene"), LEVEL_STAGE4, pLayerTag, &Pos)))
		return E_FAIL;


	Pos = { 1.6f, 35.16f + 21.f, 190.f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CraneScene0"), LEVEL_STAGE4, pLayerTag, &Pos)))
		return E_FAIL;

	Pos = { 1.6f, 35.16f + 11.58f, 190.f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CraneScene1"), LEVEL_STAGE4, pLayerTag, &Pos)))
		return E_FAIL;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CraneScene2"), LEVEL_STAGE4, pLayerTag, &Pos)))
		return E_FAIL;

	Pos = { 1.6f, 35.16f + 11.58f, 190.f, 0.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CraneScene2"), LEVEL_STAGE4, pLayerTag, &Pos)))
		return E_FAIL;

	Pos = { 1.6f,35.16f + 6.5f, 190.f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("CapsuleScene"), LEVEL_STAGE4, pLayerTag, &Pos)))
		return E_FAIL;

	Pos = { 1.344f, 35.16f + 21.f, 137.75f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Crane0"), LEVEL_STAGE4, pLayerTag, &Pos)))
		return E_FAIL;

	Pos = { 1.344f, 35.16f + 11.58f, 137.75f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Crane1"), LEVEL_STAGE4, pLayerTag, &Pos)))
		return E_FAIL;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Crane2"), LEVEL_STAGE4, pLayerTag, &Pos)))
		return E_FAIL;

	Pos = { 1.344f, 35.16f + 11.58f, 137.75f, 0.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Crane2"), LEVEL_STAGE4, pLayerTag, &Pos)))
		return E_FAIL;

	Pos = { 1.344f,19.f, 137.75f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Capsule"), LEVEL_STAGE4, pLayerTag, &Pos)))
		return E_FAIL;



	Pos = { 1.6f, 35.16f + 21.f, 190.f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Crane0"), LEVEL_STAGE4, pLayerTag, &Pos)))
		return E_FAIL;

	Pos = { 1.6f, 35.16f + 11.58f, 190.f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Crane1"), LEVEL_STAGE4, pLayerTag, &Pos)))
		return E_FAIL;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Crane2"), LEVEL_STAGE4, pLayerTag, &Pos)))
		return E_FAIL;

	Pos = { 1.6f, 35.16f + 11.58f, 190.f, 0.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Crane2"), LEVEL_STAGE4, pLayerTag, &Pos)))
		return E_FAIL;

	Pos = { 1.6f, 19.f, 190.f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Capsule"), LEVEL_STAGE4, pLayerTag, &Pos)))
		return E_FAIL;


	CCreture::CREATEINFO _tInfo;
	ZeroMemory(&_tInfo, sizeof(CCreture::CREATEINFO));


	_tInfo.vPosition = { -85.827f,27.219f,179.580f,1.f };
	_tInfo.iIndex = 0;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Tower"), LEVEL_STAGE4, pLayerTag, &_tInfo)))
		return E_FAIL;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("TowerRail"), LEVEL_STAGE4, pLayerTag, &_tInfo)))
		return E_FAIL;

	_tInfo.vPosition = { 96.7f, 3.9f,192.515f,1.f };
	_tInfo.iIndex = 1;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Tower"), LEVEL_STAGE4, pLayerTag, &_tInfo)))
		return E_FAIL;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("TowerRail"), LEVEL_STAGE4, pLayerTag, &_tInfo)))
		return E_FAIL;

	_tInfo.vPosition = { -125.430f, 0.f,83.869f,1.f };
	_tInfo.iIndex = 2;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Tower"), LEVEL_STAGE4, pLayerTag, &_tInfo)))
		return E_FAIL;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("TowerRail"), LEVEL_STAGE4, pLayerTag, &_tInfo)))
		return E_FAIL;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage4Machine0"), LEVEL_STAGE4, pLayerTag)))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Stage4Coin"), LEVEL_STAGE4, pLayerTag)))
		return E_FAIL;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_PunchText"), LEVEL_STAGE4, TEXT("Layer_UI"))))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_PunchScore"), LEVEL_STAGE4, TEXT("Layer_UI"))))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_PunchScore2"), LEVEL_STAGE4, TEXT("Layer_UI"))))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_PunchScore3"), LEVEL_STAGE4, TEXT("Layer_UI"))))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_PunchScore4"), LEVEL_STAGE4, TEXT("Layer_UI"))))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_PunchTime"), LEVEL_STAGE4, TEXT("Layer_UI"))))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_PunchTime2"), LEVEL_STAGE4, TEXT("Layer_UI"))))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_BlockScore"), LEVEL_STAGE4, TEXT("Layer_UI"))))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_BlockM"), LEVEL_STAGE4, TEXT("Layer_UI"))))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_BlockMax"), LEVEL_STAGE4, TEXT("Layer_UI"))))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_BlockScore2"), LEVEL_STAGE4, TEXT("Layer_UI"))))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_PunchScoreText"), LEVEL_STAGE4, TEXT("Layer_UI"))))
		return E_FAIL;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_KumaImage"), LEVEL_STAGE4, TEXT("Layer_UI"))))
		return E_FAIL;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_OpenOX"), LEVEL_STAGE4, TEXT("Layer_UI"))))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_OpenOX2"), LEVEL_STAGE4, TEXT("Layer_UI"))))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_OpenOX3"), LEVEL_STAGE4, TEXT("Layer_UI"))))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_OpenOX4"), LEVEL_STAGE4, TEXT("Layer_UI"))))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_OpenOX5"), LEVEL_STAGE4, TEXT("Layer_UI"))))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_OpenOX6"), LEVEL_STAGE4, TEXT("Layer_UI"))))
		return E_FAIL;
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_OpenOX7"), LEVEL_STAGE4, TEXT("Layer_UI"))))
		return E_FAIL;


	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DGTriggr"), LEVEL_STAGE4, TEXT("Layer_Triger"))))
		return E_FAIL;


	TM->Make_Potal();
	TM->Make_EPotal();
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("SentorPotal"), LEVEL_STAGE4, TEXT("Layer_Potal"))))
		return E_FAIL;



	return S_OK;
}

HRESULT CLevel_Stage4::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CUI::UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));

	UiInfoDesc.fSizeX = 322.f * 0.65f;
	UiInfoDesc.fSizeY = 322.f * 0.65f;
	UiInfoDesc.fX = 1160.f;
	UiInfoDesc.fY = 205.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_MiniMapBase"), LEVEL_STAGE3, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 688.f;
	UiInfoDesc.fSizeY = 565.f;
	UiInfoDesc.fX = 640.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_MiniMinimap"), LEVEL_STAGE3, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 192.f * 0.65f;
	UiInfoDesc.fSizeY = 66.f * 0.65f;
	UiInfoDesc.fX = 1155.f;
	UiInfoDesc.fY = 139.6f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_MiniMapUpside"), LEVEL_STAGE3, pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	
	UiInfoDesc.fSizeX = 489.f * 0.2f;
	UiInfoDesc.fSizeY = 581.f * 0.2f;
	UiInfoDesc.fX = 950.f;
	UiInfoDesc.fY = 600.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_BearType"), LEVEL_STATIC, pLayerTag, &UiInfoDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CLevel_Stage4::Ready_Lights()
{
	LIGHTDESC			LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eType = LIGHTDESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(-1.f, -1.f, -1.f, 0.f);
	LightDesc.vDiffuse = _float4(0.6f, 0.6f, 0.6f, 1.f);
	LightDesc.vAmbient = _float4(0.1f, 0.1f, 0.1f, 1.f);
	LightDesc.vSpecular = _float4(0.5f, 0.5f, 0.5f, 1.f);

	if (FAILED(GI->Add_Light(LightDesc)))
		return E_FAIL;



	_float4x4 ShadowMatrix;
	_vector		vLightEye = XMVectorSet(62.416f, 145.776f, -21.242f, 1.f);
	_vector		vLightAt = XMVectorSet(60.451f, 138.411f, -16.838f, 1.f);
	_vector		vLightUp = XMVectorSet(0.f, 1.f, 0.f, 0.f);
	XMStoreFloat4x4(&ShadowMatrix, XMMatrixLookAtLH(vLightEye, vLightAt, vLightUp));

	GI->Add_ShadowLightViewMatrix(LEVEL_STAGE4, ShadowMatrix);


	return S_OK;
}

void CLevel_Stage4::Level_Start()
{
	DM->Load_LightData(4);
	DM->Load_PhysObjData(4);
	UM->Set_QuestPointOff();
	PM->Change_Navigation(4);
	UM->Set_FrontUI(true);
	UM->Set_QuestNum(14);

	TM->Start_MiniGame_Yabawe();
	TM->Make_Stage4Triggers();

	CRM->Get_Cam()->Set_CamPos(0.f, 20.f);
	PM->Set_SceneIdle(PM->Get_SelectPlayer());
	PM->Set_SceneLook(PM->Get_SelectPlayer(), _float4(1.505f, -6.504f, -18.348f, 1.f));

	GI->StopSound(SD_NPC);
	GI->StopSound(SD_MAP);
}

CLevel_Stage4 * CLevel_Stage4::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Stage4*		pInstance = new CLevel_Stage4(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_Stage4"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Stage4::Free()
{
	__super::Free();

}


