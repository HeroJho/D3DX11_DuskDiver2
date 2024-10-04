#include "stdafx.h"
#include "..\Public\MechanicalOctopusLeg.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "ToolManager.h"
#include "Monster_Manager.h"
#include "Time_Manager.h"
#include "Camera_Manager.h"
#include "BossBomb.h"
#include "UI.h"
#include "BossMissile.h"
#include "Particle_Manager.h"
#include "Player01SmokeBall.h"

CMechanicalOctopusLeg::CMechanicalOctopusLeg(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CMechanicalOctopusLeg::CMechanicalOctopusLeg(const CMechanicalOctopusLeg & rhs)
	: CCreture(rhs)
{
}

HRESULT CMechanicalOctopusLeg::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CMechanicalOctopusLeg::Initialize(void * pArg)
{	
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	CCreture::CREATEINFO CreateInfo = *(CCreture::CREATEINFO*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Create_Navigation(CreateInfo.iNaviNum)))
		return E_FAIL;

	m_sTag = "Tag_Boss";
	// *[HeroJho]: ImGui로 디버깅을 하려면 꼭 호출해줘야 합니다.
	Make_ImGui();

	Ready_HierarchyNode();

	m_pModelCom->Set_TickPerSecond(0.f);

	m_eCurState = ATTACK1_1;
	m_pModelCom->Set_AnimIndex(ATTACK1_1);


	m_fPlayerPos = { 0.f,0.f,0.f, 1.f }; //일단 초기화
	m_vPlayerLook = { 0.f,0.f,1.f };
	
	m_fHurtTime = 0.02f;

	m_fMaxHp = 2000.f;
	m_fNowHp = m_fMaxHp;
	m_fStamina = 3.f;
	m_fDamage = 10.f;

	m_fPreHp = m_fNowHp;

	m_vTargetLook = _float3{ 0.f,0.f,1.f };
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&m_vTargetLook));

	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Dissolve"), TEXT("Com_Texture"), (CComponent**)&m_pDissolveTexture)))
		return E_FAIL;

	MM->Add_Monster(this);

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	return S_OK;
}

void CMechanicalOctopusLeg::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_MONSTER, this);
	Check_Hurt(fTimeDelta);
	Check_Rim(fTimeDelta);
	fTimeDelta *= m_fTMRatio;
	Emp_Col();
	Distance_BattlePattern(fTimeDelta);
	Update(fTimeDelta);
	Check_Death(fTimeDelta);
	Set_PreBar(fTimeDelta);

}

void CMechanicalOctopusLeg::LateTick(_float fTimeDelta)
{
	if (m_bGara)
	{
		if (nullptr == m_pRendererCom)
			return;

		fTimeDelta *= m_fTMRatio;

		m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom, m_pNavigationCom);	

		End_Animation();

		Tick_Col(m_pTransformCom->Get_WorldMatrix()* m_pParentTransformCom->Get_WorldMatrix());

		if (m_bHurt)
			Choose_Col("Monster_Body1");

		if(m_bRender)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);


		Render_Col(m_pRendererCom);

	}
}

HRESULT CMechanicalOctopusLeg::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	_float4x4		WorldMatrix;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix()));

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);


	_float fGama = 1.4f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;

	//// For.Rim
	if (FAILED(m_pShaderCom->Set_RawValue("g_CamPosition", &GI->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	_float g_RimPower = 3.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_RimPower", &g_RimPower, sizeof(_float))))
		return E_FAIL;

	
	// For.Blur
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &m_fBlurPower, sizeof(_float))))
		return E_FAIL;

	// For Emi
	if (FAILED(m_pEmissiveCom->Set_SRV(m_pShaderCom, "g_EmissiveTexture", 0)))
		return E_FAIL;
	

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{	
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pDissolveTexture->Set_SRV(m_pShaderCom, "g_DissolveTexture", 0)))
			return E_FAIL;

		m_pShaderCom->Set_RawValue("g_fDissolveAcc", &m_fDissolveAcc, sizeof(float));
		
		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, m_iShaderPath)))
			return E_FAIL;
	}	

	return S_OK;
}

// *[HeroJho]: 디버깅 창입니다. ImGui로 작동됩니다.
void CMechanicalOctopusLeg::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);

	if (m_bCheckFront) //디버깅용
	{
		m_fTest = 1.f;
	}
	else
		m_fTest = -1.f;

	ImGui::Text("distance : %f", m_fDistance);

/*
	ImGui::Text("FrontCheck : %f", m_fTest);
	ImGui::Text("distance : %f", m_fDistance);
	ImGui::Text("AttackNum : %d", m_iAttackNum);

	ImGui::DragFloat("BlurPower ", &m_fBlurPower, 0.01f);

	ImGui::Checkbox("Pause", &m_bGara);


	_float3 vSize = Get_Colliders("Monster_WeaponHead")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Monster_WeaponHead")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Get_Colliders("Monster_WeaponHead")->Set_Size(vSize);
	Get_Colliders("Monster_WeaponHead")->Set_Center(vCenter);
*/

	ImGui::Checkbox("Pause", &m_bGara);

	_float3 vSize = Get_Colliders("Monster_Body1")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Monster_Body1")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Get_Colliders("Monster_Body1")->Set_Size(vSize);
	Get_Colliders("Monster_Body1")->Set_Center(vCenter);
	ImGui::DragFloat4("Power, Time, LerpTime", (float*)&ShakeTest, 0.01f, -10.f, 10.f, "%.2f");

	ImGui::DragFloat4("Power, Time, StopTime", (float*)&FowTest, 0.01f, -10.f, 10.f, "%.2f");

	ImGui::End();
}

// *[HeroJho]: 충돌이 발생할 때 호출되는 함수입니다.
void CMechanicalOctopusLeg::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	// *[HeroJho]:
	// Desc.pOther		- 충돌한 상대편 객체의 주소입니다.
	// Desc.pMyDesc		- 상대편과 충돌한 나의 콜라이더 정보입니다.
	// Desc.pOtherDesc	- 나와 충돌한 상대편의 콜라이더 정보입니다.
}

void CMechanicalOctopusLeg::On_Hit(CCreture* pOther)
{
	//hit이 한번만 처리되게끔
	if (m_bHurt && m_bHitAble)
	{
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		vPos.y += 2.f;
		pOther->Create_HitEffect(vPos);

		GI->PlaySoundW(L"SwordHit.ogg", SD_HIT, 1.f);
		m_bRim = true;
		m_fRimAcc = 0.f;
		m_bHurt = false;
		m_bAttackCol = false;

		m_fNowHp -= (pOther->Get_Damage() - m_fStamina);

		if (m_bDamageAble && !m_bReGauge)
		{
			m_fPatternGauge -= (pOther->Get_Damage() * 1.2f);
			Check_Stun();
		}

		if (m_fNowHp <= 0.f)
		{
			m_fNowHp = 0.f;
			m_bHurt = false;
			m_bSpawn = false;
		}

		UM->Plus_Combo();
		UM->Set_Hit(true);
		UM->Set_Up(true);

	}

}

_float4 CMechanicalOctopusLeg::Get_WorldPos()
{
	XMStoreFloat4(&m_fWorldPos,  (m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix()).r[3]);
	return m_fWorldPos;
}

HRESULT CMechanicalOctopusLeg::SetUp_State(_fmatrix StateMatrix)
{
	m_pParentTransformCom->Set_State(CTransform::STATE_RIGHT, StateMatrix.r[0]);
	m_pParentTransformCom->Set_State(CTransform::STATE_UP, StateMatrix.r[1]);
	m_pParentTransformCom->Set_State(CTransform::STATE_LOOK, StateMatrix.r[2]);
	m_pParentTransformCom->Set_State(CTransform::STATE_POSITION, StateMatrix.r[3]);

	m_pParentTransformCom->Set_Scale(XMVectorSet(1.f, 1.f, 1.f, 1.f));

	return S_OK;
}

void CMechanicalOctopusLeg::Control_MechanicalOctopus()
{
	//m_bSpawn = true;
	m_bOneCheck = false;
	m_bAction = true;
}

_float4x4 CMechanicalOctopusLeg::Get_WorldMatrix()
{

	_float4x4 WorldMatrix;
	
	XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix());

	return WorldMatrix;
}

void CMechanicalOctopusLeg::Set_State(STATE eState) //end 애니메이션에서
{
	//상태 보간 하는 함수
	if (m_eNextState == eState)
		return;

	m_eNextState = eState;

	switch (m_eNextState)
	{
		//다음 상태로 넘어갈때 해줘야하는것들
		//애니메이션x
	case Client::CMechanicalOctopusLeg::ATTACK1_1:
		break;
	case Client::CMechanicalOctopusLeg::ATTACK1_2:
		m_bShootMissile1_2 = false;
		break;
	case Client::CMechanicalOctopusLeg::ATTACK1_3:
		GI->PlaySoundW(L"se_EN0103_hatch2.wav", SD_OCTOPUSLEG, 0.5f);
		m_bShootMissile1_3 = false;
		m_bShootMissile1_4 = false;
		break;
	case Client::CMechanicalOctopusLeg::ATTACK2_1:
		break;
	case Client::CMechanicalOctopusLeg::ATTACK2_2:
		m_bShootBullet2 = false;
		m_bShootMissile2_2 = false;
		break;
	case Client::CMechanicalOctopusLeg::ATTACK2_3:
		GI->PlaySoundW(L"se_EN0103_hatch2.wav", SD_OCTOPUSLEG, 0.5f);
		break;
	case Client::CMechanicalOctopusLeg::ATTACK3:
		m_bShootBullet3 = false;
		break;
	}


	m_pModelCom->SetNextIndex(m_eNextState);
	m_pModelCom->SetChangeBool(true); 
	m_eCurState = m_eNextState; //보간시작할때 상태를 바꿔줌

}

void CMechanicalOctopusLeg::End_Animation()
{
	if (m_pModelCom->GetAniEnd())
	{
		//애니메이션 끝났을때
		switch (m_eCurState)
		{
			//다음 상태로 넘어갈때 해줘야하는것들
			//애니메이션x
		case Client::CMechanicalOctopusLeg::ATTACK1_1:
			Set_State(ATTACK1_2);
			break;
		case Client::CMechanicalOctopusLeg::ATTACK1_2:
			Set_State(ATTACK1_3);
			break;
		case Client::CMechanicalOctopusLeg::ATTACK1_3:
			m_bOneRandom = true;
			m_bOneCheck = false;
			m_bMove = false;
			m_bPour = false;
			m_bAttack = false;
			m_pModelCom->Set_AnimIndex(ATTACK1_1);
			m_eCurState = ATTACK1_1;
			m_eNextState = m_eCurState;
			m_pModelCom->SetNextIndex(m_eNextState);
			m_bShootMissile1_1 = false;
			break;
		case Client::CMechanicalOctopusLeg::ATTACK2_1:
			Set_State(ATTACK2_2);
			break;
		case Client::CMechanicalOctopusLeg::ATTACK2_2:
			Set_State(ATTACK2_3);
			break;
		case Client::CMechanicalOctopusLeg::ATTACK2_3:
			m_bOneRandom = true;
			m_bOneCheck = false;
			m_bMove = false;
			m_bShootMissile2_1 = false;
			m_pModelCom->Set_AnimIndex(ATTACK2_1);
			m_eCurState = ATTACK2_1;
			m_eNextState = m_eCurState;
			m_pModelCom->SetNextIndex(m_eNextState);
			break;
		case Client::CMechanicalOctopusLeg::ATTACK3:
			m_bOneRandom = true;
			m_bOneCheck = false;
			m_pModelCom->Set_TickPerSecond(0.f);
			m_bMove = false;
			break;
		default:
			break;

		}
	}
}


void CMechanicalOctopusLeg::Update(_float fTimeDelta)
{
	m_bRangeCol = true;

	if (!m_pModelCom->GetChangeBool())
	{
		switch (m_eCurState)
		{
	
		case Client::CMechanicalOctopusLeg::ATTACK1_1:
			Attack1_1Update(fTimeDelta);
			break;
		case Client::CMechanicalOctopusLeg::ATTACK1_2:
			Attack1_2Update(fTimeDelta);
			break;
		case Client::CMechanicalOctopusLeg::ATTACK1_3:
			Attack1_3Update(fTimeDelta);
			break;
		case Client::CMechanicalOctopusLeg::ATTACK2_1:
			Attack2_1Update(fTimeDelta);
			break;
		case Client::CMechanicalOctopusLeg::ATTACK2_2:
			Attack2_2Update(fTimeDelta);
			break;
		case Client::CMechanicalOctopusLeg::ATTACK2_3:
			Attack2_3Update(fTimeDelta);
			break;
		case Client::CMechanicalOctopusLeg::ATTACK3:
			Attack3_Update(fTimeDelta);
			break;
		default:
			break;
		}

	}

}

void CMechanicalOctopusLeg::Set_Dir()
{
	//플레이어 포스 받아오기 미리 받아둘지

	//내가 바라봐야할 룩벡터 구하기, 플레이어 포스에서 내포스 빼고 노멀라이즈
	_vector vLook = XMVector3Normalize(XMLoadFloat4(&m_fPlayerPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	XMStoreFloat3(&m_vTargetLook, vLook);


}


void CMechanicalOctopusLeg::Distance_BattlePattern(_float fTimeDelta)
{
	m_fPlayerPos = PM->Get_PlayerPos();

	if (m_bMove)
	{
		BasicAttack(fTimeDelta);
	}

	
}

void CMechanicalOctopusLeg::BasicAttack(_float fTimeDelta)
{
	if (m_bOneRandom)
	{
		_uint iRandomAttack = GI->Get_Random(1, 3);
		m_bOneRandom = false;

		if (m_bPour)
			iRandomAttack = 1;

		switch (iRandomAttack)
		{
		case 1:
			Attack1(fTimeDelta);
			break;
		case 2:
			Attack2(fTimeDelta);
			break;
		case 3:
			Attack3(fTimeDelta);
			break;
		default:
			break;
		}
	}
		

}


void CMechanicalOctopusLeg::Check_Rim(_float fTimeDelta)
{
	if (m_bRim)
	{
		m_fRimAcc += 1.f * fTimeDelta;

		if (m_fRimAcc > 0.2f)
		{
			m_bRim = false;
			m_fRimAcc = 0.f;
		}
	}

	if (m_bRim)
	{
		m_fBlurPower = 0.f;
		m_iShaderPath = 1;
	}

	else
	{
		m_fBlurPower = 1.f;
		m_iShaderPath = 12;// 4;
	}

}

void CMechanicalOctopusLeg::Check_Death(_float fTimeDelta)
{

	Check_Recover(fTimeDelta);

	if (!m_pTransformCom->Get_Air() && m_fNowHp <= 0.f && !m_bDeadStart)
	{
		m_bAction = false;
		m_bHitAble = false;
		m_bHurtCol = false;
		m_bRangeCol = false;
		m_bAttackCol = false;
		m_bDeadStart = true;
		m_bMove = false;
	
		m_bDissolve = true;

		GI->PlaySoundW(L"se_EN0103_wpn_dead_1.wav", SD_OCTOPUS, 0.7f);
		MM->Event_MechanicalLeg();

		_matrix a = m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix();
		a.r[0] = XMVector3Normalize(a.r[0]);
		a.r[1] = XMVector3Normalize(a.r[1]);
		a.r[2] = XMVector3Normalize(a.r[2]);
		_float4x4 Matrix;
		XMStoreFloat4x4(&Matrix, a);

		PTM->Create_InstanceParticle(L"OctopuslegFire2", Matrix);
		PTM->Create_InstanceParticle(L"OctopuslegFire", Matrix);

	}

	if (m_bDissolve)
	{
		m_iShaderPath = 6;
		m_fDissolveAcc += 0.9f * fTimeDelta;

		if (m_fDissolveAcc >= 1.f)
		{
			m_fDissolveAcc = 1.f;

			if (m_bDeadOne)
			{
				m_bDeadOne = false;

			}
		}

	}
}

void CMechanicalOctopusLeg::Check_Stun()
{
	if (m_fPatternGauge <=  0.f)
	{
		m_bStun = true;
		m_bHitMove = false;
		m_bAction = false;
		Set_State(ATTACK1_1);
	}
}

void CMechanicalOctopusLeg::Check_Recover(_float fTimeDelta)
{
	if (m_bStun)
	{
		m_fRecoverAcc += 1.f * fTimeDelta;
		m_bDamageAble = false;
	}

	if (m_fRecoverAcc >= 6.f)
		m_bStun = false;
}

void CMechanicalOctopusLeg::Reset_Collider()
{
	m_bHurtCol = false;
}

void CMechanicalOctopusLeg::Create_Particle(_float4 fSize)
{
	
}

void CMechanicalOctopusLeg::Missile_Sound()
{
	_uint iRandomMissile = GI->Get_Random(0, 3);
	switch (iRandomMissile)
	{
	case 0:
		GI->PlaySoundW(L"se_EN0103_missle_2-01.wav", SD_BULLET, 0.3f);
		break;
	case 1:
		GI->PlaySoundW(L"se_EN0103_missle_2-02.wav", SD_BULLET, 0.3f);
		break;
	case 2:
		GI->PlaySoundW(L"se_EN0103_missle_2-03.wav", SD_BULLET, 0.3f);
		break;
	case 3:
		GI->PlaySoundW(L"se_EN0103_missle_2-04.wav", SD_BULLET, 0.3f);
		break;
	}
}


void CMechanicalOctopusLeg::Launch_Missile1() //일반 카이사큐
{
	_matrix Missile1Matrix = m_pMissileNode1->Get_CombinedTransformation()
		* m_pModelCom->Get_PivotMatrix()
		*m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix();
	_matrix Missile2Matrix = m_pMissileNode2->Get_CombinedTransformation()
		* m_pModelCom->Get_PivotMatrix()
		*m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix();
	_matrix Missile3Matrix = m_pMissileNode3->Get_CombinedTransformation()
		* m_pModelCom->Get_PivotMatrix()
		*m_pTransformCom->Get_WorldMatrix()* m_pParentTransformCom->Get_WorldMatrix();

	_matrix Missile7Matrix = m_pMissileNode7->Get_CombinedTransformation()
		* m_pModelCom->Get_PivotMatrix()
		*m_pTransformCom->Get_WorldMatrix()* m_pParentTransformCom->Get_WorldMatrix();
	_matrix Missile8Matrix = m_pMissileNode8->Get_CombinedTransformation()
		* m_pModelCom->Get_PivotMatrix()
		*m_pTransformCom->Get_WorldMatrix()* m_pParentTransformCom->Get_WorldMatrix();
	_matrix Missile9Matrix = m_pMissileNode9->Get_CombinedTransformation()
		* m_pModelCom->Get_PivotMatrix()
		*m_pTransformCom->Get_WorldMatrix()* m_pParentTransformCom->Get_WorldMatrix();

	CBossMissile::BOSSMISSILEINFO _tInfo;
	ZeroMemory(&_tInfo, sizeof(CBossMissile::BOSSMISSILEINFO));

	if(!m_bPour)
		_tInfo.iBulletType = 0;
	else
		_tInfo.iBulletType = 3;

	_tInfo.m_fSide = 1.f;
	_tInfo.iHitType = TYPE_HIT;
	XMStoreFloat4(&_tInfo.vDir, Missile1Matrix.r[2]);
	XMStoreFloat4(&_tInfo.vPosition, Missile1Matrix.r[3]);
	GI->Add_GameObjectToLayer(TEXT("BossMissile"), TM->Get_CurLevel(), TEXT("Layer_Monster"), &_tInfo);



	_matrix a = Missile2Matrix;
	a.r[0] = XMVector3Normalize(a.r[0]);
	a.r[1] = XMVector3Normalize(a.r[1]);
	a.r[2] = XMVector3Normalize(a.r[2]);
	_float4x4 Matrix;
	XMStoreFloat4x4(&Matrix, a);
	PTM->Create_InstanceParticle(L"OctoPusLight", Matrix);


	XMStoreFloat4(&_tInfo.vDir, Missile2Matrix.r[2]);

	XMStoreFloat4(&_tInfo.vPosition, Missile2Matrix.r[3]);
	GI->Add_GameObjectToLayer(TEXT("BossMissile"), TM->Get_CurLevel(), TEXT("Layer_Monster"), &_tInfo);


	//XMStoreFloat4(&_tInfo.vDir, Missile3Matrix.r[2]);
	//XMStoreFloat4(&_tInfo.vPosition, Missile3Matrix.r[3]);
	//GI->Add_GameObjectToLayer(TEXT("BossMissile"), LEVEL_TEST, TEXT("Layer_Monster"), &_tInfo);

	a = Missile3Matrix;
	a.r[0] = XMVector3Normalize(a.r[0]);
	a.r[1] = XMVector3Normalize(a.r[1]);
	a.r[2] = XMVector3Normalize(a.r[2]);
	XMStoreFloat4x4(&Matrix, a);
	PTM->Create_InstanceParticle(L"OctoPusLight", Matrix);
	XMStoreFloat4(&_tInfo.vDir, Missile7Matrix.r[2]);
	XMStoreFloat4(&_tInfo.vPosition, Missile7Matrix.r[3]);
	GI->Add_GameObjectToLayer(TEXT("BossMissile"), LEVEL_TEST, TEXT("Layer_Monster"), &_tInfo);
	a = Missile7Matrix;
	a.r[0] = XMVector3Normalize(a.r[0]);
	a.r[1] = XMVector3Normalize(a.r[1]);
	a.r[2] = XMVector3Normalize(a.r[2]);
	XMStoreFloat4x4(&Matrix, a);
	PTM->Create_InstanceParticle(L"OctoPusLight", Matrix);
	XMStoreFloat4(&_tInfo.vDir, Missile8Matrix.r[2]);
	XMStoreFloat4(&_tInfo.vPosition, Missile8Matrix.r[3]);
	GI->Add_GameObjectToLayer(TEXT("BossMissile"), TM->Get_CurLevel(), TEXT("Layer_Monster"), &_tInfo);

	//XMStoreFloat4(&_tInfo.vDir, Missile9Matrix.r[2]);
	//XMStoreFloat4(&_tInfo.vPosition, Missile9Matrix.r[3]);
	//GI->Add_GameObjectToLayer(TEXT("BossMissile"), LEVEL_TEST, TEXT("Layer_Monster"), &_tInfo);

	a = Missile9Matrix;
	a.r[0] = XMVector3Normalize(a.r[0]);
	a.r[1] = XMVector3Normalize(a.r[1]);
	a.r[2] = XMVector3Normalize(a.r[2]);
	XMStoreFloat4x4(&Matrix, a);
	PTM->Create_InstanceParticle(L"OctoPusLight", Matrix);

	Missile_Sound();
}

void CMechanicalOctopusLeg::Launch_Missile2()
{
	_matrix Missile4Matrix = m_pMissileNode4->Get_CombinedTransformation()
		* m_pModelCom->Get_PivotMatrix()
		*m_pTransformCom->Get_WorldMatrix()* m_pParentTransformCom->Get_WorldMatrix();
	_matrix Missile5Matrix = m_pMissileNode5->Get_CombinedTransformation()
		* m_pModelCom->Get_PivotMatrix()
		*m_pTransformCom->Get_WorldMatrix()* m_pParentTransformCom->Get_WorldMatrix();

	_matrix Missile10Matrix = m_pMissileNode10->Get_CombinedTransformation()
		* m_pModelCom->Get_PivotMatrix()
		*m_pTransformCom->Get_WorldMatrix()* m_pParentTransformCom->Get_WorldMatrix();
	_matrix Missile11Matrix = m_pMissileNode11->Get_CombinedTransformation()
		* m_pModelCom->Get_PivotMatrix()
		*m_pTransformCom->Get_WorldMatrix()* m_pParentTransformCom->Get_WorldMatrix();

	CBossMissile::BOSSMISSILEINFO _tInfo;
	ZeroMemory(&_tInfo, sizeof(CBossMissile::BOSSMISSILEINFO));

	if (!m_bPour)
		_tInfo.iBulletType = 1;
	else
		_tInfo.iBulletType = 3;

	_tInfo.m_fSide = 1.f;
	_tInfo.iHitType = TYPE_HIT;



	XMStoreFloat4(&_tInfo.vDir, Missile4Matrix.r[2]);
	XMStoreFloat4(&_tInfo.vPosition, Missile4Matrix.r[3]);
	GI->Add_GameObjectToLayer(TEXT("BossMissile"), TM->Get_CurLevel(), TEXT("Layer_Monster"), &_tInfo);

	//XMStoreFloat4(&_tInfo.vDir, Missile5Matrix.r[2]);
	//XMStoreFloat4(&_tInfo.vPosition, Missile5Matrix.r[3]);
	//GI->Add_GameObjectToLayer(TEXT("BossMissile"), LEVEL_TEST, TEXT("Layer_Monster"), &_tInfo);

	_tInfo.m_fSide = -1.f;
	XMStoreFloat4(&_tInfo.vDir, Missile10Matrix.r[2]);
	XMStoreFloat4(&_tInfo.vPosition, Missile10Matrix.r[3]);
	GI->Add_GameObjectToLayer(TEXT("BossMissile"), TM->Get_CurLevel(), TEXT("Layer_Monster"), &_tInfo);

	//XMStoreFloat4(&_tInfo.vDir, Missile11Matrix.r[2]);
	//XMStoreFloat4(&_tInfo.vPosition, Missile11Matrix.r[3]);
	//GI->Add_GameObjectToLayer(TEXT("BossMissile"), LEVEL_TEST, TEXT("Layer_Monster"), &_tInfo);

	Missile_Sound();


	_matrix a = Missile11Matrix;
	a.r[0] = XMVector3Normalize(a.r[0]);
	a.r[1] = XMVector3Normalize(a.r[1]);
	a.r[2] = XMVector3Normalize(a.r[2]);
	_float4x4 Matrix;
	XMStoreFloat4x4(&Matrix, a);
	PTM->Create_InstanceParticle(L"OctoPusLight", Matrix);

	a = Missile4Matrix;
	a.r[0] = XMVector3Normalize(a.r[0]);
	a.r[1] = XMVector3Normalize(a.r[1]);
	a.r[2] = XMVector3Normalize(a.r[2]);
	XMStoreFloat4x4(&Matrix, a);
	PTM->Create_InstanceParticle(L"OctoPusLight", Matrix);
}

void CMechanicalOctopusLeg::Launch_Missile3()
{
	_matrix Missile6Matrix = m_pMissileNode6->Get_CombinedTransformation()
		* m_pModelCom->Get_PivotMatrix()
		*m_pTransformCom->Get_WorldMatrix()* m_pParentTransformCom->Get_WorldMatrix();

	_matrix Missile12Matrix = m_pMissileNode12->Get_CombinedTransformation()
		* m_pModelCom->Get_PivotMatrix()
		*m_pTransformCom->Get_WorldMatrix()* m_pParentTransformCom->Get_WorldMatrix();

	CBossMissile::BOSSMISSILEINFO _tInfo;
	ZeroMemory(&_tInfo, sizeof(CBossMissile::BOSSMISSILEINFO));

	if (!m_bPour)
		_tInfo.iBulletType = 2;
	else
		_tInfo.iBulletType = 3;

	_tInfo.iHitType = TYPE_SPIN;
	_tInfo.m_fSide = 1.f;

	XMStoreFloat4(&_tInfo.vDir, Missile6Matrix.r[2]);
	XMStoreFloat4(&_tInfo.vPosition, Missile6Matrix.r[3]);
	GI->Add_GameObjectToLayer(TEXT("BossMissile"), TM->Get_CurLevel(), TEXT("Layer_Monster"), &_tInfo);

	XMStoreFloat4(&_tInfo.vDir, Missile12Matrix.r[2]);
	XMStoreFloat4(&_tInfo.vPosition, Missile12Matrix.r[3]);
	GI->Add_GameObjectToLayer(TEXT("BossMissile"), TM->Get_CurLevel(), TEXT("Layer_Monster"), &_tInfo);

	Missile_Sound();
}

void CMechanicalOctopusLeg::Attack1(_float fTimeDelta)
{

	if (!m_bOneCheck)
	{
		m_bOneCheck = true;
		m_pModelCom->Set_TickPerSecond(24.f);
		m_pModelCom->Set_AnimIndex(ATTACK1_1);
		m_eCurState = ATTACK1_1;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
		GI->PlaySoundW(L"se_EN0103_hatch1.wav", SD_OCTOPUSLEG, 0.5f);
	}


}

void CMechanicalOctopusLeg::Attack2(_float fTimeDelta)
{
	if (!m_bOneCheck)
	{
		m_bOneCheck = true;
		m_pModelCom->Set_TickPerSecond(24.f);
		m_pModelCom->Set_AnimIndex(ATTACK2_1);
		m_eCurState = ATTACK2_1;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
		GI->PlaySoundW(L"se_EN0103_hatch1.wav", SD_OCTOPUSLEG, 0.5f);
	}
}

void CMechanicalOctopusLeg::Attack3(_float fTimeDelta)
{
	m_fDistance = TargetDistance(XMLoadFloat4(&m_fPlayerPos)); //타겟과 나의 거리 계속 계산

	if (m_fDistance > 15.f)
	{
		if (!m_bOneCheck)
		{
			m_bOneCheck = true;
			m_pModelCom->Set_TickPerSecond(24.f);
			m_pModelCom->Set_AnimIndex(ATTACK2_1);
			m_eCurState = ATTACK2_1;
			m_eNextState = m_eCurState;
			m_pModelCom->SetNextIndex(m_eNextState);
			GI->PlaySoundW(L"se_EN0103_hatch1.wav", SD_OCTOPUSLEG, 0.5f);
		}
	}
	else
	{
		if (!m_bOneCheck)
		{
			m_bOneCheck = true;
			m_pModelCom->Set_TickPerSecond(24.f);
			m_pModelCom->Set_AnimIndex(ATTACK3);
			m_eCurState = ATTACK3;
			m_eNextState = m_eCurState;
			m_pModelCom->SetNextIndex(m_eNextState);
			m_bShootBullet3 = false;
		}
	}
}
void CMechanicalOctopusLeg::Attack1_1Update(_float fTimeDelta)
{
	if (!m_bMove)
		m_pModelCom->Set_TickPerSecond(0.f);

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(0) && !m_bShootMissile1_1)
	{
		m_bShootMissile1_1 = true;
		Launch_Missile1();

		if (m_bPour)
			Launch_Missile2();
	}


}

void CMechanicalOctopusLeg::Attack1_2Update(_float fTimeDelta)
{
	if (!m_bShootMissile1_2)
	{
		m_bShootMissile1_2 = true;

		//미사일 12발 생성
		Launch_Missile2();
		Launch_Missile3();

		if (m_bPour)
		{
			Launch_Missile1();
		}
	}
}

void CMechanicalOctopusLeg::Attack1_3Update(_float fTimeDelta)
{
	if (m_bPlus && !m_bShootMissile1_3)
	{
		m_bShootMissile1_3 = true;
		//Launch_Missile1();
	}



	if (m_bPour && !m_bShootMissile1_4)
	{
		m_bShootMissile1_4 = true;

		Launch_Missile1();
		Launch_Missile2();
	}
}

void CMechanicalOctopusLeg::Attack2_1Update(_float fTimeDelta)
{
	if (!m_bMove)
		m_pModelCom->Set_TickPerSecond(0.f);

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(0) && !m_bShootMissile2_1)
	{
		m_bShootMissile2_1 = true;
		Launch_Missile1();
	}


}

void CMechanicalOctopusLeg::Attack2_2Update(_float fTimeDelta)
{

	if (!m_bShootBullet2)
	{
		m_bShootBullet2 = true;

		CBossBomb::BOMBINFO BombInfo;
		BombInfo.fJumpPower = 3.f;
		BombInfo.iAttackType = 2;
		BombInfo.fSpeed = 2.f;
		BombInfo.fGravity = 60.f;
		BombInfo.vColor = { 0.933f,0.063f,0.090f, 1.f };
		BombInfo.vColor2 = { 0.f, 0.f, 0.f, 1.f };
		BombInfo.vRatio = { 1.f, 1.f };
		BombInfo.fBlurPower = 1.5f;

		BombInfo.iNaviNum = m_pNavigationCom->Get_CurCellIndex();
		_matrix Bombmatrix;

		Bombmatrix.r[0] = { 1.7f, 0.f, 0.f, 0.f };
		Bombmatrix.r[1] = { 0.f, 0.f, -1.9f, 0.f };
		Bombmatrix.r[2] = { 0.f, 1.7f, 0.f, 0.f };
		Bombmatrix.r[3] = { -0.66f, 0.99f, 0.f, 1.f };
		XMStoreFloat4x4(&BombInfo.vWorldMatrix, Bombmatrix * m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix());
		(GI->Add_GameObjectToLayer(TEXT("BossBomb"), GI->Get_Cur_LevelIndex(), TEXT("Layer_Monster"), &BombInfo));
		
		Launch_Missile2();
		Launch_Missile3();
	}


}

void CMechanicalOctopusLeg::Attack2_3Update(_float fTimeDelta)
{
}

void CMechanicalOctopusLeg::Attack3_Update(_float fTimeDelta)
{
	if (!m_bShootBullet3)
	{
		m_bShootBullet3 = true;
		GI->PlaySoundW(L"se_EN0103_hatch2.wav", SD_OCTOPUSLEG, 0.5f);

		CBossBomb::BOMBINFO BombInfo;
		BombInfo.fJumpPower = 3.f;
		BombInfo.iAttackType = 3;
		BombInfo.fSpeed = 3.f;
		BombInfo.fGravity = 45.f;
		BombInfo.vColor = { 0.933f,0.063f,0.090f, 1.f };
		BombInfo.vColor2 = { 0.f, 0.f, 0.f, 1.f };
		BombInfo.vRatio = { 1.f, 1.f };
		BombInfo.fBlurPower = 1.5f;
		BombInfo.iNaviNum = m_pNavigationCom->Get_CurCellIndex();
		_matrix Bombmatrix;

		Bombmatrix.r[0] = { 1.7f, 0.f, 0.f, 0.f };
		Bombmatrix.r[1] = { 0.f, 0.f, -1.9f, 0.f };
		Bombmatrix.r[2] = { 0.f, 1.7f, 0.f, 0.f };
		Bombmatrix.r[3] = { -0.66f, 0.99f, 0.f, 1.f };
		XMStoreFloat4x4(&BombInfo.vWorldMatrix, Bombmatrix * m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix());
		(GI->Add_GameObjectToLayer(TEXT("BossBomb"), GI->Get_Cur_LevelIndex(), TEXT("Layer_Monster"), &BombInfo));
		
		BombInfo.fSpeed = 2.f;
		Bombmatrix.r[0] = { 1.7f, 0.f, 0.f, 0.f };
		Bombmatrix.r[1] = { 0.f, 1.343f, -1.343f, 0.f };
		Bombmatrix.r[2] = { 0.f, 1.202f, 1.202f, 0.f };
		Bombmatrix.r[3] = { -0.66f, 0.99f, 0.f, 1.f };
		XMStoreFloat4x4(&BombInfo.vWorldMatrix, Bombmatrix * m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix());
		(GI->Add_GameObjectToLayer(TEXT("BossBomb"), GI->Get_Cur_LevelIndex(), TEXT("Layer_Monster"), &BombInfo));

		Bombmatrix.r[0] = { 1.2f, 0.f, 0.f, 0.f };
		Bombmatrix.r[1] = { 0.f, -1.343f, -1.343f, 0.f };
		Bombmatrix.r[2] = { 0.f,1.202f, -1.202f, 0.f };
		Bombmatrix.r[3] = { -0.66f, 0.99f, 0.f, 1.f };
		XMStoreFloat4x4(&BombInfo.vWorldMatrix, Bombmatrix * m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix());
		(GI->Add_GameObjectToLayer(TEXT("BossBomb"), GI->Get_Cur_LevelIndex(), TEXT("Layer_Monster"), &BombInfo));

	}

}

_float CMechanicalOctopusLeg::TargetDistance(_fvector vTargetPos)
{

	_vector vPos =  XMLoadFloat4(&Get_WorldPos());

	_vector vLook = vPos - vTargetPos;

	_vector vDistance = XMVector3Length(vLook);


	return XMVectorGetX(vDistance);

}

void CMechanicalOctopusLeg::LookAtPlayer(_float fLiearTime)
{
	Set_Dir();
	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&m_vTargetLook), fLiearTime);

}


HRESULT CMechanicalOctopusLeg::Ready_Components()
{
	//m_pEmissiveCom
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Oct_Emi"), TEXT("Com_TextureEmi"), (CComponent**)&m_pEmissiveCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("MechanicalOctopusLeg"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_ParentTransform */

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_ParentTransform"), (CComponent**)&m_pParentTransformCom)))
		return E_FAIL;

	m_pModelCom->Set_RootHierarchy(3);

	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	m_pCoreNode = m_pModelCom->Get_HierarchyNode("Cover_07_jo");
	if (nullptr == m_pCoreNode)
		return E_FAIL;

	ColDesc.vSize = _float3(1.5f, 1.5f, 1.5f);
	ColDesc.vCenter = _float3(0.f, 0.4f, -0.2f);
	XMStoreFloat4x4(&ColDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	ColDesc.pSocket = m_pCoreNode;
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Body1"); 

	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMechanicalOctopusLeg::Create_Navigation(_uint iNaviNum)
{
	CNavigation::NAVIGATIONDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = 0;

	switch (iNaviNum)
	{
	case 1:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage1Navigation"), TEXT("Stage1"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
		break;
	case 2:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage2Navigation"), TEXT("Stage2"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
		break;
	case 3:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage3Navigation"), TEXT("Stage3"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
		break;

	case 4:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage4Navigation"), TEXT("Stage4"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
	}

	_int iNaviIndex = m_pNavigationCom->Find_NaviIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pNavigationCom->Set_NaviIndex(iNaviIndex);

	return S_OK;
}

void CMechanicalOctopusLeg::Ready_HierarchyNode()
{
	m_pMissileNode1 = m_pModelCom->Get_HierarchyNode("Cover_01_jo");
	m_pMissileNode2 = m_pModelCom->Get_HierarchyNode("Cover_02_jo");
	m_pMissileNode3 = m_pModelCom->Get_HierarchyNode("Cover_03_jo");
	m_pMissileNode4 = m_pModelCom->Get_HierarchyNode("Cover_04_jo");
	m_pMissileNode5 = m_pModelCom->Get_HierarchyNode("Cover_05_jo");
	m_pMissileNode6 = m_pModelCom->Get_HierarchyNode("Cover_06_jo");
	m_pMissileNode7 = m_pModelCom->Get_HierarchyNode("Cover_08_jo");
	m_pMissileNode8 = m_pModelCom->Get_HierarchyNode("Cover_09_jo");
	m_pMissileNode9 = m_pModelCom->Get_HierarchyNode("Cover_10_jo");
	m_pMissileNode10 = m_pModelCom->Get_HierarchyNode("Cover_11_jo");
	m_pMissileNode11 = m_pModelCom->Get_HierarchyNode("Cover_12_jo");
	m_pMissileNode12 = m_pModelCom->Get_HierarchyNode("Cover_13_jo");

}


HRESULT CMechanicalOctopusLeg::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Monster_HP"), TM->Get_CurLevel(), pLayerTag, this)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Monster_HP_Bar"), TM->Get_CurLevel(), pLayerTag, this)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CMechanicalOctopusLeg::Set_PreBar(_float fTimeDelta)
{
	if (m_fPreHp > m_fNowHp)
	{
		m_fMinusHp = (m_fPreHp - m_fNowHp) / m_fMaxHp;
		m_fPreHp -= m_fMinusHp * 10000.f * fTimeDelta;
		if (m_fPreHp <= m_fNowHp)
		{
			m_fPreHp = m_fNowHp;
		}
	}
}

CMechanicalOctopusLeg * CMechanicalOctopusLeg::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMechanicalOctopusLeg*		pInstance = new CMechanicalOctopusLeg(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMechanicalOctopusLeg"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMechanicalOctopusLeg::Clone(void * pArg)
{
	CMechanicalOctopusLeg*		pInstance = new CMechanicalOctopusLeg(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMechanicalOctopusLeg"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMechanicalOctopusLeg::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pDissolveTexture);
	Safe_Release(m_pParentTransformCom);
	Safe_Release(m_pEmissiveCom);
}
