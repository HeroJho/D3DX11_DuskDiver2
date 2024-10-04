#include "stdafx.h"
#include "..\Public\Player1StrLv.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"



CPlayer1StrLv::CPlayer1StrLv(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CPlayer1StrLv::CPlayer1StrLv(const CPlayer1StrLv & rhs)
	: CUI(rhs)
{
}

HRESULT CPlayer1StrLv::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer1StrLv::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();




	return S_OK;
}

void CPlayer1StrLv::Tick(_float fTimeDelta)
{
	//UI_InputDebug(fTimeDelta);



	__super::Tick(fTimeDelta);
}

void CPlayer1StrLv::LateTick(_float fTimeDelta)
{
	if (UM->Get_StatusNum() != 1)
	{
		if (UM->Get_PlayerSelectNum() == 1)
			m_iStat = UM->Get_Player1Str();
		else if (UM->Get_PlayerSelectNum() == 2)
			m_iStat = UM->Get_Player2Str();
		else if (UM->Get_PlayerSelectNum() == 3)
			m_iStat = UM->Get_Player3Str();
	}
	else
	{
		if (UM->Get_PlayerSelectNum() == 1)
			m_iStat = UM->Get_Player1Str() + UM->Get_Reserver();
		else if (UM->Get_PlayerSelectNum() == 2)
			m_iStat = UM->Get_Player2Str() + UM->Get_Reserver();
		else if (UM->Get_PlayerSelectNum() == 3)
			m_iStat = UM->Get_Player3Str() + UM->Get_Reserver();
	}


	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CPlayer1StrLv::Render()
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



	if (UM->Get_StatusNum() != 1)
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






HRESULT CPlayer1StrLv::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Lv"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}







void CPlayer1StrLv::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CPlayer1StrLv * CPlayer1StrLv::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer1StrLv*		pInstance = new CPlayer1StrLv(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer1LuckLv"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer1StrLv::Clone(void * pArg)
{
	CPlayer1StrLv*		pInstance = new CPlayer1StrLv(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer1LuckLv"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CPlayer1StrLv::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
