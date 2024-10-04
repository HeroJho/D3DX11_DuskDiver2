#include "stdafx.h"
#include "..\Public\DisasterBeta.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "ToolManager.h"
#include "Monster_Manager.h"
#include "Time_Manager.h"
#include "Camera_Manager.h"
#include "UI.h"
#include "EffectCraw.h"
#include "DropItem.h"
#include "Particle_Manager.h"
#include "MonsterSpawn.h"
#include "Particle_Manager.h"
#include "Player01HitEffect_1.h"
#include "Player01HitEffect_2.h"
#include "Crack_Manager.h"


CDisasterBeta::CDisasterBeta(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CDisasterBeta::CDisasterBeta(const CDisasterBeta & rhs)
	: CCreture(rhs)
{
}

HRESULT CDisasterBeta::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CDisasterBeta::Initialize(void * pArg)
{

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;


	CCreture::CREATEINFO CreateInfo = *(CCreture::CREATEINFO*)pArg;
	_vector vPosition = XMLoadFloat4(&CreateInfo.vPosition);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	m_iIndex = CreateInfo.iIndex;
	m_vTargetLook = CreateInfo.vDir;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Create_Navigation(CreateInfo.iNaviNum)))
		return E_FAIL;
	

	m_sTag = "Tag_Monster";
	// *[HeroJho]: ImGui로 디버깅을 하려면 꼭 호출해줘야 합니다.
	Make_ImGui();

	m_eCurState = SPAWN;
	m_pModelCom->Set_AnimIndex(SPAWN);
	m_fPlayerPos = { 0.f,0.f,0.f, 1.f };
	m_vPlayerLook = { 0.f,0.f,1.f };

	m_fHurtTime = 0.02f;

	switch (m_iIndex)
	{
	case 2:
		m_fMaxHp = 2000.f;
		m_fStamina = 1.f;
		m_fPlusDamage = 0.f;
		if (CreateInfo.vPosition.x == -7.5f)
			m_fDelayTime = 2.4f;
		else
			m_fDelayTime = 1.9f;

		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));
		break;
	case 3:
		m_fMaxHp = 2500.f;
		m_fStamina = 1.f;
		m_fDelayTime = 1.f;
		m_fPlusDamage = 20.f;
		break;
	case 4:
		m_fMaxHp = 2500.f;
		m_fPlusDamage = 20.f;
		m_fStamina = 2.f;
		m_fDelayTime = 1.5f;
		break;
	case 6:
		m_fMaxHp = 2100.f;
		m_fPlusDamage = 70.f;
		m_fStamina = 1.f;
		if (CreateInfo.vPosition.x == 99.f)
			m_fDelayTime = 1.3f;
		else
			m_fDelayTime = 0.8f;
		break;
	case 7:
		m_fMaxHp = 3000.f;
		m_fPlusDamage = 50.f;
		m_fStamina = 2.f;
		m_fDelayTime = 2.f;
		break;
	case 9:
		m_fMaxHp = 3000.f;
		m_fPlusDamage = 50.f;
		m_fStamina = 1.f;
		if (CreateInfo.vPosition.x == 179.1f)
			m_fDelayTime = 1.7f;
		else
			m_fDelayTime = 3.2f;
		break;
	default:
		m_fMaxHp = 3000.f;
		m_fPlusDamage = 20.f;
		m_fStamina = 2.f;
		m_fDelayTime = 1.f;
		break;
	}

	m_fNowHp = m_fMaxHp;
	m_fDamage = 30.f;

	m_fPreHp = m_fNowHp;
	m_fMaxPg = m_fMaxHp * 0.4f;


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Dissolve"), TEXT("Com_Texture"), (CComponent**)&m_pDissolveTexture)))
		return E_FAIL;



	if (m_iIndex != 6)
	{
		MM->Add_Monster(this);
		MM->Add_DisasterBeta(this);

		if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
			return E_FAIL;
	}
	else if(6 == m_iIndex)
	{
		MM->Add_Haki(this);
	}

	return S_OK;
}

void CDisasterBeta::Tick(_float fTimeDelta)
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

void CDisasterBeta::LateTick(_float fTimeDelta)
{
	if (m_bCreate)
	{
		if (nullptr == m_pRendererCom)
			return;

		fTimeDelta *= m_fTMRatio;

		m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom, m_pNavigationCom);

		m_pTransformCom->Tick_Gravity(fTimeDelta, m_pNavigationCom, 40.f);

		End_Animation();

		if (m_bRHandCol)
			Choose_Col("Monster_RHand");
		if (m_bLHandCol)
			Choose_Col("Monster_LHand");
		if (m_bHurt)
			Choose_Col("Monster_Body1");
		
		Choose_Col("Monster_Push_Range");

		Tick_Col(m_pTransformCom->Get_WorldMatrix());
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

		Render_Col(m_pRendererCom);
	}
	
}

HRESULT CDisasterBeta::Render()
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
		/*if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_EMISSIVE, "g_EmissiveTexture")))
			return E_FAIL;*/

		if (FAILED(m_pDissolveTexture->Set_SRV(m_pShaderCom, "g_DissolveTexture", 0)))
			return E_FAIL;

		m_pShaderCom->Set_RawValue("g_fDissolveAcc", &m_fDissolveAcc, sizeof(float));

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, m_iShaderPath)))
			return E_FAIL;
	}	

	return S_OK;
}

// *[HeroJho]: 디버깅 창입니다. ImGui로 작동됩니다.
void CDisasterBeta::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);

	if (m_bCheckFront) //디버깅용
	{
		m_fTest = 1.f;
	}
	else
		m_fTest = -1.f;

	//ImGui::Text("FrontCheck : %f", m_fTest);
	ImGui::Text("distance : %f", m_fDistance);
	//ImGui::Text("AttackNum : %d", m_iAttackNum);

	//ImGui::DragFloat("BlurPower ", &m_fBlurPower, 0.01f);

	//_float3 vSize = Get_Colliders("Monster_Body1")->Get_Desc()->vSize;
	//_float3 vCenter = Get_Colliders("Monster_Body1")->Get_Desc()->vCenter;

	//ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	//ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	//Get_Colliders("Monster_Body1")->Set_Size(vSize);
	//Get_Colliders("Monster_Body1")->Set_Center(vCenter);

	ImGui::DragFloat4("Power, Time, LerpTime", (float*)&ShakeTest, 0.01f, -10.f, 10.f, "%.2f");

	ImGui::DragFloat4("Power, Time, StopTime", (float*)&FowTest, 0.01f, -10.f, 10.f, "%.2f");

	ImGui::Checkbox("Pause", &m_bCreate);

	ImGui::End();
}

// *[HeroJho]: 충돌이 발생할 때 호출되는 함수입니다.
void CDisasterBeta::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
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
			if (!m_bAttackCol)
				m_bAttackCol = true;

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

void CDisasterBeta::On_Hit(CCreture* pOther)
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
	if (m_bHurt && m_bHitAble)
	{
		m_bRim = true;
		m_fRimAcc = 0.f;
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		vPos.y += 1.f;
		pOther->Create_HitEffect(vPos);

		m_bHurt = false;
		m_bAction = false;
		m_bRHandCol = false;
		m_bLHandCol = false;
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

		if (MM->Get_MonsterCount() == 1 && m_fNowHp <= 0.f&& m_iIndex != 6)
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
			m_fPatternGauge += (pOther->Get_Damage() * 0.8f);

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

void CDisasterBeta::Create_HitEffect(_float4 vWorldPos)
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

void CDisasterBeta::Delay_Spawn(_float fTimeDelta)
{
	if (!m_bCreate)
	{
		m_fCreateAcc += 1.f * fTimeDelta;

		if (m_fCreateAcc >= (m_fDelayTime - 0.3f) && !m_bSpawnEffect)
		{
			m_bSpawnEffect = true;
			GI->PlaySoundW(L"se_enemy_spawn1.wav", SD_BETA, 0.5f);
			PTM->Create_InstanceParticle(L"MonsterAppear", m_pTransformCom->Get_WorldFloat4x4());
			PTM->Create_InstanceParticle(L"MonsterAppear_2", m_pTransformCom->Get_WorldFloat4x4());
		}

		if (m_fCreateAcc >= m_fDelayTime)
		{
			m_bCreate = true;
		}
	}
}

void CDisasterBeta::Set_State(STATE eState) //end 애니메이션에서
{
	//상태 보간 하는 함수
	if (m_eNextState == eState)
		return;

	m_eNextState = eState;

	switch (m_eNextState)
	{
		//다음 상태로 넘어갈때 해줘야하는것들
		//애니메이션x
	case Client::CDisasterBeta::ATTACK1:
		m_eHitType = TYPE_HIT;
		m_fPushPower = 8.f;
		m_fAirPower = 0.f;
		m_fDamage = 30.f + m_fPlusDamage;
		m_bAttackCol = false;
		m_bEffectTime = false;
		LookAtPlayer();
		break;
	case Client::CDisasterBeta::ATTACK2:
		m_eHitType = TYPE_AWAY;
		m_fPushPower = 10.f;
		m_fAirPower = 5.f;
		m_fDamage = 45.f + m_fPlusDamage;
		m_bAttack2Col = false;
		LookAtPlayer();
		break;
	case Client::CDisasterBeta::ATTACK3:
		m_eHitType = TYPE_HIT;
		m_fPushPower = 7.f;
		m_fAirPower = 0.f;
		m_fDamage = 35.f + m_fPlusDamage;
		m_bAttack3Col_1 = false;
		m_bAttack3Col_2 = false;
		m_bEffectTime3 = false;
		LookAtPlayer();
		break;
	case Client::CDisasterBeta::ATTACK4_1:
		LookAtPlayer();
		m_bAttackCol = false;
		m_bEffectTime4_1 = false;
		m_bEffectTime4_2 = false;
		m_bAttack4_1Sound = false;
		break;
	case Client::CDisasterBeta::AVOIDATTACK:
		m_eHitType = TYPE_SPIN;
		m_fPushPower = 10.f;
		m_fAirPower = 6.f;
		m_fDamage = 50.f + m_fPlusDamage;
		m_bAttackCol = false;
		m_bEffectTime5 = false;
		m_bAvoidAttackSound = false;
		break;
	case Client::CDisasterBeta::BOUNCE:
		m_bFly = true;
		break;
	case Client::CDisasterBeta::HITBACK:
		Reset_Collider();
		m_bFly = false;
		break;
	case Client::CDisasterBeta::HITFLY:
		Reset_Collider();
		if (m_fHAirPower == 0.f)
			m_pTransformCom->ResetAndJump(1.f);
		else
			m_pTransformCom->ResetAndJump(m_fHAirPower);
		m_bFly = true;
		break;
	case Client::CDisasterBeta::HITFRONT:
		m_bFly = false;
		Reset_Collider();
		break;
	case Client::CDisasterBeta::HITDOWN:
		m_bFly = true;
		break;
	case Client::CDisasterBeta::HITUP:
		m_bFly = true;
		Reset_Collider();
		break;
	case Client::CDisasterBeta::HITSPIN:
		m_bFly = true;
		Reset_Collider();
		break;
	case Client::CDisasterBeta::HITHORI:
		//HitAwayPower_Cehck();
		m_bFly = true;
		Reset_Collider();

		break;
	case Client::CDisasterBeta::HITSLAPDOWN:
		m_bFly = true;
		Reset_Collider();

		break;
	case Client::CDisasterBeta::DOWN:
		m_bFly = true;
		break;
	case Client::CDisasterBeta::GETUP:
		m_bFly = false;
		break;
	case Client::CDisasterBeta::SPAWN:
		break;
	case Client::CDisasterBeta::IDLE:
		if (m_iIndex == 3  || m_iIndex == 6 || m_iIndex == 7 || m_iIndex == 9)
			Control_DisasterBeta();
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
	case Client::CDisasterBeta::STUNBEGIN:
		break;
	case Client::CDisasterBeta::STUN:
		m_bWafer1 = false;
		break;
	case Client::CDisasterBeta::STUNRECOVER:
		break;
	case Client::CDisasterBeta::STRAFELEFT:
		break;
	case Client::CDisasterBeta::STRAFERIGHT:
		break;
	case Client::CDisasterBeta::RUN:
		break;
	case Client::CDisasterBeta::WALKBACK:
		m_bHurt = true;
		m_bHitAble = true;
		break;
	case Client::CDisasterBeta::WALKFRONT:
		break;

	}
	m_pModelCom->SetNextIndex(m_eNextState);
	m_pModelCom->SetChangeBool(true); 
	m_eCurState = m_eNextState; //보간시작할때 상태를 바꿔줌

}

void CDisasterBeta::Hit_State(CCreture * pOther)
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
				if (m_fNowHp <= 0.f)
					Set_State(STUNBEGIN);
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
					Set_State(HITFRONT);
				}
					
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


void CDisasterBeta::End_Animation()
{
	if (m_pModelCom->GetAniEnd())
	{
		//애니메이션 끝났을때
		switch (m_eCurState)
		{
			//다음 상태로 넘어갈때 해줘야하는것들
			//애니메이션x
		case Client::CDisasterBeta::SPAWN:
			Set_State(IDLE);
			break;
		case Client::CDisasterBeta::IDLE:
			if (m_bMove)
			{
				m_bOneCheck = false;
				m_bHitAble = true;
				m_bAction = true;
				m_bBackWalk = false;
			}
			break;
		case Client::CDisasterBeta::RUN:
			m_bOneCheck = false;
			break;
		case Client::CDisasterBeta::WALKBACK:
		{
			if (m_bBackWalk)
			{
				m_bBackWalk = false;
				m_bHitAble = true;
				Set_State(WALKBACK);
			}
			else
				RandomWalk();
		}
			break;
		case Client::CDisasterBeta::WALKFRONT:
			break;
		case Client::CDisasterBeta::STRAFELEFT:
			m_bAttack = false;
			m_bOneCheck = false;
			m_bBackWalk = false;
			Set_State(IDLE);
			break;
		case Client::CDisasterBeta::STRAFERIGHT:
			m_bAttack = false;
			m_bOneCheck = false;
			m_bBackWalk = false;
			Set_State(IDLE);
			break;
		case Client::CDisasterBeta::ATTACK1:
			m_bBackWalk = true;
			m_fAttackTime = 0.f;
			Set_State(STATE::WALKBACK);
			break;
		case Client::CDisasterBeta::ATTACK2:
			m_fAttackTime = 0.f;
			Set_State(STATE::WALKBACK);
			break;
		case Client::CDisasterBeta::ATTACK3:
			m_fAttackTime = 0.f;
			m_bBackWalk = true;
			Set_State(STATE::WALKBACK);
			break;
		case Client::CDisasterBeta::ATTACK4_1:
			m_eHitType = TYPE_HIT;
			m_fPushPower = 10.f;
			m_fAirPower = 0.f;
			m_pModelCom->Set_AnimIndex(STATE::ATTACK4_2);
			m_eCurState = STATE::ATTACK4_2;
			break;
		case Client::CDisasterBeta::ATTACK4_2:
			m_eHitType = TYPE_SPIN;
			m_fAirPower = 9.f;
			m_fPushPower = 12.f;
			m_pModelCom->Set_AnimIndex(STATE::ATTACK4_3);
			m_eCurState = STATE::ATTACK4_3;
			m_bAttackCol = false;
			m_bEffectTime4_3 = false;
			break;
		case Client::CDisasterBeta::ATTACK4_3:
			m_fAttackTime = 0.f;
			m_bBackWalk = true;
			Set_State(WALKBACK);
			break;
		case Client::CDisasterBeta::AVOIDATTACK:
			m_bAction = true;
			m_bOneCheck = false;
			m_bHitAble = true;
			m_bAvoidAttack = false;
			m_bAttack = false;
			m_fAttackTime = 0.f;
			Set_State(IDLE);
			break;
		case Client::CDisasterBeta::HITBACK:
			if (m_bDeadStart)
				Set_State(STUN);
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
		case Client::CDisasterBeta::HITFRONT:
			if (m_bDeadStart)
				Set_State(STUNBEGIN);
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
		case Client::CDisasterBeta::HITFLY:
			Set_State(HITDOWN);
			break;
		case Client::CDisasterBeta::HITDOWN:
			break;
		case Client::CDisasterBeta::HITSPIN:
			Set_State(HITDOWN);
			break;
		case Client::CDisasterBeta::HITHORI:
			Set_State(HITDOWN);
			break;
		case Client::CDisasterBeta::HITUP:
			Set_State(HITDOWN);
			break;
		case Client::CDisasterBeta::HITSLAPDOWN:
			Set_State(HITDOWN);
			break;
		case Client::CDisasterBeta::BOUNCE:
			Set_State(DOWN);
			break;
		case Client::CDisasterBeta::DOWN:
			if (!m_bDeadStart)
				Set_State(GETUP);
			else
				m_bDissolve = true;
			break;
		case Client::CDisasterBeta::GETUP:
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
		case Client::CDisasterBeta::STUNBEGIN:
			Set_State(STUN);
			break;
		case Client::CDisasterBeta::STUN:
			if (!m_bDeadStart)
				Set_State(STUNRECOVER);
			else
				m_bDissolve = true;
			break;
		case Client::CDisasterBeta::STUNRECOVER:
			Set_State(IDLE);
			break;
		default:
			break;

		}
	}
}


void CDisasterBeta::Update(_float fTimeDelta)
{
	m_bPush = true;

	if (!m_pModelCom->GetChangeBool())
	{
		switch (m_eCurState)
		{
			//다음 상태로 넘어갈때 해줘야하는것들
			//애니메이션x
		case Client::CDisasterBeta::SPAWN:
			Spawn_Update(fTimeDelta);
			break;
		case Client::CDisasterBeta::IDLE:
			LookAtPlayer();
			break;
		case Client::CDisasterBeta::RUN:
			Run_Update(fTimeDelta);
			break;
		case Client::CDisasterBeta::WALKBACK: //텀주기 위한 상태, 뒤로 살짝 물러남, 애니메이션 자체가 뒤로 물러나므로 트랜스폼함수 필요없음
			WalkBack_Update(fTimeDelta);
			break;
		case Client::CDisasterBeta::WALKFRONT:
			Walk_Update(fTimeDelta);
			break;
		case Client::CDisasterBeta::STRAFELEFT:
			Strafe_Update(fTimeDelta);
			break;
		case Client::CDisasterBeta::STRAFERIGHT:
			Strafe_Update(fTimeDelta);
			break;
		case Client::CDisasterBeta::ATTACK1:
			Attack1_Update(fTimeDelta);
			break;
		case Client::CDisasterBeta::ATTACK2:
			Attack2_Update(fTimeDelta);
			break;
		case Client::CDisasterBeta::ATTACK3:
			Attack3_Update(fTimeDelta);
			break;
		case Client::CDisasterBeta::ATTACK4_1:
			Attack4_1Update(fTimeDelta);
			break;
		case Client::CDisasterBeta::ATTACK4_2:
			Attack4_2Update(fTimeDelta);
			break;
		case Client::CDisasterBeta::ATTACK4_3:
			Attack4_3Update(fTimeDelta);
			break;
		case Client::CDisasterBeta::AVOIDATTACK:
			AvoidAttack_Update(fTimeDelta);
			break;
		case Client::CDisasterBeta::HITBACK:
			HitBack_Update(fTimeDelta);
			break;
		case Client::CDisasterBeta::HITFRONT:
			HitFront_Update(fTimeDelta);
			break;
		case Client::CDisasterBeta::HITFLY:
			HitFly_Update(fTimeDelta);
			break;
		case Client::CDisasterBeta::HITDOWN:
			HitDown_Update(fTimeDelta);
			break;
		case Client::CDisasterBeta::HITSPIN:
			HitSpin_Update(fTimeDelta);
			break;
		case Client::CDisasterBeta::HITHORI:
			m_bAction = false;
			HitHori_Update(fTimeDelta);
			break;
		case Client::CDisasterBeta::HITUP:
			HitUp_Update(fTimeDelta);
			break;
		case Client::CDisasterBeta::HITSLAPDOWN:
			HitSlapdown_Update(fTimeDelta);
			break;
		case Client::CDisasterBeta::BOUNCE:
			break;
		case Client::CDisasterBeta::DOWN:
			break;
		case Client::CDisasterBeta::GETUP:
			break;
		case Client::CDisasterBeta::STUNBEGIN:
			Stun_Update(fTimeDelta);
			break;
		case Client::CDisasterBeta::STUN:
			break;
		case Client::CDisasterBeta::STUNRECOVER:
			break;
		default:
			break;
		}

	}

}

void CDisasterBeta::Set_Dir()
{
	//플레이어 포스 받아오기 미리 받아둘지

	//내가 바라봐야할 룩벡터 구하기, 플레이어 포스에서 내포스 빼고 노멀라이즈
	_vector vLook = XMVector3Normalize(XMLoadFloat4(&m_fPlayerPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	XMStoreFloat3(&m_vTargetLook, vLook);


}

void CDisasterBeta::LookDirPlayer()
{
	_vector vLook, vRight;
	vLook = (XMVector3Normalize(XMLoadFloat3(&m_vTargetLook)));

	vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	vLook = XMVector3Normalize(XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	_float3      vScale = m_pTransformCom->Get_Scale();

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}


void CDisasterBeta::Distance_BattlePattern(_float fTimeDelta)
{
	m_fPlayerPos = PM->Get_PlayerPos();

	m_fDistance = m_pTransformCom->TargetDistance(XMLoadFloat4(&m_fPlayerPos)); //타겟과 나의 거리 계속 계산

	if (m_bSpawn && m_bAction) // 스폰이 끝났을때 시작
		ActionToPlayer(fTimeDelta);
	
	RegretToPlayer(fTimeDelta);
}

void CDisasterBeta::AttackPattern(_float fTimeDelta)
{
	//공격할수있는 상태 - AttackTime이 지정시간 이상이어야되고, 맞는 애니메이션이 끝났을때여야하고, 
	m_fAttackTime += 0.1f * fTimeDelta;

	if (m_fAttackTime > 0.3f)
	{
		m_bAttack = true;

		CloseAttack(fTimeDelta);
	}

}

void CDisasterBeta::RandomWalk()
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

void CDisasterBeta::ActionToPlayer(_float fTimeDelta)
{
	if (m_fDistance <= 50.f)
	{
		//거리에 따라서 걸을지 뛸지
		if (!m_bAttack)
			Set_State(STATE::IDLE);
		
		AttackPattern(fTimeDelta);

		return;
	}
	else
	{
		Set_State(RUN);
		return;

	}
}

void CDisasterBeta::RegretToPlayer(_float fTimeDelta)
{
	if (m_bSpawn && !m_bMove)
	{
		Set_State(IDLE);
	}

}

void CDisasterBeta::Check_Rim(_float fTimeDelta)
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
		m_iShaderPath = 4;
	}

}

void CDisasterBeta::Check_Death(_float fTimeDelta)
{
	if (!m_bMinusCount && m_fNowHp <= 0.f)
	{
		m_bMinusCount = true;
		MM->Minus_MonsterCount();
		if (6 != m_iIndex)
			Create_Item(DIE_STUN);

		Event_Stage();
	}

	if (!m_pTransformCom->Get_Air() && m_fNowHp <= 0.f && !m_bDeadStart)
	{
		m_bAction = false;
		m_bHitAble = false;
		m_bLHandCol = false;
		m_bRHandCol = false;
		m_bBodyCol = false;
		m_bDeadStart = true;
		GI->PlaySoundW(L"se_enemy_death1.wav", SD_BETA, 0.7f);
	}

	if (m_bDissolve)
	{
		m_iShaderPath = 6;
		m_fDissolveAcc += 0.1f * fTimeDelta;

		if (m_fDissolveAcc >= 0.4f)
		{
			m_fDissolveAcc = 0.4f;
			
			Set_Dead();

		}
	}
}

void CDisasterBeta::Check_Avoid()
{
	if (m_fPrePatternGauge >= m_fMaxPg)
	{
		m_bAvoidAttack = true;
		m_bAction = false;
		m_bHitAble = false;
	}
}

void CDisasterBeta::HitAwayPower_Cehck()
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

void CDisasterBeta::Reset_Collider()
{
	m_bAttackCol = false; m_bAttack2Col = false; 
	m_bAttack3Col_1 = false; m_bAttack3Col_2 = false;
}

void CDisasterBeta::Event_Stage()
{
	switch (m_iIndex)
	{
	case 2:
		MM->Event_Stage2();
		break;
	case 3:
		MM->Event_Stage3_1();
		break;
	case 4:
		MM->Event_Stage3_2();
		break;
	case 6:
		MM->Event_Stage4_2();
		break;
	case 7:
		MM->Event_Stage4_3();
		break;
	case 9:
		MM->Event_Stage4_5();
		break;
	default:
		break;
	}
}

void CDisasterBeta::Create_Effect(EFFECT eEffect)
{
	switch (eEffect)
	{
	case ATTACK1_LEFT:
	{
		CEffectCraw::CRAWINFO* Info = new CEffectCraw::CRAWINFO;
		Info->vColor = { 0.05f,0.33f,0.78f,1.f };
		Info->vColor2 = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 5.f,1.f };
		Info->vRatioSpeed = { 8.f,0.f };
		Info->fScale ={ 12.f,12.f, 12.f };
		Info->fSpeed = -1.f;
		_matrix Effectmatrix;
		Effectmatrix.r[0] = { -0.29f, 0.24f, -5.988f, 0.f };
		Effectmatrix.r[1] = { -3.824f, -4.62f, 0.f, 0.f };
		Effectmatrix.r[2] = { -4.61f, 3.81f, 0.376f, 0.f };
		Effectmatrix.r[3] = { -0.84f, 1.16f, 2.48f, 1.f };
		XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		Info->fBlurPower = 1.f;
		(GI->Add_GameObjectToLayer(L"EffectCraw", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info));

	}
		break;
	case ATTACK1_RIGHT:
	{
		CEffectCraw::CRAWINFO* Info = new CEffectCraw::CRAWINFO;
		Info->vColor = { 0.05f,0.33f,0.78f,1.f };
		Info->vColor2 = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 5.f,1.f };
		Info->vRatioSpeed = { 8.f,0.f };
		Info->fScale = { 12.f,12.f, 12.f };
		Info->fSpeed = -1.f;
		_matrix Effectmatrix;
		Effectmatrix.r[0] = { 3.487f, 1.640f, -7.010f, 0.f };
		Effectmatrix.r[1] = { -3.406f, 7.238f, 0.f, 0.f };
		Effectmatrix.r[2] = { 6.343f, 2.984f, 3.854f, 0.f };
		Effectmatrix.r[3] = { 0.86f, 1.21f, 2.39f, 1.f };
		XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		Info->fBlurPower = 1.f;
		(GI->Add_GameObjectToLayer(L"EffectCraw", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info));

	}
		break;
	case ATTACK3_LEFT:
	{
		CEffectCraw::CRAWINFO* Info = new CEffectCraw::CRAWINFO;
		Info->vColor = { 0.05f,0.33f,0.78f,1.f };
		Info->vColor2 = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 5.f,1.f };
		Info->vRatioSpeed = { 10.f,0.f };
		Info->fScale = { 16.f,16.f, 16.f };
		Info->fSpeed = -2.f;
		_matrix Effectmatrix;
		Effectmatrix.r[0] = { 0.f, 0.f, -13.989f, 0.f };
		Effectmatrix.r[1] = { 3.481f, -13.56f, 0.f, 0.f };
		Effectmatrix.r[2] = { -13.552f, -3.479f, 0.f, 0.f };
		Effectmatrix.r[3] = { 0.7f, 0.56f, 1.45f, 1.f };
		XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		Info->fBlurPower = 1.f;
		(GI->Add_GameObjectToLayer(L"EffectCraw", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info));

	}
	break;

	case ATTACK3_RIGHT:
	{
		CEffectCraw::CRAWINFO* Info = new CEffectCraw::CRAWINFO;
		Info->vColor = { 0.05f,0.33f,0.78f,1.f };
		Info->vColor2 = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 5.f,1.f };
		Info->vRatioSpeed = { 10.f,0.f };
		Info->fScale = { 16.f,16.f, 16.f };
		Info->fSpeed = -2.f;
		_matrix Effectmatrix;
		Effectmatrix.r[0] = { 1.368f, 2.908f, -5.065f, 0.f };
		Effectmatrix.r[1] = { -3.124f, 4.761f, 1.889f, 0.f };
		Effectmatrix.r[2] = { 4.936f, 2.206f, 3.854f, 0.f };
		Effectmatrix.r[3] = { 0.64f, 1.07f, 2.22f, 1.f };
		XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		Info->fBlurPower = 1.f;
		(GI->Add_GameObjectToLayer(L"EffectCraw", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info));

	}
	break;
	case ATTACK4_2LEFT:
	{
		CEffectCraw::CRAWINFO* Info = new CEffectCraw::CRAWINFO;
		Info->vColor = { 0.05f,0.33f,0.78f,1.f };
		Info->vColor2 = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 5.f,1.f };
		Info->vRatioSpeed = { 15.f,0.f };
		Info->fScale = { 14.f,14.f, 14.f };
		Info->fSpeed = -2.f;
		_matrix Effectmatrix;
		Effectmatrix.r[0] = { -3.351f, 1.842f, -4.622f, 0.f };
		Effectmatrix.r[1] = { -3.138f, -5.107f, 0.240f, 0.f };
		Effectmatrix.r[2] = { -3.861f, 2.552f, 3.816f, 0.f };
		Effectmatrix.r[3] = { -0.26f, 0.96f, 1.47f, 1.f };
		XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		Info->fBlurPower = 1.f;
		(GI->Add_GameObjectToLayer(L"EffectCraw", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info));

	}
	break;
	case ATTACK4_2RIGHT:
	{
		CEffectCraw::CRAWINFO* Info = new CEffectCraw::CRAWINFO;
		Info->vColor = { 0.05f,0.33f,0.78f,1.f };
		Info->vColor2 = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 5.f,1.f };
		Info->vRatioSpeed = { 15.f,0.f };
		Info->fScale = { 14.f,14.f, 14.f };
		Info->fSpeed = -2.f;
		_matrix Effectmatrix;
		Effectmatrix.r[0] = { 2.816f, 1.548f, -5.065f, 0.f };
		Effectmatrix.r[1] = { -1.377f, 5.754f, 0.993f, 0.f };
		Effectmatrix.r[2] = { 5.115f, 0.696f, 3.057f, 0.f };
		Effectmatrix.r[3] = { 0.47f, 1.14f, 2.14f, 1.f };
		XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		Info->fBlurPower = 1.f;
		(GI->Add_GameObjectToLayer(L"EffectCraw", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info));

	}
	break;
	case ATTACK4_3_RIGHT:
	{
		CEffectCraw::CRAWINFO* Info = new CEffectCraw::CRAWINFO;
		Info->vColor = { 0.05f,0.33f,0.78f,1.f };
		Info->vColor2 = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 5.f,1.f };
		Info->vRatioSpeed = { 12.f,0.f };
		Info->fScale = { 18.f,20.f, 20.f };
		Info->fSpeed = -3.f;
		_matrix Effectmatrix;
		Effectmatrix.r[0] = { -1.265f, -0.803f, -7.858f, 0.f };
		Effectmatrix.r[1] = { -5.358f, 8.443f, 0.f, 0.f };
		Effectmatrix.r[2] = { 8.293f, 5.263f, -1.873f, 0.f };
		Effectmatrix.r[3] = { 0.46f, 1.46f, 1.6f, 1.f };
		XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		Info->fBlurPower = 1.f;
		(GI->Add_GameObjectToLayer(L"EffectCraw", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info));

	}
	break;
	case AVOIDATTACK_LEFT:
	{
		CEffectCraw::CRAWINFO* Info = new CEffectCraw::CRAWINFO;
		Info->vColor = { 0.05f,0.33f,0.78f,1.f };
		Info->vColor2 = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 5.f,1.f };
		Info->vRatioSpeed = { 10.f,0.f };
		Info->fScale = { 16.f,16.f, 16.f };
		Info->fSpeed = 2.5f;
		_matrix Effectmatrix;
		Effectmatrix.r[0] = { 2.931f, -0.559f, 11.622f, 0.f };
		Effectmatrix.r[1] = { 2.248f, 11.787f, 0.f, 0.f };
		Effectmatrix.r[2] = { -11.417f, 2.177f, 2.984f, 0.f };
		Effectmatrix.r[3] = { -0.6f, 0.1f, 2.17f, 1.f };
		XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		Info->fBlurPower = 1.f;
		(GI->Add_GameObjectToLayer(L"EffectCraw", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info));
	}
	break;
	default:
		break;
	}
}

void CDisasterBeta::Create_Item(DIE eDie)
{
	_uint iRandomA = GI->Get_Random(0,10);

	switch (eDie)
	{
	case DIE_BOUNCE:
		{
		CDropItem::DROPITEMDESC DropItemDesc;
		DropItemDesc.iItemType = CDropItem::TYPE_WAFER;
		DropItemDesc.iNaviNum = GI->Get_Cur_LevelIndex();
		XMStoreFloat4(&DropItemDesc.vStartPosition, (m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
		for (_uint i = 0; 3 > i; ++i)
		{
			GI->Add_GameObjectToLayer(TEXT("DropItem"), GI->Get_Cur_LevelIndex(), TEXT("Monster_DropItem"), &DropItemDesc);
		}
		if (3 > iRandomA)
		{
			DropItemDesc.iItemType = CDropItem::TYPE_SKIN;
			GI->Add_GameObjectToLayer(TEXT("DropItem"), GI->Get_Cur_LevelIndex(), TEXT("Monster_DropItem"), &DropItemDesc);
		}
		if (3 <= iRandomA)
		{
			DropItemDesc.iItemType = CDropItem::TYPE_CLAW;
			GI->Add_GameObjectToLayer(TEXT("DropItem"), GI->Get_Cur_LevelIndex(), TEXT("Monster_DropItem"), &DropItemDesc);
		}
		DropItemDesc.iItemType = CDropItem::TYPE_MONEY;
		GI->Add_GameObjectToLayer(TEXT("DropItem"), GI->Get_Cur_LevelIndex(), TEXT("Monster_DropItem"), &DropItemDesc);
		}
		break;
	case DIE_STUN:
		{
		CDropItem::DROPITEMDESC DropItemDesc;
		DropItemDesc.iItemType = CDropItem::TYPE_WAFER;
		DropItemDesc.iNaviNum = GI->Get_Cur_LevelIndex();
		XMStoreFloat4(&DropItemDesc.vStartPosition, (m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
		for (_uint i = 0; 3 > i; ++i)
		{
			GI->Add_GameObjectToLayer(TEXT("DropItem"), GI->Get_Cur_LevelIndex(), TEXT("Monster_DropItem"), &DropItemDesc);
		}
		if (3 > iRandomA)
		{
			DropItemDesc.iItemType = CDropItem::TYPE_SKIN;
			GI->Add_GameObjectToLayer(TEXT("DropItem"), GI->Get_Cur_LevelIndex(), TEXT("Monster_DropItem"), &DropItemDesc);
		}
		if (3 <= iRandomA)
		{
			DropItemDesc.iItemType = CDropItem::TYPE_CLAW;
			GI->Add_GameObjectToLayer(TEXT("DropItem"), GI->Get_Cur_LevelIndex(), TEXT("Monster_DropItem"), &DropItemDesc);
		}
		DropItemDesc.iItemType = CDropItem::TYPE_MONEY;
		GI->Add_GameObjectToLayer(TEXT("DropItem"), GI->Get_Cur_LevelIndex(), TEXT("Monster_DropItem"), &DropItemDesc);
		}

		break;
	}
}

void CDisasterBeta::Control_DisasterBeta()
{
	m_bMove = true;
	m_bSpawn = true;
	m_fAttackTime = 0.f;
	m_bAttack = false;
	m_bOneCheck = false;
	m_bHitAble = true;
	m_bAction = true;
}

void CDisasterBeta::CloseAttack(_float fTimeDelta)
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

void CDisasterBeta::Attack1(_float fTimeDelta)
{
	m_bBackWalk = false;

	if (m_fDistance < 5.f)
	{
		if (!m_bOneCheck)
		{
			m_bBackWalk = true;
			Set_State(STATE::WALKBACK);

		}
	}
	else if(m_fDistance >5.f && m_fDistance < 8.f)
	{
		if (!m_bOneCheck)
		{
			m_bOneCheck = true;
			m_bOneRandom = false;
			Set_State(STATE::ATTACK1);

		}
	}
	else if (m_fDistance >= 8.f)
	{
		if (!m_bOneCheck)
			Set_State(STATE::RUN);
	}


}

void CDisasterBeta::Attack2(_float fTimeDelta)
{
	if (m_fDistance >= 8.f)
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

void CDisasterBeta::Attack3(_float fTimeDelta)
{
	if (m_fDistance >= 8.f)
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

void CDisasterBeta::Attack4(_float fTimeDelta)
{
	if (m_fDistance >= 8.f)
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

void CDisasterBeta::AvoidAttack()
{
	if (!m_bOneCheck)
	{
		GI->PlaySoundW(L"se_enemy_warning.wav", SD_BETA, 0.6f);

		_matrix Effectmatrix;
		Effectmatrix.r[0] = { 1.f, 0.f, 0.f, 0.f };
		Effectmatrix.r[1] = { 0.f, 1.f, 0.f, 0.f };
		Effectmatrix.r[2] = { 0.f, 0.f, 1.f, 0.f };
		Effectmatrix.r[3] = { 0.f, 1.48f, 1.66f, 1.f };

		_float4x4 PatternMatrx;

		XMStoreFloat4x4(&PatternMatrx, Effectmatrix * m_pTransformCom->Get_WorldMatrix());

		(PTM->Create_AlphaParticle(L"PatternGaugeCircle", PatternMatrx));
		(PTM->Create_AlphaParticle(L"PatternGaugeLine", PatternMatrx));
		(PTM->Create_AlphaParticle(L"PatternGaugeFlare", PatternMatrx));

		LookAtPlayer();
		m_bOneCheck = true;
		Set_State(STATE::AVOIDATTACK);
		m_bReGauge = true;
	}
}

void CDisasterBeta::Spawn_Update(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bRoarSound1)
	{
		m_bRoarSound1 = true;
		GI->PlaySoundW(L"se_EN0003_roar_1.wav", SD_BETA, 0.6f);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bRoarSound2)
	{
		m_bRoarSound2 = true;
		GI->PlaySoundW(L"se_EN0002_roar_1.wav", SD_BETA, 0.6f);

	}
}

void CDisasterBeta::Attack1_Update(_float fTimeDelta)
{
	m_bPush = false;
	m_bRHandCol = false;
	m_bLHandCol = false;

	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(0))
		LookAtPlayer();

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bEffectTime)
	{
		GI->PlaySoundW(L"se_EN0002_attack1.wav", SD_BETA, 0.4f);
		Create_Effect(ATTACK1_LEFT);
		Create_Effect(ATTACK1_RIGHT);
		m_bEffectTime = true;
	}
		

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(1) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(2) && !m_bAttackCol)
	{
		m_bRHandCol = true;
		m_bLHandCol = true;
	}
}

void CDisasterBeta::Attack2_Update(_float fTimeDelta)
{
	m_bPush = false;
	m_bRHandCol = false;

	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(0))
		LookAtPlayer();
	
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bAttack2Col)
	{
		GI->PlaySoundW(L"se_EN0002_attack2.wav", SD_BETA, 0.4f);
		m_bRHandCol = true;
		m_bAttack2Col = true;
	}
}

void CDisasterBeta::Attack3_Update(_float fTimeDelta)
{
	m_bPush = false;
	m_bRHandCol = false;
	m_bLHandCol = false;

	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(0) ||
		m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1))
		LookAtPlayer();
	
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bAttack3Col_1)
	{
		m_bRHandCol = true;
		m_bAttack3Col_1 = true;
	}
	
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(3) && !m_bAttack3Col_2)
	{
		GI->PlaySoundW(L"se_EN0002_attack3.wav", SD_BETA, 0.4f);
		m_bLHandCol = true;
		m_bAttack3Col_2 = true;
		Create_Effect(ATTACK3_LEFT);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(4) && !m_bEffectTime3)
	{
		GI->PlaySoundW(L"se_EN0002_attack3.wav", SD_BETA, 0.4f);
		Create_Effect(ATTACK3_RIGHT);
		m_bEffectTime3 = true;
	}
}

void CDisasterBeta::Attack4_1Update(_float fTimeDelta)
{
	LookAtPlayer();
	m_bPush = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bAttack4_1Sound)
	{
		m_bAttack4_1Sound = true;

		GI->PlaySoundW(L"se_EN0002_roar_1.wav", SD_BETA, 0.4f);
		m_bAttackCol = false;
	}


}

void CDisasterBeta::Attack4_2Update(_float fTimeDelta)
{
	m_bPush = false;
	m_bLHandCol = false;
	m_bRHandCol = false;
	
	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(0) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(1) && !m_bAttackCol)
	{
		m_bLHandCol = true;
	}
	
	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(2) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(3) && !m_bAttackCol)
	{
		m_bRHandCol = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(4))
	{
		m_bAttackCol = false;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(5) && !m_bEffectTime4_1)
	{
		GI->PlaySoundW(L"se_EN0002_attack3.wav", SD_BETA, 0.4f);
		m_bEffectTime4_1 = false;
		Create_Effect(ATTACK4_2LEFT);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(6) && !m_bEffectTime4_2)
	{
		GI->PlaySoundW(L"se_EN0002_attack3.wav", SD_BETA, 0.4f);
		m_bEffectTime4_2 = false;
		Create_Effect(ATTACK4_2RIGHT);
	}

	LookAtPlayer();
}

void CDisasterBeta::Attack4_3Update(_float fTimeDelta)
{
	m_bPush = false;
	m_bRHandCol = false;

	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(0))
		LookAtPlayer();

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(0) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(1) && !m_bAttackCol)
		m_bRHandCol = true;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bEffectTime4_3)
	{
		GI->PlaySoundW(L"se_EN0002_attack4_2.wav", SD_BETA, 0.4f);
		Create_Effect(ATTACK4_3_RIGHT);
		m_bEffectTime4_3 = true;
	}
		
}

void CDisasterBeta::AvoidAttack_Update(_float fTimeDelta)
{
	m_bPush = false;
	m_bLHandCol = false;

	//일정 키프레임 이상일때 맞게 함

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(0) && 
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(1) &&
		!m_bAttackCol)
	{
		m_bLHandCol = true;
	}

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(2) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(3) &&
		!m_bAttackCol)
	{
		LookAtPlayer();
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(4) && !m_bEffectTime5)
	{
		GI->PlaySoundW(L"se_EN0002_attack3.wav", SD_BETA, 0.4f);
		Create_Effect(AVOIDATTACK_LEFT);
		m_bEffectTime5 = true;
	}


	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(5) && !m_bAvoidAttackSound)
	{
		m_bAvoidAttackSound = true;
		GI->PlaySoundW(L"se_EN0002_attack4_1.wav", SD_BETA, 0.4f);
		
	}
}

void CDisasterBeta::HitBack_Update(_float fTimeDelta)
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

void CDisasterBeta::HitFly_Update(_float fTimeDelta)
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

void CDisasterBeta::HitFront_Update(_float fTimeDelta)
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

void CDisasterBeta::HitDown_Update(_float fTimeDelta)
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

void CDisasterBeta::HitHori_Update(_float fTimeDelta)
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

void CDisasterBeta::HitSpin_Update(_float fTimeDelta)
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

void CDisasterBeta::HitUp_Update(_float fTimeDelta)
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

void CDisasterBeta::HitSlapdown_Update(_float fTimeDelta)
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

void CDisasterBeta::Run_Update(_float fTimeDelta)
{
	m_fSpeed = 6.f;
	LookAtPlayer();
	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), m_fSpeed, fTimeDelta, m_pNavigationCom);
}

void CDisasterBeta::Walk_Update(_float fTimeDelta)
{
	m_fSpeed = 2.f;
	LookAtPlayer();
	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), m_fSpeed, fTimeDelta, m_pNavigationCom);
}

void CDisasterBeta::WalkBack_Update(_float fTimeDelta)
{
	m_fSpeed = 2.f;
	LookAtPlayer();
}

void CDisasterBeta::Strafe_Update(_float fTimeDelta)
{
	LookAtPlayer();
}

void CDisasterBeta::Stun_Update(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bWafer1)
	{
		m_bWafer1 = true;

		//Create_Item(DIE_STUN);
	}

}

void CDisasterBeta::LookAtPlayer(_float fLiearTime)
{
	Set_Dir();
	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&m_vTargetLook), fLiearTime);

}


HRESULT CDisasterBeta::Ready_Components()
{
	//m_pEmissiveCom
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Beta_Emi"), TEXT("Com_TextureEmi"), (CComponent**)&m_pEmissiveCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("DisasterBeta"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	m_pModelCom->Set_RootHierarchy(3);


	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.1f, 1.f, 1.f);
	ColDesc.vCenter = _float3(0.f, 0.4f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Push_Range"); // 밀어내기 위함

	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	//상체쪽
	CHierarchyNode*		pNeck = m_pModelCom->Get_HierarchyNode("Neck0_M");
	if (nullptr == pNeck)
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.3f, 1.3f, 1.3f);
	ColDesc.vCenter = _float3(-0.1f, 0.4f, 0.f);
	ColDesc.pSocket = pNeck;
	XMStoreFloat4x4(&ColDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	ColDesc.pOwner = this;
	ColDesc.iFlag = 0;
	strcpy(ColDesc.sTag, "Monster_Body1"); // 기본 충돌체 - 몬스터끼리 밀어내고 플레이어 또는 플레이어 공격이랑 충돌하기위한거

	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	//오른손
	CHierarchyNode*		pRightHand = m_pModelCom->Get_HierarchyNode("MiddleFinger2_R");
	if (nullptr == pRightHand)
		return E_FAIL;
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.2f, 1.1f, 1.1f);
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.pSocket = pRightHand;
	XMStoreFloat4x4(&ColDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	ColDesc.pOwner = this;
	ColDesc.iFlag = 0;
	strcpy(ColDesc.sTag, "Monster_RHand"); //몬슽터 공격용 충돌체
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	//왼손

	CHierarchyNode*		pLeftHand = m_pModelCom->Get_HierarchyNode("MiddleFinger2_L");
	if (nullptr == pLeftHand)
		return E_FAIL;
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.2f, 1.1f, 1.1f);
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

HRESULT CDisasterBeta::Create_Navigation(_uint iNaviNum)
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


HRESULT CDisasterBeta::Ready_Layer_UI(const _tchar * pLayerTag)
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

void CDisasterBeta::Set_PreBar(_float fTimeDelta)
{
	if(m_bReGauge)
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

CDisasterBeta * CDisasterBeta::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDisasterBeta*		pInstance = new CDisasterBeta(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CDisasterBeta"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDisasterBeta::Clone(void * pArg)
{
	CDisasterBeta*		pInstance = new CDisasterBeta(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CDisasterBeta"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDisasterBeta::Free()
{
	__super::Free();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pDissolveTexture);
	Safe_Release(m_pEmissiveCom);
}
