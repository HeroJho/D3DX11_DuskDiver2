#include "stdafx.h"
#include "..\Public\Equipment2Info.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"



CEquipment2Info::CEquipment2Info(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CEquipment2Info::CEquipment2Info(const CEquipment2Info & rhs)
	: CUI(rhs)
{
}

HRESULT CEquipment2Info::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEquipment2Info::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CEquipment2Info::Tick(_float fTimeDelta)
{



	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CEquipment2Info::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CEquipment2Info::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	
	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", UM->Get_Clothes(UM->Get_EquipmentNum() - 1) )))
		return E_FAIL;



	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;


	if (UM->Get_EquipmentState() == 1 && UM->Get_EquipmentClassSelect() == 3)
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}




	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CEquipment2Info::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Equipment2Info"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CEquipment2Info::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CEquipment2Info * CEquipment2Info::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEquipment2Info*		pInstance = new CEquipment2Info(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEquipment2Info"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEquipment2Info::Clone(void * pArg)
{
	CEquipment2Info*		pInstance = new CEquipment2Info(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEquipment2Info"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CEquipment2Info::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
