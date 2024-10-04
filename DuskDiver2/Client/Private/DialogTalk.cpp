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

	// ���� ��ȭ �Ǵ� �̺�Ʈ�� �ѱ�� ���
	if ((true == m_bRender) && (CK_UP(DIK_RETURN)))
	{
		// �̺�Ʈ �߻��� ���ٸ� ��ȭ�� �̾����.
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
	// ������ Render�� true�϶� ��¦���� �����ش� �̸� alpha�� 0���� �ؼ�
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
	// ����� �ڵ� -> ���߿� ������
	//return false;

#pragma region Stage1
	// Stage1 ���� ��������
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
	// Stage1 �����̴� ���� ����
	if ((6 == m_iTalkIndex))
	{
		m_bRender = false;
		UM->LEVIADA_On();
		UM->PartyOn(true);
		UM->Set_FrontUI(true);
		return true;
	}
	//����
	if (11 == m_iTalkIndex && !m_bSound)
	{
		//GI->PlaySound_Linear(L"SubWay.wav", SD_BGM, 0.3f, true, 0.2f);
		m_bSound = true;
	}
	// ���� ��ȯ
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
	// Stage2 ������ ��ȭ ���� 16
	if ((16 == m_iTalkIndex))
	{
		m_bRender = false;
		UM->Set_QuestNum(2);
		UM->Set_QuestOn(true);
		UM->Set_FrontUI(true);
		return true;
	}

	// �����̴� ��ȭ������ ���� ����
	if ((18 == m_iTalkIndex))
	{
		m_bRender = false;
		UM->Set_QuestOn(true);
		UM->Set_FrontUI(true);
		return true;
	}
	// ���� ĳ���� ȹ��
	if (27 == m_iTalkIndex)
	{
		UM->Set_QuestPoint({ -0.5f, 2.f, -3.95f }, 0); // �ⱸ ��ǥ
		m_bRender = false;
		UM->Set_FrontUI(true);
		return true;
	}
#pragma endregion Stage2

#pragma region Stage3
	// Stage3 ���۴�ȭ ����
	if ((29 == m_iTalkIndex))
	{
		m_bRender = false;
		UM->Set_QuestNum(5);
		UM->Set_QuestOn(true);
		UM->Set_FrontUI(true);
		return true;
	}
	// Stage3-1 ���� ���� �� ��ȭ ����
	if ((31 == m_iTalkIndex))
	{
		m_bRender = false;
		UM->Set_QuestOn(true);
		UM->Set_FrontUI(true);
		UM->Set_QuestPoint({ 8.5f, 2.f, 173.5f }, 0); // ����Ʈ ��ǥ
		return true;
	}

	// ����Ʈ ���� �� ��������� ��ȭ ����
	if ((34 == m_iTalkIndex))
	{
		m_bRender = false;
		UM->Set_QuestOn(true);
		UM->Set_FrontUI(true);
		UM->Set_QuestPoint({ 45.f, 2.f, 135.f }, 0); // �տ��� ��ġ -> ���߰� ���� ����

		CRKM->Create_Crack(_float4(45.f, 0.f, 135.f, 1.f), 4.f, 3, TM->Get_CurLevel());

		CRM->Get_Cam()->End_Target();
		return true;
	}
	// Stage3-2 ���� ���� �� -> ���߰� ���� óġ�� �ܼ� ȹ�� ��ȭ����
	if (36 == m_iTalkIndex)
	{
		m_bRender = false;
		UM->Set_QuestOn(true);
		UM->Set_FrontUI(true);
		UM->Set_QuestPoint({ 8.5f, 2.f, 173.5f }, 0); // ����Ʈ ��ǥ

		return true;
	}
	// �ܼ� ã�� ���� ����Ʈ ���� 
	if (41 == m_iTalkIndex)
	{
		m_bRender = false;
		UM->Set_QuestOn(true);
		UM->Set_FrontUI(true);
		UM->Set_CoinOn(true); // ���� UI On

		CRM->Get_Cam()->End_Target();
		return true;
	}

	// ��� �ܼ� ȹ���� ����Ʈ�� ���ư��� ��ȭ ����
	if (44 == m_iTalkIndex)
	{
		m_bRender = false;
		UM->Set_FrontUI(true);
		UM->Set_QuestOn(true);
		UM->Set_QuestPoint({ 8.5f, 2.f, 173.5f }, 0); // ����Ʈ ��ǥ

		CRM->Get_Cam()->End_Target();
		return true;
	}

	// ����Ʈ�� ���ƿ� �� ��������� ��ȭ ����
	if (47 == m_iTalkIndex)
	{
		m_bRender = false;
		UM->Set_QuestOn(true);
		UM->Set_FrontUI(true);
		UM->Set_QuestPointOff();
		// �տ��� ��ġ�� ����Ʈ ��ġ�� �˷����� �ʰ� �������� ã�´�.
		CRKM->Create_Crack(_float4(2.647f, 0.f, 147.f, 1.f), 4.f, 4, TM->Get_CurLevel()); // �տ��� ��ġ�� ���� 

		CRM->Get_Cam()->End_Target();
		return true;
	}
	// �� ������ ���� �տ� �߰��� ��ȭ����
	if (50 == m_iTalkIndex)
	{
		m_bRender = false;
		UM->Set_QuestOn(true);
		UM->Set_FrontUI(true);
		return true;
	}
	// ������ ���� ó���� �� ��ȭ ����
	if ((52 == m_iTalkIndex))
	{
		m_bRender = false;
		UM->Set_FrontUI(true);
		return true;
	}
	// ������ ���� ó���� �� �ܼ� ������ ���������� ����Ʈ ���� ��ȭ ����
	if ((54 == m_iTalkIndex))
	{
		m_bRender = false;
		UM->Set_QuestOn(true);
		UM->Set_FrontUI(true);
		UM->Set_QuestPoint({ 8.5f, 2.f, 173.5f }, 0); // ����Ʈ ��ǥ
		return true;
	}
	// ����Ʈ�� ���������� ���ƿ��� IT�븮�� �������� ��ȭ ����
	if (62 == m_iTalkIndex)
	{
		m_bRender = false;
		UM->Set_QuestOn(true);
		UM->Set_FrontUI(true);
		UM->Set_QuestPoint({ 45.7f, 0.f, -17.52f }, 0); // IT�븮 ��ǥ

		CRM->Get_Cam()->End_Target();
		return true;
	}
	// ���߰� ���� ������ ������ ��ȭ ����
	if (102 == m_iTalkIndex)
	{
		m_bRender = false;
		UM->Set_FrontUI(true);

		// �Ʒ� �ڵ带 ���� UI �˷��ص� 
		
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
	// Stage4 ���۴�ȭ ����
	if ((68 == m_iTalkIndex))
	{
		m_bRender = false;
		UM->Set_QuestOn(true);
		UM->Set_FrontUI(true);
		UM->Set_QuestPoint({ 1.91f, 16.7f, 132.87f }, 0); // ������ �߰���ġ(���� ���ư��°�) ��ǥ
		return true;
	}

	// ���� ���ư��� �ƽ��� ��� ���� ��ġ�� ���۹ޱ����� ��ȭ ����
	if (74 == m_iTalkIndex)
	{
		m_bRender = false;
		UM->Set_FrontUI(true);
		
		UM->Set_BattleTalkIndex(5); // ���� �ڽ��� ��� ���� ��ġ�� ��´�.
		return true;
	}
	// ��ġ ��� �ƽ� ������ ���� ������ ��ȭ����
	if (77 == m_iTalkIndex)
	{
		m_bRender = false;
		TM->Get_FunchCuma()->Set_State(CFunchCuma::STATE_IDLE); // ��ġ ���� ����
		UM->Set_FrontUI(true);
		return true;
	}
	// ���� ���� �հ�迡 ������ ��ó ���� ���� ���������� ��ġ ã���� ��ȭ ����
	if (81 == m_iTalkIndex)
	{
		m_bRender = false;
		UM->Set_FrontUI(true);
		return true;
	}
	// ù ������ ���� ��ġ�� �νñ� ���� ��ȭ����
	if (84 == m_iTalkIndex)
	{
		m_bRender = false;
		UM->Set_FrontUI(true);
		return true;
	}
	// ��Ż ���� ��ȭ ����
	if (86 == m_iTalkIndex)
	{
		m_bRender = false;
		UM->Set_FrontUI(false);
		// ��Ż �����ڵ�
		// TM->Make_Potal();
		TM->Get_QPotal()->Set_CutScene();
		return true;
	}
	// ���� ���� ��� ���� ������ ������ ��ȭ ����
	if (90 == m_iTalkIndex)
	{
		m_bRender = false;
		//UM->Set_QuestOn(true);
		UM->Set_FrontUI(true);
		UM->Set_QuestPoint({ 7.15f, 15.63f, 283.51f }, 0); // ������ ���� �տ� ��ǥ
		return true;
	}

#pragma endregion Stage4

#pragma region Stage5
	// ���� �ο���� ��ȭ����
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
