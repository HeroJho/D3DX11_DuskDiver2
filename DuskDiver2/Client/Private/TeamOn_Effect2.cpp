#include "stdafx.h"
#include "..\Public\TeamOn_Effect2.h"
#include "GameInstance.h"
#include "Player_Manager.h"
#include "ToolManager.h"
#include "Status_Manager.h"
#include "UI_Manager.h"

CTeamOn_Effect2::CTeamOn_Effect2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CTeamOn_Effect2::CTeamOn_Effect2(const CTeamOn_Effect2 & rhs)
	: CUI(rhs)
{
}

HRESULT CTeamOn_Effect2::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTeamOn_Effect2::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	return S_OK;
}

void CTeamOn_Effect2::Tick(_float fTimeDelta)
{
	if (!m_bOnce && UM->DD_State() && !UM->PartyOn_State() && !UM->Get_QuestOn())
	{
		//사이즈 조절
		m_fOnAcc += fTimeDelta;

		if (0.3f > m_fOnAcc)
		{
			m_UiInfo.fSizeX += 10.f;
			m_UiInfo.fSizeY += 10.f;
		}

		else if (0.3f <= m_fOnAcc)
		{
			m_UiInfo.fSizeX -= 10.f;
			m_UiInfo.fSizeY -= 10.f;

			if (0 >= m_UiInfo.fSizeX || 0 >= m_UiInfo.fSizeY)
			{
				m_fOnAcc = 0.f;
				m_bOnce = true;
				Set_Dead();
			}
		}
	}

	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CTeamOn_Effect2::LateTick(_float fTimeDelta)
{
	
	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CTeamOn_Effect2::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	//m_pShaderCom->Set_RawValue("g_HP", &m_fHPGauge, sizeof(_float));


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(31)))
		return E_FAIL;


	if (UM->DD_State() && !UM->Get_QuestOn())
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}

	//UI_RenderDebug();


	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CTeamOn_Effect2::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Green"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CTeamOn_Effect2::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CTeamOn_Effect2 * CTeamOn_Effect2::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTeamOn_Effect2*		pInstance = new CTeamOn_Effect2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTeamOn_Effect2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTeamOn_Effect2::Clone(void * pArg)
{
	CTeamOn_Effect2*		pInstance = new CTeamOn_Effect2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTeamOn_Effect2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CTeamOn_Effect2::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
