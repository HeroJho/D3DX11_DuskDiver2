#include "stdafx.h"
#include "..\Public\Player_Status01_SP.h"
#include "GameInstance.h"
#include "Player_Manager.h"
#include "Status_Manager.h"
#include "ToolManager.h"
#include "UI_Manager.h"


CPlayer_Status01_SP::CPlayer_Status01_SP(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CPlayer_Status01_SP::CPlayer_Status01_SP(const CPlayer_Status01_SP & rhs)
	: CUI(rhs)
{
}

HRESULT CPlayer_Status01_SP::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Status01_SP::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	if (FAILED(Make_ChildUI(26.961161f, 15.572119f, 297.5f, 89.6f, TEXT("Prototype_GameObject_EP_Battle"))))
		return E_FAIL;


	return S_OK;
}

void CPlayer_Status01_SP::Tick(_float fTimeDelta)
{


	//UI_InputDebug(fTimeDelta);


	__super::Tick(fTimeDelta);

}

void CPlayer_Status01_SP::LateTick(_float fTimeDelta)
{
	m_fNowSp = PM->Get_PlayerSP(CPlayer::PLAYER_01);
	m_fMaxSp = 100.f;
	m_fSpGauge = m_fNowSp / m_fMaxSp;

	m_fPreSp = PM->Get_PlayerPreSP(CPlayer::PLAYER_01);
	m_fPreSpGauge = m_fPreSp / m_fMaxSp;

	__super::LateTick(fTimeDelta);


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CPlayer_Status01_SP::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_SP", &m_fSpGauge, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_PreSP", &m_fPreSpGauge, sizeof(_float));

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(20)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	//UI_RenderDebug();

	if (PM->Get_BurstMode(CPlayer::PLAYER_01))
	{
		// 자식들의 Render 호출
		__super::Render();
	}

	return S_OK;
}






HRESULT CPlayer_Status01_SP::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Status_SP"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Status01_SP::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CPlayer_Status01_SP * CPlayer_Status01_SP::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer_Status01_SP*		pInstance = new CPlayer_Status01_SP(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer_Status01_SP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer_Status01_SP::Clone(void * pArg)
{
	CPlayer_Status01_SP*		pInstance = new CPlayer_Status01_SP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer_Status01_SP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CPlayer_Status01_SP::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
