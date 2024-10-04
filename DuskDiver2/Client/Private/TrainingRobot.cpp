#include "stdafx.h"
#include "..\Public\TrainingRobot.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Player_Manager.h"
#include "DataManager.h"
#include "Parts.h"
#include "UI_Manager.h"
#include "ToolManager.h"
#include "Monster_Manager.h"
#include "Time_Manager.h"
#include "Camera_Manager.h"
#include "Particle_Manager.h"
#include "Player01HitEffect_1.h"
#include "Player01HitEffect_2.h"
#include "UI.h"
#include "Player01SmokeBall.h"

CTrainingRobot::CTrainingRobot(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CTrainingRobot::CTrainingRobot(const CTrainingRobot & rhs)
	: CCreture(rhs)
{
}

HRESULT CTrainingRobot::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CTrainingRobot::Initialize(void * pArg)
{

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;


	CCreture::CREATEINFO CreateInfo = *(CCreture::CREATEINFO*)pArg;
	_vector vPosition = XMLoadFloat4(&CreateInfo.vPosition);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	m_vTargetLook = CreateInfo.vDir;

	LookDirPlayer();

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
	m_pModelCom->Set_Duration(1.f);
	m_pModelCom->Set_TickPerSecond(0.f);

	m_fPlayerPos = { 0.f,0.f,0.f, 1.f }; //일단 초기화

	m_fHurtTime = 0.02f;

	m_fMaxHp = 1500.f;
	m_fNowHp = m_fMaxHp;
	m_fStamina = 1.f;

	m_fPreHp = m_fNowHp;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Dissolve"), TEXT("Com_Texture"), (CComponent**)&m_pDissolveTexture)))
		return E_FAIL;

	MM->Add_Monster(this);
	MM->Add_TrainingBot(this);

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	return S_OK;
}

void CTrainingRobot::Tick(_float fTimeDelta)
{

	TIM->Add_TimeObj(CTime_Manager::OBJ_MONSTER, this);

	Check_Hurt(fTimeDelta);
	Check_Rim(fTimeDelta);
	fTimeDelta *= m_fTMRatio;

	Emp_Col();
	WakeUpCheck(fTimeDelta);
	Distance_Pattern(fTimeDelta);
	Update(fTimeDelta);
	Check_Death(fTimeDelta);
	Set_PreBar(fTimeDelta);

	Check_MonsterLight(fTimeDelta);

}

void CTrainingRobot::LateTick(_float fTimeDelta)
{
	if (m_bGara)
	{
		if (nullptr == m_pRendererCom)
			return;

		fTimeDelta *= m_fTMRatio;

		m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom, m_pNavigationCom);

		m_pTransformCom->Tick_Gravity(fTimeDelta, m_pNavigationCom, 30.f);

		Tick_Col(m_pTransformCom->Get_WorldMatrix());

		End_Animation();

		if (m_bHandCol)
			Choose_Col("Monster_Hand");

		if (m_bHurt) //피격
			Choose_Col("Monster_Body1");

		if (m_bAttackCol)//공격
			Choose_Col("Monster_Attack");

		
		Choose_Col("Monster_Push_Range");

		// GI->Add_ColGroup(CColliderManager::COLLIDER_MONSTER, this);

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

		Render_Col(m_pRendererCom);

	}
}

HRESULT CTrainingRobot::Render()
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
	
	_float fGama = 0.9f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;

	//// For.Rim
	if (FAILED(m_pShaderCom->Set_RawValue("g_CamPosition", &GI->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	_float g_RimPower = m_fRim;
	if (FAILED(m_pShaderCom->Set_RawValue("g_RimPower", &g_RimPower, sizeof(_float))))
		return E_FAIL;


	// For.Blur
	_float fBlurPower = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &m_fBlurPower, sizeof(_float))))
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
void CTrainingRobot::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);

	ImGui::Text("AttackCheck : %f", m_fAttackTest);
	ImGui::Text("NowHP : %f", m_fNowHp);

	ImGui::Text("distance : %f", m_fDistance);

	ImGui::DragFloat("BlurPower ", &m_fBlurPower, 0.01f);

	ImGui::DragFloat4("Power, Time, LerpTime", (float*)&ShakeTest, 0.01f, -10.f, 10.f, "%.2f");

	ImGui::Checkbox("Pause", &m_bGara);

	ImGui::End();
}

// *[HeroJho]: 충돌이 발생할 때 호출되는 함수입니다.
void CTrainingRobot::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	// *[HeroJho]:
	// Desc.pOther		- 충돌한 상대편 객체의 주소입니다.
	// Desc.pMyDesc		- 상대편과 충돌한 나의 콜라이더 정보입니다.
	// Desc.pOtherDesc	- 나와 충돌한 상대편의 콜라이더 정보입니다.
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		// 내 손 Sphere 둘중 하나와 상대편의 OBB와 충돌 했다면
		if (!strcmp("Monster_Push_Range", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Push_Range", Desc.pOtherDesc->sTag))
		{
			m_pTransformCom->PushMeXZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION), Desc.pMyDesc->vSize.x, XMLoadFloat4(&((CCreture*)Desc.pOther)->Get_WorldPos()), Desc.pOtherDesc->vSize.x, TM->Get_TimeDelta(), m_pNavigationCom);
		}
		if (strcmp("Monster_Body1", Desc.pMyDesc->sTag) && 
			strcmp("Monster_Push_Range", Desc.pMyDesc->sTag) && !strcmp("Player_Body", Desc.pOtherDesc->sTag))
		{
			((CCreture*)Desc.pOther)->On_Hit(this);
		}
	}
	else if ("Tag_Monster" == Desc.pOther->Get_Tag())
	{
			if (!strcmp("Monster_Push_Range", Desc.pMyDesc->sTag) &&
				!strcmp("Monster_Push_Range", Desc.pOtherDesc->sTag))
			{
				//밀어내기
				if (!m_pTransformCom->Get_Air() && m_bPush)
					m_pTransformCom->PushOtherXZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION), Desc.pMyDesc->vSize.x, XMLoadFloat4(&((CCreture*)Desc.pOther)->Get_WorldPos()), Desc.pOtherDesc->vSize.x, TM->Get_TimeDelta(), m_pNavigationCom);
			}
	
	}

	
}

void CTrainingRobot::On_Hit(CCreture* pOther)
{
	//hit이 한번만 처리되게끔
	if (m_bHurt && m_bSpawn)
	{
		m_bRim = true;

		//막

		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		vPos.y += 1.f;
		pOther->Create_HitEffect(vPos);

		GI->PlaySoundW(L"SwordHit.ogg", SD_HIT, 0.4f);
		m_bHurt = false;
		m_bMove = false;
		m_bHandCol = false;
		m_bAttackCol = false;

		m_fNowHp -= (pOther->Get_Damage() - m_fStamina);


		if (MM->Get_MonsterCount() == 1 && m_fNowHp <= 0.f)
		{
			TIM->Fix_All(0.14f, 1.3f);

			CRM->Get_Cam()->Fov(-3.f, 0.15f, 1.3f);
		}

		if (m_fNowHp <= 0.f)
		{
			m_bHurt = false;
			m_bSpawn = false;
		}
		Hit_State(pOther);

		m_bSameAttack = true;

		UM->Plus_Combo();
		UM->Set_Hit(true);
		UM->Set_Up(true);
	}

	

}

void CTrainingRobot::Create_HitEffect(_float4 vWorldPos)
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

HRESULT CTrainingRobot::Set_RenderPass(SHADER eShader)
{

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	//_float fGama = 1.f;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
	//	return E_FAIL;

	switch (eShader)
	{
	case Client::CTrainingRobot::SHADER_DEFAULT:
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
				return E_FAIL;
		}
		break;
	case Client::CTrainingRobot::SHADER_RIM:
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			//// For.Rim
			if (FAILED(m_pShaderCom->Set_RawValue("g_CamPosition", &GI->Get_CamPosition(), sizeof(_float4))))
				return E_FAIL;

			_float g_RimPower = m_fRim;
			if (FAILED(m_pShaderCom->Set_RawValue("g_RimPower", &g_RimPower, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;


			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 1)))
				return E_FAIL;
		}
		break;
	case Client::CTrainingRobot::SHADER_LIGHT:
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
		_float Alpha = 1.f;
			if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &Alpha, sizeof(_float))))
				return E_FAIL;

			_float Blur = 1.f;
			if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &Blur, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Set_RawValue("g_vPlayerLightColor", &m_vMonsterLightColor, sizeof(_float3))))
				return E_FAIL;

			if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 11)))
				return E_FAIL;
		}
	}
	break;
	case Client::CTrainingRobot::SHADER_DISSOLVE:
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;

			if (FAILED(m_pDissolveTexture->Set_SRV(m_pShaderCom, "g_DissolveTexture", 0)))
				return E_FAIL;

			m_pShaderCom->Set_RawValue("g_fDissolveAcc", &m_fDissolveAcc, sizeof(float));

			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 6)))
				return E_FAIL;

		}
	}
	break;
	}
	return S_OK;
}

void CTrainingRobot::Set_State(STATE eState) //end 애니메이션에서
{
	//상태 보간 하는 함수
	if (m_eNextState == eState)
		return;

	m_eNextState = eState;

	switch (m_eNextState)
	{
		//다음 상태로 넘어갈때 해줘야하는것들
		//애니메이션x
	case Client::CTrainingRobot::ATTACK1:
		GI->PlaySoundW(L"se_EN0004_attack1_1.wav", SD_TRAININGBOT1, 0.4f);
		m_bWalkBack = true;
		m_bAttack1Col = false;
		m_eHitType = TYPE_HIT;
		m_fPushPower = 10.f;
		m_fAirPower = 0.f;
		m_fDamage = 15.f;
		break;
	case Client::CTrainingRobot::ATTACK2:
		GI->PlaySoundW(L"se_EN0004_attack2_1.wav", SD_TRAININGBOT1, 0.4f);
		m_bWalkBack = true;
		m_bAttack2Col = false;
		m_bAttack2Sound = false;
		m_eHitType = TYPE_AWAY;
		m_fPushPower = 7.f;
		m_fAirPower = 7.f;
		m_fDamage = 20.f;
		LookAtPlayer();
		break;
	case Client::CTrainingRobot::ATTACK3:
		GI->PlaySoundW(L"se_EN0004_attack3_2.wav", SD_TRAININGBOT1, 0.4f);
		m_bAttack3Col_0 = false;
		m_bAttack3Col_1 = false;
		m_bAttack3Col_2 = false;
		m_bAttack3Col_3 = false;
		m_bAttack3Col_4 = false;
		m_bAttack3Col_5 = false;
		m_bAttack3Col_6 = false;
		m_bWalkBack = true;
		m_eHitType = TYPE_HIT;
		m_fPushPower = 10.f;
		m_fAirPower = 0.f;
		m_fDamage = 17.f;
		LookAtPlayer();
		break;
	case Client::CTrainingRobot::ATTACK4_1:
		GI->PlaySoundW(L"se_EN0004_attack3_2.wav", SD_TRAININGBOT1, 0.4f);
		m_bAttack4_1Col = false;
		m_bWalkBack = false;
		m_eHitType = TYPE_HIT;
		m_fPushPower = 10.f;
		m_fAirPower = 0.f;
		m_fDamage = 10.f;
		break;
	case Client::CTrainingRobot::BOUNCE:
		m_bFly = true;
		break;
	case Client::CTrainingRobot::HITBACK:
		Reset_Collider();
		m_bFly = false;
		break;
	case Client::CTrainingRobot::HITFLY:
		m_bFly = true;
		Reset_Collider();
		if (m_fHAirPower == 0.f)
			m_pTransformCom->ResetAndJump(1.f);
		else
			m_pTransformCom->ResetAndJump(m_fHAirPower);
		break;
	case Client::CTrainingRobot::HITFRONT:
		m_bFly = false;
		Reset_Collider();
		break;
	case Client::CTrainingRobot::HITDOWN:
		m_bFly = true;
		break;
	case Client::CTrainingRobot::HITUP:
		m_bFly = true;
		Reset_Collider();
		break;
	case Client::CTrainingRobot::HITSPIN:
		Reset_Collider();
		HitAwayPower_Cehck();
		m_bFly = true;
		break;
	case Client::CTrainingRobot::HITSLAPDOWN:
		Reset_Collider();
		m_bFly = true;
		break;
	case Client::CTrainingRobot::DOWN:
		m_bFly = true;
		break;
	case Client::CTrainingRobot::GETUP:
		m_bFly = false;
		m_bGetUpSound = false;
		break;
	case Client::CTrainingRobot::SPAWN1:
		m_vMonsterLightColor = { 0.f,1.f,1.f };
		m_bSpawnSound = false;
		break;
	case Client::CTrainingRobot::SPAWN2:
		m_bSpawn2Sound = false;
		break;
	case Client::CTrainingRobot::IDLE:
		m_bSpawn = true;
		m_bMove = true;
		m_bHurt = true;
		break;
	case Client::CTrainingRobot::STUN:
		if (m_bDeadStart)
		{
			Create_DeadEffect();
			Set_Dead();
			GI->PlaySoundW(L"se_EN0005_attack2_3.wav", SD_TRAININGBOT1, 0.6f);
			MM->Event_Stage1();
			m_bDeadStart = false;
		}
		break;
	case Client::CTrainingRobot::STUNRECOVER:
		break;
	case Client::CTrainingRobot::STRAFELEFT:
		m_bStrafe1Sound = false;
		m_bStrafe2Sound = false;
		break;
	case Client::CTrainingRobot::STRAFERIGHT:
		m_bStrafe1Sound = false;
		m_bStrafe2Sound = false;
		break;
	case Client::CTrainingRobot::TURNLEFT:
		break;
	case Client::CTrainingRobot::TURNRIGHT:
		break;
	case Client::CTrainingRobot::RUN:
		m_bAttack = false;
		break;
	case Client::CTrainingRobot::WALKBACK:
		m_bWalkback1Sound = false;
		m_bWalkback2Sound = false;
		break;
	case Client::CTrainingRobot::WALK:
		break;

	}


	m_pModelCom->SetNextIndex(m_eNextState);
	m_pModelCom->SetChangeBool(true); 
	m_eCurState = m_eNextState; //보간시작할때 상태를 바꿔줌

}


void CTrainingRobot::End_Animation()
{
	if (m_pModelCom->GetAniEnd())
	{
		//애니메이션 끝났을때
		switch (m_eCurState)
		{
			//다음 상태로 넘어갈때 해줘야하는것들
			//애니메이션x
		case Client::CTrainingRobot::ATTACK1:
			Set_State(STATE::WALKBACK);
			break;
		case Client::CTrainingRobot::ATTACK2:
			Set_State(STATE::IDLE);
			break;
		case Client::CTrainingRobot::ATTACK3:
			Set_State(STATE::WALKBACK);
			break;
		case Client::CTrainingRobot::ATTACK4_1:
			m_eHitType = TYPE_HIT;
			m_fPushPower = 10.f;
			m_fAirPower = 0.f;
			m_pModelCom->Set_AnimIndex(STATE::ATTACK4_2);
			m_eCurState = STATE::ATTACK4_2;
			m_bAttack4_2Col_0 = false;
			m_bAttack4_2Col_1 = false;
			m_bAttack4_2Col_2 = false;
			m_bAttack4_2Col_3 = false;
			break;
		case Client::CTrainingRobot::ATTACK4_2:
			m_eHitType = TYPE_HIT;
			m_fPushPower = 10.f;
			m_fAirPower = 0.f;
			m_pModelCom->Set_AnimIndex(STATE::ATTACK4_3);
			m_eCurState = STATE::ATTACK4_3;
			m_bAttack4_3Col_0 = false;
			m_bAttack4_3Col_1 = false;
			m_bAttack4_3Col_2 = false;
			break;
		case Client::CTrainingRobot::ATTACK4_3:
			Set_State(GETUP);
			break;
		case Client::CTrainingRobot::BOUNCE:
			Set_State(DOWN);
			break;
		case Client::CTrainingRobot::HITBACK:
			if (m_bDeadStart)
				Set_State(STUN);
			else
			{
				m_fAttackTime = 0.f;
				m_bAttack = false;
				m_bOneCheck = false;
				m_bWalkBack = false;
				m_bMove = true;
				Set_State(IDLE);
			}
			break;
		case Client::CTrainingRobot::HITFRONT:
			if (m_bDeadStart)
				Set_State(STUN);
			else
			{
				m_fAttackTime = 0.f;
				m_bAttack = false;
				m_bOneCheck = false;
				m_bWalkBack = false;
				m_bMove = true;
				Set_State(IDLE);
			}
			break;
		case Client::CTrainingRobot::HITFLY:
			Set_State(HITDOWN);
			break;
		case Client::CTrainingRobot::HITDOWN:
			break;
		case Client::CTrainingRobot::HITSPIN:
			Set_State(HITDOWN);
			break;
		case Client::CTrainingRobot::HITUP:
			Set_State(HITDOWN);
			break;
		case Client::CTrainingRobot::HITSLAPDOWN:
			Set_State(HITDOWN);
			break;
		case Client::CTrainingRobot::DOWN:
			if (!m_bDeadStart)
				Set_State(GETUP);
			else
			{
				Create_DeadEffect();
				Set_Dead();
				GI->PlaySoundW(L"se_EN0005_attack2_3.wav", SD_TRAININGBOT1, 0.6f);

				MM->Event_Stage1();
			}
			break;
		case Client::CTrainingRobot::GETUP:
			m_fAttackTime = 0.f;
			m_bAttack = false;
			m_bOneCheck = false;
			m_bWalkBack = false;
			m_bMove = true;
			Set_State(IDLE);
			break;
		case Client::CTrainingRobot::SPAWN1:
			m_pModelCom->Set_AnimIndex(STATE::SPAWN2);
			m_eCurState = STATE::SPAWN2;
			break;
		case Client::CTrainingRobot::SPAWN2:
			m_bSpawn = true;
			Set_State(IDLE);
			break;
		case Client::CTrainingRobot::IDLE:
			m_fAttackTime = 0.f;
			m_bAttack = false;
			m_bOneCheck = false;
			break;
		case Client::CTrainingRobot::STUN:
			if(!m_bDeadStart)
				Set_State(STUNRECOVER);
			else
				m_bDissolve = true;
			break;
		case Client::CTrainingRobot::STUNRECOVER:
			Set_State(IDLE);
			break;
		case Client::CTrainingRobot::STRAFELEFT:
			m_bAttack = false;
			m_bOneCheck = false;
			m_bWalkBack = false;
			break;
		case Client::CTrainingRobot::STRAFERIGHT:
			m_bAttack = false;
			m_bOneCheck = false;
			m_bWalkBack = false;
			break;
		case Client::CTrainingRobot::TURNLEFT:
			break;
		case Client::CTrainingRobot::TURNRIGHT:
			break;
		case Client::CTrainingRobot::RUN:
			m_bOneCheck = false;
			break;
		case Client::CTrainingRobot::WALKBACK:
			if (m_bWalkBack)
			{
				m_bWalkback1Sound = false;
				m_bWalkback2Sound = false;
				Set_State(WALKBACK);
				m_bWalkBack = false;
			}
			else
				RandomWalk();
			break;
		case Client::CTrainingRobot::WALK:
			m_fAttackTime = 0.f;
			m_bAttack = false;
			m_bOneCheck = false;
			break;

		default:
			break;

		}

	}
}


void CTrainingRobot::Update(_float fTimeDelta)
{
	m_bPush = true;
	if (!m_pModelCom->GetChangeBool())
	{
		switch (m_eCurState)
		{
			//다음 상태로 넘어갈때 해줘야하는것들
			//애니메이션x
		case Client::CTrainingRobot::ATTACK1:
			Attack1_Update();
			break;
		case Client::CTrainingRobot::ATTACK2:
			Attack2_Update();
			break;
		case Client::CTrainingRobot::ATTACK3:
			Attack3_Update();
			break;
		case Client::CTrainingRobot::ATTACK4_1:
			Attack4_1Update();
			break;
		case Client::CTrainingRobot::ATTACK4_2:
			Attack4_2Update();
			break;
		case Client::CTrainingRobot::ATTACK4_3:
			Attack4_3Update();
			break;
		case Client::CTrainingRobot::BOUNCE:
			break;
		case Client::CTrainingRobot::HITBACK:
			HitBack_Update(fTimeDelta);
			break;
		case Client::CTrainingRobot::HITFLY:
			HitFly_Update(fTimeDelta);
			break;
		case Client::CTrainingRobot::HITFRONT:
			HitFront_Update(fTimeDelta);
			break;
		case Client::CTrainingRobot::HITDOWN:
			if (!m_pTransformCom->Get_Air())
				Set_State(BOUNCE);
			break;
		case Client::CTrainingRobot::HITSPIN:
			HitSpin_Update(fTimeDelta);
			break;
		case Client::CTrainingRobot::HITUP:
			HitUp_Update(fTimeDelta);
			break;
		case Client::CTrainingRobot::HITSLAPDOWN:
			HitSlapdown_Update(fTimeDelta);
			break;
		case Client::CTrainingRobot::DOWN:
			break;
		case Client::CTrainingRobot::GETUP:
			GetUp_Update(fTimeDelta);
			break;
		case Client::CTrainingRobot::SPAWN1:
			Spawn1_Update(fTimeDelta);
			break;
		case Client::CTrainingRobot::SPAWN2:
			Spawn2_Update(fTimeDelta);
			break;
		case Client::CTrainingRobot::IDLE:
			if(m_bMove)
				LookAtPlayer();
			break;
		case Client::CTrainingRobot::STUN:
			break;
		case Client::CTrainingRobot::STUNRECOVER:
			break;
		case Client::CTrainingRobot::STRAFELEFT:
			Strafe_Update(fTimeDelta);
			break;
		case Client::CTrainingRobot::STRAFERIGHT:
			Strafe_Update(fTimeDelta);
			break;
		case Client::CTrainingRobot::TURNLEFT:
			break;
		case Client::CTrainingRobot::TURNRIGHT:
			break;
		case Client::CTrainingRobot::RUN:
			Run_Update(fTimeDelta);
			break;
		case Client::CTrainingRobot::WALKBACK: //텀주기 위한 상태, 뒤로 살짝 물러남, 애니메이션 자체가 뒤로 물러나므로 트랜스폼함수 필요없음
			WalkBack_Update(fTimeDelta);
			break;
		case Client::CTrainingRobot::WALK:
			Walk_Update(fTimeDelta);
			break;

		default:
			break;
		}

	}

}

void CTrainingRobot::Set_Dir()
{
	//플레이어 포스 받아오기 미리 받아둘지

	//내가 바라봐야할 룩벡터 구하기, 플레이어 포스에서 내포스 빼고 노멀라이즈
	_vector vLook = XMVector3Normalize(XMLoadFloat4(&m_fPlayerPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	XMStoreFloat3(&m_vTargetLook, vLook);


}

void CTrainingRobot::LookDirPlayer()
{
	_vector vLook, vRight;
	vLook = (XMVector3Normalize(XMLoadFloat3(&m_vTargetLook)));

	vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	vLook = XMVector3Normalize(XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	_float3      vScale = m_pTransformCom->Get_Scale();

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}


void CTrainingRobot::Distance_Pattern(_float fTimeDelta)
{
	m_fPlayerPos = PM->Get_PlayerPos();

	m_fDistance = m_pTransformCom->TargetDistance(XMLoadFloat4(&m_fPlayerPos)); //타겟과 나의 거리 계속 계산

	if (m_bSpawn && m_bMove)
		ActionToPlayer(fTimeDelta);

}

void CTrainingRobot::AttackPattern(_float fTimeDelta)
{
	//공격할수있는 상태 - AttackTime이 지정시간 이상이어야되고, 맞는 애니메이션이 끝났을때여야하고, 
	m_fAttackTime += 1.f * fTimeDelta; //애니메이션이 다 끝나거나 hit 상태일때 m_fAttackTime 0으로

	if (m_fAttackTime >= 0.2f) 
	{
		m_bAttack = true;

		if(m_fDistance <= 10.f && m_fDistance >= 2.8f)
		{
			if (!m_bOneRandom)
			{
				m_iRandomAttack = GI->Get_Random(1, 4);
				m_bOneRandom = true;
			}

			switch (m_iRandomAttack)
			{
			case 1:
				Attack4(fTimeDelta);
				break;
			case 2:
				Attack2(fTimeDelta);
				break;
			case 3:
				Attack2(fTimeDelta);
				break;
			case 4:
				Attack3(fTimeDelta);
				break;
			}
			return;
		}
		else if (m_fDistance < 2.8f) //근거리 
		{
			Attack1(fTimeDelta);
			return;
		}
	}

}

void CTrainingRobot::RandomWalk()
{
	m_iRandomNum = GI->Get_Random(1, 3);
	switch (m_iRandomNum)
	{
	case 1:
		Set_State(STATE::STRAFELEFT);
		break;
	case 2:
		Set_State(STATE::STRAFERIGHT);
		break;
	case 3:
		Set_State(IDLE);
		break;
	}
}

void CTrainingRobot::ActionToPlayer(_float fTimeDelta)
{
	if (m_fDistance <= 50.f)
	{
		//거리에 따라서 걸을지 뛸지
		if (!m_bAttack)
		{
			if (m_fDistance > 4.f)
			{
				if(!m_bOneCheck)
					Set_State(STATE::RUN);
			}
			else
				Set_State(STATE::IDLE);
		}

	
		if (m_fDistance <= 8.f) //거리가 5이하되면 공격가능, 맞는 상태가 아닐때 공격가능, 또는 반격상태일때 가능(반격상태 - 플레이어한테 계속 맞을수없으니깐 5번맞았을때 뒤로 물러나서 공격가능하게끔)
		{
			AttackPattern(fTimeDelta);

		}

		return;
	}
	else
	{
		//돌아다니게 할지 아니면 idle 상태로 있을지
		
		Set_State(RUN);
		return;

	}
}

void CTrainingRobot::RegretToPlayer(_float fTimeDelta)
{
	if(!m_bDeadStart)
		Set_State(IDLE);

}

void CTrainingRobot::Check_Rim(_float fTimeDelta)
{
	if (m_bRim)
	{
		m_fRimAcc += 1.f *fTimeDelta;

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

void CTrainingRobot::Check_Death(_float fTimeDelta)
{
	if (!m_bMinusCount && m_fNowHp <= 0.f)
	{
		m_bMinusCount = true;
		MM->Minus_MonsterCount();
	}

	if (!m_pTransformCom->Get_Air() && m_fNowHp <= 0.f && !m_bDeadStart)
	{
		//MM->Event_Stage1();
		m_bHandCol = false;
		m_bAttackCol = false;
		m_bMove = false;
		m_bBodyCol = false;
		m_bAction = false;
		m_bDeadStart = true;
		GI->PlaySoundW(L"se_EN0004_death.wav", SD_TRAININGBOT1, 0.7f); // 터지면서 나는 소리
	}

	if (m_bDissolve)
	{
		m_iShaderPath = 6;

		m_fDissolveAcc += 0.3f * fTimeDelta;

		if (m_fDissolveAcc >= 0.7f)
		{
			m_fDissolveAcc = 0.7f;
			Set_Dead();
		}
	}

}

void CTrainingRobot::WakeUpCheck(_float fTimeDelta)
{
	if (m_bWakeUP)
	{
		m_bWakeUP = false;
		m_pModelCom->Set_Duration(82.f); //원래 듀레이션 찾기
		m_pModelCom->Set_TickPerSecond(26.f); //원래 듀레이션 찾기
		m_bMove = true;
	}
}

void CTrainingRobot::Reset_Collider()
{
	m_bAttack1Col = false; m_bAttack2Col = false;
	m_bAttack3Col_0 = false;  m_bAttack3Col_1 = false;  m_bAttack3Col_2 = false; 
	m_bAttack3Col_3 = false; m_bAttack3Col_4 = false; m_bAttack3Col_5 = false; m_bAttack3Col_6 = false;
	m_bAttack4_1Col = false; m_bAttack4_2Col_0 = false; m_bAttack4_2Col_1 = false; m_bAttack4_2Col_2 = false;
	m_bAttack4_2Col_3 = false; m_bAttack4_3Col_0 = false; m_bAttack4_3Col_1 = false; m_bAttack4_3Col_2 = false;
}

void CTrainingRobot::HitAwayPower_Cehck()
{
	_uint iPushPower = (_uint)m_fHPushPower / 10;

	switch (iPushPower)
	{
	case 0:
		m_fHPushPower -= 3.f;
		break;
	case 1:
		m_fHPushPower -= 6.f;
		break;
	case 2:
		m_fHPushPower -= 14.f;
		break;
	case 3:
		m_fHPushPower -= 22.f;
		break;
	case 4:
		m_fHPushPower -= 30.f;
		break;
	default:
		break;
	}

}

void CTrainingRobot::Create_DeadEffect()
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

void CTrainingRobot::Hit_State(CCreture * pOther)
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
		m_bAway = true;
		Set_State(HITSPIN);
		m_pTransformCom->ResetAndJump(5.f);

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

void CTrainingRobot::Attack1(_float fTimeDelta)
{
	if (!m_bOneCheck)
	{
		m_bOneCheck = true;
		m_bOneRandom = false;
		Set_State(STATE::ATTACK1);

	}


}

void CTrainingRobot::Attack2(_float fTimeDelta)
{
	if (m_fDistance > 4.f)
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
			Set_State(STATE::ATTACK2);
		}
	}
}

void CTrainingRobot::Attack3(_float fTimeDelta)
{
	if (m_fDistance > 5.f)
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

void CTrainingRobot::Attack4(_float fTimeDelta)
{
	if (m_fDistance < 4.f)
	{
		if (!m_bOneCheck)
		{
			m_bOneCheck = true;
			m_bOneRandom = false;
			Set_State(STATE::ATTACK3);
		}
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

void CTrainingRobot::Attack1_Update()
{
	m_bHandCol = false;
	m_bPush = false;
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bAttack1Col)
	{
		GI->PlaySoundW(L"se_EN0004_attack1_2.wav", SD_TRAININGBOT1, 0.35f);
		m_bHandCol = true;
		m_bAttack1Col = true;

	}
	else
		LookAtPlayer();
}

void CTrainingRobot::Attack2_Update()
{
	m_bAttackCol = false;
	m_bPush = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bAttack2Col)
	{
		GI->PlaySoundW(L"se_EN0004_attack2_2.wav", SD_TRAININGBOT1, 0.35f);
		m_bAttackCol = true;
		m_bAttack2Col = true;
	}
	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(0))
		LookAtPlayer();

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bAttack2Sound)
	{
		m_bAttack2Sound = true;
		GI->PlaySoundW(L"se_EN0004_attack3_5.wav", SD_TRAININGBOT1, 0.35f);
	}
		
}

void CTrainingRobot::Attack3_Update()
{
	m_bAttackCol = false;
	m_bPush = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bAttack3Col_0)
	{
		GI->PlaySoundW(L"se_EN0004_attack3_2.wav", SD_TRAININGBOT2, 0.4f);
		m_bAttackCol = true;
		m_bAttack3Col_0 = true;
	}

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

	LookAtPlayer();
}

void CTrainingRobot::Attack4_1Update()
{
	m_bAttackCol = false;
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bAttack4_1Col)
	{
		m_bAttackCol = true;
		m_bAttack4_1Col = true;
	}


	LookAtPlayer();
}

void CTrainingRobot::Attack4_2Update()
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bAttack4_2Col_0)
	{
		GI->PlaySoundW(L"se_EN0004_attack3_3.wav", SD_TRAININGBOT1, 0.4f);
		GI->PlaySoundW(L"se_EN0004_attack3_4.wav", SD_TRAININGBOT2, 0.4f);
		m_bAttackCol = true;
		m_bAttack4_2Col_0 = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bAttack4_2Col_1)
	{
		m_bAttackCol = true;
		m_bAttack4_2Col_1 = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bAttack4_2Col_2)
	{
		m_bAttackCol = true;
		m_bAttack4_2Col_2 = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(3) && !m_bAttack4_2Col_3)
	{
		m_bAttackCol = true;
		m_bAttack4_2Col_3 = true;
	}

	LookAtPlayer();
}

void CTrainingRobot::Attack4_3Update()
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(0))
		LookAtPlayer();

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bAttack4_3Col_0)
	{
		m_bAttackCol = true;
		m_bAttack4_3Col_0 = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bAttack4_3Col_1)
	{
		m_bAttackCol = true;
		m_bAttack4_3Col_1 = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bAttack4_3Col_2)
	{
		GI->PlaySoundW(L"se_EN0004_attack4_1.wav", SD_TRAININGBOT1, 0.4f);
		m_bAttackCol = true;
		m_bAttack4_3Col_2 = true;
	}

}

void CTrainingRobot::HitBack_Update(_float fTimeDelta)
{
	if (m_bSameAttack) //똑같은 공격 받았을때
	{
		m_bSameAttack = false;
		m_bMove = false;
		m_pModelCom->Set_AnimIndex(HITBACK);
		m_eCurState = HITBACK;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
	}
}

void CTrainingRobot::HitFly_Update(_float fTimeDelta)
{
	if (!m_pTransformCom->Get_Air())
		Set_State(BOUNCE);

	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vPlayerLook), m_fHPushPower, fTimeDelta, m_pNavigationCom);


	if (m_bSameAttack) //똑같은 공격 받았을때
	{
		m_bSameAttack = false;
		m_bMove = false;
		m_pModelCom->Set_AnimIndex(HITFLY);
		m_eCurState = HITFLY;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
	}
}

void CTrainingRobot::HitFront_Update(_float fTimeDelta)
{
	if (m_bSameAttack)
	{
		m_bSameAttack = false;
		m_bMove = false;
		m_pModelCom->Set_AnimIndex(HITFRONT);
		m_eCurState = HITFRONT;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
	}
}

void CTrainingRobot::HitDown_Update(_float fTimeDelta)
{
}

void CTrainingRobot::HitSpin_Update(_float fTimeDelta)
{
	m_bMove = false;

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
		m_bMove = false;
		m_pModelCom->Set_AnimIndex(HITSPIN);
		m_eCurState = HITSPIN;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
	}
}

void CTrainingRobot::HitUp_Update(_float fTimeDelta)
{
	if (!m_pTransformCom->Get_Air())
		Set_State(BOUNCE);

	if (m_bSameAttack)
	{
		m_bSameAttack = false;
		m_bMove = false;
		m_pModelCom->Set_AnimIndex(HITUP);
		m_eCurState = HITUP;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
	}
}

void CTrainingRobot::HitSlapdown_Update(_float fTimeDelta)
{
	if (!m_pTransformCom->Get_Air())
	{
		Set_State(BOUNCE);

	}
	if (m_bSameAttack)
	{
		m_bSameAttack = false;
		m_bMove = false;
		m_pModelCom->Set_AnimIndex(HITSLAPDOWN);
		m_eCurState = HITSLAPDOWN;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
	}
}

void CTrainingRobot::Run_Update(_float fTimeDelta)
{
	m_fSpeed = 4.5f;
	LookAtPlayer();
	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), m_fSpeed, fTimeDelta, m_pNavigationCom);

	m_fRunSoundAcc += 1.f * fTimeDelta;

	if (!m_bOtherFoot)
	{
		if (m_fRunSoundAcc >= 0.5f)
		{
			GI->PlaySoundW(L"se_EN0004_step_1-01.wav", SD_TRAININGBOT1, 0.1f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = true;
		}
	}

	else
	{
		if (m_fRunSoundAcc >= 0.5f)
		{
			GI->PlaySoundW(L"se_EN0004_step_1-02.wav", SD_TRAININGBOT1, 0.1f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = false;
		}
	}
}

void CTrainingRobot::Walk_Update(_float fTimeDelta)
{
	LookAtPlayer(0.6f);
	m_fSpeed = 1.f;
	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), m_fSpeed, fTimeDelta, m_pNavigationCom);

	m_fRunSoundAcc += 1.f * fTimeDelta;

	if (!m_bOtherFoot)
	{
		if (m_fRunSoundAcc >= 0.6f)
		{
			GI->PlaySoundW(L"se_EN0004_step_1-01.wav", SD_TRAININGBOT1, 0.1f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = true;
		}
	}

	else
	{
		if (m_fRunSoundAcc >= 0.6f)
		{
			GI->PlaySoundW(L"se_EN0004_step_1-02.wav", SD_TRAININGBOT1, 0.1f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = false;
		}
	}
}

void CTrainingRobot::WalkBack_Update(_float fTimeDelta)
{
	m_fSpeed = 2.f;
	LookAtPlayer();

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bWalkback1Sound)
	{
		m_bWalkback1Sound = true;
		GI->PlaySoundW(L"se_EN0004_step_1-01.wav", SD_TRAININGBOT1, 0.15f);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bWalkback2Sound)
	{
		m_bWalkback2Sound = true;
		GI->PlaySoundW(L"se_EN0004_step_1-02.wav", SD_TRAININGBOT1, 0.15f);

	}


	//m_fRunSoundAcc += 1.f * fTimeDelta;

	//if (!m_bOtherFoot)
	//{
	//	if (m_fRunSoundAcc >= 0.6f)
	//	{
	//		GI->PlaySoundW(L"se_EN0004_step_1-01.wav", SD_TRAININGBOT1, 0.1f);
	//		m_fRunSoundAcc = 0.f;
	//		m_bOtherFoot = true;
	//	}
	//}

	//else
	//{
	//	if (m_fRunSoundAcc >= 0.6f)
	//	{
	//		GI->PlaySoundW(L"se_EN0004_step_1-02.wav", SD_TRAININGBOT1, 0.1f);
	//		m_fRunSoundAcc = 0.f;
	//		m_bOtherFoot = false;
	//	}
	//}
}

void CTrainingRobot::Strafe_Update(_float fTimeDelta)
{
	LookAtPlayer();
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bStrafe1Sound)
	{
		m_bStrafe1Sound = true;
		GI->PlaySoundW(L"se_EN0004_step_2-01.wav", SD_TRAININGBOT1, 0.15f);

	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bStrafe2Sound)
	{
		m_bStrafe2Sound = true;
		GI->PlaySoundW(L"se_EN0004_step_2-02.wav", SD_TRAININGBOT1, 0.15f);

	}


/*
	m_fRunSoundAcc += 1.f * fTimeDelta;

	if (!m_bOtherFoot)
	{
		if (m_fRunSoundAcc >= 0.9f)
		{
			GI->PlaySoundW(L"se_EN0004_step_2-01.wav", SD_TRAININGBOT1, 0.1f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = true;
		}
	}

	else
	{
		if (m_fRunSoundAcc >= 0.9f)
		{
			GI->PlaySoundW(L"se_EN0004_step_2-02.wav", SD_TRAININGBOT1, 0.1f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = false;
		}
	}*/
}

void CTrainingRobot::GetUp_Update(_float fTimeDelta)
{

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bGetUpSound)
	{
		m_bGetUpSound = true;
		GI->PlaySoundW(L"se_EN0004_attack3_5.wav", SD_TRAININGBOT1, 0.4f);
	}
}

void CTrainingRobot::Spawn1_Update(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bSpawnSound)
	{
		m_bSpawnSound = true;
		GI->PlaySoundW(L"se_EN0004_attack3_5.wav", SD_TRAININGBOT1, 0.4f);
	}
}

void CTrainingRobot::Spawn2_Update(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bSpawn2Sound)
	{
		m_bSpawn2Sound = true;
		GI->PlaySoundW(L"se_EN0004_attack3_4.wav", SD_TRAININGBOT1, 0.4f);
	}
}

void CTrainingRobot::Check_MonsterLight(_float fTimeDelta)
{
	if (m_bMonsterLight)
	{
		m_eShaderPass = SHADER_LIGHT;
	
		m_fMonsterLightAcc += 1.f * fTimeDelta;

		if (m_fMonsterLightAcc >= m_fMonsterLightLimit)
		{
			m_bMonsterLight = false;
			m_fMonsterLightAcc = 0.f;
			m_eShaderPass = SHADER_DEFAULT;

		}
	}
}


void CTrainingRobot::LookAtPlayer(_float fLiearTime)
{
	Set_Dir();
	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&m_vTargetLook), fLiearTime);

}


HRESULT CTrainingRobot::Ready_Components()
{

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("TrainingRobot"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	m_pModelCom->Set_RootHierarchy(3);

	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.4f, 0.4f, 0.4f);
	ColDesc.vCenter = _float3(0.f, 0.3f, 0.f);
	ColDesc.iFlag = 2;
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "Monster_Push_Range"); // 밀어내기 위함

	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.5f, 1.5f, 1.5f);
	ColDesc.vCenter = _float3(0.f, 1.f, 0.4f);
	ColDesc.iFlag = 2;
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "Monster_Attack"); //범위공격

	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	//오른손
	CHierarchyNode*		pRightHand = m_pModelCom->Get_HierarchyNode("Wrist_R");
	if (nullptr == pRightHand)
		return E_FAIL;
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.6f, 0.6f, 0.6f);
	ColDesc.vCenter = _float3(0.1f, 0.f, 0.4f);
	ColDesc.pSocket = pRightHand;
	XMStoreFloat4x4(&ColDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	ColDesc.pOwner = this;
	ColDesc.iFlag = 0;
	strcpy(ColDesc.sTag, "Monster_Hand"); //몬스터 공격용 충돌체
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

	return S_OK;
}

HRESULT CTrainingRobot::Create_Navigation(_uint iNaviNum)
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
	}

	_int iNaviIndex = m_pNavigationCom->Find_NaviIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pNavigationCom->Set_NaviIndex(iNaviIndex);

	return S_OK;
}


HRESULT CTrainingRobot::Ready_Layer_UI(const _tchar * pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Monster_HP"), TM->Get_CurLevel(), pLayerTag, this)))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_GameObjectToLayer(TEXT("Monster_HP_Bar"), TM->Get_CurLevel(), pLayerTag, this)))
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

void CTrainingRobot::Set_PreBar(_float fTimeDelta)
{
	if (m_fPreHp > m_fNowHp)
	{
		m_fMinusHp = (m_fPreHp - m_fNowHp) / m_fMaxHp;
		m_fPreHp -= m_fMinusHp * 3000.f * fTimeDelta;

		if (m_fPreHp <= m_fNowHp)
		{
			m_fPreHp = m_fNowHp;
		}
	}
}

CTrainingRobot * CTrainingRobot::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTrainingRobot*		pInstance = new CTrainingRobot(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTrainingRobot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTrainingRobot::Clone(void * pArg)
{
	CTrainingRobot*		pInstance = new CTrainingRobot(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTrainingRobot"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTrainingRobot::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pDissolveTexture);

}
