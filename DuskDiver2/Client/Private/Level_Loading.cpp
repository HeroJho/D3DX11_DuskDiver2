#include "../Default/stdafx.h"
#include "..\Public\Level_Loading.h"
#include "GameInstance.h"
#include "UI.h"
#include "UI_Manager.h"

#include "Level_Static.h"
#include "Level_Logo.h"
#include "Level_Test.h"
#include "Level_Tool.h"
#include "Level_Stage1.h"
#include "Level_Stage2.h"
#include "Level_Stage3.h"
#include "Level_Stage4.h"
#include "Level_Stage5.h"

#include "Loader.h"


CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevel)
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	m_eNextLevel = eNextLevel;

	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevel);
	if (nullptr == m_pLoader)
		return E_FAIL;


	CUI::UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));



	
	UiInfoDesc.fSizeX = 1280.f;
	UiInfoDesc.fSizeY = 720.f;
	UiInfoDesc.fX = 640.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_LoadingPage"), LEVEL_LOADING, TEXT("Layer_UI"), &UiInfoDesc)))
		return E_FAIL;

	
	UiInfoDesc.fSizeX = 128.f * 0.6f;
	UiInfoDesc.fSizeY = 128.f * 0.6f;
	UiInfoDesc.fX = 1010.f;
	UiInfoDesc.fY = 680.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_LoadingBar"), LEVEL_LOADING, TEXT("Layer_UI"), &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 256.f * 0.8f;
	UiInfoDesc.fSizeY = 128.f * 0.8f;
	UiInfoDesc.fX = 1160.f;
	UiInfoDesc.fY = 670.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_LoadingText"), LEVEL_LOADING, TEXT("Layer_UI"), &UiInfoDesc)))
		return E_FAIL;
	

	UM->Set_StageNum(5);

	return S_OK;
}

void CLevel_Loading::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	// m_fDelay += fTimeDelta;

	if (true == m_pLoader->Get_Finished())
	{
		

		//_tchar			m_szFPS[MAX_PATH] = TEXT("");
		//wstring s = L"로딩 시간: ";
		//s += to_wstring(m_fDelay);
		//s += L" 초";
		//MessageBox(g_hWnd, s.data(), _T("로딩 완료"), MB_OK);


			CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
			Safe_AddRef(pGameInstance);

			CLevel*		pNewLevel = nullptr;

			/*if (m_fDelay > 4.f)
			{*/
				switch (m_eNextLevel)
				{
				case LEVEL_LOAD:
					pNewLevel = CLevel_Static::Create(m_pDevice, m_pContext);
					break;
				case LEVEL_LOGO:
					pNewLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
					break;
				case LEVEL_TEST:
					pNewLevel = CLevel_Test::Create(m_pDevice, m_pContext);
					break;
				case LEVEL_TOOL:
					pNewLevel = CLevel_Tool::Create(m_pDevice, m_pContext);
					break;
				case LEVEL_STAGE1:
					pNewLevel = CLevel_Stage1::Create(m_pDevice, m_pContext);
					break;
				case LEVEL_STAGE2:
					pNewLevel = CLevel_Stage2::Create(m_pDevice, m_pContext);
					break;
				case LEVEL_STAGE3:
					pNewLevel = CLevel_Stage3::Create(m_pDevice, m_pContext);
					break;
				case LEVEL_STAGE4:
					pNewLevel = CLevel_Stage4::Create(m_pDevice, m_pContext);
					break;
				case LEVEL_STAGE5:
					pNewLevel = CLevel_Stage5::Create(m_pDevice, m_pContext);
					break;

					//case LEVEL_GAMEPLAY:
					//	pNewLevel = CLevel_GamePlay::Create(m_pDevice, m_pContext);
					//	break;
				}
			//}

			if (nullptr == pNewLevel)
				goto except;

			if (FAILED(pGameInstance->Open_Level(m_eNextLevel, pNewLevel)))
				goto except;


			except:
			Safe_Release(pGameInstance);
		
	}
}

HRESULT CLevel_Loading::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, m_pLoader->Get_LoadingText());

	return S_OK;
}

CLevel_Loading * CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLevel_Loading*		pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed To Created : CMainApp"));
		Safe_Release(pInstance);
	}

	return pInstance;

}


void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);

}


