#include "stdafx.h"
#include "..\Public\SelectSlot0.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"



CSelectSlot0::CSelectSlot0(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CSelectSlot0::CSelectSlot0(const CSelectSlot0 & rhs)
	: CUI(rhs)
{
}

HRESULT CSelectSlot0::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSelectSlot0::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CSelectSlot0::Tick(_float fTimeDelta)
{
	//UI_InputDebug(fTimeDelta);




	if (m_fTime <= 0)
	{
		m_bTime = false;
	}
	else if (m_fTime >= 0.55)
	{
		m_bTime = true;
	}

	if(!m_bTime)
		m_fTime += fTimeDelta;
	else
		m_fTime -= fTimeDelta;


	__super::Tick(fTimeDelta);
}

void CSelectSlot0::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CSelectSlot0::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_TimeDelta", &m_fTime, sizeof(_float));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(29)))
		return E_FAIL;


	if (UM->Get_BattleNum() == 1 && UM->Get_InvenNum() == 0 && !(UM->Get_UnSelect()))
	{

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}




	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CSelectSlot0::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BattleItemWindow"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CSelectSlot0::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CSelectSlot0 * CSelectSlot0::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSelectSlot0*		pInstance = new CSelectSlot0(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSelectSlot0"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSelectSlot0::Clone(void * pArg)
{
	CSelectSlot0*		pInstance = new CSelectSlot0(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CSelectSlot0"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CSelectSlot0::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
