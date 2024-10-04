#include "stdafx.h"
#include "..\Public\Wall.h"
#include "GameInstance.h"
#include "Player_Manager.h"
CWall::CWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEffect(pDevice, pContext)
{
}

CWall::CWall(const CWall& rhs)
	: CEffect(rhs)
{
}

HRESULT CWall::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	return S_OK;
}

HRESULT CWall::Initialize(void * pArg)
{
	m_WallInfo = (*(WALLINFO**)pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_fMaxUVIndexX = 1.f;
	m_fMaxUVIndexY = 4.f;
	m_fUVIndexX = 0.f;
	m_fUVIndexY = 0.f;
	m_fUVSpeed = 0.1f;
	m_bDead = false;
	
	m_pTransformCom->Set_Scale(_vector{ m_WallInfo->vSize.x, m_WallInfo->vSize.y, m_WallInfo->vSize.z });
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_WallInfo->vWorldPos));

	return S_OK;
}

void CWall::Tick(_float fTimeDelta)
{
	fTimeDelta *= m_fTMRatio;
	m_fLifeTimeAcc += 1.f * fTimeDelta;
	if (m_fLifeTimeAcc >= 1.f)
		Set_Dead();
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + (XMLoadFloat3(&m_WallInfo->vFixPosSpeed) * fTimeDelta));
	Set_UVIndex(fTimeDelta);
	Set_UVFix(fTimeDelta);
	
}

void CWall::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	

}

HRESULT CWall::Render()
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

		m_pShaderCom->Set_RawValue("g_fMaxUVIndexX", &m_fMaxUVIndexX, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_fMaxUVIndexY", &m_fMaxUVIndexY, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_fUVIndexX", &m_fUVIndexX, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_fUVIndexY", &m_fUVIndexY, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_fUVFixX", &m_vUVFixAcc.x, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_fUVFixY", &m_vUVFixAcc.y, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_vColor", &m_WallInfo->vColor, sizeof(_float4));
		m_pShaderCom->Set_RawValue("g_vColor2", &m_WallInfo->vColor2, sizeof(_float4));
		m_pShaderCom->Set_RawValue("g_fRatio1", &m_WallInfo->vRatio.x, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_fRatio2", &m_WallInfo->vRatio.y, sizeof(_float));

		if (FAILED(m_pModelCom->Render(m_pShaderCom,i, 2)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CWall::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_StaticMeshEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("OrangeWall"), TEXT("OrangeWall"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

CEffect * CWall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWall*		pInstance = new CWall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CWall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWall::Clone(void * pArg)
{
	CWall*		pInstance = new CWall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CWall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWall::Free()
{
	__super::Free();
	Safe_Delete(m_WallInfo);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	

}
