#include "stdafx.h"
#include "..\Public\EscBase.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"
#include "Player_Manager.h"


CEscBase::CEscBase(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CEscBase::CEscBase(const CEscBase & rhs)
	: CUI(rhs)
{
}

HRESULT CEscBase::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEscBase::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	for (_uint i = 0; i < 6; i++)
	{
		UM->Get_Hat(i);
		UM->Get_Clothes(i);
		UM->Get_Deco(i);
	}

	return S_OK;
}

void CEscBase::Tick(_float fTimeDelta)
{
	if (CK_DOWN(DIK_ESCAPE) && !m_bRender)
	{
		m_bRender = true;
		PM->Set_PlayerTick(false);
	}
	else if (CK_DOWN(DIK_ESCAPE) && m_bRender)
	{
		m_bRender = false;
		PM->Set_PlayerTick(true);
	}

	__super::Tick(fTimeDelta);
}

void CEscBase::LateTick(_float fTimeDelta)
{
	
	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CEscBase::Render()
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

	if (m_bRender)
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}



	// 자식들의 Render 호출
	if (m_bRender)
		__super::Render();

	return S_OK;
}






HRESULT CEscBase::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuBackGround"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}







void CEscBase::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	ImGui::End();
}



CEscBase * CEscBase::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEscBase*		pInstance = new CEscBase(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEscBase"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEscBase::Clone(void * pArg)
{
	CEscBase*		pInstance = new CEscBase(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEscBase"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CEscBase::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
