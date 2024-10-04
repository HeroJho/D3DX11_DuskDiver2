#include "stdafx.h"
#include "..\Public\MonsterSpawn.h"
#include "GameInstance.h"
#include "Player_Manager.h"
CMonsterSpawn::CMonsterSpawn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEffect(pDevice, pContext)
{
}

CMonsterSpawn::CMonsterSpawn(const CMonsterSpawn& rhs)
	: CEffect(rhs)
{
}

HRESULT CMonsterSpawn::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	return S_OK;
}

HRESULT CMonsterSpawn::Initialize(void * pArg)
{
	m_SpawnInfo = (*(SPAWNINFO**)pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;
	

	_matrix Matrix = XMMatrixIdentity();
	Matrix.r[3] = XMLoadFloat4(&m_SpawnInfo->vLocalPos);
	XMStoreFloat4x4(&m_SpawnInfo->vWorldMatrix, Matrix * XMLoadFloat4x4(&m_SpawnInfo->vWorldMatrix));
	m_pTransformCom->Set_WorldMatrix(m_SpawnInfo->vWorldMatrix);


	m_vUVFixSpeed = m_SpawnInfo->vUVFixSpeed;

	m_vUVFixAcc = { 0.f,0.f };

	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_SpawnInfo->vScale));


	/*m_fMaxUVIndexX = 1.f;
	m_fMaxUVIndexY = 1.f;
	m_fUVIndexX = 0.f;
	m_fUVIndexY = 0.f;
	m_fUVSpeed = 0.1f;
	m_bDead = false;
	m_vUVFixSpeed = { 0.f, 1.f };
	m_vUVFixAcc = { 0.f, 0.f };*/


	//m_pTransformCom->Set_Scale(_vector{ m_SpawnInfo->vSize.x, m_SpawnInfo->vSize.y, m_SpawnInfo->vSize.z });
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_SpawnInfo->vWorldPos));

	return S_OK;
}

void CMonsterSpawn::Tick(_float fTimeDelta)
{
	fTimeDelta *= m_fTMRatio;

	m_fLifeTimeAcc += 0.1f * fTimeDelta;

	if (m_fLifeTimeAcc >= 0.1f)
	{
		if (m_SpawnInfo->vRatio.x > 0.f && m_SpawnInfo->vRatio.y > 0.f)
		{
			m_SpawnInfo->vRatio.x -= m_SpawnInfo->vRatioSpeed.x * fTimeDelta;
			m_SpawnInfo->vRatio.y -= m_SpawnInfo->vRatioSpeed.y * fTimeDelta;
		}
		if (m_SpawnInfo->vRatio.x <= 0.f || m_SpawnInfo->vRatio.y <= 0.f)
		{
			Set_Dead();
		}
	}

	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pTransformCom->Get_Scale()) + (XMLoadFloat3(&m_SpawnInfo->vScaleSpeed) * fTimeDelta));



	Set_UVFix(fTimeDelta);

	
}

void CMonsterSpawn::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	

}

HRESULT CMonsterSpawn::Render()
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

		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_SpawnInfo->vColor, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor2", &m_SpawnInfo->vColor2, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio1", &m_SpawnInfo->vRatio.x, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio2", &m_SpawnInfo->vRatio.y, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fBlurPower", &m_SpawnInfo->fBlurPower, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixX", &m_vUVFixAcc.x, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixY", &m_vUVFixAcc.y, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 3)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CMonsterSpawn::Ready_Components()
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

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("MonsterSpawn1"), TEXT("MonsterSpawn1"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

CEffect * CMonsterSpawn::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonsterSpawn*		pInstance = new CMonsterSpawn(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMonsterSpawn"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMonsterSpawn::Clone(void * pArg)
{
	CMonsterSpawn*		pInstance = new CMonsterSpawn(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMonsterSpawn"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMonsterSpawn::Free()
{
	__super::Free();
	Safe_Delete(m_SpawnInfo);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	

}
