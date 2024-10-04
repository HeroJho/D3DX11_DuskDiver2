#include "stdafx.h"
#include "..\Public\Player3Icon.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"


CPlayer3Icon::CPlayer3Icon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CPlayer3Icon::CPlayer3Icon(const CPlayer3Icon & rhs)
	: CUI(rhs)
{
}

HRESULT CPlayer3Icon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer3Icon::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();




	return S_OK;
}

void CPlayer3Icon::Tick(_float fTimeDelta)
{


	//UI_InputDebug(fTimeDelta);

	m_bDD = UM->DD_State();

	if (m_bDD)
		__super::Tick(fTimeDelta);
}

void CPlayer3Icon::LateTick(_float fTimeDelta)
{
	if (UM->Get_BackNum() == 2)
	{
		if (UM->Get_PlayerSelectNum() == 3)
		{
			m_bSelect = true;
		}
		else if (UM->Get_PlayerSelectNum() != 3)
		{
			m_bSelect = false;
			m_UiInfo.fSizeX = 51.2f;
			m_UiInfo.fSizeY = 51.2f;
		}

		if (m_bSelect)
		{
			if (m_UiInfo.fSizeX >= 70.f)
			{
				m_UiInfo.fSizeX = 70.f;
				m_UiInfo.fSizeY = 70.f;
			}
			else
			{
				m_UiInfo.fSizeX += 2.5f;
				m_UiInfo.fSizeY += 2.5f;
			}
		}
	}
	else if (UM->Get_BackNum() == 4)
	{
		if (UM->Get_EquipmentPlayer() == 3)
		{
			m_bSelect = true;
		}
		else if (UM->Get_EquipmentPlayer() != 3)
		{
			m_bSelect = false;
			m_UiInfo.fSizeX = 51.2f;
			m_UiInfo.fSizeY = 51.2f;
		}

		if (m_bSelect)
		{
			if (m_UiInfo.fSizeX >= 70.f)
			{
				m_UiInfo.fSizeX = 70.f;
				m_UiInfo.fSizeY = 70.f;
			}
			else
			{
				m_UiInfo.fSizeX += 2.5f;
				m_UiInfo.fSizeY += 2.5f;
			}
		}
	}


	if (m_bDD)
		__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CPlayer3Icon::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	//UI_RenderDebug();


	// 자식들의 Render 호출



	if (m_bDD)
		__super::Render();

	return S_OK;
}






HRESULT CPlayer3Icon::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player3Icon"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}







void CPlayer3Icon::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CPlayer3Icon * CPlayer3Icon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer3Icon*		pInstance = new CPlayer3Icon(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer3Icon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer3Icon::Clone(void * pArg)
{
	CPlayer3Icon*		pInstance = new CPlayer3Icon(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer3Icon"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CPlayer3Icon::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
