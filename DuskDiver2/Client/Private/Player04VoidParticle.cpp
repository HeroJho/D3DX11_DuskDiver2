#include "stdafx.h"
#include "..\Public\Player04VoidParticle.h"
#include "GameInstance.h"
#include "Time_Manager.h"
#include "Player_Manager.h"

CPlayer04VoidParticle::CPlayer04VoidParticle(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer04VoidParticle::CPlayer04VoidParticle(const CPlayer04VoidParticle & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer04VoidParticle::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CPlayer04VoidParticle::Initialize(void * pArg)
{
	m_pInfo = (*(COLORINFO**)pArg);

	m_ParticleInfo = new PARTICLEINFO;
	m_ParticleInfo->bBlend = true;
	m_ParticleInfo->bFixColor = true;
	m_ParticleInfo->bUV = true;
	m_ParticleInfo->bUVFix = false;
	m_ParticleInfo->bUVLoof = false;
	m_ParticleInfo->fBlurPower = 1.f;
	m_ParticleInfo->fGravity = 0.f;
	m_ParticleInfo->fLifeTime = 3.f;
	m_ParticleInfo->fMaxUVIndexX = 4.f;
	m_ParticleInfo->fMaxUVIndexY = 4.f;
	m_ParticleInfo->fRotationRatio = 0.f;
	m_ParticleInfo->fRotationSpeed = 0.f;
	m_ParticleInfo->fRotationTime = 0.f;
	m_ParticleInfo->fSpeed = 0.f;
	m_ParticleInfo->fUVSpeed = 0.03f;
	m_ParticleInfo->iShaderPass = 7;
	m_ParticleInfo->TexName = L"EX_LOL_0089";
	m_ParticleInfo->TexPath = L"..Bin/Resources/Particle/EX_LOL_0089.png";
	m_ParticleInfo->vDirection = { 0.f,0.f,0.f };
	m_ParticleInfo->vRatio = { 1.f,1.f };
	m_ParticleInfo->vRatioSpeed = { 0.f,0.f };
	m_ParticleInfo->vScaleSpeed = { 0.f,0.f };
	m_ParticleInfo->vUVFixSpeed = { 0.f,0.f };
	m_ParticleInfo->WorldMatrix = PM->Get_PlayerWorldMatrix();

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_ParticleInfo->TexName, m_ParticleInfo->TexName, (CComponent**)&m_pTextureCom)))
		return E_FAIL;
	

	m_pTransformCom->Set_WorldMatrix(m_ParticleInfo->WorldMatrix);
	
	return S_OK;
}

void CPlayer04VoidParticle::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_PARTICLE, this);
	fTimeDelta *= m_fTMRatio;

	_float4 Pos = PM->Get_PlayerPos();
	Pos.y += m_pInfo->fY;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&Pos));

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

void CPlayer04VoidParticle::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;
	
	if(!m_bDead)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

}

HRESULT CPlayer04VoidParticle::Render()
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

	m_pShaderCom->Set_RawValue("g_fSizeX", &m_pInfo->vSize.x, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_fSizeY", &m_pInfo->vSize.y, sizeof(_float));
	
	m_pShaderCom->Set_RawValue("g_fMaxUVIndexX", &m_ParticleInfo->fMaxUVIndexX, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_fMaxUVIndexY", &m_ParticleInfo->fMaxUVIndexY, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_fUVIndexX", &m_fShaderUVIndexX, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_fUVIndexY", &m_fShaderUVIndexY, sizeof(_float));
	
	m_pShaderCom->Set_RawValue("g_bFixColor", &m_ParticleInfo->bFixColor, sizeof(_bool));
	m_pShaderCom->Set_RawValue("g_fRatio1", &m_ParticleInfo->vRatio.x, sizeof(_float));	
	m_pShaderCom->Set_RawValue("g_vColor", &m_pInfo->vColor, sizeof(_float4));
	m_pShaderCom->Set_RawValue("g_vColor2", &m_pInfo->vColor2, sizeof(_float4));
	m_pShaderCom->Set_RawValue("g_fRatio2", &m_ParticleInfo->vRatio.y, sizeof(_float));	
	m_pShaderCom->Set_RawValue("g_fBlurPower", &m_ParticleInfo->fBlurPower, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_fUVFixX", &m_vUVFixAcc.x, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_fUVFixY", &m_vUVFixAcc.y, sizeof(_float));
	
	if (FAILED(m_pShaderCom->Begin(m_ParticleInfo->iShaderPass)))
		return E_FAIL;
	
	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CPlayer04VoidParticle::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());
	


	ImGui::End();

}


HRESULT CPlayer04VoidParticle::Ready_Components()
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

CPlayer04VoidParticle * CPlayer04VoidParticle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer04VoidParticle*		pInstance = new CPlayer04VoidParticle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer04VoidParticle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer04VoidParticle::Clone(void * pArg)
{
	CPlayer04VoidParticle*		pInstance = new CPlayer04VoidParticle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer04VoidParticle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer04VoidParticle::Free()
{
	__super::Free();
	
	if (m_ParticleInfo != nullptr)
		Safe_Delete(m_ParticleInfo);

	if (m_pInfo != nullptr)
		Safe_Delete(m_pInfo);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);

}
