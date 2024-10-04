#include "stdafx.h"
#include "..\Public\MaskMap.h"
#include "GameInstance.h"


#include "ToolManager.h"



CMaskMap::CMaskMap(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CMaskMap::CMaskMap(const CMaskMap & rhs)
	: CUI(rhs)
{
}

HRESULT CMaskMap::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMaskMap::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CMaskMap::Tick(_float fTimeDelta)
{



	UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CMaskMap::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMaskMap::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_mWorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_mViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_mProjMatrix", &m_ProjMatrix, sizeof(_float4x4));


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_mDiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;


	//if (FAILED(m_pVIBufferCom->Render()))
	//	return E_FAIL;




	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CMaskMap::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MinimapMask"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CMaskMap::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CMaskMap * CMaskMap::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMaskMap*		pInstance = new CMaskMap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMaskMap"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMaskMap::Clone(void * pArg)
{
	CMaskMap*		pInstance = new CMaskMap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMaskMap"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CMaskMap::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
