#include "stdafx.h"
#include "..\Public\Burst_Text.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "ToolManager.h"
#include "Player_Manager.h"


CBurst_Text::CBurst_Text(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CBurst_Text::CBurst_Text(const CBurst_Text & rhs)
	: CUI(rhs)
{
}

HRESULT CBurst_Text::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBurst_Text::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();
	
	m_eBurstState = B_LOW;

	return S_OK;
}

void CBurst_Text::Tick(_float fTimeDelta)
{
	if (CPlayer::PLAYER_01 == PM->Get_SelectPlayer())
	{
		if (PM->Get_BurstMode(CPlayer::PLAYER_01))
		{
			if (3 == UM->Get_BurstBarNum())
				m_eBurstState = B_HIGH;

			else
				m_eBurstState = B_LOW;
		}
	}

	else if (CPlayer::PLAYER_03 == PM->Get_SelectPlayer())
	{
		if (PM->Get_BurstMode(CPlayer::PLAYER_03))
		{
			if (3 == UM->Get_BurstBarNum())
				m_eBurstState = B_HIGH;

			else
				m_eBurstState = B_LOW;
		}
	}

	else if (CPlayer::PLAYER_04 == PM->Get_SelectPlayer())
	{
		if (PM->Get_BurstMode(CPlayer::PLAYER_04))
		{
			if (3 == UM->Get_BurstBarNum())
				m_eBurstState = B_HIGH;

			else
				m_eBurstState = B_LOW;
		}
	}

	//UI_InputDebug(fTimeDelta);

	//±ôºýÀÌ±â
	if (B_HIGH == m_eBurstState)
	{
		if (m_bHigh)
		{
			m_fTextAcc += 3.f * fTimeDelta;

			if (1.f <= m_fTextAcc)
			{
				m_bHigh = false;
				m_fTextAcc = 1.f;
			}
		}

		else
		{
			m_fTextAcc -= 3.f * fTimeDelta;

			if (0.2f >= m_fTextAcc)
			{
				m_bHigh = true;
				m_fTextAcc = 0.2f;
			}
		}
	}

	//¾È±ôºýÀÌ±â
	else
		m_fTextAcc = 1.f;

	__super::Tick(fTimeDelta);
}

void CBurst_Text::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CBurst_Text::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_QuestGaugeAcc", &m_fTextAcc, sizeof(_float));


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", m_eBurstState)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(14)))
		return E_FAIL;

	if (CPlayer::PLAYER_01 == PM->Get_SelectPlayer())
	{
		if (PM->Get_BurstMode(CPlayer::PLAYER_01))
		{
			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}
	}

	else if (CPlayer::PLAYER_03 == PM->Get_SelectPlayer())
	{
		if (PM->Get_BurstMode(CPlayer::PLAYER_03))
		{
			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}
	}

	else if (CPlayer::PLAYER_04 == PM->Get_SelectPlayer())
	{
		if (PM->Get_BurstMode(CPlayer::PLAYER_04))
		{
			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}
	}

	//UI_RenderDebug();


	// ÀÚ½ÄµéÀÇ Render È£Ãâ
	__super::Render();

	return S_OK;
}






HRESULT CBurst_Text::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Burst_Text"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CBurst_Text::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CBurst_Text * CBurst_Text::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBurst_Text*		pInstance = new CBurst_Text(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBurst_Text"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBurst_Text::Clone(void * pArg)
{
	CBurst_Text*		pInstance = new CBurst_Text(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBurst_Text"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CBurst_Text::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
