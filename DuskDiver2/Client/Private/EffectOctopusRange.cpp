#include "stdafx.h"
#include "..\Public\EffectOctopusRange.h"
#include "GameInstance.h"
#include "Time_Manager.h"
#include "Monster_Manager.h"

CEffectOctopusRange::CEffectOctopusRange(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CEffectOctopusRange::CEffectOctopusRange(const CEffectOctopusRange & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEffectOctopusRange::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CEffectOctopusRange::Initialize(void * pArg)
{

	m_EffectRangeInfo = (*(EFFECTRANGEINFO**)pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	
	m_pTransformCom->Set_WorldMatrix(m_EffectRangeInfo->WorldMatrix);
	
	m_pTransformCom->Set_Scale(_vector{ 11.f, 11.f, 0.f });
	m_fOrigin = m_EffectRangeInfo->vRatio;
	m_fOriginSpeed = m_EffectRangeInfo->vRatioSpeed;
	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(90.f));
	m_EffectRangeInfo->bFixColor = true;
	//회전 필요
	Make_ImGui();
	return S_OK;
}

void CEffectOctopusRange::Tick(_float fTimeDelta)
{
	if (m_bRender)
	{

		TIM->Add_TimeObj(CTime_Manager::OBJ_MONSTER, this);
		fTimeDelta *= m_fTMRatio;

		_vector vPos = XMLoadFloat4(&m_vPosition);
		vPos = XMVectorSetY(vPos, 15.541f);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);

		if (m_fOriginSpeed.x > 0.f)
			m_EffectRangeInfo->vRatio.x -= m_fOriginSpeed.x * fTimeDelta;

		if (m_fOriginSpeed.y > 0.f)
			m_EffectRangeInfo->vRatio.y -= m_fOriginSpeed.y * fTimeDelta;


		//랜더 끄는걸로
		if (m_EffectRangeInfo->vRatio.x <= 0.f || m_EffectRangeInfo->vRatio.y <= 0.f)
		{
			m_EffectRangeInfo->vRatio.x = m_fOrigin.x;
			m_EffectRangeInfo->vRatio.y = m_fOrigin.y;
			m_bRender = false;
		}
		
	}

}

void CEffectOctopusRange::LateTick(_float fTimeDelta)
{
	if (m_bRender)
	{
	if (nullptr == m_pRendererCom)
		return;

		Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

	}

	

}

HRESULT CEffectOctopusRange::Render()
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


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;
	
	
	m_pShaderCom->Set_RawValue("g_bFixColor", &m_EffectRangeInfo->bFixColor, sizeof(_bool));
	m_pShaderCom->Set_RawValue("g_fRatio1", &m_EffectRangeInfo->vRatio.x, sizeof(_float));
	if (m_EffectRangeInfo->bFixColor)
	{
		m_pShaderCom->Set_RawValue("g_vColor", &m_EffectRangeInfo->vColor, sizeof(_float4));
		m_pShaderCom->Set_RawValue("g_vColor2", &m_EffectRangeInfo->vColor2, sizeof(_float4));
		m_pShaderCom->Set_RawValue("g_fRatio2", &m_EffectRangeInfo->vRatio.y, sizeof(_float));
	}

	m_pShaderCom->Set_RawValue("g_fBlurPower", &m_EffectRangeInfo->fBlurPower, sizeof(_float));

	
	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;
	

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CEffectOctopusRange::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());
	

	

	//ImGui::DragFloat("UVFixSpeedX", &m_ParticleInfo->fUVFixSpeedX, 0.001f, -30.f, 30.f, "%.3f");
	//ImGui::DragFloat("UVFixSpeedY", &m_ParticleInfo->fUVFixSpeedY, 0.001f, -30.f, 30.f, "%.3f");


	IG->Debug_Transform(m_pTransformCom);





	ImGui::End();

}


HRESULT CEffectOctopusRange::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_BossTextureEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"OctopusRange", TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

CEffectOctopusRange * CEffectOctopusRange::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEffectOctopusRange*		pInstance = new CEffectOctopusRange(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEffectOctopusRange"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEffectOctopusRange::Clone(void * pArg)
{
	CEffectOctopusRange*		pInstance = new CEffectOctopusRange(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEffectOctopusRange"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEffectOctopusRange::Free()
{
	__super::Free();
	
	if (m_EffectRangeInfo != nullptr)
		Safe_Delete(m_EffectRangeInfo);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);

}
