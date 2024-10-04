#include "stdafx.h"
#include "..\Public\DisasterBoss.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "ToolManager.h"
#include "Monster_Manager.h"
#include "Time_Manager.h"
#include "UI.h"
#include "EffectCraw.h"
#include "Particle_Manager.h"
#include "RollRock.h"
#include "DropItem.h"
#include "Particle_Manager.h"
#include "Player01HitEffect_1.h"
#include "Player01HitEffect_2.h"
#include "Crack_Manager.h"
#include "Camera_Manager.h"

CDisasterBoss::CDisasterBoss(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CDisasterBoss::CDisasterBoss(const CDisasterBoss & rhs)
	: CCreture(rhs)
{
}

HRESULT CDisasterBoss::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CDisasterBoss::Initialize(void * pArg)
{

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;


	CCreture::CREATEINFO CreateInfo = *(CCreture::CREATEINFO*)pArg;
	_vector vPosition = XMLoadFloat4(&CreateInfo.vPosition);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	m_vTargetLook = CreateInfo.vDir;
	m_iIndex = CreateInfo.iIndex;
	m_iNaviNum = CreateInfo.iNaviNum;

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
	m_fPlayerPos = { 0.f,0.f,0.f, 1.f }; //일단 초기화
	m_vPlayerLook = { 0.f,0.f,1.f };

	m_fHurtTime = 0.02f;

	m_fMaxHp = 7000.f;
	m_fNowHp = m_fMaxHp;
	m_fStamina = 1.f;

	m_fDamage = 100.f;
	m_fPreHp = m_fNowHp;
	m_fMaxPg = m_fMaxHp * 0.4f;

	m_fDelayTime = 2.4f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Dissolve"), TEXT("Com_Texture"), (CComponent**)&m_pDissolveTexture)))
		return E_FAIL;

	if (m_iIndex == 6)
	{
		MM->Add_Haki(this);
		m_fPlusDamage = 100.f;
	}
	else
	{
		m_fPlusDamage = 0.f;
		MM->Add_Boss(this);
		MM->Add_DisasterBoss(this);
		UM->Set_BossNum(0);
		if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
			return E_FAIL;
	}

	return S_OK;
}

void CDisasterBoss::Tick(_float fTimeDelta)
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

void CDisasterBoss::LateTick(_float fTimeDelta)
{
	if (m_bCreate)
	{
		if (nullptr == m_pRendererCom)
			return;

		fTimeDelta *= m_fTMRatio;

		m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom, m_pNavigationCom);

		m_pTransformCom->Tick_Gravity(fTimeDelta, m_pNavigationCom, 30.f);

		End_Animation();


		if (m_bRHandCol)
			Choose_Col("Monster_RHand");
		if (m_bLHandCol)
			Choose_Col("Monster_LHand");
		if (m_bRollCol)
			Choose_Col("Monster_Roll_Attack");
		if (m_bHurt)
			Choose_Col("Monster_Body1");

		
		Choose_Col("Monster_Push_Range");

		Tick_Col(m_pTransformCom->Get_WorldMatrix());

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

		Render_Col(m_pRendererCom);
	}
}

HRESULT CDisasterBoss::Render()
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


		_float fGama = 1.5f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;

	//// For.Rim
	if (FAILED(m_pShaderCom->Set_RawValue("g_CamPosition", &GI->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	_float g_RimPower = 1.5f;
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
void CDisasterBoss::Render_Debug()
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

	ImGui::Checkbox("Pause", &m_bCreate);

	_float3 vSize = Get_Colliders("Monster_Body1")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Monster_Body1")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Get_Colliders("Monster_Body1")->Set_Size(vSize);
	Get_Colliders("Monster_Body1")->Set_Center(vCenter);

	ImGui::End();
}

// *[HeroJho]: 충돌이 발생할 때 호출되는 함수입니다.
void CDisasterBoss::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
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

			if (m_eCurState == ATTACK4_2)
				m_bColDelay = true;

			if (!m_bAttackCol)
				m_bAttackCol = true;

			//어택 타입
			if (m_bPlayerHit)
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
			if (m_bPush)
				m_pTransformCom->PushOtherXZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION), Desc.pMyDesc->vSize.x, XMLoadFloat4(&((CCreture*)Desc.pOther)->Get_WorldPos()), Desc.pOtherDesc->vSize.x, TM->Get_TimeDelta(), m_pNavigationCom);

		}

	}


}

void CDisasterBoss::On_Hit(CCreture* pOther)
{
	if (pOther == nullptr) //쿠마 죽었을때
	{
		m_fNowHp = -10.f;
		m_bHurt = false;
		m_bAction = false;
		XMStoreFloat3(&m_vPlayerLook, XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE::STATE_LOOK)));
		Set_State(HITFLY);
		return;
	}
	
	//hit이 한번만 처리되게끔
	if (m_bHurt && m_bHitAble )
	{

		m_bRim = true;
		m_fRimAcc = 0.f;

		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		vPos.y += 1.f;
		pOther->Create_HitEffect(vPos);

		m_bHurt = false;
		m_bAction = false;
		m_bLHandCol = false;
		m_bRHandCol = false;
		m_bRollCol = false;
		m_bRangeCol = false;

		if (m_iIndex == 6)
		{
			_uint iRandom = GI->Get_Random(0, 2);

			switch (iRandom)
			{
			case 0:
				GI->PlaySoundW(L"flesh_hit_01.wav", SD_HIT, 0.4f);
				break;
			case 1:
				GI->PlaySoundW(L"flesh_hit_02.wav", SD_HIT, 0.4f);
				break;
			case 2:
				GI->PlaySoundW(L"flesh_hit_03.wav", SD_HIT, 0.4f);
				break;
			}
		}

		else
			GI->PlaySoundW(L"SwordHit.ogg", SD_HIT, 0.4f);


		if (m_iIndex != 6)
			m_fNowHp -= (pOther->Get_Damage() - m_fStamina);

		if (MM->Get_MonsterCount() == 1 && m_fNowHp <= 0.f  && m_iIndex != 6)
		{
			TIM->Fix_All(0.14f, 5.f);
			CRM->Get_Cam()->Fov(-3.f, 0.15f, 3.f);
			if (!m_bFightEnd)
			{
				GI->PlaySoundW(L"slowmotion_finish.wav", SD_UI, 0.6f);
				m_bFightEnd = true;
			}
		}

		if (!m_bReGauge && !PM->Get_PatternSkill() && m_iIndex != 6)
			m_fPatternGauge += (pOther->Get_Damage() * 1.5f);

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

void CDisasterBoss::Create_HitEffect(_float4 vWorldPos)
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

void CDisasterBoss::Control_DisasterBoss()
{
	m_fAttackTime = 0.f;
	m_bMove = true;
	m_bSpawn = true;
	m_bAttack = false;
	m_bOneCheck = false;
	m_bHitAble = true;
	m_bAction = true;
}

void CDisasterBoss::Delay_Spawn(_float fTimeDelta)
{
	if (!m_bCreate)
	{
		m_fCreateAcc += 1.f * fTimeDelta;

		if (m_fCreateAcc >= (m_fDelayTime - 0.25f) && !m_bSpawnEffect)
		{
			m_bSpawnEffect = true;
			GI->PlaySoundW(L"se_enemy_spawn3.wav", SD_DISASTERBOSS, 0.5f); // 스폰할때 울부짖는거랑 안겹치기
			PTM->Create_InstanceParticle(L"MonsterAppear", m_pTransformCom->Get_WorldFloat4x4());
			PTM->Create_InstanceParticle(L"MonsterAppear_2", m_pTransformCom->Get_WorldFloat4x4());
		}


		if (m_fCreateAcc >= m_fDelayTime)
			m_bCreate = true;
	}
}

void CDisasterBoss::Set_State(STATE eState) //end 애니메이션에서
{
	//상태 보간 하는 함수
	if (m_eNextState == eState)
		return;

	m_eNextState = eState;

	switch (m_eNextState)
	{
		//다음 상태로 넘어갈때 해줘야하는것들
		//애니메이션x
	case Client::CDisasterBoss::ATTACK1:
		m_eHitType = TYPE_HIT;
		m_fPushPower = 10.f;
		m_fAirPower = 0.f;
		m_fDamage = 70.f + m_fPlusDamage;
		m_bAttackCol = false;
		m_bAttack1Col = false;
		m_bAttack1On = false;
		break;
	case Client::CDisasterBoss::ATTACK2:
		m_eHitType = TYPE_AWAY;
		m_fPushPower = 15.f;
		m_fAirPower = 5.f;
		m_fDamage = 110.f + m_fPlusDamage;
		m_bAttackCol = false;
		m_bAttack2On = false;
		break;
	case Client::CDisasterBoss::ATTACK3:
		m_eHitType = TYPE_HIT;
		m_fPushPower = 10.f;
		m_fAirPower = 0.f;
		m_fDamage = 90.f + m_fPlusDamage;
		m_bAttackCol = false;
		m_bAttack3Col_1 = false;
		m_bAttack3Col_2 = false;
		break;
	case Client::CDisasterBoss::ATTACK4_1:
		GI->PlaySoundW(L"se_EN0003_attack5_1.wav", SD_DISASTERBOSS, 1.f);
		m_eHitType = TYPE_SPIN;
		m_fPushPower = 15.f;
		m_fAirPower = 10.f;
		m_fDamage = 110.f + m_fPlusDamage;
		m_bAttack4_1Col = false;
		m_bPlayerHit = true;
		m_bParticle = true;
		break;
	case Client::CDisasterBoss::AVOIDATTACK:
		m_eHitType = TYPE_SPIN;
		m_fPushPower = 15.f;
		m_fAirPower = 9.f;
		m_fDamage = 150.f + m_fPlusDamage;
		m_bAttackCol = false;
		m_bAttack4_Effect = false;
		m_bAvoidSound = false;
		break;
	case Client::CDisasterBoss::BOUNCE:
		m_bFly = true;
		break;
	case Client::CDisasterBoss::HITBACK:
		Reset_Collider();
		m_bFly = false;
		break;
	case Client::CDisasterBoss::HITFLY:
		m_bFly = true;
		Reset_Collider();
		if (m_fHAirPower == 0.f)
			m_pTransformCom->ResetAndJump(1.f);
		else
			m_pTransformCom->ResetAndJump(m_fHAirPower);
		break;
	case Client::CDisasterBoss::HITFRONT:
		m_bFly = false;
		Reset_Collider();
		break;
	case Client::CDisasterBoss::HITDOWN:
		m_bFly = true;
		break;
	case Client::CDisasterBoss::HITUP:
		m_bFly = true;
		Reset_Collider();
		break;
	case Client::CDisasterBoss::HITSPIN:
		m_bFly = true;
		Reset_Collider();
		break;
	case Client::CDisasterBoss::HITHORI:
		m_bFly = true;
		//HitAwayPower_Cehck();
		Reset_Collider();
		break;
	case Client::CDisasterBoss::HITSLAPDOWN:
		m_bFly = true;
		Reset_Collider();
		break;
	case Client::CDisasterBoss::DOWN:
		m_bFly = true;
		break;
	case Client::CDisasterBoss::GETUP:
		m_bFly = false;
		break;
	case Client::CDisasterBoss::SPAWN:
		break;
	case Client::CDisasterBoss::IDLE:
		if(m_iIndex != 4)
			Control_DisasterBoss();
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
	case Client::CDisasterBoss::STUN:
		break;
	case Client::CDisasterBoss::STUNRECOVER:
		break;
	case Client::CDisasterBoss::STRAFELEFT:
		m_bStrafeSound1 = false;
		m_bStrafeSound2 = false;
		break;
	case Client::CDisasterBoss::STRAFERIGHT:
		m_bStrafeSound1 = false;
		m_bStrafeSound2 = false;
		break;
	case Client::CDisasterBoss::RUN:
		m_bRunSound1 = false;
		m_bRunSound2 = false;
		break;
	case Client::CDisasterBoss::WALKBACK:
		m_bHurt = true;
		m_bHitAble = true;
		m_bBackWalkSound1 = false;
		m_bBackWalkSound2 = false;
		break;
	case Client::CDisasterBoss::WALKFRONT:
		m_bWalkSound1 = false;
		m_bWalkSound2 = false;
		break;
	case Client::CDisasterBoss::TURNLEFT:
		break;
	case Client::CDisasterBoss::TURNRIGHT:
		break;
	case Client::CDisasterBoss::DEATHSTART:
		break;
	}


	m_pModelCom->SetNextIndex(m_eNextState);
	m_pModelCom->SetChangeBool(true);
	m_eCurState = m_eNextState; //보간시작할때 상태를 바꿔줌

}

void CDisasterBoss::Hit_State(CCreture * pOther)
{
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


void CDisasterBoss::End_Animation()
{
	if (m_pModelCom->GetAniEnd())
	{
		//애니메이션 끝났을때
		switch (m_eCurState)
		{
			//다음 상태로 넘어갈때 해줘야하는것들
			//애니메이션x
		case Client::CDisasterBoss::SPAWN:
			Set_State(IDLE);
			break;
		case Client::CDisasterBoss::IDLE:
			if (m_bMove)
			{
				m_bOneCheck = false;
				m_bHitAble = true;
				m_bAction = true;
				m_bBackWalk = false;
			}
			break;
		case Client::CDisasterBoss::RUN:
			m_bOneCheck = false;
			m_bRunSound1 = false;
			m_bRunSound2 = false;
			break;
		case Client::CDisasterBoss::WALKBACK:
		{
			if (m_bBackWalk)
			{
				m_bBackWalk = false;
				m_bHitAble = true;
				m_bBackWalkSound1 = false;
				m_bBackWalkSound2 = false;
				Set_State(WALKBACK);
			}
			else
				RandomWalk();
		}
		break;
		case Client::CDisasterBoss::WALKFRONT:
			break;
		case Client::CDisasterBoss::STRAFELEFT:
			m_bAttack = false;
			m_bOneCheck = false;
			m_bBackWalk = false;
			Set_State(STATE::IDLE);
			break;
		case Client::CDisasterBoss::STRAFERIGHT:
			m_bAttack = false;
			m_bOneCheck = false;
			m_bBackWalk = false;
			Set_State(STATE::IDLE);
			break;
		case Client::CDisasterBoss::ATTACK1:
			m_bBackWalk = true;
			m_fAttackTime = 0.f;
			Set_State(STATE::WALKBACK);
			break;
		case Client::CDisasterBoss::ATTACK2:
			m_bBackWalk = true;
			Set_State(STATE::WALKBACK);
			break;
			break;
		case Client::CDisasterBoss::ATTACK3:
			m_bBackWalk = true;
			m_fAttackTime = 0.f;
			Set_State(STATE::WALKBACK);
			break;
		case Client::CDisasterBoss::ATTACK4_1:
			m_eHitType = TYPE_SPIN;
			m_fPushPower = 7.f;
			m_fAirPower = 5.f;
			m_fDamage = 115.f + m_fPlusDamage;
			m_pModelCom->Set_AnimIndex(STATE::ATTACK4_2);
			m_eCurState = STATE::ATTACK4_2;
			m_bPlayerHit = true;
			m_fColDelayTime = 0.f;
			m_bColDelay = false;
			break;
		case Client::CDisasterBoss::ATTACK4_2:
			m_pModelCom->Set_AnimIndex(STATE::ATTACK4_3);
			m_eCurState = STATE::ATTACK4_3;
			m_bRollCol = false;
			break;
		case Client::CDisasterBoss::ATTACK4_3:
			m_bAttack4On = false;
			m_bBackWalk = true;
			m_fAttackTime = 0.f;
			Set_State(STATE::WALKBACK);
			break;
		case Client::CDisasterBoss::AVOIDATTACK:
			m_bAvoidAttackOn = false;
			m_bAction = true;
			m_bOneCheck = false;
			m_bHitAble = true;
			m_bAttack = false;
			m_bAvoidAttack = false;
			m_fAttackTime = 0.f;
			Set_State(IDLE);
			break;
		case Client::CDisasterBoss::HITBACK:
			if (m_bDeadStart)
			{
				Set_State(STUN);
			}
			else
			{
				if (m_bAvoidAttack)
				{
					m_bOneCheck = false;
					AvoidAttack();
				}
				else
				{
					m_fAttackTime = 0.f;
					m_bAttack = false;
					m_bOneCheck = false;
					Set_State(IDLE);
				}
			}
			break;
		case Client::CDisasterBoss::HITFRONT:
			if (m_bDeadStart)
			{
				Set_State(STUN);
			}
			else
			{
				if (m_bAvoidAttack)
				{
					m_bOneCheck = false;
					AvoidAttack();
				}
				else
				{
					m_fAttackTime = 0.f;
					m_bAttack = false;
					m_bOneCheck = false;
					Set_State(IDLE);
				}
			}
			break;
		case Client::CDisasterBoss::HITFLY:
			Set_State(HITDOWN);
			break;
		case Client::CDisasterBoss::HITDOWN:
			break;
		case Client::CDisasterBoss::HITSPIN:
			Set_State(HITDOWN);
			break;
		case Client::CDisasterBoss::HITHORI:
			Set_State(HITDOWN);
			break;
		case Client::CDisasterBoss::HITUP:
			Set_State(HITDOWN);
			break;
		case Client::CDisasterBoss::HITSLAPDOWN:
			Set_State(HITDOWN);
			break;
		case Client::CDisasterBoss::BOUNCE:
			Set_State(DOWN);
			break;
		case Client::CDisasterBoss::DOWN:
			if (!m_bDeadStart)
				Set_State(GETUP);
			else
			{
				m_bDissolve = true;
			}
			break;
		case Client::CDisasterBoss::GETUP:
			if (m_bAvoidAttack)
			{
				m_bOneCheck = false;
				AvoidAttack();
			}
			else
			{
				m_fAttackTime = 0.f;
				m_bAttack = false;
				m_bOneCheck = false;
				m_bAction = true;
				m_bHitAble = true;
				Set_State(IDLE);
			}
			break;
		case Client::CDisasterBoss::STUN:
			if (!m_bDeadStart)
				Set_State(STUNRECOVER);
			else
			{
				m_bDissolve = true;
			}
			break;
		case Client::CDisasterBoss::STUNRECOVER:
			Set_State(IDLE);
			break;
		case Client::CDisasterBoss::TURNLEFT:
			break;
		case Client::CDisasterBoss::TURNRIGHT:
			break;
		case Client::CDisasterBoss::DEATHSTART:
			break;
		default:
			break;

		}

	}
}


void CDisasterBoss::Update(_float fTimeDelta)
{
	m_bRangeCol = false;
	m_bPush = true;

	if (!m_pModelCom->GetChangeBool())
	{
		switch (m_eCurState)
		{
			//다음 상태로 넘어갈때 해줘야하는것들
			//애니메이션x

		case Client::CDisasterBoss::SPAWN:
			Spawn_Update(fTimeDelta);
			break;
		case Client::CDisasterBoss::IDLE:
			LookAtPlayer();
			m_bRangeCol = true;
			break;
		case Client::CDisasterBoss::RUN:
			Run_Update(fTimeDelta);
			break;
		case Client::CDisasterBoss::WALKBACK: //텀주기 위한 상태, 뒤로 살짝 물러남, 애니메이션 자체가 뒤로 물러나므로 트랜스폼함수 필요없음
			WalkBack_Update(fTimeDelta);
			break;
		case Client::CDisasterBoss::WALKFRONT:
			Walk_Update(fTimeDelta);
			break;
		case Client::CDisasterBoss::STRAFELEFT:
			Strafe_Update(fTimeDelta);
		case Client::CDisasterBoss::STRAFERIGHT:
			Strafe_Update(fTimeDelta);
			break;
		case Client::CDisasterBoss::ATTACK1:
			Attack1_Update(fTimeDelta);
			break;
		case Client::CDisasterBoss::ATTACK2:
			Attack2_Update(fTimeDelta);
			break;
		case Client::CDisasterBoss::ATTACK3:
			Attack3_Update(fTimeDelta);
			break;
		case Client::CDisasterBoss::ATTACK4_1:
			Attack4_1Update(fTimeDelta);
			break;
		case Client::CDisasterBoss::ATTACK4_2:
			Attack4_2Update(fTimeDelta);
			break;
		case Client::CDisasterBoss::ATTACK4_3:
			Attack4_3Update(fTimeDelta);
			break;
		case Client::CDisasterBoss::AVOIDATTACK:
			AvoidAttack_Update(fTimeDelta);
			break;
		case Client::CDisasterBoss::HITBACK:
			HitBack_Update(fTimeDelta);
			break;
		case Client::CDisasterBoss::HITFRONT:
			HitFront_Update(fTimeDelta);
			break;
		case Client::CDisasterBoss::HITFLY:
			HitFly_Update(fTimeDelta);
			break;
		case Client::CDisasterBoss::HITDOWN:
			HitDown_Update(fTimeDelta);
			break;
		case Client::CDisasterBoss::HITSPIN:
			HitSpin_Update(fTimeDelta);
			break;
		case Client::CDisasterBoss::HITHORI:
			m_bAction = false;
			HitHori_Update(fTimeDelta);
			break;
		case Client::CDisasterBoss::HITUP:
			HitUp_Update(fTimeDelta);
			break;
		case Client::CDisasterBoss::HITSLAPDOWN:
			HitSlapdown_Update(fTimeDelta);
			break;
		case Client::CDisasterBoss::BOUNCE:
			break;
		case Client::CDisasterBoss::DOWN:
			break;
		case Client::CDisasterBoss::GETUP:
			break;
		case Client::CDisasterBoss::STUN:
			if (!m_bDeadStart)
				m_bRangeCol = true;
			break;
		case Client::CDisasterBoss::STUNRECOVER:
			break;
		case Client::CDisasterBoss::TURNLEFT:
			break;
		case Client::CDisasterBoss::TURNRIGHT:
			break;
		case Client::CDisasterBoss::DEATHSTART:
			break;
		default:
			break;
		}

	}

}

void CDisasterBoss::Set_Dir()
{
	//플레이어 포스 받아오기 미리 받아둘지

	//내가 바라봐야할 룩벡터 구하기, 플레이어 포스에서 내포스 빼고 노멀라이즈
	_vector vLook = XMVector3Normalize(XMLoadFloat4(&m_fPlayerPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	XMStoreFloat3(&m_vTargetLook, vLook);


}


void CDisasterBoss::Distance_BattlePattern(_float fTimeDelta)
{
	m_fPlayerPos = PM->Get_PlayerPos();

	m_fDistance = m_pTransformCom->TargetDistance(XMLoadFloat4(&m_fPlayerPos)); //타겟과 나의 거리 계속 계산

	if (m_bSpawn && m_bAction) // 스폰이 끝났을때 시작
		ActionToPlayer(fTimeDelta);

	RegretToPlayer(fTimeDelta);

}

void CDisasterBoss::AttackPattern(_float fTimeDelta)
{
	m_fAttackTime += 0.1f * fTimeDelta;

	if (m_fAttackTime > 0.15f)
	{
		m_bAttack = true;

	
		CloseAttack(fTimeDelta);

	}
}

void CDisasterBoss::ActionToPlayer(_float fTimeDelta)
{
	if (m_fDistance <= 50.f)
	{
		//거리에 따라서 걸을지 뛸지
		if (!m_bAttack)
			Set_State(STATE::IDLE);

		AttackPattern(fTimeDelta);

	}
	else
	{
		Set_State(RUN);
		return;
	}
}

void CDisasterBoss::RegretToPlayer(_float fTimeDelta)
{
	if (m_bSpawn && !m_bMove)
	{
		Set_State(IDLE);
	}
}

void CDisasterBoss::Check_Rim(_float fTimeDelta)
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
		m_iShaderPath = 12;// 4;// 5;
	}

}

void CDisasterBoss::Check_Death(_float fTimeDelta)
{
	if (!m_bMinusCount && m_fNowHp <= 0.f)
	{
		m_bMinusCount = true;
		MM->Minus_MonsterCount();

		if (6 != m_iIndex)
		{
			//Create_Item();
			MM->Event_Stage3_2(); // 보스를 무조건 마지막에 죽여야한다.
		}
		else
		{
			MM->Event_Stage4_2();
		}

	}

	if (!m_pTransformCom->Get_Air() && m_fNowHp <= 0.f && !m_bDeadStart)
	{

		m_bAction = false;
		m_bHitAble = false;
		m_bRangeCol = false;
		m_bLHandCol = false;
		m_bRHandCol = false;
		m_bBodyCol = false;
		m_bRollCol = false;
		m_bDeadStart = true;
		GI->PlaySoundW(L"se_enemy_death3.wav", SD_DISASTERBOSS, 1.f);
	}

	if (m_bDissolve)
	{
		
		m_iShaderPath = 6;
		m_fDissolveAcc += 0.3f * fTimeDelta;
		if (m_fDissolveAcc >= 1.f)
		{
			m_fDissolveAcc = 1.f;
			Set_Dead();
		}

	}
}

void CDisasterBoss::Check_Avoid()
{
	if (m_fPrePatternGauge >= m_fMaxPg)
	{
		m_bAvoidAttack = true;
		m_bAction = false;
		m_bHitAble = false;
	}
}

void CDisasterBoss::HitAwayPower_Cehck()
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
		m_fHPushPower -= 10.f;
		break;
	case 3:
		m_fHPushPower -= 15.f;
		break;
	case 4:
		m_fHPushPower -= 20.f;
		break;
	default:
		break;
	}
}

void CDisasterBoss::CloseAttack(_float fTimeDelta)
{
	if (!m_bOneRandom)
	{
		m_iRandomAttack = GI->Get_Random(1, 5);
		m_bOneRandom = true;
	}

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
	case 4:
		Attack4(fTimeDelta);
		break;
	case 5:
		Attack4(fTimeDelta);
		break;
	default:
		break;
	}
}

void CDisasterBoss::Reset_Collider()
{
	m_bAttack1Col = false;
	m_bAttack2Col = false;
	m_bAttack3Col_1 = false;
	m_bAttack3Col_2 = false;
	m_bAttack4_1Col = false;

}

void CDisasterBoss::RandomWalk()
{
	m_iRandom = GI->Get_Random(1, 3);
	switch (m_iRandom)
	{
	case 1:
		Set_State(STATE::STRAFELEFT);
		break;
	case 2:
		Set_State(STATE::STRAFERIGHT);
		break;
	case 3:
		m_bAttack = false;
		m_bOneCheck = false;
		break;
	}
}

void CDisasterBoss::Create_Effect(EFFECT eEffect)
{
	switch (eEffect)
	{
	case ATTACK1_LEFT:
	{
		CEffectCraw::CRAWINFO* Info = new CEffectCraw::CRAWINFO;
		Info->vColor = { 0.886f,0.309f,0.27f,1.f };
		Info->vColor2 = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 2.f,1.f };
		Info->vRatioSpeed = { 8.f,0.f };
		Info->fScale = { 24.f,24.f, 24.f };
		Info->fSpeed = -1.f;
		Info->iIndex = 1;
		_matrix Effectmatrix;
		Effectmatrix.r[0] = { 0.778f, -0.73f, -16.966f, 0.f };
		Effectmatrix.r[1] = { -11.637f, -12.392f, 0.f, 0.f };
		Effectmatrix.r[2] = { -12.368f, 11.614f, -1.067f, 0.f };
		Effectmatrix.r[3] = { -0.4f, 1.31f, 1.2f, 1.f };
		XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		Info->fBlurPower = 1.5f;
		(GI->Add_GameObjectToLayer(L"EffectCraw", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info));

	}
	break;
	case ATTACK1_RIGHT:
	{
		CEffectCraw::CRAWINFO* Info = new CEffectCraw::CRAWINFO;
		Info->vColor = { 0.886f,0.309f,0.27f,1.f };
		Info->vColor2 = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 2.f,1.f };
		Info->vRatioSpeed = { 8.f,0.f };
		Info->fScale = { 24.f,24.f, 24.f };
		Info->fSpeed = -2.f;
		Info->iIndex = 1;
		_matrix Effectmatrix;
		Effectmatrix.r[0] = { 0.f, 0.f, -16.996f, 0.f };
		Effectmatrix.r[1] = { -10.004f, 13.769f, 0.f, 0.f };
		Effectmatrix.r[2] = { 13.8f, 10.026f, 0.f, 0.f };
		Effectmatrix.r[3] = { 0.42f, 1.44f, 1.2f, 1.f };
		XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		Info->fBlurPower = 1.5f;
		(GI->Add_GameObjectToLayer(L"EffectCraw", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info));

	}
	break;
	case ATTACK3_LEFT:
	{
		CEffectCraw::CRAWINFO* Info = new CEffectCraw::CRAWINFO;
		Info->vColor = { 0.886f,0.309f,0.27f,1.f };
		Info->vColor2 = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 2.f,1.f };
		Info->vRatioSpeed = { 5.f,0.f };
		Info->fScale = { 30.f,30.f, 30.f };
		Info->fSpeed = 2.5f;
		Info->iIndex = 1;
		_matrix Effectmatrix;
		Effectmatrix.r[0] = { 1.907f, 0.619f, 15.873f, 0.f };
		Effectmatrix.r[1] = { -4.944f, 15.216f, 0.f, 0.f };
		Effectmatrix.r[2] = { -15.096f, -4.905f, 2.005f, 0.f };
		Effectmatrix.r[3] = { 0.f, 0.95f, 0.8f, 1.f };
		XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		Info->fBlurPower = 1.5f;
		(GI->Add_GameObjectToLayer(L"EffectCraw", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info));

	}
	break;

	case ATTACK3_RIGHT:
	{
		CEffectCraw::CRAWINFO* Info = new CEffectCraw::CRAWINFO;
		Info->vColor = { 0.886f,0.309f,0.27f,1.f };
		Info->vColor2 = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 2.f,1.f };
		Info->vRatioSpeed = { 5.f,0.f };
		Info->fScale = { 30.f,30.f, 30.f };
		Info->fSpeed = -2.f;
		Info->iIndex = 1;
		_matrix Effectmatrix;
		Effectmatrix.r[0] = { 1.989f, 0.251f, -15.873f, 0.f };
		Effectmatrix.r[1] = { -2.005f, 15.873f, 0.f, 0.f };
		Effectmatrix.r[2] = { 15.748f, 1.989f, 2.005f, 0.f };
		Effectmatrix.r[3] = { 1.44f, 1.35f, 0.8f, 1.f };
		XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		Info->fBlurPower = 1.f;
		(GI->Add_GameObjectToLayer(L"EffectCraw", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info));

	}
	break;
	case AVOIDATTACK_LEFT:
	{
		CEffectCraw::CRAWINFO* Info = new CEffectCraw::CRAWINFO;
		Info->vColor = { 0.886f,0.309f,0.27f,1.f };
		Info->vColor2 = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 2.f,1.f };
		Info->vRatioSpeed = { 5.f,0.f };
		Info->fScale = { 34.f,34.f, 34.f };
		Info->fSpeed = 2.5f;
		Info->iIndex = 1;
		_matrix Effectmatrix;
		Effectmatrix.r[0] = { 1.969f, -0.375f, 15.873f, 0.f };
		Effectmatrix.r[1] = { 2.998f, 15.716f, 0.f, 0.f };
		Effectmatrix.r[2] = { -15.592f, 2.97f, 2.005f, 0.f };
		Effectmatrix.r[3] = { -0.4f, 0.5f, 1.79f, 1.f };
		XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		Info->fBlurPower = 1.5f;
		(GI->Add_GameObjectToLayer(L"EffectCraw", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info));
	}
	break;
	default:
		break;
	}
}

void CDisasterBoss::Create_Particle(_float fTimeDelta)
{
	
	m_fParticleAcc += 1.f * fTimeDelta;

	if (m_fParticleAcc > 0.1f)
		m_bParticle = true;

	if (m_bParticle)
	{
		m_bParticle = false;
		m_fParticleAcc = 0.f;
		(PTM->Create_InstanceParticle(L"BossRollParticle", m_pTransformCom->Get_WorldFloat4x4()));

		if(m_iIndex != 6)
			Create_ParticleRock();
	}

	m_fParticleRockAcc += 0.1f * fTimeDelta;

	if (m_fParticleRockAcc > 0.4f)
		m_bParticleRock = true;

	if (m_bParticleRock)
	{
		m_bParticleRock = false;
		m_fParticleRockAcc = 0.f;
		
		if (m_iIndex != 6)
			Create_ParticleRock();
	}

}

HRESULT CDisasterBoss::Create_ParticleRock()
{
	CRollRock::ROLLROCKINFO RockInfo;
	RockInfo.fScale = GI->Get_FloatRandom(2.1f, 3.4f);
	RockInfo.bJump = false;
	RockInfo.iIndex = 1;
	RockInfo.iNaviNum = m_pNavigationCom->Get_CurCellIndex();
	_matrix Rockmatrix;
	Rockmatrix.r[0] = { 0.662f, 1.673f, 0.f, 0.f };
	Rockmatrix.r[1] = { -1.673f, 0.662f, 0.f, 0.f };
	Rockmatrix.r[2] = { 0.f, 0.f, 1.799f, 0.f };
	Rockmatrix.r[3] = { -0.61f, 0.1f, 0.6f, 1.f };
	XMStoreFloat4x4(&RockInfo.vWorldMatrix, Rockmatrix * m_pTransformCom->Get_WorldMatrix());

	if (FAILED(GI->Add_GameObjectToLayer(L"RollRock", GI->Get_Cur_LevelIndex(), L"Layer_MonsterParticle", &RockInfo)))
		return E_FAIL;

	RockInfo.fScale = GI->Get_FloatRandom(2.5f, 3.8f);
	RockInfo.bJump = false;
	RockInfo.iIndex = 2;

	Rockmatrix.r[0] = { -0.736f, 0.f, 1.859f, 0.f };
	Rockmatrix.r[1] = { 1.629f, -0.963f, 0.645f, 0.f };
	Rockmatrix.r[2] = { 0.895f, 1.752f, 0.354f, 0.f };
	Rockmatrix.r[3] = { 0.77f, 0.1f, 1.29f, 1.f };
	XMStoreFloat4x4(&RockInfo.vWorldMatrix, Rockmatrix * m_pTransformCom->Get_WorldMatrix());

	if (FAILED(GI->Add_GameObjectToLayer(L"RollRock", GI->Get_Cur_LevelIndex(), L"Layer_MonsterParticle", &RockInfo)))
		return E_FAIL;

	RockInfo.fScale = GI->Get_FloatRandom(1.5f, 2.4f);
	RockInfo.bJump = false;
	RockInfo.iIndex = 3;

	Rockmatrix.r[0] = { 0.963f, 0.f, -1.752f, 0.f };
	Rockmatrix.r[1] = { 0.f, 2.f, 0.f, 0.f };
	Rockmatrix.r[2] = { 1.752f, 0.f, 0.963f, 0.f };
	Rockmatrix.r[3] = { -0.74f, 0.03f, 1.92f, 1.f };
	XMStoreFloat4x4(&RockInfo.vWorldMatrix, Rockmatrix * m_pTransformCom->Get_WorldMatrix());

	if (FAILED(GI->Add_GameObjectToLayer(L"RollRock", GI->Get_Cur_LevelIndex(), L"Layer_MonsterParticle", &RockInfo)))
		return E_FAIL;

	RockInfo.fScale = GI->Get_FloatRandom(1.5f, 2.4f);
	RockInfo.bJump = false;
	RockInfo.iIndex = 4;

	Rockmatrix.r[0] = { 0.125f, 0.f, 1.996f, 0.f };
	Rockmatrix.r[1] = { -1.366f, 1.457f, 0.085f, 0.f };
	Rockmatrix.r[2] = { -1.455f, -1.369f, 0.091f, 0.f };
	Rockmatrix.r[3] = { 1.2f, 0.24f, 0.49f, 1.f };
	XMStoreFloat4x4(&RockInfo.vWorldMatrix, Rockmatrix * m_pTransformCom->Get_WorldMatrix());

	if (FAILED(GI->Add_GameObjectToLayer(L"RollRock", GI->Get_Cur_LevelIndex(), L"Layer_MonsterParticle", &RockInfo)))
		return E_FAIL;

	RockInfo.fScale = GI->Get_FloatRandom(1.6f, 2.4f);
	RockInfo.bJump = true;
	RockInfo.iIndex = 2;
	RockInfo.fJumpPower = GI->Get_FloatRandom(1.f, 1.7f);

	Rockmatrix.r[0] = { 0.942f, 0.f, 0.742f, 0.f };
	Rockmatrix.r[1] = { 0.f, 1.2f, 0.f, 0.f };
	Rockmatrix.r[2] = { -0.742f, 0., 0.942f, 0.f };
	Rockmatrix.r[3] = { -0.53f, 0.f, 0.47f, 1.f };
	XMStoreFloat4x4(&RockInfo.vWorldMatrix, Rockmatrix * m_pTransformCom->Get_WorldMatrix());

	if (FAILED(GI->Add_GameObjectToLayer(L"RollRock", GI->Get_Cur_LevelIndex(), L"Layer_MonsterParticle", &RockInfo)))
		return E_FAIL;

	RockInfo.fScale = GI->Get_FloatRandom(1.8f, 2.2f);
	RockInfo.bJump = true;
	RockInfo.iIndex = 4;
	RockInfo.fJumpPower = GI->Get_FloatRandom(1.5f, 2.1f);

	Rockmatrix.r[0] = { 1.3f, 0.f, 0.f, 0.f };
	Rockmatrix.r[1] = { 0.f, 1.3f, 0.f, 0.f };
	Rockmatrix.r[2] = { 0.f, 0., 1.3f, 0.f };
	Rockmatrix.r[3] = { 0.66f, 0.f, 0.f, 1.f };
	XMStoreFloat4x4(&RockInfo.vWorldMatrix, Rockmatrix * m_pTransformCom->Get_WorldMatrix());

	if (FAILED(GI->Add_GameObjectToLayer(L"RollRock", GI->Get_Cur_LevelIndex(), L"Layer_MonsterParticle", &RockInfo)))
		return E_FAIL;

	return S_OK;
}

void CDisasterBoss::Attack1(_float fTimeDelta)
{
	if (m_fDistance >= 4.f)
	{
		if (!m_bOneCheck)
			Set_State(STATE::RUN);
	}
	else
	{
		if (!m_bOneCheck)
		{
			m_bOneCheck = true;
			m_bOneRandom = false;
			Set_State(STATE::ATTACK1);

		}
	}

}

void CDisasterBoss::Attack2(_float fTimeDelta)
{
	if (m_fDistance >= 5.f)
	{
		if (!m_bOneCheck)
			Set_State(STATE::RUN);
	}
	else
	{
		if (!m_bOneCheck)
		{
			m_bOneCheck = true;
			m_bOneRandom = false;
			Set_State(STATE::ATTACK2);

		}
	}
}

void CDisasterBoss::Attack3(_float fTimeDelta)
{
	if (m_fDistance >= 3.f)
	{
		if (!m_bOneCheck)
			Set_State(STATE::RUN);
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

void CDisasterBoss::Attack4(_float fTimeDelta)
{
	if (m_fDistance >= 10.f)
	{
		if (!m_bOneCheck)
			Set_State(STATE::RUN);
	}
	else
	{
		if (!m_bOneCheck)
		{
			m_bOneCheck = true;
			m_bOneRandom = false;
			Set_State(STATE::ATTACK4_1);

		}
	}
}

void CDisasterBoss::AvoidAttack()
{
	if (!m_bOneCheck)
	{

		_matrix Effectmatrix;
		Effectmatrix.r[0] = { 1.f, 0.f, 0.f, 0.f };
		Effectmatrix.r[1] = { 0.f, 1.f, 0.f, 0.f };
		Effectmatrix.r[2] = { 0.f, 0.f, 1.f, 0.f };
		Effectmatrix.r[3] = { 0.f, 1.51f, 1.82f, 1.f };

		_float4x4 PatternMatrx;

		XMStoreFloat4x4(&PatternMatrx, Effectmatrix * m_pTransformCom->Get_WorldMatrix());

		(PTM->Create_AlphaParticle(L"PatternGaugeCircle", PatternMatrx));
		(PTM->Create_AlphaParticle(L"PatternGaugeLine", PatternMatrx));
		(PTM->Create_AlphaParticle(L"PatternGaugeFlare", PatternMatrx));

		LookAtPlayer();

		GI->PlaySoundW(L"se_enemy_warning.wav", SD_DISASTERBOSS, 0.6f);

		m_bOneCheck = true;
		Set_State(STATE::AVOIDATTACK);
		m_bReGauge = true;
	}
}

void CDisasterBoss::Spawn_Update(_float fTimeDelta)
{
	LookAtPlayer();
	m_bRangeCol = true;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bRoarSound1)
	{
		m_bRoarSound1 = true;
		//
		GI->PlaySoundW(L"se_EN0003_roar_1.wav", SD_DISASTERBOSS, 0.5f);

	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bRoarSound2)
	{
		m_bRoarSound2 = true;
		//
		GI->PlaySoundW(L"se_EN0101_roar_1.wav", SD_DISASTERBOSS, 0.5f);

	}
		
	
}

void CDisasterBoss::Attack1_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	m_bPush = false;
	m_bRHandCol = false;
	m_bLHandCol = false;
	m_bPlayerHit = true;

	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(0))
		LookAtPlayer();

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(1) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(2) && !m_bAttackCol)
	{
		if (!m_bAttack1On)
		{
			GI->PlaySoundW(L"se_EN0003_attack1.wav", SD_DISASTERBOSS, 1.f);
			m_bAttack1On = true;
		}

		m_bRHandCol = true;
		m_bLHandCol = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bAttack1Col)
	{
		Create_Effect(ATTACK1_LEFT);
		Create_Effect(ATTACK1_RIGHT);
		m_bAttack1Col = true;

	}
}

void CDisasterBoss::Attack2_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	m_bPush = false;
	m_bRHandCol = false;
	m_bPlayerHit = true;

	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(0))
		LookAtPlayer();

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(1) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(2) && !m_bAttackCol)
	{
		if (!m_bAttack2On)
		{
			GI->PlaySoundW(L"se_EN0003_attack2.wav", SD_DISASTERBOSS, 1.f);
			m_bAttack2On = true;
		}

		m_bRHandCol = true;
	}
}

void CDisasterBoss::Attack3_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	m_bPush = false;
	m_bLHandCol = false;
	m_bRHandCol = false;
	m_bPlayerHit = true;

	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(0))
		LookAtPlayer();

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1))
		LookAtPlayer();
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bAttack3Col_1)
	{
		GI->PlaySoundW(L"se_EN0003_attack3.wav", SD_DISASTERBOSS, 1.f);
		Create_Effect(ATTACK3_RIGHT);
		m_bAttack3Col_1 = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(4) && !m_bAttack3Col_2)
	{
		GI->PlaySoundW(L"se_EN0003_attack3.wav", SD_DISASTERBOSS, 1.f);
		Create_Effect(ATTACK3_LEFT);
		m_bAttack3Col_2 = true;
	}
	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(2) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(3) && !m_bAttackCol)
	{
		m_bRHandCol = true;
	}

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(4) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(5) && !m_bAttackCol)
	{
		m_bLHandCol = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(6))
		m_bAttackCol = false;
}

void CDisasterBoss::Attack4_1Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	m_bPush = false;
	m_bRollCol = false;
	m_bPlayerHit = true;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bAttack4_1Col)
	{
		m_bRollCol = true;
		m_bAttack4_1Col = true;
	}

	LookAtPlayer();

}

void CDisasterBoss::Attack4_2Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	m_bPush = false;
	m_bRollCol = true;

	Create_Particle(fTimeDelta);


	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(0))
		LookAtPlayer(0.7f);

	if (m_bColDelay)
	{
		if (!m_bAttack4On)
		{
			GI->PlaySoundW(L"se_EN0003_attack5_3.wav", SD_DISASTERBOSS, 1.f);
			GI->PlaySoundW(L"se_EN0003_roar_1.wav", SD_TRAININGBOT1, 0.8f);
			m_bAttack4On = true;
		}
		m_bPlayerHit = false;
		m_fColDelayTime += 0.1f * fTimeDelta;

		if (m_fColDelayTime > 2.f)
		{
			m_fColDelayTime = 0.f;
			m_bPlayerHit = true;
			m_bColDelay = false;
		}
	}

}

void CDisasterBoss::Attack4_3Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	m_bPush = false;
	m_bPlayerHit = true;
}

void CDisasterBoss::AvoidAttack_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	m_bPush = false;
	m_bLHandCol = false;
	m_bRollCol = false;

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(2) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(3) &&
		!m_bAttackCol)
	{
		m_bLHandCol = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(5) && !m_bAvoidSound)
	{
		m_bAvoidSound = true;

		GI->PlaySoundW(L"se_EN0003_attack4_1.wav", SD_DISASTERBOSS, 1.f);
	}

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(0) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(1) &&
		!m_bAttackCol)
	{
		LookAtPlayer();
		m_bHitAble = true;

		if (!m_bAvoidAttackOn)
		{
			m_bAvoidAttackOn = true;
		}
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(4) && !m_bAttack4_Effect)
	{
		GI->PlaySoundW(L"se_EN0003_attack2.wav", SD_DISASTERBOSS, 1.f);
		Create_Effect(AVOIDATTACK_LEFT);
		m_bAttack4_Effect = true;
	}

}

void CDisasterBoss::HitBack_Update(_float fTimeDelta)
{
	//m_pTransformCom->Go_Dir((m_pTransformCom->Get_State(CTransform::STATE_LOOK)), 1.5f, fTimeDelta, m_pNavigationCom);

	if (m_bSameAttack) //똑같은 공격 받았을때
	{
		m_bSameAttack = false;
		m_pModelCom->Set_AnimIndex(HITBACK);
		m_eCurState = HITBACK;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
	}
}

void CDisasterBoss::HitFly_Update(_float fTimeDelta)
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

void CDisasterBoss::HitFront_Update(_float fTimeDelta)
{
	//m_pTransformCom->Go_Dir((m_pTransformCom->Get_State(CTransform::STATE_LOOK) * -1.f), 1.5f, fTimeDelta, m_pNavigationCom);

	if (m_bSameAttack)
	{
		m_bSameAttack = false;
		m_pModelCom->Set_AnimIndex(HITFRONT);
		m_eCurState = HITFRONT;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
	}
}

void CDisasterBoss::HitDown_Update(_float fTimeDelta)
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

void CDisasterBoss::HitHori_Update(_float fTimeDelta)
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

	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(1))
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

void CDisasterBoss::HitSpin_Update(_float fTimeDelta)
{
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

void CDisasterBoss::HitUp_Update(_float fTimeDelta)
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

void CDisasterBoss::HitSlapdown_Update(_float fTimeDelta)
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

void CDisasterBoss::Run_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	m_fSpeed = 7.f;
	LookAtPlayer();
	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), m_fSpeed, fTimeDelta, m_pNavigationCom);

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bRunSound1)
	{
		m_bRunSound1 = true;
		GI->PlaySoundW(L"Boss_Step_Wlk_01.wav", SD_DISASTERBOSS, 0.15f);

	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bRunSound2)
	{
		m_bRunSound2 = true;
		GI->PlaySoundW(L"Boss_Step_Wlk_02.wav", SD_DISASTERBOSS, 0.15f);

	}
}

void CDisasterBoss::Walk_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	m_fSpeed = 4.f;
	LookAtPlayer();
	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), m_fSpeed, fTimeDelta, m_pNavigationCom);


	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bWalkSound1)
	{
		m_bWalkSound1 = true;
		GI->PlaySoundW(L"Boss_Step_Wlk_01.wav", SD_DISASTERBOSS, 0.15f);

	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bWalkSound2)
	{
		m_bWalkSound2 = true;
		GI->PlaySoundW(L"Boss_Step_Wlk_02.wav", SD_DISASTERBOSS, 0.15f);

	}

}

void CDisasterBoss::WalkBack_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	m_fSpeed = 2.f;
	LookAtPlayer();


	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bBackWalkSound1)
	{
		m_bBackWalkSound1 = true;
		GI->PlaySoundW(L"Boss_Step_Wlk_01.wav", SD_DISASTERBOSS, 0.15f);

	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bBackWalkSound2)
	{
		m_bBackWalkSound2 = true;
		GI->PlaySoundW(L"Boss_Step_Wlk_02.wav", SD_DISASTERBOSS, 0.15f);

	}
}

void CDisasterBoss::Strafe_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	LookAtPlayer();

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bStrafeSound1)
	{
		m_bStrafeSound1 = true;
		GI->PlaySoundW(L"Boss_Step_Wlk_01.wav", SD_DISASTERBOSS, 0.15f);

	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bStrafeSound2)
	{
		m_bStrafeSound2 = true;
		GI->PlaySoundW(L"Boss_Step_Wlk_02.wav", SD_DISASTERBOSS, 0.15f);

	}
}

void CDisasterBoss::Create_Item()
{
	CDropItem::DROPITEMDESC DropItemDesc;
	DropItemDesc.iItemType = CDropItem::TYPE_CLUE;
	DropItemDesc.iNaviNum = GI->Get_Cur_LevelIndex();
	XMStoreFloat4(&DropItemDesc.vStartPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	GI->Add_GameObjectToLayer(TEXT("DropItem"), GI->Get_Cur_LevelIndex(), TEXT("Monster_DropItem"), &DropItemDesc);
}

void CDisasterBoss::LookAtPlayer(_float fLiearTime)
{
	Set_Dir();
	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&m_vTargetLook), fLiearTime);

}


HRESULT CDisasterBoss::Ready_Components()
{
	//m_pEmissiveCom
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DisBoss_Emi"), TEXT("Com_TextureEmi"), (CComponent**)&m_pEmissiveCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("DisasterBoss"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	m_pModelCom->Set_RootHierarchy(3);


	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.2f, 1.2f, 1.2f);
	ColDesc.vCenter = _float3(0.f, 0.6f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Push_Range"); // 밀어내기 위함

	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(2.2f, 2.2f, 2.2f);
	ColDesc.vCenter = _float3(0.f, 0.8f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Roll_Attack"); //구르기

	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	//상체쪽
	CHierarchyNode*		pChest = m_pModelCom->Get_HierarchyNode("Chest_M");
	if (nullptr == pChest)
		ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.5f, 1.5f, 1.5f);
	ColDesc.vCenter = _float3(-0.1f, 0.4f, 0.f);
	ColDesc.pSocket = pChest;
	XMStoreFloat4x4(&ColDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	ColDesc.pOwner = this;
	ColDesc.iFlag = 0;
	strcpy(ColDesc.sTag, "Monster_Body1"); // 기본 충돌체 - 몬스터끼리 밀어내고 플레이어 또는 플레이어 공격이랑 충돌하기위한거

	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	//오른손
	CHierarchyNode*		pRightHand = m_pModelCom->Get_HierarchyNode("IndexFinger2_R");
	if (nullptr == pRightHand)
		return E_FAIL;
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.3f, 1.3f, 1.3f);
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.pSocket = pRightHand;
	XMStoreFloat4x4(&ColDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	ColDesc.pOwner = this;
	ColDesc.iFlag = 0;
	strcpy(ColDesc.sTag, "Monster_RHand"); //몬슽터 공격용 충돌체
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;
	//왼손

	CHierarchyNode*		pLeftHand = m_pModelCom->Get_HierarchyNode("IndexFinger2_L");
	if (nullptr == pLeftHand)
		return E_FAIL;
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.3f, 1.3f, 1.3f);
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.pSocket = pLeftHand;
	XMStoreFloat4x4(&ColDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	ColDesc.pOwner = this;
	ColDesc.iFlag = 0;
	strcpy(ColDesc.sTag, "Monster_LHand"); //몬스터 공격용 충돌체
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDisasterBoss::Create_Navigation(_uint iNaviNum)
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

HRESULT CDisasterBoss::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CUI::UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));

	UiInfoDesc.fSizeX = 524.f;
	UiInfoDesc.fSizeY = 42.f;
	UiInfoDesc.fX = 650.5f;
	UiInfoDesc.fY = 76.9f;
	UiInfoDesc.pDesc = this;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Boss_HP"), TM->Get_CurLevel(), pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 467.f;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Boss_HP_Bar"), TM->Get_CurLevel(), pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Boss_PG_Bar"), TM->Get_CurLevel(), pLayerTag, &UiInfoDesc)))
		return E_FAIL;

	UiInfoDesc.fSizeX = 50.f;
	UiInfoDesc.fSizeY = 50.f;
	UiInfoDesc.fX = 540.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = this;


	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_MiniBossMonster"), TM->Get_CurLevel(), TEXT("Layer_UI"), &UiInfoDesc)))
		return E_FAIL;

	return S_OK;
}

void CDisasterBoss::Set_PreBar(_float fTimeDelta)
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
		m_fPreHp -= m_fMinusHp * 10000.f * fTimeDelta ;

		if (m_fPreHp <= m_fNowHp)
		{
			m_fPreHp = m_fNowHp;
		}
	}

	if (m_fPrePatternGauge < m_fPatternGauge)
	{
		m_fMinusPg = (m_fPatternGauge - m_fPrePatternGauge) / m_fMaxPg;
		m_fPrePatternGauge += m_fMinusPg * 5000.f * fTimeDelta;

		if (m_fPrePatternGauge >= m_fPatternGauge)
		{
			m_fPrePatternGauge = m_fPatternGauge;
		}
	}
}

CDisasterBoss * CDisasterBoss::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDisasterBoss*		pInstance = new CDisasterBoss(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CDisasterBoss"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDisasterBoss::Clone(void * pArg)
{
	CDisasterBoss*		pInstance = new CDisasterBoss(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CDisasterBoss"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDisasterBoss::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pDissolveTexture);
	Safe_Release(m_pEmissiveCom);
}
