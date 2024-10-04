#include "stdafx.h"
#include "..\Public\TutorialMinimap.h"
#include "GameInstance.h"
#include "Player_Manager.h"

#include "ToolManager.h"



CTutorialMinimap::CTutorialMinimap(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CTutorialMinimap::CTutorialMinimap(const CTutorialMinimap & rhs)
	: CUI(rhs)
{
}

HRESULT CTutorialMinimap::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTutorialMinimap::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	if (FAILED(Make_ChildUI(0.f, 0.f, 30.f, 30.f, TEXT("Prototype_GameObject_MiniPlayer"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(123.05f, -227.7f, 30.f, 30.f, TEXT("Prototype_GameObject_MiniQuest"))))
		return E_FAIL;


	return S_OK;
}

void CTutorialMinimap::Tick(_float fTimeDelta)
{
	if (PM->Get_PlayerIsIdle())
	{
		m_fTime2 += fTimeDelta;
		if (m_fTime2 > 2.f)
		{
			m_bIdle = true;
		}
	}
	else
	{
		m_bIdle = false;
		m_fTime2 = 0.f;
	}

	if (m_bIdle)
	{
		if (m_fTime <= 0)
			m_fTime = 0;
		else
			m_fTime -= fTimeDelta * 0.8f;
	}
	else
	{
		if (m_fTime >= 2.f)
			m_fTime = 2.f;
		else
			m_fTime += fTimeDelta * 0.8f;
	}

	if (TM->Get_CurLevel() == 7)
	{
		m_UiInfo.fSizeX = 588.f * 0.7f;
		m_UiInfo.fSizeY = 766.f * 0.7f;
	}

	if (TM->Get_CurLevel() == 8)
	{
		m_UiInfo.fSizeX = 688.f * 0.9f;
		m_UiInfo.fSizeY = 565.f * 0.9f;
	}

	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CTutorialMinimap::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CTutorialMinimap::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_TimeDelta", &m_fTime, sizeof(_float));


	if (TM->Get_CurLevel() == 3)
	{
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;
	}

	if (TM->Get_CurLevel() == 6)
	{
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 1)))
			return E_FAIL;
	}

	if (TM->Get_CurLevel() == 7)
	{
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 8)))
			return E_FAIL;
	}

	if (TM->Get_CurLevel() == 8)
	{
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 9)))
			return E_FAIL;
	}


	if (FAILED(m_pShaderCom->Begin(30)))
		return E_FAIL;


	if (CK_PRESS(DIK_M))
	{

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
		//UI_RenderDebug();

		// 자식들의 Render 호출
		__super::Render();
	}

	return S_OK;
}






HRESULT CTutorialMinimap::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TutorialMiniMap"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CTutorialMinimap::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CTutorialMinimap * CTutorialMinimap::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTutorialMinimap*		pInstance = new CTutorialMinimap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTutorialMinimap"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTutorialMinimap::Clone(void * pArg)
{
	CTutorialMinimap*		pInstance = new CTutorialMinimap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTutorialMinimap"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CTutorialMinimap::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
