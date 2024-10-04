#include "stdafx.h"
#include "..\Public\MarvelReserveTaril.h"
#include "GameInstance.h"
#include "Time_Manager.h"
#include "Monster_Manager.h"
#include "BeastLaser.h"
#include "Effect_Manager.h"

CMarvelReserveTaril::CMarvelReserveTaril(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMarvelReserveTaril::CMarvelReserveTaril(const CMarvelReserveTaril & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMarvelReserveTaril::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CMarvelReserveTaril::Initialize(void * pArg)
{
	if (pArg != nullptr)
		m_ReserveTrailInfo = (*(RESERVETRAILINFO*)pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scale(_vector{ 1.f , 0.f, 1.f, 0.f });


	_float4 vPos;
	vPos = { m_ReserveTrailInfo.vPosition.x, 0.7f, m_ReserveTrailInfo.vPosition.z, 1.f };
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));

	Make_ImGui();
	return S_OK;
}

void CMarvelReserveTaril::Tick(_float fTimeDelta)
{
	if (m_bRender)
	{
		TIM->Add_TimeObj(CTime_Manager::OBJ_MONSTER, this);
		fTimeDelta *= m_fTMRatio;
		
		_vector vTargetLook = (XMLoadFloat4(&m_ReserveTrailInfo.vTargetPos) - XMLoadFloat4(&m_ReserveTrailInfo.vPosition));

		m_fDist = XMVectorGetX(XMVector3Length(vTargetLook));

		vTargetLook = XMVector3Normalize(vTargetLook);
		XMStoreFloat4(&m_vTargetLook, vTargetLook);
		m_pTransformCom->Set_Look(vTargetLook);

		_float4 vPos;
		XMStoreFloat4(&vPos, (XMLoadFloat4(&m_ReserveTrailInfo.vTargetPos) + XMLoadFloat4(&m_ReserveTrailInfo.vPosition)));
		vPos = _float4(vPos.x * 0.5f, 0.7f, vPos.z * 0.5f, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));

		m_pTransformCom->Set_Scale(XMVectorSet(m_ReserveTrailInfo.fScaleX, 0.f, m_fDist, 0.f));

	}
	
}

void CMarvelReserveTaril::LateTick(_float fTimeDelta)
{

	if (nullptr == m_pRendererCom)
		return;

	if (m_bRender)
	{
  		Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}

}

HRESULT CMarvelReserveTaril::Render()
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

	m_pShaderCom->Set_RawValue("g_bFixColor", &m_ReserveTrailInfo.bFixColor, sizeof(_bool));

	if (m_ReserveTrailInfo.bFixColor)
	{
		m_pShaderCom->Set_RawValue("g_fRatio1", &m_ReserveTrailInfo.vRatio.x, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_vColor", &m_ReserveTrailInfo.vColor, sizeof(_float4));
		m_pShaderCom->Set_RawValue("g_vColor2", &m_ReserveTrailInfo.vColor2, sizeof(_float4));
		m_pShaderCom->Set_RawValue("g_fRatio2", &m_ReserveTrailInfo.vRatio.y, sizeof(_float));
	}

	m_pShaderCom->Set_RawValue("g_fBlurPower", &m_ReserveTrailInfo.fBlurPower, sizeof(_float));


	if (FAILED(m_pShaderCom->Begin(1)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CMarvelReserveTaril::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());




	//ImGui::DragFloat("UVFixSpeedX", &m_ParticleInfo->fUVFixSpeedX, 0.001f, -30.f, 30.f, "%.3f");
	//ImGui::DragFloat("UVFixSpeedY", &m_ParticleInfo->fUVFixSpeedY, 0.001f, -30.f, 30.f, "%.3f");


	IG->Debug_Transform(m_pTransformCom);





	ImGui::End();

}

void CMarvelReserveTaril::SetUp_ReserveTrail(_float4 vPos, _float4 vTargetPos)
{
	vPos.y = 0.7f;
	m_ReserveTrailInfo.vPosition = vPos;
	vTargetPos.y = 0.7f;
	m_ReserveTrailInfo.vTargetPos = vTargetPos;
}


HRESULT CMarvelReserveTaril::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"TO14_T_FX_Mask_ak_30", TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_ZRect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

CMarvelReserveTaril * CMarvelReserveTaril::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMarvelReserveTaril*		pInstance = new CMarvelReserveTaril(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMarvelReserveTaril"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMarvelReserveTaril::Clone(void * pArg)
{
	CMarvelReserveTaril*		pInstance = new CMarvelReserveTaril(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMarvelReserveTaril"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMarvelReserveTaril::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
