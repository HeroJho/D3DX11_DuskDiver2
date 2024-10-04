#include "stdafx.h"
#include "..\Public\PartyOn_Contents.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "ToolManager.h"
#include "Monster_Manager.h"


CPartyOn_Contents::CPartyOn_Contents(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CPartyOn_Contents::CPartyOn_Contents(const CPartyOn_Contents & rhs)
	: CUI(rhs)
{
}

HRESULT CPartyOn_Contents::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPartyOn_Contents::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	m_iTextNum = 0;

	return S_OK;
}

void CPartyOn_Contents::Tick(_float fTimeDelta)
{
	if (CK_DOWN(DIK_RETURN) && 0 == m_iTextNum && m_bOnce)
		m_iTextNum = 1;

	if (!UM->PartyOn_State())
	{
		m_fPop = 0.f;
		m_bOn = true;
	}

	if (UM->PartyOn_State() && !m_bOnce && m_bCheck && m_bOn)
	{
		m_fPop += 1.5f * fTimeDelta;

		if (1.f <= m_fPop)
		{
			m_bOn = false;
			m_fPop = 1.f;
			m_bCheck = false;
		}
	}

	if (UM->LEVIADA_State() && !UM->PartyOn_State())
	{
		m_bCheck = true;
	}

	if (UM->PartyOn_State() && m_bOnce && !m_bOnce2 && m_bOn)
	{
		m_fPop += 1.5f * fTimeDelta;

		if (1.f <= m_fPop)
		{
			m_bOn = false;
			m_fPop = 1.f;
		}
	}

	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CPartyOn_Contents::LateTick(_float fTimeDelta)
{
	if (CK_DOWN(DIK_RETURN) && UM->LEVIADA_State() && !m_bOnce)
		m_bOnce = true;

	else if (CK_DOWN(DIK_RETURN) && UM->DD_State() && !m_bOnce2)
	{
		m_bOnce2 = true;
		//GI->PlaySound_Linear(L"BGM_Day.wav", SD_BGM, 0.3f, true, 0.1f);
	}

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CPartyOn_Contents::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_QuestGaugeAcc", &m_fPop, sizeof(_float));


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", m_iTextNum)))
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






HRESULT CPartyOn_Contents::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_PartyOn_Contents"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CPartyOn_Contents::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CPartyOn_Contents * CPartyOn_Contents::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPartyOn_Contents*		pInstance = new CPartyOn_Contents(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPartyOn_Contents"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPartyOn_Contents::Clone(void * pArg)
{
	CPartyOn_Contents*		pInstance = new CPartyOn_Contents(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPartyOn_Contents"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CPartyOn_Contents::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
