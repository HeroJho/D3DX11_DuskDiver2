#include "stdafx.h"
#include "..\Public\InvenEquipSlot14.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "ToolManager.h"



CInvenEquipSlot14::CInvenEquipSlot14(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CInvenEquipSlot14::CInvenEquipSlot14(const CInvenEquipSlot14 & rhs)
	: CUI(rhs)
{
}

HRESULT CInvenEquipSlot14::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInvenEquipSlot14::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CInvenEquipSlot14::Tick(_float fTimeDelta)
{
	switch (UM->Get_InvenEquipZero(15))
	{
	case 0:
		m_UiInfo.fY = 284.f;
		if (UM->Get_InvenEquip(14) > 0 && UM->Get_InvenSelNum() == 15)
			UM->Set_EquipmentType(14);
		break;
	case 1:
		m_UiInfo.fY = 249.f;
		if (UM->Get_InvenEquip(14) > 0 && UM->Get_InvenSelNum() == 14)
			UM->Set_EquipmentType(14);
		break;
	case 2:
		m_UiInfo.fY = 214.f;
		if (UM->Get_InvenEquip(14) > 0 && UM->Get_InvenSelNum() == 13)
			UM->Set_EquipmentType(14);
		break;
	case 3:
		m_UiInfo.fY = 179.f;
		if (UM->Get_InvenEquip(14) > 0 && UM->Get_InvenSelNum() == 12)
			UM->Set_EquipmentType(14);
		break;
	case 4:
		m_UiInfo.fY = 144.f;
		if (UM->Get_InvenEquip(14) > 0 && UM->Get_InvenSelNum() == 11)
			UM->Set_EquipmentType(14);
		break;
	case 5:
		m_UiInfo.fY = 109.f;
		if (UM->Get_InvenEquip(14) > 0 && UM->Get_InvenSelNum() == 10)
			UM->Set_EquipmentType(14);
		break;
	case 6:
		m_UiInfo.fY = 74.f;
		if (UM->Get_InvenEquip(14) > 0 && UM->Get_InvenSelNum() == 9)
			UM->Set_EquipmentType(14);
		break;
	case 7:
		m_UiInfo.fY = 39.f;
		if (UM->Get_InvenEquip(14) > 0 && UM->Get_InvenSelNum() == 8)
			UM->Set_EquipmentType(14);
		break;
	case 8:
		m_UiInfo.fY = 4.f;
		if (UM->Get_InvenEquip(14) > 0 && UM->Get_InvenSelNum() == 7)
			UM->Set_EquipmentType(14);
		break;
	case 9:
		m_UiInfo.fY = -31.f;
		if (UM->Get_InvenEquip(14) > 0 && UM->Get_InvenSelNum() == 6)
			UM->Set_EquipmentType(14);
		break;
	case 10:
		m_UiInfo.fY = -66.f;
		if (UM->Get_InvenEquip(14) > 0 && UM->Get_InvenSelNum() == 5)
			UM->Set_EquipmentType(14);
		break;
	case 11:
		m_UiInfo.fY = -101.f;
		if (UM->Get_InvenEquip(14) > 0 && UM->Get_InvenSelNum() == 4)
			UM->Set_EquipmentType(14);
		break;
	case 12:
		m_UiInfo.fY = -136.f;
		if (UM->Get_InvenEquip(14) > 0 && UM->Get_InvenSelNum() == 3)
			UM->Set_EquipmentType(14);
		break;
	case 14:
		m_UiInfo.fY = -171.f;
		if (UM->Get_InvenEquip(14) > 0 && UM->Get_InvenSelNum() == 2)
			UM->Set_EquipmentType(14);
		break;
	case 15:
		m_UiInfo.fY = -206.f;
		if (UM->Get_InvenEquip(14) > 0 && UM->Get_InvenSelNum() == 1)
			UM->Set_EquipmentType(14);
		break;
	}


	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CInvenEquipSlot14::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CInvenEquipSlot14::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 14)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;

	if (UM->Get_InvenNum() == 2)
	{
		if (UM->Get_InvenEquip(14) > 0)
		{
			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;
		}
	}



	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CInvenEquipSlot14::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InvenEquipSlot"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CInvenEquipSlot14::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CInvenEquipSlot14 * CInvenEquipSlot14::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CInvenEquipSlot14*		pInstance = new CInvenEquipSlot14(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CInvenEquipSlot14"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CInvenEquipSlot14::Clone(void * pArg)
{
	CInvenEquipSlot14*		pInstance = new CInvenEquipSlot14(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CInvenEquipSlot14"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CInvenEquipSlot14::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
