#include "stdafx.h"
#include "..\Public\Team_Player01_SP.h"
#include "GameInstance.h"
#include "Player_Manager.h"
#include "Status_Manager.h"
#include "ToolManager.h"
#include "Team_SP_Number.h"


CTeam_Player01_SP::CTeam_Player01_SP(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CTeam_Player01_SP::CTeam_Player01_SP(const CTeam_Player01_SP & rhs)
	: CUI(rhs)
{
}

HRESULT CTeam_Player01_SP::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTeam_Player01_SP::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	if (FAILED(Make_ChildUI(29.098974f, 3.321264f, 14.2f, 14.2f, TEXT("Prototype_GameObject_Team_SP_Number"))))
		return E_FAIL;

	vector<CUI*>::iterator iter = m_pChildUIs.begin();
	m_pUINum = (CTeam_SP_Number*)(*iter);

	if (FAILED(Make_ChildUI(18.281096f, 3.837559f, 10.2f, 10.2f, TEXT("Prototype_GameObject_Team_SP"))))
		return E_FAIL;



	return S_OK;
}

void CTeam_Player01_SP::Tick(_float fTimeDelta)
{
	m_pUINum->Set_Number(m_iSP);

	//UI_InputDebug(fTimeDelta);

	m_iSP = 3;


	__super::Tick(fTimeDelta);
}

void CTeam_Player01_SP::LateTick(_float fTimeDelta)
{
	_float fSP = PM->Get_PlayerSP(CPlayer::PLAYER_01);
	_float fFinalSP = 100.f;
	m_fSPGauge = fSP / fFinalSP;

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CTeam_Player01_SP::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_SP", &m_fSPGauge, sizeof(_float));


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(18)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	//UI_RenderDebug();


	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CTeam_Player01_SP::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Team_SP"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CTeam_Player01_SP::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CTeam_Player01_SP * CTeam_Player01_SP::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTeam_Player01_SP*		pInstance = new CTeam_Player01_SP(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTeam_Player01_SP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTeam_Player01_SP::Clone(void * pArg)
{
	CTeam_Player01_SP*		pInstance = new CTeam_Player01_SP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTeam_Player01_SP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CTeam_Player01_SP::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
