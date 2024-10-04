#include "stdafx.h"
#include "..\Public\UpgradeBlackOut.h"
#include "GameInstance.h"
#include "UI_Manager.h"


#include "ToolManager.h"



CUpgradeBlackOut::CUpgradeBlackOut(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CUpgradeBlackOut::CUpgradeBlackOut(const CUpgradeBlackOut & rhs)
	: CUI(rhs)
{
}

HRESULT CUpgradeBlackOut::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUpgradeBlackOut::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CUpgradeBlackOut::Tick(_float fTimeDelta)
{
	if (UM->Get_UpgradeFinish())
	{
		m_fTime += fTimeDelta;
	}
	else
	{
		m_fTime = 0.f;
		m_fAlpha = 0.2f;
	}

	if (m_fTime >= 5.92f)
	{
		m_fTime2 += fTimeDelta;
		if (m_fTime2 > 0.01f)
		{
			if (m_fAlpha <= 0.1f)
				m_fAlpha = 0.1f;
			else
				m_fAlpha -= 0.001f;

			m_fTime2 = 0.f;
		}
	}


	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CUpgradeBlackOut::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUpgradeBlackOut::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_TimeDelta", &m_fAlpha, sizeof(_float));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(7)))
		return E_FAIL;



	if (m_fTime >= 5.92f)
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}




	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CUpgradeBlackOut::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpgradeBlackOut"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CUpgradeBlackOut::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CUpgradeBlackOut * CUpgradeBlackOut::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUpgradeBlackOut*		pInstance = new CUpgradeBlackOut(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUpgradeBlackOut"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUpgradeBlackOut::Clone(void * pArg)
{
	CUpgradeBlackOut*		pInstance = new CUpgradeBlackOut(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUpgradeBlackOut"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CUpgradeBlackOut::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
