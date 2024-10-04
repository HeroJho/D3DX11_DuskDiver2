#include "stdafx.h"
#include "..\Public\BattleDialog.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "ToolManager.h"
#include "Monster_Manager.h"
#include "Player_Manager.h"
#include "Crack_Manager.h"

CBattleDialog::CBattleDialog(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI(pDevice, pContext)
{
}

CBattleDialog::CBattleDialog(const CBattleDialog & rhs)
	:CUI(rhs)
{
}

HRESULT CBattleDialog::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBattleDialog::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	UM->BackUp_BattleDialog(this);

	return S_OK;
}

void CBattleDialog::Tick(_float fTimeDelta)
{
	if (m_iTalkIndex < 100)
		Counting_DelayTime(fTimeDelta);

	if ((true == m_bRender) && (true == m_bDelayOver))
	{
		if ((false == m_bAlphaReverse) && (0.8f > m_fRenderAlpha))
		{
			m_fRenderAlpha += 5.f * fTimeDelta;
			if (0.8f < m_fRenderAlpha)
			{
				m_fRenderAlpha = 0.8f;
			}
		}

		// ������ �����ϸ� �ð��� ���
		if (true == Check_RenderTime(fTimeDelta))
		{
			// alpha���� �ٽ� 0.f�� �� ���Ŀ� �Ǵ�
			if (true == ReturnZero(fTimeDelta))
			{
				// ���� �ð��� ������ �̺�Ʈ üũ�Ͽ� �̺�Ʈ�� ������ �ε��� +1
				if (false == Check_EventTalk())
				{
					++m_iTalkIndex;
					m_fRenderAlpha = 0.f;
					m_fDelayTime = 0.f; //��ȭ�� �ѱ�� delay������ 0���� �ʱ�ȭ
					m_bDelayOver = false;
				}
			}
			
		}
		// debuging �ڵ�
		if (CK_DOWN(DIK_RETURN))
		{
			if (false == Check_EventTalk())
			{
				++m_iTalkIndex;
				m_fRenderAlpha = 0.f;
				m_fDelayTime = 0.f; //��ȭ�� �ѱ�� delay������ 0���� �ʱ�ȭ
				m_bDelayOver = false;
			}
		}
	}

	__super::Tick(fTimeDelta);
}

void CBattleDialog::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);

	if ((true == m_bRender) && (true == m_bDelayOver))
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
}

HRESULT CBattleDialog::Render()
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
	
	//UI_RenderDebug();

	return S_OK;
}

void CBattleDialog::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	ImGui::End();
}

const _bool CBattleDialog::Get_BattleTalkOnOff()
{
	return m_bRender;
}

void CBattleDialog::Set_BattleTalkIndex(_uint Index)
{
	m_bRender = true;
	m_iTalkIndex = Index;
	m_fDelayTime = 0.f;
}

HRESULT CBattleDialog::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BattleDialog"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

_bool CBattleDialog::Check_RenderTime(_float fTimeDelta)
{
	m_fRenderTime += 0.7f * fTimeDelta;
	
	if (m_fMaxRenderTime <= m_fRenderTime)
	{
		m_bAlphaReverse = true;
		return true;
	}

	return false;
}

_bool CBattleDialog::Check_EventTalk()
{
	// UM->Set_QuestPoint(_float3(118.76f, -31.07f, 216.74f), 1); // ��Ű ��ġ
	// UM->Set_QuestPoint(_float3(-104.39f, -3.89f, 122.54f), 1); // �߹��� ��ġ
	// UM->Set_QuestPoint(_float3(-81.3f, 23.46f, 126.96f), 1); // ������ ��ġ
	// UM->Set_QuestPoint(_float3(-176.03f, 39.19f, 120.42f), 1); // �δ��� ��ġ
	//
	// ��ȭ�� ����ñ���� m_bRender�� false�� ������ true�� ����

	// Stage1���� �κ������� ��ȭ����
	if (1 == m_iTalkIndex)
	{
		//�����ϸ鼭 bgm
		//GI->PlaySound_Linear(L"BGM_Battle_LM.wav", SD_BGM, 0.2f, true, 0.4f);

		MM->WakeUp_TrainingBot(0);
		m_bRender = false;
		return true;
	}
	// Stage3���� �� �տ� �߰������� ��ȭ����
	if (4 == m_iTalkIndex)
	{
		m_bRender = false;
		return true;
	}
	// Stage4���� ��ġ�����ġ Ȯ��
	if (5 == m_iTalkIndex)
	{
		UM->Set_QuestPoint(_float3(141.47f, -15.63f, 86.2f), 1); // ��ġ��� ��ġ
	}
	if ((6 == m_iTalkIndex) || (12 == m_iTalkIndex) /*|| (15 == m_iTalkIndex)*/ || (16 == m_iTalkIndex) || (18 == m_iTalkIndex) || (19 == m_iTalkIndex) ||
		(30 == m_iTalkIndex) || (31 == m_iTalkIndex) || (32 == m_iTalkIndex) || (33 == m_iTalkIndex) || (34 == m_iTalkIndex))
	{
		m_bRender = false;
		return true;
	}
	if (8 == m_iTalkIndex)
	{
		UM->Set_QuestPoint(_float3(172.4f, 3.902f, 150.566f), 1); // ������ġ
		m_bRender = false;
		return true;
	}
	if (10 == m_iTalkIndex) // ó�� ���������� �μ��� ������ ��谡 Ǯ���� ��ȭ��
	{
		UM->Set_FrontUI(true);
		m_bRender = false;
		return true;
	}
	if (15 == m_iTalkIndex) // ��Ű ���� ��. �ӽ�,  ���߿� ������
	{
		m_bRender = false;
		return true;
	}
	if (22 == m_iTalkIndex) // ������ �� �Ұ� ��
	{
		m_bRender = false;
		return true;
	}
	if (24 == m_iTalkIndex) // ������ �� �� ��ȭ��
	{
		m_bRender = false;
		return true;
	}
	if (26 == m_iTalkIndex) // DGgame start
	{
		m_bRender = false;
		return true;
	}
	if (27 == m_iTalkIndex) // �̱� ���� ���� ����������ȭ��
	{
		m_bRender = false;
		return true;
	}
	if (29 == m_iTalkIndex) // ���� ���� ������ ��ȭ ��
	{
		m_bRender = false;
		return true;
	}

	return false;
}

void CBattleDialog::Counting_DelayTime(_float fTimeDelta)
{
	// Ư�� �ε����� �ٷ� ����� �ƴ϶� ���� �ð��ڿ� ��� �� �ð��� ���⼭ ���
	if (0 == m_iTalkIndex)
	{
		m_fMaxDelay = 1.5f;
		m_bDelayOver = false;
	}
	else if (5 == m_iTalkIndex)
	{
		m_fMaxDelay = 3.f;
		m_bDelayOver = false;
	}
	else
	{
		m_fMaxDelay = 0.f;
	}

	m_fDelayTime += 1.f * fTimeDelta;
	if (m_fMaxDelay <= m_fDelayTime)
	{
		m_bDelayOver = true;
	}
}

_bool CBattleDialog::ReturnZero(_float fTimeDelta)
{
	if (0.f < m_fRenderAlpha)
	{
		m_fRenderAlpha -= 10.f * fTimeDelta;
		if (0.f > m_fRenderAlpha)
			m_fRenderAlpha = 0.f;
	}
	else
	{
		m_fRenderTime = 0.f; // �����ð��� �ʱ�ȭ���ش�
		m_bAlphaReverse = false;
		return true;
	}


	return false;
}

CBattleDialog * CBattleDialog::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBattleDialog*		pInstance = new CBattleDialog(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBattleDialog"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBattleDialog::Clone(void * pArg)
{
	CBattleDialog*		pInstance = new CBattleDialog(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBattleDialog"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBattleDialog::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
