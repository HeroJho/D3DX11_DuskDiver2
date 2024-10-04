#include "stdafx.h"
#include "..\Public\UpgradeName.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"



CUpgradeName::CUpgradeName(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CUpgradeName::CUpgradeName(const CUpgradeName & rhs)
	: CUI(rhs)
{
}

HRESULT CUpgradeName::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUpgradeName::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CUpgradeName::Tick(_float fTimeDelta)
{
	if (UM->Get_UpgradeFinish())
	{
		m_fTime += fTimeDelta;
	}
	else
	{
		m_fTime = 0.f;
	}


	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CUpgradeName::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CUpgradeName::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));


	if (m_fTime >= 5.92f)
	{
		_int i = UM->Get_ShopItem() * 7;
		if (UM->Get_Upgrade())
		{
			switch (UM->Get_BeforeWeapon())
			{
			case 0:
				if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 1 + i)))
					return E_FAIL;
				break;
			case 1:
				if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 2 + i)))
					return E_FAIL;
				break;
			case 2:
				if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 3 + i)))
					return E_FAIL;
				break;
			case 3:
				if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 4 + i)))
					return E_FAIL;
				break;
			case 4:
				if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 5 + i)))
					return E_FAIL;
				break;
			case 5:
				if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 6 + i)))
					return E_FAIL;
				break;
			}
		}
		else
			switch (UM->Get_BeforeWeapon())
			{
			case 0:
				if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0 + i - 1)))
					return E_FAIL;
				break;
			case 1:
				if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 1 + i - 1)))
					return E_FAIL;
				break;
			case 2:
				if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 2 + i - 1)))
					return E_FAIL;
				break;
			case 3:
				if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 3 + i - 1)))
					return E_FAIL;
				break;
			case 4:
				if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 4 + i - 1)))
					return E_FAIL;
				break;
			case 5:
				if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 5 + i - 1)))
					return E_FAIL;
				break;
			}
	}
	else
	{
		_int i = UM->Get_ShopItem() * 7;
		switch (UM->Get_BeforeWeapon())
		{
		case 0:
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0 + i)))
				return E_FAIL;
			break;
		case 1:
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 1 + i)))
				return E_FAIL;
			break;
		case 2:
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 2 + i)))
				return E_FAIL;
			break;
		case 3:
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 3 + i)))
				return E_FAIL;
			break;
		case 4:
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 4 + i)))
				return E_FAIL;
			break;
		case 5:
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 5 + i)))
				return E_FAIL;
			break;
		}
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






HRESULT CUpgradeName::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpgradeName"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CUpgradeName::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CUpgradeName * CUpgradeName::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CUpgradeName*		pInstance = new CUpgradeName(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUpgradeName"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CUpgradeName::Clone(void * pArg)
{
	CUpgradeName*		pInstance = new CUpgradeName(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUpgradeName"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CUpgradeName::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
