#include "stdafx.h"
#include "..\Public\MapName2.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"



CMapName2::CMapName2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CMapName2::CMapName2(const CMapName2 & rhs)
	: CUI(rhs)
{
}

HRESULT CMapName2::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapName2::Initialize(void * pArg)
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

void CMapName2::Tick(_float fTimeDelta)
{
	//if (CK_DOWN(DIK_P))
	//{
	//	UM->Subway2On();
	//}
	//if (CK_DOWN(DIK_O))
	//{
	//	UM->Subway2Off();
	//}

	if (UM->Get_Subway2())
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

void CMapName2::LateTick(_float fTimeDelta)
{



	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMapName2::Render()
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






HRESULT CMapName2::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StudyRoom"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CMapName2::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CMapName2 * CMapName2::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMapName2*		pInstance = new CMapName2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMapName2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMapName2::Clone(void * pArg)
{
	CMapName2*		pInstance = new CMapName2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMapName2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CMapName2::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
