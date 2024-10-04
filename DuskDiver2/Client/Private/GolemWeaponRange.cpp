#include "stdafx.h"
#include "..\Public\GolemWeaponRange.h"
#include "GameInstance.h"
#include "Time_Manager.h"
#include "Monster_Manager.h"

CGolemWeaponRange::CGolemWeaponRange(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CGolemWeaponRange::CGolemWeaponRange(const CGolemWeaponRange & rhs)
	: CGameObject(rhs)
{
}

HRESULT CGolemWeaponRange::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CGolemWeaponRange::Initialize(void * pArg)
{

	m_EffectRangeInfo = (*(EFFECTRANGEINFO**)pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	//m_pTransformCom->Set_Scale(_vector{ 0.1f, 0.1f, 0.f });

	m_fScale = 0.1f;
	m_pTransformCom->Set_Scale(_vector{ m_fScale, 0.f, m_fScale });

	m_fLimitSacle = 7.3f;

	m_EffectRangeInfo->vColor = { 1.f,0.038f,0.055f,1.f };
	m_EffectRangeInfo->vColor2 = { 0.f,0.f,0.f,1.f };

	m_EffectRangeInfo->bFixColor = true;

	m_fOriginSpeed = { 1.f, 0.f };

	_float4x4 fMatrix = m_EffectRangeInfo->WorldMatrix;
	
	_float4 vPos = *(_float4*)fMatrix.m[3];

	vPos.y = 0.02f;

	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(20.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));

	//회전 필요
	Make_ImGui();
	return S_OK;
}

void CGolemWeaponRange::Tick(_float fTimeDelta)
{

	TIM->Add_TimeObj(CTime_Manager::OBJ_MONSTER, this);
	fTimeDelta *= m_fTMRatio;
	
	m_fScale += 24.f * fTimeDelta;

	if (m_fScale >= m_fLimitSacle)
	{
		m_bChange = true;
		m_fScale = m_fLimitSacle;

	}

	if (m_bChange)
	{
		m_fDeadAcc += 1.f * fTimeDelta;

	}

	if (m_fDeadAcc >= 1.2f)
	{
		m_EffectRangeInfo->vRatio.x -= 2.f * fTimeDelta;

		m_EffectRangeInfo->vRatio.y -= 2.f * fTimeDelta;

		if (m_EffectRangeInfo->vRatio.x <= 0.f || m_EffectRangeInfo->vRatio.y <= 0.f)
			Set_Dead();
	}

	m_pTransformCom->Set_Scale(_vector{ m_fScale, 0.f, m_fScale });

}

void CGolemWeaponRange::LateTick(_float fTimeDelta)
{
	
	if (nullptr == m_pRendererCom)
		return;

		Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

}

HRESULT CGolemWeaponRange::Render()
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

void CGolemWeaponRange::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());
	

	IG->Debug_Transform(m_pTransformCom);





	ImGui::End();

}


HRESULT CGolemWeaponRange::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"T_Impact_Ring", TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	
	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_ZRect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

CGolemWeaponRange * CGolemWeaponRange::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CGolemWeaponRange*		pInstance = new CGolemWeaponRange(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CGolemWeaponRange"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CGolemWeaponRange::Clone(void * pArg)
{
	CGolemWeaponRange*		pInstance = new CGolemWeaponRange(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CGolemWeaponRange"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGolemWeaponRange::Free()
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
