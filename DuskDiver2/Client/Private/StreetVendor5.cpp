#include "stdafx.h"
#include "..\Public\StreetVendor5.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"

#include "NumExp.h"
#include "NumExplosive.h"
#include "NumReserveExp.h"
#include "NumHp.h"
#include "NumMental.h"
#include "NumMeta.h"
#include "NumStr.h"
#include "NumLevel.h"



CStreetVendor5::CStreetVendor5(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CStreetVendor5::CStreetVendor5(const CStreetVendor5 & rhs)
	: CUI(rhs)
{
}

HRESULT CStreetVendor5::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStreetVendor5::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CStreetVendor5::Tick(_float fTimeDelta)
{
	//if (CK_DOWN(DIK_U))




	if (UM->Get_Vendor5Select())
	{
		if (m_bSelectPage)
		{
			if (CK_DOWN(DIK_RIGHT))
			{
				m_iCountItem++;
			}
			else if (CK_DOWN(DIK_LEFT))
			{
				if (m_iCountItem == 0)
				{
					m_iCountItem = 0;
				}
				else
					m_iCountItem--;
			}
			if (CK_DOWN(DIK_RETURN))
			{
				if (m_iSelectItem == 0)
				{
					UM->Gain_BattleItem(0, m_iCountItem);
					m_iCountItem = 0;
				}
				else if (m_iSelectItem == 1)
				{
					UM->Gain_BattleItem(1, m_iCountItem);
					m_iCountItem = 0;
				}
				else if (m_iSelectItem == 2)
				{
					UM->Gain_BattleItem(2, m_iCountItem);
					m_iCountItem = 0;
				}
				else if (m_iSelectItem == 3)
				{
					UM->Gain_BattleItem(3, m_iCountItem);
					m_iCountItem = 0;
				}
				else if (m_iSelectItem == 4)
				{
					UM->Gain_BattleItem(4, m_iCountItem);
					m_iCountItem = 0;
				}
				else if (m_iSelectItem == 5)
				{
					UM->Gain_BattleItem(5, m_iCountItem);
					m_iCountItem = 0;
				}
				else if (m_iSelectItem == 6)
				{
					UM->Gain_BattleItem(6, m_iCountItem);
					m_iCountItem = 0;
				}
			}
		}

		else if (!m_bSelectPage)
		{
			if (CK_DOWN(DIK_UP))
			{
				if (m_iSelectItem == 0)
				{
					m_iSelectItem = 0;
				}
				else
				{
					m_iSelectItem--;
				}

			}
			else if (CK_DOWN(DIK_DOWN))
			{
				if (m_iSelectItem == 6)
				{
					m_iSelectItem = 6;
				}
				else
				{
					m_iSelectItem++;
				}
			}
		}

		if (CK_DOWN(DIK_RETURN) && m_bSelectPage == false)
		{
			m_bSelectPage = true;
			UM->Select_On();
		}

		else if (CK_DOWN(DIK_BACK) && m_bSelectPage == true)
		{
			m_bSelectPage = false;
			UM->Select_Off();
		}



		if (UM->Get_ShopState())
		{
			//UI_InputDebug(fTimeDelta);

			__super::Tick(fTimeDelta);
		}

		UM->Set_ShopItemCount(m_iCountItem);
		UM->Set_ShopItem(m_iSelectItem);
	}
}

void CStreetVendor5::LateTick(_float fTimeDelta)
{
	if (UM->Get_ShopState())
	{
		__super::LateTick(fTimeDelta);
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CStreetVendor5::Render()
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

	if (UM->Get_ShopState())
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;




		//UI_RenderDebug();

		// 자식들의 Render 호출
		__super::Render();
	}

	return S_OK;
}






HRESULT CStreetVendor5::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_VendorBackground"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CStreetVendor5::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CStreetVendor5 * CStreetVendor5::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStreetVendor5*		pInstance = new CStreetVendor5(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStreetVendor4"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStreetVendor5::Clone(void * pArg)
{
	CStreetVendor5*		pInstance = new CStreetVendor5(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStreetVendor4"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CStreetVendor5::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
