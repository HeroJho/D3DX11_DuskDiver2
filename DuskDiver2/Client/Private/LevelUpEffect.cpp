#include "stdafx.h"
#include "..\Public\LevelUpEffect.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"



CLevelUpEffect::CLevelUpEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CLevelUpEffect::CLevelUpEffect(const CLevelUpEffect & rhs)
	: CUI(rhs)
{
}

HRESULT CLevelUpEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevelUpEffect::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CLevelUpEffect::Tick(_float fTimeDelta)
{
	if (UM->Get_LevelUpEffect())
	{
		if (m_fTime >= 1.f)
		{
			m_fTime = 0.f;
			UM->Set_LevelUpEffect(false);
			m_UiInfo.fSizeX = 602.f;
			m_UiInfo.fSizeY = 60.f;
		}
		else
		{
			m_fTime += fTimeDelta * 0.4f;
			m_UiInfo.fSizeX += 0.7f;
			m_UiInfo.fSizeY += 3.5f;
		}
	}
	else
	{
		m_fTime = 0.f;
	}


	switch (UM->Get_StatusNum())
	{
	case 1:
		m_fYPos = -198.f;
		break;
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

	m_UiInfo.fY = m_fYPos;


	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CLevelUpEffect::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CLevelUpEffect::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_TimeDelta", &m_fTime, sizeof(_float));


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(7)))
		return E_FAIL;


	if(UM->Get_LevelUpEffect())
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}




	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CLevelUpEffect::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StatusSelectBar"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CLevelUpEffect::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CLevelUpEffect * CLevelUpEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevelUpEffect*		pInstance = new CLevelUpEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevelUpEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLevelUpEffect::Clone(void * pArg)
{
	CLevelUpEffect*		pInstance = new CLevelUpEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CLevelUpEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CLevelUpEffect::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
