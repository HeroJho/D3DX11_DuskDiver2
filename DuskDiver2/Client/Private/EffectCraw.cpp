#include "stdafx.h"
#include "..\Public\EffectCraw.h"
#include "GameInstance.h"

CEffectCraw::CEffectCraw(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEffect(pDevice, pContext)
{
}

CEffectCraw::CEffectCraw(const CEffectCraw& rhs)
	: CEffect(rhs)
{
}

HRESULT CEffectCraw::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEffectCraw::Initialize(void * pArg)
{
	m_CrawInfo = (*(CRAWINFO**)pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_bDead = false;
	
	m_pTransformCom->Set_WorldMatrix(m_CrawInfo->vWorldMatrix);

	XMStoreFloat4(&m_vMyPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pTransformCom->Set_Scale(XMVectorSet(m_CrawInfo->fScale.x, m_CrawInfo->fScale.y, m_CrawInfo->fScale.z, 1.f));
	return S_OK;
}

void CEffectCraw::Tick(_float fTimeDelta)
{
	
	fTimeDelta *= m_fTMRatio;
	
	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), m_CrawInfo->fSpeed, fTimeDelta);


	if (m_CrawInfo->vRatio.x > 0.f && m_CrawInfo->vRatio.y > 0.f)
	{
		m_CrawInfo->vRatio.x -= m_CrawInfo->vRatioSpeed.x * fTimeDelta;
		m_CrawInfo->vRatio.y -= m_CrawInfo->vRatioSpeed.y * fTimeDelta;
	}

	if (m_CrawInfo->vRatio.x <= 0.f || m_CrawInfo->vRatio.y <= 0.f)
	{
		Set_Dead();
	}
}

void CEffectCraw::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);	

}

HRESULT CEffectCraw::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_CrawInfo->vColor, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor2", &m_CrawInfo->vColor2, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio1", &m_CrawInfo->vRatio.x, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio2", &m_CrawInfo->vRatio.y, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fBlurPower", &m_CrawInfo->fBlurPower, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Render(m_pShaderCom,i, 1)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CEffectCraw::Ready_Components()
{
	/* For.Com_Transform */

	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = 1.f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MonsterMeshEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	switch (m_CrawInfo->iIndex)
	{
	case 0:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_Claw"), TEXT("Effect_Claw"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 1:

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Boss_Claw"), TEXT("Effect_Claw"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;

	case 2:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Beast_Claw"), TEXT("Effect_Claw"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	}

	return S_OK;
}

CEffect * CEffectCraw::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEffectCraw*		pInstance = new CEffectCraw(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEffectCraw"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffectCraw::Clone(void * pArg)
{
	CEffectCraw*		pInstance = new CEffectCraw(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEffectCraw"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffectCraw::Free()
{
	__super::Free();
	Safe_Delete(m_CrawInfo);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	

}
