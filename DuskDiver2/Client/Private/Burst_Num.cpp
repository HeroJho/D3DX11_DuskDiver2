#include "stdafx.h"
#include "..\Public\Burst_Num.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "ToolManager.h"
#include "Player_Manager.h"
#include "UI_Manager.h"

CBurst_Num::CBurst_Num(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CBurst_Num::CBurst_Num(const CBurst_Num & rhs)
	: CUI(rhs)
{
}

HRESULT CBurst_Num::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBurst_Num::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	m_eBurstState = B_120;

	return S_OK;
}

void CBurst_Num::Tick(_float fTimeDelta)
{
	if (CPlayer::PLAYER_01 == PM->Get_SelectPlayer())
	{
		if (PM->Get_BurstMode(CPlayer::PLAYER_01))
		{
			switch (UM->Get_BurstBarNum())
			{
			case 0:
				m_eBurstState = B_120;
				m_bBpMax = false;
				break;
			case 1:
				m_eBurstState = B_140;
				m_bBpMax = false;
				break;
			case 2:
				m_eBurstState = B_160;
				m_bBpMax = false;
				break;
			case 3:
				m_eBurstState = B_180;
				if (!m_bBpMax)
				{
					GI->PlaySoundW(L"UI_Burst_full.wav", SD_UI, 0.3f);
					m_bBpMax = true;
				}
				break;
			default:
				break;
			}
		}
	}

	else if (CPlayer::PLAYER_03 == PM->Get_SelectPlayer())
	{
		if (PM->Get_BurstMode(CPlayer::PLAYER_03))
		{
			switch (UM->Get_BurstBarNum())
			{
			case 0:
				m_eBurstState = B_120;
				m_bBpMax = false;
				break;
			case 1:
				m_eBurstState = B_140;
				m_bBpMax = false;
				break;
			case 2:
				m_eBurstState = B_160;
				m_bBpMax = false;
				break;
			case 3:
				m_eBurstState = B_180;
				if (!m_bBpMax)
				{
					GI->PlaySoundW(L"UI_Burst_full.wav", SD_UI, 0.3f);
					m_bBpMax = true;
				}
				break;
			default:
				break;
			}
		}
	}

	else if (CPlayer::PLAYER_04 == PM->Get_SelectPlayer())
	{
		if (PM->Get_BurstMode(CPlayer::PLAYER_04))
		{
			switch (UM->Get_BurstBarNum())
			{
			case 0:
				m_eBurstState = B_120;
				m_bBpMax = false;
				break;
			case 1:
				m_eBurstState = B_140;
				m_bBpMax = false;
				break;
			case 2:
				m_eBurstState = B_160;
				m_bBpMax = false;
				break;
			case 3:
				m_eBurstState = B_180;
				if (!m_bBpMax)
				{
					GI->PlaySoundW(L"UI_Burst_full.wav", SD_UI, 0.3f);
					m_bBpMax = true;
				}
				break;
			default:
				break;
			}
		}
	}

	//UI_InputDebug(fTimeDelta);



	__super::Tick(fTimeDelta);
}

void CBurst_Num::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CBurst_Num::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", m_eBurstState)))
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






HRESULT CBurst_Num::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Burst_Num"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CBurst_Num::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CBurst_Num * CBurst_Num::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBurst_Num*		pInstance = new CBurst_Num(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBurst_Num"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBurst_Num::Clone(void * pArg)
{
	CBurst_Num*		pInstance = new CBurst_Num(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBurst_Num"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CBurst_Num::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
