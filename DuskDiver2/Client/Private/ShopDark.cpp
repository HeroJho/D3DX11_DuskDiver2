#include "stdafx.h"
#include "..\Public\ShopDark.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"



CShopDark::CShopDark(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CShopDark::CShopDark(const CShopDark & rhs)
	: CUI(rhs)
{
}

HRESULT CShopDark::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShopDark::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CShopDark::Tick(_float fTimeDelta)
{
	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CShopDark::LateTick(_float fTimeDelta)
{
	if (UM->Get_Select())
	{

		__super::LateTick(fTimeDelta);

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
}

HRESULT CShopDark::Render()
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

	if ((UM->Get_Select() && UM->Get_ShopSelect() == 0) || (UM->SellGet_Select() && UM->Get_ShopSelect() == 1) || (UM->Get_UpgradeSelect() && UM->Get_ShopSelect() == 2))
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;

		__super::Render();
	}

	return S_OK;
}






HRESULT CShopDark::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopDark"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CShopDark::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CShopDark * CShopDark::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CShopDark*		pInstance = new CShopDark(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CShopDark"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShopDark::Clone(void * pArg)
{
	CShopDark*		pInstance = new CShopDark(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CShopDark"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CShopDark::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
