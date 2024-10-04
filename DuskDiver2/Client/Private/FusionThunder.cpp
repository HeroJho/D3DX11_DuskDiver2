#include "stdafx.h"
#include "..\Public\FusionThunder.h"
#include "GameInstance.h"
#include "Time_Manager.h"

CFusionThunder::CFusionThunder(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CFusionThunder::CFusionThunder(const CFusionThunder & rhs)
	: CEffect(rhs)
{
}

HRESULT CFusionThunder::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CFusionThunder::Initialize(void * pArg)
{
	m_Info = (*(THUNDER1INFO**)pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_sTag = "Fusion_Thunder";
	m_pTransformCom->Set_Scale(_vector{ m_Info->vScale.x ,m_Info->vScale.y,m_Info->vScale.z });
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_Info->vPos));
	m_fMaxUVIndexX = 4.f;
	m_fMaxUVIndexY = 4.f;
	m_fUVIndexX = 0.f;
	m_fUVIndexY = 0.f;
	m_fUVSpeed =	m_Info->fSpeed;
	m_vColor = { 0.972f,0.549f,0.439f,1.f };
	m_vColor2 = { (_float)0.941f, (_float)0.231f, (_float)0.372, 1.f };
	return S_OK;
}

void CFusionThunder::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_EFFECT, this);
	fTimeDelta *= m_fTMRatio;



	if (Set_UVIndex(fTimeDelta))
		Set_Dead();

}

void CFusionThunder::LateTick(_float fTimeDelta)
{
	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CFusionThunder::Render()
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

void CFusionThunder::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{


}

void CFusionThunder::Render_Debug()
{

}


HRESULT CFusionThunder::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MonsterTextureEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"T_01_Electric_Blue", TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}


CFusionThunder * CFusionThunder::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFusionThunder*		pInstance = new CFusionThunder(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CFusionThunder"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFusionThunder::Clone(void * pArg)
{
	CFusionThunder*		pInstance = new CFusionThunder(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CFusionThunder"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CFusionThunder::Free()
{
	__super::Free();

	
	Safe_Delete(m_Info);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}
