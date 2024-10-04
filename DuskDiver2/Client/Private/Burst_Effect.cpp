#include "stdafx.h"
#include "..\Public\Burst_Effect.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"
#include "Player_Manager.h"

CBurst_Effect::CBurst_Effect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CBurst_Effect::CBurst_Effect(const CBurst_Effect & rhs)
	: CUI(rhs)
{
}

HRESULT CBurst_Effect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBurst_Effect::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	return S_OK;
}

void CBurst_Effect::Tick(_float fTimeDelta)
{
	if (0 == PM->Get_SelectPlayer())
	{
		if (PM->Get_BurstMode(CPlayer::PLAYER_01))
		{
			if (3 == UM->Get_BurstBarNum())
			{
				if (!m_bRender)
				{
					m_bRender = true;
					m_iFrame = 0;
				}
			}

			else
			{
				m_bRender = false;
				m_iFrame = 0;
				m_fFrameAcc = 0.f;
			}
		}
	}

	else if (1 == PM->Get_SelectPlayer())
	{
		if (PM->Get_BurstMode(CPlayer::PLAYER_03))
		{
			if (3 == UM->Get_BurstBarNum())
			{
				if (!m_bRender)
				{
					m_bRender = true;
					m_iFrame = 0;
				}
			}

			else
			{
				m_bRender = false;
				m_iFrame = 0;
				m_fFrameAcc = 0.f;
			}
		}
	}

	else if (2 == PM->Get_SelectPlayer())
	{
		if (PM->Get_BurstMode(CPlayer::PLAYER_04))
		{
			if (3 == UM->Get_BurstBarNum())
			{
				if (!m_bRender)
				{
					m_bRender = true;
					m_iFrame = 0;
				}
			}

			else
			{
				m_bRender = false;
				m_iFrame = 0;
				m_fFrameAcc = 0.f;
			}
		}
	}

	if (m_bRender)
	{
		m_fFrameAcc += 20.f * fTimeDelta;

		if (1 <= m_fFrameAcc)
		{
			m_iFrame++;
			m_fFrameAcc = 0.f;

			if (14 == m_iFrame)
			{
				m_iFrame = 0;
			}
		}
	}

	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CBurst_Effect::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	if (m_bRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CBurst_Effect::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	//m_pShaderCom->Set_RawValue("XYIndex", &m_fXYIndex, sizeof(_float2));
	//m_pShaderCom->Set_RawValue("XFrame", &m_fXFrame, sizeof(_float));
	//m_pShaderCom->Set_RawValue("YFrame", &m_fYFrame, sizeof(_float));


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", m_iFrame)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(2)))
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


	// 자식들의 Render 호출
	__super::Render();


	return S_OK;
}






HRESULT CBurst_Effect::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Burst_Effect"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CBurst_Effect::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CBurst_Effect * CBurst_Effect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBurst_Effect*		pInstance = new CBurst_Effect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBurst_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBurst_Effect::Clone(void * pArg)
{
	CBurst_Effect*		pInstance = new CBurst_Effect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBurst_Effect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CBurst_Effect::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
