#include "stdafx.h"
#include "..\Public\ReserveTrail.h"
#include "GameInstance.h"
#include "Time_Manager.h"
#include "Monster_Manager.h"
#include "BeastLaser.h"

CReserveTrail::CReserveTrail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CReserveTrail::CReserveTrail(const CReserveTrail & rhs)
	: CGameObject(rhs)
{
}

HRESULT CReserveTrail::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CReserveTrail::Initialize(void * pArg)
{
	if (pArg != nullptr)
		m_ReserveTrailInfo = (*(RESERVETRAILINFO*)pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_ReserveTrailInfo.vTargetPos.y = 0.f;
	m_ReserveTrailInfo.vPosition.y = 0.f;
	m_ReserveTrailInfo.bFixColor = true;


	_vector vTargetLook = (XMLoadFloat4(&m_ReserveTrailInfo.vTargetPos) - XMLoadFloat4(&m_ReserveTrailInfo.vPosition));
	
	m_fDist = XMVectorGetX(XMVector3Length(vTargetLook));

	vTargetLook = XMVector3Normalize(vTargetLook);
	XMStoreFloat4(&m_vTargetLook, vTargetLook);
	m_pTransformCom->Set_Look(vTargetLook);

	m_pTransformCom->Set_Scale(_vector{ m_ReserveTrailInfo.fScaleX , 0.f, m_fDist * 3.f, });

	//처음 위치는 두 벡터 더한거에서 나누기 2 해야함
	_float4 vPos;
	XMStoreFloat4(&vPos, (XMLoadFloat4(&m_ReserveTrailInfo.vTargetPos) + XMLoadFloat4(&m_ReserveTrailInfo.vPosition)));
	vPos = _float4(vPos.x * 0.5f, m_ReserveTrailInfo.fStaticY, vPos.z * 0.5f, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));

	Make_ImGui();
	return S_OK;
}

void CReserveTrail::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_MONSTER, this);
	fTimeDelta *= m_fTMRatio;

	if (m_ReserveTrailInfo.fScaleX >= m_ReserveTrailInfo.fLimitScaleX)
	{
		m_bDeadStart = true;

	}
	else
		m_ReserveTrailInfo.fScaleX += m_ReserveTrailInfo.fScaleXSpeed * fTimeDelta;

	if (m_bDeadStart)
		m_fDeadAcc += 1.f * fTimeDelta;

	if (m_fDeadAcc >= m_ReserveTrailInfo.fLifeTime && m_bOneCreate)
	{
		m_bOneCreate = false;
		Set_Dead();
		CBeastLaser::LASERINFO* Info = new CBeastLaser::LASERINFO;

		Info->vPosition = m_ReserveTrailInfo.vPosition;
		Info->vTargetLook = m_vTargetLook;
		Info->fBlurPower = 3.f;
		Info->vColor1 = { 1.f,1.f,1.f,1.f };
		Info->vColor1_2 = { 1.f,1.f,1.f,1.f };
		Info->vRatio = { 1.f,1.f };
		Info->vUVFixAcc = { 0.f,0.f };	
		Info->vUVFixSpeed = { 1.f,3.f };		
		Info->fBlurPower_2 = 3.f;
		Info->vColor2 = { 0.f,0.f,1.f,1.f };
		Info->vColor2_2 = { 0.f,0.5f,1.f,1.f };
		Info->vRatio_2 = { 1.f,1.f };
		Info->vUVFixAcc_2 = { 0.f,0.f };
		Info->vUVFixSpeed_2 = { -1.f,3.f };
		Info->vScaleSpeed = { -1.f,-0.f,-1.f };
		Info->vScale = { 1.2f, 100.f, 1.2f };
		GI->Add_GameObjectToLayer(L"BeastLaser", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info);

	}

	m_pTransformCom->Set_Scale(XMVectorSet(m_ReserveTrailInfo.fScaleX, 0.f, m_fDist * 3.f, 0.f));

}

void CReserveTrail::LateTick(_float fTimeDelta)
{

	if (nullptr == m_pRendererCom)
		return;

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

}

HRESULT CReserveTrail::Render()
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

void CReserveTrail::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());




	//ImGui::DragFloat("UVFixSpeedX", &m_ParticleInfo->fUVFixSpeedX, 0.001f, -30.f, 30.f, "%.3f");
	//ImGui::DragFloat("UVFixSpeedY", &m_ParticleInfo->fUVFixSpeedY, 0.001f, -30.f, 30.f, "%.3f");


	IG->Debug_Transform(m_pTransformCom);





	ImGui::End();

}


HRESULT CReserveTrail::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"ReserveTrail", TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;


	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_ZRect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

CReserveTrail * CReserveTrail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CReserveTrail*		pInstance = new CReserveTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CReserveTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CReserveTrail::Clone(void * pArg)
{
	CReserveTrail*		pInstance = new CReserveTrail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CReserveTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CReserveTrail::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
