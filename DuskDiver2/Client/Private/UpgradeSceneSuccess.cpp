#include "stdafx.h"
#include "..\Public\UpgradeSceneSuccess.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"



CUpgradeSceneSuccess::CUpgradeSceneSuccess(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CUpgradeSceneSuccess::CUpgradeSceneSuccess(const CUpgradeSceneSuccess & rhs)
	: CUI(rhs)
{
}

HRESULT CUpgradeSceneSuccess::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUpgradeSceneSuccess::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CUpgradeSceneSuccess::Tick(_float fTimeDelta)
{
	//UI_InputDebug(fTimeDelta);
	if (UM->Get_UpgradeFinish() && UM->Get_Upgrade())
	{
		m_fTime += fTimeDelta;
	}
	else
	{
		m_fTime = 0.f;
		m_fAlpha = 0.f;
		m_bSound = false;
	}

	if (m_fTime >= 5.92f)
	{
		m_fAlpha += fTimeDelta;
	}

	if (m_fTime > 6.3f && !m_bSound)
	{
		GI->PlaySoundW(L"Success.wav", SD_UI, 1.f);
		m_bSound = true;
	}

	__super::Tick(fTimeDelta);
}

void CUpgradeSceneSuccess::LateTick(_float fTimeDelta)
{
	if (UM->Get_Select())
	{

		__super::LateTick(fTimeDelta);

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
}

HRESULT CUpgradeSceneSuccess::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));


	for (int i = 0; i < 85; i++)
	{
		if ((i + 1) * 0.08f > m_fAlpha && i * 0.08f < m_fAlpha)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", i)))
				return E_FAIL;
		}
	}
	if (m_fAlpha > 6.8f)
	{
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;
	}


	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;

	if (UM->Get_UpgradeFinish() && UM->Get_Upgrade() && m_fTime >= 5.92f)
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;

		__super::Render();
	}

	return S_OK;
}






HRESULT CUpgradeSceneSuccess::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpgradeSceneSuccess"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CUpgradeSceneSuccess::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CUpgradeSceneSuccess * CUpgradeSceneSuccess::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUpgradeSceneSuccess*		pInstance = new CUpgradeSceneSuccess(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUpgradeSceneSuccess"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUpgradeSceneSuccess::Clone(void * pArg)
{
	CUpgradeSceneSuccess*		pInstance = new CUpgradeSceneSuccess(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUpgradeSceneSuccess"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CUpgradeSceneSuccess::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
