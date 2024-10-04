#include "stdafx.h"
#include "..\Public\UpgradeScene.h"
#include "GameInstance.h"
#include "UI_Manager.h"


#include "ToolManager.h"



CUpgradeScene::CUpgradeScene(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CUpgradeScene::CUpgradeScene(const CUpgradeScene & rhs)
	: CUI(rhs)
{
}

HRESULT CUpgradeScene::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUpgradeScene::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CUpgradeScene::Tick(_float fTimeDelta)
{
	if (UM->Get_UpgradeFinish())
	{
		if (!m_bSound)
		{
			GI->PlaySoundW(L"Upgrade.wav", SD_UI, 0.2f);
			m_bSound = true;
		}

		m_fTime += fTimeDelta;
	}
	else
	{
		if (m_fTime > 0.1f)
		{
			GI->StopSound(SD_UI);
		}
		m_bSound = false;
		m_fTime = 0.f;
	}
	//if (!UM->Get_UpgradeFinish())
	//{
	//	GI->StopSound(SD_UI);
	//}

	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CUpgradeScene::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUpgradeScene::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));


	for (int i = 0; i < 148; i++)
	{
		if ((i + 1) * 0.04f > m_fTime && i * 0.04f < m_fTime)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", i)))
				return E_FAIL;
		}
	}

	if (m_fTime >= 5.92f)
	{
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;


	if (UM->Get_UpgradeFinish())
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;

	}



	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CUpgradeScene::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpgradeScene"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CUpgradeScene::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CUpgradeScene * CUpgradeScene::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUpgradeScene*		pInstance = new CUpgradeScene(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUpgradeScene"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUpgradeScene::Clone(void * pArg)
{
	CUpgradeScene*		pInstance = new CUpgradeScene(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUpgradeScene"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CUpgradeScene::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
