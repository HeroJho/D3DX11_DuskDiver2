#include "stdafx.h"
#include "..\Public\PartyBuffText.h"
#include "GameInstance.h"


#include "ToolManager.h"



CPartyBuffText::CPartyBuffText(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CPartyBuffText::CPartyBuffText(const CPartyBuffText & rhs)
	: CUI(rhs)
{
}

HRESULT CPartyBuffText::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPartyBuffText::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	if (FAILED(Make_ChildUI(-3.f, 15.f, 40.f, 41.f, TEXT("Prototype_GameObject_BuffStr"))))
		return E_FAIL;
	if (FAILED(Make_ChildUI(-3.f, 15.f, 40.f, 41.f, TEXT("Prototype_GameObject_BuffHp"))))
		return E_FAIL;

	return S_OK;
}

void CPartyBuffText::Tick(_float fTimeDelta)
{


	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CPartyBuffText::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CPartyBuffText::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;




	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CPartyBuffText::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_PartyBuff"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CPartyBuffText::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CPartyBuffText * CPartyBuffText::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPartyBuffText*		pInstance = new CPartyBuffText(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPartyBuffText"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPartyBuffText::Clone(void * pArg)
{
	CPartyBuffText*		pInstance = new CPartyBuffText(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPartyBuffText"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CPartyBuffText::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
