#include "stdafx.h"
#include "..\Public\DisasterFusion.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "ToolManager.h"
#include "Monster_Manager.h"
#include "Time_Manager.h"
#include "Camera_Manager.h"
#include "UI.h"
#include "SwordTrail.h"
#include "SwordTrailObj.h"
#include "FusionThunder.h"
#include "FusionTrail.h"
#include "Particle_Manager.h"
#include "DropItem.h"
#include "EffectPatternGauge.h"
#include "Particle_Manager.h"
#include "Player01HitEffect_1.h"
#include "Player01HitEffect_2.h"
#include "Crack_Manager.h"


CDisasterFusion::CDisasterFusion(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CDisasterFusion::CDisasterFusion(const CDisasterFusion & rhs)
	: CCreture(rhs)
{
}

HRESULT CDisasterFusion::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CDisasterFusion::Initialize(void * pArg)
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

	Ready_Trails();
	Ready_Sockets();

	m_sTag = "Tag_Monster";
	// *[HeroJho]: ImGui로 디버깅을 하려면 꼭 호출해줘야 합니다.
	Make_ImGui();

	m_eCurState = SPAWN;
	m_pModelCom->Set_AnimIndex(SPAWN);


	m_fPlayerPos = { 0.f,0.f,0.f, 1.f }; //일단 초기화
	m_pTransformCom->Set_Scale(XMVectorSet(0.7f, 0.7f, 0.7f, 1.f));

	m_fHurtTime = 0.02f;

	switch (m_iIndex)
	{
	case 2:
		m_fMaxHp = 2500.f;
		m_fStamina = 1.f;
		m_fDelayTime = 2.8f;
		m_fPlusDamage = 0.f;
		break;
	case 3:
		m_fMaxHp = 3000.f;
		m_fStamina = 1.f;
		if(CreateInfo.vPosition.x == 18.478f)
			m_fDelayTime = 1.4f;
		else
			m_fDelayTime = 2.f;
		m_fPlusDamage = 20.f;
		break;
	case 4:
		m_fMaxHp = 3000.f;
		m_fPlusDamage = 20.f;
		m_fStamina = 2.f;
		m_fDelayTime = 1.f;
		break;
	case 5:
		m_fMaxHp = 3000.f;
		m_fPlusDamage = 20.f;
		m_fStamina = 2.f;
		m_fDelayTime = 2.3f;
		m_fPlusDamage = 60.f;
		break;
	case 6:
		m_fMaxHp = 2400.f;
		m_fPlusDamage = 90.f;
		m_fStamina = 1.f;
		m_fDelayTime = 1.8f;
		break;
	case 7:
		m_fMaxHp = 3500.f;
		m_fPlusDamage = 60.f;
		m_fStamina = 1.f;
		if (CreateInfo.vPosition.x == -126.930f)
			m_fDelayTime = 1.2f;
		else
			m_fDelayTime = 1.7f;
		break;
	case 8:
		m_fMaxHp = 4000.f;
		m_fPlusDamage = 100.f;
		m_fStamina = 1.f;
		m_fDelayTime = 1.5f;
		break;
	case 9:
		m_fMaxHp = 3000.f;
		m_fPlusDamage = 60.f;
		m_fStamina = 1.f;
		m_fDelayTime = 3.f;
		break;
	case 10:
		m_fMaxHp = 1000.f;
		m_fStamina = 1.f;
		m_fDelayTime = 0.8f;
		m_fPlusDamage = 0.f;
		break;
	default:
		m_fMaxHp = 3000.f;
		m_fPlusDamage = 20.f;
		m_fStamina = 2.f;
		m_fDelayTime = 1.f;
		break;

	}

	m_fMaxHp = 3000.f;
	m_fPlusDamage = 20.f;
	m_fStamina = 2.f;
	m_fDelayTime = 1.f;

	m_fNowHp = m_fMaxHp;
	m_fDamage = 50.f;

	m_fPreHp = m_fNowHp;
	m_fMaxPg = m_fMaxHp * 0.4f;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Dissolve"), TEXT("Com_Texture"), (CComponent**)&m_pDissolveTexture)))
		return E_FAIL;


	if (m_iIndex != 6)
	{
		MM->Add_DisasterFusion(this);

		if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
			return E_FAIL;
	}
	else if (6 == m_iIndex)
	{
		MM->Add_Haki(this);
	}

	return S_OK;
}

void CDisasterFusion::Tick(_float fTimeDelta)
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

	Create_SwordTrail(fTimeDelta);
}

void CDisasterFusion::LateTick(_float fTimeDelta)
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
		
			
		Choose_Col("Monster_Push_Range");

		if (m_bHurt) //피격
		{
			Choose_Col("Monster_Body1");
		}

		Tick_Col(m_pTransformCom->Get_WorldMatrix());


		for (auto& iter : m_SwordTrails)
			iter->LateTick(fTimeDelta);

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

		Render_Col(m_pRendererCom);

	}
}

HRESULT CDisasterFusion::Render()
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
	// m_fBlurPower = 1.f;
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
void CDisasterFusion::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	// IG->Debug_Transform(m_pTransformCom);

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

	_float3 vSize = Get_Colliders("Monster_RHand")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Monster_RHand")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Get_Colliders("Monster_RHand")->Set_Size(vSize);
	Get_Colliders("Monster_RHand")->Set_Center(vCenter);


	ImGui::End();
}

// *[HeroJho]: 충돌이 발생할 때 호출되는 함수입니다.
void CDisasterFusion::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	// *[HeroJho]:
	// Desc.pOther		- 충돌한 상대편 객체의 주소입니다.
	// Desc.pMyDesc		- 상대편과 충돌한 나의 콜라이더 정보입니다.
	// Desc.pOtherDesc	- 나와 충돌한 상대편의 콜라이더 정보입니다.
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		if (!strcmp("Monster_Push_Range", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Push_Range", Desc.pOtherDesc->sTag))
		{
			
			m_pTransformCom->PushMeXZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION), Desc.pMyDesc->vSize.x, XMLoadFloat4(&((CCreture*)Desc.pOther)->Get_WorldPos()), Desc.pOtherDesc->vSize.x, TM->Get_TimeDelta(), m_pNavigationCom);
			
		}
		if (strcmp("Monster_Body1", Desc.pMyDesc->sTag) && 
			strcmp("Monster_Push_Range", Desc.pMyDesc->sTag) && !strcmp("Player_Body", Desc.pOtherDesc->sTag))
		{
			if (!m_bAttackCol)
				m_bAttackCol = true;

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

void CDisasterFusion::On_Hit(CCreture* pOther)
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
			m_SwordTrails[STR_RHAND]->TrailOff();
			m_SwordTrails[STR_LHAND]->TrailOff();
			_float4 vPos;
			XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			vPos.y += 1.f;
			pOther->Create_HitEffect(vPos);

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

			m_bHurt = false;
			m_bAction = false;
			m_bRHandCol = false;
			m_bLHandCol = false;

			if (m_iIndex != 6)
				m_fNowHp -= (pOther->Get_Damage() - m_fStamina);

			if (MM->Get_MonsterCount() == 1 && m_fNowHp <= 0.f && m_iIndex != 6)
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
				m_fPatternGauge += (pOther->Get_Damage() * 0.5f);

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

void CDisasterFusion::Create_HitEffect(_float4 vWorldPos)
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

void CDisasterFusion::Control_DisasterFusion()
{
	m_bMove = true;
	m_fAttackTime = 0.f;
	m_bSpawn = true;
	m_bAttack = false;
	m_bOneCheck = false;
	m_bHitAble = true;
	m_bAction = true;
}

void CDisasterFusion::Set_State(STATE eState) //end 애니메이션에서
{
	//상태 보간 하는 함수
	if (m_eNextState == eState)
		return;

	m_eNextState = eState;

	switch (m_eNextState)
	{
		//다음 상태로 넘어갈때 해줘야하는것들
		//애니메이션x
	case Client::CDisasterFusion::ATTACK1:
		m_iEffectIndex = 0;
		m_fEffectSpeed = 0.043f;
		m_fEffectScale = 1.4f;
		Create_Effect(EFFECT_THUNDER);
		On_RHandTrail();
		m_bWalkBack = true;
		m_bAttackCol= false;
		m_bAttack1Effect = false;
		m_eHitType = TYPE_AWAY;
		m_fPushPower = 14.f;
		m_fAirPower = 5.f;
		m_fDamage = 35.f + m_fPlusDamage;
		LookAtPlayer(0.25f);
		break;
	case Client::CDisasterFusion::ATTACK2:
		m_bAttackCol = false;
		m_bWalkBack = false;
		m_bAttack2Trail = false;
		m_bAttack2Effect_1 = false;
		m_bAttack2Effect_2 = false;
		m_bAttack2Effect_3 = false;
		break;
	case Client::CDisasterFusion::ATTACK3_1:
		m_bWalkBack = false;
		m_bAttack3Effect_1 = false;
		m_bAttack3Effect_2 = false;
		break;
	case Client::CDisasterFusion::BOUNCE:
		m_bFly = true;
		break;
	case Client::CDisasterFusion::HITBACK:
		m_bFly = false;
		break;
	case Client::CDisasterFusion::HITFLY:
		m_bFly = true;
		m_bAttackCol = false;
		if (m_fHAirPower == 0.f)
			m_pTransformCom->ResetAndJump(1.f);
		else
			m_pTransformCom->ResetAndJump(m_fHAirPower);
		break;
	case Client::CDisasterFusion::HITFRONT:
		m_bFly = false;
		m_bAttackCol = false;
		break;
	case Client::CDisasterFusion::HITDOWN:
		m_bFly = true;
		break;
	case Client::CDisasterFusion::HITUP:
		m_bFly = true;
		m_bAttackCol = false;
		break;
	case Client::CDisasterFusion::HITSPIN:
		m_bFly = true;
		m_bAttackCol = false;
		break;
	case Client::CDisasterFusion::HITHORI:
		HitAwayPower_Cehck();
		m_bFly = true;
		m_bAttackCol = false;
		break;
	case Client::CDisasterFusion::HITSLAPDOWN:
		m_bFly = true;
		m_bAttackCol = false;
		break;
	case Client::CDisasterFusion::DOWN:
		m_bFly = true;
		break;
	case Client::CDisasterFusion::GETUP:
		m_bFly = false;
		break;
	case Client::CDisasterFusion::SPAWN:
		break;
	case Client::CDisasterFusion::IDLE:
		if (m_iIndex == 3 || m_iIndex == 5 || m_iIndex == 6 || m_iIndex == 7 || m_iIndex == 8 || m_iIndex == 9 || m_iIndex == 10)
			Control_DisasterFusion();
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
	case Client::CDisasterFusion::STUNBEGIN:
		break;
	case Client::CDisasterFusion::STUN:
		break;
	case Client::CDisasterFusion::STUNRECOVER:
		break;
	case Client::CDisasterFusion::STRAFELEFT:
		break;
	case Client::CDisasterFusion::STRAFERIGHT:
		break;
	case Client::CDisasterFusion::RUN:
		m_bAttack = false;
		break;
	case Client::CDisasterFusion::WALKBACK:
		LookAtPlayer();
		m_bHurt = true;
		m_bHitAble = true;
		break;
	case Client::CDisasterFusion::WALKFRONT:
		break;

	}


	m_pModelCom->SetNextIndex(m_eNextState);
	m_pModelCom->SetChangeBool(true); 
	m_eCurState = m_eNextState; //보간시작할때 상태를 바꿔줌

}

void CDisasterFusion::Hit_State(CCreture * pOther)
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


void CDisasterFusion::End_Animation()
{
	if (m_pModelCom->GetAniEnd())
	{
		//애니메이션 끝났을때
		switch (m_eCurState)
		{
			//다음 상태로 넘어갈때 해줘야하는것들
			//애니메이션x
		case Client::CDisasterFusion::ATTACK1:
			GI->PlaySoundW(L"se_EN0009_attack1_1.wav", SD_FUSION, 0.4f);
			Set_State(STATE::WALKBACK);
			break;
		case Client::CDisasterFusion::ATTACK2:
			Set_State(STATE::WALKBACK);
			break;
		case Client::CDisasterFusion::ATTACK3_1:
			m_pModelCom->Set_AnimIndex(STATE::ATTACK3_2);
			m_eCurState = STATE::ATTACK3_2;
			m_iEffectIndex = 0;
			m_fEffectSpeed = 0.033f;
			m_fEffectScale = 1.8f;
			Create_Effect(EFFECT_THUNDER);
			break;
		case Client::CDisasterFusion::ATTACK3_2:
			m_pModelCom->Set_AnimIndex(STATE::ATTACK3_3);
			m_eCurState = STATE::ATTACK3_3;
			m_bAttackCol = false;
			m_bAttack3_3Trail = false;
			m_iEffectIndex = 0;
			m_fEffectSpeed = 0.033f;
			m_fEffectScale = 1.6f;
			Create_Effect(EFFECT_THUNDER);
			m_bAttack2On = false;
			break;
		case Client::CDisasterFusion::ATTACK3_3:
			m_bAttack3On = false;
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
			break;
		case Client::CDisasterFusion::BOUNCE:
			Set_State(DOWN);
			break;
		case Client::CDisasterFusion::HITBACK:
			if (m_bDeadStart)
				Set_State(STUN);
			else
			{
				if (m_bAvoidAttack)
				{
					Set_State(ATTACK3_1);
				}
				else
				{
					m_fAttackTime = 0.f;
					m_bAttack = false;
					m_bOneCheck = false;
					m_bWalkBack = false;
					Set_State(IDLE);
				}
			}
			break;
		case Client::CDisasterFusion::HITFRONT:
			if (m_bDeadStart)
				Set_State(STUNBEGIN);
			else
			{
				if (m_bAvoidAttack)
				{
					Set_State(ATTACK3_1);
				}
				else
				{
					m_fAttackTime = 0.f;
					m_bAttack = false;
					m_bOneCheck = false;
					m_bWalkBack = false;
					Set_State(IDLE);
				}
			}
			break;
		case Client::CDisasterFusion::HITFLY:
			Set_State(HITDOWN);
			break;
		case Client::CDisasterFusion::HITDOWN:
			break;
		case Client::CDisasterFusion::HITSPIN:
			Set_State(HITDOWN);
			break;
		case Client::CDisasterFusion::HITHORI:
			Set_State(HITDOWN);
			break;
		case Client::CDisasterFusion::HITUP:
			Set_State(HITDOWN);
			break;
		case Client::CDisasterFusion::HITSLAPDOWN:
			Set_State(HITDOWN);
			break;
		case Client::CDisasterFusion::DOWN:
			if (!m_bDeadStart)
				Set_State(GETUP);
			else
				m_bDissolve = true;
			break;
		case Client::CDisasterFusion::GETUP:
			if (m_bAvoidAttack)
			{
				Set_State(ATTACK3_1);
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
		case Client::CDisasterFusion::SPAWN:
			Set_State(IDLE);
			break;
		case Client::CDisasterFusion::IDLE:
			if (m_bMove)
			{
				m_fAttackTime = 0.f;
				m_bAttack = false;
				m_bOneCheck = false;
				m_bWalkBack = false;
				m_bHitAble = true;
				m_bAction = true;
			}
			break;
		case Client::CDisasterFusion::STUNBEGIN:
			Set_State(STUN);
			break;
		case Client::CDisasterFusion::STUN:
			if (!m_bDeadStart)
				Set_State(STUNRECOVER);
			else
				m_bDissolve = true;
			break;
		case Client::CDisasterFusion::STUNRECOVER:
			Set_State(IDLE);
			break;
		case Client::CDisasterFusion::STRAFELEFT:
			m_bAttack = false;
			m_bOneCheck = false;
			m_bWalkBack = false;
			Set_State(IDLE);
			break;
		case Client::CDisasterFusion::STRAFERIGHT:
			m_bAttack = false;
			m_bOneCheck = false;
			m_bWalkBack = false;
			Set_State(IDLE);
			break;
		case Client::CDisasterFusion::RUN:
			break;
		case Client::CDisasterFusion::WALKBACK:
			if (m_bWalkBack)
			{
				m_bWalkBack = false;
				m_bHitAble = true;
				Set_State(WALKBACK);
			}
			else
				Set_State(IDLE);
			break;
		case Client::CDisasterFusion::WALKFRONT:
			break;

		default:
			break;

		}

		

	}
}


void CDisasterFusion::Update(_float fTimeDelta)
{
	m_bPush = true;

	if (!m_pModelCom->GetChangeBool())
	{
		switch (m_eCurState)
		{
			//다음 상태로 넘어갈때 해줘야하는것들
			//애니메이션x
		case Client::CDisasterFusion::ATTACK1:
			Attack1_Update(fTimeDelta);
			break;
		case Client::CDisasterFusion::ATTACK2:
			Attack2_Update(fTimeDelta);
			break;
		case Client::CDisasterFusion::ATTACK3_1:
			Attack3_1Update(fTimeDelta);
			break;
		case Client::CDisasterFusion::ATTACK3_2:
			Attack3_2Update(fTimeDelta);
			break;
		case Client::CDisasterFusion::ATTACK3_3:
			Attack3_3Update(fTimeDelta);
			break;
		case Client::CDisasterFusion::BOUNCE:
			break;
		case Client::CDisasterFusion::HITBACK:
			HitBack_Update(fTimeDelta);
			break;
		case Client::CDisasterFusion::HITFLY:
			HitFly_Update(fTimeDelta);
			break;
		case Client::CDisasterFusion::HITHORI:
			HitHori_Update(fTimeDelta);
			break;
		case Client::CDisasterFusion::HITFRONT:
			HitFront_Update(fTimeDelta);
			break;
		case Client::CDisasterFusion::HITDOWN:
			HitDown_Update(fTimeDelta);
			break;
		case Client::CDisasterFusion::HITSPIN:
			HitSpin_Update(fTimeDelta);
			break;
		case Client::CDisasterFusion::HITUP:
			HitUp_Update(fTimeDelta);
			break;
		case Client::CDisasterFusion::HITSLAPDOWN:
			HitSlapdown_Update(fTimeDelta);
			break;
		case Client::CDisasterFusion::DOWN:
			break;
		case Client::CDisasterFusion::GETUP:
			break;
		case Client::CDisasterFusion::SPAWN:
			SpawnRoar_Update(fTimeDelta);
			
			break;
		case Client::CDisasterFusion::IDLE:
			LookAtPlayer();
			break;
		case Client::CDisasterFusion::STUNBEGIN:
			if (!m_bDeadStart)
				m_bRangeCol = true;
			break;
		case Client::CDisasterFusion::STUN:
			if (!m_bDeadStart)
				m_bRangeCol = true;
			break;
		case Client::CDisasterFusion::STUNRECOVER:
			m_bRangeCol = true;
			break;
		case Client::CDisasterFusion::STRAFELEFT:
			Strafe_Update(fTimeDelta);
			break;
		case Client::CDisasterFusion::STRAFERIGHT:
			Strafe_Update(fTimeDelta);
			break;
		case Client::CDisasterFusion::RUN:
			Run_Update(fTimeDelta);
			break;
		case Client::CDisasterFusion::WALKBACK: //텀주기 위한 상태, 뒤로 살짝 물러남, 애니메이션 자체가 뒤로 물러나므로 트랜스폼함수 필요없음
			WalkBack_Update(fTimeDelta);
			break;
		case Client::CDisasterFusion::WALKFRONT:
			Walk_Update(fTimeDelta);
			break;

		default:
			break;
		}

	}

}

void CDisasterFusion::Set_Dir()
{
	//플레이어 포스 받아오기 미리 받아둘지

	//내가 바라봐야할 룩벡터 구하기, 플레이어 포스에서 내포스 빼고 노멀라이즈
	_vector vLook = XMVector3Normalize(XMLoadFloat4(&m_fPlayerPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	XMStoreFloat3(&m_vTargetLook, vLook);


}

void CDisasterFusion::LookDirPlayer()
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


void CDisasterFusion::Distance_BattlePattern(_float fTimeDelta)
{
	m_fPlayerPos = PM->Get_PlayerPos();

	m_fDistance = m_pTransformCom->TargetDistance(XMLoadFloat4(&m_fPlayerPos)); //타겟과 나의 거리 계속 계산

	
	ActionToPlayer(fTimeDelta); //플레이어랑 관련된 액션들
	
	RegretToPlayer(fTimeDelta);
}

void CDisasterFusion::AttackPattern(_float fTimeDelta)
{
	//공격할수있는 상태 - AttackTime이 지정시간 이상이어야되고, 맞는 애니메이션이 끝났을때여야하고, 
	m_fAttackTime += 1.f * fTimeDelta; //애니메이션이 다 끝나거나 hit 상태일때 m_fAttackTime 0으로

	if (m_fAttackTime >= 0.15f) 
	{
		m_bAttack = true;

		if (m_fDistance <= 13.f)
			CloseAttack(fTimeDelta);
	
	}

}

void CDisasterFusion::ActionToPlayer(_float fTimeDelta)
{
	if (m_bSpawn && m_bAction)
	{
		if (m_fDistance <= 50.f)
		{
			//거리에 따라서 걸을지 뛸지
			if (!m_bAttack)
			{
				if (m_fDistance > 6.f)
				{
					if (!m_bOneCheck)
						Set_State(STATE::RUN);
				}
				else
					Set_State(STATE::WALKBACK);
			}

			if (m_fDistance <= 15.f)
				AttackPattern(fTimeDelta);
		}
		else
			Set_State(RUN);
	}
	
}

void CDisasterFusion::RegretToPlayer(_float fTimeDelta)
{
	if (m_bSpawn && !m_bMove)
	{
		Set_State(IDLE);
	}
}

void CDisasterFusion::Check_Rim(_float fTimeDelta)
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
		m_iShaderPath = 4;// 3;
	}

}

void CDisasterFusion::Check_Death(_float fTimeDelta)
{

	if (!m_bMinusCount && m_fNowHp <= 0.f)
	{
		m_bMinusCount = true;
		MM->Minus_MonsterCount();
		if (6 != m_iIndex)
			Create_Item();
		Event_Stage();
	}


	if (!m_pTransformCom->Get_Air() && m_fNowHp <= 0.f && !m_bDeadStart)
	{
		m_bAction = false;
		m_bHitAble = false;
		m_bRangeCol = false;
		m_bLHandCol = false;
		m_bRHandCol = false;
		m_bBodyCol = false;
		m_bDeadStart = true;
		GI->PlaySoundW(L"se_enemy_death1.wav", SD_FUSION, 0.7f);
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

void CDisasterFusion::Check_Avoid()
{
	if (m_fPrePatternGauge >= m_fMaxPg)
	{
		m_bAvoidAttack = true;
		m_bAction = false;
		m_bHitAble = false;


	}
}

void CDisasterFusion::HitAwayPower_Cehck()
{
	_uint iPushPower = (_uint)m_fHPushPower / 10;

	switch (iPushPower)
	{
	case 0:
		m_fHPushPower -= 4.f;
		break;
	case 1:
		m_fHPushPower -= 7.f;
		break;
	case 2:
		m_fHPushPower -= 16.f;
		break;
	case 3:
		m_fHPushPower -= 24.f;
		break;
	case 4:
		m_fHPushPower -= 32.f;
		break;
	}
}

void CDisasterFusion::Reset_Collider()
{
}

void CDisasterFusion::Event_Stage()
{
	switch (m_iIndex)
			{
			case 2:
				MM->Event_Stage2();
				break;
			case 3:
				MM->Event_Stage3_1();
				break;
			case 4: //도시
				MM->Event_Stage3_2();
				break;
			case 5: //우상단
				MM->Event_Stage4_1();
				break;
			case 6: //하키
				MM->Event_Stage4_2();
				break;
			case 7: //좌하단
				MM->Event_Stage4_3();
				break;
			case 8: //좌상단
				MM->Event_Stage4_4();
				break;
			case 9: //볼링장
				MM->Event_Stage4_5();
				break;
			default:
				break;
			}
}

void CDisasterFusion::CloseAttack(_float fTimeDelta)
{
	if (!m_bOneRandom)
	{
		m_bOneRandom = true;
		m_iRandomAttack = GI->Get_Random(1, 3);
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
	default:
		break;
	}
}

void CDisasterFusion::Attack1(_float fTimeDelta)
{
	if (m_fDistance > 9.f)
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
			Set_State(STATE::ATTACK1);

		}
	}

}

void CDisasterFusion::Attack2(_float fTimeDelta)
{
	if (m_fDistance > 7.f)
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

void CDisasterFusion::Attack3(_float fTimeDelta)
{
	if (m_fDistance > 10.f)
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
			Set_State(STATE::ATTACK3_1);

		}
	}
}

void CDisasterFusion::SpawnRoar_Update(_float fTimeDelta)
{

	LookAtPlayer();

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bSpawnSound)
	{
		GI->PlaySoundW(L"se_EN0009_roar_1.wav", SD_FUSION, 0.6f);
		m_bSpawnSound = true;
	}


}

void CDisasterFusion::Attack1_Update(_float fTimeDelta)
{
	m_bPush = false;
	m_bRangeCol = true;
	m_bRHandCol = false;
	m_bShake = true;
	//Create_Particle(fTimeDelta);
	
	if(m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(0))
		LookAtPlayer();

	
	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(0) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(1) && !m_bAttackCol)
		m_bRHandCol = true;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(6))
		m_SwordTrails[STR_RHAND]->TrailOff();

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bAttack1Effect)
	{
		GI->PlaySoundW(L"se_EN0009_attack1_2.wav", SD_FUSION, 0.4f);
		m_bAttack1Effect = true;
		m_iEffectIndex = 2;
		m_fEffectSpeed = 0.033f;
		m_fEffectScale = 1.2f;
		Create_Effect(EFFECT_THUNDER);
	}



}

void CDisasterFusion::Attack2_Update(_float fTimeDelta)
{
	m_bPush = false;
	m_bRangeCol = true;
	m_bRHandCol = false;
	m_bLHandCol = false;
	m_bShake = false;
	m_pModelCom->Set_TickPerSecond(29.2f);

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0))
		LookAtPlayer(0.9f);

	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(2))
		LookAtPlayer();

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(3) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(4) && !m_bAttackCol)
	{
		m_eHitType = TYPE_HIT;
		m_fPushPower = 10.f;
		m_fAirPower = 0.f;
		m_fDamage = 20.f + m_fPlusDamage;
		m_bLHandCol = true;
		if (!m_bAttack2On)
		{
			GI->PlaySoundW(L"se_EN0009_attack2_1.wav", SD_FUSION, 0.4f);
			m_bAttack2On = true;
		}
	}
	
	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(5) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(6) && !m_bAttackCol)
	{
		m_eHitType = TYPE_HIT;
		m_fPushPower = 10.f;
		m_fAirPower = 0.f;
		m_fDamage = 20.f + m_fPlusDamage;
		m_bRHandCol = true;
		if (m_bAttack2On)
		{
			GI->PlaySoundW(L"se_EN0009_attack2_1.wav", SD_FUSION, 0.4f);
			m_bAttack2On = false;
		}
	}
	
	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(7) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(8) && !m_bAttackCol)
	{
		m_eHitType = TYPE_SPIN;
		m_fPushPower = 15.f;
		m_fAirPower = 10.f;
		m_fDamage = 20.f + m_fPlusDamage;
		m_bLHandCol = true;
		m_bShake = true;
		if (!m_bAttack2On)
		{
			GI->PlaySoundW(L"se_EN0009_attack2_2.wav", SD_FUSION, 0.4f);
			m_bAttack2On = true;
		}
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(9) ||
		m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(10))
		m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(11) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(12))
		LookAtPlayer();

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(13) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(14))
		m_pModelCom->Set_TickPerSecond(20.f);


	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(15) && !m_bAttack2Trail)
	{
		m_bAttack2Trail = true;

		On_RHandTrail();
		On_LHandTrail();
	}


	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(16))
	{
		m_SwordTrails[STR_RHAND]->TrailOff();
		m_SwordTrails[STR_LHAND]->TrailOff();
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(4) && !m_bAttack2Effect_1)
	{
		m_iEffectIndex = 1;
		m_fEffectSpeed = 0.03f;
		m_fEffectScale = 1.7f;
		Create_Effect(EFFECT_THUNDER);
	}
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(5) && !m_bAttack2Effect_2)
	{
		m_iEffectIndex = 0;
		m_fEffectSpeed = 0.03f;
		m_fEffectScale = 1.7f;
		Create_Effect(EFFECT_THUNDER);
	}
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(12) && !m_bAttack2Effect_3)
	{
		m_iEffectIndex = 2;
		m_fEffectSpeed = 0.03f;
		m_fEffectScale = 2.f;
		Create_Effect(EFFECT_THUNDER);
	}

}

void CDisasterFusion::Attack3_1Update(_float fTimeDelta)
{
	m_bPush = false;
	m_bRangeCol = true;

	if (m_bAvoidAttack)
	{
		if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(1))
			LookAtPlayer();
	}
	else
		LookAtPlayer();

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bAttack3Effect_1)
	{
		m_bAttack3Effect_1 = true;
		m_iEffectIndex = 0;
		m_fEffectSpeed = 0.033f;
		m_fEffectScale = 1.9f;
		Create_Effect(EFFECT_THUNDER);
		m_iEffectIndex = 1;
		Create_Effect(EFFECT_THUNDER);
		GI->PlaySoundW(L"se_EN0009_roar_1.wav", SD_FUSION, 0.4f);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bAttack3Effect_2)
	{
		m_bAttack3Effect_2 = true;

		if(m_bAvoidAttack) //테스트용 나중에 주석풀기
			Create_Effect(EFFECT_PATTERNGAUGE);
	}
}

void CDisasterFusion::Attack3_2Update(_float fTimeDelta)
{
	m_bPush = false;
	m_bRangeCol = true;
	LookAtPlayer();

	m_fRunSoundAcc += 1.f * fTimeDelta;

	if (!m_bOtherFoot)
	{
		if (m_fRunSoundAcc >= 0.4f)
		{
			GI->PlaySoundW(L"se_EN0009_step_2-03.wav", SD_FUSION, 0.15f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = true;
		}
	}

	else
	{
		if (m_fRunSoundAcc >= 0.4f)
		{
			GI->PlaySoundW(L"se_EN0009_step_2-04.wav", SD_FUSION, 0.15f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = false;
		}
	}
}

void CDisasterFusion::Attack3_3Update(_float fTimeDelta)
{
	m_bPush = false;
	m_bRangeCol = true;
	m_bRHandCol = false;

	if (!m_bAvoidAttack)
	{
		m_eHitType = TYPE_HIT;
		m_fPushPower = 10.f;
		m_fAirPower = 0.f;
		m_fDamage = 45.f + m_fPlusDamage;
		if (!m_bAttack3On)
		{
			GI->PlaySoundW(L"se_EN0009_attack2_2.wav", SD_FUSION, 0.4f);
			m_bAttack3On = true;
		}
	}
	else
	{
		m_eHitType = TYPE_AWAY;
		m_fPushPower = 12.f;
		m_fAirPower = 5.f;
		m_fDamage = 60.f + m_fPlusDamage;
		if (!m_bAttack3On)
		{
			GI->PlaySoundW(L"se_EN0009_attack2_2.wav", SD_FUSION, 0.4f);
			m_bAttack3On = true;
		}
	}
	
	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(0))
		LookAtPlayer();
	else if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(1) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(2) && !m_bAttackCol)
	{
		m_bRHandCol = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(3) && !m_bAttack3_3Trail)
	{
		m_bAttack3_3Trail = true;
		On_RHandTrail(); 
		m_iEffectIndex = 0;
		m_fEffectSpeed = 0.03f;
		m_fEffectScale = 1.7f;
		Create_Effect(EFFECT_THUNDER);
	}


	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(4))
	{
		m_SwordTrails[STR_RHAND]->TrailOff();
	}

}

void CDisasterFusion::HitBack_Update(_float fTimeDelta)
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

void CDisasterFusion::HitFly_Update(_float fTimeDelta)
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

void CDisasterFusion::HitFront_Update(_float fTimeDelta)
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

void CDisasterFusion::HitDown_Update(_float fTimeDelta)
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

void CDisasterFusion::HitHori_Update(_float fTimeDelta)
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

void CDisasterFusion::HitSpin_Update(_float fTimeDelta)
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

void CDisasterFusion::HitUp_Update(_float fTimeDelta)
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

void CDisasterFusion::HitSlapdown_Update(_float fTimeDelta)
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

void CDisasterFusion::Run_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	m_fSpeed = 0.8f;
	LookAtPlayer();
	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), m_fSpeed, fTimeDelta, m_pNavigationCom);

	m_fRunSoundAcc += 1.f * fTimeDelta;

	if (!m_bOtherFoot)
	{
		if (m_fRunSoundAcc >= 0.5f)
		{
			GI->PlaySoundW(L"se_EN0009_step_1-01.wav", SD_FUSION, 0.1f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = true;
		}
	}

	else
	{
		if (m_fRunSoundAcc >= 0.5f)
		{
			GI->PlaySoundW(L"se_EN0009_step_1-02.wav", SD_FUSION, 0.1f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = false;
		}
	}
}

void CDisasterFusion::Walk_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	m_fSpeed = 0.5f;
	LookAtPlayer();
	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), m_fSpeed, fTimeDelta, m_pNavigationCom);

	m_fRunSoundAcc += 1.f * fTimeDelta;

	if (!m_bOtherFoot)
	{
		if (m_fRunSoundAcc >= 0.73f)
		{
			GI->PlaySoundW(L"se_EN0009_step_1-01.wav", SD_FUSION, 0.1f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = true;
		}
	}

	else
	{
		if (m_fRunSoundAcc >= 0.73f)
		{
			GI->PlaySoundW(L"se_EN0009_step_1-02.wav", SD_FUSION, 0.1f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = false;
		}
	}
}

void CDisasterFusion::WalkBack_Update(_float fTimeDelta)
{
	m_fSpeed = 2.f;
	m_bRangeCol = true;
	LookAtPlayer();


	m_fRunSoundAcc += 1.f * fTimeDelta;

	if (!m_bOtherFoot)
	{
		if (m_fRunSoundAcc >= 0.73f)
		{
			GI->PlaySoundW(L"se_EN0009_step_2-01.wav", SD_FUSION, 0.1f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = true;
		}
	}

	else
	{
		if (m_fRunSoundAcc >= 0.73f)
		{
			GI->PlaySoundW(L"se_EN0009_step_2-02.wav", SD_FUSION, 0.1f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = false;
		}
	}
}

void CDisasterFusion::Strafe_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	LookAtPlayer();

	m_fRunSoundAcc += 1.f * fTimeDelta;

	if (!m_bOtherFoot)
	{
		if (m_fRunSoundAcc >= 0.73f)
		{
			GI->PlaySoundW(L"se_EN0009_step_2-01.wav", SD_FUSION, 0.1f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = true;
		}
	}

	else
	{
		if (m_fRunSoundAcc >= 0.73f)
		{
			GI->PlaySoundW(L"se_EN0009_step_2-02.wav", SD_FUSION, 0.1f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = false;
		}
	}
}

HRESULT CDisasterFusion::Create_Effect(EFFECT eET)
{

	switch (eET)
	{
	case Client::CDisasterFusion::EFFECT_THUNDER:
	{
		_float3 vScale;
		_float4 vPos;
		_matrix ThumbMatrix;

		switch (m_iEffectIndex)
		{
		case 0:
			ThumbMatrix = m_pSockets[SOCKET_RTHUMB]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
			break;
		case 1:
			ThumbMatrix = m_pSockets[SOCKET_LTHUMB]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
			break;
		case 2:
			ThumbMatrix = m_pSockets[SOCKET_RTUMBPOINT]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
			break;
		}

		XMStoreFloat4(&vPos, ThumbMatrix.r[3]);

		vScale.x = m_fEffectScale; vScale.y = m_fEffectScale;
		CFusionThunder::THUNDER1INFO* Info = new CFusionThunder::THUNDER1INFO;
		Info->vPos = vPos;
		Info->vScale = vScale;
		Info->fSpeed = m_fEffectSpeed;
		if (FAILED(GI->Add_GameObjectToLayer(L"FusionThunder", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info)))
			return E_FAIL;

	}
	break;

	case Client::CDisasterFusion::EFFECT_TRAIL:
	{
		CFusionTrail::TRAILINFO* Info = new CFusionTrail::TRAILINFO;
		Info->vColor = { 1.f,0.964f,0.807f,1.f };
		Info->vColor2 = { 0.745f,0.207f,0.180f,1.f };
		Info->vRatio = { 1.f,3.f };
		Info->vRatioSpeed = { 1.f, 0.f };
		Info->fBlurPower = { 0.5f };
		Info->fScale = { 1.f,1.f, 1.f };
		_matrix ThumbMatrix;
		ThumbMatrix = m_pSockets[SOCKET_RTUMBPOINT2]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();

		XMStoreFloat4x4(&Info->vWorldMatrix, ThumbMatrix);
		if (FAILED(GI->Add_Get_GameObjectToLayer(L"FusionTrail", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info, &m_pTrailEffect)))
			return E_FAIL;
	}
	break;

	case Client::CDisasterFusion::EFFECT_PATTERNGAUGE:
	{
		_matrix Effectmatrix;
		Effectmatrix = m_pSockets[SOKET_PG]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
		 
		Effectmatrix.r[0] = XMVector3Normalize(Effectmatrix.r[0]) * 0.4f;
		Effectmatrix.r[3] += Effectmatrix.r[0];
		Effectmatrix.r[1] = XMVector3Normalize(Effectmatrix.r[1]) * -0.2f;
		Effectmatrix.r[3] += Effectmatrix.r[1];
		Effectmatrix.r[2] = XMVector3Normalize(Effectmatrix.r[2]) * -0.2f;
		Effectmatrix.r[3] += Effectmatrix.r[2];

		_float4x4 fTempMatrix;

		XMStoreFloat4x4(&fTempMatrix, Effectmatrix);

		(PTM->Create_AlphaParticle(L"PatternGaugeCircle", fTempMatrix));
		(PTM->Create_AlphaParticle(L"PatternGaugeLine", fTempMatrix));
		(PTM->Create_AlphaParticle(L"PatternGaugeFlare", fTempMatrix));

		GI->PlaySoundW(L"se_enemy_warning.wav", SD_FUSION, 0.6f);

	}
	break;
	}

	return S_OK;
}

void CDisasterFusion::Create_Particle(_float fTimeDelta)
{

	m_fParticleAcc += 1.f * fTimeDelta;

	if (m_fParticleAcc > 0.5f)
		m_bParticle = true;

	if (m_bParticle)
	{
		m_bParticle = false;
		m_fParticleAcc = 0.f;
		
		(PTM->Create_AlphaParticle(L"FusionSmoke", m_pTransformCom->Get_WorldFloat4x4()));
	}


}

void CDisasterFusion::Create_SwordTrail(_float fTimeDelta)
{

	Control_RHandTrail(fTimeDelta);
	Control_LHandTrail(fTimeDelta);
	
}

void CDisasterFusion::Control_RHandTrail(_float fTimeDelta)
{
	_matrix RHandMatrix;

	RHandMatrix = m_pSockets[SOCKET_RHAND]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();

	if (m_SwordTrails[STR_RHAND]->Get_TrailOn())
		m_SwordTrails[STR_RHAND]->Tick(fTimeDelta, RHandMatrix);
}

void CDisasterFusion::Control_LHandTrail(_float fTimeDelta)
{
	_matrix LHandMatrix;

	LHandMatrix = m_pSockets[SOCKET_LHAND]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();


	if (m_SwordTrails[STR_LHAND]->Get_TrailOn())
		m_SwordTrails[STR_LHAND]->Tick(fTimeDelta, LHandMatrix);
}

void CDisasterFusion::On_LHandTrail()
{
	_matrix LHandMatrix;

	LHandMatrix = m_pSockets[SOCKET_LHAND]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
	m_SwordTrails[STR_LHAND]->TrailOn(LHandMatrix);

}

void CDisasterFusion::On_RHandTrail()
{
	_matrix RHandMatrix;
	RHandMatrix = m_pSockets[SOCKET_RHAND]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
	m_SwordTrails[STR_RHAND]->TrailOn(RHandMatrix);
}

void CDisasterFusion::Create_Item()
{
	_uint iRandomA = GI->Get_Random(0, 10);

	CDropItem::DROPITEMDESC DropItemDesc;
	DropItemDesc.iItemType = CDropItem::TYPE_WAFER;
	DropItemDesc.iNaviNum = GI->Get_Cur_LevelIndex();
	//DropItemDesc.vStartPosition = { 0.f,0.1f,5.f,1.f };
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
		DropItemDesc.iItemType = CDropItem::TYPE_BLADE;
		GI->Add_GameObjectToLayer(TEXT("DropItem"), GI->Get_Cur_LevelIndex(), TEXT("Monster_DropItem"), &DropItemDesc);
	}

	DropItemDesc.iItemType = CDropItem::TYPE_MONEY;
	GI->Add_GameObjectToLayer(TEXT("DropItem"), GI->Get_Cur_LevelIndex(), TEXT("Monster_DropItem"), &DropItemDesc);
}

void CDisasterFusion::Delay_Spawn(_float fTimeDelta)
{
	if (!m_bCreate)
	{
		m_fCreateAcc += 1.f * fTimeDelta;

		if (m_fCreateAcc >= (m_fDelayTime - 0.3f) && !m_bSpawnEffect)
		{
			m_bSpawnEffect = true;

			GI->PlaySoundW(L"se_enemy_spawn1.wav", SD_FUSION, 0.5f);
			PTM->Create_InstanceParticle(L"MonsterAppear", m_pTransformCom->Get_WorldFloat4x4());
			PTM->Create_InstanceParticle(L"MonsterAppear_2", m_pTransformCom->Get_WorldFloat4x4());
		}

		if (m_fCreateAcc >= m_fDelayTime)
		{
			m_bCreate = true;
		}
	}
}


void CDisasterFusion::LookAtPlayer(_float fLiearTime)
{
	Set_Dir();
	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&m_vTargetLook), fLiearTime);

}

HRESULT CDisasterFusion::Ready_Components()
{
	//m_pEmissiveCom
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Fusion_Emi"), TEXT("Com_TextureEmi"), (CComponent**)&m_pEmissiveCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("DisasterFusion"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
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

	CHierarchyNode*		pSpine = m_pModelCom->Get_HierarchyNode("backfire_01_M"); //상체쪽
	if (nullptr == pSpine)
		ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.2f, 1.2f, 1.2f);
	ColDesc.vCenter = _float3(-0.6f, -0.65f, -0.1f);
	ColDesc.pSocket = pSpine;
	ColDesc.iFlag = 0;
	ColDesc.pOwner = this;
	XMStoreFloat4x4(&ColDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	strcpy(ColDesc.sTag, "Monster_Body1"); // 기본 충돌체 - 몬스터끼리 밀어내고 플레이어 또는 플레이어 공격이랑 충돌하기위한거

	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	//오른손
	CHierarchyNode*		pRightHand = m_pModelCom->Get_HierarchyNode("index_01_r");
	if (nullptr == pRightHand)
		return E_FAIL;
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.pSocket = pRightHand;
	ColDesc.iFlag = 0;
	ColDesc.pOwner = this;
	XMStoreFloat4x4(&ColDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	strcpy(ColDesc.sTag, "Monster_RHand"); //몬스터 공격용 충돌체
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	//왼손
	CHierarchyNode*		pLeftHand = m_pModelCom->Get_HierarchyNode("index_01_l");
	if (nullptr == pLeftHand)
		return E_FAIL;
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.pSocket = pLeftHand;
	ColDesc.iFlag = 0;
	ColDesc.pOwner = this;
	XMStoreFloat4x4(&ColDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	strcpy(ColDesc.sTag, "Monster_LHand"); //몬스터 공격용 충돌체
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CDisasterFusion::Create_Navigation(_uint iNaviNum)
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

HRESULT CDisasterFusion::Ready_Sockets()
{
	CHierarchyNode*		pRHand = m_pModelCom->Get_HierarchyNode("lowerarm_r");
	if (nullptr == pRHand)
		return E_FAIL;
	m_pSockets.push_back(pRHand);

	CHierarchyNode*		pLHand = m_pModelCom->Get_HierarchyNode("lowerarm_l");
	if (nullptr == pLHand)
		return E_FAIL;
	m_pSockets.push_back(pLHand);

	CHierarchyNode*		pRThumb = m_pModelCom->Get_HierarchyNode("middle_01_r");
	if (nullptr == pRThumb)
		return E_FAIL;
	m_pSockets.push_back(pRThumb);

	CHierarchyNode*		pLThumb = m_pModelCom->Get_HierarchyNode("HCannon_up_end_L_end");
	if (nullptr == pLThumb)
		return E_FAIL;
	m_pSockets.push_back(pLThumb);

	CHierarchyNode*		pRThumbPoint = m_pModelCom->Get_HierarchyNode("thumb_03_r");
	if (nullptr == pRThumbPoint)
		return E_FAIL;
	m_pSockets.push_back(pRThumbPoint);

	CHierarchyNode*		pRThumbPoint2 = m_pModelCom->Get_HierarchyNode("upperarm_twist_01_r_end");
	if (nullptr == pRThumbPoint2)
		return E_FAIL;
	m_pSockets.push_back(pRThumbPoint2);


	CHierarchyNode*		pSpine = m_pModelCom->Get_HierarchyNode("spine_03");
	if (nullptr == pSpine)
		return E_FAIL;
	m_pSockets.push_back(pSpine);


	CHierarchyNode*		pMask = m_pModelCom->Get_HierarchyNode("mask_up_end_M");
	if (nullptr == pMask)
		return E_FAIL;
	m_pSockets.push_back(pMask);

	return S_OK;
}

HRESULT CDisasterFusion::Ready_Trails()
{
	CSwordTrail::TRAILINFO Info;
	Info.HighAndLow.vLow = { -0.019f,0.f,0.f };
	Info.HighAndLow.vHigh = { 0.f,0.f,0.f };
	Info.TexName = L"SwordTrail_Right";
	Info.vColor1 = { 0.941f,0.231f,0.372f,1.f };
	Info.vColor2 = { 0.917f,0.164f,0.247f,1.f };
	Info.vRatio = { 2.f,3.f };
	Info.bFixColor = true;
	Info.iShaderPass = 1;
	Info.fBlurPower = 1.5f;
	CGameObject*		SwordTrail1 = GI->Clone_GameObject(TEXT("SwordTrailObj"), &Info);
	if (nullptr == SwordTrail1)
		return E_FAIL;

	m_SwordTrails.push_back((CSwordTrailObj*)SwordTrail1);

	Info.HighAndLow.vLow = { 0.f,0.f,0.f };
	Info.HighAndLow.vHigh = { 0.02f,0.f,0.f };
	Info.TexName = L"SwordTrail_Left";
	CGameObject*		SwordTrail2 = GI->Clone_GameObject(TEXT("SwordTrailObj"), &Info);
	if (nullptr == SwordTrail2)
		return E_FAIL;

	m_SwordTrails.push_back((CSwordTrailObj*)SwordTrail2);


	return S_OK;
}


HRESULT CDisasterFusion::Ready_Layer_UI(const _tchar * pLayerTag)
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

void CDisasterFusion::Set_PreBar(_float fTimeDelta)
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

CDisasterFusion * CDisasterFusion::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDisasterFusion*		pInstance = new CDisasterFusion(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CDisasterFusion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDisasterFusion::Clone(void * pArg)
{
	CDisasterFusion*		pInstance = new CDisasterFusion(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CDisasterFusion"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDisasterFusion::Free()
{
	__super::Free();

	for (auto & iter : m_SwordTrails)
		Safe_Release(iter);

	Safe_Release(m_pTrailEffect);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pDissolveTexture);
	Safe_Release(m_pEmissiveCom);
}
