#include "stdafx.h"
#include "..\Public\ShopEffect0.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"



CShopEffect0::CShopEffect0(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CShopEffect0::CShopEffect0(const CShopEffect0 & rhs)
	: CUI(rhs)
{
}

HRESULT CShopEffect0::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShopEffect0::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CShopEffect0::Tick(_float fTimeDelta)
{
	if (UM->Get_UpgradeSelect() && UM->Get_ShopSelect() == 2)
	{
		m_fTime2 += fTimeDelta;
	}
	if (CK_DOWN(DIK_RETURN) && m_fTime2 > 0.1f)
	{
		m_fTime = 0.f;
		m_fTime2 = 0.f;
	}

	if (m_fTime < 0.8f)
		m_fTime += fTimeDelta;
	if(m_fTime >= 0.8f)
		m_fTime = 0.79f;

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

void CShopEffect0::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CShopEffect0::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	for (int i = 0; i < 16; ++i)
	{
		if (m_fTime >= i * 0.05f && m_fTime < (i + 1) * 0.05f)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", i)))
				return E_FAIL;
			break;
		}
	}




	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;


	if(UM->Get_ShopSelect() == 2)
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}




	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CShopEffect0::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopEffect0"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CShopEffect0::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CShopEffect0 * CShopEffect0::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CShopEffect0*		pInstance = new CShopEffect0(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CShopEffect0"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShopEffect0::Clone(void * pArg)
{
	CShopEffect0*		pInstance = new CShopEffect0(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CShopEffect0"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CShopEffect0::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
