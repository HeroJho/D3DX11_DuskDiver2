#include "stdafx.h"
#include "..\Public\Player01Trail_1.h"
#include "GameInstance.h"
#include "Time_Manager.h"

CPlayer01Trail_1::CPlayer01Trail_1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEffect(pDevice, pContext)
{
}

CPlayer01Trail_1::CPlayer01Trail_1(const CPlayer01Trail_1& rhs)
	: CEffect(rhs)
{
}

HRESULT CPlayer01Trail_1::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer01Trail_1::Initialize(void * pArg)
{
	m_TrailInfo = (*(TRAILINFO**)pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_bDead = false;
	
	m_pTransformCom->Set_WorldMatrix(m_TrailInfo->vWorldMatrix);


	return S_OK;
}

void CPlayer01Trail_1::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_EFFECT, this);
	fTimeDelta *= m_fTMRatio;

	if (m_TrailInfo->vRatio.x > 0.f && m_TrailInfo->vRatio.y > 0.f)
	{
		m_TrailInfo->vRatio.x -= m_TrailInfo->vRatioSpeed.x * fTimeDelta;
		m_TrailInfo->vRatio.y -= m_TrailInfo->vRatioSpeed.y * fTimeDelta;
	}
	
	if(m_TrailInfo->bScaleFix)
		m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pTransformCom->Get_Scale()) + (XMLoadFloat3(&m_TrailInfo->vScaleSpeed) * fTimeDelta));

	if (m_TrailInfo->vRatio.x <= 0.f || m_TrailInfo->vRatio.y <= 0.f)
	{
		Set_Dead();
	}
}

void CPlayer01Trail_1::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	if(!m_TrailInfo->bDistortion)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	else
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DISTORTION, this);
}

HRESULT CPlayer01Trail_1::Render()
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
		if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixX", &m_vUVFixAcc.x, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixY", &m_vUVFixAcc.y, sizeof(_float))))
			return E_FAIL;
		if (!m_TrailInfo->bDistortion)
		{
			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 3)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 4)))
				return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CPlayer01Trail_1::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Player01MeshEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Player01Trail_1"), TEXT("Player01Trail_1"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

CEffect * CPlayer01Trail_1::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer01Trail_1*		pInstance = new CPlayer01Trail_1(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer01Trail_1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer01Trail_1::Clone(void * pArg)
{
	CPlayer01Trail_1*		pInstance = new CPlayer01Trail_1(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer01Trail_1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer01Trail_1::Free()
{
	__super::Free();
	Safe_Delete(m_TrailInfo);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	

}
