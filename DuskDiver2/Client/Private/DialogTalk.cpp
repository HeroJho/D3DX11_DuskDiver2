#include "stdafx.h"
#include "..\Public\DialogTalk.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "ToolManager.h"
#include "Monster_Manager.h"
#include "Player_Manager.h"
#include "Crack_Manager.h"
#include "Camera_Manager.h"

#include "Camera_Manager.h"
#include "Camera_Main.h"
#include "FunchCuma.h"

#include "Camera_Manager.h"
#include "Camera_Main.h"
#include "FunchCuma.h"

#include "QPotal.h"

CDialogTalk::CDialogTalk(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI(pDevice, pContext)
{
}

CDialogTalk::CDialogTalk(const CDialogTalk & rhs)
	:CUI(rhs)
{
}

HRESULT CDialogTalk::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDialogTalk::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	UM->BackUp_DialogTalk(this);

	return S_OK;
}

void CDialogTalk::Tick(_float fTimeDelta)
{
	//UI_InputDebug(fTimeDelta);

	if (true == m_bStartRender)
	{
		Check_RenderTime(fTimeDelta);
	}

	if (true == m_bRender)
	{
		Dialog_Move(fTimeDelta);
		Update_Alpha(fTimeDelta);
	}

	// 다음 대화 또는 이벤트로 넘기는 기능
	if ((true == m_bRender) && (CK_UP(DIK_RETURN)))
	{
		// 이벤트 발생이 없다면 대화를 이어나간다.
		if (false == Check_EventTalk())
		{
			++m_iTalkIndex;
			GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
		}
	
	}
	if (true == Check_TalkChange())
	{
		m_UiInfo.fX = 640.f - 20.f;
		m_iLastTalkIndex = m_iTalkIndex;
		m_fRenderAlpha = 0.f;
	}
	// 다음번 Render가 true일때 반짝임을 없애준다 미리 alpha를 0으로 해서
	if (false == m_bRender)
		m_fRenderAlpha = 0.f;
	
	__super::Tick(fTimeDelta);
}

void CDialogTalk::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CDialogTalk::Render()
{
	if (true == m_bRender)
	{
		if (nullptr == m_pVIBufferCom ||
			nullptr == m_pShaderCom)
			return E_FAIL;


		m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
		m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
		m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

		m_pShaderCom->Set_RawValue("g_DialogAlpha", &m_fRenderAlpha, sizeof(_float));

		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", m_iTalkIndex)))
			return E_FAIL;


		if (FAILED(m_pShaderCom->Begin(24)))
			return E_FAIL;


		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}
	//UI_RenderDebug();

	return S_OK;
}

void CDialogTalk::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	ImGui::End();
}

const _bool CDialogTalk::Get_TalkOnOff()
{
	if ((true == m_bRender) || (true == m_bStartRender))
	{
		return true;
	}

	return false;
}

void CDialogTalk::Set_TalkIndex(_uint Index)
{
	m_bStartRender = true;
	m_iTalkIndex = Index;
}

HRESULT CDialogTalk::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DialogTalk"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

_bool CDialogTalk::Check_TalkChange()
{
	if (m_iLastTalkIndex != m_iTalkIndex)
	{
		m_bStartRender = true;
		m_bRender = false;
		return true;
	}

	return false;
}

void CDialogTalk::Dialog_Move(_float fTimeDelta)
{
	if (640.f > m_UiInfo.fX)
		m_UiInfo.fX += 100.f * fTimeDelta;
	else
		m_UiInfo.fX = 640.f;
}

void CDialogTalk::Update_Alpha(_float fTimeDelta)
{
	if (1.f > m_fRenderAlpha)
		m_fRenderAlpha += 5.f * fTimeDelta;
	else
		m_fRenderAlpha = 1.f;
}

void CDialogTalk::Check_RenderTime(_float fTimeDelta)
{
	m_fRenderTime += 10.f * fTimeDelta;

	if (m_fMaxRenderTime < m_fRenderTime)
	{
		m_bRender = true;
		m_fRenderTime = 0.f;
		m_bStartRender = false;
	}

}

_bool CDialogTalk::Check_EventTalk()
{
	// 디버깅 코드 -> 나중에 지워라
	//return false;

#pragma region Stage1
	// Stage1 유모 전투시작
	if ((3 == m_iTalkIndex))
	{
		CRM->Get_Cam()->End_Target();
		m_bRender = false;
		UM->Set_QuestOn(true);
		UM->Set_QuestPoint({ 0.f, 0.f, -10.f }, 0);
		GI->Add_GameObjectToLayer(TEXT("Stage1Event1"), LEVEL_STATIC, L"Layer_Event");
		UM->Set_FrontUI(true);
		return true;
	}
	// Stage1 리베이다 전투 시작
	if ((6 == m_iTalkIndex))
	{
		m_bRender = false;
		UM->LEVIADA_On();
		UM->PartyOn(true);
		UM->Set_FrontUI(true);
		return true;
	}
	//사운드
	if (11 == m_iTalkIndex && !m_bSound)
	{
		//GI->PlaySound_Linear(L"SubWay.wav", SD_BGM, 0.3f, true, 0.2f);
		m_bSound = true;
	}
	// 레벨 전환
	if (13 == m_iTalkIndex)
	{
		CRM->Get_Cam()->End_Target();
		UM->Set_QuestOn(true);
		//UM->Set_QuestPoint({ 0.f, 0.f, -10.f }, 0);
		//GI->Add_GameObjectToLayer(TEXT("Stage1Event2"), LEVEL_STATIC, L"Layer_Event");
		m_bRender = false;
		UM->Set_FrontUI(true);
		return true;
	}
#pragma endregion Stage1
	
#pragma region Stage2
	// Stage2 도착후 대화 종료 16
	if ((16 == m_iTalkIndex))
	{
		m_bRender = false;
		UM->Set_QuestNum(2);
		UM->Set_QuestOn(true);
		UM->Set_FrontUI(true);
		return true;
	}

	// 리베이다 대화종료후 전투 시작
	if ((18 == m_iTalkIndex))
	{
		m_bRender = false;
		UM->Set_QuestOn(true);
		UM->Set_FrontUI(true);
		return true;
	}
	// 닌자 캐릭터 획득
	if (27 == m_iTalkIndex)
	{
		UM->Set_QuestPoint({ -0.5f, 2.f, -3.95f }, 0); // 출구 좌표
		m_bRender = false;
		UM->Set_FrontUI(true);
		return true;
	}
#pragma endregion Stage2

#pragma region Stage3
	// Stage3 시작대화 종료
	if ((29 == m_iTalkIndex))
	{
		m_bRender = false;
		UM->Set_QuestNum(5);
		UM->Set_QuestOn(true);
		UM->Set_FrontUI(true);
		return true;
	}
	// Stage3-1 전투 끝난 후 대화 종료
	if ((31 == m_iTalkIndex))
	{
		m_bRender = false;
		UM->Set_QuestOn(true);
		UM->Set_FrontUI(true);
		UM->Set_QuestPoint({ 8.5f, 2.f, 173.5f }, 0); // 아지트 좌표
		return true;
	}

	// 아지트 도착 후 정보요원과 대화 종료
	if ((34 == m_iTalkIndex))
	{
		m_bRender = false;
		UM->Set_QuestOn(true);
		UM->Set_FrontUI(true);
		UM->Set_QuestPoint({ 45.f, 2.f, 135.f }, 0); // 균열의 위치 -> 중중간 보스 생성

		CRKM->Create_Crack(_float4(45.f, 0.f, 135.f, 1.f), 4.f, 3, TM->Get_CurLevel());

		CRM->Get_Cam()->End_Target();
		return true;
	}
	// Stage3-2 전투 끝난 후 -> 중중간 보스 처치후 단서 획득 대화종료
	if (36 == m_iTalkIndex)
	{
		m_bRender = false;
		UM->Set_QuestOn(true);
		UM->Set_FrontUI(true);
		UM->Set_QuestPoint({ 8.5f, 2.f, 173.5f }, 0); // 아지트 좌표

		return true;
	}
	// 단서 찾기 수집 퀘스트 시작 
	if (41 == m_iTalkIndex)
	{
		m_bRender = false;
		UM->Set_QuestOn(true);
		UM->Set_FrontUI(true);
		UM->Set_CoinOn(true); // 수집 UI On

		CRM->Get_Cam()->End_Target();
		return true;
	}

	// 모든 단서 획득후 아지트로 돌아가자 대화 종료
	if (44 == m_iTalkIndex)
	{
		m_bRender = false;
		UM->Set_FrontUI(true);
		UM->Set_QuestOn(true);
		UM->Set_QuestPoint({ 8.5f, 2.f, 173.5f }, 0); // 아지트 좌표

		CRM->Get_Cam()->End_Target();
		return true;
	}

	// 아지트로 돌아온 후 정보요원과 대화 종료
	if (47 == m_iTalkIndex)
	{
		m_bRender = false;
		UM->Set_QuestOn(true);
		UM->Set_FrontUI(true);
		UM->Set_QuestPointOff();
		// 균열의 위치는 퀘스트 위치로 알려주지 않고 강아지로 찾는다.
		CRKM->Create_Crack(_float4(2.647f, 0.f, 147.f, 1.f), 4.f, 4, TM->Get_CurLevel()); // 균열의 위치를 세팅 

		CRM->Get_Cam()->End_Target();
		return true;
	}
	// 골렘 만나기 전의 균열 발견후 대화종료
	if (50 == m_iTalkIndex)
	{
		m_bRender = false;
		UM->Set_QuestOn(true);
		UM->Set_FrontUI(true);
		return true;
	}
	// 융합형 골렘을 처리한 후 대화 종료
	if ((52 == m_iTalkIndex))
	{
		m_bRender = false;
		UM->Set_FrontUI(true);
		return true;
	}
	// 융합형 골렘을 처리한 후 단서 얻은뒤 마지막으로 아지트 가자 대화 종료
	if ((54 == m_iTalkIndex))
	{
		m_bRender = false;
		UM->Set_QuestOn(true);
		UM->Set_FrontUI(true);
		UM->Set_QuestPoint({ 8.5f, 2.f, 173.5f }, 0); // 아지트 좌표
		return true;
	}
	// 아지트로 마지막으로 돌아온후 IT밸리로 가기전의 대화 종료
	if (62 == m_iTalkIndex)
	{
		m_bRender = false;
		UM->Set_QuestOn(true);
		UM->Set_FrontUI(true);
		UM->Set_QuestPoint({ 45.7f, 0.f, -17.52f }, 0); // IT밸리 좌표

		CRM->Get_Cam()->End_Target();
		return true;
	}
	// 중중간 보스 만난뒤 변신전 대화 종료
	if (102 == m_iTalkIndex)
	{
		m_bRender = false;
		UM->Set_FrontUI(true);

		// 아래 코드를 변신 UI 알려준뒤 
		
		/*MM->Control_Beta();
		MM->Control_Fusion();
		MM->Control_OldBombRobot();
		MM->Control_DisasterBoss();*/

		return true;
	}
#pragma endregion Stage3

#pragma region Stage4
	if ((65 == m_iTalkIndex))
	{
		CRM->Play_CutScene(9, false, false, nullptr);
		UM->Set_QuestNum(14);
		return false;
	}
	if ((66 == m_iTalkIndex))
	{
		CRM->End_CutScene();
		return false;
	}
	// Stage4 시작대화 종료
	if ((68 == m_iTalkIndex))
	{
		m_bRender = false;
		UM->Set_QuestOn(true);
		UM->Set_FrontUI(true);
		UM->Set_QuestPoint({ 1.91f, 16.7f, 132.87f }, 0); // 오락실 중간위치(유모 날아가는곳) 좌표
		return true;
	}

	// 유모 날아가는 컷신후 모노 쿠마의 위치를 전송받기전의 대화 종료
	if (74 == m_iTalkIndex)
	{
		m_bRender = false;
		UM->Set_FrontUI(true);
		
		UM->Set_BattleTalkIndex(5); // 전투 박스로 모노 쿠마의 위치를 얻는다.
		return true;
	}
	// 펀치 기계 컷신 끝난뒤 게임 시작전 대화종료
	if (77 == m_iTalkIndex)
	{
		m_bRender = false;
		TM->Get_FunchCuma()->Set_State(CFunchCuma::STATE_IDLE); // 펀치 게임 시작
		UM->Set_FrontUI(true);
		return true;
	}
	// 볼링 게임 앞결계에 막힌뒤 근처 몬스터 스폰 에너지공급 장치 찾기전 대화 종료
	if (81 == m_iTalkIndex)
	{
		m_bRender = false;
		UM->Set_FrontUI(true);
		return true;
	}
	// 첫 에너지 공급 장치를 부시기 전의 대화종료
	if (84 == m_iTalkIndex)
	{
		m_bRender = false;
		UM->Set_FrontUI(true);
		return true;
	}
	// 포탈 생성 대화 종료
	if (86 == m_iTalkIndex)
	{
		m_bRender = false;
		UM->Set_FrontUI(false);
		// 포탈 생성코드
		// TM->Make_Potal();
		TM->Get_QPotal()->Set_CutScene();
		return true;
	}
	// 문어 보스 잡고 나서 보스맵 가기전 대화 종료
	if (90 == m_iTalkIndex)
	{
		m_bRender = false;
		//UM->Set_QuestOn(true);
		UM->Set_FrontUI(true);
		UM->Set_QuestPoint({ 7.15f, 15.63f, 283.51f }, 0); // 보스맵 가는 균열 좌표
		return true;
	}

#pragma endregion Stage4

#pragma region Stage5
	// 보스 싸우기전 대화종료
	if (98 == m_iTalkIndex)
	{
		m_bRender = false;
		UM->Set_FrontUI(true);
		return true;
	}

#pragma endregion Stage5


	return false;
}

CDialogTalk * CDialogTalk::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDialogTalk*		pInstance = new CDialogTalk(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CDialogTalk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDialogTalk::Clone(void * pArg)
{
	CDialogTalk*		pInstance = new CDialogTalk(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CDialogTalk"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDialogTalk::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
