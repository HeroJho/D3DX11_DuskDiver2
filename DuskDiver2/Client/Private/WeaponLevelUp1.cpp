#include "stdafx.h"
#include "..\Public\WeaponLevelUp1.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"



CWeaponLevelUp1::CWeaponLevelUp1(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CWeaponLevelUp1::CWeaponLevelUp1(const CWeaponLevelUp1 & rhs)
	: CUI(rhs)
{
}

HRESULT CWeaponLevelUp1::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeaponLevelUp1::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CWeaponLevelUp1::Tick(_float fTimeDelta)
{



	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CWeaponLevelUp1::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CWeaponLevelUp1::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	switch (UM->Get_ShopItem())
	{
	case 0:
		if (UM->Get_Player1Weapon() == 0)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
				return E_FAIL;
			break;
		}
		else if (UM->Get_Player1Weapon() == 1)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 1)))
				return E_FAIL;
			break;
		}
		else if (UM->Get_Player1Weapon() == 2)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 2)))
				return E_FAIL;
			break;
		}
		else if (UM->Get_Player1Weapon() == 3)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 3)))
				return E_FAIL;
			break;
		}
		else if (UM->Get_Player1Weapon() == 4)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 4)))
				return E_FAIL;
			break;
		}
		else if (UM->Get_Player1Weapon() == 5)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 5)))
				return E_FAIL;
			break;
		}
		else if (UM->Get_Player1Weapon() == 6)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 6)))
				return E_FAIL;
			break;
		}
	case 1:
		if (UM->Get_Player2Weapon() == 0)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
				return E_FAIL;
			break;
		}
		else if (UM->Get_Player2Weapon() == 1)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 1)))
				return E_FAIL;
			break;
		}
		else if (UM->Get_Player2Weapon() == 2)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 2)))
				return E_FAIL;
			break;
		}
		else if (UM->Get_Player2Weapon() == 3)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 3)))
				return E_FAIL;
			break;
		}
		else if (UM->Get_Player2Weapon() == 4)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 4)))
				return E_FAIL;
			break;
		}
		else if (UM->Get_Player2Weapon() == 5)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 5)))
				return E_FAIL;
			break;
		}
		else if (UM->Get_Player2Weapon() == 6)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 6)))
				return E_FAIL;
			break;
		}
	case 2:
		if (UM->Get_Player3Weapon() == 0)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
				return E_FAIL;
			break;
		}
		else if (UM->Get_Player3Weapon() == 1)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 1)))
				return E_FAIL;
			break;
		}
		else if (UM->Get_Player3Weapon() == 2)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 2)))
				return E_FAIL;
			break;
		}
		else if (UM->Get_Player3Weapon() == 3)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 3)))
				return E_FAIL;
			break;
		}
		else if (UM->Get_Player3Weapon() == 4)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 4)))
				return E_FAIL;
			break;
		}
		else if (UM->Get_Player3Weapon() == 5)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 5)))
				return E_FAIL;
			break;
		}
		else if (UM->Get_Player3Weapon() == 6)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 6)))
				return E_FAIL;
			break;
		}
	default:
		break;
	}




	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;




	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CWeaponLevelUp1::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WeaponLevelUp"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CWeaponLevelUp1::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CWeaponLevelUp1 * CWeaponLevelUp1::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWeaponLevelUp1*		pInstance = new CWeaponLevelUp1(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CWeaponLevelUp1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWeaponLevelUp1::Clone(void * pArg)
{
	CWeaponLevelUp1*		pInstance = new CWeaponLevelUp1(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CWeaponLevelUp1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CWeaponLevelUp1::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
