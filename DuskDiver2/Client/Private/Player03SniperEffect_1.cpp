#include "stdafx.h"
#include "..\Public\Player03SniperEffect_1.h"
#include "GameInstance.h"
#include "Time_Manager.h"

CPlayer03SniperEffect_1::CPlayer03SniperEffect_1(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CPlayer03SniperEffect_1::CPlayer03SniperEffect_1(const CPlayer03SniperEffect_1 & rhs)
	: CEffect(rhs)
{
}

HRESULT CPlayer03SniperEffect_1::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer03SniperEffect_1::Initialize(void * pArg)
{
	m_Info = (*(SNIPERINFO**)pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_sTag = "Player03SniperEffect_1";
	m_pTransformCom->Set_Scale(_vector{ m_Info->vScale.x ,m_Info->vScale.y,m_Info->vScale.z });
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_Info->vPos));
	m_vColor = { 1.f,1.f,1.f,1.f };
	m_vColor2 = { 1.f,1.f,1.f,1.f };
	return S_OK;
}

void CPlayer03SniperEffect_1::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_EFFECT, this);
	fTimeDelta *= m_fTMRatio;

	m_vRatio.x -= 1.f * fTimeDelta;
	m_vRatio.y -= 1.f * fTimeDelta;

	if (m_vRatio.x <= 0.f)
		Set_Dead();

	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pTransformCom->Get_Scale()) + _vector{ 0.6f, 0.6f, 0.f });

}

void CPlayer03SniperEffect_1::LateTick(_float fTimeDelta)
{
	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DISTORTION, this);

}

HRESULT CPlayer03SniperEffect_1::Render()
{
	if (nullptr == m_pShaderCom)
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
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_vColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor2", &m_vColor2, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio1", &m_vRatio.x, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio2", &m_vRatio.y, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	
	return S_OK;

}

void CPlayer03SniperEffect_1::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{


}

void CPlayer03SniperEffect_1::Render_Debug()
{

}


HRESULT CPlayer03SniperEffect_1::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Player03TextureEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Wave1", TEXT("Wave1"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}


CPlayer03SniperEffect_1 * CPlayer03SniperEffect_1::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer03SniperEffect_1*		pInstance = new CPlayer03SniperEffect_1(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer03SniperEffect_1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer03SniperEffect_1::Clone(void * pArg)
{
	CPlayer03SniperEffect_1*		pInstance = new CPlayer03SniperEffect_1(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer03SniperEffect_1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CPlayer03SniperEffect_1::Free()
{
	__super::Free();

	
	Safe_Delete(m_Info);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}
