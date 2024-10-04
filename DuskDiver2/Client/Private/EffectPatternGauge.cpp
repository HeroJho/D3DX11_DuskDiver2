#include "stdafx.h"
#include "..\Public\EffectPatternGauge.h"
#include "GameInstance.h"
#include "Time_Manager.h"
#include "Monster_Manager.h"

CEffectPatternGauge::CEffectPatternGauge(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CEffectPatternGauge::CEffectPatternGauge(const CEffectPatternGauge & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffectPatternGauge::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CEffectPatternGauge::Initialize(void * pArg)
{

	m_EffectRangeInfo = (*(EFFECTRANGEINFO**)pArg);

	m_EffectInitInfo = (*(EFFECTRANGEINFO**)pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

		
	m_pTransformCom->Set_WorldMatrix(m_EffectRangeInfo->WorldMatrix);

	//처음 생성할때 값 받아놔야한다

	Make_ImGui();
	return S_OK;
}

void CEffectPatternGauge::Tick(_float fTimeDelta)
{
	if (m_bRender)
	{
		TIM->Add_TimeObj(CTime_Manager::OBJ_PARTICLE, this);
		fTimeDelta *= m_fTMRatio;

		m_fLifeTimeAcc += 1.f * fTimeDelta;

		if (m_fLifeTimeAcc >= m_EffectRangeInfo->fLifeTime)
			Set_Dead();

		if (m_EffectRangeInfo->vRatioSpeed.x > 0.f)
			m_EffectRangeInfo->vRatio.x -= m_EffectRangeInfo->vRatioSpeed.x * fTimeDelta;

		if (m_EffectRangeInfo->vRatioSpeed.y > 0.f)
			m_EffectRangeInfo->vRatio.y -= m_EffectRangeInfo->vRatioSpeed.y * fTimeDelta;

		if (m_EffectRangeInfo->vScaleSpeed.x != 0.f)
		{
			m_EffectRangeInfo->vSize.x += m_EffectRangeInfo->vScaleSpeed.x * fTimeDelta;
		}
		if (m_EffectRangeInfo->vScaleSpeed.y != 0.f)
		{
			m_EffectRangeInfo->vSize.y += m_EffectRangeInfo->vScaleSpeed.y * fTimeDelta;
		}

		if (m_EffectRangeInfo->vSize.x <= 0.f || m_EffectRangeInfo->vSize.y <= 0.f)
		{
			m_fLifeTimeAcc = 0.f;
			m_bRender = false;
			Reset_State();
		}
	}
	

}

void CEffectPatternGauge::LateTick(_float fTimeDelta)
{
	if (m_bRender)
	{
		if (nullptr == m_pRendererCom)
			return;

		Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}
	

}

HRESULT CEffectPatternGauge::Render()
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
	
	m_pShaderCom->Set_RawValue("g_fSizeX", &m_EffectRangeInfo->vSize.x, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_fSizeY", &m_EffectRangeInfo->vSize.y, sizeof(_float));\

	m_pShaderCom->Set_RawValue("g_bFixColor", &m_EffectRangeInfo->bFixColor, sizeof(_bool));
	m_pShaderCom->Set_RawValue("g_fRatio1", &m_EffectRangeInfo->vRatio.x, sizeof(_float));

	if (m_EffectRangeInfo->bFixColor)
	{
		m_pShaderCom->Set_RawValue("g_vColor", &m_EffectRangeInfo->vColor, sizeof(_float4));
		m_pShaderCom->Set_RawValue("g_vColor2", &m_EffectRangeInfo->vColor2, sizeof(_float4));
		m_pShaderCom->Set_RawValue("g_fRatio2", &m_EffectRangeInfo->vRatio.y, sizeof(_float));
	}

	m_pShaderCom->Set_RawValue("g_fBlurPower", &m_EffectRangeInfo->fBlurPower, sizeof(_float));

	m_pShaderCom->Set_RawValue("g_fUVFixX", &m_vUVFixAcc.x, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_fUVFixY", &m_vUVFixAcc.y, sizeof(_float));
	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;
	

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CEffectPatternGauge::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());
	

	

	//ImGui::DragFloat("UVFixSpeedX", &m_ParticleInfo->fUVFixSpeedX, 0.001f, -30.f, 30.f, "%.3f");
	//ImGui::DragFloat("UVFixSpeedY", &m_ParticleInfo->fUVFixSpeedY, 0.001f, -30.f, 30.f, "%.3f");


	IG->Debug_Transform(m_pTransformCom);





	ImGui::End();

}


HRESULT CEffectPatternGauge::Ready_Components()
{
	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */	
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MonsterTextureEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"T_GPP_LensFlare_02", TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffectPatternGauge::SetUp_State(_fmatrix StateMatrix)
{
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, StateMatrix.r[0]);
	m_pTransformCom->Set_State(CTransform::STATE_UP, StateMatrix.r[1]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, StateMatrix.r[2]);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, StateMatrix.r[3]);

	return S_OK;
}

void CEffectPatternGauge::Reset_State()
{
	m_EffectRangeInfo->vRatio.x = m_EffectInitInfo->vRatio.x;
	m_EffectRangeInfo->vRatio.y = m_EffectInitInfo->vRatio.y;

	m_EffectRangeInfo->vSize.x = m_EffectInitInfo->vSize.x;
	m_EffectRangeInfo->vSize.y = m_EffectInitInfo->vSize.y;
}

CEffectPatternGauge * CEffectPatternGauge::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEffectPatternGauge*		pInstance = new CEffectPatternGauge(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEffectPatternGauge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffectPatternGauge::Clone(void * pArg)
{
	CEffectPatternGauge*		pInstance = new CEffectPatternGauge(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEffectPatternGauge"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffectPatternGauge::Free()
{
	__super::Free();
	
	if (m_EffectRangeInfo != nullptr)
		Safe_Delete(m_EffectRangeInfo);

	if (m_EffectInitInfo != nullptr)
		Safe_Delete(m_EffectInitInfo);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);

}
