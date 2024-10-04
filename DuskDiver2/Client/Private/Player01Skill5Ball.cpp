#include "stdafx.h"
#include "..\Public\Player01Skill5Ball.h"
#include "GameInstance.h"
#include "Time_Manager.h"
#include "Player01Ball.h"
#include "ToolManager.h"
#include "Player_Manager.h"
#include "Particle_Manager.h"

CPlayer01Skill5Ball::CPlayer01Skill5Ball(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEffect(pDevice, pContext)
{
}

CPlayer01Skill5Ball::CPlayer01Skill5Ball(const CPlayer01Skill5Ball& rhs)
	: CEffect(rhs)
{
}

HRESULT CPlayer01Skill5Ball::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer01Skill5Ball::Initialize(void * pArg)
{
	m_pInfo = (*(SKILL5BALLINFO**)pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_bDead = false;
	
	_matrix Matrix = XMMatrixIdentity();
	Matrix.r[3] = XMLoadFloat4(&m_pInfo->vLocalPos);
	XMStoreFloat4x4(&m_pInfo->vWorldMatrix, Matrix * XMLoadFloat4x4(&m_pInfo->vWorldMatrix));
	m_pTransformCom->Set_WorldMatrix(m_pInfo->vWorldMatrix);
	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pInfo->vScale));
	m_vOriginColor = m_pInfo->vColor;
	m_vOriginColor2 = m_pInfo->vColor2;

	return S_OK;
}

void CPlayer01Skill5Ball::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_EFFECT, this);
	fTimeDelta *= m_fTMRatio;

	m_fLifeTimeAcc += 1.f * fTimeDelta;

	if(m_bScaleFix)
	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pTransformCom->Get_Scale()) + (XMLoadFloat3(&m_pInfo->vScaleSpeed) * fTimeDelta));

	if (m_fLifeTimeAcc >= m_pInfo->fLimitTime)
	{
		Set_Dead();
	}


	if (!m_bEffect1 && m_fLifeTimeAcc >= 0.2f)
	{
		m_bEffect1 = true;
		m_pInfo->vColor = { 0.f,0.f,0.f,1.f };
		m_pInfo->vColor2 = { 0.f,0.f,0.f,1.f };
		_matrix a;
		a = XMLoadFloat4x4(&PM->Get_PlayerWorldMatrix());
		a.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float4x4 Matrix;
		XMStoreFloat4x4(&Matrix, a);
		
		if(!m_pInfo->bBurst)
			PTM->Create_AlphaParticle(L"Player01Skill5", Matrix);
		else
			PTM->Create_AlphaParticle(L"Player01Skill5B", Matrix);
		m_bScaleFix = false;
		m_vOriginScale1 = m_pTransformCom->Get_Scale();
		m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vOriginScale1) - _vector{ 0.5f,0.5f,0.5f });

	}

	if (!m_bEffect2 && m_fLifeTimeAcc >= 0.27f)
	{
		m_bEffect2 = true;
		m_pInfo->vColor = m_vOriginColor;
		m_pInfo->vColor2 = m_vOriginColor2;
		m_bScaleFix = true;
		m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vOriginScale1));
	}

	if (!m_bEffect3 && m_fLifeTimeAcc >= 0.5f)
	{
		m_bEffect3 = true;
		m_pInfo->vColor = { 0.f,0.f,0.f,1.f };
		m_pInfo->vColor2 = { 0.f,0.f,0.f,1.f };
		_matrix a;
		a = XMLoadFloat4x4(&PM->Get_PlayerWorldMatrix());
		a.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float4x4 Matrix;
		XMStoreFloat4x4(&Matrix, a);

		if (!m_pInfo->bBurst)
			PTM->Create_AlphaParticle(L"Player01Skill5", Matrix);
		else
			PTM->Create_AlphaParticle(L"Player01Skill5B", Matrix);
		m_bScaleFix = false;
		m_vOriginScale2 = m_pTransformCom->Get_Scale();
		m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vOriginScale2) - _vector{ 0.7f,0.7f,0.7f });
	}

	if (!m_bEffect4 && m_fLifeTimeAcc >= 0.57f)
	{
		m_bEffect4 = true;
		m_pInfo->vColor = m_vOriginColor;
		m_pInfo->vColor2 = m_vOriginColor2;
		m_bScaleFix = true;
		m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vOriginScale2));
	}

	if (!m_bEffect5 && m_fLifeTimeAcc >= 0.8f)
	{
		m_bEffect5 = true;
		m_pInfo->vColor = { 0.f,0.f,0.f,1.f };
		m_pInfo->vColor2 = { 0.f,0.f,0.f,1.f };
		_matrix a;
		a = XMLoadFloat4x4(&PM->Get_PlayerWorldMatrix());
		a.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float4x4 Matrix;
		XMStoreFloat4x4(&Matrix, a);

		if (!m_pInfo->bBurst)
			PTM->Create_AlphaParticle(L"Player01Skill5", Matrix);
		else
			PTM->Create_AlphaParticle(L"Player01Skill5B", Matrix);
		m_bScaleFix = false;
		m_vOriginScale3 = m_pTransformCom->Get_Scale();
		m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vOriginScale3) - _vector{ 1.5f,1.5f,1.5f });
	}

	if (!m_bEffect6 && m_fLifeTimeAcc >= 0.87f)
	{
		m_bEffect6 = true;
		m_pInfo->vColor = m_vOriginColor;
		m_pInfo->vColor2 = m_vOriginColor2;
		m_bScaleFix = true;
		m_pTransformCom->Set_Scale(XMLoadFloat3(&m_vOriginScale3));
	}

	
	
	
}

void CPlayer01Skill5Ball::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CPlayer01Skill5Ball::Render()
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
		if (FAILED(m_pModelCom->Render(m_pShaderCom,i, 3)))
			return E_FAIL;
	}
	return S_OK;
}

HRESULT CPlayer01Skill5Ball::Ready_Components()
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

CEffect * CPlayer01Skill5Ball::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer01Skill5Ball*		pInstance = new CPlayer01Skill5Ball(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer01Skill5Ball"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer01Skill5Ball::Clone(void * pArg)
{
	CPlayer01Skill5Ball*		pInstance = new CPlayer01Skill5Ball(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer01Skill5Ball"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer01Skill5Ball::Free()
{
	__super::Free();
	if(m_pInfo != nullptr)
	Safe_Delete(m_pInfo);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	

}
