#include "stdafx.h"
#include "..\Public\ReserveBar.h"
#include "GameInstance.h"
#include "ToolManager.h"



CReserveBar::CReserveBar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CReserveBar::CReserveBar(const CReserveBar & rhs)
	: CUI(rhs)
{
}

HRESULT CReserveBar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CReserveBar::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	m_UiInfo.fSizeY = 0.f;
	m_UiInfo.fSizeX = 31.f * 0.82f / 2.f;
	m_UiInfo.fY += 30.f * 0.72f;
	//m_UiInfo.fX -= 15.5f * 0.82f;

	return S_OK;
}

void CReserveBar::Tick(_float fTimeDelta)
{
	if (CK_DOWN(DIK_DOWN) || CK_DOWN(DIK_UP) || CK_DOWN(DIK_RETURN) || CK_DOWN(DIK_Q) || CK_DOWN(DIK_E))
	{
		Set_Dead();
	}

	//UI_InputDebug(fTimeDelta);

	m_fTime += fTimeDelta;
	if (m_fTime > 0.008f)
	{
		if (m_UiInfo.fSizeY < 43.2f)
		{
			m_UiInfo.fSizeY += 4.f;
			m_UiInfo.fSizeX += (31.f * 0.82f) / 43.2f;
			//m_UiInfo.fX -= (31.f * 0.82f) / 43.2f * 0.75f;
			m_UiInfo.fY -= 2.f;
			m_fTime = 0.f;
		}
		else
		{
			m_UiInfo.fSizeY = 60.f * 0.72f;
			m_UiInfo.fSizeX = 31.f * 0.82f;
		}
	}

	__super::Tick(fTimeDelta);
}

void CReserveBar::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);



	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CReserveBar::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ReserveBar", &m_ProjMatrix, sizeof(_float));


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	//UI_RenderDebug();


	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CReserveBar::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ReserveBar"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}







void CReserveBar::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CReserveBar * CReserveBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CReserveBar*		pInstance = new CReserveBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CReserveBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CReserveBar::Clone(void * pArg)
{
	CReserveBar*		pInstance = new CReserveBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CReserveBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CReserveBar::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
