#include "stdafx.h"
#include "..\Public\Player2EquipmentE.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"



CPlayer2EquipmentE::CPlayer2EquipmentE(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CPlayer2EquipmentE::CPlayer2EquipmentE(const CPlayer2EquipmentE & rhs)
	: CUI(rhs)
{
}

HRESULT CPlayer2EquipmentE::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer2EquipmentE::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CPlayer2EquipmentE::Tick(_float fTimeDelta)
{
	if (UM->Get_EquipmentClassSelect() == 2)
	{
		switch (UM->Get_Player2SelectEHat())
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
		switch (UM->Get_Player2SelectEClothes())
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
		switch (UM->Get_Player2SelectEDeco())
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

void CPlayer2EquipmentE::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CPlayer2EquipmentE::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (UM->Get_EquipmentPlayer() != 2)
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
			if (UM->Get_Player2SelectEHat() != 0)
			{
				if (FAILED(m_pVIBufferCom->Render()))
					return E_FAIL;
			}
		}
		else if (UM->Get_EquipmentClassSelect() == 3)
		{
			if (UM->Get_Player2SelectEClothes() != 0)
			{
				if (FAILED(m_pVIBufferCom->Render()))
					return E_FAIL;
			}
		}
		else if (UM->Get_EquipmentClassSelect() == 4)
		{
			if (UM->Get_Player2SelectEDeco() != 0)
			{
				if (FAILED(m_pVIBufferCom->Render()))
					return E_FAIL;
			}
		}
	



	// �ڽĵ��� Render ȣ��
	__super::Render();

	return S_OK;
}






HRESULT CPlayer2EquipmentE::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EquipmentE"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CPlayer2EquipmentE::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CPlayer2EquipmentE * CPlayer2EquipmentE::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer2EquipmentE*		pInstance = new CPlayer2EquipmentE(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer2EquipmentE"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer2EquipmentE::Clone(void * pArg)
{
	CPlayer2EquipmentE*		pInstance = new CPlayer2EquipmentE(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer2EquipmentE"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CPlayer2EquipmentE::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
