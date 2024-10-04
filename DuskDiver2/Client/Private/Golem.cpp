#include "stdafx.h"
#include "..\Public\Golem.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "ToolManager.h"
#include "Monster_Manager.h"
#include "Time_Manager.h"
#include "Camera_Manager.h"
#include "UI.h"
#include "Rock.h"
#include "SwordTrail.h"
#include "SwordTrailObj.h"
#include "PointTrail.h"
#include "PointTrailObj.h"
#include "DropItem.h"
#include "GolemWeaponRange.h"
#include "GolemAttackRange.h"
#include "Particle_Manager.h"
#include "Player01HitEffect_1.h"
#include "Player01HitEffect_2.h"
#include "Player01Wind1.h"
#include "Player01Wind2.h"
#include "Player01SmokeBall.h"
#include "Player01Pillar.h"
#include "Player01Pillar2.h"
#include "Player01Pillar3.h"

CGolem::CGolem(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CGolem::CGolem(const CGolem & rhs)
	: CCreture(rhs)
{
}

HRESULT CGolem::Initialize_Prototype()
{
	
	return S_OK;
}

HRESULT CGolem::Initialize(void * pArg)
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

	m_eCurState = SPAWN1;
	m_pModelCom->Set_AnimIndex(SPAWN1);


	m_fPlayerPos = { 0.f,0.f,0.f, 1.f }; //일단 초기화
	m_vPlayerLook = { 0.f,0.f,1.f };
	
	m_fHurtTime = 0.02f;


	//트레일 생성 코드
	Ready_Socket();
	Ready_Trails();

	m_fMaxHp = 8000.f;
	m_fNowHp = m_fMaxHp;
	m_fStamina = 3.f;
	m_fDamage = 10.f;

	m_fMaxPg = 2000.f;
	m_fPatternGauge = m_fMaxPg;

	m_fPreHp = m_fNowHp;
	m_fPrePatternGauge = m_fPatternGauge;


	/*_vector vTemp = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vTemp = XMVectorSetY(vTemp, vTemp.m128_f32[1] + 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vTemp);
*/
	_vector vTemp = { 0.f, 1.6f, 0.f, 0.f };
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + vTemp);

	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(10.f));
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Dissolve"), TEXT("Com_Texture"), (CComponent**)&m_pDissolveTexture)))
		return E_FAIL;



	MM->Add_Boss(this);
	MM->Add_Golem(this);

	UM->Set_BossNum(1);

	if (FAILED(Ready_Layer_UI(TEXT("Layer_UI"))))
		return E_FAIL;




	CCamera_Main::CUTDESC CamDesc;
	CamDesc.vPos = _float3(13.194f, 8.83f, 149.767f);
	CamDesc.vLook = _float3(-1.671f, 1.811f, 147.f);
	CamDesc.bLerp = false;
	CamDesc.bLookLerp = false;
	CamDesc.fLerpSpeed = 0.f;
	CamDesc.fLookLerpSpeed = 0.f;
	CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
	CamDesc.pTran = nullptr;
	CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
	CamDesc.bPixLocal = false;
	CRM->Get_Cam()->Show_Target(CamDesc);
	UM->Set_FrontUI(false);


	return S_OK;
}

void CGolem::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_MONSTER, this);
	Check_Hurt(fTimeDelta);
	Check_Rim(fTimeDelta);
	fTimeDelta *= m_fTMRatio;
	Emp_Col();
	Distance_BattlePattern(fTimeDelta);
	Update(fTimeDelta);
	Check_Death(fTimeDelta);
	Check_MonsterLight(fTimeDelta);
	Set_PreBar(fTimeDelta);
	Create_PointTrail(fTimeDelta);

	if (CK_DOWN(DIK_DELETE))
	{
		m_bSpawn2Shout = false;
		m_bAttack = true;
		/*m_eCurState = SPAWN2;
		m_eNextState = SPAWN2;
		m_pModelCom->SetNextIndex(m_eNextState);
		m_pModelCom->Set_AnimIndex(m_eNextState);*/
		Set_State(SPAWN2);
	}
}

void CGolem::LateTick(_float fTimeDelta)
{
	if (m_bGara)
	{
		if (nullptr == m_pRendererCom)
			return;

		fTimeDelta *= m_fTMRatio;

	if (m_eCurState == SPAWN1 || m_eCurState == SPAWN2 || m_eCurState == IDLE && !m_bMove)
	{
		m_pModelCom->Play_Animation_NoMove(fTimeDelta);

	}
	else
	{
		m_pModelCom->Set_RootHierarchy(3);
		m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom, m_pNavigationCom);
		m_pTransformCom->Tick_Gravity(fTimeDelta, m_pNavigationCom, 30.f);
	}
	

		End_Animation();

		KeyInput_Attack();

		Tick_Col(m_pTransformCom->Get_WorldMatrix());

		if (m_bWeaponCol)
			Choose_Col("Monster_WeaponHead");

		if (m_bAttackCol)//공격
			Choose_Col("Monster_Attack");

		if (m_bHurt)
			Choose_Col("Monster_Body1");

		if (m_bRangeCol)
			Choose_Col("Monster_Push_Range");

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

		for (auto& iter : m_PointTrails)
			iter->LateTick(fTimeDelta);

		Render_Col(m_pRendererCom);
	}
}

HRESULT CGolem::Render()
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


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pDissolveTexture->Set_SRV(m_pShaderCom, "g_DissolveTexture", 0)))
			return E_FAIL;

		m_pShaderCom->Set_RawValue("g_fDissolveAcc", &m_fDissolveAcc, sizeof(float));

		// For Emi
		if (FAILED(m_pEmissiveCom[i]->Set_SRV(m_pShaderCom, "g_EmissiveTexture", 0)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, m_iShaderPath)))
			return E_FAIL;
	}
	return S_OK;
}

// *[HeroJho]: 디버깅 창입니다. ImGui로 작동됩니다.
void CGolem::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);

	if (m_bCheckFront) //디버깅용
	{
		m_fTest = 1.f;
	}
	else
		m_fTest = -1.f;

	ImGui::DragFloat("RimPower ", &m_fRimPower, 0.01f);
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

	_float3 vSize = Get_Colliders("Monster_WeaponHead")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Monster_WeaponHead")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Get_Colliders("Monster_WeaponHead")->Set_Size(vSize);
	Get_Colliders("Monster_WeaponHead")->Set_Center(vCenter);
	ImGui::DragFloat4("Power, Time, LerpTime", (float*)&ShakeTest, 0.01f, -10.f, 10.f, "%.2f");

	ImGui::DragFloat4("Power, Time, StopTime", (float*)&FowTest, 0.01f, -10.f, 10.f, "%.2f");

	ImGui::End();
}

// *[HeroJho]: 충돌이 발생할 때 호출되는 함수입니다.
void CGolem::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
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
			m_bWalkBack = true;

			if (m_eCurState == ATTACK1 || m_eCurState == ATTACK4 || m_eCurState == ATTACK7)
				m_bColDelay = true;

			//어택 타입
			if(m_bPlayerHit)
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

void CGolem::On_Hit(CCreture* pOther)
{
	//hit이 한번만 처리되게끔
	if (m_bHurt && m_bHitAble)
	{
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		vPos.y += 2.f;
		pOther->Create_HitEffect(vPos);

		GI->PlaySoundW(L"SwordHit.ogg", SD_HIT, 0.5f);
		m_bRim = true;
		m_bHurt = false;
		m_bWeaponCol = false;
		m_bAttackCol = false;

		m_fNowHp -= (pOther->Get_Damage() - m_fStamina);

		if (m_bDamageAble && !m_bReGauge)
		{
			m_fPatternGauge -= (pOther->Get_Damage() * 1.2f);
			Check_Stun();
		}

		if (m_fNowHp <= 0.f)
		{
			m_bHurt = false;
			m_bSpawn = false;
			TIM->Fix_All(0.14f, 5.f);
			CRM->Get_Cam()->Fov(-3.f, 0.15f, 3.f);


			if (!m_bFightEnd)
			{
				GI->PlaySoundW(L"slowmotion_finish.wav", SD_UI, 0.6f);
				m_bFightEnd = true;
			}

		}

		if(m_bHitMove)
			Hit_State(pOther);

	
		m_bSameAttack = true;



		UM->Plus_Combo();
		UM->Set_Hit(true);
		UM->Set_Up(true);

	}

}

void CGolem::Create_HitEffect(_float4 vWorldPos)
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

void CGolem::Control_Golem()
{
	m_bMove = true;
	m_bSpawn = true;
	m_bAttack = false;
	m_bOneCheck = false;
	m_bHitAble = true;
	m_bAction = true;
	UM->GolemName_Off();

	_vector vTemp = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vTemp = XMVectorSetY(vTemp, 0.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vTemp);
}

HRESULT CGolem::Set_RenderPass(SHADER eShader)
{
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	_float fGama = 1.6f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;

	switch (eShader)
	{
	case Client::CTrainingRobot::SHADER_DEFAULT:
	{
		// For.Blur
		if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &m_fBlurPower, sizeof(_float))))
			return E_FAIL;

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			//_uint iPassNum = 13; // 4

			if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;

			// For Emi
			if (FAILED(m_pEmissiveCom[i]->Set_SRV(m_pShaderCom, "g_EmissiveTexture", 0)))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 12)))
				return E_FAIL;
		}
	}
		break;
	case Client::CTrainingRobot::SHADER_RIM:
	{
		// For.Blur
		if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &m_fBlurPower, sizeof(_float))))
			return E_FAIL;

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			//// For.Rim
			if (FAILED(m_pShaderCom->Set_RawValue("g_CamPosition", &GI->Get_CamPosition(), sizeof(_float4))))
				return E_FAIL;

			//_float g_RimPower = 3.f;

			_float g_RimPower = m_fRimPower;
			if (FAILED(m_pShaderCom->Set_RawValue("g_RimPower", &g_RimPower, sizeof(_float))))
				return E_FAIL;;


			if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;


			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 1)))
				return E_FAIL;
		}
	}

	break;
	case Client::CTrainingRobot::SHADER_LIGHT:
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			//// For.Rim
			if (FAILED(m_pShaderCom->Set_RawValue("g_CamPosition", &GI->Get_CamPosition(), sizeof(_float4))))
				return E_FAIL;

			//_float g_RimPower = 3.f;

			_float g_RimPower = m_fRimPower;
			if (FAILED(m_pShaderCom->Set_RawValue("g_RimPower", &g_RimPower, sizeof(_float))))
				return E_FAIL;;


			if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;


			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 1)))
				return E_FAIL;
		}
	}
	break;
	}


	return S_OK;
}

void CGolem::KeyInput_Attack()
{
	if (CK_DOWN(DIK_NUMPAD1))
	{
		m_bAttack = true;
		Set_State(ATTACK1); //망치 두번 휘두르기
	} 
	if (CK_DOWN(DIK_NUMPAD2))
	{
		m_bAttack = true;
		Set_State(ATTACK2); //바닥에 한번 내리침
	}
	if (CK_DOWN(DIK_NUMPAD3))
	{
		m_bAttack = true;
		Set_State(ATTACK3); //망치 거꾸로 잡고 바닥에 찍기
	}

	if (CK_DOWN(DIK_NUMPAD4))
	{
		m_bAttack = true;
		Set_State(ATTACK4); //바닥 두번치고 한번 스윙
	}
	
	if (CK_DOWN(DIK_NUMPAD5))
	{
		m_bAttack = true;
		Set_State(ATTACK5); //위로 높게 점프한뒤에 바닥찍기
	}
	if (CK_DOWN(DIK_NUMPAD6))
	{
		m_bAttack = true;
		Set_State(ATTACK6_1); //앞으로 누움 - 거리에 따라 달라짐
	}

	if (CK_DOWN(DIK_NUMPAD7)) 
	{
		m_bAttack = true;
		Set_State(ATTACK6_2); //뒤로 눕기
	}
	if (CK_DOWN(DIK_NUMPAD8)) 
	{
		m_bAttack = true;
		Set_State(ATTACK7); //스윙 계속
	}
}

void CGolem::Set_State(STATE eState) //end 애니메이션에서
{
	//상태 보간 하는 함수
	if (m_eNextState == eState)
		return;

	m_eNextState = eState;

	switch (m_eNextState)
	{
		//다음 상태로 넘어갈때 해줘야하는것들
		//애니메이션x
	case Client::CGolem::ATTACK1:
		m_bEyeTrail = true;
		m_bWalkBack = false;
		m_bColDelay = false;
		m_bPlayerHit = true;
		m_fColDelayTime = 0.f;
		m_fAttack1EffectAcc = 0.05f;
		break;
	case Client::CGolem::ATTACK2:
		m_bEyeTrail = true;
		m_eHitType = TYPE_HIT;
		m_fPushPower = 20.f;
		m_fAirPower = 0.f;
		m_fDamage = 170.f;
		m_fParticle.x = 0.f;
		m_fParticle.y = 0.72f;
		m_fParticle.z = 7.f;
		m_fParticle.w = 1.f;
		m_bWalkBack = false;
		m_bAttack2Col_0 = false;
		m_fAttack1EffectAcc = 0.05f;
		break;
	case Client::CGolem::ATTACK3:
		m_bEyeTrail = true;
		m_eHitType = TYPE_AWAY;
		m_fPushPower = 3.f;
		m_fAirPower = 3.f;
		m_fDamage = 190.f;
		m_fParticle.x = 0.f;
		m_fParticle.y = 1.f;
		m_fParticle.z = 4.f;
		m_fParticle.w = 1.f;
		m_bAttack3Col_0 = false;
		m_bWalkBack = false;
		Change_WeaponColRange(_float3{ 2.8f,3.3f,1.7f }, _float3{ -0.1f, -0.2f, -4.6f });
		break;
	case Client::CGolem::ATTACK4:
		m_eHitType = TYPE_HIT;
		m_fPushPower = 10.f;
		m_fAirPower = 0.f;
		m_fDamage = 155.f;
		m_fParticle.x = 1.f;
		m_fParticle.y = 0.72f;
		m_fParticle.z = 10.f;
		m_fParticle.w = 1.f;
		m_bEyeTrail = true;
		m_bAttack4Col_0 = false;
		m_bAttack4Col_1 = false;
		m_bAttack4Col_2 = false;
		m_bAttack4Col_3 = false;
		m_fAttack1EffectAcc = 0.05f;
		break;
	case Client::CGolem::ATTACK5:
		m_bEyeTrail = true;
		m_bAttack5Effect = false;
		m_eHitType = TYPE_AWAY;
		m_fPushPower = 3.f;
		m_fAirPower = 2.f;
		m_fDamage = 185.f;
		Change_WeaponColRange(_float3{ 2.8f,3.3f,1.7f }, _float3{ -0.1f, -0.2f, -4.6f });
		break;
	case Client::CGolem::ATTACK6_1:
		if (!m_bClose)
		{
			Create_Effect(ATTAK6_1EFFECT);
			//Create_Effect(ATTAK6_1EFFECT2);
		}
		m_bEyeTrail = true;
		m_bAttack6_1Effect = false;
		m_eHitType = TYPE_HIT;
		m_fPushPower = 10.f;
		m_fAirPower = 0.f;
		m_fDamage = 165.f;
		m_bAttack6_1Col_0 = false;
		Change_AttackColRange(_float3{ 4.8f,4.4f,4.4f }, _float3{ 0.f, 1.f, 1.9f });
		m_bAttack6Sound = false;
		break;
	case Client::CGolem::ATTACK6_2:
		m_bEyeTrail = true;
		m_eHitType = TYPE_HIT;
		m_fPushPower = 10.f;
		m_fAirPower = 0.f;
		m_fDamage = 155.f;
		m_bAttack6_2Col_0 = false;
		Change_AttackColRange(_float3{ 5.2f,4.9f,4.9f }, _float3{ 0.f, 0.6f, -1.8f });
		m_bAttack6Sound_2 = false;
		break;
	case Client::CGolem::ATTACK7:
		m_bEyeTrail = true;
		m_eHitType = TYPE_HIT;
		m_fPushPower = 0.f;
		m_fAirPower = 0.f;
		m_fDamage = 175.f;
		m_bColDelay = false;
		m_bPlayerHit = true;
		m_bAttack7Effect = false;
		m_bAttack7Effect_2 = false;
		m_bAttack7Effect_3 = false;
		m_fColDelayTime = 0.f;
		break;
	case Client::CGolem::HITBACK:
		Reset_Collider();
		m_bFly = false;
		m_bEyeTrail = false;
		break;
	case Client::CGolem::DOWN:
		m_bFly = true;
		m_bEyeTrail = false;
		break;
	case Client::CGolem::GETUP:
		m_bFly = false;
		m_bEyeTrail = false;
		break;
	case Client::CGolem::RECOVER:
		m_bEyeTrail = false;
		break;
	case Client::CGolem::SPAWN1:
		m_bEyeTrail = false;
		break;
	case Client::CGolem::SPAWN2:
		m_bEyeTrail = true;
		break;
	case Client::CGolem::IDLE:
		if (m_bMove)
		{
			m_bHurt = true;
			m_bAction = true;
			m_bSpawn = true;
			_vector vTemp = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

			vTemp = XMVectorSetY(vTemp, 0.f);
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, vTemp);
		}
		else
			LookAtPlayer();
		m_bEyeTrail = false;
		break;
	case Client::CGolem::STUN:
		GI->PlaySoundW(L"VO_MON_Golem_Growl_01_G.ogg", SD_GOLEM, 1.f);
		m_bEyeTrail = false;
		m_PointTrails[PTR_WEAPON]->TrailOff();
		break;
	case Client::CGolem::STUNRECOVER:
		m_bReGauge = true;
		m_bEyeTrail = true;
		break;
	case Client::CGolem::STUNDEATH:
		m_bEyeTrail = false;
		m_PointTrails[PTR_WEAPON]->TrailOff();
		break;
	case Client::CGolem::STRAFELEFT:
		m_bEyeTrail = false;
		break;
	case Client::CGolem::STRAFERIGHT:
		m_bEyeTrail = false;
		break;
	case Client::CGolem::RUN:
		m_bEyeTrail = false;
		m_bAttack = false;
		break;
	case Client::CGolem::WALKBACK:
		m_bEyeTrail = false;
		break;
	case Client::CGolem::WALKFRONT:
		m_bEyeTrail = false;
		break;
	case Client::CGolem::TURNLEFT:
		break;
	case Client::CGolem::TURNRIGHT:
		break;
	}


	m_pModelCom->SetNextIndex(m_eNextState);
	m_pModelCom->SetChangeBool(true); 
	m_eCurState = m_eNextState; //보간시작할때 상태를 바꿔줌

}

void CGolem::Hit_State(CCreture * pOther)
{
	m_fHAirPower = pOther->Get_AirPower();
	m_fHPushPower = pOther->Get_PushPower();

	if (!m_pTransformCom->Check_TargetFront(XMLoadFloat4(&pOther->Get_WorldPos())))
	{
		Set_State(HITBACK);

	}
}

void CGolem::End_Animation()
{
	if (m_pModelCom->GetAniEnd())
	{
		//애니메이션 끝났을때
		switch (m_eCurState)
		{
			//다음 상태로 넘어갈때 해줘야하는것들
			//애니메이션x

		case Client::CGolem::SPAWN1:
		{
			m_bSpawn1Walk = false;
			Set_State(SPAWN2);
			//CRM->Get_Cam()->Show_Target(_float3(0.f, 0.f, 0.f), _float3(0.f, 0.f, 0.f), true, true, 6.f, _float3(0.f, 0.f, 0.f), m_pTransformCom, _float3(10.f, 3.f, 0.f));
			UM->GolemName_On();


			CCamera_Main::CUTDESC CamDesc;
			CamDesc.vPos = _float3(13.194f, 8.83f, 149.767f);
			CamDesc.vLook = _float3(-1.671f, 1.811f, 147.f);
			CamDesc.bLerp = true;
			CamDesc.bLookLerp = true;
			CamDesc.fLerpSpeed = 2.f;
			CamDesc.fLookLerpSpeed = 2.f;
			CamDesc.vPivotLookPos = _float3(0.f, 3.f, 0.f);
			CamDesc.pTran = m_pTransformCom;
			CamDesc.vPosDis = _float3(10.f, 3.f, 0.f);
			CamDesc.bPixLocal = false;
			CRM->Get_Cam()->Show_Target(CamDesc);
		}
			break;
		case Client::CGolem::SPAWN2:
			m_bSpawn2Shout = false;
			Set_State(IDLE);
			CRM->Get_Cam()->End_Target();
			m_bRemove = true;
			UM->Set_FrontUI(true);
			break;

		case Client::CGolem::IDLE:
			if (m_bMove)
			{
				m_fAttackTime = 0.f;
				m_bAttack = false;
				m_bOneCheck = false;
				m_bHitAble = true;
				m_bAction = true;
			}
			break;
		case Client::CGolem::RUN:
			m_bOneCheck = false;
			break;
		case Client::CGolem::WALKBACK:
		{
			m_iRandom = GI->Get_Random(1, 5);
			if (m_iRandom == 4 || m_iRandom == 5)
				m_iRandom = 3;

			switch (m_iRandom)
			{
			case 1:
				Set_State(STATE::STRAFELEFT);
				break;
			case 2:
				Set_State(STATE::STRAFERIGHT);
				break;
			case 3:
				m_fAttackTime = 0.f;
				m_bAttack = false;
				m_bOneCheck = false;
				m_bWalkBack = false;
				break;
			}
		}
		break;
		case Client::CGolem::WALKFRONT:
			m_fAttackTime = 0.f;
			m_bAttack = false;
			m_bOneCheck = false;
			m_bWalkBack = false;
			break;
		case Client::CGolem::STRAFELEFT:
			m_bAttack = false;
			m_bOneCheck = false;
			m_bWalkBack = false;
			break;
		case Client::CGolem::STRAFERIGHT:
			m_bAttack = false;
			m_bOneCheck = false;
			m_bWalkBack = false;
			break;
		case Client::CGolem::ATTACK1:
			//GI->PlaySoundW(L"se_EN0106_explosion2.wav", SD_BULLET, 0.6f); // 바닥에 불장판? 같은 이펙트 터질 때
			//m_bAttack1Fire = false;
			m_fColDelayTime = 0.f;
			m_bPlayerHit = true;
			m_bColDelay = false;
			Set_State(WALKBACK);
			break;
		case Client::CGolem::ATTACK2:
			//GI->PlaySoundW(L"se_EN0106_explosion2.wav", SD_BULLET, 0.6f); // 바닥에 불장판? 같은 이펙트 터질 때
			//m_bAttack2Fire = false;
			Set_State(IDLE);
			break;
		case Client::CGolem::ATTACK3:
			m_bAttack3Off = false;
			Change_WeaponColRange(_float3{ 1.2f, 1.9f, 3.6f }, _float3{ 0.f, -0.2f, -2.4f });
			Set_State(IDLE);
			break;
		case Client::CGolem::ATTACK4:
			m_bAttack4On = false;
			++m_iCloseAttackNum;
			m_fColDelayTime = 0.f;
			m_bPlayerHit = true;
			m_bColDelay = false;
			Change_WeaponColRange(_float3{ 1.2f, 1.9f, 3.6f }, _float3{ 0.f, -0.2f, -2.4f });
			Set_State(WALKBACK);
			break;
		case Client::CGolem::ATTACK5:
			Set_State(IDLE);
			Change_WeaponColRange(_float3{ 1.2f, 1.9f, 3.6f }, _float3{ 0.f, -0.2f, -2.4f });
			break;
		case Client::CGolem::ATTACK6_1:
			m_bAttack6On = false;
			if(m_bClose)
				++m_iCloseAttackNum;
			Set_State(STATE::WALKBACK);
			Change_AttackColRange(_float3{ 3.f, 3.f, 3.f }, _float3{ 0.f, 1.5f, 0.f });
			break;
		case Client::CGolem::ATTACK6_2:
			m_bAttack6On = false;
			++m_iCloseAttackNum;
			Set_State(STATE::WALKBACK);
			Change_AttackColRange(_float3{ 3.f, 3.f, 3.f }, _float3{ 0.f, 1.5f, 0.f });
			break;
		case Client::CGolem::ATTACK7:
			++m_iCloseAttackNum;
			m_fColDelayTime = 0.f;
			m_bPlayerHit = true;
			m_bColDelay = false;
			Set_State(WALKBACK);
			Change_WeaponColRange(_float3{ 1.2f, 1.9f, 3.6f }, _float3{ 0.f, -0.2f, -2.4f });
			break;
		case Client::CGolem::HITBACK:
			if (m_bDeadStart)
			{
				Set_State(STUN);
			}
			else
			{
				if (m_fDistance < 6.f)
					Set_State(ATTACK6_2);
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
		case Client::CGolem::DOWN:
			if (!m_bDeadStart)
				Set_State(GETUP);
			else
				m_bDissolve = true;
			break;
		case Client::CGolem::GETUP:
			m_bAttack = false;
			m_bOneCheck = false;
			m_bAction = true;
			m_bHitAble = true;
			Set_State(IDLE);
			break;
		case Client::CGolem::RECOVER:
			break;
		case Client::CGolem::STUN:
			if (!m_bDeadStart)
			{
				if (m_bStun)
					Set_State(STUN);
				else
					Set_State(STUNRECOVER);
			}
			else
				Set_State(STUNDEATH);
			break;
		case Client::CGolem::STUNRECOVER:
			m_bDamageAble = true;
			m_fRecoverAcc = 0.f;
			m_bAttack = false;
			m_bHitMove = true;
			Set_State(IDLE);
			break;
		case Client::CGolem::STUNDEATH:
			Set_State(DOWN);
			break;
		case Client::CGolem::TURNLEFT:
			break;
		case Client::CGolem::TURNRIGHT:
			break;
		default:
			break;

		}

		

	}
}

void CGolem::Update(_float fTimeDelta)
{
	m_bRangeCol = true;

	if (!m_pModelCom->GetChangeBool())
	{
		switch (m_eCurState)
		{

	
		case Client::CGolem::SPAWN1:
			Spawn1_Update(fTimeDelta);
			break;
		case Client::CGolem::SPAWN2:
			Spawn2_Update(fTimeDelta);
			break;
		case Client::CGolem::IDLE:
			Idle_Update(fTimeDelta);
			break;
		case Client::CGolem::RUN:
			Run_Update(fTimeDelta);
			break;
		case Client::CGolem::WALKBACK: //텀주기 위한 상태, 뒤로 살짝 물러남, 애니메이션 자체가 뒤로 물러나므로 트랜스폼함수 필요없음
			WalkBack_Update(fTimeDelta);
			break;
		case Client::CGolem::WALKFRONT:
			Walk_Update(fTimeDelta);
			break;
		case Client::CGolem::STRAFELEFT:
			Strafe_Update(fTimeDelta);
		case Client::CGolem::STRAFERIGHT:
			Strafe_Update(fTimeDelta);
			break;
		case Client::CGolem::ATTACK1:
			Attack1_Update(fTimeDelta);
			break;
		case Client::CGolem::ATTACK2:
			Attack2_Update(fTimeDelta);
			break;
		case Client::CGolem::ATTACK3:
			Attack3_Update(fTimeDelta);
			break;
		case Client::CGolem::ATTACK4:
			Attack4_Update(fTimeDelta);
			break;
		case Client::CGolem::ATTACK5:
			Attack5_Update(fTimeDelta);
			break;
		case Client::CGolem::ATTACK6_1:
			Attack6_1Update(fTimeDelta);
			break;
		case Client::CGolem::ATTACK6_2:
			Attack6_2Update(fTimeDelta);
			break;
		case Client::CGolem::ATTACK7:
			Attack7_Update(fTimeDelta);
			break;
		case Client::CGolem::HITBACK:
			HitBack_Update(fTimeDelta);
			break;
		case Client::CGolem::DOWN:
			break;
		case Client::CGolem::GETUP:
			break;
		case Client::CGolem::RECOVER:
			break;
		case Client::CGolem::STUN:
			m_bRangeCol = true;
			break;
		case Client::CGolem::STUNRECOVER:
			StunRecover_Update(fTimeDelta);
			break;
		case Client::CGolem::STUNDEATH:
			break;
		case Client::CGolem::TURNLEFT:
			break;
		case Client::CGolem::TURNRIGHT:
			break;
		default:
			break;
		}

	}

}

void CGolem::Set_Dir()
{
	//플레이어 포스 받아오기 미리 받아둘지

	//내가 바라봐야할 룩벡터 구하기, 플레이어 포스에서 내포스 빼고 노멀라이즈
	_vector vLook = XMVector3Normalize(XMLoadFloat4(&m_fPlayerPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	XMStoreFloat3(&m_vTargetLook, vLook);


}

void CGolem::Distance_BattlePattern(_float fTimeDelta)
{
	m_fPlayerPos = PM->Get_PlayerPos();

	m_fDistance = m_pTransformCom->TargetDistance(XMLoadFloat4(&m_fPlayerPos)); //타겟과 나의 거리 계속 계산

	if (m_bSpawn && m_bAction) // 스폰이 끝났을때 시작
		ActionToPlayer(fTimeDelta);
	
	RegretToPlayer(fTimeDelta);
}

void CGolem::Change_WeaponColRange(_float3 vSize, _float3 vCenter)
{
	Get_Colliders("Monster_WeaponHead")->Set_Size(vSize);
	Get_Colliders("Monster_WeaponHead")->Set_Center(vCenter);
}

void CGolem::Change_AttackColRange(_float3 vSize, _float3 vCenter)
{
	Get_Colliders("Monster_Attack")->Set_Size(vSize);
	Get_Colliders("Monster_Attack")->Set_Center(vCenter);
}

void CGolem::AttackPattern(_float fTimeDelta)
{
	//공격할수있는 상태 - AttackTime이 지정시간 이상이어야되고, 맞는 애니메이션이 끝났을때여야하고, 
	m_fAttackTime += 1.f * fTimeDelta; //애니메이션이  끝나거나 hit 상태일때 m_fAttackTime 0으로
	
	if (m_fAttackTime > 0.4f)
	{
		m_bAttack = true;


		if (m_fDistance <= 10.5f && m_fDistance > 6.f) // 원거리
		{
			BasicAttack(fTimeDelta);
			return;
		}
		else if (m_fDistance <= 8.f)
			CloseAttack(fTimeDelta);

	}

}

void CGolem::BasicAttack(_float fTimeDelta)
{
	if (m_bOneRandom)
	{
		m_iRandomAttack = GI->Get_Random(0, 6);
		m_bOneRandom = false;
	}
	switch (m_iRandomAttack)
	{
	case 0:
		Attack2(fTimeDelta);
		break;
	case 1:
		Attack1(fTimeDelta);
		break;
	case 2:
		Attack3(fTimeDelta);
		break;
	case 3:
		Attack5(fTimeDelta);
		break;
	case 4:
		Attack4(fTimeDelta);
		break;
	case 5:
		Attack7(fTimeDelta);
		break;
	case 6:
		m_bClose = false;
		Attack6_1(fTimeDelta);
		break;
	default:
		break;
	}

}

void CGolem::CloseAttack(_float fTimeDelta)
{
	if (m_iCloseAttackNum == 3)
		m_iCloseAttackNum = 0;

	switch (m_iCloseAttackNum)
	{
	case 0:
		Attack7(fTimeDelta);
		break;
	case 1:
		if (m_pTransformCom->Check_TargetFront(XMLoadFloat4(&PM->Get_PlayerPos())))
		{
			m_bClose = true;
			Attack6_1(fTimeDelta);
		}
		else
			Attack6_2(fTimeDelta);
		break;
	case 2:
		Attack4(fTimeDelta);
		break;
	default:
		break;
	}

}

void CGolem::ActionToPlayer(_float fTimeDelta)
{
	if (m_fDistance <= 30.f)
	{
		//공격하지않을때의 행동
		if (!m_bAttack)
		{
			LookAtPlayer();

			if (m_fDistance > 6.f)
				Set_State(STATE::WALKFRONT);
			else
				Set_State(IDLE);
		}


		if (m_fDistance <= 10.5f && m_fDistance > 6.f) // 원거리
		{
			m_bClose = false;
		}
		else if (m_fDistance <= 8.f)
		{
			m_bClose = true;
		
		}

		/*if (m_fDistance <= 10.5f )
			AttackPattern(fTimeDelta);*/

	}
	else
	{
		Set_State(RUN);
		return;

	}

}

void CGolem::RegretToPlayer(_float fTimeDelta)
{
	if (m_bSpawn && !m_bMove)
	{
		Set_State(IDLE);
	}
}

void CGolem::Check_Rim(_float fTimeDelta)
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

void CGolem::Check_Death(_float fTimeDelta)
{

	Check_Recover(fTimeDelta);

	if (!m_pTransformCom->Get_Air() && m_fNowHp <= 0.f)
	{
		/*Create_Item();

		if (m_iIndex == 4)
			MM->Event_Stage3_2();*/

		m_bAction = false;
		m_bHitAble = false;
		m_bAttackCol = false;
		m_bRangeCol = false;
		m_bWeaponCol = false;
		m_bDeadStart = true;

		if (m_bDeadOne)
		{
			Create_Item();

			UM->Set_FrontUI(false);
			UM->Set_BattleIndex(3);
			UM->Make_BattleEnd();

			m_bDeadOne = false;
			m_PointTrails[PTR_WEAPON]->TrailOff();
			Set_State(STUNDEATH);
			GI->PlaySoundW(L"se_EN0106_dead.wav", SD_GOLEM, 0.5f);
		}
	}

	if (m_bDissolve)
	{
		m_iShaderPath = 6;
		m_fDissolveAcc += 0.3f * fTimeDelta;
		if (m_fDissolveAcc >= 1.5f)
		{
			m_fDissolveAcc = 1.5f;
			Set_Dead();
		}

	}
}

void CGolem::Check_Stun()
{
	if (m_fPatternGauge <=  0.f)
	{
		m_bStun = true;
		m_bHitMove = false;
		m_bAction = false;
		Set_State(STUN);
	}
}

void CGolem::Check_Recover(_float fTimeDelta)
{
	if (m_bStun)
	{
		m_fRecoverAcc += 1.f * fTimeDelta;
		m_bDamageAble = false;
	}

	if (m_fRecoverAcc >= 6.f)
		m_bStun = false;
}

void CGolem::Reset_Collider()
{
	m_bAttack2Col_0 = false; m_bAttack3Col_0 = false; m_bAttack4Col_0 = false;
	m_bAttack4Col_1 = false; m_bAttack4Col_2 = false; m_bAttack4Col_3 = false;
	m_bAttack6_1Col_0 = false; m_bAttack6_2Col_0 = false; m_bAvoidAttackCol = false;
}

void CGolem::Create_Particle(_float4 fSize)
{
	CRock::ROCKINFO _tRockInfo;
	ZeroMemory(&_tRockInfo, sizeof(CRock::ROCKINFO));

	_vector vTemp = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vRight = m_pTransformCom->Get_State(CTransform::STATE_RIGHT);

	vLook = XMVector3Normalize(vLook) * fSize.z;
	vTemp += vLook;
	vTemp += XMVector3Normalize(vRight) * fSize.w;
	XMStoreFloat4(&_tRockInfo.vPosition, vTemp);
	_tRockInfo.iIndex = (_uint)fSize.x;
	_tRockInfo.fScale = fSize.y;
	GI->Add_GameObjectToLayer(TEXT("Rock"), LEVEL_TEST, TEXT("Layer_Particle"), &_tRockInfo);
}

void CGolem::Create_Effect(EFFECT eEffect)
{
	switch (eEffect)
	{
		case Client::CGolem::ATTACK5_EFFECT:
		{
			//밑에 범위뜨는거 생성
			CGolemWeaponRange::EFFECTRANGEINFO* Info = new CGolemWeaponRange::EFFECTRANGEINFO;
			Info->vRatio = { 1.f, 0.8f };

			_matrix Effectmatrix;
			Effectmatrix.r[0] = { 1.f, 0.f, 0.f, 0.f };
			Effectmatrix.r[1] = { 0.f, 1.f, 0.f, 0.f };
			Effectmatrix.r[2] = { 0.f, 0.f, 1.f, 0.f };
			Effectmatrix.r[3] = { 0.360f, 0.f, 8.f, 1.f };

			XMStoreFloat4x4(&Info->WorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
			Info->fBlurPower = 1.f;

			(GI->Add_GameObjectToLayer(TEXT("GolemWeaponRange"), LEVEL_TEST, TEXT("Layer_Monster"), &Info));

		}		
			break;
		case Client::CGolem::EFFECT_END:
			break;
		case Client::CGolem::ATTACK5_ROCKEFFECT:
		{
			CRock::ROCKINFO _tRockInfo;
			ZeroMemory(&_tRockInfo, sizeof(CRock::ROCKINFO));

			_matrix Effectmatrix;
			Effectmatrix.r[0] = { 1.f, 0.f, 0.f, 0.f };
			Effectmatrix.r[1] = { 0.f, 1.f, 0.f, 0.f };
			Effectmatrix.r[2] = { 0.f, 0.f, 1.f, 0.f };
			Effectmatrix.r[3] = { 0.61f, 0.f, 6.290f, 1.f };
			_matrix fMatrix;
			
			fMatrix = Effectmatrix * m_pTransformCom->Get_WorldMatrix();
		
			XMStoreFloat4x4(&_tRockInfo.vWorldMatrix, fMatrix);

			XMStoreFloat4(&_tRockInfo.vPosition, fMatrix.r[3]);
			_tRockInfo.iIndex = 2;
			_tRockInfo.fScale = 1;
			GI->Add_GameObjectToLayer(TEXT("Rock"), LEVEL_TEST, TEXT("Layer_Particle"), &_tRockInfo);

		}
		break;

		case Client::CGolem::ATTAK6_1EFFECT:
		{
			//밑에 범위뜨는거 생성
			CGolemAttackRange::RESERVETRAILINFO Info;

			ZeroMemory(&Info, sizeof(CGolemAttackRange::RESERVETRAILINFO));

			_matrix Effectmatrix;
			Effectmatrix.r[0] = { 1.f, 0.f, 0.f, 0.f };
			Effectmatrix.r[1] = { 0.f, 1.f, 0.f, 0.f };
			Effectmatrix.r[2] = { 0.f, 0.f, 1.f, 0.f };
			Effectmatrix.r[3] = { 0.f, 0.f, 10.26f, 1.f };

			Effectmatrix = Effectmatrix * m_pTransformCom->Get_WorldMatrix();

			Info.iIndex = 1;
			Info.bFixColor = true;
			Info.fBlurPower = 1.f;
			Info.fLifeTime = 1.6f;
			Info.fStaticY = 0.02f;
			Info.fScaleX = 4.8f;
			Info.vColor = { 1.f,0.045f,0.026f,1.f };
			Info.vColor2 = { 0.f,0.f,0.f,1.f };
			Info.vRatio = { 0.1f, 0.1f };
			XMStoreFloat4(&Info.vDir, m_pTransformCom->Get_State(CTransform::STATE_LOOK));	

			XMStoreFloat4(&Info.vPosition, Effectmatrix.r[3]);

			(GI->Add_GameObjectToLayer(TEXT("GolemAttackRange"), LEVEL_TEST, TEXT("Layer_Monster"), &Info));

		}
		break;
		case Client::CGolem::ATTAK6_1EFFECT2:
		{
			//밑에 범위뜨는거 생성
			CGolemAttackRange::RESERVETRAILINFO Info;

			ZeroMemory(&Info, sizeof(CGolemAttackRange::RESERVETRAILINFO));

			_matrix Effectmatrix;
			Effectmatrix.r[0] = { 1.f, 0.f, 0.f, 0.f };
			Effectmatrix.r[1] = { 0.f, 1.f, 0.f, 0.f };
			Effectmatrix.r[2] = { 0.f, 0.f, 1.f, 0.f };
			Effectmatrix.r[3] = { 0.f, 0.f, 10.26f, 1.f };

			Effectmatrix = Effectmatrix * m_pTransformCom->Get_WorldMatrix();

			Info.iIndex = 2;
			Info.bFixColor = true;
			Info.fBlurPower = 1.f;
			Info.fLifeTime = 1.f;
			Info.fStaticY = 0.02f;
			Info.fScaleX = 0.1f;
			Info.vColor = { 1.f,0.045f,0.026f,1.f };
			Info.vColor2 = { 0.f,0.f,0.f,1.f };
			Info.vRatio = { 0.8f, 1.f };
			Info.fScaleXSpeed = 3.2f;
			Info.fLimitScaleX = 4.8f;
			XMStoreFloat4(&Info.vDir, m_pTransformCom->Get_State(CTransform::STATE_LOOK));

			XMStoreFloat4(&Info.vPosition, Effectmatrix.r[3]);

			(GI->Add_GameObjectToLayer(TEXT("GolemAttackRange"), LEVEL_TEST, TEXT("Layer_Monster"), &Info));

		}
		break;
		default:
			break;
	}
}

void CGolem::Attack1(_float fTimeDelta)
{

	if (!m_bOneCheck)
	{
		m_bOneCheck = true;
		m_bOneRandom = true;
		Set_State(STATE::ATTACK1);

	}


}

void CGolem::Attack2(_float fTimeDelta)
{
	if (!m_bOneCheck)
	{
		m_bOneCheck = true;
		m_bOneRandom = true;
		Set_State(STATE::ATTACK2);

	}
}

void CGolem::Attack3(_float fTimeDelta)
{
	if (!m_bOneCheck)
	{
		m_bOneCheck = true;
		m_bOneRandom = true;
		Set_State(STATE::ATTACK3);

	}
}

void CGolem::Attack4(_float fTimeDelta)
{
	if (!m_bOneCheck)
	{
		m_bOneCheck = true;
		m_bOneRandom = true;
		Set_State(STATE::ATTACK4);

	}
}

void CGolem::Attack5(_float fTimeDelta)
{
	if (!m_bOneCheck)
	{
		m_bOneCheck = true;
		m_bOneRandom = true;
		Set_State(STATE::ATTACK5);

	}
}

void CGolem::Attack6_1(_float fTimeDelta)
{
	if (!m_bOneCheck)
	{
		m_bOneCheck = true;
		m_bOneRandom = true;
		Set_State(STATE::ATTACK6_1);

	}
}

void CGolem::Attack6_2(_float fTimeDelta)
{
	if (!m_bOneCheck)
	{
		m_bOneCheck = true;
		Set_State(STATE::ATTACK6_2);

	}
}

void CGolem::Attack7(_float fTimeDelta)
{
	if (!m_bOneCheck)
	{
		m_bOneCheck = true;
		m_bOneRandom = true;
		Set_State(STATE::ATTACK7);

	}
}

void CGolem::Attack1_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	m_bWeaponCol = false;
	m_bWeaponTrail = false;

	if (m_pModelCom->Get_KeyFrame() >= 18.f &&
		m_pModelCom->Get_KeyFrame() <= 60.f)
	{
		m_fAttack1EffectAcc += 1.f * fTimeDelta;
		if (m_fAttack1EffectAcc >= 0.05f)
		{
			_matrix WeaponMatrix = m_Sockets[SOKET_WEAPON]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
			WeaponMatrix.r[0] = XMVector3Normalize(WeaponMatrix.r[0]);
			WeaponMatrix.r[1] = XMVector3Normalize(WeaponMatrix.r[1]);
			WeaponMatrix.r[2] = XMVector3Normalize(WeaponMatrix.r[2]);
			_float4x4 Matrix;
			XMStoreFloat4x4(&Matrix, WeaponMatrix);
			PTM->Create_InstanceParticle(L"GolemTrailParticle", Matrix);
			m_fAttack1EffectAcc = 0.f;
		}
	}
	

	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(0))
	{
		LookAtPlayer();
	}

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(1) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(2))
	{	
		m_bWeaponCol = true;
		
		m_eHitType = TYPE_HIT;
		m_fPushPower = 10.f;
		m_fAirPower = 0.f;
		m_fDamage = 150.f;
	}


	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(3) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(4))
	{
		m_bWeaponCol = true;

		m_eHitType = TYPE_AWAY;
		m_fPushPower = 15.f;
		m_fAirPower = 5.f;
		m_fDamage = 210.f;
	}
	
	m_bWeaponTrail = true;

	if (m_bColDelay)
	{
		m_bPlayerHit = false;
		m_fColDelayTime += fTimeDelta;

		if (m_fColDelayTime > 0.5f)
		{
			m_fColDelayTime = 0.f;
			m_bPlayerHit = true;
			m_bColDelay = false;
		}
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(5) && !m_bAttack1On)
	{
		GI->PlaySoundW(L"se_EN0102_attack1_1.wav", SD_GOLEM, 1.f);
		GI->PlaySoundW(L"GolemSkill4_2.ogg", SD_GOLEM, 0.7f);
		m_bAttack1On = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(3) && m_bAttack1On)
	{
		GI->PlaySoundW(L"se_EN0102_attack1_2.wav", SD_GOLEM, 1.f);
		GI->PlaySoundW(L"GolemSkill4.ogg", SD_GOLEM, 0.7f);
		m_bAttack1On = false;
	}

	// 바닥에 불장판? 같은 이펙트 생길 때
	/*if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(6) && !m_bAttack1Fire)
	{
		GI->PlaySoundW(L"se_EN0106_explosion1.wav", SD_BULLET, 0.6f);
		m_bAttack1Fire = true;
	}*/
}

void CGolem::Attack2_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	m_bWeaponCol = false;
	m_bWeaponTrail = false;
	m_bPlayerHit = true;

	if (m_pModelCom->Get_KeyFrame() >= 14.f &&
		m_pModelCom->Get_KeyFrame() <= 19.f)
	{
		m_fAttack1EffectAcc += 1.f * fTimeDelta;
		if (m_fAttack1EffectAcc >= 0.05f)
		{
			_matrix WeaponMatrix = m_Sockets[SOKET_WEAPON]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
			WeaponMatrix.r[0] = XMVector3Normalize(WeaponMatrix.r[0]);
			WeaponMatrix.r[1] = XMVector3Normalize(WeaponMatrix.r[1]);
			WeaponMatrix.r[2] = XMVector3Normalize(WeaponMatrix.r[2]);
			_float4x4 Matrix;
			XMStoreFloat4x4(&Matrix, WeaponMatrix);
			PTM->Create_InstanceParticle(L"GolemTrailParticle", Matrix);
			m_fAttack1EffectAcc = 0.f;
		}
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bAttack2Col_0)
	{
		GI->PlaySoundW(L"GolemSkill1_2.ogg", SD_GOLEM, 0.7f);
		m_bWeaponCol = true;
		m_bAttack2Col_0 = true;
		CRM->Get_Cam()->Shake(0.15f, 0.08f);
		Create_Particle(m_fParticle);
		GI->PlaySoundW(L"se_EN0102_attack2.wav", SD_GOLEM, 1.f);

		PTM->Create_InstanceParticle(L"GolemAttack2", m_pTransformCom->Get_WorldFloat4x4());

		CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
		Info3->fBlurPower = 1.f;
		Info3->vLocalPos = { 1.f,0.f,8.f,1.f };
		Info3->vColor = { 0.5f,0.5f,0.5f,1.f };
		Info3->vColor2 = { 0.5f,0.5f,0.5f,1.f };
		Info3->vRatio = { 1.f,1.f };
		Info3->vRatioSpeed = { 1.f,1.f };
		Info3->vScaleSpeed = { 70.f,50.f,70.f };
		Info3->vScale = { 10.f,10.f,10.f };
		Info3->fTurnSpeed = 7.f;
		Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3);
			
		CPlayer01Wind2::WIND2INFO* Info4 = new CPlayer01Wind2::WIND2INFO;
		Info4->fBlurPower = 1.f;
		Info4->vLocalPos = { 1.f,0.f,8.f,1.f };
		Info4->vColor = { 0.5f,0.5f,0.5f,1.f };
		Info4->vColor2 = { 0.5f,0.5f,0.5f,1.f };
		Info4->vRatio = { 1.f,1.f };
		Info4->vRatioSpeed = { 1.f,1.f };
		Info4->vScaleSpeed = { 70.f,50.f,70.f };
		Info4->vScale = { 10.f,10.f,10.f };
		Info4->fTurnSpeed = 5.f;
		Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4);			

	}
	
	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(1))
	{
		LookAtPlayer();
	}

	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(2))
		m_bWeaponTrail = true;

	// 바닥에 불장판? 같은 이펙트 생길 때
	/*if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(3) && !m_bAttack2Fire)
	{
		GI->PlaySoundW(L"se_EN0106_explosion1.wav", SD_BULLET, 0.6f);
		m_bAttack2Fire = false;
	}*/
}

void CGolem::Attack3_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	m_bPlayerHit = true;
	m_bWeaponCol = false;


	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bAttack3Col_0)
	{
		GI->PlaySoundW(L"GolemSkill8.ogg", SD_GOLEM, 0.7f);
		m_bWeaponCol = true;
		CRM->Get_Cam()->Shake(0.15f, 0.15f, false, 0.1f);
		Create_Particle(m_fParticle);
		m_bAttack3Col_0 = true;
		GI->PlaySoundW(L"EFF_CHR_Spell_Electric_03_A.ogg", SD_GOLEM, 0.8f);

		PTM->Create_InstanceParticle(L"GolemAttack2", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"GolemAttack3", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"GolemSunder", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"GolemSunder_2", m_pTransformCom->Get_WorldFloat4x4());

		CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
		Info3->fBlurPower = 1.f;
		Info3->vLocalPos = { 0.f,0.f,7.f,1.f };
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
		Info4->vLocalPos = { 0.f,0.f,7.f,1.f };
		Info4->vColor = { 0.5f,0.5f,0.5f,1.f };
		Info4->vColor2 = { 0.5f,0.5f,0.5f,1.f };
		Info4->vRatio = { 1.f,1.f };
		Info4->vRatioSpeed = { 1.f,1.f };
		Info4->vScaleSpeed = { 140.f,100.f,140.f };
		Info4->vScale = { 10.f,10.f,10.f };
		Info4->fTurnSpeed = 5.f;
		Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4);

	}

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(1) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(2))
	{
		LookAtPlayer();
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(3) && !m_bAttack3Off)
	{
		GI->PlaySoundW(L"CHR_Axe_Whirl_01_D.ogg", SD_BULLET, 0.6f);
		m_bAttack3Off = false;
	}
}

void CGolem::Attack4_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	m_bWeaponCol = false;
	m_bAttackCol = false;
	m_bWeaponTrail = false;

	if (m_pModelCom->Get_KeyFrame() >= 16.f &&	m_pModelCom->Get_KeyFrame() <= 21.f)
	{
		m_fAttack1EffectAcc += 1.f * fTimeDelta;
		if (m_fAttack1EffectAcc >= 0.03f)
		{
			_matrix WeaponMatrix = m_Sockets[SOKET_WEAPON]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
			WeaponMatrix.r[0] = XMVector3Normalize(WeaponMatrix.r[0]);
			WeaponMatrix.r[1] = XMVector3Normalize(WeaponMatrix.r[1]);
			WeaponMatrix.r[2] = XMVector3Normalize(WeaponMatrix.r[2]);
			_float4x4 Matrix;
			XMStoreFloat4x4(&Matrix, WeaponMatrix);
			PTM->Create_InstanceParticle(L"GolemTrailParticle", Matrix);
			m_fAttack1EffectAcc = 0.f;
		}
	}

	if (m_pModelCom->Get_KeyFrame() >= 40.f &&	m_pModelCom->Get_KeyFrame() <= 46.f)
	{
		m_fAttack1EffectAcc += 1.f * fTimeDelta;
		if (m_fAttack1EffectAcc >= 0.03f)
		{
			_matrix WeaponMatrix = m_Sockets[SOKET_WEAPON]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
			WeaponMatrix.r[0] = XMVector3Normalize(WeaponMatrix.r[0]);
			WeaponMatrix.r[1] = XMVector3Normalize(WeaponMatrix.r[1]);
			WeaponMatrix.r[2] = XMVector3Normalize(WeaponMatrix.r[2]);
			_float4x4 Matrix;
			XMStoreFloat4x4(&Matrix, WeaponMatrix);
			PTM->Create_InstanceParticle(L"GolemTrailParticle", Matrix);
			m_fAttack1EffectAcc = 0.f;
		}
	}

	if (m_pModelCom->Get_KeyFrame() >= 74.f &&	m_pModelCom->Get_KeyFrame() <= 92.f)
	{
		m_fAttack1EffectAcc += 1.f * fTimeDelta;
		if (m_fAttack1EffectAcc >= 0.03f)
		{
			_matrix WeaponMatrix = m_Sockets[SOKET_WEAPON]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
			WeaponMatrix.r[0] = XMVector3Normalize(WeaponMatrix.r[0]);
			WeaponMatrix.r[1] = XMVector3Normalize(WeaponMatrix.r[1]);
			WeaponMatrix.r[2] = XMVector3Normalize(WeaponMatrix.r[2]);
			_float4x4 Matrix;
			XMStoreFloat4x4(&Matrix, WeaponMatrix);
			PTM->Create_InstanceParticle(L"GolemTrailParticle", Matrix);
			m_fAttack1EffectAcc = 0.f;
		}
	}


	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(0))
	{
		LookAtPlayer();
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bAttack4Col_0)
	{
		GI->PlaySoundW(L"GolemSkill1_2.ogg", SD_GOLEM, 0.7f);
		m_bPlayerHit = true;
		m_bWeaponCol = true;
		m_bAttack4Col_0 = true;
		CRM->Get_Cam()->Shake(0.05f, 0.1f);
		Create_Particle(m_fParticle);
		GI->PlaySoundW(L"se_EN0102_attack1_2.wav", SD_GOLEM, 1.f);
		PTM->Create_InstanceParticle(L"GolemAttack4", m_pTransformCom->Get_WorldFloat4x4());

		CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
		Info3->fBlurPower = 1.f;
		Info3->vLocalPos = { 1.f,0.f,10.f,1.f };
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
		Info4->vLocalPos = { 1.f,0.f,10.f,1.f };
		Info4->vColor = { 0.5f,0.5f,0.5f,1.f };
		Info4->vColor2 = { 0.5f,0.5f,0.5f,1.f };
		Info4->vRatio = { 1.f,1.f };
		Info4->vRatioSpeed = { 1.f,1.f };
		Info4->vScaleSpeed = { 140.f,100.f,140.f };
		Info4->vScale = { 10.f,10.f,10.f };
		Info4->fTurnSpeed = 5.f;
		Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4);
	}
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bAttack4Col_1)
	{
		GI->PlaySoundW(L"GolemSkill1_2.ogg", SD_GOLEM, 0.7f);
		m_bPlayerHit = true;
		m_bWeaponCol = true;
		m_bAttack4Col_1 = true;
		CRM->Get_Cam()->Shake(0.05f, 0.1f);
		Create_Particle(m_fParticle);
		GI->PlaySoundW(L"se_EN0102_attack1_2.wav", SD_GOLEM, 1.f);
		PTM->Create_InstanceParticle(L"GolemAttack4", m_pTransformCom->Get_WorldFloat4x4());

		CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
		Info3->fBlurPower = 1.f;
		Info3->vLocalPos = { 1.f,0.f,10.f,1.f };
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
		Info4->vLocalPos = { 1.f,0.f,10.f,1.f };
		Info4->vColor = { 0.5f,0.5f,0.5f,1.f };
		Info4->vColor2 = { 0.5f,0.5f,0.5f,1.f };
		Info4->vRatio = { 1.f,1.f };
		Info4->vRatioSpeed = { 1.f,1.f };
		Info4->vScaleSpeed = { 140.f,100.f,140.f };
		Info4->vScale = { 10.f,10.f,10.f };
		Info4->fTurnSpeed = 5.f;
		Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4);
	}
	//Get_TimeLimit(11) 휘두르는 타이밍
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(11) && !m_bAttack4On)
	{
		GI->PlaySoundW(L"GolemSkill3.ogg", SD_GOLEM, 0.7f);
		GI->PlaySoundW(L"se_EN0102_attack4_2.wav", SD_GOLEM, 1.f);
		m_bAttack4On = true;

		CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
		Info3->fBlurPower = 1.f;
		Info3->vLocalPos = { 0.f,2.f,0.f,1.f };
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
		Info4->vLocalPos = { 0.f,2.f,0.f,1.f };
		Info4->vColor = { 0.5f,0.5f,0.5f,1.f };
		Info4->vColor2 = { 0.5f,0.5f,0.5f,1.f };
		Info4->vRatio = { 1.f,1.f };
		Info4->vRatioSpeed = { 1.f,1.f };
		Info4->vScaleSpeed = { 140.f,100.f,140.f };
		Info4->vScale = { 10.f,10.f,10.f };
		Info4->fTurnSpeed = 5.f;
		Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4);

	}
	//Get_TimeLimit(12) 무기 올리는 타이밍
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(12) && m_bAttack4On)
	{
		GI->PlaySoundW(L"CHR_Axe_Whirl_01_C.ogg", SD_BULLET, 0.6f);
		m_bAttack4On = false;
	}
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(3))
	{
		m_bColDelay = false;
		m_bPlayerHit = true;
		m_fColDelayTime = 0.f;
	}

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(3) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(4))
	{
		m_eHitType = TYPE_SPIN;
		m_fPushPower = 12.f;
		m_fAirPower = 10.f;
		m_fDamage = 200.f;
		Change_WeaponColRange(_float3{ 3.6f, 3.6f, 4.f }, _float3{ 0.f, -0.2f, -2.4f });

		m_bWeaponCol = true;
		
		if (m_bColDelay)
		{
			m_bPlayerHit = false;
			m_fColDelayTime += 1.f * fTimeDelta;

			if (m_fColDelayTime > 0.5f)
			{
				m_fColDelayTime = 0.f;
				m_bPlayerHit = true;
				m_bColDelay = false;
			}
		}
	}

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(5) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(6))
		m_bWeaponTrail = true;

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(7) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(8))
		m_bWeaponTrail = true;
	
	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(9) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(10))
		m_bWeaponTrail = true;
}

void CGolem::Attack5_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	m_bWeaponCol = false;

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(0) && m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(1))
	{
		m_bWeaponCol = true;
		CPlayer01Pillar::PILLARINFO* Info = new CPlayer01Pillar::PILLARINFO;
		Info->fBlurPower = 1.f;
		Info->vLocalPos = { 0.f,0.f,6.f,1.f };
		Info->vColor = { 1.f,1.f,1.f,1.f };
		Info->vColor2 = { 0.f,1.f,1.f,1.f };
		Info->vRatio = { 1.f,1.f };
		Info->vRatioSpeed = { 2.f,2.f };
		Info->vScale = { 10.f,30.f,10.f };
		Info->vScaleSpeed = { 150.f,300.f,150.f };
		Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Pillar", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);


		CPlayer01Pillar2::PILLAR2INFO* Info2 = new CPlayer01Pillar2::PILLAR2INFO;
		Info2->fBlurPower = 3.f;
		Info2->vLocalPos = { 0.f,0.f,6.f,1.f };
		Info2->vColor = { 1.f,1.f,1.f,1.f };
		Info2->vColor2 = { 0.f,1.f,1.f,1.f };
		Info2->vRatio = { 1.f,1.f };
		Info2->vRatioSpeed = { 1.5f,1.5f };
		Info2->vScale = { 10.f,6.f,10.f };
		Info2->vScaleSpeed = { 120.f,75.f,120.f };
		Info2->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Pillar2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2);

		CPlayer01Pillar3::PILLAR3INFO* Info3 = new CPlayer01Pillar3::PILLAR3INFO;
		Info3->fBlurPower = 3.f;
		Info3->vLocalPos = { 0.f,0.f,6.f,1.f };
		Info3->vColor = { 1.f,1.f,1.f,1.f };
		Info3->vColor2 = { 0.f,1.f,1.f,1.f };
		Info3->vRatio = { 1.f,1.f };
		Info3->vRatioSpeed = { 1.5f,1.5f };
		Info3->vScale = { 7.f,7.f,7.f };
		Info3->vScaleSpeed = { 75.f,75.f,75.f };
		Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Pillar3", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3);

			
		PTM->Create_InstanceParticle(L"GolemAttack5", m_pTransformCom->Get_WorldFloat4x4());
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1))
	{
		CRM->Get_Cam()->Shake(0.25f, 0.3f, false, 0.1f);
		Create_Effect(ATTACK5_ROCKEFFECT);
	}


	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(2))
	{
		LookAtPlayer();
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(8) && !m_bAttack5Effect)
	{

		m_bAttack5Effect = true;
		//이펙트 생성
		Create_Effect(ATTACK5_EFFECT);
	}

	//Get_TimeLimit(5) 점프시작 타이밍
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(5) && !m_bAttack5On)
	{
		GI->PlaySoundW(L"se_EN0102_attack5_1.wav", SD_GOLEM, 1.f);
		m_bAttack5On = true;
	}

	//Get_TimeLimit(7) 올라가는 타이밍
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(7) && m_bAttack5On)
	{
		GI->PlaySoundW(L"se_EN0102_attack5_2.wav", SD_BULLET, 1.f);
		m_bAttack5On = false;
	}

	//Get_TimeLimit(0) 찍을떄 타이밍
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bAttack5On)
	{
		GI->PlaySoundW(L"GolemStart.ogg", SD_GOLEM, 1.f);
		GI->PlaySoundW(L"GolemSkill10_1.ogg", SD_GOLEM, 1.f);
		m_bAttack5On = true;
	}

	//Get_TimeLimit(6) 무기 올리는 타이밍
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(6) && m_bAttack5On)
	{
		GI->PlaySoundW(L"CHR_Axe_Whirl_01_C.ogg", SD_BULLET, 0.6f);
		m_bAttack5On = false;
	}
}

void CGolem::Attack6_1Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	m_bWeaponCol = false;
	m_bPlayerHit = true;
	m_bRangeCol = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(3) && !m_bAttack6Sound)
	{
		m_bAttack6Sound = true;
		GI->PlaySoundW(L"GolemSkill8.ogg", SD_GOLEM, 1.f);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bAttack6_1Col_0)
	{
		m_bAttackCol = true;
		m_bAttack6_1Col_0 = true;
		GI->PlaySoundW(L"se_EN0102_attack4_2.wav", SD_GOLEM, 1.f);
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
	}

	if (!m_bClose) //멀리있을때
	{
		if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(1) &&
			m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(2))
		{
			m_pTransformCom->Go_Dir(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 15.f, fTimeDelta);
		}

		if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(7))
			m_pModelCom->Set_TickPerSecond(10.f);
		else
			m_pModelCom->Set_TickPerSecond(28.1f);


		if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(3) &&
			m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(5))
		{
			m_bRangeCol = true;
		}

	}

	//Get_TimeLimit(6) 무기 올리는 타이밍
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(6) && !m_bAttack6On)
	{
		GI->PlaySoundW(L"CHR_Axe_Whirl_01_C.ogg", SD_BULLET, 0.6f);
		m_bAttack6On = true;
	}
}

void CGolem::Attack6_2Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	m_bWeaponCol = false;
	m_bPlayerHit = true;

	if (m_pModelCom->Get_KeyFrame() == 48.f && !m_bAttack6Sound_2)
	{
		m_bAttack6Sound_2 = true;
		GI->PlaySoundW(L"GolemSkill9.ogg", SD_GOLEM, 1.f);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bAttack6_2Col_0)
	{
		m_bAttackCol = true;
		m_bAttack6_2Col_0 = true;
		GI->PlaySoundW(L"se_EN0102_attack4_2.wav", SD_GOLEM, 1.f); 
		CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
		Info3->fBlurPower = 1.f;
		Info3->vLocalPos = { 0.f,0.f,-1.f,1.f };
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
		Info4->vLocalPos = { 0.f,0.f,-1.f,1.f };
		Info4->vColor = { 0.5f,0.5f,0.5f,1.f };
		Info4->vColor2 = { 0.5f,0.5f,0.5f,1.f };
		Info4->vRatio = { 1.f,1.f };
		Info4->vRatioSpeed = { 1.f,1.f };
		Info4->vScaleSpeed = { 140.f,100.f,140.f };
		Info4->vScale = { 10.f,10.f,10.f };
		Info4->fTurnSpeed = 5.f;
		Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4);
	}

	//Get_TimeLimit(1) 무기 올리는 타이밍
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bAttack6On)
	{
		GI->PlaySoundW(L"CHR_Axe_Whirl_01_C.ogg", SD_BULLET, 0.6f);
		m_bAttack6On = true;
	}

}

void CGolem::Attack7_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	m_bWeaponCol = false;
	m_bAttackCol = false;
	m_bWeaponTrail = false;

	if (m_pModelCom->Get_KeyFrame() >= 30.f && !m_bAttack7Effect)
	{
		GI->PlaySoundW(L"GolemSkill2_2.ogg", SD_GOLEM, 0.7f);

		m_bAttack7Effect = true;
		CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
		Info3->fBlurPower = 1.f;
		Info3->vLocalPos = { 0.f,1.f,0.f,1.f };
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
		Info4->vLocalPos = { 0.f,1.f,0.f,1.f };
		Info4->vColor = { 0.5f,0.5f,0.5f,1.f };
		Info4->vColor2 = { 0.5f,0.5f,0.5f,1.f };
		Info4->vRatio = { 1.f,1.f };
		Info4->vRatioSpeed = { 1.f,1.f };
		Info4->vScaleSpeed = { 140.f,100.f,140.f };
		Info4->vScale = { 10.f,10.f,10.f };
		Info4->fTurnSpeed = 5.f;
		Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4);
	}

	if (m_pModelCom->Get_KeyFrame() >= 46.f && !m_bAttack7Effect_2)
	{
		GI->PlaySoundW(L"GolemSkill2_2.ogg", SD_GOLEM, 0.7f);

		m_bAttack7Effect_2 = true;
		CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
		Info3->fBlurPower = 1.f;
		Info3->vLocalPos = { 0.f,1.f,0.f,1.f };
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
		Info4->vLocalPos = { 0.f,1.f,7.f,1.f };
		Info4->vColor = { 0.5f,0.5f,0.5f,1.f };
		Info4->vColor2 = { 0.5f,0.5f,0.5f,1.f };
		Info4->vRatio = { 1.f,1.f };
		Info4->vRatioSpeed = { 1.f,1.f };
		Info4->vScaleSpeed = { 140.f,100.f,140.f };
		Info4->vScale = { 10.f,10.f,10.f };
		Info4->fTurnSpeed = 5.f;
		Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4);
	}

	if (m_pModelCom->Get_KeyFrame() >= 66.f && !m_bAttack7Effect_3)
	{
		GI->PlaySoundW(L"GolemSkill3.ogg", SD_GOLEM, 0.7f);
		m_bAttack7Effect_3 = true;
		CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
		Info3->fBlurPower = 1.f;
		Info3->vLocalPos = { 0.f,1.f,0.f,1.f };
		Info3->vColor = { 0.5f,0.5f,0.5f,1.f };
		Info3->vColor2 = { 0.5f,0.5f,0.5f,1.f };
		Info3->vRatio = { 1.f,1.f };
		Info3->vRatioSpeed = { 0.7f,0.7f };
		Info3->vScaleSpeed = { 200.f,150.f,200.f };
		Info3->vScale = { 10.f,10.f,10.f };
		Info3->fTurnSpeed = 10.f;
		Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3);

		CPlayer01Wind2::WIND2INFO* Info4 = new CPlayer01Wind2::WIND2INFO;
		Info4->fBlurPower = 1.f;
		Info4->vLocalPos = { 0.f,1.f,0.f,1.f };
		Info4->vColor = { 0.5f,0.5f,0.5f,1.f };
		Info4->vColor2 = { 0.5f,0.5f,0.5f,1.f };
		Info4->vRatio = { 1.f,1.f };
		Info4->vRatioSpeed = { 0.7f,0.7f };
		Info4->vScaleSpeed = { 200.f,150.f,200.f };
		Info4->vScale = { 10.f,10.f,10.f };
		Info4->fTurnSpeed = 7.f;
		Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4);
	}

	if (m_pModelCom->Get_KeyFrame() >= 24.f &&	m_pModelCom->Get_KeyFrame() <= 80.f)
	{
		m_fAttack1EffectAcc += 1.f * fTimeDelta;
		if (m_fAttack1EffectAcc >= 0.03f)
		{
			_matrix WeaponMatrix = m_Sockets[SOKET_WEAPON]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
			WeaponMatrix.r[0] = XMVector3Normalize(WeaponMatrix.r[0]);
			WeaponMatrix.r[1] = XMVector3Normalize(WeaponMatrix.r[1]);
			WeaponMatrix.r[2] = XMVector3Normalize(WeaponMatrix.r[2]);
			_float4x4 Matrix;
			XMStoreFloat4x4(&Matrix, WeaponMatrix);
			PTM->Create_InstanceParticle(L"GolemTrailParticle", Matrix);
			m_fAttack1EffectAcc = 0.f;
		}
	}

	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(0))
	{
		LookAtPlayer();
	}

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(1) &&
		m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(2))
	{		 
		Change_WeaponColRange(_float3{ 3.f, 3.f, 4.f }, _float3{ 0.f, -0.2f, -2.4f });

		m_bWeaponCol = true;
		m_bAttackCol = true;
	}

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(3))
	{
		m_eHitType = TYPE_AWAY;
		m_fPushPower = 30.f;
		m_fAirPower = 5.f;
		m_fDamage = 250.f;
		m_bAttackCol = false;
	}

	if (m_bColDelay)
	{
		m_bPlayerHit = false;
		m_fColDelayTime += 1.f * fTimeDelta;

		if (m_fColDelayTime > 0.5f)
		{
			m_fColDelayTime = 0.f;
			m_bPlayerHit = true;
			m_bColDelay = false;
		}
	}

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(4) )
		m_bWeaponTrail = true;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bAttack7On)
	{
		m_bAttack7On = true;
		GI->PlaySoundW(L"se_EN0102_attack6.wav", SD_GOLEM, 1.f);
	}

	//Get_TimeLimit(5) 무기 올리는 타이밍
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(5) && m_bAttack7On)
	{
		GI->PlaySoundW(L"CHR_Axe_Whirl_01_C.ogg", SD_BULLET, 0.6f);
		m_bAttack7On = false;
	}
}

void CGolem::Spawn1_Update(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(0))
		m_pTransformCom->Go_Dir(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.8f, fTimeDelta, m_pNavigationCom);

	if (!m_bSpawn1Walk)
	{
		if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) ||
			m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(3) ||
			m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(5))
		{
			CRM->Get_Cam()->Shake(0.08f, 0.15f, false, 0.02f);
			GI->PlaySoundW(L"Boss_Step_Wlk_01.wav", SD_GOLEM, 1.f);
			m_bSpawn1Walk = true;
		}
	}

	else
	{
		if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) || 
			m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(4))
		{
			CRM->Get_Cam()->Shake(0.08f, 0.15f, false, 0.02f);
			GI->PlaySoundW(L"Boss_Step_Wlk_02.wav", SD_TRAININGBOT2, 1.f);
			m_bSpawn1Walk = false;
		}
	}
}

void CGolem::Spawn2_Update(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bSpawn2Shout)
	{
		CRM->Get_Cam()->Shake(0.3f, 1.8f, false, 0.1f);
		GI->PlaySoundW(L"VO_MON_Golem_Long_Shout_01_A.ogg", SD_TRAININGBOT1, 1.f);
		m_bSpawn2Shout = true;
	}

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(0) && m_pModelCom->Get_KeyFrame() <= 80.f)
	{
		m_fCutEffectAcc += 1.f * fTimeDelta;
		m_fCutEffect2Acc += 1.f * fTimeDelta;
		if (m_fCutEffectAcc >= 0.16f)
		{
			m_fCutEffectAcc = 0.f;
			PTM->Create_InstanceParticle(L"GolemCutScene", m_pTransformCom->Get_WorldFloat4x4());
			PTM->Create_InstanceParticle(L"GolemCutScene_2", m_pTransformCom->Get_WorldFloat4x4());
			PTM->Create_InstanceParticle(L"GolemCutScene_3", m_pTransformCom->Get_WorldFloat4x4());
			
		}

		if (m_fCutEffect2Acc >= 0.1f)
		{
		
			PTM->Create_InstanceParticle(L"GolemCutScene_4", m_pTransformCom->Get_WorldFloat4x4());
			m_fCutEffect2Acc = 0.f;
		}
	}
}

void CGolem::Idle_Update(_float fTimeDelta)
{
	if (!m_bMove)
		LookAtPlayer();

	m_bRangeCol = true;

	if (m_bRemove)
		m_fRemoveAcc += 1.f * fTimeDelta;

	if (m_fRemoveAcc > 1.f && m_bOneMove)
	{
		m_bOneMove = false;
		m_bRemove = false;
		MM->Control_Golem();
	}
}

void CGolem::HitBack_Update(_float fTimeDelta)
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

void CGolem::StunRecover_Update(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bRecoverOn)
	{
		GI->PlaySoundW(L"VO_MON_Golem_Growl_01_B.ogg", SD_GOLEM, 1.f);
		m_bRecoverOn = true;
	}
}

void CGolem::Run_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	m_fSpeed = 5.f;
	LookAtPlayer();
	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), m_fSpeed, fTimeDelta, m_pNavigationCom);

	m_fRunSoundAcc += 1.f * fTimeDelta;

	if (!m_bOtherFoot)
	{
		if (m_fRunSoundAcc >= 0.74f)
		{
			GI->PlaySoundW(L"MON_Gigantic_Garganta_Bone_Move_01_E.ogg", SD_GOLEM, 0.8f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = true;
		}
	}

	else
	{
		if (m_fRunSoundAcc >= 0.74f)
		{
			GI->PlaySoundW(L"MON_Gigantic_Garganta_Bone_Move_01_F.ogg", SD_GOLEM, 0.8f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = false;
		}
	}
}

void CGolem::Walk_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	m_fSpeed = 1.3f;
	LookAtPlayer();
	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), m_fSpeed, fTimeDelta, m_pNavigationCom);

	m_fRunSoundAcc += 1.f * fTimeDelta;

	if (!m_bOtherFoot)
	{
		if (m_fRunSoundAcc >= 1.3f)
		{
			GI->PlaySoundW(L"MON_Gigantic_Garganta_Bone_Move_01_E.ogg", SD_GOLEM, 0.6f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = true;
		}
	}

	else
	{
		if (m_fRunSoundAcc >= 1.3f)
		{
			GI->PlaySoundW(L"MON_Gigantic_Garganta_Bone_Move_01_F.ogg", SD_GOLEM, 0.6f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = false;
		}
	}
}

void CGolem::WalkBack_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	m_fSpeed = 2.f;
	LookAtPlayer();

	m_fRunSoundAcc += 1.f * fTimeDelta;

	if (!m_bOtherFoot)
	{
		if (m_fRunSoundAcc >= 1.3f)
		{
			GI->PlaySoundW(L"MON_Gigantic_Garganta_Bone_Move_01_E.ogg", SD_GOLEM, 0.6f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = true;
		}
	}

	else
	{
		if (m_fRunSoundAcc >= 1.3f)
		{
			GI->PlaySoundW(L"MON_Gigantic_Garganta_Bone_Move_01_F.ogg", SD_GOLEM, 0.6f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = false;
		}
	}
}

void CGolem::Strafe_Update(_float fTimeDelta)
{
	m_bRangeCol = true;
	LookAtPlayer();

	m_fRunSoundAcc += 1.f * fTimeDelta;

	if (!m_bOtherFoot)
	{
		if (m_fRunSoundAcc >= 2.5f)
		{
			GI->PlaySoundW(L"MON_Gigantic_Garganta_Bone_Move_01_E.ogg", SD_GOLEM, 0.6f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = true;
		}
	}

	else
	{
		if (m_fRunSoundAcc >= 2.5f)
		{
			GI->PlaySoundW(L"MON_Gigantic_Garganta_Bone_Move_01_F.ogg", SD_GOLEM, 0.6f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = false;
		}
	}
}

HRESULT CGolem::Ready_Trails()
{

	CPointTrail::TRAILCREATEINFO Info2;
	Info2.vColor = { 0.145f, 0.603f, 0.858f,1.f };
	Info2.vColor2 = { 0.145f, 0.603f, 0.858f,1.f };
	Info2.bFixColor = true;
	Info2.fBlurPower = 1.f;
	Info2.iShaderPass = 1;
	Info2.vRatio = { 1.f,1.f };
	Info2.fSize = 1.5f;
	Info2.TexName = L"WeaponTrail";
	Info2.iMaxIndex = 22;

	CGameObject*		WeaponTrail = GI->Clone_GameObject(TEXT("PointTrailObj"), &Info2);
	if (nullptr == WeaponTrail)
		return E_FAIL;
	m_PointTrails.push_back((CPointTrailObj*)WeaponTrail);


	Info2.vColor = { 0.454f,0.764f,0.792f,1.f };
	Info2.vColor2 = { 0.345f,0.686f,0.721f,1.f };
	Info2.bFixColor = true;
	Info2.fBlurPower = 1.f;
	Info2.iShaderPass = 1;
	Info2.vRatio = { 1.f,1.f };
	Info2.fSize = 0.3f;
	Info2.TexName = L"EyeTrail";
	Info2.iMaxIndex = 16;


	CGameObject*		REyeTrail = GI->Clone_GameObject(TEXT("PointTrailObj"), &Info2);
	if (nullptr == REyeTrail)
		return E_FAIL;
	m_PointTrails.push_back((CPointTrailObj*)REyeTrail);

	CGameObject*		LEyeTrail = GI->Clone_GameObject(TEXT("PointTrailObj"), &Info2);
	if (nullptr == LEyeTrail)
		return E_FAIL;
	m_PointTrails.push_back((CPointTrailObj*)LEyeTrail);

	return S_OK;
}

HRESULT CGolem::Ready_Socket()
{
	CHierarchyNode* pHead = m_pModelCom->Get_HierarchyNode("Head_M");
	if (nullptr == pHead)
		return E_FAIL;
	m_Sockets.push_back(pHead);

	return S_OK;
}

void CGolem::Create_PointTrail(_float fTimeDelta)
{

	Control_WeaponTrail(fTimeDelta);

	Control_RightEyeTrail(fTimeDelta);

	Control_LeftEyeTrail(fTimeDelta);

}

void CGolem::Control_WeaponTrail(_float fTimeDelta)
{
	if (m_bWeaponTrail)
	{
		_matrix WeaponMatrix;

		WeaponMatrix = m_Sockets[SOKET_WEAPON]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();

		_float4 WeaponPos;

		_vector vTemp = WeaponMatrix.r[3];
		_vector vUp = WeaponMatrix.r[1];
		_vector vLook = WeaponMatrix.r[2];

		vUp = XMVector3Normalize(vUp) * 0.6f;
		vTemp += vUp;
		vLook = XMVector3Normalize(vLook) * 4.4f;
		vTemp -= vLook;


		/*vUp = XMVector3Normalize(vUp) * 0.8f;
		vTemp -= vUp;
		vLook = XMVector3Normalize(vLook) * 4.f;
		vTemp -= vLook;*/

		XMStoreFloat4(&WeaponPos, vTemp);

		m_PointTrails[PTR_WEAPON]->TrailOn();
		m_PointTrails[PTR_WEAPON]->Add_Point(WeaponPos);

		m_PointTrails[PTR_WEAPON]->Tick(fTimeDelta);
	}
	else
		m_PointTrails[PTR_WEAPON]->TrailOff();
	

}

void CGolem::Control_RightEyeTrail(_float fTimeDelta)
{
	
	_matrix HeadMatrix;

	HeadMatrix = m_Sockets[SOCKET_HEAD]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();

	_float4 REyePos;

	_vector vTemp = HeadMatrix.r[3];
	_vector vRight = HeadMatrix.r[0];
	_vector vUp = HeadMatrix.r[1];
	_vector vLook = HeadMatrix.r[2];

	vRight = XMVector3Normalize(vRight) * 0.21f;
	vTemp += vRight;

	vUp = XMVector3Normalize(vUp) * 0.41f;
	vTemp += vUp; //앞뒤

	vLook = XMVector3Normalize(vLook) * 0.17f;
	vTemp += vLook; //좌우

	XMStoreFloat4(&REyePos, vTemp);


	m_PointTrails[PTR_REYE]->TrailOn();
	m_PointTrails[PTR_REYE]->Add_Point(REyePos);	
	m_PointTrails[PTR_REYE]->Tick(fTimeDelta);
	
}

void CGolem::Control_LeftEyeTrail(_float fTimeDelta)
{
	
	_matrix HeadMatrix;

	HeadMatrix = m_Sockets[SOCKET_HEAD]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();

	_float4 LEyePos;

	_vector vTemp = HeadMatrix.r[3];
	_vector vRight = HeadMatrix.r[0];
	_vector vUp = HeadMatrix.r[1];
	_vector vLook = HeadMatrix.r[2];

	vRight = XMVector3Normalize(vRight) * 0.21f;
	vTemp += vRight;

	vUp = XMVector3Normalize(vUp) * 0.41f;
	vTemp += vUp; //앞뒤

	vLook = XMVector3Normalize(vLook) * 0.17f;
	vTemp -= vLook; //좌우

	XMStoreFloat4(&LEyePos, vTemp);


	m_PointTrails[PRT_LEYE]->TrailOn();
	m_PointTrails[PRT_LEYE]->Add_Point(LEyePos);
	m_PointTrails[PRT_LEYE]->Tick(fTimeDelta);
}

void CGolem::Create_Item()
{
	CDropItem::DROPITEMDESC DropItemDesc;
	DropItemDesc.iItemType = CDropItem::TYPE_CLUE;
	DropItemDesc.iNaviNum = GI->Get_Cur_LevelIndex();
	XMStoreFloat4(&DropItemDesc.vStartPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	GI->Add_GameObjectToLayer(TEXT("DropItem"), GI->Get_Cur_LevelIndex(), TEXT("Monster_DropItem"), &DropItemDesc);
}

void CGolem::Check_MonsterLight(_float fTimeDelta)
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

void CGolem::LookAtPlayer(_float fLiearTime)
{
	Set_Dir();
	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&m_vTargetLook), fLiearTime);

}

HRESULT CGolem::Ready_Components()
{
	//m_pEmissiveCom
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_GolemA_Emi"), TEXT("Com_TextureEmi0"), (CComponent**)&m_pEmissiveCom[0])))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_GolemB_Emi"), TEXT("Com_TextureEmi1"), (CComponent**)&m_pEmissiveCom[1])))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Golem"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	m_pModelCom->Set_RootHierarchy(5);

	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(2.f, 2.f, 2.f);
	ColDesc.vCenter = _float3(0.f, 0.6f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Push_Range"); // 밀어내기 위함

	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(3.f, 3.f, 3.f);
	ColDesc.vCenter = _float3(0.f, 1.5f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Attack"); //범위공격

	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	//상체쪽
	CHierarchyNode*		pChest = m_pModelCom->Get_HierarchyNode("Chest_M");
	if (nullptr == pChest)
		return E_FAIL;

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(3.3f, 3.3f, 3.3f);
	ColDesc.vCenter = _float3(0.f, 0.f, -0.7f);
	ColDesc.pSocket = pChest;
	XMStoreFloat4x4(&ColDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	ColDesc.pOwner = this;
	ColDesc.iFlag = 0;
	strcpy(ColDesc.sTag, "Monster_Body1"); // 기본 충돌체 - 몬스터끼리 밀어내고 플레이어 또는 플레이어 공격이랑 충돌하기위한거

	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	//오른손
	CHierarchyNode* pWeapon = m_pModelCom->Get_HierarchyNode("Weapon_M");
	if (nullptr == pWeapon)
		return E_FAIL;
	ColDesc.vSize = _float3(2.4f, 3.8f, 7.2f);
	ColDesc.vCenter = _float3(0.f, -0.2f, -2.4f);
	ColDesc.pSocket = pWeapon;
	ColDesc.iFlag = 0;
	XMStoreFloat4x4(&ColDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "Monster_WeaponHead"); //몬스터 공격용 충돌체
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;

	m_Sockets.push_back(pWeapon);

	return S_OK;
}

HRESULT CGolem::Create_Navigation(_uint iNaviNum)
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
	}

	_int iNaviIndex = m_pNavigationCom->Find_NaviIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pNavigationCom->Set_NaviIndex(iNaviIndex);

	return S_OK;
}

HRESULT CGolem::Ready_Layer_UI(const _tchar * pLayerTag)
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

void CGolem::Set_PreBar(_float fTimeDelta)
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

CGolem * CGolem::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CGolem*		pInstance = new CGolem(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CGolem"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CGolem::Clone(void * pArg)
{
	CGolem*		pInstance = new CGolem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CGolem"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGolem::Free()
{
	__super::Free();

	for (auto& iter : m_PointTrails)
		Safe_Release(iter);

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pDissolveTexture);
	for (_uint i = 0; 2 > i; ++i)
	{
		Safe_Release(m_pEmissiveCom[i]);
	}
}
