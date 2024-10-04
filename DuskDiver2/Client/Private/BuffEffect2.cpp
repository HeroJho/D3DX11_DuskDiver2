#include "stdafx.h"
#include "..\Public\BuffEffect2.h"
#include "GameInstance.h"


#include "ToolManager.h"



CBuffEffect2::CBuffEffect2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CBuffEffect2::CBuffEffect2(const CBuffEffect2 & rhs)
	: CUI(rhs)
{
}

HRESULT CBuffEffect2::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBuffEffect2::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CBuffEffect2::Tick(_float fTimeDelta)
{
	m_fTime += fTimeDelta;

	if (m_fTime > 0.01f)
	{
		m_UiInfo.fSizeX += 0.5f;
		m_UiInfo.fSizeY += 0.5f;
		m_fTime = 0.f;
		m_fAlpha += fTimeDelta * 0.2f;
	}

	if (m_UiInfo.fSizeX > 60.f)
		Set_Dead();


	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CBuffEffect2::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CBuffEffect2::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_TimeDelta", &m_fAlpha, sizeof(_float));


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(7)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;




	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CBuffEffect2::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BuffEffectStr"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CBuffEffect2::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CBuffEffect2 * CBuffEffect2::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBuffEffect2*		pInstance = new CBuffEffect2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBuffEffect2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBuffEffect2::Clone(void * pArg)
{
	CBuffEffect2*		pInstance = new CBuffEffect2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBuffEffect2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CBuffEffect2::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
