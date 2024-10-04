#include "stdafx.h"
#include "..\Public\AlphaParticle.h"
#include "GameInstance.h"
#include "Time_Manager.h"
#include "Player_Manager.h"

CAlphaParticle::CAlphaParticle(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CAlphaParticle::CAlphaParticle(const CAlphaParticle & rhs)
	: CGameObject(rhs)
{
}

HRESULT CAlphaParticle::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CAlphaParticle::Initialize(void * pArg)
{

	m_ParticleInfo = (*(PARTICLEINFO**)pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_ParticleInfo->TexName, m_ParticleInfo->TexName, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	_matrix Matrix = XMMatrixIdentity();
	Matrix.r[3] = XMLoadFloat4(&m_ParticleInfo->vLocalPos) + XMLoadFloat4(&m_ParticleInfo->vRandomPos);
	Matrix.r[3].m128_f32[3] = 1.f;
	XMStoreFloat4x4(&m_ParticleInfo->WorldMatrix, Matrix * XMLoadFloat4x4(&m_ParticleInfo->WorldMatrix));
	
	m_pTransformCom->Set_WorldMatrix(m_ParticleInfo->WorldMatrix);
	
	if (m_ParticleInfo->vDirection.x == 0 && m_ParticleInfo->vDirection.y == 0 && m_ParticleInfo->vDirection.z == 0)
		XMStoreFloat3(&m_ParticleInfo->vDirection, _vector{ m_ParticleInfo->WorldMatrix._41, m_ParticleInfo->WorldMatrix._42 , m_ParticleInfo->WorldMatrix._43 } -_vector{ 0.f,0.f,0.f });
	

	
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4x4(&m_ParticleInfo->WorldMatrix).r[3]);

	Make_ImGui();
	return S_OK;
}

void CAlphaParticle::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_PARTICLE, this);
	fTimeDelta *= m_fTMRatio;

	m_fLifeTimeAcc += 1.f * fTimeDelta;
	if (m_fLifeTimeAcc >= m_ParticleInfo->fLifeTime)
		Set_Dead();

	if (m_ParticleInfo->bFollow)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&PM->Get_PlayerPos()) + XMLoadFloat4(&m_ParticleInfo->vLocalPos) + XMLoadFloat4(&m_ParticleInfo->vRandomPos));
	}

	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_ParticleInfo->vDirection), m_ParticleInfo->fSpeed, fTimeDelta);

	if (m_ParticleInfo->fGravity > 0.f)
	{
		m_fGravityAcc += m_ParticleInfo->fGravity * fTimeDelta;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - _vector{ 0.f,m_fGravityAcc, 0.f,0.f });
	}

	if (m_ParticleInfo->bUV)
	{
		m_fShaderUVAcc += 1.f * fTimeDelta;
		if (m_fShaderUVAcc >= m_ParticleInfo->fUVSpeed)
		{
			m_fShaderUVAcc = 0.f;
			m_fShaderUVIndexX += 1.f;
			if (m_fShaderUVIndexX >= m_ParticleInfo->fMaxUVIndexX)
			{
				m_fShaderUVIndexX = 0.f;
				m_fShaderUVIndexY += 1.f;
				if (m_fShaderUVIndexY >= m_ParticleInfo->fMaxUVIndexY)
				{
					m_fShaderUVIndexY = 0.f;
					if (!m_ParticleInfo->bUVLoof)
						Set_Dead();
				}
			}
		}
	}

	if (m_ParticleInfo->iShaderPass == 8 || m_ParticleInfo->iShaderPass == 9)
	{		
		m_fRotationTimeAcc += 1.f * fTimeDelta;
		if (m_fRotationTimeAcc <= m_ParticleInfo->fRotationTime)
		{
			m_ParticleInfo->fRotationSpeed += m_ParticleInfo->fRotationRatio * fTimeDelta;
			m_fRotationAngle += m_ParticleInfo->fRotationSpeed * fTimeDelta;
		}
	}

	if (m_ParticleInfo->vRatioSpeed.x > 0.f)
		m_ParticleInfo->vRatio.x -= m_ParticleInfo->vRatioSpeed.x * fTimeDelta;

	if (m_ParticleInfo->vRatioSpeed.y > 0.f)
		m_ParticleInfo->vRatio.y -= m_ParticleInfo->vRatioSpeed.y * fTimeDelta;

	if (m_ParticleInfo->vScaleSpeed.x != 0.f)
	{
		m_ParticleInfo->vSize.x += m_ParticleInfo->vScaleSpeed.x * fTimeDelta;	
	}
	if (m_ParticleInfo->vScaleSpeed.y != 0.f)
	{
		m_ParticleInfo->vSize.y += m_ParticleInfo->vScaleSpeed.y * fTimeDelta;
	}
	if (m_ParticleInfo->vSize.x <= 0.f || m_ParticleInfo->vSize.y <= 0.f)
		Set_Dead();

	if (m_ParticleInfo->bUVFix)
	{
		m_vUVFixAcc.x += m_ParticleInfo->vUVFixSpeed.x * fTimeDelta;
		m_vUVFixAcc.y += m_ParticleInfo->vUVFixSpeed.y * fTimeDelta;
	}

}

void CAlphaParticle::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;
	
	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	if(!m_bDead)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

}

HRESULT CAlphaParticle::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &GI->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &GI->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &GI->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	
	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	if (m_ParticleInfo->iShaderPass == 8 || m_ParticleInfo->iShaderPass == 9)
	{
		XMStoreFloat3(&m_vCamLook, XMVector3Normalize(XMLoadFloat4(&GI->Get_CamPosition()) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));

		m_RotationMatrix = XMMatrixRotationAxis(XMLoadFloat3(&m_vCamLook), XMConvertToRadians(m_fRotationAngle));

		if (FAILED(m_pShaderCom->Set_RawValue("g_RotationMatrix", &m_RotationMatrix, sizeof(_matrix))))
			return E_FAIL;
	}

	m_pShaderCom->Set_RawValue("g_fSizeX", &m_ParticleInfo->vSize.x, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_fSizeY", &m_ParticleInfo->vSize.y, sizeof(_float));
	
	if (m_ParticleInfo->bUV)
	{
		m_pShaderCom->Set_RawValue("g_fMaxUVIndexX", &m_ParticleInfo->fMaxUVIndexX, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_fMaxUVIndexY", &m_ParticleInfo->fMaxUVIndexY, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_fUVIndexX", &m_fShaderUVIndexX, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_fUVIndexY", &m_fShaderUVIndexY, sizeof(_float));
	}

	m_pShaderCom->Set_RawValue("g_bFixColor", &m_ParticleInfo->bFixColor, sizeof(_bool));
	m_pShaderCom->Set_RawValue("g_fRatio1", &m_ParticleInfo->vRatio.x, sizeof(_float));
	if (m_ParticleInfo->bFixColor)
	{
		m_pShaderCom->Set_RawValue("g_vColor", &m_ParticleInfo->vColor, sizeof(_float4));
		m_pShaderCom->Set_RawValue("g_vColor2", &m_ParticleInfo->vColor2, sizeof(_float4));
		m_pShaderCom->Set_RawValue("g_fRatio2", &m_ParticleInfo->vRatio.y, sizeof(_float));
	}
	m_pShaderCom->Set_RawValue("g_fBlurPower", &m_ParticleInfo->fBlurPower, sizeof(_float));

	m_pShaderCom->Set_RawValue("g_fUVFixX", &m_vUVFixAcc.x, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_fUVFixY", &m_vUVFixAcc.y, sizeof(_float));
	
	if (FAILED(m_pShaderCom->Begin(m_ParticleInfo->iShaderPass)))
		return E_FAIL;
	

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CAlphaParticle::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());
	


	ImGui::End();

}


HRESULT CAlphaParticle::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AlphaParticle"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

CAlphaParticle * CAlphaParticle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CAlphaParticle*		pInstance = new CAlphaParticle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CAlphaParticle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CAlphaParticle::Clone(void * pArg)
{
	CAlphaParticle*		pInstance = new CAlphaParticle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CAlphaParticle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAlphaParticle::Free()
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
