#include "stdafx.h"
#include "..\Public\LoadingText.h"
#include "GameInstance.h"
#include "ToolManager.h"



CLoadingText::CLoadingText(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CLoadingText::CLoadingText(const CLoadingText & rhs)
	: CUI(rhs)
{
}

HRESULT CLoadingText::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLoadingText::Initialize(void * pArg)
{
	/*if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;*/

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		m_UiInfo = *((UIINFODESC*)pArg);
	}
	else
	{
		m_UiInfo.pParent = nullptr;

		m_UiInfo.fSizeX = g_iWinSizeX;
		m_UiInfo.fSizeY = g_iWinSizeY;

		m_UiInfo.fX = g_iWinSizeX * 0.5f;
		m_UiInfo.fY = g_iWinSizeY * 0.5f;
	}


	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixTranspose(XMMatrixOrthographicLH((_float)g_iWinSizeX, (_float)g_iWinSizeY, 0.f, 1.f)));

	/*if (FAILED(Ready_Components()))
		return E_FAIL;*/

	if (FAILED(__super::Add_Component(LEVEL_LOADING, TEXT("Prototype_Component_Texture_LoadingText"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	Make_ImGui();




	return S_OK;
}

void CLoadingText::Tick(_float fTimeDelta)
{
	m_fTime += fTimeDelta;

	if (m_fTime >= 2.f)
		m_fTime = 0.f;
	//UI_InputDebug(fTimeDelta);



	__super::Tick(fTimeDelta);
}

void CLoadingText::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CLoadingText::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	for (int i = 0; i < 4; i++)
	{
		if (m_fTime >= i * 0.5f && m_fTime < (i + 1) * 0.5f)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", i)))
				return E_FAIL;
		}
	}


	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	//UI_RenderDebug();


	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CLoadingText::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_LOADING, TEXT("Prototype_Component_Texture_LoadingText"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}







void CLoadingText::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CLoadingText * CLoadingText::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLoadingText*		pInstance = new CLoadingText(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CLoadingText"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLoadingText::Clone(void * pArg)
{
	CLoadingText*		pInstance = new CLoadingText(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CLoadingText"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CLoadingText::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
