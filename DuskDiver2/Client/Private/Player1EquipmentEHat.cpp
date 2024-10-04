#include "stdafx.h"
#include "..\Public\Player1EquipmentEHat.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"



CPlayer1EquipmentEHat::CPlayer1EquipmentEHat(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CPlayer1EquipmentEHat::CPlayer1EquipmentEHat(const CPlayer1EquipmentEHat & rhs)
	: CUI(rhs)
{
}

HRESULT CPlayer1EquipmentEHat::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer1EquipmentEHat::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CPlayer1EquipmentEHat::Tick(_float fTimeDelta)
{
	if (UM->Get_EquipmentClassSelect() == 2)
	{
		switch (UM->Get_Player1SelectEHat())
		{
		case 1:
			m_UiInfo.fY = -203.3f;
			break;
		case 2:
			m_UiInfo.fY = -150.3f;
			break;
		case 3:
			m_UiInfo.fY = -97.3f;
			break;
		case 4:
			m_UiInfo.fY = -44.3f;
			break;
		case 5:
			m_UiInfo.fY = 8.7f;
			break;
		case 6:
			m_UiInfo.fY = 61.7f;
			break;
		case 7:
			m_UiInfo.fY = 114.7f;
			break;
		}
	}
	else if (UM->Get_EquipmentClassSelect() == 3)
	{
		switch (UM->Get_Player1SelectEClothes())
		{
		case 1:
			m_UiInfo.fY = -203.3f;
			break;
		case 2:
			m_UiInfo.fY = -150.3f;
			break;
		case 3:
			m_UiInfo.fY = -97.3f;
			break;
		case 4:
			m_UiInfo.fY = -44.3f;
			break;
		case 5:
			m_UiInfo.fY = 8.7f;
			break;
		case 6:
			m_UiInfo.fY = 61.7f;
			break;
		case 7:
			m_UiInfo.fY = 114.7f;
			break;
		}
	}

	else if (UM->Get_EquipmentClassSelect() == 4)
	{
		switch (UM->Get_Player1SelectEDeco())
		{
		case 1:
			m_UiInfo.fY = -203.3f;
			break;
		case 2:
			m_UiInfo.fY = -150.3f;
			break;
		case 3:
			m_UiInfo.fY = -97.3f;
			break;
		case 4:
			m_UiInfo.fY = -44.3f;
			break;
		case 5:
			m_UiInfo.fY = 8.7f;
			break;
		case 6:
			m_UiInfo.fY = 61.7f;
			break;
		case 7:
			m_UiInfo.fY = 114.7f;
			break;
		}
	}


	__super::Tick(fTimeDelta);
}

void CPlayer1EquipmentEHat::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CPlayer1EquipmentEHat::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (UM->Get_EquipmentPlayer() != 1)
	{
		if (FAILED(m_pShaderCom->Begin(22)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pShaderCom->Begin(2)))
			return E_FAIL;
	}


		if (UM->Get_EquipmentClassSelect() == 2)
		{
			if (UM->Get_Player1SelectEHat() != 0)
			{
				if (FAILED(m_pVIBufferCom->Render()))
					return E_FAIL;
			}
		}
		else if (UM->Get_EquipmentClassSelect() == 3)
		{
			if (UM->Get_Player1SelectEClothes() != 0)
			{
				if (FAILED(m_pVIBufferCom->Render()))
					return E_FAIL;
			}
		}
		else if (UM->Get_EquipmentClassSelect() == 4)
		{
			if (UM->Get_Player1SelectEDeco() != 0)
			{
				if (FAILED(m_pVIBufferCom->Render()))
					return E_FAIL;
			}
		}




	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CPlayer1EquipmentEHat::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EquipmentE"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CPlayer1EquipmentEHat::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CPlayer1EquipmentEHat * CPlayer1EquipmentEHat::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer1EquipmentEHat*		pInstance = new CPlayer1EquipmentEHat(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer1EquipmentEHat"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer1EquipmentEHat::Clone(void * pArg)
{
	CPlayer1EquipmentEHat*		pInstance = new CPlayer1EquipmentEHat(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer1EquipmentEHat"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CPlayer1EquipmentEHat::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
