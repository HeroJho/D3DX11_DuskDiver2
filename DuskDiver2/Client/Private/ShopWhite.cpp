#include "stdafx.h"
#include "..\Public\ShopWhite.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"



CShopWhite::CShopWhite(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CShopWhite::CShopWhite(const CShopWhite & rhs)
	: CUI(rhs)
{
}

HRESULT CShopWhite::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShopWhite::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CShopWhite::Tick(_float fTimeDelta)
{
	if (UM->Get_ShopItem() == 0)
		m_UiInfo.fY = -210.f;
	else if (UM->Get_ShopItem() == 1)
		m_UiInfo.fY = -175.f;
	else if (UM->Get_ShopItem() == 2)
		m_UiInfo.fY = -140.f;
	else if (UM->Get_ShopItem() == 3)
		m_UiInfo.fY = -105.f;
	else if (UM->Get_ShopItem() == 4)
		m_UiInfo.fY = -70.f;
	else if (UM->Get_ShopItem() == 5)
		m_UiInfo.fY = -35.f;
	else if (UM->Get_ShopItem() == 6)
		m_UiInfo.fY = 0.f;


	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CShopWhite::LateTick(_float fTimeDelta)
{
	if (UM->Get_Select() || UM->SellGet_Select())
	{
		m_fTime2 += fTimeDelta;
	}

	if (CK_DOWN(DIK_RETURN) && m_fTime2 > 0.5f)
	{
		m_fTime = 0.f;
		m_fTime2 = 0.f;
	}

	if (m_fTime >= 1.f)
		m_fTime = 1.f;
	else
		m_fTime += fTimeDelta * 0.5f;

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CShopWhite::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_TimeDelta", &m_fTime, sizeof(_float));


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






HRESULT CShopWhite::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopWhite"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CShopWhite::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CShopWhite * CShopWhite::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CShopWhite*		pInstance = new CShopWhite(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CShopWhite"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShopWhite::Clone(void * pArg)
{
	CShopWhite*		pInstance = new CShopWhite(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CShopWhite"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CShopWhite::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
