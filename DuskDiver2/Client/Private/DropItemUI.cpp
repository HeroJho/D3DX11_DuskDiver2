#include "stdafx.h"
#include "..\Public\DropItemUI.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "NumWaper.h"

#include "ToolManager.h"



CDropItemUI::CDropItemUI(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CDropItemUI::CDropItemUI(const CDropItemUI & rhs)
	: CUI(rhs)
{
}

HRESULT CDropItemUI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDropItemUI::Initialize(void* pArg)
{

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	m_iIndexNum = *(_int*)(m_UiInfo.pDesc);

	if (m_iIndexNum == 0)
	{
		m_iRandom = GI->Get_Random(1, 25);
		UM->Gain_Money(m_iRandom);
	}

	UM->Gain_DropItemNum();
	m_iItemNum = UM->Get_DropItemNum();
	m_iOriginNum = m_iItemNum;

	if (m_iItemNum > 4 )
	{
		UM->DropItem_On();
	}
	else if (UM->Get_DropItem4On())
	{
		if (m_iItemNum == 1)
		{
			UM->DropItem_On();
			UM->DropItem4_Off();
		}
		else
		{
			m_iItemNum = 5;
			UM->DropItem_On();
			UM->DropItem4_Off();
		}
	}

	m_UiInfo.fSizeX = 673.f * 0.4f;
	m_UiInfo.fSizeY = 79.f * 0.4f;

	m_fOriginPosx = m_UiInfo.fX;

	if (m_iItemNum == 1)
	{
		m_UiInfo.fY = 311.1f;
	}
	else if (m_iItemNum == 2)
	{
		m_UiInfo.fY =  346.1f;
	}
	else if (m_iItemNum == 3)
	{
		m_UiInfo.fY = 381.1f;
	}
	else if (m_iItemNum == 4)
	{
		m_UiInfo.fY = 416.1f;
	}
	else
	{
		m_UiInfo.fY = 451.1f;
	}

	if (FAILED(Make_ChildUI(90.f, 0.f, 18.f, 18.f, TEXT("Prototype_GameObject_NumWaper"))))
		return E_FAIL;

	vector<CUI*>::iterator iter = m_pChildUIs.begin();
	m_pUINum = (CNumWaper*)(*iter);

	return S_OK;
}

void CDropItemUI::Tick(_float fTimeDelta)
{
	m_fTime += fTimeDelta;
	m_fTime2 += fTimeDelta;

	if (m_iIndexNum == 0)
	{
		m_pUINum->Set_Number(m_iRandom);
	}

	if (m_fTime > 5.f && !m_bVainish)
	{
		m_bVainish = true;
		UM->Erase_DropItemNum();
		UM->DropItemVanish_On();
		m_bVanishOn = true;
	}

	if (m_iItemNum == 1)
	{
		if (m_UiInfo.fY <= 311.1f)
			m_UiInfo.fY = 311.1f;
		else
		{
			m_fItemNumTime += fTimeDelta;
			if (m_fItemNumTime > 0.01f)
			{
				m_UiInfo.fY -= 2.5f;
				m_fItemNumTime = 0.f;
			}
		}
	}
	else if (m_iItemNum == 2)
	{
		if (m_UiInfo.fY <= 346.1f)
			m_UiInfo.fY = 346.1f;
		else
		{
			m_fItemNumTime += fTimeDelta;
			if (m_fItemNumTime > 0.01f)
			{
				m_UiInfo.fY -= 2.5f;
				m_fItemNumTime = 0.f;
			}
		}
	}
	else if (m_iItemNum == 3)
	{
		if (m_UiInfo.fY <= 381.1f)
			m_UiInfo.fY = 381.1f;
		else
		{
			m_fItemNumTime += fTimeDelta;
			if (m_fItemNumTime > 0.01f)
			{
				m_UiInfo.fY -= 2.5f;
				m_fItemNumTime = 0.f;
			}
		}
	}
	else if (m_iItemNum == 4)
	{
		if (m_UiInfo.fY <= 416.1f)
			m_UiInfo.fY = 416.1f;
		else
		{
			m_fItemNumTime += fTimeDelta;
			if (m_fItemNumTime > 0.01f)
			{
				m_UiInfo.fY -= 2.5f;
				m_fItemNumTime = 0.f;
			}
		}
	}
	else if (m_iItemNum == 5)
	{
		if (m_UiInfo.fY <= 451.1f)
			m_UiInfo.fY = 451.1f;
		else
		{
			m_fItemNumTime += fTimeDelta;
			if (m_fItemNumTime > 0.01f)
			{
				m_UiInfo.fY -= 2.5f;
				m_fItemNumTime = 0.f;
			}
		}
	}
	else
	{
		if (m_UiInfo.fY <= 311.1f)
			m_UiInfo.fY = 311.1f;
		else
		{
			m_fItemNumTime += fTimeDelta;
			if (m_fItemNumTime > 0.01f)
			{
				m_UiInfo.fY -= 2.5f;
				m_fItemNumTime = 0.f;
			}
		}
	}

	if (m_UiInfo.fX <= m_fOriginPosx - 30.f)
		m_UiInfo.fX = m_fOriginPosx - 30.f;
	else
	{
		m_fItemNumTime += fTimeDelta;
		if (m_fItemNumTime > 0.01f)
		{
			m_UiInfo.fX -= 2.5f;
			m_fItemNumTime = 0.f;
		}
	}


	if (UM->Get_DropItemOn())
	{
		if (m_iItemNum == 1)
		{
			m_bVainish = true;
			UM->Erase_DropItemNum();
		}
		else if(m_iItemNum == 5)
		{
			m_iItemNum--;
			UM->DropItem4_On();
		}
		else
			m_iItemNum--;
	}

	if (UM->Get_DropItemVanish())
	{
		m_iItemNum--;
		//UM->DropItemVanish_Off();
	}

	if (m_bVainish)
	{
		m_fVanishTime += fTimeDelta;
		if (m_fVanishTime > 0.01f)
		{
			m_UiInfo.fX -= 5.25f;
			m_UiInfo.fSizeX += 10.5f;
			m_UiInfo.fSizeY -= 7.f;
			m_fVanishTime = 0.f;
		}
		if (m_UiInfo.fSizeY <= 0.f)
		{
			if (m_iOriginNum == 5 && m_bVanishOn)
			{
				UM->DropItem4_Off();
				UM->DropItemVanish_Off();
				m_bVanishOn = false;
				//UM->Gain_DropItemNum();
			}
			Set_Dead();
		}

	}


	//UI_InputDebug(fTimeDelta);

	if (!m_bVainish)
	{
		__super::Tick(fTimeDelta);
	}
}

void CDropItemUI::LateTick(_float fTimeDelta)
{
	if (UM->Get_DropItemOn())
	{
		UM->DropItem_Off();
	}

	if (UM->Get_DropItemVanish())
	{
		UM->DropItemVanish_Off();
	}

	if (!m_bVainish)
	{
		__super::LateTick(fTimeDelta);
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CDropItemUI::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_TimeDelta", &m_fAlphaTime, sizeof(_float));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", m_iIndexNum)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(44)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;




	//UI_RenderDebug();

	// 자식들의 Render 호출

	if (!m_bVainish)
	{
		__super::Render();
	}

	return S_OK;
}






HRESULT CDropItemUI::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DropItemUI"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CDropItemUI::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CDropItemUI * CDropItemUI::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDropItemUI*		pInstance = new CDropItemUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CDropItemUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDropItemUI::Clone(void * pArg)
{
	CDropItemUI*		pInstance = new CDropItemUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CDropItemUI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CDropItemUI::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
