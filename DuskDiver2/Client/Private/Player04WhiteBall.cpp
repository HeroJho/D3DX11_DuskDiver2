#include "stdafx.h"
#include "..\Public\Player04WhiteBall.h"
#include "GameInstance.h"
#include "Time_Manager.h"
#include "Particle_Manager.h"
#include "Player01SmokeBall.h"
#include "ToolManager.h"
#include "Camera_Manager.h"
#include "Player01Wind2.h"
#include "Player01Wind1.h"
#include "Effect_Manager.h"
#include "ViewAlpha.h"
#include "Player04Aura.h"
#include "Tornado.h"
#include "TornadoSide.h"
#include "Player_Manager.h"

CPlayer04WhiteBall::CPlayer04WhiteBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCreture(pDevice, pContext)
{
}

CPlayer04WhiteBall::CPlayer04WhiteBall(const CPlayer04WhiteBall& rhs)
	: CCreture(rhs)
{
}

HRESULT CPlayer04WhiteBall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer04WhiteBall::Initialize(void * pArg)
{
	m_pInfo = (*(WHITEBALLINFO**)pArg);

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_bDead = false;
	
	_matrix Matrix = XMMatrixIdentity();
	Matrix.r[3] = XMLoadFloat4(&m_pInfo->vLocalPos);
	XMStoreFloat4x4(&m_pInfo->vWorldMatrix, Matrix * XMLoadFloat4x4(&m_pInfo->vWorldMatrix));
	m_pTransformCom->Set_WorldMatrix(m_pInfo->vWorldMatrix);
	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pInfo->vScale));
	m_vUVFixAcc = { 0.f,0.f };
	PTM->Create_InstanceParticle(L"Player04RageSkill1", m_pTransformCom->Get_WorldFloat4x4());
	PTM->Create_InstanceParticle(L"Player04RageSkill1_2", m_pTransformCom->Get_WorldFloat4x4());
	EM->Add_Player04WhiteBall(this);
	m_fP3ScaleUpSpeed = 10.f;
	m_fP3ScaleDownSpeed = 10.f;
	m_bP2ScaleFix = true;
	return S_OK;
}

void CPlayer04WhiteBall::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_EFFECT, this);
	fTimeDelta *= m_fTMRatio;

	if (CK_DOWN(DIK_END))
	{
		Create_BlackBall(fTimeDelta);
	}

	switch (m_iPhase)
	{
	case 1:
		Phase1(fTimeDelta);
		break;
	case 2:
		Phase2(fTimeDelta);
		break;

	case 3:
		Phase3(fTimeDelta);
		break;

	case 4:
		Phase4(fTimeDelta);
		break;
	}
	
	
}

void CPlayer04WhiteBall::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;	
	fTimeDelta *= m_fTMRatio;

	if (m_bBalckBall)
		Create_BlackBall(fTimeDelta);

	if (m_bTornado)
		Create_Tornado(fTimeDelta);

	if (m_bLightEffect)
		Create_LightEffect(fTimeDelta);

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	
}

HRESULT CPlayer04WhiteBall::Render()
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
		if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixX", &m_vUVFixAcc.x, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixY", &m_vUVFixAcc.y, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Render(m_pShaderCom,i, 2)))
			return E_FAIL;
	}
	return S_OK;
}

void CPlayer04WhiteBall::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());
	_float3 vSize = Get_Colliders("Player_Attack")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Player_Attack")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);

	Get_Colliders("Player_Attack")->Set_Size(vSize);
	Get_Colliders("Player_Attack")->Set_Center(vCenter);

	ImGui::End();
}

_float4 CPlayer04WhiteBall::Get_BallPos()
{
	if (!m_bDead)
	{
		_float4 Pos;
		XMStoreFloat4(&Pos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		return Pos;
	}
	return _float4();
}

void CPlayer04WhiteBall::Create_RageParticleEffect(_uint iIndex)
{
	m_fTimeLimitAcc = 0.f;
	if (!EM->Get_RageParticleOn())
	{
		
		m_bBalckBall = true;
		m_iPhase = 3;
		return;
	}

	else
	{
		CRM->Get_Cam()->Shake(0.2f, 0.1f, false, 0.1f);
		if (iIndex == 0)
		{
			CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
			Info->fBlurPower = 0.8f;
			Info->vColor = { 0.4f,0.f,1.f,1.f };
			Info->vColor2 = { 0.f,0.f,0.f,1.f };
			Info->vLocalPos = { 0.f,0.f,0.f,1.f };
			Info->vRatio = { 1.f,3.f };
			Info->vRatioSpeed = { 3.f,4.f };
			Info->vScale = { 0.1f,0.1f,0.1f };
			Info->vScaleSpeed = { 300.f,300.f,300.f };
			Info->vUvFixSpeed = { 1.f,3.f };
			Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			Info->bFixPos = false;
			GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
			PTM->Create_InstanceParticle(L"Player04Phase3", m_pTransformCom->Get_WorldFloat4x4());
		}
		else
		{
			CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
			Info->fBlurPower = 0.8f;
			Info->vColor = { 0.4f,0.f,0.f,1.f };
			Info->vColor2 = { 0.f,0.f,0.f,1.f };
			Info->vLocalPos = { 0.f,0.f,0.f,1.f };
			Info->vRatio = { 1.f,3.f };
			Info->vRatioSpeed = { 3.f,4.f };
			Info->vScale = { 0.1f,0.1f,0.1f };
			Info->vScaleSpeed = { 300.f,300.f,300.f };
			Info->vUvFixSpeed = { 1.f,3.f };
			Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			Info->bFixPos = false;
			GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
			PTM->Create_InstanceParticle(L"Player04Phase3_2", m_pTransformCom->Get_WorldFloat4x4());
		}
		PTM->Create_InstanceParticle(L"Player04RageParticleEffect", m_pTransformCom->Get_WorldFloat4x4());

		if (m_bP2ScaleFix)
			return;
		else
		{
			m_bP2ScaleFix = true;
			m_bP2ScaleUp = true;
			m_fP2ScaleUpAcc = 0.f;
			m_fP2ScaleDownAcc = 0.f;
			m_pInfo->vColor = { 0.f,0.f,0.f,1.f };
			m_pInfo->vColor2 = { 0.f,0.f,0.f,1.f };
		}
	}

}

void CPlayer04WhiteBall::Phase1(_float fTimeDelta)
{
	
	if (m_bP1ScaleUp)
	{
		m_fP1ScaleUpAcc += 1.f * fTimeDelta;
		if (m_fP1ScaleUpAcc >= 0.11f)
		{
			m_bP1ScaleUp = false;
			m_fP1ScaleUpAcc = 0.f;
		}
		else
		{
			m_fP1ScaleUpSpeed = 200.f * fTimeDelta;
			m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pTransformCom->Get_Scale()) + _vector{ m_fP1ScaleUpSpeed, m_fP1ScaleUpSpeed, m_fP1ScaleUpSpeed } );

		}
	}

	else
	{
		m_fP2ScaleDownAcc += 1.f * fTimeDelta;
		if (m_fP2ScaleDownAcc >= 0.1f)
		{
			m_bP2ScaleUp = false;
			m_fP2ScaleUpAcc = 0.f;
			m_iPhase = 2;
		}
		else
		{
			m_fP1ScaleDownSpeed = 200.f * fTimeDelta;
			m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pTransformCom->Get_Scale()) - _vector{ m_fP1ScaleDownSpeed, m_fP1ScaleDownSpeed, m_fP1ScaleDownSpeed } );

		}
	}
	
	
}

void CPlayer04WhiteBall::Phase2(_float fTimeDelta)
{
	m_fTimeLimitAcc += 1.f * fTimeDelta;
	if (m_fTimeLimitAcc >= 7.f)
	{
		Set_Dead();
		PM->Set_Player04Rage1End();
	}
	if (m_bP2ScaleFix)
	{		
		if (m_bP2ScaleUp)
		{
			m_fP2ScaleUpAcc += 1.f * fTimeDelta;
			if (m_fP2ScaleUpAcc >= 0.05f)
			{
				m_bP2ScaleUp = false;
				m_fP2ScaleUpAcc = 0.f;
			}
			else
			{
				m_fP2ScaleUpSpeed = 300.f * fTimeDelta;
				m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pTransformCom->Get_Scale()) + _vector{ m_fP2ScaleUpSpeed, m_fP2ScaleUpSpeed, m_fP2ScaleUpSpeed } );

			}
		}

		else
		{
			m_fP2ScaleDownAcc += 1.f * fTimeDelta;
			if (m_fP2ScaleDownAcc >= 0.045f)
			{
				m_bP2ScaleUp = true;
				m_fP2ScaleDownAcc = 0.f;
				m_pInfo->vColor = { 1.f,1.f,1.f,1.f };
				m_pInfo->vColor2 = { 1.f,1.f,1.f,1.f };
				m_bP2ScaleFix = false;
			}
			else
			{
				m_fP2ScaleDownSpeed = 300.f * fTimeDelta;
				m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pTransformCom->Get_Scale()) - _vector{ m_fP2ScaleDownSpeed, m_fP2ScaleDownSpeed, m_fP2ScaleDownSpeed } );

			}
		}
	}


	
}

void CPlayer04WhiteBall::Phase3(_float fTimeDelta)
{
	m_fTimeLimitAcc += 1.f * fTimeDelta;

	if(m_fTimeLimitAcc <= 7.f)
		Phase3_Update(fTimeDelta);
	
	if (m_fTimeLimitAcc > 7.f && !m_bP3ShakeEnd)
	{
		CRM->Get_Cam()->Shake(3.f, 0.001f, false, 0.f);
		_matrix WorldMatrix = XMMatrixIdentity();
		WorldMatrix.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float4x4 Matrix;
		XMStoreFloat4x4(&Matrix, WorldMatrix);
		PTM->Create_InstanceParticle(L"Player04BlackBallEnd", Matrix);	
		m_bP3ShakeEnd = true;
		GI->PlaySoundW(L"EFF_CHR_Spell_Disappearance_01_A.ogg", SD_PLAYER4_SK, 0.8f); 
	}	
	
	if (m_fTimeLimitAcc >= 8.f)
	{
		m_bLightEffect = true;
		m_iPhase = 4;
		m_fTimeLimitAcc = 0.f;
	}
}

void CPlayer04WhiteBall::Phase4(_float fTimeDelta)
{
	m_fTimeLimitAcc += 1.f * fTimeDelta;
	if (m_fTimeLimitAcc >= 0.5f)
	{
		m_bTornado = true;
	}

	if (m_fTimeLimitAcc >= 0.6f)
	{
		EM->Release_Player04WHiteBall();
		Set_Dead();
	}
}

void CPlayer04WhiteBall::Create_BlackBall(_float fTimeDelta)
{
	BlackBallOn(fTimeDelta);

	CPlayer04Aura::AURAINFO* Info = new CPlayer04Aura::AURAINFO;
	Info->bRageAura = true;
	Info->fAngle = 0.f;
	Info->fBlurPower = 0.f;
	Info->fLifeTime = 7.f;
	Info->vColor = { 0.f,0.f,0.f,1.f };
	Info->vColor2 = { 0.4f,0.f,0.f,1.f };
	Info->vLocalPos = { 0.f,0.f,0.f,1.f };
	Info->vRatio = { 1.f,1.5f };
	Info->vRatioSpeed = { 5.f,5.f };
	Info->vScale = { 60.f,60.f,60.f };
	Info->vUVFixSpeed = { 0.f,0.5f };
	Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
	GI->Add_GameObjectToLayer(L"Player04Aura", TM->Get_CurLevel(), L"Layer_Player04Effect", &Info);

	CPlayer04Aura::AURAINFO* Info2 = new CPlayer04Aura::AURAINFO;
	Info2->bRageAura = true;
	Info2->fAngle = 112.f;
	Info2->fBlurPower = 0.f;
	Info2->fLifeTime = 7.f;
	Info2->vColor = { 0.f,0.f,0.f,1.f };
	Info2->vColor2 = { 0.4f,0.f,1.f,1.f };
	Info2->vLocalPos = { 0.f,0.f,0.f,1.f };
	Info2->vRatio = { 1.f,1.5f };
	Info2->vRatioSpeed = { 5.f,5.f };
	Info2->vScale = { 50.f,50.f,50.f };
	Info2->vUVFixSpeed = { 0.f,0.5f };
	Info2->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
	GI->Add_GameObjectToLayer(L"Player04Aura", TM->Get_CurLevel(), L"Layer_Player04Effect", &Info2);

	m_pInfo->vColor = { 0.f,0.f,0.f,1.f };
	m_pInfo->vColor2 = { 0.f,0.f,0.f,1.f };
	m_pTransformCom->Set_Scale(_vector{ 3.f,3.f,3.f });
	_vector Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	Pos.m128_f32[1] += 0.5f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Pos);

	m_bBalckBall = false;

}

void CPlayer04WhiteBall::Create_Tornado(_float fTimeDelta)
{
	if (!m_bTornadoCreate)
	{
		TIM->Reset_All();
		//CRM->Get_Cam()->Shake(0.6f, 0.5f, false, 0.1f);
		CPlayer04Aura::AURAINFO* Info5 = new CPlayer04Aura::AURAINFO;
		Info5->fBlurPower = 0.f;
		Info5->fLifeTime = 5.f;
		Info5->fAngle = 104.f;
		Info5->vColor = { 0.f,0.f,0.f,1.f };
		Info5->vColor2 = { 0.4f,0.f,0.f,1.f };
		Info5->vLocalPos = { 0.f,0.f,3.f,1.f };
		Info5->vRatio = { 1.f,1.f };
		Info5->vRatioSpeed = { 5.f,5.f };
		Info5->vScale = { 410.f,410.f,410.f };
		Info5->vUVFixSpeed = { 0.f,2.f };
		Info5->vWorldMatrix = PM->Get_PlayerWorldMatrix();
		GI->Add_GameObjectToLayer(L"Player04Aura", TM->Get_CurLevel(), L"Layer_Player04Effect", &Info5);

		CPlayer04Aura::AURAINFO* Info6 = new CPlayer04Aura::AURAINFO;
		Info6->fBlurPower = 0.f;
		Info6->fLifeTime = 5.f;
		Info6->fAngle = 54.f;
		Info6->vColor = { 0.f,0.f,0.f,1.f };
		Info6->vColor2 = { 0.4f,0.f,1.f,1.f };
		Info6->vLocalPos = { 0.f,0.f,3.f,1.f };
		Info6->vRatio = { 1.f,1.f };
		Info6->vRatioSpeed = { 5.f,5.f };
		Info6->vScale = { 410.f,410.f,410.f };
		Info6->vUVFixSpeed = { 0.f,2.f };
		Info6->vWorldMatrix = PM->Get_PlayerWorldMatrix();
		GI->Add_GameObjectToLayer(L"Player04Aura", TM->Get_CurLevel(), L"Layer_Player04Effect", &Info6);



		CTornadoSide::TORNADOSIDEINFO* Info3 = new CTornadoSide::TORNADOSIDEINFO;
		Info3->fBlurPower = 1.f;
		Info3->fLifeTime = 5.f;
		Info3->vColor = { 0.f,0.f,0.f,1.f };
		Info3->vColor2 = { 0.4f,0.f,1.f,1.f };
		Info3->vLocalPos = { 0.f,0.f,3.f,1.f };
		Info3->vRatio = { 1.5f,1.5f };
		Info3->vRatioSpeed = { 2.f,2.f };
		Info3->vScale = { 10.f,8.f,10.f };
		Info3->vUVFixSpeed = { 3.f,0.f };
		Info3->vWorldMatrix = PM->Get_PlayerWorldMatrix();
		GI->Add_GameObjectToLayer(L"TornadoSide", TM->Get_CurLevel(), L"Layer_Player04Effect", &Info3);

		CTornadoSide::TORNADOSIDEINFO* Info4 = new CTornadoSide::TORNADOSIDEINFO;
		Info4->fBlurPower = 1.f;
		Info4->fLifeTime = 5.f;
		Info4->vColor = { 0.f,0.f,0.f,1.f };
		Info4->vColor2 = { 0.4f,0.f,1.f,1.f };
		Info4->vLocalPos = { 0.f,0.f,3.f,1.f };
		Info4->vRatio = { 1.5f,1.5f };
		Info4->vRatioSpeed = { 2.f,2.f };
		Info4->vScale = { 10.f,8.f,10.f };
		Info4->vUVFixSpeed = { 3.f,0.f };
		Info4->vUVFixAcc = { 1.f,0.f };
		Info4->fAngle = 90.f;
		Info4->vWorldMatrix = PM->Get_PlayerWorldMatrix();
		GI->Add_GameObjectToLayer(L"TornadoSide", TM->Get_CurLevel(), L"Layer_Player04Effect", &Info4);

		CPlayer01Wind2::WIND2INFO* Info7 = new CPlayer01Wind2::WIND2INFO;
		Info7->bLookFix = false;
		Info7->bRotation = false;
		Info7->fBlurPower = 0.f;
		Info7->fLookSpeed = 0.f;
		Info7->fTurnSpeed = 15.f;
		Info7->vColor = { 0.f,0.f,0.f,1.f };
		Info7->vColor2 = { 0.4f,0.f,1.f,1.f };
		Info7->vLocalPos = { 0.f,0.f,3.f,1.f };
		Info7->vRatio = { 1.5f,1.f };
		Info7->vRatioSpeed = { 5.f,5.f };
		Info7->vScale = { 100.f,60.f,100.f };
		Info7->vScaleSpeed = { 0.f,0.f,0.f };
		Info7->fLifeTime = 5.f;
		Info7->vWorldMatrix = PM->Get_PlayerWorldMatrix();
		GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player04Effect", &Info7);

		CPlayer01Wind2::WIND2INFO* Info8 = new CPlayer01Wind2::WIND2INFO;
		Info8->bLookFix = false;
		Info8->bRotation = false;
		Info8->fBlurPower = 0.f;
		Info8->fLookSpeed = 0.f;
		Info8->fTurnSpeed = 10.f;
		Info8->vColor = { 0.f,0.f,0.f,1.f };
		Info8->vColor2 = { 0.4f,0.f,0.f,1.f };
		Info8->vLocalPos = { 0.f,0.f,3.f,1.f };
		Info8->vRatio = { 1.5f,1.f };
		Info8->vRatioSpeed = { 5.f,5.f };
		Info8->vScale = { 70.f,60.f,70.f };
		Info8->vScaleSpeed = { 0.f,0.f,0.f };
		Info8->fLifeTime = 5.f;
		Info8->vWorldMatrix = PM->Get_PlayerWorldMatrix();
		GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player04Effect", &Info8);

		CPlayer01Wind1::WIND1INFO* Info9 = new CPlayer01Wind1::WIND1INFO;
		Info9->bLookFix = false;
		Info9->bRotation = false;
		Info9->fBlurPower = 0.f;
		Info9->fLookSpeed = 0.f;
		Info9->fTurnSpeed = 8.f;
		Info9->vColor = { 0.f,0.f,0.f,1.f };
		Info9->vColor2 = { 0.f,0.f,0.f,1.f };
		Info9->vLocalPos = { 0.f,0.f,3.f,1.f };
		Info9->vRatio = { 1.f,1.f };
		Info9->vRatioSpeed = { 5.f,5.f };
		Info9->vScale = { 50.f,20.f,50.f };
		Info9->vScaleSpeed = { 0.f,0.f,0.f };
		Info9->fLifeTime = 5.f;
		Info9->vWorldMatrix = PM->Get_PlayerWorldMatrix();
		GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player04Effect", &Info9);

		CTornado::TORNADOINFO* Info = new CTornado::TORNADOINFO;
		Info->vWorldMatrix = PM->Get_PlayerWorldMatrix();
		Info->fBlurPower = 1.f;
		Info->fTurnSpeed = 8.f;
		Info->vScale = { 8.f, 8.f, 8.f };
		Info->vLocalPos = { 0.f,0.f,3.f,1.f };
		Info->vColor = { 0.f,0.f,0.f,1.f };
		Info->vColor2 = { 1.f,0.f,0.f,1.f };
		Info->vRatio = { 2.f,1.5f };
		Info->fLifeTime = 5.f;
		Info->vRatioSpeed = { 2.f,2.f };
		Info->vUVFixSpeed = { -0.5f,-0.5f };
		Info->bOrigin = true;
		GI->Add_GameObjectToLayer(L"Tornado", TM->Get_CurLevel(), L"Layer_Player04Effect", &Info);

		CTornado::TORNADOINFO* Info2 = new CTornado::TORNADOINFO;
		Info2->vWorldMatrix = PM->Get_PlayerWorldMatrix();
		Info2->fBlurPower = 1.f;
		Info2->vScale = { 10.f, 8.f, 10.f };
		Info2->vLocalPos = { 0.f,0.f,3.f,1.f };
		Info2->fTurnSpeed = 20.f;
		Info2->vColor = { 0.f,0.f,0.f,1.f };
		Info2->vColor2 = { 0.1f,0.f,0.3f,1.f };
		Info2->vRatio = { 2.f,1.5f };
		Info2->fLifeTime = 5.f;
		Info2->vRatioSpeed = { 2.f,2.f };
		Info2->vUVFixSpeed = { 1.f,1.f };
		Info2->bOrigin = false;
		GI->Add_GameObjectToLayer(L"Tornado", TM->Get_CurLevel(), L"Layer_Player04Effect", &Info2);


		CViewAlpha::VIEWALPHAINFO* Info10 = new CViewAlpha::VIEWALPHAINFO;
		Info10->fBlurPower = 1.f;
		Info10->vColor1 = { 0.f,0.f,0.f,1.f };
		Info10->vColor2 = { 0.2f,0.f,0.5f,1.f };
		Info10->vRatio = { 2.f,1.f };
		Info10->vRatioSpeed = { 2.f,0.5f };
		GI->Add_GameObjectToLayer(L"ViewAlpha", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info10);
		m_bTornadoCreate = true;

		//CRM->Get_Cam()->Fov(1.5f, 0.8f, 0.5f);
		
	}
}

void CPlayer04WhiteBall::Create_LightEffect(_float fTimeDelta)
{
	m_bLightEffect = false;
}

void CPlayer04WhiteBall::Phase3_Update(_float fTimeDelta)
{
	

	if (m_bP3ScaleFix)
	{
		if (m_bP3ScaleUp)
		{
			m_fP3ScaleUpAcc += 1.f * fTimeDelta;
			if (m_fP3ScaleUpAcc >= 0.1f)
			{
				m_bP3ScaleUp = false;
				m_fP3ScaleUpAcc = 0.f;
				m_fP3ShakePower += 0.01f;
				
			}
			else
			{		
				m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pTransformCom->Get_Scale()) + _vector{ m_fP3ScaleUpSpeed, m_fP3ScaleUpSpeed, m_fP3ScaleUpSpeed } *fTimeDelta);
			}
		}
		else
		{
			m_fP3ScaleDownAcc += 1.f * fTimeDelta;
			if (m_fP3ScaleDownAcc >= 0.1f)
			{
				m_bP3ScaleUp = true;
				m_fP3ScaleDownAcc = 0.f;
				m_bP3ScaleFix = false;
			}
			else
			{
				
				m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pTransformCom->Get_Scale()) - _vector{ m_fP3ScaleDownSpeed, m_fP3ScaleDownSpeed, m_fP3ScaleDownSpeed } *fTimeDelta);

			}
		}
	}
	else
	{
		m_fP3ScaleFixAcc += 1.f * fTimeDelta;
		if (m_fP3ScaleFixAcc >= m_fP3ScaleFixLimit)
		{
			m_bP3ScaleFix = true;
			if (m_fP3ScaleFixLimit >= 0.1f)
				m_fP3ScaleFixLimit -= 0.05f;
			m_fP3ScaleFixAcc = 0.f;			
			m_fP3ScaleUpSpeed += 2.5f;
			m_fP3ScaleDownSpeed += 2.5f;
			CRM->Get_Cam()->Shake(m_fP3ShakePower, 3.f, false, 0.1f);
			int iRandom = GI->Get_Random(0, 1);
			if (iRandom == 0)
				Create_BlackBallEffect(iRandom, fTimeDelta);
			else
				Create_BlackBallEffect(iRandom, fTimeDelta);
			GI->PlaySoundW(L"EFF_CHR_Chakram_Attack_Shot_01_A.ogg", SD_PLAYER4_SK, 0.5f); 	

		}
	}
}

void CPlayer04WhiteBall::BlackBallOn(_float fTimeDelta)
{
	CRM->Get_Cam()->Shake(0.4f, 0.1f, false, 0.2f);

	CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
	Info->fBlurPower = 0.f;
	Info->vColor = { 0.f,0.f,0.f,1.f };
	Info->vColor2 = { 0.4f,0.f,1.f,1.f };
	Info->vLocalPos = { 0.f,0.f,0.f,1.f };
	Info->vRatio = { 3.f,1.f };
	Info->vRatioSpeed = { 2.f,2.f };
	Info->vScale = { 1.f,1.f,1.f };
	Info->vScaleSpeed = { 400.f,400.f,400.f };
	Info->vUvFixSpeed = { 2.f,1.f };
	Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
	Info->bFixPos = false;
	GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
			
	CPlayer01SmokeBall::SMOKEBALLINFO* Info2 = new CPlayer01SmokeBall::SMOKEBALLINFO;
	Info2->fBlurPower = 0.f;
	Info2->vColor = { 0.f,0.f,0.f,1.f };
	Info2->vColor2 = { 0.4f,0.f,0.f,1.f };
	Info2->vLocalPos = { 0.f,0.f,0.f,1.f };
	Info2->vRatio = { 2.f,2.f };
	Info2->vRatioSpeed = { 2.f,1.f };
	Info2->vScale = { 1.f,1.f,1.f };
	Info2->vScaleSpeed = { 400.f,400.f,400.f };
	Info2->vUvFixSpeed = { -2.f,-1.f };
	Info2->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
	Info2->bFixPos = false;
	GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2);

	PTM->Create_InstanceParticle(L"Player04BlackBallOn", m_pTransformCom->Get_WorldFloat4x4());
	PTM->Create_InstanceParticle(L"Player04BlackBallOn_2", m_pTransformCom->Get_WorldFloat4x4());
	PTM->Create_InstanceParticle(L"Player04BlackBallOn_3", m_pTransformCom->Get_WorldFloat4x4());

}

void CPlayer04WhiteBall::Create_BlackBallEffect(_uint iIndex, _float fTimeDelta)
{
	_matrix WorldMatrix = XMMatrixIdentity();
	WorldMatrix.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float4x4 Matrix;
	XMStoreFloat4x4(&Matrix, WorldMatrix);

	CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
	Info->fBlurPower = 0.f;
	Info->fAngle = GI->Get_FloatRandom(0.f, 360.f);
	Info->vColor = { 0.f,0.f,0.f,1.f };
	Info->vColor2 = { 0.4f,0.f,1.f,1.f };
	Info->vLocalPos = { 0.f,0.f,0.f,1.f };
	Info->vRatio = { 1.5f,1.f };
	Info->vRatioSpeed = { 4.f,4.f };
	Info->vScale = { 1.f,1.f,1.f };
	Info->vScaleSpeed = { 300.f,300.f,300.f };
	Info->vUvFixSpeed = { 2.f,1.f };
	Info->vWorldMatrix = Matrix;
	Info->bFixPos = false;
	GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);

	CPlayer01SmokeBall::SMOKEBALLINFO* Info2 = new CPlayer01SmokeBall::SMOKEBALLINFO;
	Info2->fBlurPower = 0.f;
	Info2->fAngle = GI->Get_FloatRandom(0.f, 360.f);
	Info2->vColor = { 0.f,0.f,0.f,1.f };
	Info2->vColor2 = { 0.4f,0.f,0.f,1.f };
	Info2->vLocalPos = { 0.f,0.f,0.f,1.f };
	Info2->vRatio = { 1.5f,1.f };
	Info2->vRatioSpeed = { 4.f,4.f };
	Info2->vScale = { 1.f,1.f,1.f };
	Info2->vScaleSpeed = { 300.f,300.f,300.f };
	Info2->vUvFixSpeed = { -2.f,-1.f };
	Info2->vWorldMatrix = Matrix;
	Info2->bFixPos = false;
	GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2);

	CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
	Info3->fBlurPower = 1.f;
	Info3->vLocalPos = { 0.f,-1.9f,0.f,1.f };
	Info3->vColor = { 0.f,0.f,0.f,1.f };
	Info3->vColor2 = { 0.4f,0.f,0.f,1.f };
	Info3->vRatio = { 3.f,1.5f };
	Info3->vRatioSpeed = { 4.f,4.f };
	Info3->vScaleSpeed = { 100.f,140.f,100.f };
	Info3->fTurnSpeed = 20.f;
	Info3->vWorldMatrix = Matrix;
	GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player04Effect", &Info3);
		

	CPlayer01Wind2::WIND2INFO* Info4 = new CPlayer01Wind2::WIND2INFO;
	Info4->fBlurPower = 1.f;
	Info4->vLocalPos = { 0.f,-1.6f,0.f,1.f };
	Info4->vColor = { 0.f,0.f,0.f,1.f };
	Info4->vColor2 = { 0.2f,0.f,0.5f,1.f };
	Info4->vRatio = { 3.f,1.5f };
	Info4->vRatioSpeed = { 4.f,4.f };
	Info4->vScaleSpeed = { 100.f,140.f,100.f };
	Info4->fTurnSpeed = 15.f;
	Info4->vWorldMatrix = Matrix;
	GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player04Effect", &Info4);
	

	PTM->Create_InstanceParticle(L"Player04BlackBallOn_4", m_pTransformCom->Get_WorldFloat4x4());

	if (m_fTimeLimitAcc >= 3.f)
	{		
		CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
		Info->fBlurPower = 0.f;
		Info->fAngle = GI->Get_FloatRandom(0.f, 360.f);
		Info->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
		Info->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 0.7f,0.7f };
		Info->vRatioSpeed = { 0.5f,0.5f };
		Info->vScale = { 0.1f,0.1f,0.1f };
		Info->vScaleSpeed = { 400.f,400.f,400.f };
		Info->vUvFixSpeed = { 0.5f,1.f };
		Info->vWorldMatrix = Matrix;
		GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
	}
	
}

HRESULT CPlayer04WhiteBall::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Player04MeshEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_BlastBall"), TEXT("Effect_BlastBall"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

CCreture * CPlayer04WhiteBall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer04WhiteBall*		pInstance = new CPlayer04WhiteBall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer04WhiteBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer04WhiteBall::Clone(void * pArg)
{
	CPlayer04WhiteBall*		pInstance = new CPlayer04WhiteBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer04WhiteBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer04WhiteBall::Free()
{
	__super::Free();
	if(m_pInfo != nullptr)
	Safe_Delete(m_pInfo);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	

}
