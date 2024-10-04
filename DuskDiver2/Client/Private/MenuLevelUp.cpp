#include "stdafx.h"
#include "..\Public\MenuLevelup.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"



CMenuLevelUp::CMenuLevelUp(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CMenuLevelUp::CMenuLevelUp(const CMenuLevelUp & rhs)
	: CUI(rhs)
{
}

HRESULT CMenuLevelUp::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMenuLevelUp::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();




	return S_OK;
}

void CMenuLevelUp::Tick(_float fTimeDelta)
{
	m_iCurstate = UM->Get_MenuNum();



	//UI_InputDebug(fTimeDelta);



	__super::Tick(fTimeDelta);
}

void CMenuLevelUp::LateTick(_float fTimeDelta)
{
	if (m_iCurstate != 1 && UM->Get_MenuNum() == 1)
	{
		m_bSizeUp = true;
	}
	else if (UM->Get_MenuNum() != 1)
	{
		m_UiInfo.fSizeX = 304.f * 0.7f;
		m_UiInfo.fSizeY = 99.f * 0.7f;
		m_bSizeUp = false;
		m_fSizeUp = 0.f;
	}

	if (m_bSizeUp)
	{
		m_fSizeUp += fTimeDelta;
		if (m_fSizeUp < 0.2f)
		{
			m_UiInfo.fSizeX += 1.4f;
			m_UiInfo.fSizeY += 1.4f;
		}
		else if (0.2f < m_fSizeUp && m_fSizeUp < 0.35f)
		{
			m_UiInfo.fSizeX -= 1.4f;
			m_UiInfo.fSizeY -= 1.4f;
		}
	}


	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMenuLevelUp::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (UM->Get_MenuNum() == 1)
	{
		if (FAILED(m_pShaderCom->Begin(4)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Begin(0)))
			return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	//UI_RenderDebug();


	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CMenuLevelUp::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LevelUp"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}








void CMenuLevelUp::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CMenuLevelUp * CMenuLevelUp::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMenuLevelUp*		pInstance = new CMenuLevelUp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMenuLevelUp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMenuLevelUp::Clone(void * pArg)
{
	CMenuLevelUp*		pInstance = new CMenuLevelUp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMenuLevelUp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CMenuLevelUp::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
