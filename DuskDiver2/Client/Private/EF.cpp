#include "stdafx.h"
#include "..\Public\EF.h"
#include "GameInstance.h"
#include "Player_Manager.h"
#include "ToolManager.h"
#include "Status_Manager.h"
#include "UI_Manager.h"



CEF::CEF(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CEF::CEF(const CEF & rhs)
	: CUI(rhs)
{
}

HRESULT CEF::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEF::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();




	return S_OK;
}

void CEF::Tick(_float fTimeDelta)
{
	//플레이어1
	if (0 == PM->Get_SelectPlayer())
	{
		//꽉찼을때 속도
		if (100 <= PM->Get_PlayerEP(CPlayer::PLAYER_01))
			m_fEFSpeed = 3.5f;

		else
			m_fEFSpeed = 1.5f;
	}

	//플레이어3
	else if (1 == PM->Get_SelectPlayer())
	{
		//꽉찼을때 속도
		if (100 <= PM->Get_PlayerEP(CPlayer::PLAYER_03))
			m_fEFSpeed = 3.5f;

		else
			m_fEFSpeed = 1.5f;
	}

	//플레이어4
	else if (2 == PM->Get_SelectPlayer())
	{
		//꽉찼을때 속도
		if (100 <= PM->Get_PlayerEP(CPlayer::PLAYER_04))
			m_fEFSpeed = 3.5f;

		else
			m_fEFSpeed = 1.5f;
	}

	if (m_bEF)
	{
		m_fEFAcc += m_fEFSpeed * fTimeDelta;

		if (1.f <= m_fEFAcc)
		{
			m_bEF = false;
			m_fEFAcc = 1.f;
		}
	}

	else
	{
		m_fEFAcc -= m_fEFSpeed * fTimeDelta;

		if (0.f >= m_fEFAcc)
		{
			m_bEF = true;
			m_fEFAcc = 0.f;
		}
	}
	
	//UI_InputDebug(fTimeDelta);



	__super::Tick(fTimeDelta);
}

void CEF::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);
	

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CEF::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_QuestGaugeAcc", &m_fEFAcc, sizeof(_float));


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(14)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	//UI_RenderDebug();


	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CEF::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EF"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}


void CEF::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CEF * CEF::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEF*		pInstance = new CEF(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEF"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEF::Clone(void * pArg)
{
	CEF*		pInstance = new CEF(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEF"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CEF::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
