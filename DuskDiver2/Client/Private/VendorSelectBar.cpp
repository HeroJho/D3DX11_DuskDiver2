#include "stdafx.h"
#include "..\Public\VendorSelectBar.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"



CVendorSelectBar::CVendorSelectBar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CVendorSelectBar::CVendorSelectBar(const CVendorSelectBar & rhs)
	: CUI(rhs)
{
}

HRESULT CVendorSelectBar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVendorSelectBar::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CVendorSelectBar::Tick(_float fTimeDelta)
{
	if (UM->Get_ShopItem() == 0)
		m_UiInfo.fY = -140.f;
	else if (UM->Get_ShopItem() == 1)
		m_UiInfo.fY = -113.f;
	else if (UM->Get_ShopItem() == 2)
		m_UiInfo.fY = -46.f;
		

	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CVendorSelectBar::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CVendorSelectBar::Render()
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






HRESULT CVendorSelectBar::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopSelectBar"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CVendorSelectBar::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CVendorSelectBar * CVendorSelectBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CVendorSelectBar*		pInstance = new CVendorSelectBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CVendorSelectBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CVendorSelectBar::Clone(void * pArg)
{
	CVendorSelectBar*		pInstance = new CVendorSelectBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CVendorSelectBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CVendorSelectBar::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
