#include "stdafx.h"
#include "..\Public\MarvelTimeBackground.h"
#include "GameInstance.h"


#include "ToolManager.h"



CMarvelTimeBackground::CMarvelTimeBackground(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CMarvelTimeBackground::CMarvelTimeBackground(const CMarvelTimeBackground & rhs)
	: CUI(rhs)
{
}

HRESULT CMarvelTimeBackground::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMarvelTimeBackground::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CMarvelTimeBackground::Tick(_float fTimeDelta)
{
	if (CK_DOWN(DIK_NUMPAD0))
	{
		m_bPattern = true;
		m_fTime2 = 0;
		m_fTime3 = 0;
	}

	if (m_bPattern)
	{
		m_fTime += fTimeDelta;
		m_fTime3 += fTimeDelta;
		if (m_fTime3 > 7.4f)
		{
			m_fTime2 += fTimeDelta;
		}
		if (m_fTime > 0.01f)
		{
			if (m_fAlpha <= 0.07f)
				m_fAlpha = 0.07f;
			else
				m_fAlpha -= 0.01f;

			m_fTime = 0.f;
		}

		if (m_fTime2 >= 30)
		{
			m_UiInfo.fSizeX = 560.f;
			m_UiInfo.fX = 680.f;
			m_bPattern = false;

		}
	}


	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CMarvelTimeBackground::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMarvelTimeBackground::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_TimeDelta", &m_fAlpha, sizeof(_float));


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(7)))
		return E_FAIL;

	if (m_bPattern)
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}




	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CMarvelTimeBackground::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MarvelTimeBackground"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CMarvelTimeBackground::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CMarvelTimeBackground * CMarvelTimeBackground::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMarvelTimeBackground*		pInstance = new CMarvelTimeBackground(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMarvelTimeBackground"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMarvelTimeBackground::Clone(void * pArg)
{
	CMarvelTimeBackground*		pInstance = new CMarvelTimeBackground(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMarvelTimeBackground"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CMarvelTimeBackground::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
