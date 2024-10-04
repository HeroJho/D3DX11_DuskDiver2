#include "stdafx.h"
#include "..\Public\MiniBossMonster.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Monster_Manager.h"
#include "UI_Manager.h"
#include "Creture.h"
#include "Player_Manager.h"

CMiniBossMonster::CMiniBossMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CMiniBossMonster::CMiniBossMonster(const CMiniBossMonster & rhs)
	: CUI(rhs)
{
}

HRESULT CMiniBossMonster::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiniBossMonster::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pMonster = (CCreture*)m_UiInfo.pDesc;

	m_UiInfo.fSizeX = 40.f;
	m_UiInfo.fSizeY = 40.f;

	if (m_pMonster != nullptr)
		m_vMonsterPos = m_pMonster->Get_WorldPos();

	m_fMaxHp = m_pMonster->Get_MaxHp();

	return S_OK;
}

void CMiniBossMonster::Tick(_float fTimeDelta)
{
	if (m_pMonster != nullptr)
		m_vMonsterPos = m_pMonster->Get_WorldPos();

	__super::Tick(fTimeDelta);
}

void CMiniBossMonster::LateTick(_float fTimeDelta)
{
	if (m_pMonster != nullptr)
		m_fNowHp = m_pMonster->Get_NowHp();
	
	if (m_pMonster != nullptr)
	{
		if (0 >= m_fNowHp)
			Set_Dead();
	}


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
		m_UiInfo.fX = m_vMonsterPos.x * 2.2f + 540.f + 580.f + UM->Get_MinimapPosx() - 40.f;
		m_UiInfo.fY = m_vMonsterPos.z * -2.2f + 545.f - 160.f + UM->Get_MinimapPosy();
	}

	if (TM->Get_CurLevel() == 8)
	{
		m_UiInfo.fX = PM->Get_PlayerPos().x * 1.25f + 640.f;
		m_UiInfo.fY = PM->Get_PlayerPos().z * -1.151f + 520.f - 40.f;
	}


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

HRESULT CMiniBossMonster::Render()
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




	if (m_fX > PM->Get_PlayerPos().x * 10.7f - 60.2f + 640.f && m_fX < PM->Get_PlayerPos().x * 10.7f + 60.2f + 640.f &&
		m_fY > PM->Get_PlayerPos().z * -9.f - 75.f + 360.f && m_fY < PM->Get_PlayerPos().z * -9.f + 75.f + 360.f
		)
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}



	__super::Render();

	return S_OK;
}

void CMiniBossMonster::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}
HRESULT CMiniBossMonster::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniBossMonster"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}




CMiniBossMonster * CMiniBossMonster::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMiniBossMonster*		pInstance = new CMiniBossMonster(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMiniBossMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMiniBossMonster::Clone(void * pArg)
{
	CMiniBossMonster*		pInstance = new CMiniBossMonster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMiniBossMonster"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CMiniBossMonster::Free()
{
	__super::Free();


	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
