#include "stdafx.h"
#include "..\Public\Clue1.h"
#include "GameInstance.h"


#include "ToolManager.h"
#include "UI_Manager.h"


CClue1::CClue1(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CClue1::CClue1(const CClue1 & rhs)
	: CUI(rhs)
{
}

HRESULT CClue1::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CClue1::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//Make_ImGui();

	UM->BackUp_Clue1(this);

	return S_OK;
}

void CClue1::Tick(_float fTimeDelta)
{
	if (m_bRender)
	{
		if (false == m_bReverse)
		{
			if (m_fTime <= 0.f)
				m_fTime = 0.f;
			else
				m_fTime -= fTimeDelta * 0.08f;

			m_fDeadTime += 1.f * fTimeDelta;
			if ((9.f < m_fDeadTime) || (CK_DOWN(DIK_RETURN)))
			{
				m_bReverse = true;
			}
		}
		else
		{
			m_fTime += fTimeDelta * 0.12f;

			if (0.3f <= m_fTime)
			{
				Set_Dead();
				UM->Clear_Clue1();
				UM->Set_TalkIndex(35);
			}
		}
			
	}

	//UI_InputDebug(fTimeDelta);
	if (LEVEL_STAGE4 == TM->Get_CurLevel())
		Set_Dead();

	__super::Tick(fTimeDelta);
}

void CClue1::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	if (m_bRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CClue1::Render()
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


	if (FAILED(m_pShaderCom->Begin(7)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;




	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CClue1::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Clue"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CClue1::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CClue1 * CClue1::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CClue1*		pInstance = new CClue1(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CClue1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CClue1::Clone(void * pArg)
{
	CClue1*		pInstance = new CClue1(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CClue1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CClue1::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
