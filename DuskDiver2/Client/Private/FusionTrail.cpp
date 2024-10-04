#include "stdafx.h"
#include "..\Public\FusionTrail.h"
#include "GameInstance.h"

CFusionTrail::CFusionTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEffect(pDevice, pContext)
{
}

CFusionTrail::CFusionTrail(const CFusionTrail& rhs)
	: CEffect(rhs)
{
}

HRESULT CFusionTrail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFusionTrail::Initialize(void * pArg)
{
	m_TrailInfo = (*(TRAILINFO**)pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_bDead = false;
	
	m_pTransformCom->Set_WorldMatrix(m_TrailInfo->vWorldMatrix);

	m_pTransformCom->Set_Scale(XMVectorSet(m_TrailInfo->fScale.x, m_TrailInfo->fScale.y, m_TrailInfo->fScale.z, 1.f));

	return S_OK;
}

void CFusionTrail::Tick(_float fTimeDelta)
{
	
	fTimeDelta *= m_fTMRatio;

	
		if (m_TrailInfo->vRatio.x > 0.f && m_TrailInfo->vRatio.y > 0.f)
		{
			m_TrailInfo->vRatio.x -= m_TrailInfo->vRatioSpeed.x * fTimeDelta;
			m_TrailInfo->vRatio.y -= m_TrailInfo->vRatioSpeed.y * fTimeDelta;
		}

		if (m_TrailInfo->vRatio.x <= 0.f || m_TrailInfo->vRatio.y <= 0.f)
		{
			Set_Dead();
		}
	
		

}

void CFusionTrail::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CFusionTrail::Render()
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

		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_TrailInfo->vColor, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor2", &m_TrailInfo->vColor2, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio1", &m_TrailInfo->vRatio.x, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio2", &m_TrailInfo->vRatio.y, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fBlurPower", &m_TrailInfo->fBlurPower, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Render(m_pShaderCom,i, 1)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CFusionTrail::SetUp_State(_fmatrix StateMatrix)
{

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, StateMatrix.r[0]);
	m_pTransformCom->Set_State(CTransform::STATE_UP, StateMatrix.r[1]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, StateMatrix.r[2]);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, StateMatrix.r[3]);

	return S_OK;
}

HRESULT CFusionTrail::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MonsterMeshEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_Blade1"), TEXT("Effect_Blade1"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

CEffect * CFusionTrail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFusionTrail*		pInstance = new CFusionTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CFusionTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFusionTrail::Clone(void * pArg)
{
	CFusionTrail*		pInstance = new CFusionTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CFusionTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFusionTrail::Free()
{
	__super::Free();
	Safe_Delete(m_TrailInfo);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	

}
