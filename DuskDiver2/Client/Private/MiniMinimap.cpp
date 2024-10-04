#include "stdafx.h"
#include "..\Public\MiniMinimap.h"
#include "GameInstance.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "ToolManager.h"



CMiniMinimap::CMiniMinimap(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CMiniMinimap::CMiniMinimap(const CMiniMinimap & rhs)
	: CUI(rhs)
{
}

HRESULT CMiniMinimap::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiniMinimap::Initialize(void * pArg)
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

void CMiniMinimap::Tick(_float fTimeDelta)
{
	//UiInfoDesc.fSizeX = 451.f;
	//UiInfoDesc.fSizeY = 624.f;
	//UiInfoDesc.fX = 640.f;	580
	//UiInfoDesc.fY = 360.f;	-120

	if (TM->Get_CurLevel() == 3)
	{
		m_iMapNum = 0;

		m_fPosx = (PM->Get_PlayerPos().x + 20.f) / 40.f;
		m_fPosy = (PM->Get_PlayerPos().z - 30.f) / -60.f;


		m_UiInfo.fX = 1180.f - (m_fPosx - 0.5f) * 451.f - 25.f;
		m_UiInfo.fY = 200.f - (m_fPosy - 0.5f) * 624.f - 3.f;

		UM->Set_MinimapPosx((m_fPosx - 0.5f) * -451.f);
		UM->Set_MinimapPosy((m_fPosy - 0.5f) * -624.f);
	}
	else if (TM->Get_CurLevel() == 6)
	{
		m_iMapNum = 1;

		m_UiInfo.fSizeX = 768.f * 0.7f;
		m_UiInfo.fSizeY = 694.f * 0.7f;

		m_fPosx = (-PM->Get_PlayerPos().x + 23.769f) / 63.769f - 0.02f;
		m_fPosy = (-PM->Get_PlayerPos().z - 32.156f) / -56.863f - 0.15f;

		m_UiInfo.fX = 1180.f - (m_fPosx - 0.5f) * 537.6f - 25.f;
		m_UiInfo.fY = 200.f - (m_fPosy - 0.5f) * 485.8f - 3;

		UM->Set_MinimapPosx((m_fPosx - 0.5f) * -537.6f);
		UM->Set_MinimapPosy((m_fPosy - 0.5f) * -485.8f);
	}


	if (TM->Get_CurLevel() == 7)
	{
		m_iMapNum = 2;

		m_UiInfo.fSizeX = 470.4f;
		m_UiInfo.fSizeY = 612.8f;

		m_fPosx = (PM->Get_PlayerPos().x + 52.f) / 188.f;
		m_fPosy = (PM->Get_PlayerPos().z - 84.86f) / -293.86f + 0.5f;


		m_UiInfo.fX = 1180.f - (m_fPosx - 0.5f) * 470.4f - 25.f;
		m_UiInfo.fY = 200.f - (m_fPosy - 0.5f) * 612.8f;

		UM->Set_MinimapPosx((m_fPosx - 0.5f) * -470.4f);
		UM->Set_MinimapPosy((m_fPosy - 0.5f) * -612.8f);
	}

	if (TM->Get_CurLevel() == 8)
	{
		m_iMapNum = 3;

		m_UiInfo.fSizeX = 688.f * 0.9f;
		m_UiInfo.fSizeY = 565.f * 0.9f;

		m_fPosx = (PM->Get_PlayerPos().x + 240.f) / 456.4f - 0.02f;
		m_fPosy = (PM->Get_PlayerPos().z - 67.f) / -434.2f + 0.7f;


		m_UiInfo.fX = 1180.f - (m_fPosx - 0.5f) * 688.f * 0.9f - 25.f;
		m_UiInfo.fY = 200.f - (m_fPosy - 0.5f) * 565.f * 0.9f;

		UM->Set_MinimapPosx((m_fPosx - 0.5f) * -688.f * 0.9f);
		UM->Set_MinimapPosy((m_fPosy - 0.5f) * -565.f * 0.9f);
	}




	//UI_InputDebug(fTimeDelta);

	if (UM->Get_FrontUI() && !UM->Get_QuestOn() && !UM->PartyOn_State() && !UM->Get_ShopState() && !UM->Get_StreetShop())
		__super::Tick(fTimeDelta);
}

void CMiniMinimap::LateTick(_float fTimeDelta)
{
	if (UM->Get_FrontUI() && !UM->Get_QuestOn() && !UM->PartyOn_State() && !UM->Get_ShopState() && !UM->Get_StreetShop())
		__super::LateTick(fTimeDelta);

	if (UM->Get_FrontUI() && !UM->Get_QuestOn() && !UM->PartyOn_State() && !UM->Get_ShopState() && !UM->Get_StreetShop())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMiniMinimap::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_Playerx", &m_fPosx, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_Playery", &m_fPosy, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_MapNum", &m_iMapNum, sizeof(_int));


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

	if (FAILED(m_pShaderCom->Begin(45)))
		return E_FAIL;


	if (TM->Get_CurLevel() != 9)
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}


	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();
	

	return S_OK;
}






HRESULT CMiniMinimap::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TutorialMiniMap"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CMiniMinimap::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CMiniMinimap * CMiniMinimap::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMiniMinimap*		pInstance = new CMiniMinimap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMiniMinimap"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMiniMinimap::Clone(void * pArg)
{
	CMiniMinimap*		pInstance = new CMiniMinimap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMiniMinimap"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CMiniMinimap::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
