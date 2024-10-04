#include "stdafx.h"
#include "..\Public\ShopBuyCount.h"
#include "GameInstance.h"
#include "NumBuy.h"
#include "NumBuy2.h"
#include "UI_Manager.h"

#include "ToolManager.h"



CShopBuyCount::CShopBuyCount(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CShopBuyCount::CShopBuyCount(const CShopBuyCount & rhs)
	: CUI(rhs)
{
}

HRESULT CShopBuyCount::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShopBuyCount::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	if (FAILED(Make_ChildUI(10.7f, -34.4f, 20.f, 20.f, TEXT("Prototype_GameObject_NumBuy"))))
		return E_FAIL;

	vector<CUI*>::iterator iter = m_pChildUIs.begin();
	m_pUINum = (CNumBuy*)(*iter);

	if (FAILED(Make_ChildUI(87.5f, 0.83f, 25.f, 25.f, TEXT("Prototype_GameObject_NumBuy2"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter++;
	m_pUINum2 = (CNumBuy2*)(*iter);

	return S_OK;
}

void CShopBuyCount::Tick(_float fTimeDelta)
{
	if (UM->Get_Select())
	{
		m_pUINum->Set_Number(UM->Get_ShopItemCount());
		switch (UM->Get_ShopItem())
		{
		case 0:
			m_pUINum2->Set_Number(UM->Get_ShopItemCount() * 100);
			break;
		case 1:
			m_pUINum2->Set_Number(UM->Get_ShopItemCount() * 300);
			break;
		case 2:
			m_pUINum2->Set_Number(UM->Get_ShopItemCount() * 100);
			break;
		case 3:
			m_pUINum2->Set_Number(UM->Get_ShopItemCount() * 300);
			break;
		case 4:
			m_pUINum2->Set_Number(UM->Get_ShopItemCount() * 100);
			break;
		case 5:
			m_pUINum2->Set_Number(UM->Get_ShopItemCount() * 300);
			break;
		case 6:
			m_pUINum2->Set_Number(UM->Get_ShopItemCount() * 1000);
			break;
		default:
			break;
		}


	}

	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CShopBuyCount::LateTick(_float fTimeDelta)
{
	if (UM->Get_Select())
	{

		__super::LateTick(fTimeDelta);

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
}

HRESULT CShopBuyCount::Render()
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

	if (UM->Get_Select() && UM->Get_ShopSelect() == 0 )
	{

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;


		//UI_RenderDebug();

		// 자식들의 Render 호출
		__super::Render();
	}

	return S_OK;
}






HRESULT CShopBuyCount::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopBuyCount"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CShopBuyCount::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CShopBuyCount * CShopBuyCount::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CShopBuyCount*		pInstance = new CShopBuyCount(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CShopBuyCount"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShopBuyCount::Clone(void * pArg)
{
	CShopBuyCount*		pInstance = new CShopBuyCount(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CShopBuyCount"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CShopBuyCount::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
