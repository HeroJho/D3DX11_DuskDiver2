#include "stdafx.h"
#include "..\Public\UpgradeSceneFail.h"
#include "GameInstance.h"
#include "UI_Manager.h"


#include "ToolManager.h"



CUpgradeSceneFail::CUpgradeSceneFail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CUpgradeSceneFail::CUpgradeSceneFail(const CUpgradeSceneFail & rhs)
	: CUI(rhs)
{
}

HRESULT CUpgradeSceneFail::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUpgradeSceneFail::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CUpgradeSceneFail::Tick(_float fTimeDelta)
{
	if (UM->Get_UpgradeFinish() && !UM->Get_Upgrade())
	{
		m_fTime += fTimeDelta;
	}
	else
	{
		m_fTime = 0.f;
		m_fAlpha = 0.f;
	}

	if (m_fTime >= 5.92f)
	{
		m_fAlpha += fTimeDelta;
	}

	if (m_fTime > 6.3f && !m_bSound)
	{
		GI->PlaySoundW(L"Fail.wav", SD_UI, 1.f);
		m_bSound = true;
	}
	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CUpgradeSceneFail::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUpgradeSceneFail::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	for (int i = 0; i < 43; i++)
	{
		if ((i + 1) * 0.08f > m_fAlpha && i * 0.08f < m_fAlpha)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", i)))
				return E_FAIL;
		}
	}
	if (m_fAlpha > 3.44f)
	{
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;
	}


	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;

	if (UM->Get_UpgradeFinish() && !UM->Get_Upgrade() && m_fTime >= 5.92f)
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}




	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CUpgradeSceneFail::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpgradeSceneFail"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CUpgradeSceneFail::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CUpgradeSceneFail * CUpgradeSceneFail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUpgradeSceneFail*		pInstance = new CUpgradeSceneFail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUpgradeSceneFail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUpgradeSceneFail::Clone(void * pArg)
{
	CUpgradeSceneFail*		pInstance = new CUpgradeSceneFail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUpgradeSceneFail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CUpgradeSceneFail::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
