#include "stdafx.h"
#include "..\Public\Player1StaminaLv.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"



CPlayer1StaminaLv::CPlayer1StaminaLv(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CPlayer1StaminaLv::CPlayer1StaminaLv(const CPlayer1StaminaLv & rhs)
	: CUI(rhs)
{
}

HRESULT CPlayer1StaminaLv::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer1StaminaLv::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();




	return S_OK;
}

void CPlayer1StaminaLv::Tick(_float fTimeDelta)
{
	//UI_InputDebug(fTimeDelta);



	__super::Tick(fTimeDelta);
}

void CPlayer1StaminaLv::LateTick(_float fTimeDelta)
{
	if (UM->Get_StatusNum() != 2)
	{
		if (UM->Get_PlayerSelectNum() == 1)
			m_iStat = UM->Get_Player1Stamina();
		else if (UM->Get_PlayerSelectNum() == 2)
			m_iStat = UM->Get_Player2Stamina();
		else if (UM->Get_PlayerSelectNum() == 3)
			m_iStat = UM->Get_Player3Stamina();
	}
	else
	{
		if (UM->Get_PlayerSelectNum() == 1)
			m_iStat = UM->Get_Player1Stamina() + UM->Get_Reserver();
		else if (UM->Get_PlayerSelectNum() == 2)
			m_iStat = UM->Get_Player2Stamina() + UM->Get_Reserver();
		else if (UM->Get_PlayerSelectNum() == 3)
			m_iStat = UM->Get_Player3Stamina() + UM->Get_Reserver();
	}


	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CPlayer1StaminaLv::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));


	for (int i = 1; i < 21; i++)
	{
		if (m_iStat == i)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", i)))
				return E_FAIL;
		}
	}


	if (UM->Get_StatusNum() != 2)
	{
		if (FAILED(m_pShaderCom->Begin(2)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Begin(9)))
			return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	//UI_RenderDebug();


	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CPlayer1StaminaLv::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Lv"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}







void CPlayer1StaminaLv::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CPlayer1StaminaLv * CPlayer1StaminaLv::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer1StaminaLv*		pInstance = new CPlayer1StaminaLv(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer1StaminaLv"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer1StaminaLv::Clone(void * pArg)
{
	CPlayer1StaminaLv*		pInstance = new CPlayer1StaminaLv(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer1StaminaLv"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CPlayer1StaminaLv::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
