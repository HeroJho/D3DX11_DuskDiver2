#include "stdafx.h"
#include "..\Public\Player_Status01_BP.h"
#include "GameInstance.h"
#include "Player_Manager.h"
#include "Status_Manager.h"
#include "ToolManager.h"
#include "UI_Manager.h"


CPlayer_Status01_BP::CPlayer_Status01_BP(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CPlayer_Status01_BP::CPlayer_Status01_BP(const CPlayer_Status01_BP & rhs)
	: CUI(rhs)
{
}

HRESULT CPlayer_Status01_BP::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Status01_BP::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	if (FAILED(Make_ChildUI(124.048965f, -0.148963f, 38.75f, 16.f, TEXT("Prototype_GameObject_EF"))))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Status01_BP::Tick(_float fTimeDelta)
{


	//UI_InputDebug(fTimeDelta);



	__super::Tick(fTimeDelta);
}

void CPlayer_Status01_BP::LateTick(_float fTimeDelta)
{
	m_fNowEp = PM->Get_PlayerEP(CPlayer::PLAYER_01);
	m_fMaxEp = 100.f;
	m_fEpGauge = m_fNowEp / m_fMaxEp;

	m_fPreEp = PM->Get_PlayerPreEP(CPlayer::PLAYER_01);
	m_fPreEpGauge = m_fPreEp / m_fMaxEp;

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CPlayer_Status01_BP::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_EP", &m_fEpGauge, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_PreEP", &m_fPreEpGauge, sizeof(_float));

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(21)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	//UI_RenderDebug();

	//3분의 1 이하이거나 변신 중일때는 안보이도록 
	if (34.f <= m_fNowEp && !PM->Get_BurstMode(CPlayer::PLAYER_01))
	{
		// 자식들의 Render 호출
		__super::Render();
	}

	return S_OK; 
}






HRESULT CPlayer_Status01_BP::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Status_BP"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CPlayer_Status01_BP::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CPlayer_Status01_BP * CPlayer_Status01_BP::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer_Status01_BP*		pInstance = new CPlayer_Status01_BP(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer_Status01_BP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer_Status01_BP::Clone(void * pArg)
{
	CPlayer_Status01_BP*		pInstance = new CPlayer_Status01_BP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer_Status01_BP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CPlayer_Status01_BP::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
