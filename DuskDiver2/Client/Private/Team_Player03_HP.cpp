#include "stdafx.h"
#include "..\Public\Team_Player03_HP.h"
#include "GameInstance.h"
#include "Player_Manager.h"
#include "ToolManager.h"
#include "Status_Manager.h"
#include "PS01_STNumber.h"

CTeam_Player03_HP::CTeam_Player03_HP(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CTeam_Player03_HP::CTeam_Player03_HP(const CTeam_Player03_HP & rhs)
	: CUI(rhs)
{
}

HRESULT CTeam_Player03_HP::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTeam_Player03_HP::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();



	return S_OK;
}

void CTeam_Player03_HP::Tick(_float fTimeDelta)
{

	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CTeam_Player03_HP::LateTick(_float fTimeDelta)
{
	_float fHP = PM->Get_PlayerHP(CPlayer::PLAYER_03);
	_float fFinalHP = STM->Get_FinalHp(CStatus_Manager::PLAYER03);
	m_fHPGauge = fHP / fFinalHP;

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CTeam_Player03_HP::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_HP", &m_fHPGauge, sizeof(_float));


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(38)))
		return E_FAIL;

	if (PM->Get_ReHp(CPlayer::PLAYER_03))
	{
		//부활했을때는
		if (FAILED(m_pShaderCom->Begin(40)))
			return E_FAIL;
	 }

	else
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}
	//UI_RenderDebug();


	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CTeam_Player03_HP::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Team_HP"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CTeam_Player03_HP::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CTeam_Player03_HP * CTeam_Player03_HP::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTeam_Player03_HP*		pInstance = new CTeam_Player03_HP(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTeam_Player03_HP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTeam_Player03_HP::Clone(void * pArg)
{
	CTeam_Player03_HP*		pInstance = new CTeam_Player03_HP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTeam_Player03_HP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CTeam_Player03_HP::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
