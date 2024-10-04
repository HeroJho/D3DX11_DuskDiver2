#include "stdafx.h"
#include "..\Public\OldBombRobot.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "ToolManager.h"
#include "Monster_Manager.h"
#include "GameInstance.h"
#include "MonsterMissile.h"
#include "Time_Manager.h"
#include "UI.h"
#include "DropItem.h"
#include "Particle_Manager.h"
#include "Particle_Manager.h"
#include "Player01HitEffect_1.h"
#include "Player01HitEffect_2.h"
#include "Player01SmokeBall.h"
#include "Camera_Manager.h"

COldBombRobot::COldBombRobot(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

COldBombRobot::COldBombRobot(const COldBombRobot & rhs)
	: CCreture(rhs)
{
}

HRESULT COldBombRobot::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT COldBombRobot::Initialize(void * pArg)
{

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;


	CCreture::CREATEINFO CreateInfo = *(CCreture::CREATEINFO*)pArg;
	_vector vPosition = XMLoadFloat4(&CreateInfo.vPosition);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	m_iIndex = CreateInfo.iIndex;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Create_Navigation(CreateInfo.iNaviNum)))
		return E_FAIL;
	


	m_sTag = "Tag_Monster";
	// *[HeroJho]: ImGui로 디버깅을 하려면 꼭 호출해줘야 합니다.
	Make_ImGui();

	m_eCurState = SPAWN;
	m_pModelCom->Set_AnimIndex(SPAWN);

	m_vTargetLook = { 0.f,0.f,1.f };
	m_fPlayerPos = { 0.f,0.f,0.f, 1.f };
	m_vPlayerLook = { 0.f,0.f,1.f };

	m_fHurtTime = 0.02f;


	switch (m_iIndex)
	{
	case 2:
		m_fMaxHp = 800.f;
		m_fStamina = 1.f;
		break;
	case 3:
		m_fMaxHp = 3000.f;
		m_fStamina = 1.f;
		m_fDelayTime = 2.4f;
		m_fPlusDamage = 0.f;
		break;
	case 4:
		m_fMaxHp = 3000.f;
		m_fPlusDamage = 0.f;
		m_fStamina = 2.f;
		m_fDelayTime = 1.8f;
		break;
	case 7:
		m_fMaxHp = 4000.f;
		m_fPlusDamage = 60.f;
		m_fStamina = 1.f;
		m_fDelayTime = 1.5f;
		break;
	}

	m_fNowHp = m_fMaxHp;
	m_fDamage = 40.f + m_fPlusDamage;

	m_fPreHp = m_fNowHp;
	m_fMaxPg = m_fMaxHp * 0.4f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Dissolve"), TEXT("Com_Texture"), (CComponent**)&m_pDissolveTexture)))
		return E_FAIL;


	MM->Add_Monster(this);
	MM->Add_OldBombRobot(this);

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;
		


	return S_OK;
}

void COldBombRobot::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_MONSTER, this);
	Delay_Spawn(fTimeDelta);
	Check_Hurt(fTimeDelta);
	Check_Rim(fTimeDelta);
	fTimeDelta *= m_fTMRatio;
	Emp_Col();
	Distance_BattlePattern(fTimeDelta);
	Update(fTimeDelta);
	Check_Death(fTimeDelta);
	Set_PreBar(fTimeDelta);

}

void COldBombRobot::LateTick(_float fTimeDelta)
{
	if (m_bCreate)
	{
		if (nullptr == m_pRendererCom)
			return;
		fTimeDelta *= m_fTMRatio;

		m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom, m_pNavigationCom);

		m_pTransformCom->Tick_Gravity(fTimeDelta, m_pNavigationCom, 30.f);

		End_Animation();

		Tick_Col(m_pTransformCom->Get_WorldMatrix());

		if(m_bAttackCol)
			Choose_Col("Monster_Attack");
		if (m_bHurt)
			Choose_Col("Monster_Body1");
			
		Choose_Col("Monster_Push_Range");

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

		Render_Col(m_pRendererCom);


	}
	
}

HRESULT COldBombRobot::Render()
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


	_float fGama = 1.6f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;

	//// For.Rim
	if (FAILED(m_pShaderCom->Set_RawValue("g_CamPosition", &GI->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	_float g_RimPower = -0.5f;
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
void COldBombRobot::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);

	if (m_bCheckFront) //디버깅용
	{
		m_fTest = 1.f;
	}
	else
		m_fTest = -1.f;

	ImGui::Text("FrontCheck : %f", m_fTest);
	ImGui::Text("distance : %f", m_fDistance);

	ImGui::DragFloat("BlurPower ", &m_fBlurPower, 0.01f);
	ImGui::DragFloat("RimPower ", &m_fRimPower, 0.01f);

	_float3 vSize = Get_Colliders("Monster_Attack")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Monster_Attack")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Get_Colliders("Monster_Attack")->Set_Size(vSize);
	Get_Colliders("Monster_Attack")->Set_Center(vCenter);


	ImGui::End();
}

// *[HeroJho]: 충돌이 발생할 때 호출되는 함수입니다.
void COldBombRobot::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	// *[HeroJho]:
	// Desc.pOther		- 충돌한 상대편 객체의 주소입니다.
	// Desc.pMyDesc		- 상대편과 충돌한 나의 콜라이더 정보입니다.
	// Desc.pOtherDesc	- 나와 충돌한 상대편의 콜라이더 정보입니다.
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		// 내 손 Sphere 둘중 하나와 상대편의 OBB와 충돌 했다면
		if (strcmp("Monster_Body1", Desc.pMyDesc->sTag) &&
			strcmp("Monster_Push_Range", Desc.pMyDesc->sTag) && !strcmp("Player_Body", Desc.pOtherDesc->sTag))
		{
			((CCreture*)Desc.pOther)->On_Hit(this);
		}
		
		if (!strcmp("Monster_Push_Range", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Push_Range", Desc.pOtherDesc->sTag))
		{
			m_pTransformCom->PushMeXZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION), Desc.pMyDesc->vSize.x, XMLoadFloat4(&((CCreture*)Desc.pOther)->Get_WorldPos()), Desc.pOtherDesc->vSize.x, TM->Get_TimeDelta(), m_pNavigationCom);
		}
	}
	else if ("Tag_Monster" == Desc.pOther->Get_Tag())
	{
			if (!strcmp("Monster_Push_Range", Desc.pMyDesc->sTag) &&
				!strcmp("Monster_Push_Range", Desc.pOtherDesc->sTag))
			{
				//밀어내기
				if(m_bPush)
					m_pTransformCom->PushOtherXZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION), Desc.pMyDesc->vSize.x, XMLoadFloat4(&((CCreture*)Desc.pOther)->Get_WorldPos()), Desc.pOtherDesc->vSize.x, TM->Get_TimeDelta(), m_pNavigationCom);

			}
	
	}
	else if ("Tag_Boss" == Desc.pOther->Get_Tag())
	{
		if (!strcmp("Monster_Push_Range", Desc.pMyDesc->sTag) &&
			!strcmp("Monster_Push_Range", Desc.pOtherDesc->sTag))
		{
			//밀어내기
			m_pTransformCom->PushMeXZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION), Desc.pMyDesc->vSize.x, XMLoadFloat4(&((CCreture*)Desc.pOther)->Get_WorldPos()), Desc.pOtherDesc->vSize.x, TM->Get_TimeDelta(), m_pNavigationCom);
		}
	}
	
}

void COldBombRobot::On_Hit(CCreture* pOther)
{
	//hit이 한번만 처리되게끔
	if (m_bHurt && m_bHitAble)
	{
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		vPos.y += 1.f;
		pOther->Create_HitEffect(vPos);

		GI->PlaySoundW(L"SwordHit.ogg", SD_HIT, 0.4f);
		m_bRim = true;
		m_fRimAcc = 0.f;

		m_bHurt = false;
		m_bAction = false;
		m_bAttackCol = false;
		m_bDontWalk = false;
		m_bEscape = false;

		m_fNowHp -= (pOther->Get_Damage() - m_fStamina);

		if (MM->Get_MonsterCount() == 1 && m_fNowHp <= 0.f)
		{
			TIM->Fix_All(0.14f, 5.f);
			CRM->Get_Cam()->Fov(-3.f, 0.15f, 3.f);
			if (!m_bFightEnd)
			{
				GI->PlaySoundW(L"slowmotion_finish.wav", SD_UI, 0.6f);
				m_bFightEnd = true;
			}
		}

		if (!m_bReGauge && !PM->Get_PatternSkill())
			m_fPatternGauge += (pOther->Get_Damage() * 0.7f);

		if (m_fNowHp <= 0.f)
		{
			m_bHurt = false;
			m_bSpawn = false;
		}

		Hit_State(pOther);

		m_bSameAttack = true;

		Check_Avoid();

		if (pOther->Get_Damage() != 0.f)
		{
			UM->Plus_Combo();
			UM->Set_Hit(true);
			UM->Set_Up(true);
		}
	}

}

void COldBombRobot::Create_HitEffect(_float4 vWorldPos)
{
	_float4 vPos;
	XMStoreFloat4(&vPos, XMLoadFloat4(&vWorldPos) +
		XMVector3Normalize((XMLoadFloat4(&GI->Get_CamPosition()) - XMLoadFloat4(&vWorldPos) * 1.f)));
	XMStoreFloat4(&vPos, XMLoadFloat4(&vPos) + _vector{ GI->Get_FloatRandom(-0.5f,0.5f),
		GI->Get_FloatRandom(-0.5f,0.5f) ,  GI->Get_FloatRandom(-0.5f,0.5f) });

	CPlayer01HitEffect_2::HITEFFECT2INFO* HitEffect2 = new CPlayer01HitEffect_2::HITEFFECT2INFO;
	HitEffect2->vScale = { 3.f,3.f,3.f };
	HitEffect2->vPosition = vPos;
	HitEffect2->vRatio = { 3.f,2.f,20.f };
	HitEffect2->vColor = { 0.f,0.f,0.f,1.f };
	HitEffect2->vColor2 = { 0.f,0.f,0.f,1.f };
	GI->Add_GameObjectToLayer(L"Player01HitEffect_2", GI->Get_Cur_LevelIndex(), L"Layer_PlayerEffect", &HitEffect2);

	CPlayer01HitEffect_1::HITEFFECT1INFO* HitEffect = new CPlayer01HitEffect_1::HITEFFECT1INFO;
	HitEffect->vScale = { 2.f,2.f,2.f };
	HitEffect->vPosition = vPos;
	HitEffect->vRatio = { 3.f,2.f,15.f };
	HitEffect->vColor = { 1.f,1.f,0.9f,1.f };
	HitEffect->vColor2 = { 1.f,0.f,0.f,1.f };
	GI->Add_GameObjectToLayer(L"Player01HitEffect_1", GI->Get_Cur_LevelIndex(), L"Layer_PlayerEffect", &HitEffect);



	_matrix Matrix = XMMatrixIdentity();
	Matrix.r[3] = XMLoadFloat4(&vPos);
	_float4x4 Matrix2;
	XMStoreFloat4x4(&Matrix2, Matrix);
	PTM->Create_InstanceParticle(L"Player01Hit1", Matrix2);
	PTM->Create_InstanceParticle(L"Player01Hit1_2", Matrix2);
}

void COldBombRobot::Control_OldBombRobot()
{
	m_bMove = true;
	m_bSpawn = true;
	m_fAttackAcc = 0.f;
	m_bAttack = false;
	m_bOneCheck = false;
	m_bHitAble = true;
	m_bAction = true;
}

void COldBombRobot::Delay_Spawn(_float fTimeDelta)
{
	if (!m_bCreate)
	{
		m_fCreateAcc += 1.f * fTimeDelta;

		if (m_fCreateAcc >= (m_fDelayTime - 0.3f) && !m_bSpawnEffect)
		{
			m_bSpawnEffect = true;
			GI->PlaySoundW(L"se_enemy_spawn1.wav", SD_TRAININGBOT2, 0.5f);
			PTM->Create_InstanceParticle(L"MonsterAppear", m_pTransformCom->Get_WorldFloat4x4());
			PTM->Create_InstanceParticle(L"MonsterAppear_2", m_pTransformCom->Get_WorldFloat4x4());
		}

		if (m_fCreateAcc >= m_fDelayTime)
			m_bCreate = true;
	}
}

void COldBombRobot::Set_State(STATE eState) //end 애니메이션에서
{
	//상태 보간 하는 함수
	if (m_eNextState == eState)
		return;

	m_eNextState = eState;

	switch (m_eNextState)
	{
		//다음 상태로 넘어갈때 해줘야하는것들
		//애니메이션x
	case Client::COldBombRobot::ATTACK1:
		m_bShootBullet1 = false;
		m_bShootBullet2 = false;
		m_bShootBullet3 = false;
		m_bShootBullet4 = false;
		m_bShootBullet5 = false;
		m_bShootBullet6 = false;
		m_bAttack1Sound = false;
		LookAtPlayer();
		break;
	case Client::COldBombRobot::ATTACK2:
		m_bShootMissile1 = false;
		m_bAttack2Effect = false;
		m_bOnePatternEffect = false;
		m_bAttack2Sound = false;
		LookAtPlayer();
		break;
	case Client::COldBombRobot::ATTACK3:
		GI->PlaySoundW(L"se_EN0004_attack3_2.wav", SD_TRAININGBOT1, 0.4f);
		m_eHitType = TYPE_HIT;
		m_fPushPower = 4.f;
		m_fAirPower = 0.f;
		m_bAttack3Col_1 = false;
		m_bAttack3Col_2 = false;
		m_bAttack3Col_3 = false;
		m_bAttack3Col_4 = false;
		m_bAttack3Col_5 = false;
		m_bAttack3Col_6 = false;
		LookAtPlayer();
		break;
	case Client::COldBombRobot::BOUNCE:
		m_bFly = true;
		break;
	case Client::COldBombRobot::HITBACK:
		m_bFly = false;
		break;
	case Client::COldBombRobot::HITFLY:
		m_bFly = true;
		if (m_fHAirPower == 0.f)
			m_pTransformCom->ResetAndJump(1.f);
		else
			m_pTransformCom->ResetAndJump(m_fHAirPower);
		break;
	case Client::COldBombRobot::HITFRONT:
		m_bFly = false;
		break;
	case Client::COldBombRobot::HITDOWN:
		m_bFly = true;
		break;
	case Client::COldBombRobot::HITUP:
		m_bFly = true;
		Reset_Collider();
		break;
	case Client::COldBombRobot::HITSPIN:
		m_bFly = true;
		Reset_Collider();
		break;
	case Client::COldBombRobot::HITHORI:
		HitAwayPower_Cehck();
		m_bFly = true;
		Reset_Collider();
		break;
	case Client::COldBombRobot::HITSLAPDOWN:
		m_bFly = true;
		Reset_Collider();
		break;
	case Client::COldBombRobot::DOWN:
		m_bFly = true;
		break;
	case Client::COldBombRobot::GETUP:
		m_bFly = false;
		break;
	case Client::COldBombRobot::SPAWN:
		break;
	case Client::COldBombRobot::IDLE:
		if (m_iIndex == 3 || m_iIndex == 6 || m_iIndex == 7)
			Control_OldBombRobot();
		else
		{
			if (m_bMove)
			{
				m_bHurt = true;
				m_bAction = true;
				m_bSpawn = true;
			}
		}
		break;
	case Client::COldBombRobot::STUN:
		if (m_bDeadStart)
		{
			Create_DeadEffect();
			Set_Dead();
			GI->PlaySoundW(L"se_EN0005_attack2_3.wav", SD_TRAININGBOT1, 0.6f);

		}
		break;
	case Client::COldBombRobot::STUNRECOVER:
		break;
	case Client::COldBombRobot::STRAFELEFT:
		m_bStrafeSound1 = false;
		m_bStrafeSound2 = false;
		break;
	case Client::COldBombRobot::STRAFERIGHT:
		m_bStrafeSound1 = false;
		m_bStrafeSound2 = false;
		break;
	case Client::COldBombRobot::RUN:
		m_bRunSound1 = false;
		m_bRunSound2 = false;
		break;
	case Client::COldBombRobot::WALKBACK:
		m_bHurt = true;
		m_bHitAble = true;
		m_bBackWalkSound1 = false;
		m_bBackWalkSound2 = false;
		break;
	case Client::COldBombRobot::WALKFRONT:
		m_bWalkSound1 = false;
		m_bWalkSound2 = false;
		break;

	}


	m_pModelCom->SetNextIndex(m_eNextState);
	m_pModelCom->SetChangeBool(true); 
	m_eCurState = m_eNextState; //보간시작할때 상태를 바꿔줌

}

void COldBombRobot::Hit_State(CCreture * pOther)
{
	m_bDontWalk = true;

	m_fHAirPower = pOther->Get_AirPower();
	m_fHPushPower = pOther->Get_PushPower();

	if (pOther->Get_HitType() == TYPE_HIT)
	{
		if (m_eCurState == DOWN || m_bFly)
		{
			switch (pOther->Get_PushType())
			{
			case PUSH:
				m_vPlayerLook = (pOther)->Get_Dir(_float3(0.f, 0.f, 1.f));
				break;

			case PULL:
				XMStoreFloat3(&m_vPlayerLook, XMLoadFloat4(&pOther->Get_WorldPos()) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));
				break;

			case TARGET:
				m_vPlayerLook = pOther->Get_PushLook();
				break;

			}
			XMStoreFloat3(&m_vPlayerLook, XMVector3Normalize(XMLoadFloat3(&m_vPlayerLook)));
			Set_State(HITFLY);
		}
		else
		{
			if (m_pTransformCom->Check_TargetFront(XMLoadFloat4(&pOther->Get_WorldPos())))
			{
			
				switch (pOther->Get_PushType())
				{
				case PUSH:
					m_vPlayerLook = (pOther)->Get_Dir(_float3(0.f, 0.f, 1.f));
					break;

				case PULL:
					XMStoreFloat3(&m_vPlayerLook, XMLoadFloat4(&pOther->Get_WorldPos()) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));
					break;

				case TARGET:
					m_vPlayerLook = pOther->Get_PushLook();
					break;

				}
				XMStoreFloat3(&m_vPlayerLook, XMVector3Normalize(XMLoadFloat3(&m_vPlayerLook)));
				Set_State(HITFRONT);
				
			}
			else
			{
				switch (pOther->Get_PushType())
				{
				case PUSH:
					m_vPlayerLook = (pOther)->Get_Dir(_float3(0.f, 0.f, 1.f));
					break;

				case PULL:
					XMStoreFloat3(&m_vPlayerLook, XMLoadFloat4(&pOther->Get_WorldPos()) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));
					break;

				case TARGET:
					m_vPlayerLook = pOther->Get_PushLook();
					break;

				}
				XMStoreFloat3(&m_vPlayerLook, XMVector3Normalize(XMLoadFloat3(&m_vPlayerLook)));
				Set_State(HITBACK);

			}

		}

	}
	else if (pOther->Get_HitType() == TYPE_SPIN)
	{
		switch (pOther->Get_PushType())
		{
		case PUSH:
			m_vPlayerLook = (pOther)->Get_Dir(_float3(0.f, 0.f, 1.f));
			break;

		case PULL:
			XMStoreFloat3(&m_vPlayerLook, XMLoadFloat4(&pOther->Get_WorldPos()) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			break;

		case TARGET:
			m_vPlayerLook = pOther->Get_PushLook();
			break;

		}
		XMStoreFloat3(&m_vPlayerLook, XMVector3Normalize(XMLoadFloat3(&m_vPlayerLook)));
		XMStoreFloat3(&m_vTargetLook, XMLoadFloat4(&pOther->Get_WorldPos()) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&m_vTargetLook), 0.8f);
		Set_State(HITSPIN);
		m_pTransformCom->ResetAndJump(m_fHAirPower);
	}
	else if (pOther->Get_HitType() == TYPE_AWAY)
	{
		switch (pOther->Get_PushType())
		{
		case PUSH:
			m_vPlayerLook = (pOther)->Get_Dir(_float3(0.f, 0.f, 1.f));
			break;

		case PULL:
			XMStoreFloat3(&m_vPlayerLook, XMLoadFloat4(&pOther->Get_WorldPos()) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			break;

		case TARGET:
			m_vPlayerLook = pOther->Get_PushLook();
			break;

		}
		XMStoreFloat3(&m_vPlayerLook, XMVector3Normalize(XMLoadFloat3(&m_vPlayerLook)));
		XMStoreFloat3(&m_vTargetLook, XMLoadFloat4(&pOther->Get_WorldPos()) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		Set_State(HITHORI);
		m_pTransformCom->ResetAndJump(m_fHAirPower);

	}
	else if (pOther->Get_HitType() == TYPE_UP)
	{
		Set_State(HITUP);
		m_pTransformCom->ResetAndJump(m_fHAirPower);
	}
	else if (pOther->Get_HitType() == TYPE_FASTDOWN)
	{
		Set_State(HITSLAPDOWN);
		m_pTransformCom->ResetAndJump(m_fHAirPower);
	}
}


void COldBombRobot::End_Animation()
{
	if (m_pModelCom->GetAniEnd())
	{
		//애니메이션 끝났을때
		switch (m_eCurState)
		{
		case Client::COldBombRobot::SPAWN:
			Set_State(IDLE);
			break;
		case Client::COldBombRobot::IDLE:
			if (m_bMove)
			{
				m_fAttackAcc = 0.f;
				m_bAttack = false;
				m_bOneCheck = false;
				m_bHitAble = true;
				m_bAction = true;
				m_bBackWalk = false;
			}
			break;
		case Client::COldBombRobot::RUN:
			if (m_bRun && !m_bEscape)
			{
				m_bAttack = false;
				m_bOneCheck = false;
				m_pModelCom->Set_AnimIndex(RUN);
				m_eCurState = RUN;
				m_eNextState = m_eCurState;
				m_pModelCom->SetNextIndex(m_eNextState);
			}
			m_bRunSound1 = false;
			m_bRunSound2 = false;
			break;
		case Client::COldBombRobot::WALKBACK:
			if (m_bBackWalk)
			{
				m_bBackWalk = false;
				m_bHitAble = true;
				m_bBackWalkSound1 = false;
				m_bBackWalkSound2 = false;
				Set_State(WALKBACK);
			}
			else
				Set_State(IDLE);
			break;
		case Client::COldBombRobot::WALKFRONT:
			break;
		case Client::COldBombRobot::STRAFELEFT:
			m_fAttackAcc = 0.f;
			m_bAttack = false;
			m_bOneCheck = false;
			m_bHitAble = true;
			m_bAction = true;
			m_bBackWalk = false;
			break;
		case Client::COldBombRobot::STRAFERIGHT:
			m_fAttackAcc = 0.f;
			m_bAttack = false;
			m_bOneCheck = false;
			m_bHitAble = true;
			m_bAction = true;
			m_bBackWalk = false;
			break;
		case Client::COldBombRobot::ATTACK1:
			m_bEscape = false;
			m_bEscapeStart = false;
			m_fEscapeAcc = 0.f;
			Set_State(STATE::WALKBACK);
			break;
		case Client::COldBombRobot::ATTACK2:
			if (m_bAvoidAttack)
			{
				m_bAttack = false;
				m_bAction = true;
				m_bOneCheck = false;
				m_bHitAble = true;
				m_bAvoidAttack = false;
			}
			else
			{
				m_bBackWalk = true;
				Set_State(WALKBACK);
			}
			break;
		case Client::COldBombRobot::ATTACK3:
			Set_State(STATE::WALKBACK);
			break;
		case Client::COldBombRobot::HITBACK:
			if (m_bDeadStart)
				Set_State(STUN);
			else
			{
				if (m_bAvoidAttack)
				{
					Set_State(ATTACK2);
					m_bReGauge = true;
				}
				else
				{
					m_fAttackAcc = 0.f;
					m_bAttack = false;
					m_bOneCheck = false;
					m_bDontWalk = false;
					Set_State(IDLE);
				}
			}
			break;
		case Client::COldBombRobot::HITFRONT:
			if (m_bDeadStart)
				Set_State(STUN);
			else
			{
				if (m_bAvoidAttack)
				{
					Set_State(ATTACK2);
					m_bReGauge = true;
				}
				else
				{
					m_fAttackAcc = 0.f;
					m_bAttack = false;
					m_bOneCheck = false;
					m_bDontWalk = false;
					Set_State(IDLE);
				}
			}
			break;
		case Client::COldBombRobot::HITFLY:
			Set_State(HITDOWN);
			break;
		case Client::COldBombRobot::HITDOWN:
			m_bDontWalk = false;
			break;
		case Client::COldBombRobot::HITSPIN:
			Set_State(HITDOWN);
			break;
		case Client::COldBombRobot::HITHORI:
			Set_State(BOUNCE);
			break;
		case Client::COldBombRobot::HITUP:
			Set_State(HITDOWN);
			break;
		case Client::COldBombRobot::HITSLAPDOWN:
			Set_State(HITDOWN);
			break;
		case Client::COldBombRobot::BOUNCE:
			Set_State(DOWN);
			break;
		case Client::COldBombRobot::DOWN:
			if (!m_bDeadStart)
				Set_State(GETUP);
			else
			{
				Create_DeadEffect();
				Set_Dead();
				GI->PlaySoundW(L"se_EN0005_attack2_3.wav", SD_TRAININGBOT1, 0.6f);
			}
			break;
		case Client::COldBombRobot::GETUP:
			if (m_bAvoidAttack)
			{
				Set_State(ATTACK2);
				m_bReGauge = true;
			}
			else
			{
				m_fAttackAcc = 0.f;
				m_bAttack = false;
				m_bOneCheck = false;
				m_bAction = true;
				m_bHitAble = true;
				m_bDontWalk = false;
				Set_State(IDLE);
			}
			break;
		case Client::COldBombRobot::STUN:
			if (!m_bDeadStart)
				Set_State(STUNRECOVER);
			else
			{
				m_bDissolve = true;
			}
			break;
		case Client::COldBombRobot::STUNRECOVER:
			Set_State(IDLE);
			break;
		default:
			break;

		}

	}
}


void COldBombRobot::Update(_float fTimeDelta)
{
	m_bRangeCol = false;
	m_bPush = true;

	if (!m_pModelCom->GetChangeBool())
	{
		switch (m_eCurState)
		{
			//다음 상태로 넘어갈때 해줘야하는것들
			//애니메이션x
	
		case Client::COldBombRobot::SPAWN:
			m_bRangeCol = true;
			Spawn_Update(fTimeDelta);
			break;
		case Client::COldBombRobot::IDLE:
			m_bRangeCol = true;
			LookAtPlayer();
			break;
		case Client::COldBombRobot::RUN:
			Run_Update(fTimeDelta);
			break;
		case Client::COldBombRobot::WALKBACK: //텀주기 위한 상태, 뒤로 살짝 물러남, 애니메이션 자체가 뒤로 물러나므로 트랜스폼함수 필요없음
			WalkBack_Update(fTimeDelta);
			break;
		case Client::COldBombRobot::WALKFRONT:
			Walk_Update(fTimeDelta);
			break;
		case Client::COldBombRobot::STRAFELEFT:
			Strafe_Update(fTimeDelta);
			break;
		case Client::COldBombRobot::STRAFERIGHT:
			Strafe_Update(fTimeDelta);
			break;
		case Client::COldBombRobot::ATTACK1:
			Attack1_Update(fTimeDelta);
			break;
		case Client::COldBombRobot::ATTACK2:
			Attack2_Update(fTimeDelta);
			break;
		case Client::COldBombRobot::ATTACK3:
			Attack3_Update(fTimeDelta);
			break;
		case Client::COldBombRobot::HITBACK:
			HitBack_Update(fTimeDelta);
			break;
		case Client::COldBombRobot::HITFRONT:
			HitFront_Update(fTimeDelta);
			break;
		case Client::COldBombRobot::HITFLY:
			HitFly_Update(fTimeDelta);
			break;
		case Client::COldBombRobot::HITDOWN:
			HitDown_Update(fTimeDelta);
			break;
		case Client::COldBombRobot::HITSPIN:
			HitSpin_Update(fTimeDelta);
			break;
		case Client::COldBombRobot::HITHORI:
			m_bAction = false;
			HitHori_Update(fTimeDelta);
			break;
		case Client::COldBombRobot::HITUP:
			HitUp_Update(fTimeDelta);
			break;
		case Client::COldBombRobot::HITSLAPDOWN:
			HitSlapdown_Update(fTimeDelta);
			break;
		case Client::COldBombRobot::BOUNCE:
			break;
		case Client::COldBombRobot::DOWN:
			break;
		case Client::COldBombRobot::GETUP:
			break;
		case Client::COldBombRobot::STUN:
			break;
		case Client::COldBombRobot::STUNRECOVER:
			break;
		default:
			break;
		}

	}

}

void COldBombRobot::Set_Dir()
{
	//플레이어 포스 받아오기 미리 받아둘지

	//내가 바라봐야할 룩벡터 구하기, 플레이어 포스에서 내포스 빼고 노멀라이즈
	_vector vLook = XMVector3Normalize(XMLoadFloat4(&m_fPlayerPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	XMStoreFloat3(&m_vTargetLook, vLook);


}

void COldBombRobot::LookDirPlayer()
{
	Set_Dir();

	_vector vLook, vRight;
	vLook = (XMVector3Normalize(XMLoadFloat3(&m_vTargetLook)));

	vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	vLook = XMVector3Normalize(XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	_float3      vScale = m_pTransformCom->Get_Scale();

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}


void COldBombRobot::Distance_BattlePattern(_float fTimeDelta)
{
	m_fPlayerPos = PM->Get_PlayerPos();

	m_fDistance = m_pTransformCom->TargetDistance(XMLoadFloat4(&m_fPlayerPos)); //타겟과 나의 거리 계속 계산

	if (m_bSpawn && m_bAction) // 스폰이 끝났을때 시작
		ActionToPlayer(fTimeDelta);

	RegretToPlayer(fTimeDelta);
}


void COldBombRobot::ActionToPlayer(_float fTimeDelta)
{
	if (m_fDistance <= 50.f)
	{
		//거리에 따라서 걸을지 뛸지
		if (!m_bAttack)
		{
			if (m_fDistance > 5.f)
			{
				m_bAttack = true;
				m_bRun = true;
				Set_State(STATE::RUN);
			}
			else
			{
				Set_State(STATE::IDLE);
				m_bRun = false;
			}
		}

		AttackPattern(fTimeDelta);
	}
	else
	{
		Set_State(RUN);
	}
}


void COldBombRobot::AttackPattern(_float fTimeDelta)
{
	m_fAttackAcc += 1.f * fTimeDelta;

	if (m_fAttackAcc > 0.4f)
	{
		m_bAttack = true;

		if (!m_bOneRandom)
		{
			m_iRandomAttack = GI->Get_Random(1, 3);
			m_bOneRandom = true;
		}


		//공격할수있는 상태 - AttackTime이 지정시간 이상이어야되고, 맞는 애니메이션이 끝났을때여야하고, 
		switch (m_iRandomAttack)
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
void COldBombRobot::RegretToPlayer(_float fTimeDelta)
{
	if (m_bSpawn && !m_bMove)
	{
		Set_State(IDLE);
	}
}

void COldBombRobot::Check_Rim(_float fTimeDelta)
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
		m_iShaderPath = 1;
	else
		m_iShaderPath = 4;//0;
}

void COldBombRobot::Check_Death(_float fTimeDelta)
{
	if (!m_bMinusCount && m_fNowHp <= 0.f)
	{
		m_bMinusCount = true;
		MM->Minus_MonsterCount();
		Event_Stage();
		Create_Item();
	}

	if (!m_pTransformCom->Get_Air() && m_fNowHp <= 0.f && !m_bDeadStart)
	{

		m_bAction = false;
		m_bHitAble = false;
		m_bRangeCol = false;
		m_bAttackCol = false;
		m_bBodyCol = false;
		m_bDeadStart = true;
		GI->PlaySoundW(L"se_EN0004_death.wav", SD_TRAININGBOT1, 0.7f); // 터지면서 나는 소리
	}

}

void COldBombRobot::Check_Avoid()
{
	if (m_fPrePatternGauge >= m_fMaxPg)
	{
		m_bAvoidAttack = true;
		m_bAction = false;
		m_bHitAble = false;
	}
}

void COldBombRobot::HitAwayPower_Cehck()
{
	_uint iPushPower = (_uint)m_fHPushPower / 10;

	switch (iPushPower)
	{
	case 0:
		m_fHPushPower -= 1.f;
		break;
	case 1:
		m_fHPushPower -= 5.f;
		break;
	case 2:
		m_fHPushPower -= 5.f;
		break;
	case 3:
		m_fHPushPower -= 10.f;
		break;
	case 4:
		m_fHPushPower -= 15.f;
		break;
	default:
		break;
	}
}

void COldBombRobot::Event_Stage()
{
	switch (m_iIndex)
	{
	case 3:
		MM->Event_Stage3_1();
		break;
	case 4:
		MM->Event_Stage3_2();
		break;
	case 7:
		MM->Event_Stage4_3();
		break;
	default:
		break;
	}
}

void COldBombRobot::Reset_Collider()
{
	m_bAttack3Col_1 = false;
	m_bAttack3Col_2 = false;
	m_bAttack3Col_3 = false;
	m_bAttack3Col_4 = false;
	m_bAttack3Col_5 = false;
	m_bAttack3Col_6 = false;
}

void COldBombRobot::Attack1(_float fTimeDelta)
{
	
	if (!m_bOneCheck)
	{
		m_bOneCheck = true;
		m_bOneRandom = false;
		Set_State(STATE::ATTACK1);
	}
	
}

void COldBombRobot::Attack2(_float fTimeDelta)
{
	
	if (!m_bOneCheck)
	{
		m_bOneCheck = true;
		m_bOneRandom = false;
		Set_State(STATE::ATTACK2);
	}
	

}

void COldBombRobot::Attack3(_float fTimeDelta)
{
	if (m_fDistance > 3.f)
	{
		if (!m_bOneCheck)
			Set_State(RUN);
	}
	else
	{
		if (!m_bOneCheck)
		{
			m_bOneCheck = true;
			m_bOneRandom = false;
			Set_State(STATE::ATTACK3);
		}
	}
}

void COldBombRobot::EscapePlayer(_float fTimeDelta)
{
	_vector vPlayerPos = XMLoadFloat4(&PM->Get_PlayerPos());


		_float fTempX = GI->Get_FloatRandom(-14.f, -7.f);
		_float fTempY = GI->Get_FloatRandom(-4.f, 0.f);
		vPlayerPos = XMVectorSetX(vPlayerPos,  fTempX);
		vPlayerPos = XMVectorSetY(vPlayerPos,  fTempY);
	

	_vector vLook = XMVector3Normalize(vPlayerPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), vLook, 0.1f);

	m_pTransformCom->Go_Dir(vLook, m_fSpeed, fTimeDelta, m_pNavigationCom);

}

void COldBombRobot::Spawn_Update(_float fTimeDelta)
{

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bSpawnSound)
	{
		m_bSpawnSound = true;
		GI->PlaySoundW(L"se_enemy_spawn2.wav", SD_TRAININGBOT2, 0.5f);

	}

}


void COldBombRobot::Attack1_Update(_float fTimeDelta)
{
	m_bPush = false;
	m_bRangeCol = true;
	
	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(0))
		LookAtPlayer();

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(7) && !m_bAttack1Sound)
	{
		m_bAttack1Sound = true;

		GI->PlaySoundW(L"se_EN0005_attack1_1.wav", SD_TRAININGBOT1, 0.4f); // 포즈 취할 때 넣어주세요

	}
		

	_matrix MuzzleRMatrix = m_pBulletRNode->Get_CombinedTransformation()
		* m_pModelCom->Get_PivotMatrix()
		*m_pTransformCom->Get_WorldMatrix();

	_matrix MuzzleLMatrix = m_pBulletLNode->Get_CombinedTransformation()
		* m_pModelCom->Get_PivotMatrix()
		*m_pTransformCom->Get_WorldMatrix();

	CMonsterMissile::MISSILEINFO _tInfo;
	ZeroMemory(&_tInfo, sizeof(CMonsterMissile::MISSILEINFO));
	_tInfo.iBulletType = 0;
	XMStoreFloat4(&_tInfo.vDir, m_pTransformCom->Get_State(CTransform::STATE_LOOK));

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bShootBullet1)
	{
		GI->PlaySoundW(L"se_EN0005_attack1_2.wav", SD_TRAININGBOT1, 0.4f);
		m_bShootBullet1 = true;
		XMStoreFloat4(&_tInfo.vPosition, MuzzleRMatrix.r[3]);
		GI->Add_GameObjectToLayer(TEXT("MonsterMissile"), LEVEL_TEST, TEXT("Layer_Monster"), &_tInfo);
		XMStoreFloat4(&_tInfo.vPosition, MuzzleLMatrix.r[3]);
		GI->Add_GameObjectToLayer(TEXT("MonsterMissile"), LEVEL_TEST, TEXT("Layer_Monster"), &_tInfo);
	}
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bShootBullet2)
	{
		GI->PlaySoundW(L"se_EN0005_attack1_2.wav", SD_TRAININGBOT2, 0.4f);
		m_bShootBullet2 = true;
		XMStoreFloat4(&_tInfo.vPosition, MuzzleRMatrix.r[3]);
		GI->Add_GameObjectToLayer(TEXT("MonsterMissile"), LEVEL_TEST, TEXT("Layer_Monster"), &_tInfo);
		XMStoreFloat4(&_tInfo.vPosition, MuzzleLMatrix.r[3]);
		GI->Add_GameObjectToLayer(TEXT("MonsterMissile"), LEVEL_TEST, TEXT("Layer_Monster"), &_tInfo);

	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(3) && !m_bShootBullet3)
	{
		GI->PlaySoundW(L"se_EN0005_attack1_2.wav", SD_TRAININGBOT1, 0.4f);
		m_bShootBullet3 = true;
		XMStoreFloat4(&_tInfo.vPosition, MuzzleRMatrix.r[3]);
		GI->Add_GameObjectToLayer(TEXT("MonsterMissile"), LEVEL_TEST, TEXT("Layer_Monster"), &_tInfo);
		XMStoreFloat4(&_tInfo.vPosition, MuzzleLMatrix.r[3]);
		GI->Add_GameObjectToLayer(TEXT("MonsterMissile"), LEVEL_TEST, TEXT("Layer_Monster"), &_tInfo);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(4) && !m_bShootBullet4)
	{
		GI->PlaySoundW(L"se_EN0005_attack1_2.wav", SD_TRAININGBOT2, 0.4f);
		m_bShootBullet4 = true;
		XMStoreFloat4(&_tInfo.vPosition, MuzzleRMatrix.r[3]);
		GI->Add_GameObjectToLayer(TEXT("MonsterMissile"), LEVEL_TEST, TEXT("Layer_Monster"), &_tInfo);
		XMStoreFloat4(&_tInfo.vPosition, MuzzleLMatrix.r[3]);
		GI->Add_GameObjectToLayer(TEXT("MonsterMissile"), LEVEL_TEST, TEXT("Layer_Monster"), &_tInfo);
	}
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(5) && !m_bShootBullet5)
	{
		GI->PlaySoundW(L"se_EN0005_attack1_2.wav", SD_TRAININGBOT1, 0.4f);
		m_bShootBullet5 = true;
		XMStoreFloat4(&_tInfo.vPosition, MuzzleRMatrix.r[3]);
		GI->Add_GameObjectToLayer(TEXT("MonsterMissile"), LEVEL_TEST, TEXT("Layer_Monster"), &_tInfo);
		XMStoreFloat4(&_tInfo.vPosition, MuzzleLMatrix.r[3]);
		GI->Add_GameObjectToLayer(TEXT("MonsterMissile"), LEVEL_TEST, TEXT("Layer_Monster"), &_tInfo);
	}
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(6) && !m_bShootBullet6)
	{
		GI->PlaySoundW(L"se_EN0005_attack1_2.wav", SD_TRAININGBOT2, 0.4f);
		m_bShootBullet6 = true;
		XMStoreFloat4(&_tInfo.vPosition, MuzzleRMatrix.r[3]);
		GI->Add_GameObjectToLayer(TEXT("MonsterMissile"), LEVEL_TEST, TEXT("Layer_Monster"), &_tInfo);
		XMStoreFloat4(&_tInfo.vPosition, MuzzleLMatrix.r[3]);
		GI->Add_GameObjectToLayer(TEXT("MonsterMissile"), LEVEL_TEST, TEXT("Layer_Monster"), &_tInfo);
	}
}

void COldBombRobot::Attack2_Update(_float fTimeDelta)
{
	_matrix MuzzleRMatrix = m_pMissileRNode->Get_CombinedTransformation()
		* m_pModelCom->Get_PivotMatrix()
		*m_pTransformCom->Get_WorldMatrix();
	_matrix MuzzleLMatrix = m_pMissileLNode->Get_CombinedTransformation()
		* m_pModelCom->Get_PivotMatrix()
		*m_pTransformCom->Get_WorldMatrix();

	CMonsterMissile::MISSILEINFO _tInfo;
	ZeroMemory(&_tInfo, sizeof(CMonsterMissile::MISSILEINFO));
	_tInfo.iBulletType = 1;
	XMStoreFloat4(&_tInfo.vDir, m_pTransformCom->Get_State(CTransform::STATE_LOOK));

	m_bPush = false;
	m_bRangeCol = true;

	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(0))
		LookAtPlayer();

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(3) && !m_bOnePatternEffect)
	{
		m_bOnePatternEffect = true;
		if (m_bAvoidAttack)
			Create_PatternEffect();
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bShootMissile1)
	{
		GI->PlaySoundW(L"se_EN0005_attack2_2.wav", SD_TRAININGBOT1, 0.4f);

		m_bShootMissile1 = true;
		
		XMStoreFloat4(&_tInfo.vPosition, MuzzleRMatrix.r[3]);
		_tInfo.m_fSide = -1.f;
		_tInfo.iMissileIndex = 0;
		GI->Add_GameObjectToLayer(TEXT("MonsterMissile"), LEVEL_TEST, TEXT("Layer_Monster"), &_tInfo);
		_tInfo.iMissileIndex = 1;
		GI->Add_GameObjectToLayer(TEXT("MonsterMissile"), LEVEL_TEST, TEXT("Layer_Monster"), &_tInfo);
		_tInfo.iMissileIndex = 2;
		GI->Add_GameObjectToLayer(TEXT("MonsterMissile"), LEVEL_TEST, TEXT("Layer_Monster"), &_tInfo);

		XMStoreFloat4(&_tInfo.vPosition, MuzzleLMatrix.r[3]);
		_tInfo.m_fSide = 1.f;
		_tInfo.iMissileIndex = 0;
		GI->Add_GameObjectToLayer(TEXT("MonsterMissile"), LEVEL_TEST, TEXT("Layer_Monster"), &_tInfo);
		_tInfo.iMissileIndex = 1;
		GI->Add_GameObjectToLayer(TEXT("MonsterMissile"), LEVEL_TEST, TEXT("Layer_Monster"), &_tInfo);
		_tInfo.iMissileIndex = 2;
		GI->Add_GameObjectToLayer(TEXT("MonsterMissile"), LEVEL_TEST, TEXT("Layer_Monster"), &_tInfo);

	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bAttack2Effect)
	{
		m_bAttack2Effect = true;

		_matrix a = m_pTransformCom->Get_WorldMatrix();
		a.r[0] = XMVector3Normalize(a.r[0]);
		a.r[1] = XMVector3Normalize(a.r[1]);
		a.r[2] = XMVector3Normalize(a.r[2]);
		_float4x4 Matrix;
		XMStoreFloat4x4(&Matrix, a);
		PTM->Create_AlphaParticle(L"MonsterMissileStart", Matrix);
		PTM->Create_AlphaParticle(L"MonsterMissileLeft", Matrix);
	}


	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bAttack2Sound)
	{
		m_bAttack2Sound = true;

		GI->PlaySoundW(L"se_EN0007_attack1_1.wav", SD_TRAININGBOT1, 0.6f); // 포즈 취할 때 넣어주세요
	}
}

void COldBombRobot::Attack3_Update(_float fTimeDelta)
{
	m_bPush = false;
	m_bRangeCol = true;
	m_bAttackCol = false;

	//공격할때 attackcol 키기
	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(0))
		LookAtPlayer();

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bAttack3Col_1)
	{
		GI->PlaySoundW(L"se_EN0004_attack3_3.wav", SD_TRAININGBOT1, 0.4f);
		m_bAttackCol = true;
		m_bAttack3Col_1 = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bAttack3Col_2)
	{
		GI->PlaySoundW(L"se_EN0004_attack3_4.wav", SD_TRAININGBOT2, 0.4f);
		m_bAttackCol = true;
		m_bAttack3Col_2 = true;
	}
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(3) && !m_bAttack3Col_3)
	{
		m_bAttackCol = true;
		m_bAttack3Col_3 = true;
	}
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(4) && !m_bAttack3Col_4)
	{
		m_bAttackCol = true;
		m_bAttack3Col_4 = true;
	}
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(5) && !m_bAttack3Col_5)
	{
		m_bAttackCol = true;
		m_bAttack3Col_5 = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(6) && !m_bAttack3Col_6)
	{
		m_bAttackCol = true;
		m_bAttack3Col_6 = true;
	}

}

void COldBombRobot::HitBack_Update(_float fTimeDelta)
{
	if (m_bSameAttack) //똑같은 공격 받았을때
	{
		m_bSameAttack = false;
		m_pModelCom->Set_AnimIndex(HITBACK);
		m_eCurState = HITBACK;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
	}
}

void COldBombRobot::HitFly_Update(_float fTimeDelta)
{
	if (!m_pTransformCom->Get_Air())
		Set_State(BOUNCE);

	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vPlayerLook), m_fHPushPower, fTimeDelta, m_pNavigationCom);

	if (m_bSameAttack) //똑같은 공격 받았을때
	{
		m_bSameAttack = false;
		m_pModelCom->Set_AnimIndex(HITFLY);
		m_eCurState = HITFLY;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
	}
}

void COldBombRobot::HitFront_Update(_float fTimeDelta)
{
	if (m_bSameAttack)
	{
		m_bSameAttack = false;
		m_pModelCom->Set_AnimIndex(HITFRONT);
		m_eCurState = HITFRONT;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
	}
}

void COldBombRobot::HitDown_Update(_float fTimeDelta)
{
	if (!m_pTransformCom->Get_Air())
	{
		m_pModelCom->Set_AnimIndex(BOUNCE);
		m_eCurState = BOUNCE;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
		m_pTransformCom->ResetAndJump(1.f);

	}

	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vPlayerLook), m_fHPushPower, fTimeDelta, m_pNavigationCom);
}

void COldBombRobot::HitHori_Update(_float fTimeDelta)
{

	if (!m_pTransformCom->Get_Air())
	{
		Set_State(BOUNCE);
	}

	if (m_fHPushPower > 5.f)
		m_fHPushPower -= 5.f * fTimeDelta;

	if (m_fHPushPower <= 5.f)
		m_fHPushPower = 5.f;

	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vPlayerLook), m_fHPushPower, fTimeDelta, m_pNavigationCom);

	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(0))
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&m_vTargetLook), 0.8f);

	if (m_bSameAttack)
	{
		m_bSameAttack = false;
		m_pModelCom->Set_AnimIndex(HITHORI);
		m_eCurState = HITHORI;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
	}
}

void COldBombRobot::HitSpin_Update(_float fTimeDelta)
{
	m_bAction = false;

	if (!m_pTransformCom->Get_Air())
		Set_State(BOUNCE);

	if (m_fHPushPower > 1.f)
		m_fHPushPower -= 1.f * fTimeDelta;

	if (m_fHPushPower < 1.f)
	{
		m_fHPushPower = 1.f;
	}

	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vPlayerLook), m_fHPushPower, fTimeDelta, m_pNavigationCom);


	if (m_bSameAttack)
	{
		m_bSameAttack = false;
		m_pModelCom->Set_AnimIndex(HITSPIN);
		m_eCurState = HITSPIN;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
	}
}

void COldBombRobot::HitUp_Update(_float fTimeDelta)
{
	if (!m_pTransformCom->Get_Air())
		Set_State(BOUNCE);

	if (m_bSameAttack)
	{
		m_bSameAttack = false;
		m_pModelCom->Set_AnimIndex(HITUP);
		m_eCurState = HITUP;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
	}
}

void COldBombRobot::HitSlapdown_Update(_float fTimeDelta)
{
	if (!m_pTransformCom->Get_Air())
	{
		Set_State(BOUNCE);

	}
	if (m_bSameAttack)
	{
		m_bSameAttack = false;
		m_pModelCom->Set_AnimIndex(HITSLAPDOWN);
		m_eCurState = HITSLAPDOWN;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
	}
}

void COldBombRobot::Run_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	m_fSpeed = 6.f;

	if (m_bEscape)
	{
		Set_Dir();
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&m_vTargetLook) * -1.f, 0.1f);
		m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook) * -1.f, m_fSpeed, fTimeDelta, m_pNavigationCom);
	}
	else
	{
		LookAtPlayer();
		m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), m_fSpeed, fTimeDelta, m_pNavigationCom);
	}


	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bRunSound1)
	{
		m_bRunSound1 = true;
		GI->PlaySoundW(L"se_EN0004_step_1-01.wav", SD_TRAININGBOT1, 0.1f);

	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bRunSound2)
	{
		m_bRunSound2 = true;
		GI->PlaySoundW(L"se_EN0004_step_1-01.wav", SD_TRAININGBOT1, 0.1f);

	}
}

void COldBombRobot::Walk_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	m_fSpeed = 4.f;

	LookAtPlayer();
	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), m_fSpeed, fTimeDelta, m_pNavigationCom);


	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bWalkSound1)
	{
		m_bWalkSound1 = true;
		GI->PlaySoundW(L"se_EN0004_step_1-01.wav", SD_TRAININGBOT1, 0.1f);

	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bWalkSound2)
	{
		m_bWalkSound2 = true;
		GI->PlaySoundW(L"se_EN0004_step_1-02.wav", SD_TRAININGBOT1, 0.1f);

	}
}

void COldBombRobot::WalkBack_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	m_fSpeed = 6.f;
	LookAtPlayer();

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bBackWalkSound1)
	{
		m_bBackWalkSound1 = true;
		GI->PlaySoundW(L"se_EN0004_step_1-01.wav", SD_TRAININGBOT1, 0.1f);

	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bBackWalkSound2)
	{
		m_bBackWalkSound2 = true;
		GI->PlaySoundW(L"se_EN0004_step_1-02.wav", SD_TRAININGBOT1, 0.1f);

	}


}

void COldBombRobot::Strafe_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	LookAtPlayer();

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bStrafeSound1)
	{
		m_bStrafeSound1 = true;
		GI->PlaySoundW(L"se_EN0004_step_2-01.wav", SD_TRAININGBOT1, 0.1f);

	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bStrafeSound2)
	{
		m_bStrafeSound2 = true;
		GI->PlaySoundW(L"se_EN0004_step_2-02.wav", SD_TRAININGBOT1, 0.1f);

	}
}

void COldBombRobot::Create_Item()
{
	_uint iRandomA = GI->Get_Random(0, 10);

	CDropItem::DROPITEMDESC DropItemDesc;
	DropItemDesc.iItemType = CDropItem::TYPE_WAFER;
	DropItemDesc.iNaviNum = GI->Get_Cur_LevelIndex();
	//DropItemDesc.vStartPosition = { 0.f,0.1f,5.f,1.f };
	XMStoreFloat4(&DropItemDesc.vStartPosition, (m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
	for (_uint i = 0; 2 > i; ++i)
	{
		GI->Add_GameObjectToLayer(TEXT("DropItem"), GI->Get_Cur_LevelIndex(), TEXT("Monster_DropItem"), &DropItemDesc);
	}

	DropItemDesc.iItemType = CDropItem::TYPE_ARM;
	GI->Add_GameObjectToLayer(TEXT("DropItem"), GI->Get_Cur_LevelIndex(), TEXT("Monster_DropItem"), &DropItemDesc);
	
	if (2 <= iRandomA)
	{
		DropItemDesc.iItemType = CDropItem::TYPE_CHIP;
		GI->Add_GameObjectToLayer(TEXT("DropItem"), GI->Get_Cur_LevelIndex(), TEXT("Monster_DropItem"), &DropItemDesc);
	}

	DropItemDesc.iItemType = CDropItem::TYPE_MONEY;
	GI->Add_GameObjectToLayer(TEXT("DropItem"), GI->Get_Cur_LevelIndex(), TEXT("Monster_DropItem"), &DropItemDesc);
}

void COldBombRobot::Create_PatternEffect()
{
	_matrix Effectmatrix;
	Effectmatrix.r[0] = { 1.f, 0.f, 0.f, 0.f };
	Effectmatrix.r[1] = { 0.f, 1.f, 0.f, 0.f };
	Effectmatrix.r[2] = { 0.f, 0.f, 1.f, 0.f };
	Effectmatrix.r[3] = { 0.f, 1.73f, 1.2f, 1.f };

	_float4x4 PatternMatrx;

	XMStoreFloat4x4(&PatternMatrx, Effectmatrix * m_pTransformCom->Get_WorldMatrix());

	(PTM->Create_AlphaParticle(L"PatternGaugeCircle", PatternMatrx));
	(PTM->Create_AlphaParticle(L"PatternGaugeLine", PatternMatrx));
	(PTM->Create_AlphaParticle(L"PatternGaugeFlare", PatternMatrx));

	LookAtPlayer();

	GI->PlaySoundW(L"se_enemy_warning.wav", SD_TRAININGBOT2, 0.6f);

}

void COldBombRobot::Create_DeadEffect()
{
	_matrix a = m_pTransformCom->Get_WorldMatrix();
	a.r[0] = XMVector3Normalize(a.r[0]);
	a.r[1] = XMVector3Normalize(a.r[1]);
	a.r[2] = XMVector3Normalize(a.r[2]);
	_float4x4 Matrix;
	XMStoreFloat4x4(&Matrix, a);
	Matrix.m[3][1] = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] + 1.f;
	CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
	Info->fBlurPower = 0.f;
	Info->vColor = { 0.7f,0.7f,0.7f,1.f };
	Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
	Info->vLocalPos = { 0.f,0.f,0.f,1.f };
	Info->vRatio = { 1.f,1.f };
	Info->vRatioSpeed = { 1.5f,1.5f };
	Info->vScale = { 0.1f,0.1f,0.1f };
	Info->vScaleSpeed = { 100.f,100.f,100.f };
	Info->vUvFixSpeed = { 1.f,3.f };
	Info->vWorldMatrix = Matrix;
	GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);

	CPlayer01SmokeBall::SMOKEBALLINFO* Info2 = new CPlayer01SmokeBall::SMOKEBALLINFO;
	Info2->fBlurPower = 1.f;
	Info2->vColor = { 0.7f,0.7f,0.7f,1.f };
	Info2->vColor2 = { 0.7f,0.7f,0.7f,1.f };
	Info2->vLocalPos = { 0.f,0.f,0.f,1.f };
	Info2->vRatio = { 1.f,1.f };
	Info2->vRatioSpeed = { 1.5f,1.5f };
	Info2->vScale = { 0.1f,0.1f,0.1f };
	Info2->vScaleSpeed = { 100.f,100.f,100.f };
	Info2->vUvFixSpeed = { -1.f,-3.f };
	Info2->vWorldMatrix = Matrix;
	GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2);

	PTM->Create_InstanceParticle(L"Player03Bomb_3", Matrix);
	PTM->Create_InstanceParticle(L"ExplosionEmbers", Matrix);
	PTM->Create_InstanceParticle(L"Player03Bomb_5", Matrix);
	PTM->Create_InstanceParticle(L"Player03Bomb_6", Matrix);
	PTM->Create_InstanceParticle(L"Player03Bomb_7", Matrix);
	PTM->Create_InstanceParticle(L"Player03Bomb_8", Matrix);
}

void COldBombRobot::LookAtPlayer(_float fLiearTime)
{
	Set_Dir();
	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&m_vTargetLook), fLiearTime);

}


HRESULT COldBombRobot::Ready_Components()
{
	//m_pEmissiveCom
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_OldBomb_Emi"), TEXT("Com_TextureEmi"), (CComponent**)&m_pEmissiveCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("OldBombRobot"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	m_pModelCom->Set_RootHierarchy(3);

	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.4f, 0.4f, 0.4f);
	ColDesc.vCenter = _float3(0.f, 0.3f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Push_Range"); // 밀어내기 위함

	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.5f, 1.5f, 1.5f);
	ColDesc.vCenter = _float3(0.f, 1.f, 0.9f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Attack"); //범위공격

	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	CHierarchyNode*		pChest = m_pModelCom->Get_HierarchyNode("Chest_M");
	if (nullptr == pChest)
		return E_FAIL;
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.9f, 0.9f, 0.9f);
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.pSocket = pChest;
	XMStoreFloat4x4(&ColDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	ColDesc.pOwner = this;
	ColDesc.iFlag = 0;
	strcpy(ColDesc.sTag, "Monster_Body1"); //몬스터 공격용 충돌체
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	m_pMissileLNode = m_pModelCom->Get_HierarchyNode("Scapula_L");
	m_pMissileRNode = m_pModelCom->Get_HierarchyNode("Scapula_R");
	m_pBulletRNode = m_pModelCom->Get_HierarchyNode("Wrist_R");
	m_pBulletLNode = m_pModelCom->Get_HierarchyNode("Wrist_L");
	return S_OK;
}


HRESULT COldBombRobot::Create_Navigation(_uint iNaviNum)
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
		break;
	}

	_int iNaviIndex = m_pNavigationCom->Find_NaviIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pNavigationCom->Set_NaviIndex(iNaviIndex);

	return S_OK;
}


HRESULT COldBombRobot::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Monster_HP2"), TM->Get_CurLevel(), pLayerTag, this)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Monster_HP2_Bar"), TM->Get_CurLevel(), pLayerTag, this)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Monster_PG_Bar"), TM->Get_CurLevel(), pLayerTag, this)))
		return E_FAIL;

	CUI::UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));

	UiInfoDesc.fSizeX = 50.f;
	UiInfoDesc.fSizeY = 50.f;
	UiInfoDesc.fX = 540.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = this;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_MiniMonster"), TM->Get_CurLevel(), TEXT("Layer_UI"), &UiInfoDesc)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

void COldBombRobot::Set_PreBar(_float fTimeDelta)
{
	if (m_bReGauge)
	{
		m_fPrePatternGauge -= 500.f * fTimeDelta;
		m_fPatternGauge -= 500.f * fTimeDelta;

		if (m_fPrePatternGauge <= 0.f)
		{
			m_fPrePatternGauge = 0.f;
			m_fPatternGauge = m_fPrePatternGauge;
			m_bReGauge = false;
		}
	}

	if (m_fPreHp > m_fNowHp)
	{
		m_fMinusHp = (m_fPreHp - m_fNowHp) / m_fMaxHp;
		m_fPreHp -= m_fMinusHp * 3000.f * fTimeDelta;

		if (m_fPreHp <= m_fNowHp)
		{
			m_fPreHp = m_fNowHp;
		}
	}

	if (m_fPrePatternGauge < m_fPatternGauge)
	{
		m_fMinusPg = (m_fPatternGauge - m_fPrePatternGauge) / m_fMaxPg;
		m_fPrePatternGauge += m_fMinusPg * 1000.f * fTimeDelta;

		if (m_fPrePatternGauge >= m_fPatternGauge)
		{
			m_fPrePatternGauge = m_fPatternGauge;
		}
	}
}

COldBombRobot * COldBombRobot::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	COldBombRobot*		pInstance = new COldBombRobot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : COldBombRobot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * COldBombRobot::Clone(void * pArg)
{
	COldBombRobot*		pInstance = new COldBombRobot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : COldBombRobot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void COldBombRobot::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pDissolveTexture);
	Safe_Release(m_pEmissiveCom);
}
