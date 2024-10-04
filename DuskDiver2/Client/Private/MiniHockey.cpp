#include "stdafx.h"
#include "..\Public\MiniHockey.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Monster_Manager.h"
#include "UI_Manager.h"
#include "Creture.h"
#include "Player_Manager.h"

CMiniHockey::CMiniHockey(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CMiniHockey::CMiniHockey(const CMiniHockey & rhs)
	: CUI(rhs)
{
}

HRESULT CMiniHockey::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiniHockey::Initialize(void * pArg)
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

void CMiniHockey::Tick(_float fTimeDelta)
{
	if (m_pMonster != nullptr)
		m_vMonsterPos = m_pMonster->Get_WorldPos();

	__super::Tick(fTimeDelta);
}

void CMiniHockey::LateTick(_float fTimeDelta)
{

	m_fX = m_vMonsterPos.x * 1.4f + 645.f;
	m_fY = m_vMonsterPos.z * -1.45f + 565.f;
	m_UiInfo.fX = m_vMonsterPos.x * 1.4f + 635.f + 580.f + UM->Get_MinimapPosx() - 75.f;
	m_UiInfo.fY = m_vMonsterPos.z * -1.4f + 570.f - 170.f + UM->Get_MinimapPosy() + 5.f;

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

HRESULT CMiniHockey::Render()
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


		if (m_fX > PM->Get_PlayerPos().x * 2.2f - 115.2f + 625.f && m_fX < PM->Get_PlayerPos().x * 2.2f + 105.2f + 625.f &&
			m_fY > PM->Get_PlayerPos().z * -2.2f - 62.f + 675.f && m_fY < PM->Get_PlayerPos().z * -2.2f + 87.f + 675.f &&
			PM->Get_PlayerPos().y < -26.f 
			)
		{
			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}

		
	__super::Render();

	return S_OK;
}

void CMiniHockey::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}
HRESULT CMiniHockey::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniHockey"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}




CMiniHockey * CMiniHockey::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMiniHockey*		pInstance = new CMiniHockey(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMiniHockey"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMiniHockey::Clone(void * pArg)
{
	CMiniHockey*		pInstance = new CMiniHockey(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMiniHockey"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CMiniHockey::Free()
{
	__super::Free();


	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
