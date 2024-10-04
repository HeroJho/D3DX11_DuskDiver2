#include "stdafx.h"
#include "..\Public\ShopSign.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"



CShopSign::CShopSign(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CShopSign::CShopSign(const CShopSign & rhs)
	: CUI(rhs)
{
}

HRESULT CShopSign::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShopSign::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CShopSign::Tick(_float fTimeDelta)
{
	if (UM->Get_UsedMoney())
		m_fVanish += fTimeDelta * 0.5f;
	else
		m_fVanish = 0.f;

	if (m_fVanish > 0.8f && UM->Get_UsedMoney())
	{
		UM->Set_UsedMoney(false);
		m_fVanish = 0.f;
	}
	if (UM->Get_ShopSelect() == 0 || UM->Get_ShopSelect() == 2)
		m_iType = 0;
	else
		m_iType = 1;


	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CShopSign::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CShopSign::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_TimeDelta", &m_fVanish, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_Type", &m_iType, sizeof(_float));




	if (UM->Get_UsedMoney())
	{
		if (UM->Get_ShopSelect() == 0 || UM->Get_ShopSelect() == 2)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 1)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
				return E_FAIL;
		}

		if (FAILED(m_pShaderCom->Begin(37)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;

	}




	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CShopSign::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopSign"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CShopSign::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CShopSign * CShopSign::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CShopSign*		pInstance = new CShopSign(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CShopSign"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShopSign::Clone(void * pArg)
{
	CShopSign*		pInstance = new CShopSign(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CShopSign"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CShopSign::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
