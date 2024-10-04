#include "stdafx.h"
#include "..\Public\MainApp.h"

#include "GameInstance.h"
#include "ToolManager.h"
#include "DataManager.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "ImGui_Manager.h"
#include "Level_Loading.h"
#include "Camera_Manager.h"
#include "LoadingBar.h"
#include "LoadingText.h"
#include "LoadingPage.h"
#include "Particle_Manager.h"
#include "ParticleInstance.h"
#include "Status_Manager.h"
#include "Monster_Manager.h"
#include "Time_Manager.h"
#include "Route_Manager.h"
#include "Effect_Manager.h"
#include "Crack_Manager.h"
#include "CutSceneManager.h"

using namespace Client;

CMainApp::CMainApp()
	: m_pGameInstance(CGameInstance::Get_Instance())
{



	Safe_AddRef(m_pGameInstance);
}

HRESULT CMainApp::Initialize()
{


	GRAPHICDESC			GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof(GRAPHICDESC));

	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.eWinMode = GRAPHICDESC::MODE_WIN;
	GraphicDesc.iWinSizeX = g_iWinSizeX;
	GraphicDesc.iWinSizeY = g_iWinSizeY;

	if (FAILED(m_pGameInstance->Initialize_Engine(LEVEL_END, g_hInst, GraphicDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Fonts(m_pDevice, m_pContext, TEXT("Font_Nexon"), TEXT("../Bin/Resources/Fonts/129ex.spritefont"))))
		return E_FAIL;


	/* For.Prototype_Component_Renderer */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_UI */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_UI.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Transform */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_VIBuffer_ZRect */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_ZRect"),
		CVIBuffer_ZRect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_LOADING, TEXT("Prototype_Component_Texture_LoadingBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Loading/Loading%d.png"), 17))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_LOADING, TEXT("Prototype_Component_Texture_LoadingText"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Loading/LoadingText%d.png"), 4))))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LoadingPage"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Loading/LoadingPage%d.png"), 6))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_LoadingBar"),
		CLoadingBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_LoadingText"),
		CLoadingText::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_LoadingPage"),
		CLoadingPage::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(CToolManager::Get_Instance()->Init(m_pDevice, m_pContext)))
		return E_FAIL;
	if (FAILED(CDataManager::Get_Instance()->Init(m_pDevice, m_pContext)))
		return E_FAIL;
	if (FAILED(CImGui_Manager::Get_Instance()->Init(m_pDevice, m_pContext)))
		return E_FAIL;
	CImGui_Manager::Get_Instance()->Add_Renderer(m_pRenderer);


	m_pGameInstance->Reserve_Level((_uint)LEVEL_LOAD);
	
	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	TM->Set_TimeDelta(fTimeDelta);
	Get_KeyInput();

	m_pGameInstance->Tick_Engine(fTimeDelta);

	TIM->Tick_TimeManager(fTimeDelta);

#ifdef _DEBUG
	CImGui_Manager::Get_Instance()->Tick(fTimeDelta);
#endif
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	m_pGameInstance->Clear_BackBuffer_View(IG->Get_BackBufferColor());
	m_pGameInstance->Clear_DepthStencil_View();
	
	m_pRenderer->Draw();
	m_pGameInstance->Render_Level();

#ifdef _DEBUG
	CImGui_Manager::Get_Instance()->Render();
#endif

	m_pGameInstance->Present();
	Open_Level();

	return S_OK;
}

void CMainApp::Get_KeyInput()
{

	if (K_DOWN(DIK_F7))
	{
		UM->Set_FrontUI(false);
	}

	if (K_DOWN(DIK_F6))
	{
		IG->Set_ClipWindow();
		IG->Set_ShowCursor();
		ShowCursor(IG->Get_ShowCursor());
	}

	if (IG->Get_ClipWindow())
	{
		GetClientRect(g_hWnd, &m_Clip);
		m_p1.x = m_Clip.left + (LONG)300.f;
		m_p1.y = m_Clip.top + (LONG)300.f;
		m_p2.x = m_Clip.right - (LONG)300.f;
		m_p2.y = m_Clip.bottom - (LONG)300.f;
		ClientToScreen(g_hWnd, &m_p1);
		ClientToScreen(g_hWnd, &m_p2);
		m_Clip.left = m_p1.x;
		m_Clip.top = m_p1.y;
		m_Clip.right = m_p2.x;
		m_Clip.bottom = m_p2.y;
		ClipCursor(&m_Clip);
	}
	else
	{
		ClipCursor(nullptr);
	}


	//키인풋 모드전환
	if (K_DOWN(DIK_F2))
	{
		if (GI->Get_DebugInput() == 0)
			GI->Set_DebugInput(1);
		else
			GI->Set_DebugInput(0);
	}

	//카메라 모드전환
	if (K_DOWN(DIK_F3))
		CRM->Set_CamMode();

	//카메라 잠금
	if (K_DOWN(DIK_F4))
		TM->Set_CameraMove();

	
	if (CK_DOWN(DIK_8))
		PM->Change_Player(CPlayer::PLAYER_01);

	if (CK_DOWN(DIK_9))
		PM->Change_Player(CPlayer::PLAYER_03);

	if (CK_DOWN(DIK_0))
		PM->Change_Player(CPlayer::PLAYER_04);
	
}

HRESULT CMainApp::Open_Level()
{

	LEVEL eLevel = (LEVEL)GI->Get_Reserve_LevelIndex();

	if (LEVEL_END == eLevel)
		return S_OK;

	switch (eLevel)
	{
	case Client::LEVEL_LOAD:
		GI->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_LOAD));
		break;
	case Client::LEVEL_LOGO:
		GI->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_LOGO));
		break;
	case Client::LEVEL_TEST:
		GI->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_TEST));
		break;
	case Client::LEVEL_TOOL:
		GI->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_TOOL));
		break;
	case Client::LEVEL_STAGE1:
		GI->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE1));
		break;
	case Client::LEVEL_STAGE2:
		GI->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE2));
		break;
	case Client::LEVEL_STAGE3:
		GI->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE3));
		break;
	case Client::LEVEL_STAGE4:
		GI->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE4));
		break;
	case Client::LEVEL_STAGE5:
		GI->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_STAGE5));
		break;
	case Client::LEVEL_END:
		break;
	default:
		break;
	}

	GI->Reserve_Level((_uint)LEVEL_END);
	return S_OK;
}




CMainApp * CMainApp::Create()
{	
	CMainApp*		pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CMainApp"));
		Safe_Release(pInstance);
	}
	
	return pInstance;
}


// *[HeroJho]: 어떤 그룹을 충돌할지 결정합니다.
void CMainApp::Check_Collider()
{
	// 플레이어와 아이템 그룹을 충돌처리 하겠다.
	m_pGameInstance->Calcul_ColGroup(CColliderManager::COLLIDER_PLAYER, CColliderManager::COLLIDER_ITEM);
	// 플레이어와 몬스터 그룹을 충돌처리 하겠다.
	m_pGameInstance->Calcul_ColGroup(CColliderManager::COLLIDER_PLAYER, CColliderManager::COLLIDER_MONSTER);
	//몬스터그룹끼리 충돌처리 하겠다.
	m_pGameInstance->Calcul_ColGroup(CColliderManager::COLLIDER_MONSTER, CColliderManager::COLLIDER_MONSTER);

	// 마지막에 호출 해줘야하는 함수!
	m_pGameInstance->Clear_ColGroup();
}




void CMainApp::Free()
{

	CCutSceneManager::Get_Instance()->Destroy_Instance();
	CDataManager::Get_Instance()->Destroy_Instance();
	CToolManager::Get_Instance()->Destroy_Instance();
	CPlayer_Manager::Get_Instance()->Destroy_Instance();
	CUI_Manager::Get_Instance()->Destroy_Instance();
	CCamera_Manager::Get_Instance()->Destroy_Instance();
	CParticle_Manager::Get_Instance()->Destroy_Instance();
	CStatus_Manager::Get_Instance()->Destroy_Instance();
	CMonster_Manager::Get_Instance()->Destroy_Instance();
	CTime_Manager::Get_Instance()->Destroy_Instance();
	CRoute_Manager::Get_Instance()->Destroy_Instance();
	CEffect_Manager::Get_Instance()->Destroy_Instance();
	CCrack_Manager::Get_Instance()->Destroy_Instance();
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pGameInstance);
	CImGui_Manager::Destroy_Instance();
	CGameInstance::Release_Engine();

}
