#include "stdafx.h"
#include "..\Public\StoryQuest_Point.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "ToolManager.h"



CStoryQuest_Point::CStoryQuest_Point(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CStoryQuest_Point::CStoryQuest_Point(const CStoryQuest_Point & rhs)
	: CUI(rhs)
{
}

HRESULT CStoryQuest_Point::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStoryQuest_Point::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();




	return S_OK;
}

void CStoryQuest_Point::Tick(_float fTimeDelta)
{
	//UI_InputDebug(fTimeDelta);

	if (!UM->Get_QuestOn())
	{
		m_fOnAcc = 0.f;
		m_bOn = true;
	}

	else
	{
		if (m_bOn)
		{
			m_fOnAcc += 2.5f * fTimeDelta;

			if (1.f <= m_fOnAcc)
			{
				m_bOn = false;
				m_fOnAcc = 1.f;
			}
		}

		if (UM->Get_QuestOff())
		{
			m_fOnAcc -= 3.f * fTimeDelta;

			if (0.f >= m_fOnAcc)
				m_fOnAcc = 0.f;
		} 
	}


	__super::Tick(fTimeDelta);
}

void CStoryQuest_Point::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CStoryQuest_Point::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_QuestGaugeAcc", &m_fOnAcc, sizeof(_float));


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






HRESULT CStoryQuest_Point::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StoryQuest_Point"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CStoryQuest_Point::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CStoryQuest_Point * CStoryQuest_Point::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStoryQuest_Point*		pInstance = new CStoryQuest_Point(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStoryQuest_Point"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStoryQuest_Point::Clone(void * pArg)
{
	CStoryQuest_Point*		pInstance = new CStoryQuest_Point(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStoryQuest_Point"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CStoryQuest_Point::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
