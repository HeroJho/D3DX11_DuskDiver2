#include "stdafx.h"
#include "..\Public\Player04Slash2.h"
#include "GameInstance.h"
#include "Time_Manager.h"
#include "Camera_Manager.h"
#include "Player_Manager.h"
#include "Status_Manager.h"

CPlayer04Slash2::CPlayer04Slash2(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCreture(pDevice, pContext)
{
}

CPlayer04Slash2::CPlayer04Slash2(const CPlayer04Slash2& rhs)
	: CCreture(rhs)
{
}

HRESULT CPlayer04Slash2::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer04Slash2::Initialize(void * pArg)
{
	m_pInfo = (*(SLASHINFO**)pArg);

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_bDead = false;

	m_pTransformCom->Set_WorldMatrix(m_pInfo->Worldmatrix);
	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pInfo->vScale));

	m_fMaxUVIndexX = 0.f;
	m_fMaxUVIndexY = 0.f;
	m_vUVFixAcc = { 0.f,0.f };
	m_vUVFixSpeed = { 0.f,0.f };
	m_pTransformCom->TurnAngle(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(90.f));
	m_pTransformCom->TurnAngle(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(m_pInfo->fAngle));
	m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER04) * 1.5f;
	return S_OK;
}

void CPlayer04Slash2::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_EFFECT, this);
	fTimeDelta *= m_fTMRatio;

	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pTransformCom->Get_Scale()) + (XMLoadFloat3(&m_pInfo->vScaleSpeed) * fTimeDelta));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&PM->Get_PlayerPos()));

	if(PM->Get_PlayerJump())
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), m_pInfo->fTurnSpeed, fTimeDelta);
	else
	{
		m_pInfo->vRatio.x -= m_pInfo->vRatioSpeed.x * fTimeDelta;
		m_pInfo->vRatio.y -= m_pInfo->vRatioSpeed.y * fTimeDelta;

		if (m_pInfo->vRatio.x <= 0.f || m_pInfo->vRatio.y <= 0.f)
			Set_Dead();
	}
	m_fLifeTimeAcc += 1.f * fTimeDelta;
	if (m_fLifeTimeAcc >= m_pInfo->fLimitTime)
	{
		Set_Dead();
	}

	
}

void CPlayer04Slash2::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;
	
	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);	
	
}

HRESULT CPlayer04Slash2::Render()
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
		if (FAILED(m_pShaderCom->Set_RawValue("g_fMaxUVIndexX", &m_fMaxUVIndexX, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fMaxUVIndexY", &m_fMaxUVIndexY, sizeof(_float))))
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

void CPlayer04Slash2::Render_Debug()
{
	
}

void CPlayer04Slash2::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{

}

HRESULT CPlayer04Slash2::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Player04MeshEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Player04Slash"), TEXT("Player04Slash"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	return S_OK;
}

CCreture * CPlayer04Slash2::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer04Slash2*		pInstance = new CPlayer04Slash2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer04Slash2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer04Slash2::Clone(void * pArg)
{
	CPlayer04Slash2*		pInstance = new CPlayer04Slash2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer04Slash2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer04Slash2::Free()
{
	__super::Free();
	if(m_pInfo != nullptr)
		Safe_Delete(m_pInfo);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	

}
