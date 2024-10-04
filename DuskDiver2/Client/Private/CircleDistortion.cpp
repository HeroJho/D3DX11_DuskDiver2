#include "stdafx.h"
#include "..\Public\CircleDistortion.h"
#include "GameInstance.h"
#include "Time_Manager.h"
#include "Player01Ball.h"
#include "ToolManager.h"
#include "Player_Manager.h"
#include "Particle_Manager.h"

CCircleDistortion::CCircleDistortion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEffect(pDevice, pContext)
{
}

CCircleDistortion::CCircleDistortion(const CCircleDistortion& rhs)
	: CEffect(rhs)
{
}

HRESULT CCircleDistortion::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCircleDistortion::Initialize(void * pArg)
{
	m_pInfo = (*(CIRCLEDISTORTIONINFO**)pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_bDead = false;
	
	_matrix Matrix = XMMatrixIdentity();
	Matrix.r[3] = XMLoadFloat4(&m_pInfo->vLocalPos);
	XMStoreFloat4x4(&m_pInfo->vWorldMatrix, Matrix * XMLoadFloat4x4(&m_pInfo->vWorldMatrix));
	m_pTransformCom->Set_WorldMatrix(m_pInfo->vWorldMatrix);
	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pInfo->vScale));


	return S_OK;
}

void CCircleDistortion::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_EFFECT, this);
	fTimeDelta *= m_fTMRatio;

	m_fLifeTimeAcc += 1.f * fTimeDelta;

	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pTransformCom->Get_Scale()) + (XMLoadFloat3(&m_pInfo->vScaleSpeed) * fTimeDelta));

	if (XMVector3Length(XMLoadFloat3(&m_pTransformCom->Get_Scale())).m128_f32[0] <= 0.1f)
		Set_Dead();

	m_pInfo->vRatio.x -= m_pInfo->vRatioSpeed.x * fTimeDelta;
	m_pInfo->vRatio.y -= m_pInfo->vRatioSpeed.y * fTimeDelta;

	if (m_pInfo->vRatio.x <= 0.f || m_pInfo->vRatio.y <= 0.f)
		Set_Dead();

	if (m_fLifeTimeAcc >= m_pInfo->fLimitTime)
	{
		Set_Dead();
	}

	
}

void CCircleDistortion::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;
	
	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DISTORTION, this);
	
}

HRESULT CCircleDistortion::Render()
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

		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor2", &m_vColor2, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio1", &m_pInfo->vRatio.x, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio2", &m_pInfo->vRatio.y, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fBlurPower", &m_pInfo->fBlurPower, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Render(m_pShaderCom,i, 4)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CCircleDistortion::Ready_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_BlastBall"), TEXT("Player01Ball"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

CEffect * CCircleDistortion::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCircleDistortion*		pInstance = new CCircleDistortion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCircleDistortion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCircleDistortion::Clone(void * pArg)
{
	CCircleDistortion*		pInstance = new CCircleDistortion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCircleDistortion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCircleDistortion::Free()
{
	__super::Free();
	if(m_pInfo != nullptr)
	Safe_Delete(m_pInfo);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	

}
