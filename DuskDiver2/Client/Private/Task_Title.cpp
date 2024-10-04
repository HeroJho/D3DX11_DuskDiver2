#include "stdafx.h"
#include "..\Public\Task_Title.h"
#include "GameInstance.h"


#include "ToolManager.h"



CTask_Title::CTask_Title(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CTask_Title::CTask_Title(const CTask_Title & rhs)
	: CUI(rhs)
{
}

HRESULT CTask_Title::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTask_Title::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();




	return S_OK;
}

void CTask_Title::Tick(_float fTimeDelta)
{
	if (m_bQuest)
	{
		m_fQuestGaugeAcc += 0.5f * fTimeDelta;

		if (1.f <= m_fQuestGaugeAcc)
		{
			m_bQuest = false;
			m_fQuestGaugeAcc = 1.f;
		}
	}
	
	else
	{
		m_fQuestGaugeAcc -= 0.5f * fTimeDelta;

		if (0.f >= m_fQuestGaugeAcc)
		{
			m_bQuest = true;
			m_fQuestGaugeAcc = 0.f;
		}
	}

	//UI_InputDebug(fTimeDelta);



	__super::Tick(fTimeDelta);
}

void CTask_Title::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CTask_Title::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_QuestGaugeAcc", &m_fQuestGaugeAcc, sizeof(_float));


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	//UI_RenderDebug();


	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}

HRESULT CTask_Title::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Task_Title"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CTask_Title::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CTask_Title * CTask_Title::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTask_Title*		pInstance = new CTask_Title(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTask_Title"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTask_Title::Clone(void * pArg)
{
	CTask_Title*		pInstance = new CTask_Title(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTask_Title"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CTask_Title::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
