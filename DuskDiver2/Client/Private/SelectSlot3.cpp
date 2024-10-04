#include "stdafx.h"
#include "..\Public\SelectSlot3.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"



CSelectSlot3::CSelectSlot3(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CSelectSlot3::CSelectSlot3(const CSelectSlot3 & rhs)
	: CUI(rhs)
{
}

HRESULT CSelectSlot3::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSelectSlot3::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CSelectSlot3::Tick(_float fTimeDelta)
{

	if (m_fTime <= 0)
	{
		m_bTime = false;
	}
	else if (m_fTime >= 0.55)
	{
		m_bTime = true;
	}

	if (!m_bTime)
		m_fTime += fTimeDelta;
	else
		m_fTime -= fTimeDelta;


	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CSelectSlot3::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CSelectSlot3::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 3)))
		return E_FAIL;


	if (UM->Get_BattleNum() == 4 && UM->Get_InvenNum() == 0 && !(UM->Get_UnSelect()))
	{

		if (FAILED(m_pShaderCom->Begin(29)))
			return E_FAIL;


		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}




	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CSelectSlot3::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BattleItemWindow"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CSelectSlot3::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CSelectSlot3 * CSelectSlot3::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSelectSlot3*		pInstance = new CSelectSlot3(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSelectSlot3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSelectSlot3::Clone(void * pArg)
{
	CSelectSlot3*		pInstance = new CSelectSlot3(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CSelectSlot3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CSelectSlot3::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
