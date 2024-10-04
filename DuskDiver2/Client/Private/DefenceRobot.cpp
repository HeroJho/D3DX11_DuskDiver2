#include "stdafx.h"
#include "..\Public\DefenceRobot.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "ToolManager.h"
#include "Monster_Manager.h"
#include "Time_Manager.h"
#include "UI.h"
#include "EffectShield.h"
#include "DropItem.h"
#include "Player01Wind1.h"
#include "Particle_Manager.h"
#include "Player01HitEffect_1.h"
#include "Player01HitEffect_2.h"
#include "Player01SmokeBall.h"
#include "Camera_Manager.h"


CDefenceRobot::CDefenceRobot(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CDefenceRobot::CDefenceRobot(const CDefenceRobot & rhs)
	: CCreture(rhs)
{
}

void CDefenceRobot::Control_DefenceRobot()
{
	m_bMove = true;
	m_bSpawn = true;
	m_bAttack = false;
	m_bOneCheck = false;
	m_bHitAble = true;
	m_bAction = true;
}

HRESULT CDefenceRobot::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CDefenceRobot::Initialize(void * pArg)
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

	m_eCurState = SPAWN1;
	m_pModelCom->Set_AnimIndex(SPAWN1);

	m_vTargetLook = { 0.f,0.f,1.f };
	m_fPlayerPos = { 0.f,0.f,0.f, 1.f };
	m_vPlayerLook = { 0.f,0.f,1.f };
	m_fHurtTime = 0.02f;


	switch (m_iIndex)
	{
	case 5:
		m_fMaxHp = 5000.f;
		m_fStamina = 1.f;
		m_fPlusDamage = 0.f;
		if (CreateInfo.vPosition.x == 103.4f)
			m_fDelayTime = 1.5f;
		else
			m_fDelayTime = 1.8f;
		break;
	case 7:
		m_fMaxHp = 5000.f;
		m_fStamina = 2.f;
		m_fDelayTime = 2.f;
		m_fPlusDamage = 0.f;
		break;
	case 8:
		m_fMaxHp = 5500.f;
		m_fStamina = 1.f;
		m_fPlusDamage = 0.f;
		if (CreateInfo.vPosition.x == 103.4f)
			m_fDelayTime = 2.5f;
		else
			m_fDelayTime = 1.f;
		break;
	case 9:
		m_fMaxHp = 5000.f;
		m_fStamina = 1.f;
		m_fPlusDamage = 20.f;
		if (CreateInfo.vPosition.x == 176.f)
			m_fDelayTime = 2.3f;
		else
			m_fDelayTime = 1.9f;
		break;
	}

	m_fNowHp = m_fMaxHp;
	m_fDamage = 10.f;

	m_fPreHp = m_fNowHp;
	m_fMaxPg = m_fMaxHp * 0.4f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Dissolve"), TEXT("Com_Texture"), (CComponent**)&m_pDissolveTexture)))
		return E_FAIL;

	MM->Add_Monster(this);

	//MM->Add_DefenceRobot(this);

	//패턴게이지 생기면 들어가서 Mon_PG_Bar 생성하는거 주석 풀면 됩니당
	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	return S_OK;
}

void CDefenceRobot::Tick(_float fTimeDelta)
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

void CDefenceRobot::LateTick(_float fTimeDelta)
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

		if (m_bAttackCol)
			Choose_Col("Monster_Attack");
		if (m_bHurt)
			Choose_Col("Monster_Body1");


		Choose_Col("Monster_Push_Range");

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

		Render_Col(m_pRendererCom);
	}
	
}

HRESULT CDefenceRobot::Render()
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

	_float fGama = 1.f;
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
void CDefenceRobot::Render_Debug()
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
void CDefenceRobot::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
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

void CDefenceRobot::On_Hit(CCreture* pOther)
{
	//hit이 한번만 처리되게끔
	if (m_bHurt && m_bHitAble)
	{
		GI->PlaySoundW(L"SwordHit.ogg", SD_HIT, 0.4f);
		m_bRim = true;
		m_fRimAcc = 0.f;

		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		vPos.y += 1.f;
		pOther->Create_HitEffect(vPos);

		m_bHurt = false;
		m_bAction = false;
		m_bAttackCol = false;
		m_bRangeCol = false;

		if (!m_bDefendMode)
			m_fNowHp -= (pOther->Get_Damage() - m_fStamina);
		else
			m_fNowHp -= (pOther->Get_Damage() * 0.5f);

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

		if(!m_bDefendMode)
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

void CDefenceRobot::Create_HitEffect(_float4 vWorldPos)
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

void CDefenceRobot::Delay_Spawn(_float fTimeDelta)
{
	if (!m_bCreate)
	{
		m_fCreateAcc += 1.f * fTimeDelta;

		if (m_fCreateAcc >= (m_fDelayTime - 0.3f) && !m_bSpawnEffect)
		{
			m_bSpawnEffect = true;

			GI->PlaySoundW(L"se_enemy_spawn1.wav", SD_DEFENCEBOT, 0.5f);

			//GI->PlaySoundW(L"se_enemy_spawn2.wav", SD_DEFENCEBOT, 0.5f);//스폰애니메이션에 넣어주기
			PTM->Create_InstanceParticle(L"MonsterAppear", m_pTransformCom->Get_WorldFloat4x4());
			PTM->Create_InstanceParticle(L"MonsterAppear_2", m_pTransformCom->Get_WorldFloat4x4());
		}

		if (m_fCreateAcc >= m_fDelayTime)
			m_bCreate = true;
	}
}

HRESULT CDefenceRobot::Create_Effect(EFFECT eEffect)
{
	switch (eEffect)
	{
		case Client::CDefenceRobot::ATTACK1_EFFECT:
		{
			//밑에 범위뜨는거 생성
			CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
			Info3->fBlurPower = 3.f;
			Info3->vLocalPos = { 0.f,0.940f,0.23f,1.f };
			Info3->vColor = { 1.f,1.f,1.f,1.f };
			Info3->vColor2 = { 0.6f,0.850f,0.917f,1.f };
			Info3->vRatio = { 1.f, 3.f };
			Info3->vRatioSpeed = { 1.f, 1.f };
			Info3->vScaleSpeed = { 110.f, 110.f, 110.f };
			Info3->vScale = { 0.1f, 0.2f, 0.1f };
			Info3->vRotation = { 0.f,0.f, 180.f };
			Info3->fLifeTime = 0.2f;
			Info3->bRotation = true;
			Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			(GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3));

		}
		break;
		default:
			break;
	}

	return S_OK;
}

void CDefenceRobot::Set_State(STATE eState) //end 애니메이션에서
{
	//상태 보간 하는 함수
	if (m_eNextState == eState)
		return;

	m_eNextState = eState;

	switch (m_eNextState)
	{
		//다음 상태로 넘어갈때 해줘야하는것들
		//애니메이션x
	case Client::CDefenceRobot::ATTACK1:
		Change_AttackColRange(_float3{ 2.4f, 2.4f, 2.4f }, _float3{ 0.f, 0.f, 0.f });
		if (!m_bAvoidAttack)
		{
			m_eHitType = TYPE_SPIN;
			m_fPushPower = 7.f;
			m_fAirPower = 8.f;
			m_fDamage = 30.f + m_fPlusDamage;
		}
		else
		{
			m_eHitType = TYPE_AWAY;
			m_fPushPower = 12.f;
			m_fAirPower = 5.f;
			m_fDamage = 50.f + m_fPlusDamage;
		}
		m_bAttack1Effect = false;
		m_bOnePatternEffect = false;
		m_bWalkBack = true;
		m_bAttack1Col = false;
		LookAtPlayer();
		break;
	case Client::CDefenceRobot::ATTACK2:
		Change_AttackColRange(_float3{ 1.3f, 1.3f, 1.3f }, _float3{ 0.f, 0.f, -0.6f });
		m_eHitType = TYPE_HIT;
		m_fPushPower = 7.f;
		m_fAirPower = 0.f;
		m_fDamage = 35.f + m_fPlusDamage;
		m_bWalkBack = true;
		m_bAttack2Col = false;
		LookAtPlayer();
		break;
	case Client::CDefenceRobot::ATTACK3:
		Change_AttackColRange(_float3{ 1.3f, 1.3f, 1.3f }, _float3{ 0.f, 0.3f, 0.f });
		m_eHitType = TYPE_HIT;
		m_fPushPower = 7.f;
		m_fAirPower = 0.f;
		m_fDamage = 25.f  +m_fPlusDamage;
		m_bWalkBack = true;
		m_bAttack3Col_1 = false;
		m_bAttack3Sound = false;
		LookAtPlayer();
		break;
	case Client::CDefenceRobot::DEFEND_1:
	{		
		m_bShieldOff = false;
		static_cast<CEffectShield*>(m_pShieldEffect)->Shield_OnOff(true);
		Change_PushColRange(_float3{ 1.4f, 1.4f, 1.4f }, _float3{ 0.f, 0.4f, 0.f });		
		m_bDefendMode = true;
		LookAtPlayer();
		GI->PlaySoundW(L"se_EN0006_transform.wav", SD_DEFENCEBOT, 0.4f);
	}
		break;
	case Client::CDefenceRobot::DEFEND_2:
		break;
	case Client::CDefenceRobot::BOUNCE:
		m_bFly = true;
		break;
	case Client::CDefenceRobot::HITBACK:
		Reset_Collider();
		m_bFly = false;
		break;
	case Client::CDefenceRobot::HITFLY:
		m_bFly = true;
		Reset_Collider();
		if (m_fHAirPower == 0.f)
			m_pTransformCom->ResetAndJump(1.f);
		else
			m_pTransformCom->ResetAndJump(m_fHAirPower);
		break;
	case Client::CDefenceRobot::HITFRONT:
		m_bFly = false;
		Reset_Collider();
		break;
	case Client::CDefenceRobot::HITDOWN:
		m_bFly = true;
		break;
	case Client::CDefenceRobot::HITUP:
		m_bFly = true;
		Reset_Collider();
		break;
	case Client::CDefenceRobot::HITSPIN:
		m_bFly = true;
		Reset_Collider();
		break;
	case Client::CDefenceRobot::HITHORI:
		Reset_Collider();
		m_bFly = true;
		break;
	case Client::CDefenceRobot::HITSLAPDOWN:
		Reset_Collider();
		m_bFly = true;
		break;
	case Client::CDefenceRobot::DOWN:
		m_bFly = true;
		break;
	case Client::CDefenceRobot::GETUP:
		m_bFly = false;
		break;
	case Client::CDefenceRobot::SPAWN1:
		break;
	case Client::CDefenceRobot::SPAWN2:
		break;
	case Client::CDefenceRobot::IDLE:
		Control_DefenceRobot();
		break;
	case Client::CDefenceRobot::STUN:
		if (m_bDeadStart)
		{
			Create_DeadEffect();
			Set_Dead();
			GI->PlaySoundW(L"se_EN0005_attack2_3.wav", SD_TRAININGBOT1, 0.6f);

		}
		break;
	case Client::CDefenceRobot::STUNRECOVER:
		break;
	case Client::CDefenceRobot::STRAFELEFT:
		break;
	case Client::CDefenceRobot::STRAFERIGHT:
		break;
	case Client::CDefenceRobot::RUN:
		m_bAttack = false;
		break;
	case Client::CDefenceRobot::WALKBACK:
		m_bHurt = true;
		m_bHitAble = true;
		break;
	case Client::CDefenceRobot::WALKFRONT:
		break;

	}


	m_pModelCom->SetNextIndex(m_eNextState);
	m_pModelCom->SetChangeBool(true); 
	m_eCurState = m_eNextState; //보간시작할때 상태를 바꿔줌

}

void CDefenceRobot::Hit_State(CCreture * pOther)
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


void CDefenceRobot::End_Animation()
{
	if (m_pModelCom->GetAniEnd())
	{
		//애니메이션 끝났을때
		switch (m_eCurState)
		{
		case Client::CDefenceRobot::SPAWN1:
			GI->PlaySoundW(L"se_enemy_spawn2.wav", SD_DEFENCEBOT, 0.5f);
			m_pModelCom->Set_AnimIndex(STATE::SPAWN2);
			m_eCurState = STATE::SPAWN2;
			break;
		case Client::CDefenceRobot::SPAWN2:
			Set_State(IDLE);
			break;
		case Client::CDefenceRobot::IDLE:
			if (m_bMove)
			{
				m_bAttack = false;
				m_bOneCheck = false;
				m_bWalkBack = false;
				m_bHitAble = true;
				m_bAction = true;
			}
			break;
		case Client::CDefenceRobot::RUN:
			m_bOneCheck = false;
			break;
		case Client::CDefenceRobot::WALKBACK:
		{
			if (m_bWalkBack)
			{
				m_bWalkBack = false;
				m_bHitAble = true;
				Set_State(WALKBACK);
			}
			else
				Random_Walk();
		}
			break;
		case Client::CDefenceRobot::WALKFRONT:
			break;
		case Client::CDefenceRobot::STRAFELEFT:
			m_bAttack = false;
			m_bOneCheck = false;
			Set_State(IDLE);
			break;
		case Client::CDefenceRobot::STRAFERIGHT:
			m_bAttack = false;
			m_bOneCheck = false;
			Set_State(IDLE);
			break;
		case Client::CDefenceRobot::ATTACK1:
			if (m_bAvoidAttack)
			{
				m_bAttack = false;
				m_bAction = true;
				m_bOneCheck = false;
				m_bHitAble = true;
				m_bAvoidAttack = false;
				m_bReGauge = true;
			}
			else
				Set_State(STATE::WALKBACK);

			Change_AttackColRange(_float3{ 1.1f, 1.1f, 1.1f }, _float3{ 0.f, 0.f, 0.f });
			break;
		case Client::CDefenceRobot::ATTACK2:
			Set_State(STATE::WALKBACK);
			Change_AttackColRange(_float3{ 1.1f, 1.1f, 1.1f }, _float3{ 0.f, 0.f, 0.f });
			break;
		case Client::CDefenceRobot::ATTACK3:
			Set_State(STATE::WALKBACK);
			Change_AttackColRange(_float3{ 1.1f, 1.1f, 1.1f }, _float3{ 0.f, 0.f, 0.f });
			break;
		case Client::CDefenceRobot::DEFEND_1:
			m_bAttack4On = false;
			Set_State(STATE::DEFEND_2);
			break;
		case Client::CDefenceRobot::DEFEND_2:
			Change_PushColRange(_float3{ 1.f, 1.f, 1.f }, _float3{ 0.f, 0.4f, 0.f });
			Set_State(STATE::WALKBACK);
			m_fStamina = 1.f;
			break;
		case Client::CDefenceRobot::HITBACK:
			if (m_bDeadStart)
				Set_State(STUN);
			else
			{
				if (m_bAvoidAttack)
				{
					Set_State(ATTACK1);
				}
				else
				{
					m_bAttack = false;
					m_bOneCheck = false;
					m_bHitAble = true;
					m_bAction = true;
					Set_State(IDLE);
				}
			}
			break;
		case Client::CDefenceRobot::HITFRONT:
			if (m_bDeadStart)
				Set_State(STUN);
			else
			{
				if (m_bAvoidAttack)
				{
					Set_State(ATTACK1);
				}
				else
				{
					m_bAttack = false;
					m_bOneCheck = false;
					m_bHitAble = true;
					m_bAction = true;
					Set_State(IDLE);
				}
			}
			break;
		case Client::CDefenceRobot::HITFLY:
			Set_State(HITDOWN);
			break;
		case Client::CDefenceRobot::HITDOWN:
			break;
		case Client::CDefenceRobot::HITSPIN:
			Set_State(HITDOWN);
			break;
		case Client::CDefenceRobot::HITHORI:
			Set_State(HITDOWN);
			break;
		case Client::CDefenceRobot::HITUP:
			Set_State(HITDOWN);
			break;
		case Client::CDefenceRobot::HITSLAPDOWN:
			Set_State(HITDOWN);
			break;
		case Client::CDefenceRobot::BOUNCE:
			Set_State(DOWN);
			break;
		case Client::CDefenceRobot::DOWN:
			if (!m_bDeadStart)
				Set_State(GETUP);
			else
			{
				Create_DeadEffect();
				Set_Dead();
				GI->PlaySoundW(L"se_EN0005_attack2_3.wav", SD_TRAININGBOT1, 0.6f);
			}
			break;
		case Client::CDefenceRobot::GETUP:
			if (m_bAvoidAttack)
			{
				Set_State(ATTACK1);
			}
			else
			{
				m_bAttack = false;
				m_bOneCheck = false;
				m_bAction = true;
				m_bHitAble = true;
				Set_State(IDLE);
			}
			break;
		case Client::CDefenceRobot::STUN:
			if (!m_bDeadStart)
				Set_State(STUNRECOVER);
			else
			{
				m_bDissolve = true;
			}
			break;
		case Client::CDefenceRobot::STUNRECOVER:
			Set_State(IDLE);
			break;
		default:
			break;

		}

	}
}


void CDefenceRobot::Update(_float fTimeDelta)
{
	m_bRangeCol = false;
	m_bPush = true;

	if (!m_pModelCom->GetChangeBool())
	{
		switch (m_eCurState)
		{
			//다음 상태로 넘어갈때 해줘야하는것들
			//애니메이션x
	
		case Client::CDefenceRobot::SPAWN1:
			m_bRangeCol = true;
			break;
		case Client::CDefenceRobot::SPAWN2:
			m_bRangeCol = true;
			LookAtPlayer();
			break;
		case Client::CDefenceRobot::IDLE:
			m_bRangeCol = true;
			LookAtPlayer();
			break;
		case Client::CDefenceRobot::RUN:
			Run_Update(fTimeDelta);
			break;
		case Client::CDefenceRobot::WALKBACK: //텀주기 위한 상태, 뒤로 살짝 물러남, 애니메이션 자체가 뒤로 물러나므로 트랜스폼함수 필요없음
			WalkBack_Update(fTimeDelta);
			break;
		case Client::CDefenceRobot::WALKFRONT:
			Walk_Update(fTimeDelta);
			break;
		case Client::CDefenceRobot::STRAFELEFT:
			Strafe_Update(fTimeDelta);
			break;
		case Client::CDefenceRobot::STRAFERIGHT:
			Strafe_Update(fTimeDelta);
			break;
		case Client::CDefenceRobot::ATTACK1:
			Attack1_Update(fTimeDelta);
			break;
		case Client::CDefenceRobot::ATTACK2:
			Attack2_Update(fTimeDelta);
			break;
		case Client::CDefenceRobot::ATTACK3:
			Attack3_Update(fTimeDelta);
			break;
		case Client::CDefenceRobot::DEFEND_1:
			Attack4_1Update(fTimeDelta);
			break;
		case Client::CDefenceRobot::DEFEND_2:
			Attack4_2Update(fTimeDelta);
			break;
		case Client::CDefenceRobot::HITBACK:
			HitBack_Update(fTimeDelta);
			break;
		case Client::CDefenceRobot::HITFRONT:
			HitFront_Update(fTimeDelta);
			break;
		case Client::CDefenceRobot::HITFLY:
			HitFly_Update(fTimeDelta);
			break;
		case Client::CDefenceRobot::HITDOWN:
			HitDown_Update(fTimeDelta);
			break;
		case Client::CDefenceRobot::HITSPIN:
			HitSpin_Update(fTimeDelta);
			break;
		case Client::CDefenceRobot::HITHORI:
			m_bAction = false;
			HitHori_Update(fTimeDelta);
			break;
		case Client::CDefenceRobot::HITUP:
			HitUp_Update(fTimeDelta);
			break;
		case Client::CDefenceRobot::HITSLAPDOWN:
			HitSlapdown_Update(fTimeDelta);
			break;
		case Client::CDefenceRobot::BOUNCE:
			break;
		case Client::CDefenceRobot::DOWN:
			break;
		case Client::CDefenceRobot::GETUP:
			break;
		case Client::CDefenceRobot::STUN:
			break;
		case Client::CDefenceRobot::STUNRECOVER:
			break;
		default:
			break;
		}

	}

}

void CDefenceRobot::Set_Dir()
{
	//플레이어 포스 받아오기 미리 받아둘지

	//내가 바라봐야할 룩벡터 구하기, 플레이어 포스에서 내포스 빼고 노멀라이즈
	_vector vLook = XMVector3Normalize(XMLoadFloat4(&m_fPlayerPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	XMStoreFloat3(&m_vTargetLook, vLook);


}

void CDefenceRobot::LookDirPlayer()
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


void CDefenceRobot::Distance_BattlePattern(_float fTimeDelta)
{
	m_fPlayerPos = PM->Get_PlayerPos();

	m_fDistance = m_pTransformCom->TargetDistance(XMLoadFloat4(&m_fPlayerPos)); //타겟과 나의 거리 계속 계산

	
	ActionToPlayer(fTimeDelta);
	
	RegretToPlayer(fTimeDelta);
}

void CDefenceRobot::Change_AttackColRange(_float3 vSize, _float3 vCenter)
{
	Get_Colliders("Monster_Attack")->Set_Size(vSize);
	Get_Colliders("Monster_Attack")->Set_Center(vCenter);
}

void CDefenceRobot::Change_BodyColRange(_float3 vSize, _float3 vCenter)
{

	Get_Colliders("Monster_Body1")->Set_Size(vSize);
	Get_Colliders("Monster_Body1")->Set_Center(vCenter);
}

void CDefenceRobot::Change_PushColRange(_float3 vSize, _float3 vCenter)
{
	Get_Colliders("Monster_Push_Range")->Set_Size(vSize);
	Get_Colliders("Monster_Push_Range")->Set_Center(vCenter);
}

void CDefenceRobot::ActionToPlayer(_float fTimeDelta)
{
	if (m_bSpawn && m_bAction) 
	{
		if (m_fDistance <= 50.f)
		{
			//거리에 따라서 걸을지 뛸지
			if (!m_bAttack)
			{
				if (m_fDistance > 4.f)
					Set_State(STATE::RUN);
				else
					Set_State(STATE::IDLE);
			}

			if (m_fDistance <= 6.f)
			{
				AttackPattern(fTimeDelta);

			}

			return;
		}
		else
		{
			Set_State(RUN);
			return;

		}
	}
}


void CDefenceRobot::AttackPattern(_float fTimeDelta)
{
	//공격할수있는 상태 - AttackTime이 지정시간 이상이어야되고, 맞는 애니메이션이 끝났을때여야하고, 
	
	m_bAttack = true;

	 if (m_fDistance <= 3.5f) // 공격 길이 다똑같음
		CloseAttack(fTimeDelta);

}
void CDefenceRobot::RegretToPlayer(_float fTimeDelta)
{
	if (m_bSpawn && !m_bMove)
	{
		Set_State(IDLE);
	}
}

void CDefenceRobot::Check_Rim(_float fTimeDelta)
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
		m_iShaderPath = 0;
}

void CDefenceRobot::Check_Death(_float fTimeDelta)
{
	if (!m_bMinusCount && m_fNowHp <= 0.f)
	{
		m_pShieldEffect->Set_Dead();
		m_bMinusCount = true;
		MM->Minus_MonsterCount();
	}

	if (!m_pTransformCom->Get_Air() && m_fNowHp <= 0.f && !m_bDeadStart)
	{
		Create_Item();
		Event_Stage();

		m_bAction = false;
		m_bDeadStart = true;
		m_bHitAble = false;
		m_bRangeCol = false;
		m_bAttackCol = false;
		GI->PlaySoundW(L"se_enemy_death2.wav", SD_DEFENCEBOT, 0.7f);
		if (m_bDefendMode)
			Set_State(STUN);
	}
}

void CDefenceRobot::Check_Avoid()
{
	if (m_fPatternGauge >= m_fMaxPg)
	{
		m_bAvoidAttack = true;
		m_bAction = false;
		m_bHitAble = false;
	}
}

void CDefenceRobot::HitAwayPower_Cehck()
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
		m_fHPushPower -= 12.f;
		break;
	case 4:
		m_fHPushPower -= 18.f;
		break;
	}
}

void CDefenceRobot::Reset_Collider()
{
	m_bAttack1Col = false;
	m_bAttack2Col = false;
	m_bAttack3Col_1 = false;
}

void CDefenceRobot::Random_Walk()
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
		Set_State(STATE::IDLE);
		break;
	}
}

void CDefenceRobot::Event_Stage()
{
	switch (m_iIndex)
	{
	case 5: // 우상단
		MM->Event_Stage4_1();
		break;
	case 7:
		MM->Event_Stage4_3();
		break;
	case 8:
		MM->Event_Stage4_4();
		break;
	case 9:
		MM->Event_Stage4_5();
		break;
	default:
		break;
	}
}

void CDefenceRobot::CloseAttack(_float fTimeDelta)
{
	if (!m_bOneRandom)
	{
		m_iRandomAttack = GI->Get_Random(1, 4);

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
	default:
		break;
	}

}

void CDefenceRobot::Attack1(_float fTimeDelta)
{

	if (!m_bOneCheck)
	{
		m_bOneCheck = true;
		m_bOneRandom = false;
		Set_State(STATE::ATTACK1);

	}


}

void CDefenceRobot::Attack2(_float fTimeDelta)
{
	if (!m_bOneCheck)
	{
		m_bOneCheck = true;
		m_bOneRandom = false;
		Set_State(STATE::ATTACK2);

	}
}

void CDefenceRobot::Attack3(_float fTimeDelta)
{
	if (!m_bOneCheck)
	{
		m_bOneCheck = true;
		m_bOneRandom = false;
		Set_State(STATE::ATTACK3);

	}
}

void CDefenceRobot::Attack4(_float fTimeDelta)
{
	if (!m_bOneCheck)
	{
		m_bOneCheck = true;
		m_bOneRandom = false;
		Set_State(STATE::DEFEND_1);
	}
}


void CDefenceRobot::Attack1_Update(_float fTimeDelta)
{
	m_bPush = false;
	m_bRangeCol = true;
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(0))
		LookAtPlayer();
	else if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bAttack1Col)
	{
		GI->PlaySoundW(L"se_EN0006_attack1.wav", SD_DEFENCEBOT, 0.4f);
		m_bAttackCol = true;
		m_bAttack1Col = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bAttack1Effect)
	{
		m_bAttack1Effect = true;
		Create_Effect(ATTACK1_EFFECT);
	}

	if (m_bAvoidAttack)
	{
		if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(3) && !m_bOnePatternEffect)
		{
			m_bOnePatternEffect = true;
		
				GI->PlaySoundW(L"se_enemy_warning.wav", SD_DEFENCEBOT, 0.6f);

				_matrix Effectmatrix;
				Effectmatrix.r[0] = { 1.f, 0.f, 0.f, 0.f };
				Effectmatrix.r[1] = { 0.f, 1.f, 0.f, 0.f };
				Effectmatrix.r[2] = { 0.f, 0.f, 1.f, 0.f };
				Effectmatrix.r[3] = { 0.f, 1.27f, 1.39f, 1.f };

				_float4x4 PatternMatrx;

				XMStoreFloat4x4(&PatternMatrx, Effectmatrix * m_pTransformCom->Get_WorldMatrix());

				(PTM->Create_AlphaParticle(L"PatternGaugeCircle", PatternMatrx));
				(PTM->Create_AlphaParticle(L"PatternGaugeLine", PatternMatrx));
				(PTM->Create_AlphaParticle(L"PatternGaugeFlare", PatternMatrx));

			}
		}
}

void CDefenceRobot::Attack2_Update(_float fTimeDelta)
{
	m_bPush = false;
	m_bRangeCol = true;
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(0))
		LookAtPlayer();
	
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bAttack2Col)
	{
		GI->PlaySoundW(L"se_EN0006_attack3.wav", SD_DEFENCEBOT, 0.4f);
		m_bAttackCol = true;
		m_bAttack2Col = true;
	}
}

void CDefenceRobot::Attack3_Update(_float fTimeDelta)
{
	m_bPush = false;
	m_bRangeCol = true;
	m_bAttackCol = false;


	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(0))
		LookAtPlayer();
	
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bAttack3Col_1)
	{
		GI->PlaySoundW(L"se_EN0006_attack2_1.wav", SD_DEFENCEBOT, 0.4f);
		m_bAttackCol = true;
		m_bAttack3Col_1 = true;
	}



	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bAttack3Sound)
	{
		m_bAttack3Sound = true;
		GI->PlaySoundW(L"se_EN0006_attack2_2.wav", SD_DEFENCEBOT, 0.6f); // 바닥 찍기 공격하고 자세 바꿀때 넣기
	}


}

void CDefenceRobot::Attack4_1Update(_float fTimeDelta)
{
	m_bPush = false;
	m_bRangeCol = true;

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	static_cast<CEffectShield*>(m_pShieldEffect)->Update_ShiledState(fTimeDelta, vPos);
	m_fDefenceSoundAcc += 1.f * fTimeDelta;

	if (m_fDefenceSoundAcc >= 1.f)
	{
		GI->PlaySoundW(L"se_EN0006_attack2_loop.wav", SD_DEFENCEBOT, 0.4f);
		m_fDefenceSoundAcc = 0.f;
	}
}

void CDefenceRobot::Attack4_2Update(_float fTimeDelta)
{
	m_bPush = false;
	m_bRangeCol = true;

	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	static_cast<CEffectShield*>(m_pShieldEffect)->Update_ShiledState(fTimeDelta, vPos);

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(0))
	{
		m_bDefendMode = false;
		if (!m_bAttack4On)
		{
			m_bAttack4On = true;
			GI->PlaySoundW(L"se_EN0006_transform.wav", SD_DEFENCEBOT, 0.4f);
		}
	}

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(1) && !m_bShieldOff)
	{
		m_bShieldOff = true;
		static_cast<CEffectShield*>(m_pShieldEffect)->Shield_Off();

	}

}

void CDefenceRobot::Defend_EndUpdate(_float fTimeDelta)
{
}


void CDefenceRobot::HitBack_Update(_float fTimeDelta)
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

void CDefenceRobot::HitFly_Update(_float fTimeDelta)
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

void CDefenceRobot::HitFront_Update(_float fTimeDelta)
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

void CDefenceRobot::HitDown_Update(_float fTimeDelta)
{
	if (!m_pTransformCom->Get_Air())
	{
		m_pModelCom->Set_AnimIndex(BOUNCE);
		m_eCurState = BOUNCE;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);

	}

	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vPlayerLook), m_fHPushPower, fTimeDelta, m_pNavigationCom);
}

void CDefenceRobot::HitHori_Update(_float fTimeDelta)
{

	m_bAction = false;

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

void CDefenceRobot::HitSpin_Update(_float fTimeDelta)
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

void CDefenceRobot::HitUp_Update(_float fTimeDelta)
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

void CDefenceRobot::HitSlapdown_Update(_float fTimeDelta)
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

void CDefenceRobot::Run_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	m_fSpeed = 7.f;
	LookAtPlayer();
	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), m_fSpeed, fTimeDelta, m_pNavigationCom);
}

void CDefenceRobot::Walk_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	m_fSpeed = 2.f;
	LookAtPlayer();
	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), m_fSpeed, fTimeDelta, m_pNavigationCom);
}

void CDefenceRobot::WalkBack_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	m_fSpeed = 2.f;
	LookAtPlayer();
}

void CDefenceRobot::Strafe_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	LookAtPlayer();
}

void CDefenceRobot::Create_Item()
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

	if (4 <= iRandomA)
	{
		DropItemDesc.iItemType = CDropItem::TYPE_ARM;
		GI->Add_GameObjectToLayer(TEXT("DropItem"), GI->Get_Cur_LevelIndex(), TEXT("Monster_DropItem"), &DropItemDesc);
	}
	
	DropItemDesc.iItemType = CDropItem::TYPE_CHIP;
	GI->Add_GameObjectToLayer(TEXT("DropItem"), GI->Get_Cur_LevelIndex(), TEXT("Monster_DropItem"), &DropItemDesc);

	DropItemDesc.iItemType = CDropItem::TYPE_MONEY;
	GI->Add_GameObjectToLayer(TEXT("DropItem"), GI->Get_Cur_LevelIndex(), TEXT("Monster_DropItem"), &DropItemDesc);
}

void CDefenceRobot::Create_DeadEffect()
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

void CDefenceRobot::LookAtPlayer(_float fLiearTime)
{
	Set_Dir();
	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&m_vTargetLook), fLiearTime);

}


HRESULT CDefenceRobot::Ready_Components()
{
	//m_pEmissiveCom
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Defence_Emi"), TEXT("Com_TextureEmi"), (CComponent**)&m_pEmissiveCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("DefenceRobot"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	m_pModelCom->Set_RootHierarchy(3);

	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	ColDesc.vCenter = _float3(0.f, 0.4f, 0.f);
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "Monster_Push_Range"); // 밀어내기 위함

	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	CHierarchyNode*		pCenter = m_pModelCom->Get_HierarchyNode("Ball_M");
	if (nullptr == pCenter)
		return E_FAIL;
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.3f, 1.3f, 1.3f);
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.pSocket = pCenter;
	XMStoreFloat4x4(&ColDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "Monster_Body1"); // 기본 충돌체 - 몬스터끼리 밀어내고 플레이어 또는 플레이어 공격이랑 충돌하기위한거

	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.1f, 1.1f, 1.1f);
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.pSocket = pCenter;
	XMStoreFloat4x4(&ColDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "Monster_Attack"); //몬스터 공격용 충돌체
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	CEffectShield::EFFECTSHIELDDESC ShieldDesc;
	ShieldDesc.pDesc = this;
	ShieldDesc.fSize = 14.f;
	ShieldDesc.fMagicNum = 1.f;

	XMStoreFloat4(&ShieldDesc.vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	(GI->Add_Get_GameObjectToLayer(TEXT("EffectShield"), TM->Get_CurLevel(), TEXT("Layer_Effect"), &ShieldDesc, &m_pShieldEffect));

	Safe_AddRef(m_pShieldEffect);


	return S_OK;
}


HRESULT CDefenceRobot::Create_Navigation(_uint iNaviNum)
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
	case 4:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage4Navigation"), TEXT("Stage4"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
		break;
	}

	_int iNaviIndex = m_pNavigationCom->Find_NaviIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pNavigationCom->Set_NaviIndex(iNaviIndex);

	return S_OK;
}



HRESULT CDefenceRobot::Ready_Layer_UI(const _tchar * pLayerTag)
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

void CDefenceRobot::Set_PreBar(_float fTimeDelta)
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

CDefenceRobot * CDefenceRobot::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDefenceRobot*		pInstance = new CDefenceRobot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CDefenceRobot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDefenceRobot::Clone(void * pArg)
{
	CDefenceRobot*		pInstance = new CDefenceRobot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CDefenceRobot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDefenceRobot::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pShieldEffect);
	Safe_Release(m_pDissolveTexture);
	Safe_Release(m_pEmissiveCom);
}
