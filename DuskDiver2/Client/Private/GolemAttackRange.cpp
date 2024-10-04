#include "stdafx.h"
#include "..\Public\GolemAttackRange.h"
#include "GameInstance.h"
#include "Time_Manager.h"
#include "Monster_Manager.h"
#include "BeastLaser.h"
#include "GolemAttackRange.h"

CGolemAttackRange::CGolemAttackRange(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CGolemAttackRange::CGolemAttackRange(const CGolemAttackRange & rhs)
	: CGameObject(rhs)
{
}

HRESULT CGolemAttackRange::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CGolemAttackRange::Initialize(void * pArg)
{
	if (pArg != nullptr)
		m_ReserveTrailInfo = (*(RESERVETRAILINFO*)pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_ReserveTrailInfo.vPosition.y = 0.02f;

	m_fDist = 13.f;
	m_vTargetLook = m_ReserveTrailInfo.vDir;
	m_pTransformCom->Set_Look(XMLoadFloat4(&m_vTargetLook));

	m_fOriginSpeed = { 1.2f, 1.2f };
	m_pTransformCom->Set_Scale(_vector{ m_ReserveTrailInfo.fScaleX , 0.f, m_fDist, 1.f });

	_float4 vPos;
	XMStoreFloat4(&vPos, (XMLoadFloat4(&m_ReserveTrailInfo.vPosition)));
	vPos = _float4(vPos.x, m_ReserveTrailInfo.fStaticY, vPos.z, 1.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));

	Make_ImGui();
	return S_OK;
}

void CGolemAttackRange::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_MONSTER, this);
	fTimeDelta *= m_fTMRatio;

	if (m_ReserveTrailInfo.iIndex == 1)
	{

		if (m_fOriginSpeed.x > 0.f)
			m_ReserveTrailInfo.vRatio.x += m_fOriginSpeed.x * fTimeDelta;

		if (m_fOriginSpeed.y > 0.f)
			m_ReserveTrailInfo.vRatio.y += m_fOriginSpeed.y * fTimeDelta;

		if (m_ReserveTrailInfo.vRatio.x >= 0.8 || m_ReserveTrailInfo.vRatio.y >= 0.8)
		{
			m_ReserveTrailInfo.vRatio.x = 0.8f;
			m_ReserveTrailInfo.vRatio.y = 0.8f;

			if (m_bOneCreate)
			{
				m_bOneCreate = false;
				//생성
				CGolemAttackRange::RESERVETRAILINFO Info;
				ZeroMemory(&Info, sizeof(CGolemAttackRange::RESERVETRAILINFO));

				Info.iIndex = 2;
				Info.bFixColor = true;
				Info.fBlurPower = 1.f;
				Info.fLifeTime = 0.9f;
				Info.fStaticY = 0.02f;
				Info.fScaleX = 0.1f;
				Info.vColor = { 1.f,0.045f,0.026f,1.f };
				Info.vColor2 = { 0.f,0.f,0.f,1.f };
				Info.vRatio = { 0.8f, 1.f };
				Info.fScaleXSpeed = 6.f;
				Info.fLimitScaleX = 4.8f;
			
				Info.vDir = m_ReserveTrailInfo.vDir;
				Info.vPosition = m_ReserveTrailInfo.vPosition;
				(GI->Add_GameObjectToLayer(TEXT("GolemAttackRange"), LEVEL_TEST, TEXT("Layer_Monster"), &Info));
			}
			m_bDeadStart = true;
			//생성
		}
		//ratio값 더해줘서 삭 나타나게
		if(m_bDeadStart)
			m_fDeadAcc += 1.f * fTimeDelta;

		if (m_fDeadAcc > m_ReserveTrailInfo.fLifeTime)
			Set_Dead();

		m_pTransformCom->Set_Scale(_vector{ m_ReserveTrailInfo.fScaleX , 0.f, m_fDist, });

	}

	else
	{
		if (m_ReserveTrailInfo.fScaleX >= m_ReserveTrailInfo.fLimitScaleX)
		{
			m_bDeadStart = true;

		}
		else
			m_ReserveTrailInfo.fScaleX += m_ReserveTrailInfo.fScaleXSpeed * fTimeDelta;

		if (m_bDeadStart)
			m_fDeadAcc += 1.f * fTimeDelta;

		if (m_fDeadAcc >= m_ReserveTrailInfo.fLifeTime)
		{
			if (m_fOriginSpeed.x > 0.f)
				m_ReserveTrailInfo.vRatio.x -= m_fOriginSpeed.x * fTimeDelta;

			if (m_fOriginSpeed.y > 0.f)
				m_ReserveTrailInfo.vRatio.y -= m_fOriginSpeed.y * fTimeDelta;


			if (m_ReserveTrailInfo.vRatio.x <= 0.f || m_ReserveTrailInfo.vRatio.y <= 0.f)
			{
				Set_Dead();
			}
		}


		m_pTransformCom->Set_Scale(XMVectorSet(m_ReserveTrailInfo.fScaleX, 0.f, m_fDist, 0.f));
	}

	

}

void CGolemAttackRange::LateTick(_float fTimeDelta)
{

	if (nullptr == m_pRendererCom)
		return;

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

}

HRESULT CGolemAttackRange::Render()
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

void CGolemAttackRange::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());




	//ImGui::DragFloat("UVFixSpeedX", &m_ParticleInfo->fUVFixSpeedX, 0.001f, -30.f, 30.f, "%.3f");
	//ImGui::DragFloat("UVFixSpeedY", &m_ParticleInfo->fUVFixSpeedY, 0.001f, -30.f, 30.f, "%.3f");


	IG->Debug_Transform(m_pTransformCom);





	ImGui::End();

}


HRESULT CGolemAttackRange::Ready_Components()
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

	if (m_ReserveTrailInfo.iIndex == 1)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, L"ReserveTrail2", TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, L"ReserveTrail", TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
			return E_FAIL;
	}


	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_ZRect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

CGolemAttackRange * CGolemAttackRange::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CGolemAttackRange*		pInstance = new CGolemAttackRange(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CGolemAttackRange"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CGolemAttackRange::Clone(void * pArg)
{
	CGolemAttackRange*		pInstance = new CGolemAttackRange(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CGolemAttackRange"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGolemAttackRange::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
