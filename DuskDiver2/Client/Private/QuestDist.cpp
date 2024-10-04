#include "stdafx.h"
#include "..\Public\QuestDist.h"
#include "GameInstance.h"
#include "UI_Manager.h"

CQuestDist::CQuestDist(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CUI(pDevice, pContext)
{
}

CQuestDist::CQuestDist(const CQuestDist & rhs)
	:CUI(rhs)
{
}

HRESULT CQuestDist::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CQuestDist::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	return S_OK;
}

void CQuestDist::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet((m_DistPos.x - g_iWinSizeX * 0.5f), (-m_DistPos.y + g_iWinSizeY * 0.5f), 0.0f, 1.f));
}

void CQuestDist::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);

	if (true == m_bRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_WORLDUI, this);
}

HRESULT CQuestDist::Render()
{
	if (true == m_bRender)
	{
		if (nullptr == m_pVIBufferCom ||
			nullptr == m_pShaderCom)
			return E_FAIL;

		m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
		m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
		m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", m_iNumIndex)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Begin(2)))
			return E_FAIL;

		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}
	
	//UI_RenderDebug();

	return S_OK;
}

void CQuestDist::Render_Debug()
{
}

void CQuestDist::Set_DistPos(_float2 vPos, _uint iNumber)
{
	m_DistPos = vPos;
	m_iNumIndex = iNumber;
	m_bRender = true;
}

HRESULT CQuestDist::Ready_Components()
{
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Number3"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}


CQuestDist * CQuestDist::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CQuestDist*		pInstance = new CQuestDist(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CQuestDist"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CQuestDist::Clone(void * pArg)
{
	CQuestDist*		pInstance = new CQuestDist(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CQuestDist"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CQuestDist::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
