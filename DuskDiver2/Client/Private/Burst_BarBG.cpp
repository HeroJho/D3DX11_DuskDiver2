#include "stdafx.h"
#include "..\Public\Burst_BarBG.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Player_Manager.h"
#include "UI_Manager.h"

CBurst_BarBG::CBurst_BarBG(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CBurst_BarBG::CBurst_BarBG(const CBurst_BarBG & rhs)
	: CUI(rhs)
{
}

HRESULT CBurst_BarBG::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBurst_BarBG::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();




	return S_OK;
}

void CBurst_BarBG::Tick(_float fTimeDelta)
{
	//UI_InputDebug(fTimeDelta);


	__super::Tick(fTimeDelta);
}

void CBurst_BarBG::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CBurst_BarBG::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fBP", &m_fBurstBp, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fLimitAngle", &m_fLimitAngle, sizeof(_float))))
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






HRESULT CBurst_BarBG::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Burst_BarBG"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CBurst_BarBG::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	ImGui::DragFloat("BP ", &m_fBurstBp, 0.f);

	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;


	ImGui::End();
}



CBurst_BarBG * CBurst_BarBG::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBurst_BarBG*		pInstance = new CBurst_BarBG(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBurst_BarBG"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBurst_BarBG::Clone(void * pArg)
{
	CBurst_BarBG*		pInstance = new CBurst_BarBG(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBurst_BarBG"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CBurst_BarBG::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
