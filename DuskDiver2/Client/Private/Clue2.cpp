#include "stdafx.h"
#include "..\Public\Clue2.h"
#include "GameInstance.h"


#include "ToolManager.h"
#include "UI_Manager.h"


CClue2::CClue2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CClue2::CClue2(const CClue2 & rhs)
	: CUI(rhs)
{
}

HRESULT CClue2::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CClue2::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//Make_ImGui();

	UM->BackUp_Clue2(this);

	return S_OK;
}

void CClue2::Tick(_float fTimeDelta)
{
	_uint i = UM->Get_ClueCnt(); // debug

	if ((false == m_bCollectAll) && (true == m_bRender) && (2 == UM->Get_ClueCnt())) // 조각1 
	{
		m_Test = true;
		m_UiInfo.fSizeX = 900.f;
		m_UiInfo.fSizeY = 600.f;
		m_iClueType = 1;
	}
	else if ((false == m_bCollectAll) && (true == m_bRender) && (3 == UM->Get_ClueCnt())) // 조각2 
	{
		m_Test = true;
		m_UiInfo.fSizeX = 700.f;
		m_UiInfo.fSizeY = 600.f;
		m_iClueType = 2;
	}
	if ((false == m_bCollectAll) && (true == m_bRender) && (4 == UM->Get_ClueCnt())) // 조각3 == 1 
	{
		m_Test = true;
		m_UiInfo.fSizeX = 900.f;
		m_UiInfo.fSizeY = 600.f;
		m_iClueType = 1;
	}
	else if ((false == m_bCollectAll) && (true == m_bRender) && (5 == UM->Get_ClueCnt()))// 조각4
	{
		m_Test = true;
		m_UiInfo.fSizeX = 600.f;
		m_UiInfo.fSizeY = 750.f;
		m_iClueType = 3;
	}

	else if ((true == m_bCollectAll) && (true == m_bRender) && (5 == UM->Get_ClueCnt()))// 전체
	{
		m_Test = true;
		m_UiInfo.fSizeX = 1400.f * 0.8f;
		m_UiInfo.fSizeY = 900.f * 0.8f;
		m_iClueType = 4;
	}

	if (m_Test)
	{
		if (false == m_bReverse)
		{
			if (m_fTime <= 0.f)
				m_fTime = 0.f;
			else
				m_fTime -= fTimeDelta * 0.08f;

			m_fDeadTime += 1.f * fTimeDelta;
			if ((7.5f < m_fDeadTime) || (CK_DOWN(DIK_RETURN)))
			{
				m_bReverse = true;
				m_fDeadTime = 0.f;
			}
		}
		else
		{
			m_fTime += fTimeDelta * 0.12f;

			if (0.3f <= m_fTime)
			{
				m_fTime = 0.3f;

				m_Test = false;
				m_bRender = false;
				UM->Set_FrontUI(true);
				UM->Set_CoinUp(true); // 코인 증가 반짝이는 UI이펙트
				m_bReverse = false;

				if (3 == m_iClueType)
				{
					m_bRender = true;
					UM->Set_FrontUI(false);
					m_bCollectAll = true;
				}
				if (4 == m_iClueType)
				{
					Set_Dead();
					UM->Set_TalkIndex(42);
					UM->Set_FrontUI(false);
				}
			}
		}
	}

	if (LEVEL_STAGE4 == TM->Get_CurLevel())
		Set_Dead();

	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CClue2::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	if (m_bRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CClue2::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_TimeDelta", &m_fTime, sizeof(_float));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", m_iClueType - 1)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(7)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;




	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CClue2::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Picture"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CClue2::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CClue2 * CClue2::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CClue2*		pInstance = new CClue2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CClue2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CClue2::Clone(void * pArg)
{
	CClue2*		pInstance = new CClue2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CClue2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CClue2::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
