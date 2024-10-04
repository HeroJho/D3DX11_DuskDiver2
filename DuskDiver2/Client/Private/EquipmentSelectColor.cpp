#include "stdafx.h"
#include "..\Public\EquipmentSelectColor.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "ToolManager.h"



CEquipmentSelectColor::CEquipmentSelectColor(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CEquipmentSelectColor::CEquipmentSelectColor(const CEquipmentSelectColor & rhs)
	: CUI(rhs)
{
}

HRESULT CEquipmentSelectColor::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEquipmentSelectColor::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CEquipmentSelectColor::Tick(_float fTimeDelta)
{
	m_iCurNum = UM->Get_EquipmentClassSelect();


	if (UM->Get_EquipmentState() == 1)
	{
		m_fSelectTime += fTimeDelta;
		if (CK_DOWN(DIK_RETURN) && m_fSelectTime > 0.1f)
		{
			m_bRed = true;
		}
	}

	if (m_bRed)
	{
		if (m_fGreen <= 0)
		{
			m_fGreen = 0.5f;
			m_bRed = false;
			m_fSelectTime = 0.f;
		}
		else
			m_fGreen -= fTimeDelta;
	}

	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CEquipmentSelectColor::LateTick(_float fTimeDelta)
{
	if (m_iCurNum != UM->Get_EquipmentClassSelect())
	{
		if (m_iCurNum > UM->Get_EquipmentClassSelect())
		{
			switch (UM->Get_EquipmentClassSelect())
			{
			case 1:
				m_fYPos = -213.4f;
				break;
			case 2:
				m_fYPos = -63.4f;
				break;
			case 3:
				m_fYPos = 95.4f;
				break;
			}
		}
		else
		{
			switch (UM->Get_EquipmentClassSelect())
			{
			case 2:
				m_fYPos = -63.4f;
				break;
			case 3:
				m_fYPos = 95.4f;
				break;
			case 4:
				m_fYPos = 245.4f;
				break;
			}
		}

		m_bRender = false;
	}


	if (m_fCountRender > 0.2f)
	{
		m_UiInfo.fY = m_fYPos;
	}

	if (!m_bRender)
	{
		m_fCountRender += fTimeDelta;
		if (m_fCountRender > 0 && m_fCountRender <= 0.2f)
		{
			m_fBlend += fTimeDelta * 0.9f;
		}
		else
		{
			m_fBlend -= fTimeDelta* 0.9f;
		}
	}


	if (m_fCountRender > 0.4f)
	{
		m_bRender = true;
		m_fCountRender = 0.f;
		m_fBlend = 0.f;
	}


	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CEquipmentSelectColor::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_TimeDelta", &m_fBlend, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_fGreen", &m_fGreen, sizeof(_float));


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (m_bRed)
	{
		if (FAILED(m_pShaderCom->Begin(43)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Begin(7)))
			return E_FAIL;
	}


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;




	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CEquipmentSelectColor::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EquipmentSelectColor"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CEquipmentSelectColor::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CEquipmentSelectColor * CEquipmentSelectColor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEquipmentSelectColor*		pInstance = new CEquipmentSelectColor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEquipmentSelectColor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEquipmentSelectColor::Clone(void * pArg)
{
	CEquipmentSelectColor*		pInstance = new CEquipmentSelectColor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEquipmentSelectColor"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CEquipmentSelectColor::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
