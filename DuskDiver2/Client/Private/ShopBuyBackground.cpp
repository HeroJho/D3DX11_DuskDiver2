#include "stdafx.h"
#include "..\Public\ShopBuyBackground.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "ToolManager.h"



CShopBuyBackground::CShopBuyBackground(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CShopBuyBackground::CShopBuyBackground(const CShopBuyBackground & rhs)
	: CUI(rhs)
{
}

HRESULT CShopBuyBackground::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShopBuyBackground::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();




	return S_OK;
}

void CShopBuyBackground::Tick(_float fTimeDelta)
{
	if (m_UiInfo.fSizeX < 380.f)
		m_UiInfo.fSizeX += 30.f;
	else
		m_UiInfo.fSizeX = 380.f;

	if (UM->Get_ShopSelect() != 0)
		m_UiInfo.fSizeX = 150.f;


	//UI_InputDebug(fTimeDelta);



	__super::Tick(fTimeDelta);
}

void CShopBuyBackground::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CShopBuyBackground::Render()
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

	if (UM->Get_ShopSelect() == 0 && !UM->Get_ShopSelectBar())
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}

	//UI_RenderDebug();


	__super::Render();

	return S_OK;
}






HRESULT CShopBuyBackground::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopBuyBackground"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}







void CShopBuyBackground::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CShopBuyBackground * CShopBuyBackground::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CShopBuyBackground*		pInstance = new CShopBuyBackground(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CShopBuyBackground"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShopBuyBackground::Clone(void * pArg)
{
	CShopBuyBackground*		pInstance = new CShopBuyBackground(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CShopBuyBackground"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CShopBuyBackground::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
