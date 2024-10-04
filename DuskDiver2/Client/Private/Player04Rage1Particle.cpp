#include "stdafx.h"
#include "..\Public\Player04Rage1Particle.h"
#include "GameInstance.h"
#include "Time_Manager.h"
#include "Player_Manager.h"
#include "Effect_Manager.h"

CPlayer04Rage1Particle::CPlayer04Rage1Particle(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer04Rage1Particle::CPlayer04Rage1Particle(const CPlayer04Rage1Particle & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer04Rage1Particle::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CPlayer04Rage1Particle::Initialize(void * pArg)
{
	m_CreateInfo = (*(RAGEPARTICLEINFO**)pArg);

	m_ParticleInfo = new PARTICLEINFO;
	m_ParticleInfo->bFixColor = true;
	m_ParticleInfo->fBlurPower = 0.f;
	m_ParticleInfo->fLifeTime = 10.f;
	m_ParticleInfo->fMaxUVIndexX = 8.f;
	m_ParticleInfo->fMaxUVIndexY = 8.f;
	m_ParticleInfo->fRotationRatio = 0.f;
	m_ParticleInfo->fRotationSpeed = 50.f;
	m_ParticleInfo->fRotationTime = 10.f;
	m_ParticleInfo->fSpeed =  GI->Get_FloatRandom(1.f,4.f);
	m_ParticleInfo->fUVSpeed = 0.01f;
	m_ParticleInfo->iShaderPass = 9;
	m_ParticleInfo->TexName = L"Player04Rage1";
	m_ParticleInfo->TexPath = L"..Bin/Resources/Particle/Player04Rage1.png";
	m_iIndex = GI->Get_Random(0, 1);
	if (m_iIndex == 0)
	{
		m_ParticleInfo->vColor = { 0.f,0.f,0.f,1.f };
		m_ParticleInfo->vColor2 = { 0.4f,0.f,1.f,1.f };
	}
	else
	{
		m_ParticleInfo->vColor = { 0.f,0.f,0.f,1.f };
		m_ParticleInfo->vColor2 = { 0.4f,0.f,0.f,1.f };
	}
	m_ParticleInfo->vRatio = { 1.f,2.f };
	m_ParticleInfo->vRatioSpeed = { 0.f,0.f };
	_float fSize = GI->Get_FloatRandom(0.5f, 1.5f);
	m_ParticleInfo->vSize = { fSize,fSize };

	GI->PlaySoundW(L"SE_440.wav", SD_PLAYER4_SK, 0.8f);
	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_ParticleInfo->TexName, m_ParticleInfo->TexName, (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	_matrix Identity = XMMatrixIdentity();
	Identity.r[3] = XMLoadFloat4(&m_CreateInfo->vCreatePos);
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, Identity);
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);

	
	
	return S_OK;
}

void CPlayer04Rage1Particle::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_PARTICLE, this);
	fTimeDelta *= m_fTMRatio;
	XMStoreFloat3(&m_vTargetLook, XMVector3Normalize(XMLoadFloat4(&EM->Get_Player04WhiteBallPos()) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), m_ParticleInfo->fSpeed, fTimeDelta);
	
	if (XMVectorGetX(XMVector4Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - XMLoadFloat4(&EM->Get_Player04WhiteBallPos()))) <= 0.1f)
	{
		EM->Release_RageParticle();
		EM->Create_RageParticleEffect(m_iIndex);
		Set_Dead();
		if (!m_bCombine)
		{
			GI->StopSound(SD_PLAYER4_SK);
			GI->PlaySoundW(L"SE_BT_BOSS11_SKILL12_1.wav", SD_PLAYER4_SK, 1.f);
			m_bCombine = true;
		}
	}

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
			}
		}
	}

}

void CPlayer04Rage1Particle::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;
	
	if(!m_bDead)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

}

HRESULT CPlayer04Rage1Particle::Render()
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

	XMStoreFloat3(&m_vCamLook, XMVector3Normalize(XMLoadFloat4(&GI->Get_CamPosition()) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));

	m_RotationMatrix = XMMatrixRotationAxis(XMLoadFloat3(&m_vCamLook), XMConvertToRadians(m_fRotationAngle));

	if (FAILED(m_pShaderCom->Set_RawValue("g_RotationMatrix", &m_RotationMatrix, sizeof(_matrix))))
		return E_FAIL;

	m_pShaderCom->Set_RawValue("g_fSizeX", &m_ParticleInfo->vSize.x, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_fSizeY", &m_ParticleInfo->vSize.y, sizeof(_float));
	
	m_pShaderCom->Set_RawValue("g_fMaxUVIndexX", &m_ParticleInfo->fMaxUVIndexX, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_fMaxUVIndexY", &m_ParticleInfo->fMaxUVIndexY, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_fUVIndexX", &m_fShaderUVIndexX, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_fUVIndexY", &m_fShaderUVIndexY, sizeof(_float));
	
	m_pShaderCom->Set_RawValue("g_bFixColor", &m_ParticleInfo->bFixColor, sizeof(_bool));
	m_pShaderCom->Set_RawValue("g_fRatio1", &m_ParticleInfo->vRatio.x, sizeof(_float));	
	m_pShaderCom->Set_RawValue("g_vColor", &m_ParticleInfo->vColor, sizeof(_float4));
	m_pShaderCom->Set_RawValue("g_vColor2", &m_ParticleInfo->vColor2, sizeof(_float4));
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

void CPlayer04Rage1Particle::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());
	


	ImGui::End();

}


HRESULT CPlayer04Rage1Particle::Ready_Components()
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

CPlayer04Rage1Particle * CPlayer04Rage1Particle::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer04Rage1Particle*		pInstance = new CPlayer04Rage1Particle(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer04Rage1Particle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer04Rage1Particle::Clone(void * pArg)
{
	CPlayer04Rage1Particle*		pInstance = new CPlayer04Rage1Particle(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer04Rage1Particle"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer04Rage1Particle::Free()
{
	__super::Free();
	
	if (m_ParticleInfo != nullptr)
		Safe_Delete(m_ParticleInfo);

	if (m_CreateInfo != nullptr)
		Safe_Delete(m_CreateInfo);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);

}
