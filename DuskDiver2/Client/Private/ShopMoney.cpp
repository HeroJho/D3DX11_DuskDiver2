#include "stdafx.h"
#include "..\Public\ShopMoney.h"
#include "GameInstance.h"
#include "MenuNumber1.h"
#include "UI_Manager.h"
#include "ShopUseMoney.h"
#include "ToolManager.h"



CShopMoney::CShopMoney(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CShopMoney::CShopMoney(const CShopMoney & rhs)
	: CUI(rhs)
{
}

HRESULT CShopMoney::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShopMoney::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	if (FAILED(Make_ChildUI(30.f, 0.f, 20.f, 20.f, TEXT("Prototype_GameObject_MenuNumber1"))))
		return E_FAIL;

	vector<CUI*>::iterator iter = m_pChildUIs.begin();
	m_pUINum = (CMenuNumber1*)(*iter);

	if (FAILED(Make_ChildUI(117.f, 0.f, 25.f, 25.f, TEXT("Prototype_GameObject_ShopUseMoney"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter++;
	m_pUINum2 = (CShopUseMoney*)(*iter);

	if (FAILED(Make_ChildUI(100.67f, 1.66f, 25.f, 25.f, TEXT("Prototype_GameObject_ShopSign"))))
		return E_FAIL;

	return S_OK;
}

void CShopMoney::Tick(_float fTimeDelta)
{
	m_pUINum->Set_Number(UM->Get_Money());
	m_pUINum2->Set_Number(UM->Get_ShopUsedMoney());


	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CShopMoney::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CShopMoney::Render()
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






HRESULT CShopMoney::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopMoney"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CShopMoney::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CShopMoney * CShopMoney::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CShopMoney*		pInstance = new CShopMoney(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CShopMoney"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShopMoney::Clone(void * pArg)
{
	CShopMoney*		pInstance = new CShopMoney(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CShopMoney"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CShopMoney::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
