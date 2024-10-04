#include "stdafx.h"
#include "..\Public\SkullVanish.h"
#include "GameInstance.h"


#include "ToolManager.h"



CSkullVanish::CSkullVanish(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CSkullVanish::CSkullVanish(const CSkullVanish & rhs)
	: CUI(rhs)
{
}

HRESULT CSkullVanish::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkullVanish::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CSkullVanish::Tick(_float fTimeDelta)
{

	m_fTime += fTimeDelta;
	m_fTime2 += fTimeDelta;
	if (m_fTime2 > 1.5f)
	{
		if (m_fTime > 0.03f)
		{
			m_UiInfo.fSizeX += 20.f;
			m_UiInfo.fSizeY += 20.f;
			m_fTime = 0.f;
		}
	}

	if (m_fTime2 > 2.f)
	{
		Set_Dead();
	}

	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CSkullVanish::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CSkullVanish::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(35)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;




	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CSkullVanish::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkullVanish"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CSkullVanish::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CSkullVanish * CSkullVanish::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSkullVanish*		pInstance = new CSkullVanish(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSkullVanish"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSkullVanish::Clone(void * pArg)
{
	CSkullVanish*		pInstance = new CSkullVanish(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CSkullVanish"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CSkullVanish::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
