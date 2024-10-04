#include "stdafx.h"
#include "..\Public\EP_Battle.h"
#include "GameInstance.h"
#include "Player_Manager.h"
#include "ToolManager.h"
#include "Status_Manager.h"
#include "UI_Manager.h"


CEP_Battle::CEP_Battle(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CEP_Battle::CEP_Battle(const CEP_Battle & rhs)
	: CUI(rhs)
{
}

HRESULT CEP_Battle::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEP_Battle::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();




	return S_OK;
}

void CEP_Battle::Tick(_float fTimeDelta)
{

	m_fFrameAcc += 20.f * fTimeDelta;

	if (1 <= m_fFrameAcc)
	{
		m_iFrame++;
		m_fFrameAcc = 0.f;

		if (15 == m_iFrame)
			m_iFrame = 0;
	}		


	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CEP_Battle::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CEP_Battle::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", m_iFrame)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;

	if (0 == PM->Get_SelectPlayer())
	{
		if (PM->Get_BurstMode(CPlayer::PLAYER_01))
		{
			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}
	}

	else if (1 == PM->Get_SelectPlayer())
	{
		if (PM->Get_BurstMode(CPlayer::PLAYER_03))
		{
			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}
	}

	else if (2 == PM->Get_SelectPlayer())
	{
		if (PM->Get_BurstMode(CPlayer::PLAYER_04))
		{
			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}
	}

	//UI_RenderDebug();


	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CEP_Battle::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EP_Battle"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}


void CEP_Battle::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CEP_Battle * CEP_Battle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEP_Battle*		pInstance = new CEP_Battle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEP_Battle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEP_Battle::Clone(void * pArg)
{
	CEP_Battle*		pInstance = new CEP_Battle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEP_Battle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CEP_Battle::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
