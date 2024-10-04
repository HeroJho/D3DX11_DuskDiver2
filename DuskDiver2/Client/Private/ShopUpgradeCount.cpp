#include "stdafx.h"
#include "..\Public\ShopUpgradeCount.h"
#include "GameInstance.h"
#include "NumBuy.h"
#include "NumBuy2.h"
#include "UI_Manager.h"

#include "ToolManager.h"



CShopUpgradeCount::CShopUpgradeCount(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CShopUpgradeCount::CShopUpgradeCount(const CShopUpgradeCount & rhs)
	: CUI(rhs)
{
}

HRESULT CShopUpgradeCount::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShopUpgradeCount::Initialize(void * pArg)
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

	if (FAILED(Make_ChildUI(0.f, -40.f, 256.f * 0.8f, 38.f * 0.8f, TEXT("Prototype_GameObject_WeaponLevelUp1"))))
		return E_FAIL;

	return S_OK;
}

void CShopUpgradeCount::Tick(_float fTimeDelta)
{
	if (UM->Get_ShopSelect() == 2 && UM->Get_ShopItem() == 0)
	{
		switch (UM->Get_Player1Weapon())
		{
		case 0:
			m_pUINum2->Set_Number(100);
			break;
		case 1:
			m_pUINum2->Set_Number(200);
			break;
		case 2:
			m_pUINum2->Set_Number(300);
			break;
		case 3:
			m_pUINum2->Set_Number(400);
			break;
		case 4:
			m_pUINum2->Set_Number(500);
			break;
		case 5:
			m_pUINum2->Set_Number(600);
			break;
		case 6:
			m_pUINum2->Set_Number(700);
			break;
		default:
			break;
		}
	}
	else if (UM->Get_ShopSelect() == 2 && UM->Get_ShopItem() == 1)
	{
		switch (UM->Get_Player1Weapon())
		{
		case 0:
			m_pUINum2->Set_Number(100);
			break;
		case 1:
			m_pUINum2->Set_Number(200);
			break;
		case 2:
			m_pUINum2->Set_Number(300);
			break;
		case 3:
			m_pUINum2->Set_Number(400);
			break;
		case 4:
			m_pUINum2->Set_Number(500);
			break;
		case 5:
			m_pUINum2->Set_Number(600);
			break;
		case 6:
			m_pUINum2->Set_Number(700);
			break;
		default:
			break;
		}
	}
	else if (UM->Get_ShopSelect() == 2 && UM->Get_ShopItem() == 2)
	{
		switch (UM->Get_Player1Weapon())
		{
		case 0:
			m_pUINum2->Set_Number(100);
			break;
		case 1:
			m_pUINum2->Set_Number(200);
			break;
		case 2:
			m_pUINum2->Set_Number(300);
			break;
		case 3:
			m_pUINum2->Set_Number(400);
			break;
		case 4:
			m_pUINum2->Set_Number(500);
			break;
		case 5:
			m_pUINum2->Set_Number(600);
			break;
		case 6:
			m_pUINum2->Set_Number(700);
			break;
		default:
			break;
		}
	}

	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CShopUpgradeCount::LateTick(_float fTimeDelta)
{
	if (UM->Get_Select())
	{

		__super::LateTick(fTimeDelta);

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
}

HRESULT CShopUpgradeCount::Render()
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

	if (UM->Get_UpgradeSelect() && UM->Get_ShopSelect() == 2)
	{

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;


		//UI_RenderDebug();

		// 자식들의 Render 호출
		__super::Render();
	}

	return S_OK;
}






HRESULT CShopUpgradeCount::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopUpgradeWeaponCount"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CShopUpgradeCount::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CShopUpgradeCount * CShopUpgradeCount::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CShopUpgradeCount*		pInstance = new CShopUpgradeCount(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CShopUpgradeCount"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShopUpgradeCount::Clone(void * pArg)
{
	CShopUpgradeCount*		pInstance = new CShopUpgradeCount(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CShopUpgradeCount"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CShopUpgradeCount::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
