#include "stdafx.h"
#include "..\Public\UpgradeTexture.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"



CUpgradeTexture::CUpgradeTexture(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CUpgradeTexture::CUpgradeTexture(const CUpgradeTexture & rhs)
	: CUI(rhs)
{
}

HRESULT CUpgradeTexture::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUpgradeTexture::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CUpgradeTexture::Tick(_float fTimeDelta)
{

	if (UM->Get_UpgradeFinish())
	{
		m_fTime += fTimeDelta;
	}
	else
	{
		m_fTime = 0.f;
		m_fAlpha = 0.2f;
	}

	if (m_fTime >= 5.92f)
	{
		if (m_fAlpha <= 0)
			m_fAlpha = 0.f;
		else
			m_fAlpha -= fTimeDelta * 0.05f;
	}

	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CUpgradeTexture::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUpgradeTexture::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_TimeDelta", &m_fAlpha, sizeof(_float));


	if (UM->Get_ShopSelect() == 2 && UM->Get_ShopItem() == 0)
	{
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;
	}
	else if (UM->Get_ShopSelect() == 2 && UM->Get_ShopItem() == 1)
	{
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 1)))
			return E_FAIL;
	}
	else if (UM->Get_ShopSelect() == 2 && UM->Get_ShopItem() == 2)
	{
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 2)))
			return E_FAIL;
	}


	if (FAILED(m_pShaderCom->Begin(7)))
		return E_FAIL;

	if(UM->Get_UpgradeFinish())
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}




	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CUpgradeTexture::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpgradeTexture"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CUpgradeTexture::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CUpgradeTexture * CUpgradeTexture::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUpgradeTexture*		pInstance = new CUpgradeTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUpgradeTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUpgradeTexture::Clone(void * pArg)
{
	CUpgradeTexture*		pInstance = new CUpgradeTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUpgradeTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CUpgradeTexture::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
