#include "stdafx.h"
#include "..\Public\ParticleInstance.h"
#include "GameInstance.h"
#include "Time_Manager.h"
#include "Player_Manager.h"

CParticleInstance::CParticleInstance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CParticleInstance::CParticleInstance(const CParticleInstance & rhs)
	: CGameObject(rhs)
{
}

HRESULT CParticleInstance::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CParticleInstance::Initialize(void * pArg)
{
	m_ParticleInfo = (*(PARTICLEINSTANCEINFO**)pArg);
	

	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_ParticleInfo->TexName, m_ParticleInfo->TexName, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	_matrix Matrix = XMMatrixIdentity();
	Matrix.r[3] = XMLoadFloat4(&m_ParticleInfo->vLocalPos);
	XMStoreFloat4x4(&m_ParticleInfo->WorldMatrix, Matrix * XMLoadFloat4x4(&m_ParticleInfo->WorldMatrix));
	
	m_pTransformCom->Set_WorldMatrix(m_ParticleInfo->WorldMatrix);

	return S_OK;
}

void CParticleInstance::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_PARTICLE, this);
	fTimeDelta *= m_fTMRatio;

	m_fLifeTimeAcc += 1.f * fTimeDelta;
	if (m_fLifeTimeAcc >= m_ParticleInfo->fMaxLifeTime)
		Set_Dead();

	if (m_ParticleInfo->bFollow)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&PM->Get_PlayerPos()) + XMLoadFloat4(&m_ParticleInfo->vLocalPos));
	}

	if (m_ParticleInfo->bUV)
	{
		m_fShaderUVAcc += 1.f * fTimeDelta;
		if (m_fShaderUVAcc >= m_ParticleInfo->fUVSpeed)
		{
			m_fShaderUVAcc = 0.f;
			m_fShaderUVIndexX += 1.f;
			if (m_fShaderUVIndexX >= m_ParticleInfo->fUVMaxIndexX)
			{
				m_fShaderUVIndexX = 0.f;
				m_fShaderUVIndexY += 1.f;
				if (m_fShaderUVIndexY >= m_ParticleInfo->fUVMaxIndexY)
				{
					m_fShaderUVIndexY = 0.f;
					if (!m_ParticleInfo->bUVLoof)
						Set_Dead();
				}
			}
		}
	}

	if (m_ParticleInfo->bFixColor)
	{
		if (m_ParticleInfo->vRatioSpeed.x > 0.f)
			m_ParticleInfo->vRatio.x -= m_ParticleInfo->vRatioSpeed.x * fTimeDelta;

		if (m_ParticleInfo->vRatioSpeed.y > 0.f)
			m_ParticleInfo->vRatio.y -= m_ParticleInfo->vRatioSpeed.y * fTimeDelta;

		if (m_ParticleInfo->vRatio.x <= 0.f || m_ParticleInfo->vRatio.y <= 0.f)
			Set_Dead();
	}

	m_fGravityAcc += 1.f * m_ParticleInfo->fGravity;

	m_pVIBufferCom->Update(m_fLifeTimeAcc, m_fGravityAcc, m_ParticleInfo->fChargeRange, fTimeDelta);

}

void CParticleInstance::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	if (!m_bDead)
	{
		if (m_ParticleInfo->bDistortion)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DISTORTION, this);
		else
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}


}

HRESULT CParticleInstance::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &GI->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &GI->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_vCamPosition", &GI->Get_CamPosition(), sizeof(_float4));

	if (m_pTextureCom != nullptr)
	{
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture")))
			return E_FAIL;
	}

	m_pShaderCom->Set_RawValue("g_fBlurPower", &m_ParticleInfo->fBlurPower, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_fAlphaTest", &m_ParticleInfo->fAlphaTest, sizeof(_float));

	if (m_ParticleInfo->bFixColor)
	{
		m_pShaderCom->Set_RawValue("g_vColor", &m_ParticleInfo->vColor, sizeof(_float4));
		m_pShaderCom->Set_RawValue("g_vColor2", &m_ParticleInfo->vColor2, sizeof(_float4));
		m_pShaderCom->Set_RawValue("g_fRatio1", &m_ParticleInfo->vRatio.x, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_fRatio2", &m_ParticleInfo->vRatio.y, sizeof(_float));
	}


	if (m_ParticleInfo->bUV)
	{
		m_pShaderCom->Set_RawValue("g_fMaxUVIndexX", &m_ParticleInfo->fUVMaxIndexX, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_fMaxUVIndexY", &m_ParticleInfo->fUVMaxIndexY, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_fUVIndexX", &m_fShaderUVIndexX, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_fUVIndexY", &m_fShaderUVIndexY, sizeof(_float));	
	}

	if (m_ParticleInfo->bDistortion)
	{
		m_pShaderCom->Set_RawValue("g_fDistortionPower", &m_ParticleInfo->fDistortionPower, sizeof(_float));
	}

	if (FAILED(m_pShaderCom->Begin(m_ParticleInfo->iShaderPass)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}


HRESULT CParticleInstance::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ParticleInstance"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	/* For.Com_VIBuffer */

	PTINSTANCEINFO* PTInfo = new PTINSTANCEINFO;
	PTInfo->vMinPos = m_ParticleInfo->vMinPos;
	PTInfo->vMaxPos = m_ParticleInfo->vMaxPos;
	PTInfo->vMinSize = m_ParticleInfo->vMinSize;
	PTInfo->vMaxSize = m_ParticleInfo->vMaxSize;
	PTInfo->vDirAngle = m_ParticleInfo->vDirAngle;
	PTInfo->vRangeAngle1 = m_ParticleInfo->vRangeAngle1;
	PTInfo->vRangeAngle2 = m_ParticleInfo->vRangeAngle2;
	PTInfo->iNumInstance = m_ParticleInfo->iParticleNum;
	PTInfo->vReleaseSpeed = m_ParticleInfo->vReleaseSpeed;
	PTInfo->fMinSpeed = m_ParticleInfo->fMinSpeed;
	PTInfo->fMaxSpeed = m_ParticleInfo->fMaxSpeed;
	PTInfo->fMinLifeTime = m_ParticleInfo->fMinLifeTime;
	PTInfo->fMaxLifeTime = m_ParticleInfo->fMaxLifeTime;
	PTInfo->fGravity = m_ParticleInfo->fGravity;
	PTInfo->bCharge = m_ParticleInfo->bCharge;
	PTInfo->vChargePos = m_ParticleInfo->vChargePos;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle_Instance"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &PTInfo)))
		return E_FAIL;

	return S_OK;
}

CParticleInstance * CParticleInstance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CParticleInstance*		pInstance = new CParticleInstance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CParticleInstance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CParticleInstance::Clone(void * pArg)
{
	CParticleInstance*		pInstance = new CParticleInstance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CParticleInstance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParticleInstance::Free()
{
	__super::Free();
	
	if (m_ParticleInfo != nullptr)
		Safe_Delete(m_ParticleInfo);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);

}
