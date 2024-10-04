#include "stdafx.h"
#include "..\Public\EquipmentSelectCase.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"



CEquipmentSelectCase::CEquipmentSelectCase(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CEquipmentSelectCase::CEquipmentSelectCase(const CEquipmentSelectCase & rhs)
	: CUI(rhs)
{
}

HRESULT CEquipmentSelectCase::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEquipmentSelectCase::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CEquipmentSelectCase::Tick(_float fTimeDelta)
{
	m_iCurNum = UM->Get_EquipmentClassSelect();

	
	if (UM->Get_EquipmentState() == 0)
	{
		m_UiInfo.fSizeX = 420.f;
		m_UiInfo.fSizeY = 117.f;
		m_bRend = true;
	}
	else if (UM->Get_EquipmentState() == 1)
	{
		if (m_UiInfo.fSizeX <= 405.f)
		{
			m_UiInfo.fSizeX = 405.f;
			m_bRend = false;
		}
		else
		{
			m_UiInfo.fSizeX -= 1.5f;
			m_UiInfo.fSizeY -= 10.f;
		}
	}

	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CEquipmentSelectCase::LateTick(_float fTimeDelta)
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

	m_fYPos;

	if (m_fCountRender > 0.2f)
	{
		m_UiInfo.fY = m_fYPos;
	}

	if (!m_bRender)
	{
		m_fCountRender += fTimeDelta;
		if (m_fCountRender > 0 && m_fCountRender <= 0.2f)
		{
			m_fBlend += fTimeDelta;
		}
		else
		{
			m_fBlend -= fTimeDelta;
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

HRESULT CEquipmentSelectCase::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_TimeDelta", &m_fBlend, sizeof(_float));


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(7)))
		return E_FAIL;

	if (m_bRend)
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}




	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CEquipmentSelectCase::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EquipmentSelectCase"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CEquipmentSelectCase::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CEquipmentSelectCase * CEquipmentSelectCase::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEquipmentSelectCase*		pInstance = new CEquipmentSelectCase(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEquipmentSelectCase"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEquipmentSelectCase::Clone(void * pArg)
{
	CEquipmentSelectCase*		pInstance = new CEquipmentSelectCase(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEquipmentSelectCase"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CEquipmentSelectCase::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
