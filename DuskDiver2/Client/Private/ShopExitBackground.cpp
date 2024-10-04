#include "stdafx.h"
#include "..\Public\ShopExitBackground.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "ToolManager.h"



CShopExitBackground::CShopExitBackground(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CShopExitBackground::CShopExitBackground(const CShopExitBackground & rhs)
	: CUI(rhs)
{
}

HRESULT CShopExitBackground::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShopExitBackground::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();




	return S_OK;
}

void CShopExitBackground::Tick(_float fTimeDelta)
{
	if (m_UiInfo.fSizeX < 380.f)
		m_UiInfo.fSizeX += 30.f;
	else
		m_UiInfo.fSizeX = 380.f;

	if (UM->Get_ShopSelect() != 3)
		m_UiInfo.fSizeX = 150.f;


	//UI_InputDebug(fTimeDelta);



	__super::Tick(fTimeDelta);
}

void CShopExitBackground::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CShopExitBackground::Render()
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

	if (UM->Get_ShopSelect() == 3 && !UM->Get_ShopSelectBar())
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}

	//UI_RenderDebug();


	__super::Render();

	return S_OK;
}






HRESULT CShopExitBackground::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopExitBackground"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}







void CShopExitBackground::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CShopExitBackground * CShopExitBackground::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CShopExitBackground*		pInstance = new CShopExitBackground(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CShopExitBackground"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShopExitBackground::Clone(void * pArg)
{
	CShopExitBackground*		pInstance = new CShopExitBackground(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CShopExitBackground"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CShopExitBackground::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
