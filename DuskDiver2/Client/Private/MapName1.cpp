#include "stdafx.h"
#include "..\Public\MapName1.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"



CMapName1::CMapName1(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CMapName1::CMapName1(const CMapName1 & rhs)
	: CUI(rhs)
{
}

HRESULT CMapName1::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapName1::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	if (FAILED(Make_ChildUI(0.f, 0.f, 550.f, 35.f, TEXT("Prototype_GameObject_MapNameEffect"))))
		return E_FAIL;


	return S_OK;
}

void CMapName1::Tick(_float fTimeDelta)
{
	//if (CK_DOWN(DIK_P))
	//{
	//	UM->Subway1On();
	//}
	//if (CK_DOWN(DIK_O))
	//{
	//	UM->Subway1Off();
	//}

	if (UM->Get_Subway1())
	{
		if (m_fAlpha <= 0.f)
			m_fAlpha = 0.f;
		else
			m_fAlpha -= fTimeDelta * 0.11f;
		m_bTrue = true;
	}
	else if (m_bTrue)
	{
		if (m_fAlpha >= 0.3f)
			m_fAlpha = 0.3f;
		else
			m_fAlpha += fTimeDelta * 0.1f;
	}


	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CMapName1::LateTick(_float fTimeDelta)
{



	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMapName1::Render()
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


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;




	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CMapName1::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Guro"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CMapName1::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CMapName1 * CMapName1::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMapName1*		pInstance = new CMapName1(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMapName1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMapName1::Clone(void * pArg)
{
	CMapName1*		pInstance = new CMapName1(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMapName1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CMapName1::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
