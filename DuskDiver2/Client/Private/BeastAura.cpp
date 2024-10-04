#include "stdafx.h"
#include "..\Public\BeastAura.h"
#include "GameInstance.h"
#include "Time_Manager.h"
#include "ToolManager.h"
#include "Player01Pillar2.h"
#include "Player01Pillar3.h"
#include "Player01Wind1.h"
#include "Particle_Manager.h"
#include "Effect_Manager.h"

CBeastAura::CBeastAura(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCreture(pDevice, pContext)
{
}

CBeastAura::CBeastAura(const CBeastAura& rhs)
	: CCreture(rhs)
{
}

HRESULT CBeastAura::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBeastAura::Initialize(void * pArg)
{
	m_pInfo = (*(AURAINFO**)pArg);

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_bDead = false;
	
	_matrix Matrix = XMMatrixIdentity();
	Matrix.r[3] = XMLoadFloat4(&m_pInfo->vLocalPos);
	XMStoreFloat4x4(&m_pInfo->vWorldMatrix, Matrix * XMLoadFloat4x4(&m_pInfo->vWorldMatrix));
	m_pTransformCom->Set_WorldMatrix(m_pInfo->vWorldMatrix);
	m_pTransformCom->Set_Scale(_vector{ 1.f,1.f,1.f });
	m_pTransformCom->TurnAngle(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(m_pInfo->fAngle));

	m_fMaxUVIndexX = 0.f;
	m_fMaxUVIndexY = 0.f;
	m_vUVFixAcc = { 0.f,0.f };

	return S_OK;
}

void CBeastAura::Tick(_float fTimeDelta)
{
	if (m_bRender)
	{

		TIM->Add_TimeObj(CTime_Manager::OBJ_EFFECT, this);
		fTimeDelta *= m_fTMRatio;
		m_vUVFixAcc.x += m_pInfo->vUVFixSpeed.x * fTimeDelta;
		m_vUVFixAcc.y += m_pInfo->vUVFixSpeed.y * fTimeDelta;
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), m_pInfo->fTurnSpeed, fTimeDelta);

		if (m_bStartFix)
		{
			if (m_pInfo->vRatio.x <= m_pInfo->vOriginRatio.x)
				m_pInfo->vRatio.x += m_pInfo->vRatioSpeed.x;
			else
				m_pInfo->vRatio.x = m_pInfo->vRatio.x;

			if (m_pInfo->vRatio.y <= m_pInfo->vOriginRatio.y)
				m_pInfo->vRatio.y += m_pInfo->vRatioSpeed.y;
			else
				m_pInfo->vRatio.y = m_pInfo->vRatio.y;

			if(m_pTransformCom->Get_Scale().x <= m_pInfo->vOriginScale.x)
				m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pTransformCom->Get_Scale()) + XMLoadFloat3(&m_pInfo->vScaleSpeed));
			else
			{
				m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pInfo->vOriginScale));
				m_bStartFix = false;
			}
		}

		if (m_bOff)
		{
			m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pTransformCom->Get_Scale()) - XMLoadFloat3(&m_pInfo->vScaleSpeed));

			if (m_pTransformCom->Get_Scale().x <= 0.2f)
			{
				m_pTransformCom->Set_State(CTransform::STATE_UP, _vector{ 0.f,0.1f,0.f });
				m_pTransformCom->Set_Scale(_vector{ 0.1f,0.1f,0.1f });
				m_bRender = false;
			}
		}

	}
	
	
}

void CBeastAura::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;
	
	if (m_bRender)
	{

		Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);

	}

}

HRESULT CBeastAura::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_pInfo->vColor, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor2", &m_pInfo->vColor2, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio1", &m_pInfo->vRatio.x, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio2", &m_pInfo->vRatio.y, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fBlurPower", &m_pInfo->fBlurPower, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fMaxUVIndexX", &m_fMaxUVIndexX, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixX", &m_fMaxUVIndexX, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixY", &m_vUVFixAcc.y, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Render(m_pShaderCom,i, 2)))
			return E_FAIL;
	}

	return S_OK;
}

void CBeastAura::Set_On(_float4 vPos)
{
	m_pTransformCom->Set_Scale(_vector{ 1.f,1.f,1.f });
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));
	m_bRender = true;
	m_bOff = false;
	m_bStartFix = true;
	m_vUVFixAcc = { 0.f,0.f };

	CPlayer01Pillar3::PILLAR3INFO* Info3 = new CPlayer01Pillar3::PILLAR3INFO;
	Info3->fBlurPower = 5.f;
	Info3->vLocalPos = { 0.f,0.f,0.f,1.f };
	Info3->vColor = { 1.f,0.f,0.f,1.f };
	Info3->vColor2 = { 0.f,0.f,0.f,1.f };
	Info3->vRatio = { 1.f,1.f };
	Info3->vRatioSpeed = { 2.f,2.f };
	Info3->vScaleSpeed = { 30.f,30.f,30.f };
	Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
	GI->Add_GameObjectToLayer(L"Player01Pillar3", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3);

	CPlayer01Wind1::WIND1INFO* Info5 = new CPlayer01Wind1::WIND1INFO;
	Info5->fBlurPower = 0.f;
	Info5->fLifeTime = 0.f;
	Info5->fTurnSpeed = 5.f;
	Info5->vColor = { 1.f,0.f,0.f,1.f };
	Info5->vColor2 = { 0.f,0.f,0.f,1.f };
	Info5->vLocalPos = { 0.f,0.f,0.f,1.f };
	Info5->vRatio = { 1.f,1.f };
	Info5->vRatioSpeed = { 1.f,0.f };
	Info5->vScale = { 2.f,10.f,2.f };
	Info5->vScaleSpeed = { 50.f,15.f,50.f };
	Info5->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
	GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_BeastEffect", &Info5);


}

void CBeastAura::Set_Off()
{
	m_bOff = true;
	PTM->Create_InstanceParticle(L"BeastMarbleEnd", m_pTransformCom->Get_WorldFloat4x4());
}

void CBeastAura::Create_Effect()
{
	if (!EM->Get_MarbleCol() && !EM->Get_MarbleEnd())
	{
		CPlayer01Pillar3::PILLAR3INFO* Info3 = new CPlayer01Pillar3::PILLAR3INFO;
		Info3->fBlurPower = 5.f;
		Info3->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info3->vColor = { 1.f,0.f,0.f,1.f };
		Info3->vColor2 = { 0.f,0.f,0.f,1.f };
		Info3->vRatio = { 1.f,1.f };
		Info3->vRatioSpeed = { 2.f,2.f };
		Info3->vScaleSpeed = { 30.f,30.f,30.f };
		Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Pillar3", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3);

		CPlayer01Wind1::WIND1INFO* Info5 = new CPlayer01Wind1::WIND1INFO;
		Info5->fBlurPower = 0.f;
		Info5->fLifeTime = 0.f;
		Info5->fTurnSpeed = 5.f;
		Info5->vColor = { 1.f,0.f,0.f,1.f };
		Info5->vColor2 = { 0.f,0.f,0.f,1.f };
		Info5->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info5->vRatio = { 1.f,1.f };
		Info5->vRatioSpeed = { 1.f,0.f };
		Info5->vScale = { 2.f,10.f,2.f };
		Info5->vScaleSpeed = { 50.f,15.f,50.f };
		Info5->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_BeastEffect", &Info5);
	}
}

HRESULT CBeastAura::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Player04MeshEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_KickAura"), TEXT("Effect_KickAura"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}


CCreture * CBeastAura::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBeastAura*		pInstance = new CBeastAura(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBeastAura"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBeastAura::Clone(void * pArg)
{
	CBeastAura*		pInstance = new CBeastAura(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBeastAura"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBeastAura::Free()
{
	__super::Free();
	Safe_Delete(m_pInfo);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	

}
