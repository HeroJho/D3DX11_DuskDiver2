#include "stdafx.h"
#include "..\Public\EquipmentWeaponBackground.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"



CEquipmentWeaponBackground::CEquipmentWeaponBackground(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CEquipmentWeaponBackground::CEquipmentWeaponBackground(const CEquipmentWeaponBackground & rhs)
	: CUI(rhs)
{
}

HRESULT CEquipmentWeaponBackground::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEquipmentWeaponBackground::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CEquipmentWeaponBackground::Tick(_float fTimeDelta)
{



	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CEquipmentWeaponBackground::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CEquipmentWeaponBackground::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));


	if (UM->Get_EquipmentPlayer() == 1)
	{
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", UM->Get_Player1Weapon())))
			return E_FAIL;
	}
	else if (UM->Get_EquipmentPlayer() == 2)
	{
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", UM->Get_Player2Weapon() + 7)))
			return E_FAIL;
	}
	else if (UM->Get_EquipmentPlayer() == 3)
	{
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", UM->Get_Player3Weapon() + 14)))
			return E_FAIL;
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






HRESULT CEquipmentWeaponBackground::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EquipmentWeaponBackground"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CEquipmentWeaponBackground::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CEquipmentWeaponBackground * CEquipmentWeaponBackground::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEquipmentWeaponBackground*		pInstance = new CEquipmentWeaponBackground(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEquipmentWeaponBackground"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEquipmentWeaponBackground::Clone(void * pArg)
{
	CEquipmentWeaponBackground*		pInstance = new CEquipmentWeaponBackground(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEquipmentWeaponBackground"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CEquipmentWeaponBackground::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
