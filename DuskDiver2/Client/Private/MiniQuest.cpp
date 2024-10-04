#include "stdafx.h"
#include "..\Public\MiniQuest.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Player_Manager.h"
#include "ToolManager.h"



CMiniQuest::CMiniQuest(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CMiniQuest::CMiniQuest(const CMiniQuest & rhs)
	: CUI(rhs)
{
}

HRESULT CMiniQuest::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiniQuest::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CMiniQuest::Tick(_float fTimeDelta)
{
	if (TM->Get_CurLevel() == 3)
	{
		m_UiInfo.fX = UM->Get_QuestPos().x * 10.7f;
		m_UiInfo.fY = UM->Get_QuestPos().z * -9.f;

	}
	if (TM->Get_CurLevel() == 6)
	{
		m_UiInfo.fX = UM->Get_QuestPos().x * -8.0f - 70.f;
		m_UiInfo.fY = UM->Get_QuestPos().z * 8.f - 40.f;
	}

	if (TM->Get_CurLevel() == 7)
	{
		m_UiInfo.fX = UM->Get_QuestPos().x * 2.2f - 100.f;
		m_UiInfo.fY = UM->Get_QuestPos().z * -2.2f + 185.f;
	}

	if (TM->Get_CurLevel() == 8)
	{
		m_UiInfo.fX = UM->Get_QuestPos().x * 1.4f * 0.95f + 5.f * 0.9f;
		m_UiInfo.fY = UM->Get_QuestPos().z * -1.4f * 0.95f + 200.f * 0.9f;
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

	__super::Tick(fTimeDelta);
}

void CMiniQuest::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMiniQuest::Render()
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


	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;

	if (!UM->Get_QuestOn() && !UM->PartyOn_State() && UM->Get_FrontUI() && !UM->Get_ShopState() && UM->Get_QuestPointRenderState())
	{
		if (sqrt((UM->Get_QuestPos().x - PM->Get_PlayerPos().x) * (UM->Get_QuestPos().x - PM->Get_PlayerPos().x)
			+ (UM->Get_QuestPos().z - PM->Get_PlayerPos().z) * (UM->Get_QuestPos().z - PM->Get_PlayerPos().z)) < 6.5f)
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






HRESULT CMiniQuest::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniQuest"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CMiniQuest::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CMiniQuest * CMiniQuest::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMiniQuest*		pInstance = new CMiniQuest(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMiniQuest"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMiniQuest::Clone(void * pArg)
{
	CMiniQuest*		pInstance = new CMiniQuest(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMiniQuest"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CMiniQuest::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
