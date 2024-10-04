#include "stdafx.h"
#include "..\Public\MapNameEffect.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"



CMapNameEffect::CMapNameEffect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CMapNameEffect::CMapNameEffect(const CMapNameEffect & rhs)
	: CUI(rhs)
{
}

HRESULT CMapNameEffect::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMapNameEffect::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CMapNameEffect::Tick(_float fTimeDelta)
{
	if (UM->Get_Subway())
	{
		m_fTime2 += fTimeDelta;
		if (m_fTime2 > 2.f)
		{
			m_fTime += fTimeDelta;
			if (m_fTime > 0.06f)
			{
				if (m_iNum >= 15)
					m_iNum = 15;
				else
				{
					m_iNum++;
				}
				m_fTime = 0.f;
			}
		}
		else
			m_iNum = 0;
	}

	if (UM->Get_Subway1())
	{
		m_fTime2 += fTimeDelta;
		if (m_fTime2 > 2.f)
		{
			m_fTime += fTimeDelta;
			if (m_fTime > 0.05f)
			{
				if (m_iNum >= 15)
					m_iNum = 15;
				else
				{
					m_iNum++;
				}
				m_fTime = 0.f;
			}
		}
		else
			m_iNum = 0;
	}


	if (UM->Get_Subway2())
	{
		m_fTime2 += fTimeDelta;
		if (m_fTime2 > 2.f)
		{
			m_fTime += fTimeDelta;
			if (m_fTime > 0.07f)
			{
				if (m_iNum >= 15)
					m_iNum = 15;
				else
				{
					m_iNum++;
				}
				m_fTime = 0.f;
			}
		}
		else
			m_iNum = 0;
	}


	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CMapNameEffect::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMapNameEffect::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", m_iNum)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;


	if (m_fTime2 > 2.f)
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}



	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CMapNameEffect::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MapNameEffect"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CMapNameEffect::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CMapNameEffect * CMapNameEffect::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMapNameEffect*		pInstance = new CMapNameEffect(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMapNameEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMapNameEffect::Clone(void * pArg)
{
	CMapNameEffect*		pInstance = new CMapNameEffect(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMapNameEffect"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CMapNameEffect::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
