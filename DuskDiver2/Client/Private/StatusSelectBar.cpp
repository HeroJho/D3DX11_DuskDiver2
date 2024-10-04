#include "stdafx.h"
#include "..\Public\StatusSelectBar.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"


CStatusSelectBar::CStatusSelectBar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CStatusSelectBar::CStatusSelectBar(const CStatusSelectBar & rhs)
	: CUI(rhs)
{
}

HRESULT CStatusSelectBar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStatusSelectBar::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();




	return S_OK;
}

void CStatusSelectBar::Tick(_float fTimeDelta)
{

	m_iCurNum = UM->Get_StatusNum();

	//UI_InputDebug(fTimeDelta);


	__super::Tick(fTimeDelta);
}

void CStatusSelectBar::LateTick(_float fTimeDelta)
{
	if (m_iCurNum != UM->Get_StatusNum())
	{
		if (m_iCurNum > UM->Get_StatusNum())
		{
			switch (UM->Get_StatusNum())
			{
			case 1:
				m_fYPos = -198.f;
				break;
			case 2:
				m_fYPos = -132.1f;
				break;
			case 3:
				m_fYPos = -67.1f;
				break;
			case 4:
				m_fYPos = 3.88f;
				break;
			case 5:
				m_fYPos = 68.91f;
				break;
			case 6:
				m_fYPos = 133.1f;
				break;
			}
		}
		else
		{
			switch (UM->Get_StatusNum())
			{
			case 2:
				m_fYPos = -129.1f;
				break;
			case 3:
				m_fYPos = -64.1f;
				break;
			case 4:
				m_fYPos = 6.88f;
				break;
			case 5:
				m_fYPos = 71.91f;
				break;
			case 6:
				m_fYPos = 136.1f;
				break;
			case 7:
				m_fYPos = 206.2f;
				break;
			}
		}

		m_bRender = false;
	}
	float i = m_UiInfo.fSizeX;

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

HRESULT CStatusSelectBar::Render()
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

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	//UI_RenderDebug();


	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CStatusSelectBar::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StatusSelectBar"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}







void CStatusSelectBar::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CStatusSelectBar * CStatusSelectBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStatusSelectBar*		pInstance = new CStatusSelectBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStatusSelectBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStatusSelectBar::Clone(void * pArg)
{
	CStatusSelectBar*		pInstance = new CStatusSelectBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStatusSelectBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CStatusSelectBar::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
