#include "stdafx.h"
#include "..\Public\InvenEquipSlot8.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "ToolManager.h"



CInvenEquipSlot8::CInvenEquipSlot8(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CInvenEquipSlot8::CInvenEquipSlot8(const CInvenEquipSlot8 & rhs)
	: CUI(rhs)
{
}

HRESULT CInvenEquipSlot8::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInvenEquipSlot8::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CInvenEquipSlot8::Tick(_float fTimeDelta)
{

	if (UM->Get_InvenEquipZero(9) == 8)
	{
		m_UiInfo.fY = -206.f;
		if (UM->Get_InvenEquip(8) > 0 && UM->Get_InvenSelNum() == 1)
			UM->Set_EquipmentType(8);
	}
	else if (UM->Get_InvenEquipZero(9) == 7)
	{
		m_UiInfo.fY = -171.f;
		if (UM->Get_InvenEquip(8) > 0 && UM->Get_InvenSelNum() == 2)
			UM->Set_EquipmentType(8);
	}
	else if (UM->Get_InvenEquipZero(9) == 6)
	{
		m_UiInfo.fY = -136.f;
		if (UM->Get_InvenEquip(8) > 0 && UM->Get_InvenSelNum() == 3)
			UM->Set_EquipmentType(8);
	}
	else if (UM->Get_InvenEquipZero(9) == 5)
	{
		m_UiInfo.fY = -101.f;
		if (UM->Get_InvenEquip(8) > 0 && UM->Get_InvenSelNum() == 4)
			UM->Set_EquipmentType(8);
	}
	else if (UM->Get_InvenEquipZero(9) == 4)
	{
		m_UiInfo.fY = -66.f;
		if (UM->Get_InvenEquip(8) > 0 && UM->Get_InvenSelNum() == 5)
			UM->Set_EquipmentType(8);
	}
	else if (UM->Get_InvenEquipZero(9) == 3)
	{
		m_UiInfo.fY = -31.f;
		if (UM->Get_InvenEquip(8) > 0 && UM->Get_InvenSelNum() == 6)
			UM->Set_EquipmentType(8);
	}
	else if (UM->Get_InvenEquipZero(9) == 2)
	{
		m_UiInfo.fY = 4.f;
		if (UM->Get_InvenEquip(8) > 0 && UM->Get_InvenSelNum() == 7)
			UM->Set_EquipmentType(8);
	}
	else if (UM->Get_InvenEquipZero(9) == 1)
	{
		m_UiInfo.fY = 39.f;
		if (UM->Get_InvenEquip(8) > 0 && UM->Get_InvenSelNum() == 8)
			UM->Set_EquipmentType(8);
	}
	else
	{
		m_UiInfo.fY = 74.f;
		if (UM->Get_InvenEquip(8) > 0 && UM->Get_InvenSelNum() == 9)
			UM->Set_EquipmentType(8);
	}

	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CInvenEquipSlot8::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CInvenEquipSlot8::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 8)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;

	if (UM->Get_InvenNum() == 2)
	{
		if (UM->Get_InvenEquip(8) > 0)
		{
			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}
	}



	//UI_RenderDebug();

	// �ڽĵ��� Render ȣ��
	__super::Render();

	return S_OK;
}






HRESULT CInvenEquipSlot8::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InvenEquipSlot"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CInvenEquipSlot8::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CInvenEquipSlot8 * CInvenEquipSlot8::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CInvenEquipSlot8*		pInstance = new CInvenEquipSlot8(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CInvenEquipSlot8"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CInvenEquipSlot8::Clone(void * pArg)
{
	CInvenEquipSlot8*		pInstance = new CInvenEquipSlot8(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CInvenEquipSlot8"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CInvenEquipSlot8::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
