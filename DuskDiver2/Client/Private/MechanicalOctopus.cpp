#include "stdafx.h"
#include "..\Public\MechanicalOctopus.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "ToolManager.h"
#include "Monster_Manager.h"
#include "Time_Manager.h"
#include "Camera_Manager.h"
#include "Camera_Manager.h"
#include "MechanicalOctopusLeg.h"
#include "EffectOctopusRange.h"
#include "UI.h"
#include "Boss_Cuma.h"
#include "Crack_Manager.h"
#include "Particle_Manager.h"
#include "Player01HitEffect_1.h"
#include "Player01HitEffect_2.h"
#include "Player01Wind1.h"
#include "Player01Wind2.h"
#include "Player01Pillar.h"
#include "Player01Pillar2.h"
#include "Player01Pillar3.h"
#include "Player01SmokeBall.h"
#include "Player01Metamor.h"
#include "Player01Landing.h"
#include "Ring.h"

CMechanicalOctopus::CMechanicalOctopus(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CMechanicalOctopus::CMechanicalOctopus(const CMechanicalOctopus & rhs)
	: CCreture(rhs)
{
}

HRESULT CMechanicalOctopus::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CMechanicalOctopus::Initialize(void * pArg)
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

	m_sTag = "Tag_Boss";
	// *[HeroJho]: ImGui로 디버깅을 하려면 꼭 호출해줘야 합니다.
	Make_ImGui();

	m_eCurState = IDLE1;
	m_pModelCom->Set_AnimIndex(IDLE1);


	m_fPlayerPos = { 0.f,0.f,0.f, 1.f }; //일단 초기화
	m_vPlayerLook = { 0.f,0.f,1.f };

	Ready_Sockets();
	Ready_LegParts(CreateInfo.iNaviNum);

	m_fHurtTime = 0.02f;

	m_fMaxHp = 1.f;
	m_fNowHp = m_fMaxHp;
	m_fStamina = 3.f;
	m_fDamage = 100.f;

	m_fMaxPg = 8000.f;
	m_fPatternGauge = m_fMaxPg;

	m_fPreHp = m_fNowHp;
	m_fPrePatternGauge = m_fPatternGauge;


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Dissolve"), TEXT("Com_Texture"), (CComponent**)&m_pDissolveTexture)))
		return E_FAIL;


	MM->Add_Boss(this);
	MM->Add_MechanicalOctopus(this);
	UM->Set_BossNum(2);

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;


	// For. CutScene

	CGameObject* pObj = nullptr;
	GI->Add_Get_GameObjectToLayer(TEXT("Boss_Cuma"), TM->Get_CurLevel(), TEXT("Layer_BossCuma"), nullptr, &pObj);
	((CBoss_Cuma*)pObj)->Set_Boss(this);


	return S_OK;
}

void CMechanicalOctopus::Tick(_float fTimeDelta)
{
	if (m_bCutScene)
		Tick_Scene(fTimeDelta);


	Tick_Game(fTimeDelta);
}

void CMechanicalOctopus::LateTick(_float fTimeDelta)
{
	if (m_bGara)
	{
		if (nullptr == m_pRendererCom)
			return;

		fTimeDelta *= m_fTMRatio;


		m_pTransformCom->Tick_Gravity(fTimeDelta, m_pNavigationCom, 30.f);
		m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom, m_pNavigationCom);

		End_Animation();

		Update_Parts();
		for (auto& iter : m_Parts)
			iter->LateTick(fTimeDelta);


		Tick_Col(m_pTransformCom->Get_WorldMatrix());

		if (CK_DOWN(DIK_NUMPAD1)) //회전하면서 플레이어 한테 돌진
		{
			m_bAttack = true;
			Set_State(ATTACK1_1);
		}

		if (CK_DOWN(DIK_NUMPAD2)) //높게 점프한뒤에 플레이어 위로 착지
		{
			m_bAttack = true;
			Set_State(ATTACK2);
		}

		if (m_bAttackCol)
			Choose_Col("Monster_Attack");

		if (m_bHurt)
			Choose_Col("Monster_Body1");

		if (m_bRangeCol)
			Choose_Col("Monster_Push_Range");

		if(m_bRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

		Render_Col(m_pRendererCom);

	}
}

HRESULT CMechanicalOctopus::Render()
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
		
		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, m_iShaderPath)))
			return E_FAIL;
	}	

	return S_OK;
}

// *[HeroJho]: 디버깅 창입니다. ImGui로 작동됩니다.
void CMechanicalOctopus::Render_Debug()
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
	ImGui::Text("attacktime : %f", m_fAttackTime);
	ImGui::Text("movingGuage : %f", m_fMovingGuage);
	ImGui::Text("ShootAcc : %f", m_fShootAcc);
	ImGui::Text("BreakAcc : %f", m_fBreakAcc);
	ImGui::Text("PatternGauge : %f", m_fPatternGauge);

	
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

	_float3 vSize = Get_Colliders("Monster_Attack")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Monster_Attack")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Get_Colliders("Monster_Attack")->Set_Size(vSize);
	Get_Colliders("Monster_Attack")->Set_Center(vCenter);
	ImGui::DragFloat4("Power, Time, LerpTime", (float*)&ShakeTest, 0.01f, -10.f, 10.f, "%.2f");

	ImGui::DragFloat4("Power, Time, StopTime", (float*)&FowTest, 0.01f, -10.f, 10.f, "%.2f");

	ImGui::End();

	for (auto& iter : m_Parts)
		iter->Render_Debug();
}

// *[HeroJho]: 충돌이 발생할 때 호출되는 함수입니다.
void CMechanicalOctopus::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
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
			if (!m_bHurtCol)
				m_bHurtCol = true;
			//어택 타입
			((CCreture*)Desc.pOther)->On_Hit(this);
		}
	}
	else if ("Tag_Monster" == Desc.pOther->Get_Tag())
	{
			if (!strcmp("Monster_Push_Range", Desc.pMyDesc->sTag) &&
				!strcmp("Monster_Push_Range", Desc.pOtherDesc->sTag))
			{
				//밀어내기
				m_pTransformCom->PushMeXZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION), Desc.pMyDesc->vSize.x, XMLoadFloat4(&((CCreture*)Desc.pOther)->Get_WorldPos()), Desc.pOtherDesc->vSize.x, TM->Get_TimeDelta(), m_pNavigationCom);
			}
	
	}

	
}

void CMechanicalOctopus::On_Hit(CCreture* pOther)
{

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

		if (m_fNowHp <= 0.f)
		{
			m_bHurt = false;
			m_bSpawn = false;
		}

		UM->Plus_Combo();
		UM->Set_Hit(true);
		UM->Set_Up(true);

	}

}

void CMechanicalOctopus::Create_HitEffect(_float4 vWorldPos)
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

void CMechanicalOctopus::Set_OctopusCutSceneState(_float fPosX, _float fPoZ, _float fSpeed)
{
	_float4 vPos = { fPosX, 15.531f, fPoZ, 1.f };
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));

	//점프해서 올라가는 애니메이션 실행

	m_fAnimSpeed = fSpeed;
	m_pModelCom->Set_AnimIndex(ATTACK2);
	m_eCurState = ATTACK2;
	m_eNextState = m_eCurState;
	m_pModelCom->SetNextIndex(m_eNextState);
	m_bDownSound = false;

}

void CMechanicalOctopus::CutScene_Update(_float fTimeDelta)
{
	m_pModelCom->Set_TickPerSecond(m_fAnimSpeed);


	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(13)) // 착지할때 랜더 켜짐
	{
		m_bRender = true;
		m_pModelCom->Set_TickPerSecond(24.f); //착륙할때 속도 안에 값만 바꿔주면됨
		for (auto& pLeg : m_Parts)
			static_cast<CMechanicalOctopusLeg*>(pLeg)->Render_On();

		if (!m_bDownSound)
		{
			m_bDownSound = true;
			GI->PlaySoundW(L"se_EN0103_jumpatk_3.wav", SD_OCTOPUS, 0.5f);
		}

	}



	if (m_pModelCom->GetAniEnd()) //착지했을때 아이들 상태로
	{
		m_pModelCom->Set_AnimIndex(IDLE1);
		m_eCurState = IDLE1;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
	}
}

_float4x4 CMechanicalOctopus::Get_BoneMatrix()
{
	_float4x4 pHeadMatrix;
	XMStoreFloat4x4(&pHeadMatrix, m_pHead->Get_CombinedTransformation() *m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix());

	return pHeadMatrix;
}

void CMechanicalOctopus::Die()
{
	//  죽는 코드
	Set_Dead();
	CRKM->Create_Crack(_float4(7.07f, 15.63f, 283.f, 1.f), 5.f, 5, TM->Get_CurLevel());
}

void CMechanicalOctopus::Control_MechanicalOctopus()
{
	m_bMove = true;
	m_bHurt = true;
	m_bSpawn = true;
	m_bAttack = false;
	m_bOneCheck = false;
	m_bOneRandom = true;
	m_bAction = true;

	for (auto& pLeg : m_Parts)
		static_cast<CMechanicalOctopusLeg*>(pLeg)->Spawn_On();

}

void CMechanicalOctopus::Set_State(STATE eState) //end 애니메이션에서
{
	//상태 보간 하는 함수
	if (m_eNextState == eState)
		return;

	m_eNextState = eState;

	switch (m_eNextState)
	{
		//다음 상태로 넘어갈때 해줘야하는것들
		//애니메이션x
	case Client::CMechanicalOctopus::ATTACK1_1:
		GI->PlaySoundW(L"se_EN0103_moving_1.wav", SD_OCTOPUS, 0.5f);
		m_bSpinSound = false;
		m_bHurtCol = false;
		m_eHitType = TYPE_SPIN;
		m_fPushPower = 12.f;
		m_fAirPower = 10.f;
		m_fDamage = 210.f;
		m_bAttack1EffectAcc = 1.f;
		break;
	case Client::CMechanicalOctopus::ATTACK1_2:
		m_bHurtCol = false;
		m_eHitType = TYPE_SPIN;
		m_fPushPower = 12.f;
		m_fAirPower = 10.f;
		m_fDamage = 230.f;
		break;
	case Client::CMechanicalOctopus::ATTACK1_3:
		m_bHurtCol = false;
		m_eHitType = TYPE_SPIN;
		m_fPushPower = 12.f;
		m_fAirPower = 10.f;
		m_fDamage = 240.f;
		break;
	case Client::CMechanicalOctopus::ATTACK2:
		GI->StopSound(SD_OCTOPUS);
		m_bHurtCol = false;
		m_bOnePos = false;
		m_bAttack2Shake = false;
		m_bRangeEffect1 = false;
		m_bJumpSound = false;
		m_bDownSound = false;
		m_pModelCom->Set_TickPerSecond(24.f);
		m_eHitType = TYPE_AWAY;
		m_fPushPower = 20.f;
		m_fAirPower = 7.f;
		m_fDamage = 350.f;
		m_bAttack2Effect = false;
		m_bAttack2Effect_2 = false;
		break;
	case Client::CMechanicalOctopus::IDLE1:
		if (m_bMove)
		{
			m_bHurt = true;
			m_bAction = true;
			m_bSpawn = true;
		}
		m_bAddShoot = true;
		m_fShootAcc = 0.f;
		break;
	case Client::CMechanicalOctopus::IDLE2:
		break;
	case Client::CMechanicalOctopus::MOVE:
		GI->StopSound(SD_OCTOPUS);
		m_bMoveShoot = true;
		break;
	case Client::CMechanicalOctopus::BRAKE:
		break;
	case Client::CMechanicalOctopus::STUNBEGIN:
		break;
	case Client::CMechanicalOctopus::STUN:
		m_bHitAble = true;
		break;
	case Client::CMechanicalOctopus::DEATH:
		break;
	}


	m_pModelCom->SetNextIndex(m_eNextState);
	m_pModelCom->SetChangeBool(true); 
	m_eCurState = m_eNextState; //보간시작할때 상태를 바꿔줌

}

void CMechanicalOctopus::End_Animation()
{
	if (m_pModelCom->GetAniEnd())
	{
		//애니메이션 끝났을때
		switch (m_eCurState)
		{
			//다음 상태로 넘어갈때 해줘야하는것들
			//애니메이션x
		case Client::CMechanicalOctopus::IDLE1:
			m_bOneCheck = false;
			m_bAction = true;
			m_bAttack = false;
			m_bOneRandom = true;
			m_bIdleOn = false;
			break;
		case Client::CMechanicalOctopus::IDLE2:
			break;
		case Client::CMechanicalOctopus::ATTACK1_1:
			GI->PlaySoundW(L"se_EN0103_spin_2.wav", SD_OCTOPUS, 0.5f);
			m_pModelCom->Set_AnimIndex(ATTACK1_2);
			m_eCurState = ATTACK1_2;
			m_eNextState = m_eCurState;
			m_pModelCom->SetNextIndex(m_eNextState);
			m_bSpinSound = false;
			Change_AttackColRange(_float3{ 5.6f, 5.6f, 5.6f }, _float3{ 0.f, 0.f, 1.6f });
			break;
		case Client::CMechanicalOctopus::ATTACK1_2:
			//GI->PlaySoundW(L"se_EN0103_spin_3.wav", SD_OCTOPUS, 0.5f);
			GI->PlaySoundW(L"se_EN0103_spin_2.wav", SD_TRAININGBOT1, 0.5f);
			m_bAttack1On = false;
			m_pModelCom->Set_AnimIndex(ATTACK1_3);
			m_eCurState = ATTACK1_3;
			m_eNextState = m_eCurState;
			m_pModelCom->SetNextIndex(m_eNextState);
			Change_AttackColRange(_float3{ 3.5f,3.5f,3.5f }, _float3{ 0.f, 0.f, 1.6f });
			m_bAttack1_3Sound = false;
			break;
		case Client::CMechanicalOctopus::ATTACK1_3:
			m_fAttackTime = 0.f;
			m_bAddTime = true;
			m_bOneRandom = true;
			m_bOneCheck = false;
			m_pModelCom->Set_AnimIndex(IDLE1);
			m_eCurState = IDLE1;
			m_eNextState = m_eCurState;
			m_pModelCom->SetNextIndex(m_eNextState);
			break;
		case Client::CMechanicalOctopus::ATTACK2:
			m_bRangeOn = false;
			m_fAttackTime = 0.f;
			m_bAddTime = true;
			m_bOneRandom = true;
			m_bOneCheck = false;
			if (m_bAvoidCheck)
			{
				m_bAvoidAttack = false;
				m_bAvoidCheck = false;
			}
			m_pModelCom->Set_TickPerSecond(24.f);
			m_pModelCom->Set_AnimIndex(IDLE1);
			m_eCurState = IDLE1;
			m_eNextState = m_eCurState;
			m_pModelCom->SetNextIndex(m_eNextState);
			break;
		case Client::CMechanicalOctopus::MOVE:
			m_bMoveOn = false;
			Set_State(BRAKE);
			break;
		case Client::CMechanicalOctopus::BRAKE:
			m_bBreakOn = false;
			m_fAttackTime = 0.f;
			m_bAddTime = true;
			m_bOneRandom = true;
			m_bOneCheck = false;
			Set_State(IDLE1);
			break;
		case Client::CMechanicalOctopus::STUNBEGIN:
			Set_State(STUN);
			break;
		case Client::CMechanicalOctopus::STUN:
			Set_State(STUN);
			break;
		case Client::CMechanicalOctopus::DEATH:
			break;
		default:
			break;

		}

		

	}
}

void CMechanicalOctopus::Update(_float fTimeDelta)
{
	m_bRangeCol = true;

	if (!m_pModelCom->GetChangeBool())
	{
		switch (m_eCurState)
		{
		case Client::CMechanicalOctopus::IDLE1:
			Idle_Update(fTimeDelta);
			break;
		case Client::CMechanicalOctopus::IDLE2:
			m_bRangeCol = true;
			break;
		case Client::CMechanicalOctopus::MOVE:
			Move_Update(fTimeDelta);
			break;
		case Client::CMechanicalOctopus::BRAKE:
			Break_Update(fTimeDelta);
			break;
		case Client::CMechanicalOctopus::ATTACK1_1:
			Attack1_1Update(fTimeDelta);
			break;
		case Client::CMechanicalOctopus::ATTACK1_2:
			Attack1_2Update(fTimeDelta);
			break;
		case Client::CMechanicalOctopus::ATTACK1_3:
			Attack1_3Update(fTimeDelta);
			break;
		case Client::CMechanicalOctopus::ATTACK2:
		{
			if (m_bCutScene)
				CutScene_Update(fTimeDelta);
			else
				Attack2_Update(fTimeDelta);
		}
			break;
		case Client::CMechanicalOctopus::STUNBEGIN:
			break;
		case Client::CMechanicalOctopus::STUN:
			Stun_Update(fTimeDelta);
			break;
		case Client::CMechanicalOctopus::DEATH:
			Death_Update(fTimeDelta);
			break;
		default:
			break;
		}

	}

}

void CMechanicalOctopus::Set_Dir()
{
	//플레이어 포스 받아오기 미리 받아둘지

	//내가 바라봐야할 룩벡터 구하기, 플레이어 포스에서 내포스 빼고 노멀라이즈
	_vector vLook = XMVector3Normalize(XMLoadFloat4(&m_fPlayerPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	XMStoreFloat3(&m_vTargetLook, vLook);


}

void CMechanicalOctopus::Distance_BattlePattern(_float fTimeDelta)
{
	m_fPlayerPos = PM->Get_PlayerPos();

	m_fDistance = m_pTransformCom->TargetDistance(XMLoadFloat4(&m_fPlayerPos)); //타겟과 나의 거리 계속 계산

	if (m_bSpawn && m_bAction) // 스폰이 끝났을때 시작
		ActionToPlayer(fTimeDelta);
	
	RegretToPlayer(fTimeDelta);
}

void CMechanicalOctopus::Change_AttackColRange(_float3 vSize, _float3 vCenter)
{
	Get_Colliders("Monster_Attack")->Set_Size(vSize);
	Get_Colliders("Monster_Attack")->Set_Center(vCenter);
}

void CMechanicalOctopus::AttackPattern(_float fTimeDelta)
{
	//공격할수있는 상태 - AttackTime이 지정시간 이상이어야되고, 맞는 애니메이션이 끝났을때여야하고, 

	if(m_bAddTime)
		m_fAttackTime += 0.1f * fTimeDelta; //애니메이션이  끝나거나 hit 상태일때 m_fAttackTime 0으로
	
	if (m_fAttackTime > 0.6f)
	{
		m_bAttack = true;

		BasicAttack(fTimeDelta);

		m_bAddTime = false;
	}

}

void CMechanicalOctopus::BasicAttack(_float fTimeDelta)
{
	if (m_bOneRandom)
	{
		_uint iRandomAttack = GI->Get_Random(1, 2);
		m_bOneRandom = false;
		
		switch (iRandomAttack)
		{
		case 1:
			Attack1(fTimeDelta);
			break;
		case 2:
			Attack2(fTimeDelta);
			break;
		default:
			break;
		}
	}
	

}

void CMechanicalOctopus::Moving(_float fTimeDelta)
{

	if (!m_bOneCheck)
	{
		m_bOneCheck = true;
		Set_State(STATE::MOVE);

	}

	if (m_fDistance >= 8.f)
		m_bClose = false;
	else
		m_bClose = true;

}

void CMechanicalOctopus::ActionToPlayer(_float fTimeDelta)
{
	if (m_fDistance <= 100.f)
	{
		//공격하지않을때의 행동
		if (!m_bAttack && !m_bAvoidAttack)
		{
			
			Set_State(IDLE1);
	}	

		//AttackPattern(fTimeDelta);

		return;
	}

}

void CMechanicalOctopus::RegretToPlayer(_float fTimeDelta)
{
	if (m_bSpawn && !m_bMove)
	{
		Set_State(IDLE1);
	}
}

void CMechanicalOctopus::Check_Rim(_float fTimeDelta)
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

void CMechanicalOctopus::Check_Death(_float fTimeDelta)
{

	if (!m_pTransformCom->Get_Air() && m_fNowHp <= 0.f)
	{
		m_bAction = false;
		m_bHitAble = false;
		m_bHurtCol = false;
		m_bRangeCol = false;
		m_bAttackCol = false;
		m_bDeadStart = true;
		m_bStunGas = false;

		if (m_bOneDead)
		{
			GI->PlaySoundW(L"se_EN0103_dead_2.wav", SD_OCTOPUS, 0.5f);
			m_bOneDead = false;
			Set_State(DEATH);

			TM->Set_BossDie(true);
		}
	}

}

void CMechanicalOctopus::Check_PatternGauge(_float fTimeDelta)
{
	m_fPatternGauge = m_Parts[LEG_A]->Get_NowHp() + m_Parts[LEG_B]->Get_NowHp() + m_Parts[LEG_C]->Get_NowHp() + m_Parts[LEG_D]->Get_NowHp();
	if (m_fPatternGauge <= 6000.f && !m_bPattern1)
	{
		m_bPattern1 = true;
		m_bPatternStart = true;
		m_bShootMissile = true;

	}

	if (m_fPatternGauge <= 4000.f && !m_bPattern2)
	{
		m_bPattern2 = true;
		m_bPatternStart = true;
		m_bShootMissile = true;
		m_bPour = true;

		for (auto& pLeg : m_Parts)
		{
			if (pLeg->Get_NowHp() > 0.f)
				static_cast<CMechanicalOctopusLeg*>(pLeg)->Pour_MissilePlus();
		}

	}

	if (m_fPatternGauge <= 2000.f && !m_bPattern3)
	{
		m_bPattern3 = true;
		m_bPatternStart = true;
		m_bShootMissile = true;

	}

	if (m_fPatternGauge <= 0.f)
	{
		m_bStun = true;
		m_bAction = false;

		m_fGasSoundAcc += 1.f * fTimeDelta;

		if (m_bStunGas)
		{
			if (!m_bStunOn)
			{
				if (m_fGasSoundAcc >= 3.f)
				{
					GI->PlaySoundW(L"se_EN0103_stun_gas.wav", SD_OCTOPUS, 0.3f);
					m_fGasSoundAcc = 0.f;
					m_bStunOn = true;
				}
			}
			else
			{
				if (m_fGasSoundAcc >= 3.f)
				{
					GI->PlaySoundW(L"se_EN0103_stun_gas.wav", SD_OCTOPUS, 0.3f);
					m_fGasSoundAcc = 0.f;
					m_bStunOn = true;
				}
			}
		}

		if (m_bStunOne)
		{
			Set_State(STUNBEGIN);
			m_bStunOne = false;
			m_bStunGas = true;

			TM->Set_BossGro(true);
		}
	}
}

void CMechanicalOctopus::Reset_Collider()
{
	m_bHurtCol = false;
}

void CMechanicalOctopus::Check_ShootAttack(_float fTimeDelta)
{
	if (m_bAddShoot)
		m_fShootAcc += 0.1f * fTimeDelta;

	if (m_fShootAcc >= 0.6f)
	{
		if (!m_bOneTime)
		{
			m_bShoot = true;
			m_bOneTime = true;
		}
		m_bAddShoot = false;
		m_bBreak = true;
	}

	if (m_bBreak)
		m_fBreakAcc += 1.f * fTimeDelta;

	if (m_fBreakAcc > 1.5f)
	{
		m_fBreakAcc = 0.f;
		m_bAddShoot = true;
		m_bBreak = false;
		m_bOneTime = false;
	}
}

void CMechanicalOctopus::Find_NearstMonster(_float fTimeDelta)
{
	
	CCreture* pNearstMonster = nullptr;
	_float fNeastDis = FLT_MAX;
	for (auto& pMonster : m_Parts)
	{
		if (pMonster->Get_NowHp() <= 0.f)
			continue;

		_vector vMonsterPos = XMLoadFloat4(&((CCreture*)pMonster)->Get_WorldPos());
		_float fDis = XMVectorGetX(XMVector3Length(vMonsterPos - XMLoadFloat4(&m_fPlayerPos)));
		if (fDis < fNeastDis)
		{
			pNearstMonster = (CCreture*)pMonster;
			fNeastDis = fDis;
		}
	}

	if (pNearstMonster != nullptr)
		static_cast<CMechanicalOctopusLeg*>(pNearstMonster)->Attack_On();

}

void CMechanicalOctopus::Shoot_AllLeg()
{
	for (auto& pLeg : m_Parts)
	{
		if (pLeg->Get_NowHp() > 0.f)
			static_cast<CMechanicalOctopusLeg*>(pLeg)->Attack_On();
	}
}

void CMechanicalOctopus::Create_Particle(_float4 fSize)
{
	
}


void CMechanicalOctopus::Attack1(_float fTimeDelta)
{

	if (!m_bOneCheck)
	{
		m_bOneCheck = true;
		Set_State(STATE::ATTACK1_1);

	}


}

void CMechanicalOctopus::Attack2(_float fTimeDelta)
{
	if (!m_bOneCheck)
	{
		m_bOneCheck = true;
		Set_State(STATE::ATTACK2);

	}
}

void CMechanicalOctopus::Attack1_1Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(0))
	{
		LookAtPlayer();

		if (!m_bAttack1On)
		{
			GI->PlaySoundW(L"se_EN0103_spin_1.wav", SD_OCTOPUS, 0.5f);
			m_bAttack1On = true;
		}
	}


}

void CMechanicalOctopus::Attack1_2Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	m_bRangeCol = false;

	m_fSpinAcc += 0.1f * fTimeDelta;

	if (m_fSpinAcc >= 0.2f)
	{
		GI->StopSound(SD_OCTOPUS);
		GI->PlaySoundW(L"se_EN0103_spin_2.wav", SD_OCTOPUS, 0.5f);
		m_fSpinAcc = 0.f;
		m_bSpinSound = true;
	}



	m_bAttack1EffectAcc += 1.f * fTimeDelta;
	if (m_bAttack1EffectAcc >= 0.3f)
	{
		CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
		Info3->fBlurPower = 1.f;
		Info3->vLocalPos = { 0.f,0.f,3.f,1.f };
		Info3->vColor = { 0.5f,0.5f,0.5f,1.f };
		Info3->vColor2 = { 0.5f,0.5f,0.5f,1.f };
		Info3->vRatio = { 1.f,1.f };
		Info3->vRatioSpeed = { 1.f,1.f };
		Info3->vScaleSpeed = { 140.f,100.f,140.f };
		Info3->vScale = { 10.f,10.f,10.f };
		Info3->fTurnSpeed = 7.f;
		Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3);

		CPlayer01Wind2::WIND2INFO* Info4 = new CPlayer01Wind2::WIND2INFO;
		Info4->fBlurPower = 1.f;
		Info4->vLocalPos = { 0.f,0.f,3.f,1.f };
		Info4->vColor = { 0.5f,0.5f,0.5f,1.f };
		Info4->vColor2 = { 0.5f,0.5f,0.5f,1.f };
		Info4->vRatio = { 1.f,1.f };
		Info4->vRatioSpeed = { 1.f,1.f };
		Info4->vScaleSpeed = { 140.f,100.f,140.f };
		Info4->vScale = { 10.f,10.f,10.f };
		Info4->fTurnSpeed = 5.f;
		Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4);

		PTM->Create_InstanceParticle(L"OctopusAttack1", m_pTransformCom->Get_WorldFloat4x4());

		m_bAttack1EffectAcc = 0.f;
	}

	if (!m_bHurtCol)
	{
		m_bAttackCol = true;
	}

	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(0))
	{
		LookAtPlayer();

		if (m_bAttack1On)
		{
			m_bAttack1On = false;
			//GI->PlaySoundW(L"se_EN0103_spin_2.wav", SD_OCTOPUS, 0.7f);
		}
	}
}

void CMechanicalOctopus::Attack1_3Update(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(0) && !m_bAttack1_3Sound)
	{
		m_bAttack1_3Sound = true;
		GI->PlaySoundW(L"se_EN0103_moving_2.wav", SD_OCTOPUS, 0.5f);	
	}
	m_bAttackCol = false;
}

void CMechanicalOctopus::Attack2_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	m_bAttackCol = false;
	m_pModelCom->Set_TickPerSecond(27.f);

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(14) && !m_bJumpSound)
	{
		m_bJumpSound = true;
		GI->PlaySoundW(L"se_EN0103_jumpatk_1.wav", SD_OCTOPUS, 0.5f);

	}

	if (m_pModelCom->Get_KeyFrame() == 68.f && !m_bDownSound)
	{
		m_bDownSound = true;
		GI->PlaySoundW(L"se_EN0103_jumpatk_3.wav", SD_OCTOPUS, 0.5f);

	}

	if (m_pModelCom->Get_KeyFrame() == 46.f && !m_bAttack2Effect)
	{
		m_bAttack2Effect = true;

		CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
		Info3->fBlurPower = 1.f;
		Info3->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info3->vColor = { 0.5f,0.5f,0.5f,1.f };
		Info3->vColor2 = { 0.5f,0.5f,0.5f,1.f };
		Info3->vRatio = { 1.f,1.f };
		Info3->vRatioSpeed = { 1.f,1.f };
		Info3->vScaleSpeed = { 180.f,100.f,180.f };
		Info3->vScale = { 10.f,10.f,10.f };
		Info3->fTurnSpeed = 7.f;
		Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3);

		CPlayer01Wind2::WIND2INFO* Info4 = new CPlayer01Wind2::WIND2INFO;
		Info4->fBlurPower = 1.f;
		Info4->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info4->vColor = { 0.5f,0.5f,0.5f,1.f };
		Info4->vColor2 = { 0.5f,0.5f,0.5f,1.f };
		Info4->vRatio = { 1.f,1.f };
		Info4->vRatioSpeed = { 1.f,1.f };
		Info4->vScaleSpeed = { 180.f,100.f,180.f };
		Info4->vScale = { 10.f,10.f,10.f };
		Info4->fTurnSpeed = 5.f;
		Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4);
		PTM->Create_InstanceParticle(L"OctopusAttack2", m_pTransformCom->Get_WorldFloat4x4());
	}

	if (m_pModelCom->Get_KeyFrame() == 73.f && !m_bAttack2Effect_2)
	{
		m_bAttack2Effect_2 = true;
		CPlayer01Wind1::WIND1INFO* Info = new CPlayer01Wind1::WIND1INFO;
		Info->bRotation = false;
		Info->bPosFix = true;
		Info->fUpSpeed = 0.5f;
		Info->fBlurPower = 0.f;
		Info->fTurnSpeed = 4.f;
		Info->vColor = { 0.5f,0.5f,0.5f,1.f };
		Info->vColor2 = { 0.5f,0.5f,0.5f,1.f };
		Info->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 0.6f,1.f };
		Info->vRatioSpeed = { 0.3f,0.f };
		Info->vScale = { 10.f,10.f,10.f };
		Info->vScaleSpeed = { 90.f,150.f,90.f };
		Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);

		CRing::RINGINFO* Info2 = new CRing::RINGINFO;
		Info2->bFixPos = true;
		Info2->fBlurPower = 0.f;
		Info2->fUpSpeed = 0.5f;
		Info2->vColor = { 0.5f,0.5f,0.5f,1.f };
		Info2->vColor2 = { 0.5f,0.5f,0.5f,1.f };
		Info2->vFixScaleSpeed = { 10.f,4.f,10.f };
		Info2->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info2->vRatio = { 0.6f,1.f };
		Info2->vRatioSpeed = { 0.3f,0.f };
		Info2->vScale = { 1.f,1.f,1.f };
		Info2->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Ring", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2);

		CPlayer01Metamor::METAMORINFO* Info3 = new CPlayer01Metamor::METAMORINFO;
		Info3->fBlurPower = 0.f;
		Info3->vLocalPos = { 0.f,0.f,3.f,1.f };
		Info3->vColor = { 0.5f,0.5f,0.5f,1.f };
		Info3->vColor2 = { 0.5f,0.5f,0.5f,1.f };
		Info3->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info3->vRatio = { 0.6f,1.f };
		Info3->vRatioSpeed = { 0.3f,0.f };
		Info3->vScale = { 1.f,0.5f,1.f };
		Info3->vScaleSpeed = { 6.f,1.f,6.f };
		Info3->vUVFixSpeed = { 1.f,0.5f };
		Info3->bPosFix = false;
		Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Metamor", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3);

		CPlayer01Landing::LANDINGINFO* Info4 = new CPlayer01Landing::LANDINGINFO;
		Info4->bPosFix = true;
		Info4->fBlurPower = 0.f;
		Info4->fTurnSpeed = 5.f;
		Info4->fUpSpeed = 0.5f;
		Info4->vColor = { 0.5f,0.5f,0.5f,1.f };
		Info4->vColor2 = { 0.5f,0.5f,0.5f,1.f };
		Info4->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info4->vRatio = { 0.6f,1.f };
		Info4->vRatioSpeed = { 0.3f,0.f };
		Info4->vScale = { 10.f,10.f,10.f };
		Info4->vScaleSpeed = { 80.f,100.f,80.f };
		Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Landing", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4);
		PTM->Create_InstanceParticle(L"OctopusAttack2", m_pTransformCom->Get_WorldFloat4x4());
	}

	if (!m_bAvoidAttack)
	{
		if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(2) &&
			m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(3))
		{
			LookAtPlayer();
			//Set_Dir();

			m_pModelCom->Set_TickPerSecond(5.f);	
			m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), 16.f, fTimeDelta, m_pNavigationCom);

		}

		if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(0) &&
			m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(1) && !m_bHurtCol)
		{
			m_bAttackCol = true;
			m_bRangeCol = false;
		}

		if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(10) &&
			m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(11))
		{
			m_pModelCom->Set_TickPerSecond(7.f);
		}

	}
	else
	{

		if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(4) &&
			m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(5))
		{
			m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), 20.f, fTimeDelta, m_pNavigationCom);

		}

		if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(2) &&
			m_pModelCom->Get_KeyFrame() < m_pModelCom->Get_TimeLimit(3))
		{
			m_pModelCom->Set_TickPerSecond(2.f); //올라갔을때

			if (m_bShootMissile)
			{
				m_bShootMissile = false;
				for (auto& pLeg : m_Parts)
				{
					if (pLeg->Get_NowHp() > 0.f)
					{
						static_cast<CMechanicalOctopusLeg*>(pLeg)->Pour_Missile();
						static_cast<CMechanicalOctopusLeg*>(pLeg)->Attack_On();


					}
				}
			}
		}
		
	}


	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(6) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(7))
	{
		if (!m_bRangeEffect1)
		{
			m_bRangeEffect1 = true;
			Create_Effect(RANGE);
		}
		if (!static_cast<CEffectOctopusRange*>(m_pRangeEffect)->Get_RenderOnOff())
		{
			static_cast<CEffectOctopusRange*>(m_pRangeEffect)->RenderOnOff(true);
			static_cast<CEffectOctopusRange*>(m_pRangeEffect)->Set_RatioSpeed(_float2{ 2.f, 0.f });
			_float4 vPos;
			XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			static_cast<CEffectOctopusRange*>(m_pRangeEffect)->SetUp_Position(vPos);
			if (!m_bRangeOn)
			{
				m_bRangeOn = true;
				GI->PlaySoundW(L"se_EN0103_jumpatk_2.wav", SD_OCTOPUS, 0.5f);
			}
		}

		
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(10) && !m_bOnePos)
	{
		//마법진에 플레이어 위치 주기
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		static_cast<CEffectOctopusRange*>(m_pRangeEffect)->SetUp_Position(vPos);
	}

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(8) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(9))
	{
		if (!static_cast<CEffectOctopusRange*>(m_pRangeEffect)->Get_RenderOnOff())
		{
			static_cast<CEffectOctopusRange*>(m_pRangeEffect)->Set_RatioSpeed(_float2{ 9.f, 0.f });
			static_cast<CEffectOctopusRange*>(m_pRangeEffect)->RenderOnOff(true);
			if (m_bRangeOn)
			{
				m_bRangeOn = false;
				GI->PlaySoundW(L"se_EN0103_jumpatk_2.wav", SD_OCTOPUS, 0.5f);
			}
		}
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(12) && !m_bAttack2Shake)
	{
		m_bAttack2Shake = true;
		CRM->Get_Cam()->Shake(0.25f, 0.3f, false, 0.1f);
		GI->PlaySoundW(L"se_EN0103_jumpatk_3.wav", SD_OCTOPUS, 0.5f);
	}

}

void CMechanicalOctopus::Break_Update(_float fTimeDelta)
{
	if (!m_bBreakOn)
	{
		GI->PlaySoundW(L"se_EN0103_jumpatk_3.wav", SD_OCTOPUS, 0.5f);
		m_bBreakOn = true;
	}
}

void CMechanicalOctopus::Move_Update(_float fTimeDelta)
{
		//플레이어 밀치고 가기
		//처음에만 플레이어 바라보는 룩 넣은다음에 쭉 갈수있도록
	m_bRangeCol = false;
	Set_Dir();
	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook) * -1.f, 10.f, fTimeDelta, m_pNavigationCom);

	if (!m_bMoveOn)
	{
		GI->PlaySoundW(L"se_EN0103_moving_1.wav", SD_OCTOPUS, 0.5f);
		m_bMoveOn = true;
	}

	if (m_fDistance >= 17.f)
		Set_State(BRAKE);

	if (m_bMoveShoot)
	{
		m_bMoveShoot = false;

		if (!m_bPour)
			Find_NearstMonster(fTimeDelta);
		else
			Shoot_AllLeg();
	}

}

void CMechanicalOctopus::Idle_Update(_float fTimeDelta)
{
	m_fGasSoundAcc += 1.f * fTimeDelta;

	if (!m_bIdleOn)
	{
		//GI->PlaySoundW(L"se_EN0103_idle.wav", SD_OCTOPUS, 0.5f);
		m_bIdleOn = true;
		m_bIdleStop = false;
	}

	//패턴게이지 50퍼센트 이하면 공격 계속

	//플레이어가 가까이 있고 공격 계속 받으면 이동
	
	if (m_bPatternStart)
	{
		m_bPatternStart = false;
		m_bAvoidAttack = true;
		m_bAvoidCheck = true;
		m_bRangeEffect1 = false;
		m_pModelCom->Set_AnimIndex(ATTACK2);
		m_eCurState = ATTACK2;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
	}
	else
	{
		if (m_fMovingGuage > 15.f)
		{
			if (!m_bOneCheck)
			{
				m_bOneCheck = true;
				m_bShoot = true;
				Set_State(MOVE);

			}
			m_fMovingGuage = 0.f;
		}

		//가장 가까이 있는 다리에서 공격 시작
		if (m_bSpawn)
		{
			Check_ShootAttack(fTimeDelta);


			if (m_bShoot)
			{
				m_bShoot = false;

				if(!m_bPour)
					Find_NearstMonster(fTimeDelta);
				else
					Shoot_AllLeg();

				m_fShootAcc = 0.f;
			}

		}

	}

}


void CMechanicalOctopus::Stun_Update(_float fTimeDelta)
{
	m_fStunSoundAcc += 1.f * fTimeDelta;

	if (!m_bOneSound && !m_bTwoSound)
	{
		if (m_fStunSoundAcc >= 1.f)
		{
			GI->PlaySoundW(L"se_EN0103_stun_sparks-01.wav", SD_TRAININGBOT1, 0.3f);
			m_fStunSoundAcc = 0.f;
			m_bOneSound = true;
		}
	}

	else if (m_bOneSound && !m_bTwoSound)
	{
		if (m_fStunSoundAcc >= 0.55f)
		{
			GI->PlaySoundW(L"se_EN0103_stun_sparks-02.wav", SD_TRAININGBOT1, 0.3f);
			m_fStunSoundAcc = 0.f;
			m_bOneSound = false;
			m_bTwoSound = true;
		}
	}

	else if (!m_bOneSound && m_bTwoSound)
	{
		if (m_fStunSoundAcc >= 0.55f)
		{
			GI->PlaySoundW(L"se_EN0103_stun_sparks-03.wav", SD_TRAININGBOT1, 0.3f);
			m_fStunSoundAcc = 0.f;
			m_bTwoSound = false;
		}
	}
}

void CMechanicalOctopus::Death_Update(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(0))
	{
		m_pModelCom->Set_TickPerSecond(0.f);
		m_bDissolve = true;
	}

}

void CMechanicalOctopus::Create_Effect(EFFECT eEffect)
{
	switch (eEffect)
	{
	case RANGE:
	{
		CEffectOctopusRange::EFFECTRANGEINFO* Info = new CEffectOctopusRange::EFFECTRANGEINFO;
		Info->vColor = { 0.980f,0.619f,0.490f,1.f };
		Info->vColor2 = { 0.909f,0.380f,0.262f,1.f };
		Info->vRatio = { 2.f,5.f };
		Info->vRatioSpeed = { 2.f,0.f };

		_matrix Effectmatrix;

		XMStoreFloat4x4(&Info->WorldMatrix, m_pTransformCom->Get_WorldMatrix());
		Info->fBlurPower = 1.2f;


		(GI->Add_Get_GameObjectToLayer(TEXT("EffectOctopusRange"), LEVEL_TEST, TEXT("Layer_Monster"), &Info, &m_pRangeEffect));

	}
	break;
	
	default:
		break;
	}
}

void CMechanicalOctopus::LookAtPlayer(_float fLiearTime)
{
	Set_Dir();
	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&m_vTargetLook), fLiearTime);

}


HRESULT CMechanicalOctopus::Ready_Sockets()
{
	CHierarchyNode*		pLeg1 = m_pModelCom->Get_HierarchyNode("Leg03_L");
	if (nullptr == pLeg1)
		return E_FAIL;
	m_Sockets.push_back(pLeg1);

	CHierarchyNode*		pLeg2 = m_pModelCom->Get_HierarchyNode("Leg03_R");
	if (nullptr == pLeg2)
		return E_FAIL;
	m_Sockets.push_back(pLeg2);

	CHierarchyNode*		pLeg3 = m_pModelCom->Get_HierarchyNode("Leg03_B");
	if (nullptr == pLeg3)
		return E_FAIL;
	m_Sockets.push_back(pLeg3);

	CHierarchyNode*		pLeg4 = m_pModelCom->Get_HierarchyNode("Leg03_F");
	if (nullptr == pLeg4)
		return E_FAIL;
	m_Sockets.push_back(pLeg4);

	return S_OK;
}

HRESULT CMechanicalOctopus::Ready_LegParts(_uint iNaviNum)
{
	CCreture::CREATEINFO _tInfo;
	ZeroMemory(&_tInfo, sizeof(CCreture::CREATEINFO));

	_tInfo.iNaviNum = iNaviNum;
	CGameObject*		pLegA = GI->Clone_GameObject(TEXT("MechanicalOctopusLeg"), &_tInfo);
	if (nullptr == pLegA)
		return E_FAIL;
	m_Parts.push_back((CCreture*)pLegA);

	CGameObject*		pLegB = GI->Clone_GameObject(TEXT("MechanicalOctopusLeg"), &_tInfo);
	if (nullptr == pLegB)
		return E_FAIL;
	m_Parts.push_back((CCreture*)pLegB);

	CGameObject*		pLegC = GI->Clone_GameObject(TEXT("MechanicalOctopusLeg"), &_tInfo);
	if (nullptr == pLegC)
		return E_FAIL;
	m_Parts.push_back((CCreture*)pLegC);

	CGameObject*		pLegD = GI->Clone_GameObject(TEXT("MechanicalOctopusLeg"), &_tInfo);
	if (nullptr == pLegD)
		return E_FAIL;
	m_Parts.push_back((CCreture*)pLegD);

	return S_OK;
}

HRESULT CMechanicalOctopus::Update_Parts()
{
	XMStoreFloat4x4(&m_LegAMatrix, m_Sockets[LEG_A]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix());
	m_Parts[LEG_A]->SetUp_State(XMLoadFloat4x4(&m_LegAMatrix));

	XMStoreFloat4x4(&m_LegBMatrix, m_Sockets[LEG_B]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix());
	m_Parts[LEG_B]->SetUp_State(XMLoadFloat4x4(&m_LegBMatrix));

	XMStoreFloat4x4(&m_LegCMatrix, m_Sockets[LEG_C]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix());
	m_Parts[LEG_C]->SetUp_State(XMLoadFloat4x4(&m_LegCMatrix));

	XMStoreFloat4x4(&m_LegDMatrix, m_Sockets[LEG_D]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix());
	m_Parts[LEG_D]->SetUp_State(XMLoadFloat4x4(&m_LegDMatrix));

	return S_OK;
}

HRESULT CMechanicalOctopus::Ready_Components()
{

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("MechanicalOctopus"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	//m_pEmissiveCom
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Oct_Emi"), TEXT("Com_TextureEmi"), (CComponent**)&m_pEmissiveCom)))
		return E_FAIL;

	m_pModelCom->Set_RootHierarchy(3);

	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(4.2f, 4.2f, 4.2f);
	ColDesc.vCenter = _float3(0.f, 0.3f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Body1"); 

	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	CHierarchyNode*		pBody = m_pModelCom->Get_HierarchyNode("Body_M");
	if (nullptr == pBody)
		return E_FAIL;

	ColDesc.vSize = _float3(3.f, 3.f, 3.f);
	ColDesc.vCenter = _float3(0.f, 0.f, 2.f);
	XMStoreFloat4x4(&ColDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	ColDesc.pSocket = pBody;
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Push_Range"); // 밀어내기 위함

	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	ColDesc.vSize = _float3(3.5f, 3.2f, 3.2f);
	ColDesc.vCenter = _float3(0.f, 0.f, 1.6f);
	XMStoreFloat4x4(&ColDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	ColDesc.pSocket = pBody;
	ColDesc.pOwner = this;
	ColDesc.iFlag = 0;
	strcpy(ColDesc.sTag, "Monster_Attack"); //몬스터 공격용 충돌체
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	m_pHead = m_pModelCom->Get_HierarchyNode("TopShell_M");

	return S_OK;
}

HRESULT CMechanicalOctopus::Create_Navigation(_uint iNaviNum)
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


void CMechanicalOctopus::Tick_Game(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_MONSTER, this);
	Check_Hurt(fTimeDelta);
	Check_Rim(fTimeDelta);
	fTimeDelta *= m_fTMRatio;
	Emp_Col();
	Distance_BattlePattern(fTimeDelta);
	Update(fTimeDelta);
	Check_PatternGauge(fTimeDelta);
	Check_Death(fTimeDelta);
	Set_PreBar(fTimeDelta);

	for (auto& iter : m_Parts)
		iter->Tick(fTimeDelta);



}

void CMechanicalOctopus::Tick_Scene(_float fTimeDelta)
{
	for (auto& iter : m_Parts)
		iter->Tick(fTimeDelta);

}

HRESULT CMechanicalOctopus::Ready_Layer_UI(const _tchar * pLayerTag)
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

void CMechanicalOctopus::Set_PreBar(_float fTimeDelta)
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

	if (m_fPrePatternGauge > m_fPatternGauge)
	{
		m_fMinusPg = (m_fPrePatternGauge - m_fPatternGauge) / m_fMaxPg;
		m_fPrePatternGauge -= m_fMinusPg * 5000.f * fTimeDelta;

		if (m_fPrePatternGauge <= m_fPatternGauge)
		{
			m_fPrePatternGauge = m_fPatternGauge;
		}
	}
}

CMechanicalOctopus * CMechanicalOctopus::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMechanicalOctopus*		pInstance = new CMechanicalOctopus(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMechanicalOctopus"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMechanicalOctopus::Clone(void * pArg)
{
	CMechanicalOctopus*		pInstance = new CMechanicalOctopus(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMechanicalOctopus"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMechanicalOctopus::Free()
{
	__super::Free();

	for (auto& pPart : m_Parts)
		Safe_Release(pPart);

	m_Parts.clear();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pDissolveTexture);
	Safe_Release(m_pEmissiveCom);
}
