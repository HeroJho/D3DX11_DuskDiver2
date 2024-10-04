#include "stdafx.h"
#include "..\Public\LevelUpName2.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"


CLevelUpName2::CLevelUpName2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CLevelUpName2::CLevelUpName2(const CLevelUpName2 & rhs)
	: CUI(rhs)
{
}

HRESULT CLevelUpName2::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevelUpName2::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();




	return S_OK;
}

void CLevelUpName2::Tick(_float fTimeDelta)
{
	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CLevelUpName2::LateTick(_float fTimeDelta)
{
	if (UM->Get_BackNum() == 1)
	{
		if (m_iCurrentNum != 1 && UM->Get_LevelUpSelectPageNum() == 2)
			m_bAppear = true;

		else if (UM->Get_LevelUpSelectPageNum() != 2)
		{
			m_bAppear = false;
			m_UiInfo.fX = 30.f;
			m_UiInfo.fY = 91.6f;
			m_bCut = false;
		}
		if (m_bAppear)
		{
			if (!m_bCut)
			{
				m_UiInfo.fX = 10.f;
				m_UiInfo.fY = 111.6f;
				m_bCut = true;
			}
			else
			{
				if (m_UiInfo.fX >= 30.f)
				{
					m_UiInfo.fX = 30.f;
					m_UiInfo.fY = 91.6f;
				}
				else
				{
					m_UiInfo.fX += 2.5f;
					m_UiInfo.fY -= 2.5f;
				}
			}
		}
	}

	if (UM->Get_BackNum() == 3)
	{
		if (m_iCurrentNum != 1 && UM->Get_EquipSelectPageNum() == 2)
			m_bAppear = true;

		else if (UM->Get_EquipSelectPageNum() != 2)
		{
			m_bAppear = false;
			m_UiInfo.fX = 30.f;
			m_UiInfo.fY = 91.6f;
			m_bCut = false;
		}
		if (m_bAppear)
		{
			if (!m_bCut)
			{
				m_UiInfo.fX = 10.f;
				m_UiInfo.fY = 111.6f;
				m_bCut = true;
			}
			else
			{
				if (m_UiInfo.fX >= 30.f)
				{
					m_UiInfo.fX = 30.f;
					m_UiInfo.fY = 91.6f;
				}
				else
				{
					m_UiInfo.fX += 2.5f;
					m_UiInfo.fY -= 2.5f;
				}
			}
		}
	}

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CLevelUpName2::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (UM->Get_BackNum() == 1)
	{
		if (UM->Get_LevelUpSelectPageNum() == 2)
		{
			if (FAILED(m_pShaderCom->Begin(2)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pShaderCom->Begin(6)))
				return E_FAIL;
		}
	}

	else if (UM->Get_BackNum() == 3)
	{
		if (UM->Get_EquipSelectPageNum() == 2)
		{
			if (FAILED(m_pShaderCom->Begin(2)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pShaderCom->Begin(6)))
				return E_FAIL;
		}
	}

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	//UI_RenderDebug();


	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CLevelUpName2::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LevelUpName2"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}









void CLevelUpName2::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CLevelUpName2 * CLevelUpName2::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevelUpName2*		pInstance = new CLevelUpName2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevelUpName2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLevelUpName2::Clone(void * pArg)
{
	CLevelUpName2*		pInstance = new CLevelUpName2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CLevelUpName2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CLevelUpName2::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
