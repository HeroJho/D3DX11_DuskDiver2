#include "stdafx.h"
#include "..\Public\MiniMonster.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Monster_Manager.h"
#include "UI_Manager.h"
#include "Creture.h"
#include "Player_Manager.h"

CMiniMonster::CMiniMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CMiniMonster::CMiniMonster(const CMiniMonster & rhs)
	: CUI(rhs)
{
}

HRESULT CMiniMonster::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiniMonster::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pMonster = (CCreture*)m_UiInfo.pDesc;

	m_UiInfo.fSizeX = 30.f;
	m_UiInfo.fSizeY = 30.f;

	if(m_pMonster != nullptr)
		m_vMonsterPos = m_pMonster->Get_WorldPos();

	m_fMaxHp = m_pMonster->Get_MaxHp();

	return S_OK;
}

void CMiniMonster::Tick(_float fTimeDelta)
{
	if (!m_pMonster->Get_Dead())
	{
		m_vMonsterPos = m_pMonster->Get_WorldPos();
	}
	__super::Tick(fTimeDelta);
}

void CMiniMonster::LateTick(_float fTimeDelta)
{

	if (m_pMonster != nullptr)
	{

		m_fNowHp = m_pMonster->Get_NowHp();
	}


	if (0 >= m_fNowHp || m_pMonster->Get_Dead())
		Set_Dead();

	if (TM->Get_CurLevel() == 3)
	{
		m_fX = m_vMonsterPos.x * 10.7f + 640.f;
		m_fY = m_vMonsterPos.z * -9.f + 360.f;
		m_UiInfo.fX = m_vMonsterPos.x * 10.7f + 640.f + 580.f + UM->Get_MinimapPosx() - 40.f;
		m_UiInfo.fY = m_vMonsterPos.z * -9.f + 360.f - 160.f + UM->Get_MinimapPosy();
	}

	if (TM->Get_CurLevel() == 6)
	{
		m_fX = m_vMonsterPos.x * -8.0f + 640.f - 70.f;
		m_fY = m_vMonsterPos.z * 8.f - 40.f + 360.f;
		m_UiInfo.fX = m_vMonsterPos.x * -8.f + 570.f + 580.f + UM->Get_MinimapPosx() - 40.f;
		m_UiInfo.fY = m_vMonsterPos.z * 8.f - 180.f + 360.f + UM->Get_MinimapPosy();
	}

	if (TM->Get_CurLevel() == 7)
	{
		m_fX = m_vMonsterPos.x * 2.2f + 540.f;
		m_fY = m_vMonsterPos.z * -2.2f + 545.f;
		m_UiInfo.fX = m_vMonsterPos.x * 2.2f + 540.f + 580.f + UM->Get_MinimapPosx() - 60.f;
		m_UiInfo.fY = m_vMonsterPos.z * -2.2f + 545.f - 160.f + UM->Get_MinimapPosy();
	}

	if (TM->Get_CurLevel() == 8)
	{
		m_fX = m_vMonsterPos.x * 1.4f + 645.f;
		m_fY = m_vMonsterPos.z * -1.45f + 565.f;
		m_UiInfo.fX = m_vMonsterPos.x * 1.4f + 635.f + 580.f + UM->Get_MinimapPosx() - 53.f;
		m_UiInfo.fY = m_vMonsterPos.z * -1.4f + 570.f - 170.f + UM->Get_MinimapPosy() - 5.f;
	}

	//if (TM->Get_CurLevel() == 7)
	//{
	//	m_UiInfo.fX = m_vMonsterPos.x  * 2.2f + 714.f;
	//	m_UiInfo.fY = m_vMonsterPos.z  * -3.5f + 397.5f;
	//}

	if (PM->Get_PlayerIsIdle())
	{
		m_fTime2 += fTimeDelta;
		if (m_fTime2 > 2.f)
		{
			m_bIdle = true;
		}
	}
	else
	{
		m_bIdle = false;
		m_fTime2 = 0.f;
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


	if (!UM->Get_QuestOn() && !UM->PartyOn_State() && UM->Get_FrontUI() && !UM->Get_ShopState())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);

	__super::LateTick(fTimeDelta);
}

HRESULT CMiniMonster::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;


	if (TM->Get_CurLevel() == 3)
	{
		if (m_fX > PM->Get_PlayerPos().x * 10.7f - 35.2f + 580.f && m_fX < PM->Get_PlayerPos().x * 10.7f + 85.2f + 580.f &&
			m_fY > PM->Get_PlayerPos().z * -9.f - 50.f + 360.f && m_fY < PM->Get_PlayerPos().z * -9.f + 50.f + 360.f
			)
		{
			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}
	}
	else if (TM->Get_CurLevel() == 6)
	{
		if (m_fX > PM->Get_PlayerPos().x * -8.0f - 30.2f + 640.f -90.f && m_fX < PM->Get_PlayerPos().x  * -8.0f + 95.2f + 640.f -90.f &&
			m_fY > PM->Get_PlayerPos().z * 8.f - 77.f + 360.f - 40.f && m_fY < PM->Get_PlayerPos().z * 8.f + 47.f + 360.f - 40.f
			)
		{
			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}
	}

	if (TM->Get_CurLevel() == 7)
	{
		if (m_fX > PM->Get_PlayerPos().x * 2.2f - 95.2f + 480.f&& m_fX < PM->Get_PlayerPos().x * 2.2f + 95.2f + 480.f&&
			m_fY > PM->Get_PlayerPos().z * -2.2f - 67.f + 545.f && m_fY < PM->Get_PlayerPos().z * -2.2f + 67.f + 545.f
			)
		{
			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}
	}

	if (TM->Get_CurLevel() == 8)
	{
		if (m_fX > PM->Get_PlayerPos().x * 2.2f - 125.2f + 625.f && m_fX < PM->Get_PlayerPos().x * 2.2f + 125.2f + 625.f&&
			m_fY > PM->Get_PlayerPos().z * -2.2f - 57.f + 675.f && m_fY < PM->Get_PlayerPos().z * -2.2f + 82.f + 675.f
			)
		{
			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}
	}

	__super::Render();

	return S_OK;
}

void CMiniMonster::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}
HRESULT CMiniMonster::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniMonster"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}




CMiniMonster * CMiniMonster::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMiniMonster*		pInstance = new CMiniMonster(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMiniMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMiniMonster::Clone(void * pArg)
{
	CMiniMonster*		pInstance = new CMiniMonster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMiniMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CMiniMonster::Free()
{
	__super::Free();


	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
