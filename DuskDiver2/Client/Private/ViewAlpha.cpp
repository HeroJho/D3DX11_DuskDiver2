#include "stdafx.h"
#include "..\Public\ViewAlpha.h"
#include "GameInstance.h"
#include "Time_Manager.h"

CViewAlpha::CViewAlpha(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CViewAlpha::CViewAlpha(const CViewAlpha & rhs)
	: CGameObject(rhs)
{
}

HRESULT CViewAlpha::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CViewAlpha::Initialize(void * pArg)
{
	m_pViewAlphaInfo = (*(VIEWALPHAINFO**)pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(XMVectorSet((_float)g_iWinSizeX, (_float)g_iWinSizeY, 1.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.0f, 1.f));

	return S_OK;
}

void CViewAlpha::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_EFFECT, this);
	fTimeDelta *= m_fTMRatio;

	m_pViewAlphaInfo->vRatio.x -= m_pViewAlphaInfo->vRatioSpeed.x * fTimeDelta;
	m_pViewAlphaInfo->fBlurPower -= m_pViewAlphaInfo->vRatioSpeed.x * fTimeDelta;
	if (m_pViewAlphaInfo->vRatio.x <= 0.f)
		Set_Dead();
	//m_pViewAlphaInfo->vRatio.y -= m_pViewAlphaInfo->vRatioSpeed.y * fTimeDelta;
}

void CViewAlpha::LateTick(_float fTimeDelta)
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CViewAlpha::Render()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &GI->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_IDENTITY), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &GI->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_UIPROJ), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_pViewAlphaInfo->vColor1, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor2", &m_pViewAlphaInfo->vColor2, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio1", &m_pViewAlphaInfo->vRatio.x, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio2", &m_pViewAlphaInfo->vRatio.y, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fBlurPower", &m_pViewAlphaInfo->fBlurPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(5)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CViewAlpha::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{


}

void CViewAlpha::Render_Debug()
{

}

HRESULT CViewAlpha::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"AlphaTex", TEXT("AlphaTex"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}


CViewAlpha * CViewAlpha::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CViewAlpha*		pInstance = new CViewAlpha(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CViewAlpha"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CViewAlpha::Clone(void * pArg)
{
	CViewAlpha*		pInstance = new CViewAlpha(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CViewAlpha"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CViewAlpha::Free()
{
	__super::Free();

	if (m_pViewAlphaInfo != nullptr)
		Safe_Delete(m_pViewAlphaInfo);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
}
