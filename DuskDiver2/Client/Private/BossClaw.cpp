#include "stdafx.h"
#include "..\Public\BossClaw.h"
#include "GameInstance.h"
#include "Time_Manager.h"

CBossClaw::CBossClaw(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEffect(pDevice, pContext)
{
}

CBossClaw::CBossClaw(const CBossClaw& rhs)
	: CEffect(rhs)
{
}

HRESULT CBossClaw::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBossClaw::Initialize(void * pArg)
{
	m_CrawInfo = (*(CRAWINFO**)pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_bDead = false;
	
	_matrix Matrix = XMMatrixIdentity();
	Matrix.r[3] = XMLoadFloat4(&m_CrawInfo->vLocalPos);
	_float4x4 LocalMatrix;
	XMStoreFloat4x4(&LocalMatrix, Matrix);
	m_pTransformCom->Set_WorldMatrix(LocalMatrix);
	m_pTransformCom->TurnAngle(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(m_CrawInfo->vRotation.x));
	m_pTransformCom->TurnAngle(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(m_CrawInfo->vRotation.y));
	m_pTransformCom->TurnAngle(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(m_CrawInfo->vRotation.z));
	XMStoreFloat4x4(&m_CrawInfo->vWorldMatrix, m_pTransformCom->Get_WorldMatrix() * XMLoadFloat4x4(&m_CrawInfo->vWorldMatrix));
	m_pTransformCom->Set_WorldMatrix(m_CrawInfo->vWorldMatrix);


	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_CrawInfo->vScale));

	

	return S_OK;
}

void CBossClaw::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_EFFECT, this);
	fTimeDelta *= m_fTMRatio;

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

void CBossClaw::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);	

}

HRESULT CBossClaw::Render()
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


HRESULT CBossClaw::Ready_Components()
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


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Beast_Claw"), TEXT("Effect_Claw"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	return S_OK;
}

CEffect * CBossClaw::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBossClaw*		pInstance = new CBossClaw(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBossClaw"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBossClaw::Clone(void * pArg)
{
	CBossClaw*		pInstance = new CBossClaw(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBossClaw"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossClaw::Free()
{
	__super::Free();
	Safe_Delete(m_CrawInfo);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	

}
