#include "stdafx.h"
#include "..\Public\MenuNum1.h"
#include "GameInstance.h"


#include "ToolManager.h"



CMenuNum1::CMenuNum1(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CMenuNum1::CMenuNum1(const CMenuNum1 & rhs)
	: CUI(rhs)
{
}

HRESULT CMenuNum1::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMenuNum1::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();




	return S_OK;
}

void CMenuNum1::Tick(_float fTimeDelta)
{

	if (m_fSlide < 10.f)
	{
		m_UiInfo.fX -= 2.f;
		m_fSlide += 2.f;
	}


	if (CK_DOWN(DIK_ESCAPE))
	{
		m_UiInfo.fX = -149.1f + 10.f;
		m_fSlide = 0.f;
	}


	//UI_InputDebug(fTimeDelta);



	__super::Tick(fTimeDelta);
}

void CMenuNum1::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMenuNum1::Render()
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

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	//UI_RenderDebug();


	// �ڽĵ��� Render ȣ��
	__super::Render();

	return S_OK;
}






HRESULT CMenuNum1::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuNum1"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}









void CMenuNum1::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CMenuNum1 * CMenuNum1::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMenuNum1*		pInstance = new CMenuNum1(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMenuNum1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMenuNum1::Clone(void * pArg)
{
	CMenuNum1*		pInstance = new CMenuNum1(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMenuNum1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CMenuNum1::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
