#include "stdafx.h"
#include "..\Public\EquipMentBackGround.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "ToolManager.h"



CEquipMentBackGround::CEquipMentBackGround(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CEquipMentBackGround::CEquipMentBackGround(const CEquipMentBackGround & rhs)
	: CUI(rhs)
{
}

HRESULT CEquipMentBackGround::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEquipMentBackGround::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();




	return S_OK;
}

void CEquipMentBackGround::Tick(_float fTimeDelta)
{


	if (m_UiInfo.fSizeX < 350.f)
		m_UiInfo.fSizeX += 30.f;
	else
		m_UiInfo.fSizeX = 350.f;

	if (UM->Get_MenuNum() != 3)
		m_UiInfo.fSizeX = 150.f;


	//UI_InputDebug(fTimeDelta);



	__super::Tick(fTimeDelta);
}

void CEquipMentBackGround::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CEquipMentBackGround::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (UM->Get_MenuNum() == 3)
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}

	//UI_RenderDebug();


	__super::Render();

	return S_OK;
}






HRESULT CEquipMentBackGround::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EquipmentBackGround"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}







void CEquipMentBackGround::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CEquipMentBackGround * CEquipMentBackGround::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEquipMentBackGround*		pInstance = new CEquipMentBackGround(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEquipMentBackGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEquipMentBackGround::Clone(void * pArg)
{
	CEquipMentBackGround*		pInstance = new CEquipMentBackGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEquipMentBackGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CEquipMentBackGround::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
