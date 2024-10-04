#include "stdafx.h"
#include "..\Public\Player01Sunder_2.h"
#include "GameInstance.h"
#include "Time_Manager.h"

CPlayer01Sunder_2::CPlayer01Sunder_2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CPlayer01Sunder_2::CPlayer01Sunder_2(const CPlayer01Sunder_2 & rhs)
	: CEffect(rhs)
{
}

HRESULT CPlayer01Sunder_2::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer01Sunder_2::Initialize(void * pArg)
{
	m_Info = (*(SUNDER2INFO**)pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_sTag = "Player01Sunder_1";
	m_pTransformCom->Set_Scale(_vector{ m_Info->vScale.x ,m_Info->vScale.y,m_Info->vScale.z });
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_Info->vPos));
	m_fMaxUVIndexX = 3.f;
	m_fMaxUVIndexY = 3.f;
	m_fUVIndexX = m_fMaxUVIndexX;
	m_fUVIndexY = m_fMaxUVIndexY;
	m_fUVSpeed = 0.033f;
	m_vColor = { 0.f,0.f,0.f,1.f };
	m_vColor2 = { (_float)0.247f, (_float)0.909f, (_float)0.850, 1.f };

	return S_OK;
}

void CPlayer01Sunder_2::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_EFFECT, this);
	fTimeDelta *= m_fTMRatio;

	if (Set_UVReverse(fTimeDelta))
		Set_Dead();

}

void CPlayer01Sunder_2::LateTick(_float fTimeDelta)
{
	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CPlayer01Sunder_2::Render()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	
	_vector		vLook = XMLoadFloat3(&m_vCamLook);

	_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));

	vLook = XMVector3Normalize(XMVector3Cross(vRight, m_pTransformCom->Get_State(CTransform::STATE_UP)));

	XMStoreFloat4(&m_vBackPos, (XMVectorSet(m_Info->vPos.x, m_Info->vPos.y, m_Info->vPos.z, 1.f) - (0.1f * (vLook))));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vector{ m_vBackPos.x, m_vBackPos.y , m_vBackPos.z, 1.f });

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

	m_pShaderCom->Set_RawValue("g_fMaxUVIndexX", &m_fMaxUVIndexX, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_fMaxUVIndexY", &m_fMaxUVIndexY, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_fUVIndexX", &m_fUVIndexX, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_fUVIndexY", &m_fUVIndexY, sizeof(_float));

	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CPlayer01Sunder_2::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{


}

void CPlayer01Sunder_2::Render_Debug()
{

}


HRESULT CPlayer01Sunder_2::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Player01TextureEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Player01Sunder_1", TEXT("Player01Sunder_1"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}


CPlayer01Sunder_2 * CPlayer01Sunder_2::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer01Sunder_2*		pInstance = new CPlayer01Sunder_2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer01Sunder_2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer01Sunder_2::Clone(void * pArg)
{
	CPlayer01Sunder_2*		pInstance = new CPlayer01Sunder_2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer01Sunder_2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CPlayer01Sunder_2::Free()
{
	__super::Free();

	
	Safe_Delete(m_Info);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}
