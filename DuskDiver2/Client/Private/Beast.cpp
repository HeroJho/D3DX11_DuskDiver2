#include "stdafx.h"
#include "..\Public\Beast.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "ToolManager.h"
#include "Monster_Manager.h"
#include "Time_Manager.h"
#include "Camera_Manager.h"
#include "UI.h"
#include "BossClaw.h"
#include "BeastStoneBody.h"
#include "BeastBreath.h"
#include "BossAttackRange.h"
#include "ReserveTrail.h"
#include "BeastMarvelPush.h"
#include "BeastMarvelBall.h"
#include "BeastMarvelObstacle.h"
#include "MarvelReserveTaril.h"
#include "Rock.h"
#include "Player01Wind1.h"
#include "Player01Wind2.h"
#include "Player01SmokeBall.h"
#include "Particle_Manager.h"
#include "BeastClaw.h"
#include "BeastTornado.h"
#include "BeastTornadoSide.h"
#include "BeastBreath2.h"
#include "Effect_Manager.h"
#include "ViewAlpha.h"
#include "Particle_Manager.h"
#include "Player01HitEffect_1.h"
#include "Player01HitEffect_2.h"
#include "ToolManager.h"


CBeast::CBeast(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CBeast::CBeast(const CBeast & rhs)
	: CCreture(rhs)
{
}

HRESULT CBeast::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CBeast::Initialize(void * pArg)
{

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	CCreture::CREATEINFO CreateInfo = *(CCreture::CREATEINFO*)pArg;
	_vector vPosition = XMLoadFloat4(&CreateInfo.vPosition);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(180.f));

	if (FAILED(Ready_Components()))
		return E_FAIL;

	if (FAILED(Create_Navigation(CreateInfo.iNaviNum)))
		return E_FAIL;

	m_sTag = "Tag_Boss";
	// *[HeroJho]: ImGui로 디버깅을 하려면 꼭 호출해줘야 합니다.
	Make_ImGui();

	m_eCurState = IDLE;
	m_pModelCom->Set_AnimIndex(IDLE);


	m_fPlayerPos = { 0.f,0.f,0.f, 1.f };
	m_vPlayerLook = { 0.f,0.f,1.f };

	m_fHurtTime = 0.03f;
	m_fAttackDelayT = 0.4f;

	m_fMaxHp = 12000.f;
	m_fNowHp = m_fMaxHp;
	m_fStamina = 3.f;
	m_fDamage = 10.f;

	m_fMaxPg = 2000.f;
	m_fPatternGauge = m_fMaxPg;
	m_fPreHp = m_fNowHp;
	m_fPrePatternGauge = m_fPatternGauge;

	if (FAILED(Create_StoneBody()))
		return E_FAIL;

	if (FAILED(Create_Breath()))
		return E_FAIL;

	if (FAILED(Create_MarvelPush()))
		return E_FAIL;

	if (FAILED(Create_MarvelObstacle()))
		return E_FAIL;

	if (FAILED(Create_MarvelBall()))
		return E_FAIL;

	if (FAILED(Create_MarvelReserveTrail()))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Dissolve"), TEXT("Com_Texture"), (CComponent**)&m_pDissolveTexture)))
		return E_FAIL;

	MM->Add_Boss(this);
	UM->Set_BossNum(3);
	m_iBreathCount = 0;
	m_iChargeMaxCount = 10;
	Control_Beast();

	CBeastBreath2::BREATHINFO* Info = new CBeastBreath2::BREATHINFO;
	Info->fBlurPower = 2.f;
	Info->vColor = { 1.f,1.f,1.f,1.f };
	Info->vColor2 = { 0.f,0.5f,1.f,1.f };
	Info->vLocalPos = { 0.f,0.f,0.f,0.f };
	Info->vRatio = { 1.f,2.f };
	Info->vRatioSpeed = { 0.f,0.f };
	Info->vScale = { 30.f,20.f,30.f };
	Info->vScaleSpeed = { 0.f,0.f,0.f };
	Info->vUvFixSpeed = { 0.f,4.f };
	Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
	m_BeastBreath2.push_back(GI->Clone_GameObject(L"BeastBreath2", &Info));

	CBeastBreath2::BREATHINFO* Info2 = new CBeastBreath2::BREATHINFO;
	Info2->fBlurPower = 2.f;
	Info2->vColor = { 1.f,1.f,1.f,1.f };
	Info2->vColor2 = { 0.f,0.5f,1.f,1.f };
	Info2->vLocalPos = { 0.f,0.f,0.f,0.f };
	Info2->vRatio = { 1.f,2.f };
	Info2->vRatioSpeed = { 0.f,0.f };
	Info2->vScale = { 40.f,20.f,40.f };
	Info2->vScaleSpeed = { 0.f,0.f,0.f };
	Info2->vUvFixSpeed = { 0.f,5.f };
	Info2->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
	m_BeastBreath2.push_back(GI->Clone_GameObject(L"BeastBreath2", &Info2));

	CBeastBreath2::BREATHINFO* Info3 = new CBeastBreath2::BREATHINFO;
	Info3->fBlurPower = 2.f;
	Info3->vColor = { 1.f,1.f,1.f,1.f };
	Info3->vColor2 = { 0.f,0.5f,1.f,1.f };
	Info3->vLocalPos = { 0.f,0.f,0.f,0.f };
	Info3->vRatio = { 1.f,2.f };
	Info3->vRatioSpeed = { 0.f,0.f };
	Info3->vScale = { 20.f,20.f,20.f };
	Info3->vScaleSpeed = { 0.f,0.f,0.f };
	Info3->vUvFixSpeed = { 0.f,5.f };
	Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
	m_BeastBreath2.push_back(GI->Clone_GameObject(L"BeastBreath2", &Info3));

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;

	MM->Add_Beast(this);

	return S_OK;
}

void CBeast::Tick(_float fTimeDelta)
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
	if (m_bBreath)
	{	
		Update_Breath2Matrix(fTimeDelta);
	}

	if (m_fNowHp <= 6000.f && !m_bPase2Hp)
	{
		m_bPase2Hp = true;
		Check_Stun();
	}

	/*if (CK_DOWN(DIK_HOME))
	{
		m_bStun = true;
		m_bAction = false;
		Set_State(STUNBEGIN);
	}*/

	

}

void CBeast::LateTick(_float fTimeDelta)
{
	if (m_bGara)
	{
		if (nullptr == m_pRendererCom)
			return;

		fTimeDelta *= m_fTMRatio;

		m_pTransformCom->Tick_Gravity(fTimeDelta, m_pNavigationCom, 30.f);
		m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom, m_pNavigationCom);

		End_Animation();

		Key_InputAttack();

		Tick_Col(m_pTransformCom->Get_WorldMatrix());

		Push_Collider();
	
		if (EM->Get_MarbleEnd() && !m_bMarbleAttack)
		{
			m_bMarbleAttack = true;
			m_fNowHp = 0.f;
			Check_Stun();
			CRM->Get_Cam()->Shake(0.5f, 0.3f, false, 0.1f);
		}
	
		if (m_bBreath)
		{
			for (auto& iter : m_BeastBreath2)
			{
				iter->LateTick(fTimeDelta);
			}
		}

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
		Render_Col(m_pRendererCom);

	}
}

HRESULT CBeast::Render()
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

	_float fGama = 1.2f;
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
void CBeast::Render_Debug()
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

	ImGui::Text("Breathdistance : %f", m_fBreathDistance);
	ImGui::Text("HowlDelayAcc : %f", m_fHowlDelayAcc);
	ImGui::Text("CreateReserveAcc : %f", m_fCreateReserveAcc);
	ImGui::Text("MarvelGameAcc : %f", m_fMarvelGameAcc);

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



	//_float3 vSize = Get_Colliders("Monster_Body2")->Get_Desc()->vSize;
	//_float3 vCenter = Get_Colliders("Monster_Body2")->Get_Desc()->vCenter;

	//ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	//ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	//Get_Colliders("Monster_Body2")->Set_Size(vSize);
	//Get_Colliders("Monster_Body2")->Set_Center(vCenter);
	//ImGui::Checkbox("Pause", &m_bGara);

	ImGui::End();
}

void CBeast::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		if (strcmp("Monster_Body1", Desc.pMyDesc->sTag) && strcmp("Monster_Body2", Desc.pMyDesc->sTag) && strcmp("Monster_Body3", Desc.pMyDesc->sTag) &&
			strcmp("Monster_Push_Range", Desc.pMyDesc->sTag) && !strcmp("Player_Body", Desc.pOtherDesc->sTag))
		{
			if (!m_bHurtCol)
				m_bHurtCol = true;

			((CCreture*)Desc.pOther)->On_Hit(this);
		}

		if (!strcmp("Monster_Body1", Desc.pMyDesc->sTag) && !strcmp("Player_Attack", Desc.pOtherDesc->sTag))
		{
			m_eHitPostion = BEAST_HEAD;
		}

		if (!strcmp("Monster_Body2", Desc.pMyDesc->sTag) && !strcmp("Player_Attack", Desc.pOtherDesc->sTag))
		{
			m_eHitPostion = BEAST_LEFT;
		}

		if (!strcmp("Monster_Body3", Desc.pMyDesc->sTag) && !strcmp("Player_Attack", Desc.pOtherDesc->sTag))
		{
			m_eHitPostion = BEAST_RIGHT;
		}

	}
	else if ("Tag_Monster" == Desc.pOther->Get_Tag())
	{
		if (!strcmp("Monster_Push_Range", Desc.pMyDesc->sTag) &&
			!strcmp("Monster_Push_Range", Desc.pOtherDesc->sTag))
		{
			m_pTransformCom->PushMeXZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION), Desc.pMyDesc->vSize.x, XMLoadFloat4(&((CCreture*)Desc.pOther)->Get_WorldPos()), Desc.pOtherDesc->vSize.x, TM->Get_TimeDelta(), m_pNavigationCom);
		}

	}


}

void CBeast::On_Hit(CCreture* pOther)
{
	//hit이 한번만 처리되게끔
	if (m_bHurt && m_bHitAble)
	{
		_float4 vPos;

		if (m_eHitPostion == BEAST_HEAD)
			vPos = { -0.507f, 1.665f, -26.677f, 1.f };
		else if (m_eHitPostion == BEAST_RIGHT)
			vPos = { 4.893f, 1.765f, -24.477f, 1.f };
		else if (m_eHitPostion == BEAST_LEFT)
			vPos = { -5.707f, 1.665f, -26.877f, 1.f };
		else
			vPos = { -5.707f, 1.f, -26.877f, 1.f };

		pOther->Create_HitEffect(vPos);


		GI->PlaySoundW(L"SwordHit.ogg", SD_HIT, 0.5f);
		m_bRim = true;
		m_fRimAcc = 0.f;
		m_bHurt = false;
		m_bRHandCol = false;
		m_bLHandCol = false;

		m_fNowHp -= (pOther->Get_Damage() - m_fStamina);

		/*if (m_bDamageAble && !m_bReGauge)
		{
			m_fPatternGauge -= (pOther->Get_Damage() * 1.2f);
			Check_Stun();
		}*/

		if (m_fNowHp <= 0.f && m_eCurState == STUN && !m_bBeastEndEnd)
		{
			m_bBeastEndEnd = true;
			TIM->Fix_All(0.14f, 5.f);
			CRM->Get_Cam()->Fov(-3.4f, 0.15f, 3.f);
			//영우잉형 코드
			GI->StopSound(SD_BEAST2);
			GI->StopSound(SD_BGM);
			TM->Set_BossCutBlock(true);
			GI->PlaySoundW(L"slowmotion_finish.wav", SD_BEAST6, 1.f);
		}

		UM->Plus_Combo();
		UM->Set_Hit(true);
		UM->Set_Up(true);
	}

}

void CBeast::Create_HitEffect(_float4 vWorldPos)
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

void CBeast::Control_Beast()
{
	m_bMove = true;
	m_bAttack = false;
	m_bOneCheck = false;
	m_bHitAble = true;
	m_bAction = true;
}

void CBeast::Set_BeastCry()
{
	m_bAttack = true;
	m_iHowlingAttack = 7;
	Set_State(ATTACK5);
}

void CBeast::Key_InputAttack()
{
	if (CK_DOWN(DIK_NUMPAD1)) // 손톱 할퀴기 - BossClaw
	{
		m_bAttack = true;
		Set_State(ATTACK1);
	}
	if (CK_DOWN(DIK_NUMPAD2)) //바닥치기 - 
	{
		m_bAttack = true;
		Set_State(ATTACK2);
	}
	if (CK_DOWN(DIK_NUMPAD3)) //돌 소환 - BeastStoneBody, BeastStonePart
	{
		m_bAttack = true;
		Set_State(ATTACK3);
	}

	if (CK_DOWN(DIK_NUMPAD4)) //브레스 - 돌있으면 빨간브레스 없으면 파란브레스 BeastBreath
	{
		m_bAttack = true;
		m_bMarvelGame = false;
		Set_State(ATTACK4_1);
	}

	if (CK_DOWN(DIK_NUMPAD5)) //밀어내기 
	{
		m_bAttack = true;
		m_iHowlingAttack = 1;
		Set_State(ATTACK5);
	}
	if (CK_DOWN(DIK_NUMPAD6)) //검기 - BeastClaw
	{
		m_bAttack = true;
		m_iHowlingAttack = 2;
		Set_State(ATTACK5);
	}
	if (CK_DOWN(DIK_NUMPAD7)) //불기둥 - BeastPillar
	{
		m_bAttack = true;
		m_iHowlingAttack = 3;
		Set_State(ATTACK5);
	}

	if (CK_DOWN(DIK_NUMPAD8)) //회오리 - BeastTornado
	{
		m_bAttack = true;
		m_iHowlingAttack = 4;
		Set_State(ATTACK5);
	}
	if (CK_DOWN(DIK_NUMPAD9)) //손오공 - BeastLaser
	{
		m_bAttack = true;
		m_iHowlingAttack = 5;
		Set_State(ATTACK5);
	}
	if (CK_DOWN(DIK_NUMPAD0)) //구슬치기 BeastMarvelPush(크리스탈) BeastMarvelObstacle(방해물), BeastMarvelBall(구슬)
	{
		m_bAttack = true;
		m_iHowlingAttack = 6;
		m_fMarvelGameAcc = 0.f;
		m_bMarvelGame = true;
		Set_State(ATTACK5);
	}
}

void CBeast::Set_State(STATE eState) //end 애니메이션에서
{
	//상태 보간 하는 함수
	if (m_eNextState == eState)
		return;

	m_eNextState = eState;

	switch (m_eNextState)
	{
		//다음 상태로 넘어갈때 해줘야하는것들
		//애니메이션x
	case Client::CBeast::ATTACK1:	
		m_bHurtCol = false;
		m_bAttack1Voice = false;
		m_bEffect_Attack1 = false;
		m_eHitType = TYPE_AWAY;
		m_fPushPower = 15.f;
		m_fAirPower = 5.f;
		m_fDamage = 250.f;
		break;
	case Client::CBeast::ATTACK2:
		m_bHurtCol = false;
		m_bAttack2Sound = false;
		m_bAttack2Voice = false;
		m_bAttack2Effect = false;
		m_eHitType = TYPE_AWAY;
		m_fPushPower = 15.f;
		m_fAirPower = 7.f;
		m_fDamage = 400.f;
		break;
	case Client::CBeast::ATTACK3:
		GI->PlaySoundW(L"se_EN0301_attack3_3.wav", SD_BEAST, 1.f);
		m_bCreateStone = false;
		m_bAttack3Voice = false;
		m_eHitType = TYPE_AWAY;
		m_fPushPower = 2.f;
		m_fAirPower = 3.f;
		m_fDamage = 70.f;
		break;
	case Client::CBeast::ATTACK4_1:
		m_eHitType = TYPE_HIT;
		m_fPushPower = 10.f;
		m_fAirPower = 0.f;
		m_fDamage = 55.f;
		GI->PlaySoundW(L"DragonSkill6.ogg", SD_BEAST7, 0.8f);
		m_bBreathNormalCircle = false;
		m_fBreathSoundAcc = 0.25f;
		m_fBreathNormalAcc = 0.f;
		m_bBreath = false;
		m_fBreathParticleAcc = 2.f;
		m_fBreathNormalAcc = 2.f;
		m_fMarvelGameAcc = 0.f;
		m_iBreathCount = 0;
		for (auto& iter : m_pStoneBodys) // 돌 있는지 체크
		{						
			if (iter->Get_RenderOnOff())
			{
				CBeastBreath::BREATHINFO Info;
				Info.fBlurPower = 10.f;
				Info.vColor1 = { 1.f,0.f,0.f,1.f };
				Info.vColor1_2 = { 0.f,0.f,0.f,1.f };
				Info.vRatio = { 2.f,10.f };
				Info.vScale = { 1.f,1.f,1.f };
				Info.vUVFixAcc = { 0.f,0.f };
				Info.vUVFixSpeed = { 1.f,3.f };
				Info.fBlurPower_2 = 2.f;
				Info.vColor2 = { 0.f,0.f,0.f,1.f };
				Info.vColor2_2 = { 0.f,0.f,0.f,1.f };
				Info.vRatio_2 = { 1.f,3.f };
				Info.vUVFixAcc_2 = { 0.f,0.f };
				Info.vUVFixSpeed_2 = { -1.f,3.f };
				CBeastBreath::BREATH_RED;
				m_pBeastBreath->Set_BreathInfo(Info);
				m_bChasePlayer = true; //빨간 브레스
				m_eBreathType = BREATH_RED;
				Fix_BreathEffectInfo(BREATH_RED);
				
				
			}
			else
			{
				CBeastBreath::BREATHINFO Info;
				Info.fBlurPower = 3.f;
				Info.vColor1 = { 1.f,1.f,1.f,1.f };
				Info.vColor1_2 = { 1.f,1.f,1.f,1.f };
				Info.vRatio = { 1.f,1.f };
				Info.vScale = { 1.f,1.f,1.f };
				Info.vUVFixAcc = { 0.f,0.f };
				Info.vUVFixSpeed = { 1.f,3.f };
				Info.fBlurPower_2 = 3.f;
				Info.vColor2 = { 0.f,0.f,1.f,1.f };
				Info.vColor2_2 = { 0.f,0.5f,1.f,1.f };
				Info.vRatio_2 = { 3.f,1.f };
				Info.vUVFixAcc_2 = { 0.f,0.f };
				Info.vUVFixSpeed_2 = { -1.f,3.f };
				Info.eBreathType = CBeastBreath::BREATH_NORMAL;
				m_pBeastBreath->Set_BreathInfo(Info);
				m_bChasePlayer = false; //파란 브레스
				m_eBreathType = BREATH_NORMAL;
				Fix_BreathEffectInfo(BREATH_NORMAL);
				
			}
		}
		if (m_bMarvelGame)
		{
			CBeastBreath::BREATHINFO Info;
			Info.fBlurPower = 3.f;
			Info.vColor1 = { 1.f,1.f,1.f,1.f };
			Info.vColor1_2 = { 1.f,1.f,1.f,1.f };
			Info.vRatio = { 1.f,1.f };
			Info.vScale = { 3.f,1.f,3.f };
			Info.vUVFixAcc = { 0.f,0.f };
			Info.vUVFixSpeed = { 1.f,3.f };
			Info.fBlurPower_2 = 3.f;
			Info.vColor2 = { 0.f,0.f,0.f,1.f };
			Info.vColor2_2 = { 0.f,0.f,0.f,1.f };
			Info.vRatio_2 = { 3.f,1.f };
			Info.vUVFixAcc_2 = { 0.f,0.f };
			Info.vUVFixSpeed_2 = { -1.f,3.f };
			m_eBreathType = BREATH_WHITE;
			m_pBeastBreath->Set_BreathInfo(Info);
			m_bChasePlayer = true; //하얀 브레스
			Fix_BreathEffectInfo(BREATH_WHITE);
		
		}
		break;
	case Client::CBeast::ATTACK4_2:
		m_bOneCheckPos = false;
		break;
	case Client::CBeast::ATTACK5:
		Reset_HowlingAttack();
		m_bHurtCol = false;
		m_fPillarSoundAcc = 0.f;
		m_bPillarSound = false;
		GI->PlaySoundW(L"BladeAttackStart.ogg", SD_BEAST3, 0.8f);
		m_bExplosion = false;
		m_bAttack5Voice = false;
		m_bFireBreath = false; 
		m_bAttack5EffectOn = false;
		EM->Set_MarbleCol(false);
		EM->Set_MarbleEnd(false);
		m_fAttack3CreateAcc = false;
		m_bCreateTornado = false;
		m_bMarbleAttack = false;
		m_bCry1Col = false;
		m_bLineTrailOn = false;
		m_bOneSet = true;
		m_fWaveAcc = 0.24f;
		m_eHitType = TYPE_SPIN;
		m_fPushPower = 15.f;
		m_fAirPower = 7.f;
		m_fDamage = 55.f;
		m_fAttackRange = 0.f;
		m_fHowlDelayAcc = 0.f;
		m_bOneCheckPos = false;
		m_fMarvelGameAcc = 0.f;
		//하울링 공격 랜덤으로 뽑아주기
		break;
	case Client::CBeast::IDLE:
		if (m_bMove)
		{
			m_bHurt = true;
			m_bAction = true;
		}
		break;
	case Client::CBeast::STUNBEGIN:
		GI->PlaySoundW(L"DragonDie.ogg", SD_BEAST4, 0.8f);
		m_bStunBeginDownSound = false;
		break;
	case Client::CBeast::STUN:
		m_fStunSoundAcc = 1.5f;
		
		break;
	case Client::CBeast::STUNRECOVER:
		m_fStunRecoverAcc = 0.24f;
		m_bStunRecoverSound = false;
		m_bReGauge = true;
		m_bStunCol = false;
		break;
	case Client::CBeast::DEATH:
		GI->PlaySoundW(L"DragonDie.ogg", SD_BEAST4, 0.8f);
		//Update나 End_Anim에서 윗줄 사운드가 끝날때
		//GI->PlaySoundW(L"se_EN0301_dead2.wav", SD_BEAST, 0.8f);
		break;
	}


	m_pModelCom->SetNextIndex(m_eNextState);
	m_pModelCom->SetChangeBool(true);
	m_eCurState = m_eNextState; //보간시작할때 상태를 바꿔줌

}

void CBeast::End_Animation()
{
	if (m_pModelCom->GetAniEnd())
	{
		//애니메이션 끝났을때
		switch (m_eCurState)
		{
			//다음 상태로 넘어갈때 해줘야하는것들
			//애니메이션x
		case Client::CBeast::IDLE:
			m_bHitAble = true;
			m_bAction = true;
			m_bAttack = false;
			break;
		case Client::CBeast::ATTACK1:
			++m_iAttackCount;
			m_fAttackDelayT = 0.4f;
			Reset_AttackState();
			Set_IDleState();
			break;
		case Client::CBeast::ATTACK2:
			++m_iAttackCount;
			m_fAttackDelayT = 0.4f;
			Reset_AttackState();
			Set_IDleState();
			break;
		case Client::CBeast::ATTACK3:
			m_fAttackDelayT = 0.9f;
			Reset_AttackState();
			Set_IDleState();
			break;
		case Client::CBeast::ATTACK4_1:
			Set_State(ATTACK4_2);
			break;
		case Client::CBeast::ATTACK4_2:
			Check_BreathState();
			m_bStoneSound = false;
			break;
		case Client::CBeast::ATTACK4_3:
			m_fAttackDelayT = 0.4f;
			Reset_AttackState();
			for (auto iter : m_pStoneBodys)
			{
				if (iter->Get_RenderOnOff())
				{
					iter->Fallen_Start();
					iter->Render_OnOff(false);

					//사운드
					if (!m_bStoneSound)
					{
						m_bStoneSound = true;
						GI->PlaySoundW(L"EFF_CHR_Impact_Rock_02_A.ogg", SD_BEASTVOICE, 1.f);
					}
				}

				
			}


			m_bMarvelGame = false; //게임 종료
			Set_IDleState();
			break;
		case Client::CBeast::ATTACK5:
			++m_iAttackCount; //나중에 수정
			m_fAttackDelayT = 0.4f;
			Reset_AttackState();
			Change_AttackColRange(_float3{ 10.f,8.f,8.f }, _float3{ 0.f, 0.f, 0.f });
			
			if(m_iHowlingAttack == 6)
				Set_State(ATTACK4_1);
			else
				Set_State(IDLE);

			//구슬 치기인 경우에는 breath쪽으로 넘어가야한다

			break;
		case Client::CBeast::STUNBEGIN:
			Set_State(STUN);
			break;
		case Client::CBeast::STUN:
			if (m_fNowHp <= 0.f)
				Set_State(STUN);
			else
				Set_State(STUNRECOVER);
			break;
		case Client::CBeast::STUNRECOVER:
			End_Receover();
			m_bStunRecoverSound = false;
			Set_State(IDLE);
			break;
		case Client::CBeast::DEATH:
			//m_bDissolve = true;
			break;
		default:
			break;

		}



	}
}

void CBeast::Update(_float fTimeDelta)
{
	m_bStunCol = false;

	if (!m_pModelCom->GetChangeBool())
	{
		switch (m_eCurState)
		{
		case Client::CBeast::IDLE:
			Idle_Update(fTimeDelta);
			break;
		case Client::CBeast::ATTACK1:
			Attack1_Update(fTimeDelta);
			break;
		case Client::CBeast::ATTACK2:
			Attack2_Update(fTimeDelta);
			break;
		case Client::CBeast::ATTACK3:
			Attack3_Update(fTimeDelta);
			break;
		case Client::CBeast::ATTACK4_1:
			Attack4_1Update(fTimeDelta);
			break;
		case Client::CBeast::ATTACK4_2:
			//구슬치기용이랑 아닌거랑 나누기
			if(m_bMarvelGame)
				Attack4_2MarvelUpdate(fTimeDelta);
			else //구슬치기 아닌 브레스
				Attack4_2Update(fTimeDelta);
			break;
		case Client::CBeast::ATTACK4_3:
			Attack4_3Update(fTimeDelta);
			break;
		case Client::CBeast::ATTACK5:
			Attack5_Update(fTimeDelta);
			break;
		case Client::CBeast::STUNBEGIN:
			StunBegin_Update(fTimeDelta);
			break;
		case Client::CBeast::STUN:
			Stun_Update(fTimeDelta);
			break;
		case Client::CBeast::STUNRECOVER:
			StunRecover_Update(fTimeDelta);
			break;
		case Client::CBeast::DEATH:
			break;
		default:
			break;
		}

	}

}

void CBeast::Push_Collider()
{
	if (m_bRHandCol)
		Choose_Col("Monster_RHand");

	if (m_bLHandCol)
		Choose_Col("Monster_LHand");

	if (m_bAttackCol)
		Choose_Col("Monster_Attack");

	
	//바디콜 상태에 따라서 키고 끄기, 플레이어 위치

	if (m_bHurt)
	{
		if (m_bStunCol)
			Choose_Col("Monster_Body1");
		else
		{
			Choose_Col("Monster_Body2");
			Choose_Col("Monster_Body3");
		}
	}
	

	Choose_Col("Monster_Push_Range");
}

void CBeast::Set_Dir()
{
	//플레이어 포스 받아오기 미리 받아둘지

	//내가 바라봐야할 룩벡터 구하기, 플레이어 포스에서 내포스 빼고 노멀라이즈
	_vector vLook = XMVector3Normalize(XMLoadFloat4(&m_fPlayerPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	XMStoreFloat3(&m_vTargetLook, vLook);


}

void CBeast::Distance_BattlePattern(_float fTimeDelta)
{
	m_fPlayerPos = PM->Get_PlayerPos();

	m_fDistance = m_pTransformCom->TargetDistance(XMLoadFloat4(&m_fPlayerPos)); //타겟과 나의 거리 계속 계산

	if (m_bAction) // 스폰이 끝났을때 시작
		ActionToPlayer(fTimeDelta);

	RegretToPlayer(fTimeDelta); //idle 상태
}

void CBeast::Change_AttackColRange(_float3 vSize, _float3 vCenter)
{
	Get_Colliders("Monster_Attack")->Set_Size(vSize);
	Get_Colliders("Monster_Attack")->Set_Center(vCenter);
}

void CBeast::AttackPattern(_float fTimeDelta)
{
	if (m_bAddTime)
		m_fAttackTime += 0.1f * fTimeDelta; //애니메이션이  끝나거나 hit 상태일때 m_fAttackTime 0으로

	if (m_fAttackTime > m_fAttackDelayT)
	{
		m_bAttack = true;

		if (m_fDistance <= 15.f)
			CloseAttack(fTimeDelta);
		else
			BasicAttack(fTimeDelta);

		m_bAddTime = false;
	}

}

void CBeast::BasicAttack(_float fTimeDelta)
{
	if (m_bOneRandom)
	{
		m_bOneRandom = false;

		switch (m_iBasicAttack)
		{
		case 1:
			Attack3(fTimeDelta); //돌 떨어트림
			break;
		case 2:
			Attack4(fTimeDelta); //브레스
			break;
		case 5:
			Attack5(fTimeDelta); //하울링
			break;
		default:
			break;
		}
	}
}

void CBeast::CloseAttack(_float fTimeDelta)
{
	if (m_bOneRandom)
	{
		m_bOneRandom = false;

		switch (m_iCloseAttack)
		{
		case 0:
			Attack1(fTimeDelta); //손톱 할퀴기
			break;
		case 1:
			Attack2(fTimeDelta); //바닥 치기
			break;
		case 2:
			Attack5(fTimeDelta); //하울링 - 공격 여러개로 나뉘어져있음
			break;
		default:
			break;
		}
	}

}

void CBeast::ActionToPlayer(_float fTimeDelta)
{
	if (m_fDistance <= 100.f)
	{
		if (!m_bAttack)
			Set_State(IDLE);

		//AttackPattern(fTimeDelta);
	}
}

void CBeast::RegretToPlayer(_float fTimeDelta)
{
	if (m_bSpawn && !m_bMove)
	{
		Set_State(IDLE);
	}
}

void CBeast::Check_Rim(_float fTimeDelta)
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

void CBeast::Check_Death(_float fTimeDelta)
{
	Check_Recover(fTimeDelta);

	if (!m_pTransformCom->Get_Air() && m_fNowHp <= 0.f)
	{
		m_bAction = false;
		m_bHurtCol = false;
		m_bRangeCol = false;
		m_bRHandCol = false;
		m_bLHandCol = false;
		m_bDeadStart = true;
		m_bStunCol = true;


		if (m_bDeadOne)
		{
			m_bDeadOne = false;
			Set_State(STUNBEGIN);
		}
	}

	if (m_bDissolve)
	{
		m_iShaderPath = 6;
		m_fDissolveAcc += 0.1f * fTimeDelta;
		if (m_fDissolveAcc >= 1.f)
		{
			m_fDissolveAcc = 1.f;
			Set_Dead();
			MM->Release_Beast();

		}

	}
}

void CBeast::Check_DeadState()
{
	if (!m_bDeadStart)
	{
		if (m_bStun)
			Set_State(STUN);
		else
			Set_State(STUNRECOVER);
	}
}

void CBeast::Check_BreathState()
{
	if (m_bMarvelGame)
	{
		m_iMaxCount = 4; //구슬 게임 브레스 뿜는 시간이라고 생각하면됨 

		if (m_bFireBreath)
			++m_iBreathCount;
	}
	else
	{
		//imaxCount 브레스 뿜는 애니메이션 반복횟수
		if (m_bChasePlayer) //chase - 빨간브레스 더 길어야함
		{
			/*m_iMaxCount = 40;
			m_iChargeMaxCount = 20;*/

			m_iMaxCount = 15;
			m_iChargeMaxCount = 5;
		}
		else
		{
			m_iMaxCount = 10;
			m_iChargeMaxCount = 3;

		}
		++m_iBreathCount;
	}

	if (m_iBreathCount >= m_iMaxCount)
	{
		Set_State(ATTACK4_3);
		m_iBreathCount = 0;
		m_pBeastBreath->Render_OnOff(false); 
		m_pBeastBreath->Attack_OnOff(false);
		m_pBeastBreath->PlayerHit_On();
		m_bBreath = false;
		GI->StopSound(SD_BEAST3);
		GI->StopSound(SD_BEAST6);
		GI->StopSound(SD_BEAST7);
	}
	else
		Set_State(ATTACK4_2);
}

void CBeast::Check_Stun()
{
	
		m_bStun = true;
		m_bAction = false;
		Set_State(STUNBEGIN);
	
}

void CBeast::Check_Recover(_float fTimeDelta)
{
	if (m_bStun)
	{
		m_fRecoverAcc += 1.f * fTimeDelta;
		m_bDamageAble = false;
	}

	if (m_fNowHp <= 0.f)
	{
		m_bStunCol = true;
		m_fNowHp = 0.f;
	}

	if (m_fRecoverAcc >= 2.f)
		m_bStun = false;
}

void CBeast::Reset_Collider()
{
	m_bHurtCol = false;
}

void CBeast::Set_IDleState()
{
	m_pModelCom->Set_AnimIndex(IDLE);
	m_eCurState = IDLE;
	m_eNextState = m_eCurState;
	m_pModelCom->SetNextIndex(m_eNextState);
}

void CBeast::Reset_AttackState()
{
	m_fAttackTime = 0.f;
	if (!m_bHowlingAttack)
		m_bAddTime = true;
	m_bOneRandom = true;
	m_bOneCheck = false;
}

void CBeast::Reset_HowlingAttack()
{
	m_bCreateClaw = true;
	m_bCreateRange = true;
	m_bCreateTornado = true;
	m_bCreateObstacle = true;
	m_bCreatePush = true;
	m_bCreateMarvelBall = true;
	m_pMarvelBall->Reset_State();
}

void CBeast::End_Receover()
{
	m_bDamageAble = true;
	m_fRecoverAcc = 0.f;
	m_fAttackTime = 0.f;
	m_fHowlDelayAcc = 0.f;
	m_bAttack = false;
	m_bAddTime = true;
	m_bOneRandom = true;
	m_bOneCheck = false;
}

void CBeast::Create_Effect(EFFECT eEffect)
{
	switch (eEffect)
	{
	case ATTACK1_RIGHT:
	{
		CBossClaw::CRAWINFO* Info = new CBossClaw::CRAWINFO;
		Info->vColor = { 1.f, 1.f,0.5f,1.f };
		Info->vColor2 = { 1.f, 0.3f, 0.f,1.f };
		Info->vLocalPos = { 3.f, 1.5f, 10.f, 1.f };
		Info->vRatio = { 1.5f, 1.f };
		Info->vRatioSpeed = { 1.f, 0.f }; 
		Info->vScale = { 120.f, 20.f, 120.f };
		Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		Info->fBlurPower = 1.f;
		Info->vRotation = { 0.f,0.f,-173.f };
		GI->Add_GameObjectToLayer(L"BossClaw", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info);
	}
	break;

	case HOWLING_ATTACK3:
	{
		CBossAttackRange::EFFECTRANGEINFO Info;
		Info.iIndex = 1;
		Info.bSound = true;
		(GI->Add_GameObjectToLayer(L"BossAttackRange", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info));
		Info.bSound = false;
		Info.iIndex = 2;
		(GI->Add_GameObjectToLayer(L"BossAttackRange", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info));
		Info.iIndex = 3;
		(GI->Add_GameObjectToLayer(L"BossAttackRange", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info));
		Info.iIndex = 4;
		(GI->Add_GameObjectToLayer(L"BossAttackRange", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info));
		Info.iIndex = 5;
		(GI->Add_GameObjectToLayer(L"BossAttackRange", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info));
		Info.iIndex = 6;
		(GI->Add_GameObjectToLayer(L"BossAttackRange", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info));
		Info.iIndex = 7;
		(GI->Add_GameObjectToLayer(L"BossAttackRange", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info));
		Info.iIndex = 8;
		(GI->Add_GameObjectToLayer(L"BossAttackRange", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info));
		Info.iIndex = 9;
		(GI->Add_GameObjectToLayer(L"BossAttackRange", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info));
		Info.iIndex = 10;
		(GI->Add_GameObjectToLayer(L"BossAttackRange", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info));
		Info.iIndex = 11;
		(GI->Add_GameObjectToLayer(L"BossAttackRange", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info));
		Info.iIndex = 12;
		(GI->Add_GameObjectToLayer(L"BossAttackRange", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info));
		
	}
	break;

	case ET_ATTACK2:
	{
		CRock::ROCKINFO Info;
		Info.fRotation = 0.f;
		Info.fScale = 1.f;
		Info.iIndex = 2;
		_vector Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) +
			(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 14.f)
			+ (XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)) * 0.1f);
		XMStoreFloat4(&Info.vPosition, Pos);
		GI->Add_GameObjectToLayer(L"Rock", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info);

		CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
		Info3->fBlurPower = 0.f;
		Info3->vLocalPos = { 0.f,-0.5f,14.f,1.f };
		Info3->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info3->vColor2 = { 0.7f,0.7f,0.7f,1.f };
		Info3->vRatio = { 1.f,1.f };
		Info3->vScale = { 1.f,1.f,1.f };
		Info3->vRatioSpeed = { 1.f,0.f };
		Info3->vScaleSpeed = { 250.f,200.f,250.f };
		Info3->fTurnSpeed = 5.f;
		Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3);
		

		CPlayer01Wind2::WIND2INFO* Info4 = new CPlayer01Wind2::WIND2INFO;
		Info4->fBlurPower = 0.f;
		Info4->vLocalPos = { 0.f,0.5f,14.f,1.f };
		Info4->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info4->vColor2 = { 0.7f,0.7f,0.7f,1.f };
		Info4->vRatio = { 1.f,1.f };
		Info4->vRatioSpeed = { 1.f,0.f };
		Info4->vScaleSpeed = { 250.f,100.f,250.f };
		Info4->vScale = { 1.f,1.f,1.f };
		Info4->fTurnSpeed = 5.f;
		Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4);

		CPlayer01SmokeBall::SMOKEBALLINFO* Info2 = new CPlayer01SmokeBall::SMOKEBALLINFO;
		Info2->fBlurPower = 1.f;
		Info2->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info2->vColor2 = { 0.7f,0.7f,0.7f,1.f };
		Info2->vLocalPos = { 0.f,0.f,14.f,1.f };
		Info2->vRatio = { 1.f,1.f };
		Info2->vRatioSpeed = { 1.5f,1.5f };
		Info2->vScale = { 0.1f,0.1f,0.1f };
		Info2->vScaleSpeed = { 350.f,350.f,350.f };
		Info2->vUvFixSpeed = { 1.f,3.f };
		Info2->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2);

		CPlayer01SmokeBall::SMOKEBALLINFO* Info5 = new CPlayer01SmokeBall::SMOKEBALLINFO;
		Info5->fBlurPower = 1.f;
		Info5->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info5->vColor2 = { 0.5f,0.5f,0.5f,1.f };
		Info5->vLocalPos = { 0.f,0.f,14.f,1.f };
		Info5->vRatio = { 1.f,1.f };
		Info5->vRatioSpeed = { 1.5f,1.5f };
		Info5->vScale = { 0.1f,0.1f,0.1f };
		Info5->vScaleSpeed = { 350.f,350.f,350.f };
		Info5->vUvFixSpeed = { -2.f,-4.f };
		Info5->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info5);

		PTM->Create_InstanceParticle(L"BeastAttack2", m_pTransformCom->Get_WorldFloat4x4());
		

	}
	}
}

void CBeast::Create_Particle(_float4 fSize)
{

}

HRESULT CBeast::Create_StoneBody()
{
	CCreture::CREATEINFO _tInfo;
	ZeroMemory(&_tInfo, sizeof(CCreture::CREATEINFO));

	_tInfo.iNaviNum = 5;

	_tInfo.iIndex = 1;
	CGameObject*		pStoneA = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("BeastStoneBody"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &_tInfo, &pStoneA));
	if (nullptr == pStoneA)
		return E_FAIL;
	m_pStoneBodys.push_back((CBeastStoneBody*)pStoneA);

	_tInfo.iIndex = 2;
	CGameObject*		pStoneB = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("BeastStoneBody"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &_tInfo, &pStoneB));
	if (nullptr == pStoneB)
		return E_FAIL;
	m_pStoneBodys.push_back((CBeastStoneBody*)pStoneB);

	_tInfo.iIndex = 3;
	CGameObject*		pStoneC = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("BeastStoneBody"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &_tInfo, &pStoneC));
	if (nullptr == pStoneC)
		return E_FAIL;
	m_pStoneBodys.push_back((CBeastStoneBody*)pStoneC);

	_tInfo.iIndex = 4;
	CGameObject*		pStoneD = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("BeastStoneBody"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &_tInfo, &pStoneD));
	if (nullptr == pStoneD)
		return E_FAIL;
	m_pStoneBodys.push_back((CBeastStoneBody*)pStoneD);

	return S_OK;
}

HRESULT CBeast::Create_Breath()
{

	CCreture::CREATEINFO _tInfo;
	ZeroMemory(&_tInfo, sizeof(CCreture::CREATEINFO));
	_tInfo.iNaviNum = 5;
	//입 위치 잡아주기
	XMStoreFloat4x4(&m_BreathMatrix, m_pBreathNode->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix());

	_tInfo.vPosition = *(_float4*)m_BreathMatrix.m[3];

	CGameObject*		pBreath = nullptr;
	if (FAILED(GI->Add_Get_GameObjectToLayer(TEXT("BeastBreath"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &_tInfo, &pBreath)))
		return E_FAIL;

	m_pBeastBreath = (CBeastBreath*)pBreath;

	return S_OK;
}

void CBeast::Attack1(_float fTimeDelta)
{

	if (!m_bOneCheck)
	{
		m_bOneCheck = true;
		Set_State(STATE::ATTACK1);
	}


}

void CBeast::Attack2(_float fTimeDelta)
{
	if (!m_bOneCheck)
	{
		m_bOneCheck = true;
		Set_State(STATE::ATTACK2);
	}
}

void CBeast::Attack3(_float fTimeDelta)
{
	for (auto& iter : m_pStoneBodys)
	{
		if (iter->Get_RenderOnOff()) //돌이 있는 상태
		{
			m_bChasePlayer = true;

			if (!m_bOneCheck)
			{
				m_bOneCheck = true;
				Set_State(STATE::ATTACK4_1); //브레스

			}
		}
		else //없는 상태
		{
			m_bChasePlayer = false;

			if (!m_bOneCheck) 
			{
				m_bOneCheck = true;
				Set_State(STATE::ATTACK3); //돌 소환

			}
		}
	}

}

void CBeast::Attack4(_float fTimeDelta)
{
	if (!m_bOneCheck)
	{
		m_bOneCheck = true;
		Set_State(STATE::ATTACK4_1);

	}
}

void CBeast::Attack5(_float fTimeDelta)
{
	if (!m_bOneCheck)
	{
		m_bOneCheck = true;
		Set_State(STATE::ATTACK5);

	}
}

void CBeast::Idle_Update(_float fTimeDelta)
{
	//플레이어에 가까이 있는 돌만 플레이어 어택과 충돌하기
	Find_NearstStoneAttackBody(fTimeDelta);

	if (m_bHowlingAttack)
	{
		switch (m_iHowlingAttack)
		{					
		case 4:
			Idle_HowlingAttack4(fTimeDelta);
			break;
		case 5:
			Idle_HowlingAttack5(fTimeDelta);
			break;
		default:
			break;
		}
	}
}

void CBeast::Attack1_Update(_float fTimeDelta)
{
	m_bRHandCol = false;

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(0) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(1) && !m_bHurtCol)
		m_bRHandCol = true;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(5) - 5 && !m_bAttack1Voice)
	{
		GI->PlaySoundW(L"DragonSkill13.ogg", SD_BEASTVOICE, 1.f);
		m_bAttack1Voice = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(5) && !m_bEffect_Attack1)
	{
		m_bEffect_Attack1 = true;
		Create_Effect(ATTACK1_RIGHT); //손톱 이펙트	
		GI->PlaySoundW(L"se_EN0301_attack1_2.wav", SD_BEAST, 1.f);
	}

}

void CBeast::Attack2_Update(_float fTimeDelta)
{
	m_bLHandCol = false;

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(0) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(1) && !m_bHurtCol)
	{
		m_bLHandCol = true;
	
	}

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(0) && !m_bAttack2Effect)
	{
		m_bAttack2Effect = true;
		Create_Effect(ET_ATTACK2);
	}
	

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(2) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(3))
		LookAtPlayer();

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(4))
	{
		m_vTargetLook = { 0.f, 0.f, -1.f };
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&m_vTargetLook), 0.05f);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(5) - 15 && !m_bAttack2Voice)
	{
		GI->PlaySoundW(L"DragonSkill13.ogg", SD_BEASTVOICE, 1.f);
		m_bAttack2Voice = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(5) && !m_bAttack2Sound)
	{
		//카메라 쉐이킹
		CRM->Get_Cam()->Shake(0.15f, 0.3f, false, 0.1f);
		GI->PlaySoundW(L"EFF_CHR_Spell_Ground_13_C.ogg", SD_BEAST, 1.f);
		m_bAttack2Sound = true;
	}

}

void CBeast::Attack3_Update(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bAttack3Voice)
	{
		CRM->Get_Cam()->Shake(0.1f, 0.55f);
		GI->PlaySoundW(L"01A_202_UltCutscene_05C.ogg", SD_BEAST, 1.f);
		m_bAttack3Voice = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bCreateStone)
	{
		m_bCreateStone = true;
		_uint iRandom = GI->Get_Random(1, 4);

		for (auto& pStonebodys : m_pStoneBodys)
		{
			pStonebodys->Set_InitState(); //기둥 위치 랜덤으로 리셋
			pStonebodys->Render_OnOff(true); //돌기둥 렌더
			pStonebodys->Reset_InitState(); //돌기둥 초기화
			pStonebodys->Set_RandomIndex(iRandom); // 랜덤으로 돌기둥 빨강 파랑 정해주기
		}

	}

}

void CBeast::Attack4_1Update(_float fTimeDelta)
{

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(0) && !m_bMarvelGame)
	{
		LookAtPlayer();
	}

	Find_NearstStoneAttackBody(fTimeDelta);
}

void CBeast::Attack4_2Update(_float fTimeDelta)
{
	//하울링이 true이면 계속 차징하는걸로 그리고 벽에 부딪혔을때 브레스 쏘는걸로 넘어가야한다....breathcount
	if (m_bChasePlayer)
	{
		LookAtPlayer();
	}
	MM->Set_BreathDamage(600.f);
	if (m_iBreathCount < m_iChargeMaxCount)
		LookAtPlayer();

	Find_NearstStoneShieldBody(fTimeDelta); //가장 가까이에있는 돌기둥의 충돌체 킴

	SetUp_Breath(); //브레스 길이 계속 계산

	if (m_pBeastBreath->Get_ColStone())
		m_pBeastBreath->Change_BreathSize(); //바위랑 충돌처리 됐을때 사이즈 바꿔줌

	if (m_bBreath)
	{
		m_pBeastBreath->Render_OnOff(true); //브레스 뿜는 부분
		m_pBeastBreath->Attack_OnOff(true); //플레이어와 충돌할수있는 충돌체 킴
		m_pBeastBreath->Set_ColStoneOnOff(false);

		Create_BreathEffect(m_eBreathType, fTimeDelta);
		m_fBreathSoundAcc += 1.f * fTimeDelta;
		if (m_fBreathSoundAcc >= 0.25f)
		{
			GI->PlaySoundW(L"BladeAttack.ogg", SD_BEAST5, GI->Get_FloatRandom(0.5f,0.8f));
			m_fBreathSoundAcc = 0.f;
		}

	}

	
	if (m_iBreathCount < m_iChargeMaxCount)
	{
		Create_BreathChargeEffect(m_eBreathType, fTimeDelta);
	}
	

	//count가 2가 됐을때 브레스 render키기 
	if (m_iBreathCount == m_iChargeMaxCount && !m_bOneCheckPos)
	{
		CRM->Get_Cam()->Shake(0.5f, 0.2f, false, 0.2f);
		m_bOneCheckPos = true;
		m_bBreath = true;
		m_bOneMatrix = true;
		m_fPlayerStartPos = PM->Get_PlayerPos();
		GI->PlaySoundW(L"01A_202_UltCutscene_03C.ogg", SD_BEAST4, 1.f);
		GI->PlaySoundW(L"BreathBack.mp3", SD_BEAST7, 1.f);

		if (m_eBreathType == BREATH_NORMAL)
		{			
			PTM->Create_InstanceParticle(L"BreathStartBlue", m_pTransformCom->Get_WorldFloat4x4());
		}

		else if (m_eBreathType == BREATH_RED)
		{
			PTM->Create_InstanceParticle(L"BreathStartRed", m_pTransformCom->Get_WorldFloat4x4());
		}
	}
}

void CBeast::Attack4_2MarvelUpdate(_float fTimeDelta)
{
	Marvel_HowlingAttack6(fTimeDelta);
	MM->Set_BreathDamage(4000.f);
	//count가 2가 됐을때 브레스 render키기 
	if (m_bFireBreath && !m_bOneCheckPos)
	{
		
	}

	if(!m_bBreath)
	{
		Create_BreathChargeEffect(m_eBreathType, fTimeDelta);
	}

	SetUp_Breath();

	if (m_bBreath)
	{
		Create_BreathEffect(m_eBreathType, fTimeDelta);
		m_pBeastBreath->Render_OnOff(true); //브레스 렌더
		m_pBeastBreath->Attack_OnOff(true); //플레이어와 충돌할수있는 충돌체 킴

		m_fBreathSoundAcc += 1.f * fTimeDelta;
		if (m_fBreathSoundAcc >= 0.25f)
		{
			GI->PlaySoundW(L"BladeAttack.ogg", SD_BEAST5, GI->Get_FloatRandom(0.5f, 0.8f));
			m_fBreathSoundAcc = 0.f;
		}
	}
	
}

void CBeast::Attack4_3Update(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(0))
	{
		m_vTargetLook = { 0.f, 0.f, -1.f };
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&m_vTargetLook), 0.05f);
	}
}

void CBeast::Attack5_Update(_float fTimeDelta)
{
	//set_state 했을때 랜덤으로 설정해주자 -> 랜덤 맨날 이상하게 돼서 일단은 차례대로 나오도록
	//공격 종류 달라짐, 하울링 했을때 1번째면 일반 플레이어 밀치기 두번째일때는 손톱 날리기

	if (!m_bAttack5EffectOn)
	{
		Create_Attack5OnEffect();
		m_bAttack5EffectOn = true;
	}


	switch (m_iHowlingAttack)
	{
	case 1:
		HowlingAttack1(fTimeDelta); //플레이어 밀어내는 공격
		break;
	case 2:
		HowlingAttack2(fTimeDelta); //플레이어가 점프 해야하는 공격
		break;
	case 3:
		HowlingAttack3(fTimeDelta); //기둥 생기는 공격
		Idle_HowlingAttack3(fTimeDelta);
		break;
	case 4:
		HowlingAttack4(fTimeDelta); //회오리 공격
		break;
	case 5:
		HowlingAttack5(fTimeDelta); //손오공 공격
		break;
	case 6:
		HowlingAttack6(fTimeDelta); //구슬치기
		break;
	}

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(0) && m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(0) + 25)
	{
		m_fWaveAcc += 1.f * fTimeDelta;
		if (m_fWaveAcc >= 0.24f)
		{
			m_fWaveAcc = 0.f;
			PTM->Create_InstanceParticle(L"BeastCry", m_pTransformCom->Get_WorldFloat4x4());
			PTM->Create_InstanceParticle(L"BeastCry_2", m_pTransformCom->Get_WorldFloat4x4());
			PTM->Create_InstanceParticle(L"BeastCry_3", m_pTransformCom->Get_WorldFloat4x4());
		}
	}

	//카메라 쉐이킹은 공통
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(3) && !m_bAttack5Voice)
	{
		m_bAttack5Voice = true;
		//카메라 쉐이킹
		CRM->Get_Cam()->Shake(0.3f, 1.1f, false, 0.2f);
		GI->PlaySoundW(L"DragonSkill14.ogg", SD_BEASTVOICE, 1.f);
		//GI->PlaySoundW(L"se_EN0301_attack5_2.wav", SD_BULLET, 1.f);
		
	}

}

void CBeast::HowlingAttack1(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bHurtCol)
	{
		//공격용 충돌체 키기
		Change_AttackColRange(_float3{ 30.f,8.f,8.f }, _float3{ 0.f, 0.f, 0.f });
		m_bAttackCol = true;
			
	}

	

}

void CBeast::HowlingAttack2(_float fTimeDelta)
{
	//손톱 생성
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(4) && m_bCreateClaw)
	{
		m_bCreateClaw = false;

		//손톱 생성 비스트 z값 좀 땡기고 y축도 올려서 

		CBeastClaw::CLAWINFO* Info = new CBeastClaw::CLAWINFO;
		Info->fBlurPower = 2.f;
		Info->vColor = { 0.f,0.f,0.f,1.f };
		Info->vColor2 = { 1.f, 0.f, 0.f, 1.f };
		Info->vLocalPos = { 0.f,1.165f,0.f,1.f };
		Info->vRatio = { 1.5f,3.f };
		Info->vRatioSpeed = { 0.f,0.f };
		Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(TEXT("BeastClaw"), LEVEL_STAGE5, TEXT("Layer_Monster"), &Info);
	}

}

void CBeast::HowlingAttack3(_float fTimeDelta)
{
	//좌표 네개 랜덤으로 받아와서 그 곳에 이펙트 생성하기



	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(5) && m_bCreateRange) //인덱스 4에서 첫번째 시작
	{
		m_bCreateRange = false;
		m_bHowlingAttack = true;
	
		++m_iHowlAttack3Count;
		m_pModelCom->Set_TickPerSecond(2.f);
		
	}

	if (!m_bCreateRange)
	{
		m_fPillarSoundAcc += 1.f * fTimeDelta;
		if (m_fPillarSoundAcc >= 0.8f)
		{
			Create_Effect(HOWLING_ATTACK3);
			m_bPillarSound = true;
			m_fPillarSoundAcc = 0.f;
		}
	}

	if (m_pModelCom->Get_KeyFrame() > m_pModelCom->Get_TimeLimit(5)) //인덱스 4에서 첫번째 시작
	{
		m_fAttack3CreateAcc += 1.f * fTimeDelta;
		if (m_fAttack3CreateAcc >= 0.5f)
		{
			CBossAttackRange::EFFECTRANGEINFO Info;
			Info.iIndex = GI->Get_Random(1,12);
			Info.bSound = false;
			GI->Add_GameObjectToLayer(L"BossAttackRange", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &Info);
			m_fAttack3CreateAcc = 0.f;
		
		}

		/*m_fPillarPattern += 1.f * fTimeDelta;
		if (m_fPillarPattern >= 2.f)
		{
			Create_Effect(HOWLING_ATTACK3);
			m_fPillarPattern = 0.f;
		}*/

	}

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(5) + 30.f) //인덱스 4에서 첫번째 시작
	{
		m_pModelCom->Set_TickPerSecond(24.f);
	}
}

void CBeast::HowlingAttack4(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(5) && !m_bHowlingAttack) //인덱스 4에서 토네이도 생성
	{
		
		m_bHowlingAttack = true;
	

	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(5) - 20 && !m_bCreateTornado) //인덱스 4에서 토네이도 생성
	{
		m_bCreateTornado = true;
		CBeastTornadoSide::TORNADOSIDEINFO* Info3 = new CBeastTornadoSide::TORNADOSIDEINFO;
		Info3->fBlurPower = 0.5f;
		Info3->fLifeTime = 5.f;
		Info3->vColor = { 0.f,0.f,0.f,1.f };
		Info3->vColor2 = { 0.f,0.f,0.f,1.f };
		XMStoreFloat4(&Info3->vLocalPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 20.f);
		Info3->vRatio = { 1.5f,1.5f };
		Info3->vRatioSpeed = { 5.f,5.f };
		Info3->vScale = { 1.f, 1.f, 1.f };
		Info3->vUVFixSpeed = { 3.f,0.f };
		Info3->vUVFixAcc = { 0.f,0.f };
		Info3->vWorldMatrix = PM->Get_PlayerWorldMatrix();
		GI->Add_GameObjectToLayer(L"BeastTornadoSide", TM->Get_CurLevel(), L"Layer_BeastEffect", &Info3);

		CBeastTornadoSide::TORNADOSIDEINFO* Info4 = new CBeastTornadoSide::TORNADOSIDEINFO;
		Info4->fBlurPower = 0.5f;
		Info4->fLifeTime = 5.f;
		Info4->vColor = { 0.f,0.f,0.f,1.f };
		Info4->vColor2 = { 1.f,0.f,0.f,1.f };
		XMStoreFloat4(&Info4->vLocalPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 20.f);
		Info4->vRatio = { 2.f,1.5f };
		Info4->vRatioSpeed = { 5.f,5.f };
		Info4->vScale = { 1.f, 1.f, 1.f };
		Info4->vUVFixSpeed = { 3.f,0.f };
		Info4->vUVFixAcc = { 1.f,0.f };
		Info4->fAngle = 90.f;
		Info4->vWorldMatrix = PM->Get_PlayerWorldMatrix();
		GI->Add_GameObjectToLayer(L"BeastTornadoSide", TM->Get_CurLevel(), L"Layer_BeastEffect", &Info4);

		CBeastTornado::TORNADOINFO* Info = new CBeastTornado::TORNADOINFO;
		Info->vWorldMatrix = PM->Get_PlayerWorldMatrix();
		Info->fBlurPower = 0.5f;
		Info->fTurnSpeed = 8.f;
		Info->vScale = { 1.f, 1.f, 1.f };
		XMStoreFloat4(&Info->vLocalPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 20.f);
		Info->vColor = { 0.f,0.f,0.f,1.f };
		Info->vColor2 = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 2.f,1.5f };
		Info->fLifeTime = 5.f;
		Info->vRatioSpeed = { 2.f,2.f };
		Info->vUVFixSpeed = { -0.5f,-0.5f };
		Info->bOrigin = true;
		GI->Add_GameObjectToLayer(L"BeastTornado", TM->Get_CurLevel(), L"Layer_BeastEffect", &Info);

		CBeastTornado::TORNADOINFO* Info2 = new CBeastTornado::TORNADOINFO;
		Info2->vWorldMatrix = PM->Get_PlayerWorldMatrix();
		Info2->fBlurPower = 0.5f;
		Info2->vScale = { 1.f, 1.f, 1.f };
		XMStoreFloat4(&Info2->vLocalPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 20.f);
		Info2->fTurnSpeed = 20.f;
		Info2->vColor = { 0.f,0.f,0.f,1.f };
		Info2->vColor2 = { 1.f,0.f,0.f,1.f };
		Info2->vRatio = { 2.f,1.5f };
		Info2->fLifeTime = 5.f;
		Info2->vRatioSpeed = { 2.f,2.f };
		Info2->vUVFixSpeed = { 1.f,1.f };
		Info2->bOrigin = false;
		GI->Add_GameObjectToLayer(L"BeastTornado", TM->Get_CurLevel(), L"Layer_BeastEffect", &Info2);
	}
}

void CBeast::HowlingAttack5(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(5))
	{
		m_bHowlingAttack = true;
		m_bProgress = true;
		//이펙트 생성
		m_fReserveLimitTime = 0.1f;
	}
}

void CBeast::HowlingAttack6(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(5) && m_bCreateObstacle) //구슬 생기기 시작
	{
		m_bHowlingAttack = true;
		m_bCreateObstacle = false;

		Create_MarvelObj();
		int iMarvelIndex = 0;
		//방해물 위치 잡아주고 생성해야함
		for (auto& pObs : m_pMarvelObstacle) //방해물 생성 시작
		{		
			pObs->Reset_State(vRouteObstaclePos[0][iMarvelIndex]);
			pObs->Appear_On();
			++iMarvelIndex;
		}

		GI->PlaySoundW(L"EFF_CHR_KarmaSwap_Orb_01.ogg", SD_BEAST6, 0.7f);
	
	}
}

void CBeast::Idle_HowlingAttack3(_float fTimeDelta)
{
	//시간 텀 두고 공격 여러번 더하기
	m_fHowlDelayAcc += 1.f * fTimeDelta;

	if (m_iHowlAttack3Count == 4)
	{
		m_bHowlingAttack = false;
		m_bAddTime = true;
		m_fHowlDelayAcc = 0.f;
		m_iHowlAttack3Count = 0;
	}

	/*if (m_fHowlDelayAcc > 2.f)
	{
		++m_iHowlAttack3Count;
		m_fHowlDelayAcc = 0.f;
		Create_Effect(HOWLING_ATTACK3);
	}*/

}

void CBeast::Idle_HowlingAttack4(_float fTimeDelta)
{
	//토네이도 사라질때 다시 공격 시작
	m_fHowlDelayAcc += 0.1f * fTimeDelta;

	if (m_fHowlDelayAcc > 1.5f)
	{
		m_bAddTime = true;
		m_bHowlingAttack = false;
		m_fHowlDelayAcc = 0.f;
	}

}

void CBeast::Idle_HowlingAttack5(_float fTimeDelta)
{
	//손오공 게임 시작
	if (m_bProgress)
		m_fCreateReserveAcc += 0.1f * fTimeDelta;

	if (m_fCreateReserveAcc >= m_fReserveLimitTime)
	{
		m_bProgress = false;
		m_bReserveGameStart = true;
	}

	if (m_bReserveGameStart)
	{
		m_bReserveGameStart = false;
		++m_iTurnIndex;
		Play_Game(m_iTurnIndex);
	}

	if (m_iTurnIndex == 21)
	{
		m_iTurnIndex = 0;
		m_bProgress = false;
		m_fCreateReserveAcc = 0.f;
		m_bAddTime = true;
		m_bHowlingAttack = false;
	}


}

void CBeast::Marvel_HowlingAttack6(_float fTimeDelta)
{
	EM->Set_MableAcc(m_fMarvelGameAcc);
	m_fMarvelGameAcc += 1.f * fTimeDelta;

	if (m_fMarvelGameAcc >= 0.01f && !m_bLineTrailOn)
	{		
		for (auto& pMarvelTrail : m_pMarvelTrail)
			pMarvelTrail->Render_OnOff(true);
		m_bLineTrailOn = true;
	}

	Render_MarvelTrail();
		
	if (!m_bBreath)
		LookAtPlayer();

	if (m_pMarvelBall->Get_ExplosionState() || m_pMarvelBall->Get_SuccessState()) //방해물에 닿거나 보스에 맞았거나
	{
		m_bExplosion = true;
	}

	//if (m_bExplosion) //충돌했을때
	//{
	//	//Off_MarvelObj(); //관련 오브젝트들 랜더 끄기
	//	m_bExplosion = false;	
	//}


	if (m_fMarvelGameAcc >= 30.f && m_bOneSet) //시간 지나면 구슬 움직이기 스타트
	{
		m_bOneSet = false;
		Create_Obstacle();


		for (auto& pMarvelTrail : m_pMarvelTrail)
			pMarvelTrail->Render_OnOff(false);

		m_pMarvelBall->Chase_Start();
		m_pMarvelBall->Attack_On(true);

		GI->PlaySoundW(L"EFF_CHR_Chakram_Attack_Shot_01_A.ogg", SD_BEAST6, 0.7f);
	}

	if (m_fMarvelGameAcc >= 36.f && !m_bOneCheckPos)
	{
		CViewAlpha::VIEWALPHAINFO* Info2 = new CViewAlpha::VIEWALPHAINFO;
		Info2->fBlurPower = 1.f;
		Info2->vColor1 = { 1.f,1.f,1.f,1.f };
		Info2->vColor2 = { 1.f,1.f,1.f,1.f };
		Info2->vRatio = { 1.f,1.f };
		Info2->vRatioSpeed = { 3.f,2.f };
		GI->Add_GameObjectToLayer(L"ViewAlpha", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2);
			

		m_bBreath = true;
		m_bFireBreath = true;
		CRM->Get_Cam()->Shake(0.5f, 0.2f, false, 0.2f);
		m_bOneCheckPos = true;
		m_bBreath = true;
		m_bOneMatrix = true;
		m_fPlayerStartPos = PM->Get_PlayerPos();
		GI->PlaySoundW(L"01A_202_UltCutscene_03C.ogg", SD_BEAST4, 1.f);
		GI->PlaySoundW(L"BreathBack.mp3", SD_BEAST7, 1.f);

		_matrix Breathmatrix = m_pBreathNode->Get_CombinedTransformation() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
		Breathmatrix.r[0] = { 1.f,0.f,0.f,0.f };
		Breathmatrix.r[1] = { 0.f,1.f,0.f,0.f };
		Breathmatrix.r[2] = { 0.f,0.f,1.f,0.f };
		Breathmatrix.r[3] += XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 4.f;
		_float4x4 WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, Breathmatrix);
		PTM->Create_InstanceParticle(L"BreathStartWhite", WorldMatrix);
	}
}

void CBeast::Create_ReserveTrail(_uint iTrailIndex)
{
	CReserveTrail::RESERVETRAILINFO _tInfo;
	_tInfo.bFixColor = true;
	_tInfo.fBlurPower = 1.3f;
	_tInfo.fLifeTime = 0.5f;
	_tInfo.fStaticY = 0.1f;
	_tInfo.fLimitScaleX = 2.f;
	_tInfo.fScaleXSpeed = 3.2f;
	_tInfo.fScaleX = 0.1f;
	_tInfo.vColor = { 1.f,0.067f,0.076f,1.f };
	_tInfo.vColor2 = { 0.f, 0.f, 0.f, 1.f };
	_tInfo.vRatio = { 1.f, 1.f };

	switch (iTrailIndex)
	{
	case 1: //일직선 세로
		_tInfo.vPosition = _float4{ m_fPlayerPos.x, 0.f, -34.690f, 1.f };
		_tInfo.vTargetPos = _float4{ m_fPlayerPos.x, 0.f, -73.755f, 1.f };
		m_fPrePos = m_fPlayerPos.x;
		break;
	case 2: // 일직선 가로
		_tInfo.vPosition = _float4{ -20.245f, 0.f, m_fPlayerPos.z, 1.f };
		_tInfo.vTargetPos = _float4{ 20.245f, 0.f, m_fPlayerPos.z, 1.f };
		m_fPrePos = m_fPlayerPos.z;
		break;
	case 3: //일직선 세로 - 플레이어랑 좀 떨어짐
		_tInfo.vPosition = _float4{ m_fPrePos + 4.f, 0.f, -34.690f, 1.f };
		_tInfo.vTargetPos = _float4{ m_fPrePos + 4.f, 0.f, -73.755f, 1.f };
		break;
	case 4: //일직선 가로 - 플레이어랑 좀 떨어짐
		_tInfo.vPosition = _float4{ -20.245f, 0.f, m_fPrePos + 4.f, 1.f };
		_tInfo.vTargetPos = _float4{ 20.245f, 0.f, m_fPrePos + 4.f, 1.f };
		break;
	case 5: //값 정해줄수있는거
		_tInfo.vPosition = _float4{ m_fReservePos.x, 0.f, m_fReservePos.z, 1.f };
		_tInfo.vTargetPos = _float4{ m_fReserveTarget.x, 0.f,  m_fReserveTarget.z, 1.f };
		break;
	case 6: //일직선 세로 - 플레이어랑 좀 떨어짐
		_tInfo.vPosition = _float4{ m_fPrePos - 4.f, 0.f, -34.690f, 1.f };
		_tInfo.vTargetPos = _float4{ m_fPrePos - 4.f, 0.f, -73.755f, 1.f };
		break;
	case 7: //일직선 가로 - 플레이어랑 좀 떨어짐
		_tInfo.vPosition = _float4{ -20.245f, 0.f, m_fPrePos - 4.f, 1.f };
		_tInfo.vTargetPos = _float4{ 20.245f, 0.f, m_fPrePos - 4.f, 1.f };
		break;
	}

	(GI->Add_GameObjectToLayer(L"ReserveTrail", GI->Get_Cur_LevelIndex(), L"Layer_MonsterEffect", &_tInfo));
	m_bProgress = true;
	m_fCreateReserveAcc = 0.f;

}

void CBeast::Play_Game(_uint iTurnIndex)
{
	switch (m_iTurnIndex)
	{
	case 1:
		Create_ReserveTrail(1);
		m_fReserveLimitTime = 0.2f;
		break;
	case 2:
		Create_ReserveTrail(2);
		m_fReserveLimitTime = 0.2f;
		break;
	case 3:
		Create_ReserveTrail(1);
		Create_ReserveTrail(2);
		m_fReserveLimitTime = 0.2f;
		break;
	case 4: // 4,5,6,7 한패턴 - 직선 네개
		Create_ReserveTrail(1);
		m_fReserveLimitTime = 0.08f;
		break;
	case 5:
		Create_ReserveTrail(3);
		Create_ReserveTrail(6);
		m_fReserveLimitTime = 0.06f;
		break;
	case 6:
		Create_ReserveTrail(2);
		m_fReserveLimitTime = 0.08f;
		break;
	case 7:
		Create_ReserveTrail(4);
		Create_ReserveTrail(7);
		m_fReserveLimitTime = 0.2f;
		break;


	case 8: //원점에서 모이는 4개 텀 조금씩
		m_fReservePos.x = 13.6f;
		m_fReservePos.z = -33.373f;
		m_fReserveTarget.x = -14.2f;
		m_fReserveTarget.z = -67.674f;
		Create_ReserveTrail(5);
		m_fReserveLimitTime = 0.08f;
		break;
	case 9: //원점에서 모이는 4개 텀 조금씩
		Create_ReserveTrail(1);
		m_fReserveLimitTime = 0.04f;
		break;
	case 10: //원점에서 모이는 4개 텀 조금씩
		m_fReservePos.x = -9.24f;
		m_fReservePos.z = -33.381f;
		m_fReserveTarget.x = 12.14f;
		m_fReserveTarget.z = -70.2f;
		Create_ReserveTrail(5);
		m_fReserveLimitTime = 0.08f;
		break;
	case 11: //원점에서 모이는 4개 텀 조금씩
		Create_ReserveTrail(2);
		m_fReserveLimitTime = 0.2f;
		break;

	case 12:
		Create_ReserveTrail(1);
		Create_ReserveTrail(2);
		Create_ReserveTrail(4);
		m_fReservePos.x = 13.6f;
		m_fReservePos.z = -33.373f;
		m_fReserveTarget.x = -14.2f;
		m_fReserveTarget.z = -67.674f;
		Create_ReserveTrail(5);
		m_fReservePos.x = -9.24f;
		m_fReservePos.z = -33.381f;
		m_fReserveTarget.x = 12.14f;
		m_fReserveTarget.z = -70.2f;
		Create_ReserveTrail(5);
		m_fReserveLimitTime = 0.2f;
		break;
		//계속 여러방향으로 7개 

	case 13: //대각선
		if (!m_bMonkeyParttern)
		{
			m_fReservePos.x = -12.346f;
			m_fReservePos.z = -69.166f;
			m_fReserveTarget.x = 6.225f;
			m_fReserveTarget.z = -37.515f;
			Create_ReserveTrail(5);
			m_fReserveLimitTime = 0.05f;
			m_bMonkeyParttern = true;
		}
		else
		{
			m_fReservePos.x = 12.346f;
			m_fReservePos.z = -69.166f;
			m_fReserveTarget.x = -6.225f;
			m_fReserveTarget.z = -37.515f;
			Create_ReserveTrail(5);
			m_fReserveLimitTime = 0.05f;
		}
		break;
	case 14: 
		if (!m_bMonkeyParttern)
		{
			m_fReservePos.x = 0.246f;
			m_fReservePos.z = -38.988f;
			m_fReserveTarget.x = -18.575f;
			m_fReserveTarget.z = -73.378f;
			Create_ReserveTrail(5);
			m_fReserveLimitTime = 0.01f;
		}
		else
		{
			m_fReservePos.x = 2.246f;
			m_fReservePos.z = -36.988f;
			m_fReserveTarget.x = -16.575f;
			m_fReserveTarget.z = -72.378f;
			Create_ReserveTrail(5);
			m_fReserveLimitTime = 0.01f;
		}
		break;
	case 15: 
		if (!m_bMonkeyParttern)
		{
			m_fReservePos.x = 7.87f;
			m_fReservePos.z = -68.982f;
			m_fReserveTarget.x = -17.011f;
			m_fReserveTarget.z = -45.218f;
			Create_ReserveTrail(5);
			m_fReserveLimitTime = 0.02f;
		}
		else
		{
			m_fReservePos.x = 7.87f;
			m_fReservePos.z = -68.982f;
			m_fReserveTarget.x = -17.011f;
			m_fReserveTarget.z = -45.218f;
			Create_ReserveTrail(5);
			m_fReserveLimitTime = 0.02f;
		}
		break;
	case 16: 
		if (!m_bMonkeyParttern)
		{
			m_fReservePos.x = 4.516f;
			m_fReservePos.z = -32.867f;
			m_fReserveTarget.x = 4.516f;
			m_fReserveTarget.z = -69.806f;
			Create_ReserveTrail(5);
			m_fReserveLimitTime = 0.02f;
		}
		else
			Create_ReserveTrail(2);
		break;
	case 17: //대각선
		if (!m_bMonkeyParttern)
		{
			m_fReservePos.x = -18.497f;
			m_fReservePos.z = -58.472f;
			m_fReserveTarget.x = -0.335f;
			m_fReserveTarget.z = -33.257f;
			Create_ReserveTrail(5);
			m_fReserveLimitTime = 0.01f;
		}
		else
		{
				m_fReservePos.x = 4.516f;
				m_fReservePos.z = -32.867f;
				m_fReserveTarget.x = 4.516f;
				m_fReserveTarget.z = -69.806f;
				Create_ReserveTrail(5);
				m_fReserveLimitTime = 0.02f;
		}
		break;
	case 18: 
		if (!m_bMonkeyParttern)
		{
			m_fReservePos.x = 14.228f;
			m_fReservePos.z = -44.069f;
			m_fReserveTarget.x = -16.445f;
			m_fReserveTarget.z = -44.069f;
			Create_ReserveTrail(5);
			m_fReserveLimitTime = 0.03f;
		}
		else
		{
			m_fReservePos.x = 14.228f;
			m_fReservePos.z = -44.069f;
			m_fReserveTarget.x = -16.445f;
			m_fReserveTarget.z = -44.069f;
			Create_ReserveTrail(5);
			m_fReserveLimitTime = 0.03f;
		}
		break;
	case 19: //대각선
		if (!m_bMonkeyParttern)
		{
			m_fReservePos.x = 16.580f;
			m_fReservePos.z = -63.443f;
			m_fReserveTarget.x = -7.124f;
			m_fReserveTarget.z = -32.899f;
			Create_ReserveTrail(5);
			m_fReserveLimitTime = 0.02f;
		}
		else
		{
			m_fReservePos.x = 13.f;
			m_fReservePos.z = -57.8f;
			m_fReserveTarget.x = -6.4f;
			m_fReserveTarget.z = -31.899f;
			Create_ReserveTrail(5);
			m_fReserveLimitTime = 0.02f;
		}
		break;
	case 20: //대각선
		if (!m_bMonkeyParttern)
		{
			m_fReservePos.x = -13.717f;
			m_fReservePos.z = -39.832f;
			m_fReserveTarget.x = 6.382f;
			m_fReserveTarget.z = -70.594f;
			Create_ReserveTrail(5);
			m_fReserveLimitTime = 0.2f;
		}
		else
		{
			m_fReservePos.x = -13.717f;
			m_fReservePos.z = -39.832f;
			m_fReserveTarget.x = 6.382f;
			m_fReserveTarget.z = -70.594f;
			Create_ReserveTrail(5);
			m_fReserveLimitTime = 0.2f;
		}			
		break;

	case 21: //마지막
		if (!m_bMonkeyParttern)
		{
			m_fReservePos.x = -12.346f;
			m_fReservePos.z = -69.166f;
			m_fReserveTarget.x = 6.225f;
			m_fReserveTarget.z = -37.515f;
			Create_ReserveTrail(5);
			m_fReservePos.x = 0.246f;
			m_fReservePos.z = -38.988f;
			m_fReserveTarget.x = -18.575f;
			m_fReserveTarget.z = -73.378f;
			Create_ReserveTrail(5);
			m_fReservePos.x = 7.87f;
			m_fReservePos.z = -68.982f;
			m_fReserveTarget.x = -17.011f;
			m_fReserveTarget.z = -45.218f;
			Create_ReserveTrail(5);
			Create_ReserveTrail(2);
			m_fReservePos.x = -18.497f;
			m_fReservePos.z = -58.472f;
			m_fReserveTarget.x = -0.335f;
			m_fReserveTarget.z = -33.257f;
			Create_ReserveTrail(5);
			m_fReservePos.x = 14.228f;
			m_fReservePos.z = -44.069f;
			m_fReserveTarget.x = -16.445f;
			m_fReserveTarget.z = -44.069f;
			Create_ReserveTrail(5);
			m_fReservePos.x = 16.580f;
			m_fReservePos.z = -63.443f;
			m_fReserveTarget.x = -7.124f;
			m_fReserveTarget.z = -32.899f;
			Create_ReserveTrail(5);
			m_fReservePos.x = -13.717f;
			m_fReservePos.z = -39.832f;
			m_fReserveTarget.x = 6.382f;
			m_fReserveTarget.z = -70.594f;
			Create_ReserveTrail(5);
		}
		else
		{
			m_fReservePos.x = 12.346f;
			m_fReservePos.z = -69.166f;
			m_fReserveTarget.x = -6.225f;
			m_fReserveTarget.z = -37.515f;
			Create_ReserveTrail(5);
			m_fReservePos.x = 2.246f;
			m_fReservePos.z = -36.988f;
			m_fReserveTarget.x = -16.575f;
			m_fReserveTarget.z = -72.378f;
			Create_ReserveTrail(5);
			m_fReservePos.x = 7.87f;
			m_fReservePos.z = -68.982f;
			m_fReserveTarget.x = -17.011f;
			m_fReserveTarget.z = -45.218f;
			Create_ReserveTrail(5);
			m_fReservePos.x = 4.516f;
			m_fReservePos.z = -32.867f;
			m_fReserveTarget.x = 4.516f;
			m_fReserveTarget.z = -69.806f;
			Create_ReserveTrail(5);
			m_fReservePos.x = 4.516f;
			m_fReservePos.z = -32.867f;
			m_fReserveTarget.x = 4.516f;
			m_fReserveTarget.z = -69.806f;
			Create_ReserveTrail(5);
			m_fReservePos.x = 14.228f;
			m_fReservePos.z = -44.069f;
			m_fReserveTarget.x = -16.445f;
			m_fReserveTarget.z = -44.069f;
			Create_ReserveTrail(5);
			m_fReservePos.x = 13.f;
			m_fReservePos.z = -57.8f;
			m_fReserveTarget.x = -6.4f;
			m_fReserveTarget.z = -31.899f;
			Create_ReserveTrail(5);
			m_fReservePos.x = -13.717f;
			m_fReservePos.z = -39.832f;
			m_fReserveTarget.x = 6.382f;
			m_fReserveTarget.z = -70.594f;
			Create_ReserveTrail(5);
		}
		break;
	default:
		break;
	}
}

void CBeast::Off_MarvelObj()
{
	for (auto& iter : m_pMarvelObstacle)
		iter->Render_OnOff(false);

	for (auto& pMarvelTrail : m_pMarvelTrail)
		pMarvelTrail->Render_OnOff(false);

	if (m_pMarvelBall->Get_ExplosionState())
	{
		m_bFireBreath = true; //방해물에 충돌했을때는 브레스 쏘기
		m_pMarvelBall->Attack_On(false);
	}
	if (m_pMarvelBall->Get_SuccessState())
		On_Hit((CCreture*)m_pMarvelBall);

	m_pMarvelBall->Render_OnOff(false);

}

void CBeast::Render_MarvelTrail()
{
	
	if (m_iTrailIndex >= 5)
		m_iTrailIndex = 0;

	for (auto& pMarvelTrail : m_pMarvelTrail)
	{
		if (m_iTrailIndex == 0)
			pMarvelTrail->SetUp_ReserveTrail({ -0.451f, 0.02f, -66.831f, 1.f }, m_pMarvelPush[m_iTrailIndex]->Get_WorldPos());
		else if(m_iTrailIndex >=1 && m_iTrailIndex <= 3)
			pMarvelTrail->SetUp_ReserveTrail(m_pMarvelPush[m_iTrailIndex - 1]->Get_WorldPos(), m_pMarvelPush[m_iTrailIndex]->Get_WorldPos());
		else
			pMarvelTrail->SetUp_ReserveTrail(m_pMarvelPush[m_iTrailIndex - 1]->Get_WorldPos(), { 0.051f, 0.7f, -24.f, 1.f });
		//push는 인덱스 3까지 있음
		++m_iTrailIndex;
	}

}

void CBeast::Create_MarvelObj()
{
	if (m_bCreateMarvelBall)
	{
		m_bCreateMarvelBall = false;
		m_pMarvelBall->Render_OnOff(true);
	}

	if (m_bCreatePush)
	{
		m_bCreatePush = false;

		Set_PushNewPos();
		
		
	}

	
}

void CBeast::Create_Obstacle()
{
	_uint iObstacle = 0;
	if (iObstacle <= 3)
	{
		for (auto& iter : m_pMarvelPush)
		{
			_float4 vPos = iter->Get_WorldPos();
			m_pMarvelBall->Set_TargetPos(vPos, iObstacle);
			++iObstacle;
		}
	}
}

void CBeast::Stun_Update(_float fTimeDelta)
{
	m_fStunSoundAcc += 1.f * fTimeDelta;
	if (m_fStunSoundAcc >= 1.5f && !m_bBeastEndEnd)
	{
		GI->PlaySoundW(L"DragonWait.ogg", SD_BEAST2, 0.8f);
		m_fStunSoundAcc = 0.f;
	}

	m_bStunCol = true;
	//m_bStun = true;
}

void CBeast::StunBegin_Update(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(0))
	{
		m_vTargetLook = { 0.f, 0.f, -1.f };
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&m_vTargetLook), 0.05f);
	}

	if (m_pModelCom->Get_KeyFrame() == 68.f && !m_bStunBeginDownSound)
	{
		GI->PlaySoundW(L"DragonAttack13.ogg", SD_BEAST6, 0.6f);
		m_bStunBeginDownSound = true;
		_matrix WindMatrix = XMMatrixIdentity();
		WindMatrix.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float4x4 Wind;
		XMStoreFloat4x4(&Wind, WindMatrix);

		CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
		Info3->fBlurPower = 1.f;
		Info3->vLocalPos = { 0.f,0.f,-5.f,1.f };
		Info3->vColor = { 0.5f,0.5f,0.5f,1.f };
		Info3->vColor2 = { 0.4f,0.4f,0.4f,1.f };
		Info3->vRatio = { 1.3f,1.3f };
		Info3->vScale = { 20.f,10.f,20.f };
		Info3->vRatioSpeed = { 1.2f,1.2f };
		Info3->vScaleSpeed = { 100.f,200.f,100.f };
		Info3->fTurnSpeed = 0.f;
		Info3->vWorldMatrix = Wind;
		Info3->fTurnSpeed = 5.f;
		GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3);


		CPlayer01Wind2::WIND2INFO* Info4 = new CPlayer01Wind2::WIND2INFO;
		Info4->fBlurPower = 1.f;
		Info4->fTurnSpeed = 0.f;
		Info4->fTurnSpeed = 3.f;
		Info4->vScale = { 20.f,10.f,20.f };
		Info4->vLocalPos = { 0.f,0.f,-5.f,1.f };
		Info4->vColor = { 0.5f,0.5f,0.5f,1.f };
		Info4->vColor2 = { 0.4f,0.4f,0.4f,1.f };
		Info4->vRatio = { 1.3f,1.3f };
		Info4->vRatioSpeed = { 1.2f,1.2f };
		Info4->vScaleSpeed = { 100.f,200.f,100.f };
		Info4->vWorldMatrix = Wind;
		GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4);
	}

	
}

void CBeast::StunRecover_Update(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() == 65.f && !m_bStunRecoverSound)
	{
		m_bStunRecoverSound = true;
		GI->PlaySoundW(L"DragonStart.ogg", SD_BEAST5, 0.7f);
	}

	if (m_pModelCom->Get_KeyFrame() >= 65.f && !m_bPase2On)
	{
		TM->Set_2Page(true);
	}

	if (m_pModelCom->Get_KeyFrame() >= 65.f && m_pModelCom->Get_KeyFrame() <= 90.f)
	{
		m_fStunRecoverAcc += 1.f * fTimeDelta;
		if (m_fStunRecoverAcc >= 0.24f)
		{
			m_fStunRecoverAcc = 0.f;
			PTM->Create_InstanceParticle(L"BeastCry", m_pTransformCom->Get_WorldFloat4x4());
			PTM->Create_InstanceParticle(L"BeastCry_2", m_pTransformCom->Get_WorldFloat4x4());
			PTM->Create_InstanceParticle(L"BeastCry_3", m_pTransformCom->Get_WorldFloat4x4());
		}
	}
}

void CBeast::LookAtPlayer(_float fLiearTime)
{
	Set_Dir();
	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&m_vTargetLook), fLiearTime);

}

HRESULT CBeast::Ready_Components()
{

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;     //Prototype_Component_Shader_AnimModel_Emissive     //   Prototype_Component_Shader_AnimModel

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Beast"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	//m_pEmissiveCom
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Beast_Emi"), TEXT("Com_TextureEmi"), (CComponent**)&m_pEmissiveCom)))
		return E_FAIL;

	m_pModelCom->Set_RootHierarchy(3);

	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;   
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(9.5f, 9.5f, 9.5f);
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Push_Range"); // 밀어내기 위함

	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(10.f, 10.f, 10.f);
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Attack"); // 밀어내기 위함

	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	//상체쪽
	CHierarchyNode*		pHead = m_pModelCom->Get_HierarchyNode("HeadEnd_M");
	if (nullptr == pHead)
		return E_FAIL;
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(2.2f, 2.f, 2.f);
	ColDesc.vCenter = _float3(1.1f, 0.5f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.pSocket = pHead;
	XMStoreFloat4x4(&ColDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Body1"); // 기본 충돌체 - 몬스터끼리 밀어내고 플레이어 또는 플레이어 공격이랑 충돌하기위한거

	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	//오른손
	CHierarchyNode*		pRHand = m_pModelCom->Get_HierarchyNode("Wrist_R");
	if (nullptr == pRHand)
		return E_FAIL;
	ColDesc.vSize = _float3(4.f, 3.2f, 3.2f);
	ColDesc.vCenter = _float3(0.3f, -0.9f, 0.f);
	ColDesc.pSocket = pRHand;
	ColDesc.iFlag = 0;
	XMStoreFloat4x4(&ColDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "Monster_RHand"); //몬스터 공격용 충돌체
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	ColDesc.vSize = _float3(2.8f, 2.8f, 2.8f);
	ColDesc.vCenter = _float3(0.8f, -0.2f, 0.f);
	ColDesc.pSocket = pRHand;
	ColDesc.iFlag = 0;
	XMStoreFloat4x4(&ColDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "Monster_Body2"); 
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	CHierarchyNode*		pLHand = m_pModelCom->Get_HierarchyNode("Wrist_L");
	if (nullptr == pLHand)
		return E_FAIL;
	ColDesc.vSize = _float3(3.8f, 3.2f, 3.2f);
	ColDesc.vCenter = _float3(-1.2f, 0.f, -0.1f);
	ColDesc.pSocket = pLHand;
	ColDesc.iFlag = 0;
	XMStoreFloat4x4(&ColDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "Monster_LHand"); //몬스터 공격용 충돌체
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	ColDesc.vSize = _float3(2.4f, 2.4f, 2.4f);
	ColDesc.vCenter = _float3(-1.2f, 0.f, -0.1f);
	ColDesc.pSocket = pLHand;
	ColDesc.iFlag = 0;
	XMStoreFloat4x4(&ColDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "Monster_Body3"); //몬스터 공격용 충돌체
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	m_pClawNode = m_pModelCom->Get_HierarchyNode("Fingers1_R");

	m_pBreathNode = m_pModelCom->Get_HierarchyNode("Jaw_M");

	return S_OK;
}

HRESULT CBeast::Create_Navigation(_uint iNaviNum)
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
	case 5:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage5Navigation"), TEXT("Stage5"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
		break;
	}

	_int iNaviIndex = m_pNavigationCom->Find_NaviIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pNavigationCom->Set_NaviIndex(iNaviIndex);

	return S_OK;
}

HRESULT CBeast::Create_MarvelObstacle()
{
	//밀수있는 오브젝트 나중에 인덱스로 구분해야할듯
	CCreture::CREATEINFO _tInfo;
	ZeroMemory(&_tInfo, sizeof(CCreture::CREATEINFO));

	_tInfo.iNaviNum = 5;
	//float4 담은 배열 필요 
	_tInfo.iIndex = 0;
	CGameObject*		pObstacleA = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("BeastMarvelObstacle"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &_tInfo, &pObstacleA));
	if (nullptr == pObstacleA)
		return E_FAIL;
	m_pMarvelObstacle.push_back((CBeastMarvelObstacle*)pObstacleA);

	_tInfo.iIndex = 1;
	CGameObject*		pObstacleB = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("BeastMarvelObstacle"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &_tInfo, &pObstacleB));
	if (nullptr == pObstacleB)
		return E_FAIL;
	m_pMarvelObstacle.push_back((CBeastMarvelObstacle*)pObstacleB);

	_tInfo.iIndex = 2;
	CGameObject*		pObstacleC = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("BeastMarvelObstacle"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &_tInfo, &pObstacleC));
	if (nullptr == pObstacleC)
		return E_FAIL;
	m_pMarvelObstacle.push_back((CBeastMarvelObstacle*)pObstacleC);
	
	_tInfo.iIndex = 3;

	CGameObject*		pObstacleD = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("BeastMarvelObstacle"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &_tInfo, &pObstacleD));
	if (nullptr == pObstacleD)
		return E_FAIL;
	m_pMarvelObstacle.push_back((CBeastMarvelObstacle*)pObstacleD);

	_tInfo.iIndex = 4;

	CGameObject*		pObstacleE = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("BeastMarvelObstacle"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &_tInfo, &pObstacleE));
	if (nullptr == pObstacleE)
		return E_FAIL;
	m_pMarvelObstacle.push_back((CBeastMarvelObstacle*)pObstacleE);

	_tInfo.iIndex = 5;
	CGameObject*		pObstacleF = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("BeastMarvelObstacle"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &_tInfo, &pObstacleF));
	if (nullptr == pObstacleF)
		return E_FAIL;
	m_pMarvelObstacle.push_back((CBeastMarvelObstacle*)pObstacleF);

	_tInfo.iIndex = 6;
	CGameObject*		pObstacleG = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("BeastMarvelObstacle"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &_tInfo, &pObstacleG));
	if (nullptr == pObstacleG)
		return E_FAIL;
	m_pMarvelObstacle.push_back((CBeastMarvelObstacle*)pObstacleF);

	_tInfo.iIndex = 7;
	CGameObject*		pObstacleH = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("BeastMarvelObstacle"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &_tInfo, &pObstacleH));
	if (nullptr == pObstacleH)
		return E_FAIL;
	m_pMarvelObstacle.push_back((CBeastMarvelObstacle*)pObstacleH);

	_tInfo.iIndex = 8;
	CGameObject*		pObstacleI = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("BeastMarvelObstacle"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &_tInfo, &pObstacleI));
	if (nullptr == pObstacleI)
		return E_FAIL;
	m_pMarvelObstacle.push_back((CBeastMarvelObstacle*)pObstacleI);

	_tInfo.iIndex = 9;
	CGameObject*		pObstacleI4 = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("BeastMarvelObstacle"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &_tInfo, &pObstacleI4));
	if (nullptr == pObstacleI4)
		return E_FAIL;
	m_pMarvelObstacle.push_back((CBeastMarvelObstacle*)pObstacleI4);

	_tInfo.iIndex = 10;
	CGameObject*		pObstacleI5 = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("BeastMarvelObstacle"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &_tInfo, &pObstacleI5));
	if (nullptr == pObstacleI5)
		return E_FAIL;
	m_pMarvelObstacle.push_back((CBeastMarvelObstacle*)pObstacleI5);

	return S_OK;
}

HRESULT CBeast::Create_MarvelPush()
{
	//경우의 수 한 두개 준다음에 그 위치에 생성되게 하자
		
	CBeastMarvelPush::PUSHINFO* Info = new CBeastMarvelPush::PUSHINFO;
	Info->fBlurPower = 1.f;
	Info->iIndex = 1;
	Info->iNaviNum = 5;
	Info->vColor = { 1.f,0.f,0.f,1.f };
	Info->vColor2 = { 0.f,0.f,0.f,1.f };
	Info->vPosition = { 0.451f, 0.02f, -60.f, 1.f };
	Info->vRatio = { 0.5f,1.f };
	Info->vRatioSpeed = { 2.f,2.f };
	Info->vScale = { 1.f,1.f,1.f };
	Info->vScaleSpeed = { 3.f,3.f,3.f };
	CGameObject*		pPushA = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("BeastMarvelPush"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &Info, &pPushA));
	if (nullptr == pPushA)
		return E_FAIL;
	m_pMarvelPush.push_back((CBeastMarvelPush*)pPushA);

	CBeastMarvelPush::PUSHINFO* Info2 = new CBeastMarvelPush::PUSHINFO;
	Info2->fBlurPower = 1.f;
	Info2->iIndex = 2;
	Info2->iNaviNum = 5;
	Info2->vColor = { 1.f,1.f,0.f,1.f };
	Info2->vColor2 = { 1.f,0.f,0.f,1.f };
	Info2->vPosition = { -8.f, 0.02f, -53.f, 1.f };
	Info2->vRatio = { 0.5f,1.f };
	Info2->vRatioSpeed = { 2.f,2.f };
	Info2->vScale = { 1.f,1.f,1.f };
	Info2->vScaleSpeed = { 3.f,3.f,3.f };

	CGameObject*		pPushB = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("BeastMarvelPush"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &Info2, &pPushB));
	if (nullptr == pPushB)
		return E_FAIL;
	m_pMarvelPush.push_back((CBeastMarvelPush*)pPushB);


	CBeastMarvelPush::PUSHINFO* Info3 = new CBeastMarvelPush::PUSHINFO;
	Info3->fBlurPower = 1.f;
	Info3->iIndex = 3;
	Info3->iNaviNum = 5;
	Info3->vColor = { 0.f,0.f,1.f,1.f };
	Info3->vColor2 = { 0.f,0.5f,1.f,1.f };
	Info3->vPosition = { 0.451f, 0.02f, -40.f, 1.f };
	Info3->vRatio = { 0.5f,1.f };
	Info3->vRatioSpeed = { 2.f,2.f };
	Info3->vScale = { 1.f,1.f,1.f };
	Info3->vScaleSpeed = { 3.f,3.f,3.f };
	CGameObject*		pPushC = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("BeastMarvelPush"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &Info3, &pPushC));
	if (nullptr == pPushC)
		return E_FAIL;
	m_pMarvelPush.push_back((CBeastMarvelPush*)pPushC);

	CBeastMarvelPush::PUSHINFO* Info4 = new CBeastMarvelPush::PUSHINFO;
	Info4->fBlurPower = 1.f;
	Info4->iIndex = 4;
	Info4->iNaviNum = 5;
	Info4->vColor = { 0.f,1.f,0.9f,1.f };
	Info4->vColor2 = { 0.f,1.f,0.9f,1.f };
	Info4->vPosition = { 8.f, 0.02f, -53.f, 1.f };
	Info4->vRatio = { 0.5f,1.f };
	Info4->vRatioSpeed = { 2.f,2.f };
	Info4->vScale = { 1.f,1.f,1.f };
	Info4->vScaleSpeed = { 3.f,3.f,3.f };
	CGameObject*		pPushD = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("BeastMarvelPush"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &Info4, &pPushD));
	if (nullptr == pPushD)
		return E_FAIL;
	m_pMarvelPush.push_back((CBeastMarvelPush*)pPushD);
	return S_OK;
}

HRESULT CBeast::Create_MarvelBall()
{
	CBeastMarvelBall::BALLINFO* Info = new CBeastMarvelBall::BALLINFO;
	Info->fBlurPower = 2.f;
	Info->vColor = { 1.f,1.f,1.f,1.f };
	Info->vColor2 = { 0.f,1.f,1.f,1.f };
	Info->vPos = { -0.451f, 0.02f, -66.831f, 1.f };
	Info->vRatio = { 1.f,10.f };
	Info->vRatioSpeed = { 2.f,20.f };
	Info->vScale = { 7.f,7.f,7.f };
	Info->vScaleSpeed = { 5.f,5.f,5.f };
	CGameObject*		pMarvelBall = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("BeastMarvelBall"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &Info, &pMarvelBall));
	m_pMarvelBall = (CBeastMarvelBall*)pMarvelBall;

	return S_OK;
}

HRESULT CBeast::Create_MarvelReserveTrail()
{

	CMarvelReserveTaril::RESERVETRAILINFO  _tInfo;
	ZeroMemory(&_tInfo, sizeof(CMarvelReserveTaril::RESERVETRAILINFO));

	_tInfo.bFixColor = true;
	_tInfo.fBlurPower = 1.2f;
	_tInfo.fStaticY = 0.655f;
	_tInfo.fScaleX = 0.4f;
	_tInfo.vColor = { 1.f,0.127f,0.096f,1.f };
	_tInfo.vColor2 = { 1.f,0.097f,0.056f,1.f };
	_tInfo.vRatio = { 1.f, 1.2f };
	_tInfo.vPosition = { 0.f, 0.02f, 0.f, 1.f };
	
	//구슬이랑 이어지는 트레일도 하나 더 필요
	CGameObject*		pTrailA = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("MarvelReserveTaril"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &_tInfo, &pTrailA));
	if (nullptr == pTrailA)
		return E_FAIL;
	m_pMarvelTrail.push_back((CMarvelReserveTaril*)pTrailA);

	CGameObject*		pTrailB = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("MarvelReserveTaril"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &_tInfo, &pTrailB));
	if (nullptr == pTrailB)
		return E_FAIL;
	m_pMarvelTrail.push_back((CMarvelReserveTaril*)pTrailB);

	CGameObject*		pTrailC = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("MarvelReserveTaril"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &_tInfo, &pTrailC));
	if (nullptr == pTrailC)
		return E_FAIL;
	m_pMarvelTrail.push_back((CMarvelReserveTaril*)pTrailC);

	CGameObject*		pTrailD = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("MarvelReserveTaril"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &_tInfo, &pTrailD));
	if (nullptr == pTrailD)
		return E_FAIL;
	m_pMarvelTrail.push_back((CMarvelReserveTaril*)pTrailD);

	CGameObject*		pTrailE = nullptr;
	(GI->Add_Get_GameObjectToLayer(TEXT("MarvelReserveTaril"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &_tInfo, &pTrailE));
	if (nullptr == pTrailE)
		return E_FAIL;
	m_pMarvelTrail.push_back((CMarvelReserveTaril*)pTrailE);

	return S_OK;
}

void CBeast::SetUp_Breath()
{
	//고정형이냐 아니냐의 차이도 필요 고정형이면 딱한번 위치 받아와서 그 상태만 계속 넣어줘야함

	//DIST 
	_matrix Breathmatrix = m_pBreathNode->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
	_vector vTargetPos;

	if (!m_bChasePlayer)
		vTargetPos = XMLoadFloat4(&m_fPlayerStartPos);
	else
		vTargetPos = XMLoadFloat4(&m_fPlayerPos);

	_vector vPosition = Breathmatrix.r[3];

	_vector vTargetLook = vTargetPos - vPosition;
	_float fDist = XMVectorGetX(XMVector3Length(vTargetLook));
	m_fBreathDistance = fDist;

	_matrix BreathRotmatrix;

	//따라오거나 안따라올때 둘다
	if (m_fBreathDistance <= 10.f) //거리에 따라 브레스 회전
	{
		BreathRotmatrix.r[0] = { 0.866f, 0.5f, 0.f, 0.f };
		BreathRotmatrix.r[1] = { 0.5f, -0.866f, 0.f, 0.f };
		BreathRotmatrix.r[2] = { 0.f, 0.f, -1.f, 0.f };
		BreathRotmatrix.r[3] = { 0.f, 0.f, 0.f, 1.f };

	}
	if (m_fBreathDistance >10.f && m_fBreathDistance <= 12.f)
	{
		BreathRotmatrix.r[0] = { 0.882f, 0.469f, 0.f, 0.f };
		BreathRotmatrix.r[1] = { 0.469f, -0.882f, 0.f, 0.f };
		BreathRotmatrix.r[2] = { 0.f, 0.f, -1.f, 0.f };
		BreathRotmatrix.r[3] = { 0.f, 0.f, 0.f, 1.f };

	}
	else if (m_fBreathDistance > 12.f && m_fBreathDistance <= 17.f)
	{
		BreathRotmatrix.r[0] = { 0.920f, 0.390f, 0.f, 0.f };
		BreathRotmatrix.r[1] = { 0.390f, -0.920f, 0.f, 0.f };
		BreathRotmatrix.r[2] = { 0.f, 0.f, -1.f, 0.f };
		BreathRotmatrix.r[3] = { 0.f, 0.f, 0.f, 1.f };

	}
	else if (m_fBreathDistance >17.f && m_fBreathDistance <= 22.f)
	{
		BreathRotmatrix.r[0] = { 0.939f, 0.342f, 0.f, 0.f };
		BreathRotmatrix.r[1] = { 0.342f, -0.939f, 0.f, 0.f };
		BreathRotmatrix.r[2] = { 0.f, 0.f, -1.f, 0.f };
		BreathRotmatrix.r[3] = { 0.f, 0.f, 0.f, 1.f };
	}
	else
	{
		BreathRotmatrix.r[0] = { 0.956f, 0.292f, 0.f, 0.f };
		BreathRotmatrix.r[1] = { 0.292f, -0.956f, 0.f, 0.f };
		BreathRotmatrix.r[2] = { 0.f, 0.f, -1.f, 0.f };
		BreathRotmatrix.r[3] = { 0.f, 0.f, 0.f, 1.f };

	}
	XMStoreFloat4x4(&m_BreathMatrix, BreathRotmatrix * Breathmatrix);

	if (m_bOneMatrix) //처음 받아온 매트릭스만
	{
		m_bOneMatrix = false;
		XMStoreFloat4x4(&m_BreathFixMatrix, BreathRotmatrix * Breathmatrix);
	}

	if (m_bChasePlayer)
		m_pBeastBreath->SetUp_State(XMLoadFloat4x4(&m_BreathMatrix));
	else
		m_pBeastBreath->SetUp_State(XMLoadFloat4x4(&m_BreathFixMatrix));

	m_pBeastBreath->Change_Size((m_fBreathDistance / 0.5f));
	m_pBeastBreath->Change_ColRange(_float3{ 1.f, (m_fBreathDistance / 3.8f) * 2.f, 1.f }, _float3{ 0.f, (m_fBreathDistance / 3.8f) * 2.f, 0.f });

}

void CBeast::Find_NearstStoneShieldBody(_float fTimeDelta)
{
	CCreture* pNearstStoneBody = nullptr;
	_float fNeastDis = FLT_MAX;

	for (auto& pStone : m_pStoneBodys)
	{
		if (pStone->Get_RenderOnOff())
		{
			pStone->BodyCol_OnOff(true);
			pStone->Shield_OnOff(false);
			_vector vStonePos = XMLoadFloat4(&((CCreture*)pStone)->Get_WorldPos());
			_float fDis = XMVectorGetX(XMVector3Length(vStonePos - XMLoadFloat4(&m_fPlayerPos)));
			if (fDis < fNeastDis)
			{
				pNearstStoneBody = (CCreture*)pStone;
				fNeastDis = fDis;
			}
		}
	}

	if (pNearstStoneBody != nullptr)
	{
		static_cast<CBeastStoneBody*>(pNearstStoneBody)->Shield_OnOff(true);
	}
}

void CBeast::Find_NearstStoneAttackBody(_float fTimeDelta)
{
	CCreture* pNearstStoneBody = nullptr;
	_float fNeastDis = FLT_MAX;

	for (auto& pStone : m_pStoneBodys)
	{
		pStone->BodyCol_OnOff(false);
		_vector vStonePos = XMLoadFloat4(&((CCreture*)pStone)->Get_WorldPos());
		_float fDis = XMVectorGetX(XMVector3Length(vStonePos - XMLoadFloat4(&m_fPlayerPos)));
		if (fDis < fNeastDis)
		{
			pNearstStoneBody = (CCreture*)pStone;
			fNeastDis = fDis;
		}
	}

	if (pNearstStoneBody != nullptr)
	{
		static_cast<CBeastStoneBody*>(pNearstStoneBody)->BodyCol_OnOff(true);
	}
}

void CBeast::Fix_BreathEffectInfo(BREATHTYPE eType)
{
	switch (eType)
	{
	case Client::CBeast::BREATH_NORMAL:
	{
		CBeastBreath2::FIXINFO Info;
		Info.fBlurPower = 1.f;
		Info.vColor = { 1.f,1.f,1.f,1.f };
		Info.vColor2 = { 0.f,0.7f,1.f,1.f };
		Info.vRatio = { 5.f,2.f };
		Info.vRatioSpeed = { 0.f,0.f };
		Info.vScale = { 30.f,20.f,30.f };
		Info.vUvFixSpeed = { 0.f,4.f };

		CBeastBreath2::FIXINFO Info2;
		Info2.fBlurPower = 1.f;
		Info2.vColor = { 1.f,1.f,1.f,1.f };
		Info2.vColor2 = { 0.f,0.7f,1.f,1.f };
		Info2.vRatio = { 5.f,2.f };
		Info2.vRatioSpeed = { 0.f,0.f };
		Info2.vScale = { 40.f,20.f,40.f };
		Info2.vUvFixSpeed = { 0.f,4.f };

		CBeastBreath2::FIXINFO Info3;
		Info3.fBlurPower = 1.f;
		Info3.vColor = { 1.f,1.f,1.f,1.f };
		Info3.vColor2 = { 0.f,0.7f,1.f,1.f };
		Info3.vRatio = { 5.f,2.f };
		Info3.vRatioSpeed = { 0.f,0.f };
		Info3.vScale = { 20.f,20.f,20.f };
		Info3.vUvFixSpeed = { 0.f,4.f };
		((CBeastBreath2*)m_BeastBreath2[0])->Fix_Info(Info);
		((CBeastBreath2*)m_BeastBreath2[1])->Fix_Info(Info2);
		((CBeastBreath2*)m_BeastBreath2[2])->Fix_Info(Info3);
	}
		break;
	case Client::CBeast::BREATH_RED:
	{
		CBeastBreath2::FIXINFO Info;
		Info.fBlurPower = 1.f;
		Info.vColor = { 0.f,0.f,0.f,1.f };
		Info.vColor2 = { 1.f,0.f,0.f,1.f };
		Info.vRatio = { 5.f,2.f };
		Info.vRatioSpeed = { 0.f,0.f };
		Info.vScale = { 30.f,20.f,30.f };
		Info.vUvFixSpeed = { 0.f,4.f };

		CBeastBreath2::FIXINFO Info2;
		Info2.fBlurPower = 1.f;
		Info2.vColor = { 0.f,0.f,0.f,1.f };
		Info2.vColor2 = { 1.f,0.f,0.f,1.f };
		Info2.vRatio = { 5.f,2.f };
		Info2.vRatioSpeed = { 0.f,0.f };
		Info2.vScale = { 40.f,20.f,40.f };
		Info2.vUvFixSpeed = { 0.f,4.f };

		CBeastBreath2::FIXINFO Info3;
		Info3.fBlurPower = 1.f;
		Info3.vColor = { 0.f,0.f,0.f,1.f };
		Info3.vColor2 = { 1.f,0.f,0.f,1.f };
		Info3.vRatio = { 5.f,2.f };
		Info3.vRatioSpeed = { 0.f,0.f };
		Info3.vScale = { 20.f,20.f,20.f };
		Info3.vUvFixSpeed = { 0.f,4.f };
		((CBeastBreath2*)m_BeastBreath2[0])->Fix_Info(Info);
		((CBeastBreath2*)m_BeastBreath2[1])->Fix_Info(Info2);
		((CBeastBreath2*)m_BeastBreath2[2])->Fix_Info(Info3);
	}
		break;
	case Client::CBeast::BREATH_WHITE:
	{
		CBeastBreath2::FIXINFO Info;
		Info.fBlurPower = 5.f;
		Info.vColor = { 0.f,0.f,0.f,1.f };
		Info.vColor2 = { 1.f,1.f,1.f,1.f };
		Info.vRatio = { 1.5f,5.f };
		Info.vRatioSpeed = { 0.f,0.f };
		Info.vScale = { 30.f,20.f,30.f };
		Info.vUvFixSpeed = { 0.f,4.f };

		CBeastBreath2::FIXINFO Info2;
		Info2.fBlurPower = 5.f;
		Info2.vColor = { 0.f,0.f,0.f,1.f };
		Info2.vColor2 = { 1.f,1.f,1.f,1.f };
		Info2.vRatio = { 1.5f,5.f };
		Info2.vRatioSpeed = { 0.f,0.f };
		Info2.vScale = { 40.f,20.f,40.f };
		Info2.vUvFixSpeed = { 0.f,4.f };

		CBeastBreath2::FIXINFO Info3;
		Info3.fBlurPower = 1.f;
		Info3.vColor = { 0.f,0.f,0.f,1.f };
		Info3.vColor2 = { 1.f,1.f,1.f,1.f };
		Info3.vRatio = { 1.5f,5.f };
		Info3.vRatioSpeed = { 0.f,0.f };
		Info3.vScale = { 20.f,20.f,20.f };
		Info3.vUvFixSpeed = { 0.f,4.f };
		((CBeastBreath2*)m_BeastBreath2[0])->Fix_Info(Info);
		((CBeastBreath2*)m_BeastBreath2[1])->Fix_Info(Info2);
		((CBeastBreath2*)m_BeastBreath2[2])->Fix_Info(Info3);
	}
		break;
	
	}

}

void CBeast::Update_Breath2Matrix(_float fTimeDelta)
{
	_matrix Breathmatrix = m_pBreathNode->Get_CombinedTransformation() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();	
	_matrix RotationX = XMMatrixRotationAxis(Breathmatrix.r[2], XMConvertToRadians(180.f));
	Breathmatrix = RotationX * Breathmatrix;
	_matrix RotationY = XMMatrixRotationAxis(Breathmatrix.r[1], XMConvertToRadians(20.f));
	Breathmatrix = RotationY * Breathmatrix;
	Breathmatrix.r[3] += XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 1.5f;
	Breathmatrix.r[3].m128_f32[1] -= 0.5f;
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, Breathmatrix);
	
	for (auto& iter : m_BeastBreath2)
	{
		((CBeastBreath2*)iter)->Update_Breath2Matrix(WorldMatrix);
		iter->Tick(fTimeDelta);
	}
}

void CBeast::Create_BreathChargeEffect(BREATHTYPE eType, _float fTimeDelta)
{
	m_fBreathParticleAcc += 1.f * fTimeDelta;
	if (m_fBreathParticleAcc >= 0.4f)
	{
		switch (m_eBreathType)
		{
		case Client::CBeast::BREATH_NORMAL:
		{
			_matrix Breathmatrix = m_pBreathNode->Get_CombinedTransformation() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
			Breathmatrix.r[0] = { 1.f,0.f,0.f,0.f };
			Breathmatrix.r[1] = { 0.f,1.f,0.f,0.f };
			Breathmatrix.r[2] = { 0.f,0.f,1.f,0.f };
			Breathmatrix.r[3] += XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 4.f;
			_float4x4 WorldMatrix;
			XMStoreFloat4x4(&WorldMatrix, Breathmatrix);
			PTM->Create_InstanceParticle(L"BreathChargeNormal", WorldMatrix);
			PTM->Create_InstanceParticle(L"BreathChargeNormal_2", WorldMatrix);
			PTM->Create_InstanceParticle(L"BreathChargeNormal_4", WorldMatrix);
		}
		break;
		case Client::CBeast::BREATH_RED:
		{

			_matrix Breathmatrix = m_pBreathNode->Get_CombinedTransformation() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
			Breathmatrix.r[0] = { 1.f,0.f,0.f,0.f };
			Breathmatrix.r[1] = { 0.f,1.f,0.f,0.f };
			Breathmatrix.r[2] = { 0.f,0.f,1.f,0.f };
			Breathmatrix.r[3] += XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 4.f;
			_float4x4 WorldMatrix;
			XMStoreFloat4x4(&WorldMatrix, Breathmatrix);
			PTM->Create_InstanceParticle(L"BreathChargeRed", WorldMatrix);
			PTM->Create_InstanceParticle(L"BreathChargeNormal_2", WorldMatrix);
			PTM->Create_InstanceParticle(L"BreathChargeRed_2", WorldMatrix);

		}
		break;
		case Client::CBeast::BREATH_WHITE:
			_matrix Breathmatrix = m_pBreathNode->Get_CombinedTransformation() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
			Breathmatrix.r[0] = { 1.f,0.f,0.f,0.f };
			Breathmatrix.r[1] = { 0.f,1.f,0.f,0.f };
			Breathmatrix.r[2] = { 0.f,0.f,1.f,0.f };
			Breathmatrix.r[3] += XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 4.f;
			_float4x4 WorldMatrix;
			XMStoreFloat4x4(&WorldMatrix, Breathmatrix);
			PTM->Create_InstanceParticle(L"BreathChargeWhite", WorldMatrix);
			PTM->Create_InstanceParticle(L"BreathChargeNormal_2", WorldMatrix);
			PTM->Create_InstanceParticle(L"BreathChargeWhite_2", WorldMatrix);
			break;

		}
		m_fBreathParticleAcc = 0.f;
		GI->StopSound(SD_BEAST3);
		GI->PlaySoundW(L"EFF_CHR_Spell_Darkness_02_B.ogg", SD_BEAST3, 0.6f);
	}
	
}

void CBeast::Create_BreathEffect(BREATHTYPE eType, _float fTimeDelta)
{

	m_fBreathNormalAcc += 1.f * fTimeDelta;
	if (m_fBreathNormalAcc >= 0.2f)
	{
		switch (eType)
		{
		case Client::CBeast::BREATH_NORMAL:
		{
			CRM->Get_Cam()->Shake(0.15f, 0.2f, false, 0.1f);
			m_fBreathNormalAcc = 0.f;
			_matrix Breathmatrix = m_pBreathNode->Get_CombinedTransformation() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
			Breathmatrix.r[0] = { 1.f,0.f,0.f,0.f };
			Breathmatrix.r[1] = { 0.f,1.f,0.f,0.f };
			Breathmatrix.r[2] = { 0.f,0.f,1.f,0.f };
			Breathmatrix.r[3] += XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 2.f;
			Breathmatrix.r[3].m128_f32[1] -= 0.5f;
			_float4x4 WorldMatrix;
			XMStoreFloat4x4(&WorldMatrix, Breathmatrix);
			PTM->Create_InstanceParticle(L"BreathNormal", WorldMatrix);
			PTM->Create_InstanceParticle(L"BreathNormal_2", WorldMatrix);
		}
			break;
		case Client::CBeast::BREATH_RED:
		{
			CRM->Get_Cam()->Shake(0.15f, 0.2f, false, 0.1f);
			m_fBreathNormalAcc = 0.f;
			_matrix Breathmatrix = m_pBreathNode->Get_CombinedTransformation() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
			Breathmatrix.r[0] = { 1.f,0.f,0.f,0.f };
			Breathmatrix.r[1] = { 0.f,1.f,0.f,0.f };
			Breathmatrix.r[2] = { 0.f,0.f,1.f,0.f };
			Breathmatrix.r[3] += XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 2.f;
			Breathmatrix.r[3].m128_f32[1] -= 0.5f;
			_float4x4 WorldMatrix;
			XMStoreFloat4x4(&WorldMatrix, Breathmatrix);
			PTM->Create_InstanceParticle(L"BreathNormal", WorldMatrix);
			PTM->Create_InstanceParticle(L"BreathRed", WorldMatrix);
		}
			break;
		case Client::CBeast::BREATH_WHITE:
		{
			CRM->Get_Cam()->Shake(0.15f, 0.2f, false, 0.1f);
			m_fBreathNormalAcc = 0.f;
			_matrix Breathmatrix = m_pBreathNode->Get_CombinedTransformation() * m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
			Breathmatrix.r[0] = { 1.f,0.f,0.f,0.f };
			Breathmatrix.r[1] = { 0.f,1.f,0.f,0.f };
			Breathmatrix.r[2] = { 0.f,0.f,1.f,0.f };
			Breathmatrix.r[3] += XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * 2.f;
			Breathmatrix.r[3].m128_f32[1] -= 0.5f;
			_float4x4 WorldMatrix;
			XMStoreFloat4x4(&WorldMatrix, Breathmatrix);
			PTM->Create_InstanceParticle(L"BreathNormal", WorldMatrix);
			PTM->Create_InstanceParticle(L"BreathWhite", WorldMatrix);
		}
			break;
	
		}
		
	}
}

void CBeast::Set_PushNewPos()
{

	int iRandom = GI->Get_Random(1,4);

	switch (iRandom)
	{
	case 1:
		((CBeastMarvelPush*)m_pMarvelPush[0])->Set_NewPos(_float4{ 0.451f, 0.02f, -60.f, 1.f });
		((CBeastMarvelPush*)m_pMarvelPush[0])->Set_On();
		((CBeastMarvelPush*)m_pMarvelPush[1])->Set_NewPos(_float4{ -8.f, 0.02f, -53.f, 1.f });
		((CBeastMarvelPush*)m_pMarvelPush[1])->Set_On();
		((CBeastMarvelPush*)m_pMarvelPush[2])->Set_NewPos(_float4{ 0.451f, 0.02f, -40.f, 1.f });
		((CBeastMarvelPush*)m_pMarvelPush[2])->Set_On();
		((CBeastMarvelPush*)m_pMarvelPush[3])->Set_NewPos(_float4{ 8.f, 0.02f, -53.f, 1.f });
		((CBeastMarvelPush*)m_pMarvelPush[3])->Set_On();
		break;
	case 2:
		((CBeastMarvelPush*)m_pMarvelPush[0])->Set_NewPos(_float4{ 8.f, 0.02f, -53.f, 1.f });
		((CBeastMarvelPush*)m_pMarvelPush[0])->Set_On();
		((CBeastMarvelPush*)m_pMarvelPush[1])->Set_NewPos(_float4{ -8.f, 0.02f, -53.f, 1.f });
		((CBeastMarvelPush*)m_pMarvelPush[1])->Set_On();
		((CBeastMarvelPush*)m_pMarvelPush[2])->Set_NewPos(_float4{ 0.451f, 0.02f, -40.f, 1.f });
		((CBeastMarvelPush*)m_pMarvelPush[2])->Set_On();
		((CBeastMarvelPush*)m_pMarvelPush[3])->Set_NewPos(_float4{ 0.451f, 0.02f, -60.f, 1.f });
		((CBeastMarvelPush*)m_pMarvelPush[3])->Set_On();
		break;
	case 3:
		((CBeastMarvelPush*)m_pMarvelPush[0])->Set_NewPos(_float4{ -8.f, 0.02f, -53.f, 1.f });
		((CBeastMarvelPush*)m_pMarvelPush[0])->Set_On();
		((CBeastMarvelPush*)m_pMarvelPush[1])->Set_NewPos(_float4{ 0.451f, 0.02f, -60.f, 1.f });
		((CBeastMarvelPush*)m_pMarvelPush[1])->Set_On();
		((CBeastMarvelPush*)m_pMarvelPush[2])->Set_NewPos(_float4{ 8.f, 0.02f, -53.f, 1.f });
		((CBeastMarvelPush*)m_pMarvelPush[2])->Set_On();
		((CBeastMarvelPush*)m_pMarvelPush[3])->Set_NewPos(_float4{ 0.451f, 0.02f, -40.f, 1.f });
		((CBeastMarvelPush*)m_pMarvelPush[3])->Set_On();
		break;
	case 4:
		((CBeastMarvelPush*)m_pMarvelPush[0])->Set_NewPos(_float4{ 0.451f, 0.02f, -60.f, 1.f });
		((CBeastMarvelPush*)m_pMarvelPush[0])->Set_On();
		((CBeastMarvelPush*)m_pMarvelPush[1])->Set_NewPos(_float4{ 0.451f, 0.02f, -40.f, 1.f });
		((CBeastMarvelPush*)m_pMarvelPush[1])->Set_On();
		((CBeastMarvelPush*)m_pMarvelPush[2])->Set_NewPos(_float4{ -8.f, 0.02f, -53.f, 1.f });
		((CBeastMarvelPush*)m_pMarvelPush[2])->Set_On();
		((CBeastMarvelPush*)m_pMarvelPush[3])->Set_NewPos(_float4{ 8.f, 0.02f, -53.f, 1.f });
		((CBeastMarvelPush*)m_pMarvelPush[3])->Set_On();
		break;
	}

}

void CBeast::Create_Attack5OnEffect()
{
	switch (m_iHowlingAttack)
	{
	case 1:
		PTM->Create_InstanceParticle(L"BeastMagic", m_pTransformCom->Get_WorldFloat4x4());
		break;
	case 2:
		PTM->Create_InstanceParticle(L"BeastMagic_2", m_pTransformCom->Get_WorldFloat4x4());
		break;
	case 3:
		PTM->Create_InstanceParticle(L"BeastMagic_3", m_pTransformCom->Get_WorldFloat4x4());
		break;
	case 4:
		PTM->Create_InstanceParticle(L"BeastMagic_4", m_pTransformCom->Get_WorldFloat4x4());
		break;
	case 5:
		PTM->Create_InstanceParticle(L"BeastMagic_5", m_pTransformCom->Get_WorldFloat4x4());
		break;
	case 6:
		PTM->Create_InstanceParticle(L"BeastMagic_6", m_pTransformCom->Get_WorldFloat4x4());
		break;
	}
	PTM->Create_InstanceParticle(L"BeastDT", m_pTransformCom->Get_WorldFloat4x4());
}

HRESULT CBeast::Ready_Layer_UI(const _tchar * pLayerTag)
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
	UiInfoDesc.fY = 80.23f;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Boss_HP_Bar"), TM->Get_CurLevel(), pLayerTag, &UiInfoDesc)))
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

void CBeast::Set_PreBar(_float fTimeDelta)
{
	if (m_bReGauge)
	{
		m_fPatternGauge += 800.f * fTimeDelta;

		if (m_fPatternGauge >= 2000.f)
		{
			m_fPatternGauge = 2000.f;
			m_fPrePatternGauge = m_fPatternGauge;
			m_bReGauge = false;
		}
	}

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

CBeast * CBeast::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBeast*		pInstance = new CBeast(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBeast"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBeast::Clone(void * pArg)
{
	CBeast*		pInstance = new CBeast(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBeast"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBeast::Free()
{
	__super::Free();

	for (auto& iter : m_pStoneBodys)
		Safe_Release(iter);
	m_pStoneBodys.clear();

	for (auto& iter : m_pMarvelObstacle)
		Safe_Release(iter);
	m_pMarvelObstacle.clear();

	for (auto& iter : m_pMarvelPush)
		Safe_Release(iter);
	m_pMarvelPush.clear();

	for (auto& iter : m_pMarvelTrail)
		Safe_Release(iter);
	m_pMarvelTrail.clear();

	for (auto& iter : m_BeastBreath2)
		Safe_Release(iter);
	m_BeastBreath2.clear();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pDissolveTexture);
	Safe_Release(m_pEmissiveCom);
}
