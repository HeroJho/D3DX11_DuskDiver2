#include "stdafx.h"
#include "..\Public\Player04VoidEffect_2.h"
#include "GameInstance.h"
#include "Time_Manager.h"

CPlayer04VoidEffect_2::CPlayer04VoidEffect_2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPlayer04VoidEffect_2::CPlayer04VoidEffect_2(const CPlayer04VoidEffect_2 & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer04VoidEffect_2::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer04VoidEffect_2::Initialize(void * pArg)
{
	P04VOIDEFFECT_2 Info = (*(P04VOIDEFFECT_2*)pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Player04VoidEffect_2";
	m_pTransformCom->Set_Scale(_vector{ m_fScale,m_fScale,m_fScale });
	XMStoreFloat4(&Info.vPosition, XMLoadFloat4(&Info.vPosition) + (XMLoadFloat3(&GI->Get_CamDir(CPipeLine::DIR_RIGHT)) * -0.5f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(Info.vPosition.x, Info.vPosition.y + 1.f, Info.vPosition.z, 1.f));
	

	return S_OK;
}

void CPlayer04VoidEffect_2::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_EFFECT, this);
	fTimeDelta *= m_fTMRatio;

	if (m_fScale <= 1.5f)
	{
		m_fScale -= 2.f * fTimeDelta;
		if (m_fScale <= 1.f)
		{
			m_fScale = 1.f;
			m_fDeadAcc += 1.f * fTimeDelta;

			if (m_fDeadAcc >= 0.15f)
				Set_Dead();
		}
		m_pTransformCom->Set_Scale(_vector{ m_fScale,m_fScale,m_fScale });
	}
}

void CPlayer04VoidEffect_2::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CPlayer04VoidEffect_2::Render()
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
	if (FAILED(m_pShaderCom->Set_RawValue("g_vRGB", &m_vRGB, sizeof(_float3))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(4)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;





	return S_OK;
}

void CPlayer04VoidEffect_2::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{


}

void CPlayer04VoidEffect_2::Render_Debug()
{

}

HRESULT CPlayer04VoidEffect_2::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Player04TextureEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Player04VoidEffect_2", TEXT("Player04VoidEffect_2"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}


CPlayer04VoidEffect_2 * CPlayer04VoidEffect_2::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer04VoidEffect_2*		pInstance = new CPlayer04VoidEffect_2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer04VoidEffect_2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer04VoidEffect_2::Clone(void * pArg)
{
	CPlayer04VoidEffect_2*		pInstance = new CPlayer04VoidEffect_2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer04VoidEffect_2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CPlayer04VoidEffect_2::Free()
{
	__super::Free();


	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}
