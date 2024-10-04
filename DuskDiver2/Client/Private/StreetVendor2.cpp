#include "stdafx.h"
#include "..\Public\StreetVendor2.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"
#include "Camera_Manager.h"

#include "NumExp.h"
#include "NumExplosive.h"
#include "NumReserveExp.h"
#include "NumHp.h"
#include "NumMental.h"
#include "NumMeta.h"
#include "NumStr.h"
#include "NumLevel.h"



CStreetVendor2::CStreetVendor2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CStreetVendor2::CStreetVendor2(const CStreetVendor2 & rhs)
	: CUI(rhs)
{
}

HRESULT CStreetVendor2::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStreetVendor2::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();
	if (FAILED(Make_ChildUI(0.f, -140.f, 860.f * 0.7f, 55.f * 0.7f, TEXT("Prototype_GameObject_VendorSelectBar"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, 0.f, 1028.f * 0.7f, 400.f * 0.7f, TEXT("Prototype_GameObject_ShopBuyCount"))))
		return E_FAIL;



	if (FAILED(Make_ChildUI(182.f, -140.f, 20.f, 20.f, TEXT("Prototype_GameObject_NumMental"))))
		return E_FAIL;

	vector<CUI*>::iterator iter = m_pChildUIs.begin();
	iter += 2;
	m_pNumMental = (CNumMental*)(*iter);



	if (FAILED(Make_ChildUI(524.5f, 19.168f, 530.f * 0.7f, 820.f * 0.7f, TEXT("Prototype_GameObject_VendorText"))))
		return E_FAIL;

	return S_OK;
}

void CStreetVendor2::Tick(_float fTimeDelta)
{
	if (UM->Get_Vendor2Select())
	{
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(59.019f,2.5f, 154.220f);
		CamDesc.vPos.z -= 1.f;
		CamDesc.vPos.y -= 1.f;
		CamDesc.vLook = _float3(59.019f, 1.7f, 156.220f);
		//CamDesc.vLook.y += 1.f;
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 3.f;
		CamDesc.fLookLerpSpeed = 3.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);

		m_pNumMental->Set_Number(UM->Get_Food(2));


		if (m_bSelectPage)
		{
			if (CK_DOWN(DIK_RIGHT))
			{
				GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
				m_iCountItem++;
			}
			else if (CK_DOWN(DIK_LEFT))
			{
				if (m_iCountItem == 0)
				{
					m_iCountItem = 0;
				}
				else
				{
					GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
					m_iCountItem--;
				}
			}
			if (CK_DOWN(DIK_RETURN))
			{
				GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
				if (m_iSelectItem == 0)
				{
					UM->Gain_Food(2, m_iCountItem);
					UM->Use_Money(m_iCountItem * 100);
					m_iCountItem = 0;
				}
			}
		}

		//else if (!m_bSelectPage)
		//{
		//	if (CK_DOWN(DIK_UP))
		//	{
		//		if (m_iSelectItem == 0)
		//		{
		//			m_iSelectItem = 0;
		//		}
		//		else
		//		{
		//			GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
		//			m_iSelectItem--;
		//		}

		//	}
		//	else if (CK_DOWN(DIK_DOWN))
		//	{
		//		if (m_iSelectItem == 0)
		//		{
		//			m_iSelectItem = 0;
		//		}
		//		else
		//		{
		//			GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
		//			m_iSelectItem++;
		//		}
		//	}
		//}

		if (CK_DOWN(DIK_RETURN) && m_bSelectPage == true)
		{
			GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
			m_bSelectPage = false;
			UM->Select_Off();
		}
		
		else if (CK_DOWN(DIK_RETURN) && m_bSelectPage == false)
		{
			GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
			m_bSelectPage = true;
			UM->Select_On();
		}

		else if (CK_DOWN(DIK_BACK) && m_bSelectPage == true)
		{
			GI->PlaySoundW(L"UI_Cancel.wav", SD_UI, 0.6f);
			m_bSelectPage = false;
			UM->Select_Off();
		}

		if (m_iSelectItem == 0)
			UM->Set_VendorText(2);

		if (UM->Get_Vendor2Select())
		{
			//UI_InputDebug(fTimeDelta);

			__super::Tick(fTimeDelta);
		}

		UM->Set_ShopItemCount(m_iCountItem);
		UM->Set_ShopItem(m_iSelectItem);
	}
}

void CStreetVendor2::LateTick(_float fTimeDelta)
{
	if (UM->Get_Vendor2Select())
	{
		__super::LateTick(fTimeDelta);
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CStreetVendor2::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 2)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;

	if (UM->Get_Vendor2Select())
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;




		//UI_RenderDebug();

		// 자식들의 Render 호출
		__super::Render();
	}

	return S_OK;
}






HRESULT CStreetVendor2::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_VendorBackground"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CStreetVendor2::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CStreetVendor2 * CStreetVendor2::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStreetVendor2*		pInstance = new CStreetVendor2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStreetVendor2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStreetVendor2::Clone(void * pArg)
{
	CStreetVendor2*		pInstance = new CStreetVendor2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStreetVendor2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CStreetVendor2::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
