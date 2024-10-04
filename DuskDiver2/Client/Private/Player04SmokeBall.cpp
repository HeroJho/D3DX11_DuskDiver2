#include "stdafx.h"
#include "..\Public\Player04SmokeBall.h"
#include "GameInstance.h"
#include "Time_Manager.h"

CPlayer04SmokeBall::CPlayer04SmokeBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCreture(pDevice, pContext)
{
}

CPlayer04SmokeBall::CPlayer04SmokeBall(const CPlayer04SmokeBall& rhs)
	: CCreture(rhs)
{
}

HRESULT CPlayer04SmokeBall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer04SmokeBall::Initialize(void * pArg)
{
	m_pInfo = (*(SMOKEBALLINFO**)pArg);

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_bDead = false;
	
	_matrix Matrix = XMMatrixIdentity();
	Matrix.r[3] = XMLoadFloat4(&m_pInfo->vLocalPos);
	XMStoreFloat4x4(&m_pInfo->vWorldMatrix, Matrix * XMLoadFloat4x4(&m_pInfo->vWorldMatrix));
	m_pTransformCom->Set_WorldMatrix(m_pInfo->vWorldMatrix);
	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pInfo->vScale));
	m_vUVFixAcc = { 0.f,0.f };

	return S_OK;
}

void CPlayer04SmokeBall::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_EFFECT, this);
	fTimeDelta *= m_fTMRatio;

	m_fTimeLimitAcc += 1.f * fTimeDelta;
	m_vUVFixAcc.x += m_pInfo->vUvFixSpeed.x * fTimeDelta;
	m_vUVFixAcc.y += m_pInfo->vUvFixSpeed.y * fTimeDelta;
	m_pTransformCom->Go_Dir(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_pInfo->fSpeed, fTimeDelta);


	

	if (m_fTimeLimitAcc >= m_pInfo->fTimeLimit)
	{
		
		

		if (m_pInfo->vRatio.x > 0.f)
		{
			m_pInfo->vRatio.x -= m_pInfo->vRatioSpeed.x * fTimeDelta;
			m_pInfo->vRatio.y -= m_pInfo->vRatioSpeed.y * fTimeDelta;
		}

		if (m_pInfo->vRatio.x <= 0.f || m_pInfo->vRatio.y <= 0.f)
		{
			Set_Dead();
		}
	}
	
	
}

void CPlayer04SmokeBall::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NOBLUR, this);
}

HRESULT CPlayer04SmokeBall::Render()
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

		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_pInfo->vColor, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor2", &m_pInfo->vColor2, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio1", &m_pInfo->vRatio.x, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio2", &m_pInfo->vRatio.y, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fBlurPower", &m_pInfo->fBlurPower, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixX", &m_vUVFixAcc.x, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixY", &m_vUVFixAcc.y, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Render(m_pShaderCom,i, 2)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CPlayer04SmokeBall::Ready_Components()
{

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Player04MeshEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_SmokeBall"), TEXT("Effect_SmokeBall"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

CCreture * CPlayer04SmokeBall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer04SmokeBall*		pInstance = new CPlayer04SmokeBall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer04SmokeBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer04SmokeBall::Clone(void * pArg)
{
	CPlayer04SmokeBall*		pInstance = new CPlayer04SmokeBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer04SmokeBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer04SmokeBall::Free()
{
	__super::Free();
	if(m_pInfo != nullptr)
	Safe_Delete(m_pInfo);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	

}
