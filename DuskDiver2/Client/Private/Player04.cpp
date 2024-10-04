#include "stdafx.h"
#include "..\Public\Player04.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Player_Manager.h"
#include "DataManager.h"
#include "ToolManager.h"
#include "Parts.h"
#include "Status_Manager.h"
#include "Particle_Manager.h"
#include "Player04VoidEffect_2.h"
#include "Time_Manager.h"
#include "Player04Hair.h"
#include "Player04HairBattle.h"
#include "UI_Manager.h"
#include "Camera_Manager.h"
#include "Player01HitEffect_1.h"
#include "Player01HitEffect_2.h"
#include "Wall.h"
#include "Wind.h"
#include "Player01Trail_1.h"
#include "EffectSpin.h"
#include "Tornado.h"
#include "ViewAlpha.h"
#include "PointTrail.h"
#include "SwordTrail.h"
#include "PointTrailObj.h"
#include "SwordTrailObj.h"
#include "Player04MotionTrail.h"
#include "Player04Slash.h"
#include "Player01Wind1.h"
#include "Player01Wind2.h"
#include "Player01Ball.h"
#include "Player01Ring.h"
#include "Player01Pillar.h"
#include "Player01Pillar2.h"
#include "Player01Pillar3.h"
#include "Player01Skill5Ball.h"
#include "CircleDistortion.h"
#include "Player01Rage1Ball.h"
#include "Player01SmokeBall.h"
#include "Player01Metamor.h"
#include "Player01Landing.h"
#include "Player01Upper.h"
#include "Player04Slash2.h"
#include "Player04Sword1.h"
#include "Player04SmokeBall.h"
#include "Player04DarkBall.h"
#include "TornadoSide.h"
#include "Player04Aura.h"
#include "Player04WhiteBall.h"
#include "Player04Rage1Particle.h"
#include "Effect_Manager.h"
#include "Player04Col.h"
#include "Player04VoidParticle.h"

CPlayer04::CPlayer04(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CPlayer04::CPlayer04(const CPlayer04 & rhs)
	: CCreture(rhs)
{
}

HRESULT CPlayer04::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer04::Initialize(void * pArg)
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Player";
	// *[HeroJho]: ImGui로 디버깅을 하려면 꼭 호출해줘야 합니다.
	Make_ImGui();

	m_pModelCom->Set_AnimIndex(IDLE1);
	m_eCurState = IDLE1;
	m_eNextState = IDLE1;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 25.f, 1.f));

	// *[HeroJho]: 현재 내가 서있는 네비 인덱스를 자동으로 구해줍니다. 비용이 비싸기 때문에 Tick과 같은 반복 호출 금지입니다!
	_int iNaviIndex = m_pNavigationCom->Find_NaviIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pNavigationCom->Set_NaviIndex(iNaviIndex);

	m_fHurtTime = 0.2f;

	Ready_Sockets();
	Ready_PlayerParts();
	Ready_Trails();

	PM->Add_Player(this);
	STM->Set_FinalHp(CStatus_Manager::PLAYER04);
	m_fMaxHp = STM->Get_FinalHp(CStatus_Manager::PLAYER04);
	m_fNowHp = m_fMaxHp;
	STM->Set_FinalStr(CStatus_Manager::PLAYER04);

	((CPlayer04Hair*)m_Parts[PARTS_HAIR])->Set_Sleep();
	((CPlayer04HairBattle*)m_Parts[PARTS_HAIRBATTLE])->Set_Sleep();
	Get_Colliders("Player_HairCut")->Set_Sleep();

	((CPlayer04Hair*)m_Parts[PARTS_HAIR])->Set_Sleep();
	((CPlayer04HairBattle*)m_Parts[PARTS_HAIRBATTLE])->Set_Sleep();
	Get_Colliders("Player_HairCut")->Set_Sleep();

	m_fPreHp = m_fNowHp;
	m_fMaxSp = 100.f;
	m_fNowSP = m_fMaxSp;
	m_fPreSp = m_fNowSP;
	m_fMaxEp = 100.f;
	m_fNowEP = 100.f;
	m_fPreEp = m_fNowEP;

	

	_matrix  RHandMatrix = m_Sockets[SOCKET_WEAPON_R]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
	m_SwordTrails[STR_SWORD]->TrailOn(RHandMatrix);

	return S_OK;
}

void CPlayer04::Tick(_float fTimeDelta)
{
	if (m_bSelectPlayer && PM->Get_PlayerTick())
	{

		
		TIM->Add_TimeObj(CTime_Manager::OBJ_PLAYER, this);

		Check_SP(fTimeDelta);

		Check_Hurt(fTimeDelta);

		Check_Rim(fTimeDelta);

		Check_Burst(fTimeDelta * m_fTMRatio, fTimeDelta);

		fTimeDelta *= m_fTMRatio;

		Emp_Col();

		Get_KeyInput(fTimeDelta);

		Update(fTimeDelta);

		Check_HP(fTimeDelta);
		
		Check_PlayerLight(fTimeDelta);	
		
		
		

		Create_MotionTrail(fTimeDelta);
		Create_SwordTrail(fTimeDelta);		
		Create_PointTrail(fTimeDelta);
					
	}
	
}

void CPlayer04::LateTick(_float fTimeDelta)
{
	if (m_bSelectPlayer && PM->Get_PlayerTick())
	{
		if (nullptr == m_pRendererCom)
			return;

		fTimeDelta *= m_fTMRatio;

		LateTick_Func(fTimeDelta);


	}
}

HRESULT CPlayer04::Render()
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

	// For.Blur
	_float fBlurPower = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
		return E_FAIL;

	_float fGama = 1.6f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;

	switch (m_eShaderPass)
	{
	case Client::CPlayer04::SHADER_DEFAULT:
		if (FAILED(Set_RenderPass(SHADER_DEFAULT)))
			return E_FAIL;
		break;
	case Client::CPlayer04::SHADER_RIM:
		if (FAILED(Set_RenderPass(SHADER_RIM)))
			return E_FAIL;
		break;
	case Client::CPlayer04::SHADER_LIGHT:
		if (FAILED(Set_RenderPass(SHADER_LIGHT)))
			return E_FAIL;
		break;
	}
		
	return S_OK;
}

// *[HeroJho]: 디버깅 창입니다. ImGui로 작동됩니다.
void CPlayer04::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	//IG->Debug_Transform(m_pTransformCom);
	//m_Parts[PARTS_SWORD]->Render_Debug();

	_float3 vSize = Get_Colliders("Player_Attack")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Player_Attack")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Change_ColRange(vSize, vCenter);

	//ImGui::DragFloat("Time", &m_fBurstParticleLimit, 0.001f, 0.001f, 3.f, "%.3f");

	ImGui::End();


}

// *[HeroJho]: 충돌이 발생할 때 호출되는 함수입니다.
void CPlayer04::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Monster" == Desc.pOther->Get_Tag())
	{
		// 내 OBB와 상대편의 OBB와 충돌 했다면

		if ((!strcmp("Player_Attack", Desc.pMyDesc->sTag) &&
			!strcmp("Monster_Body1", Desc.pOtherDesc->sTag)))
		{
			m_bRangeCol = false;
			((CCreture*)Desc.pOther)->On_Hit(this);
			Check_BP(TM->Get_TimeDelta() * m_fTMRatio);
			CRM->Get_Cam()->Shake(0.05f, 0.15f);

			if (m_eHE == HE_RAGE1)
				Desc.pOther->Fix_Timer(0.f, 20.f);

		}


		

	}
	else if ("Tag_Boss" == Desc.pOther->Get_Tag())
	{
		m_fHurtTime = 0.6f;

		if (!strcmp("Player_Push_Range", Desc.pMyDesc->sTag) &&
			!strcmp("Monster_Push_Range", Desc.pOtherDesc->sTag))
		{
			//밀어내기
			m_pTransformCom->PushMeXZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION), Desc.pMyDesc->vSize.x, XMLoadFloat4(&((CCreture*)Desc.pOther)->Get_WorldPos()), Desc.pOtherDesc->vSize.x, TM->Get_TimeDelta(), m_pNavigationCom);
			Update_Parts();
		}

		if ((!strcmp("Player_Attack", Desc.pMyDesc->sTag) &&
			!strcmp("Monster_Body1", Desc.pOtherDesc->sTag)))
		{
			
			((CCreture*)Desc.pOther)->On_Hit(this);
			Check_BP(TM->Get_TimeDelta() * m_fTMRatio);
		}

		if ((!strcmp("Player_Attack", Desc.pMyDesc->sTag) &&
			!strcmp("Monster_Body2", Desc.pOtherDesc->sTag)))
		{

			((CCreture*)Desc.pOther)->On_Hit(this);
			Check_BP(TM->Get_TimeDelta() * m_fTMRatio);
		}

		if ((!strcmp("Player_Attack", Desc.pMyDesc->sTag) &&
			!strcmp("Monster_Body3", Desc.pOtherDesc->sTag)))
		{

			((CCreture*)Desc.pOther)->On_Hit(this);
			Check_BP(TM->Get_TimeDelta() * m_fTMRatio);
		}
	}
}

void CPlayer04::On_Hit(CCreture * pOther)
{

	if (m_bHurt && !m_bReHp && !Get_PreUp())
	{
		GI->PlaySoundW(L"SwordHit.ogg", SD_PLAYER4_SK, 0.5f);

		_uint iRandomVoice = GI->Get_Random(0, 6);
		switch (iRandomVoice)
		{
		case 0:
			GI->PlaySoundW(L"Hero05_ba_09.wav", SD_PLAYER4_VO, 0.6f);
			break;
		case 1:
			GI->PlaySoundW(L"Hero05_ba_10.wav", SD_PLAYER4_VO, 0.6f);
			break;
		case 2:
			GI->PlaySoundW(L"Hero05_ba_11.wav", SD_PLAYER4_VO, 0.6f);
			break;
		case 3:
			GI->PlaySoundW(L"Hero05_ba_12.wav", SD_PLAYER4_VO, 0.6f);
			break;
		case 4:
			GI->PlaySoundW(L"Hero05_ba_13.wav", SD_PLAYER4_VO, 0.6f);
			break;
		case 5:
			GI->PlaySoundW(L"Hero05_ba_14.wav", SD_PLAYER4_VO, 0.6f);
			break;
		case 6:
			GI->PlaySoundW(L"Hero05_ba_15.wav", SD_PLAYER4_VO, 0.6f);
			break;
		}

		Hit_Effect(pOther);

		Hit_State(pOther);

		m_bHurt = false;
	}
}

void CPlayer04::Create_HitEffect(_float4 vWorldPos)
{
	switch (m_eHE)
	{
	case Client::CPlayer04::HE_1:
		Create_HE_1(vWorldPos);
		break;

	case Client::CPlayer04::HE_RAGE1:
		Create_HE_RAGE1(vWorldPos);
		break;

	}
}

_float4 CPlayer04::Get_PlayerPos()
{
	_float4 PlayerPos;
	XMStoreFloat4(&PlayerPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	return PlayerPos;
}

void CPlayer04::Change_PlayerPos(_float4 vChanges)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMLoadFloat4(&vChanges));
}

void CPlayer04::Set_ChangeLook(DIR eDir, _float3 vDir)
{
	m_eDir = eDir;
	m_vTargetLook = vDir;
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&vDir));
}

_int CPlayer04::Get_NaviIndex()
{
	return m_pNavigationCom->Get_CurCellIndex();
}

void CPlayer04::Set_NaviIndex(_int iNaviIndex)
{
	m_pNavigationCom->Set_NaviIndex(iNaviIndex);
}

void CPlayer04::Reset_Animation()
{
	Reset_Collider();
	if (m_ePlayerMode == PMODE_NORMAL)
	{
		m_pModelCom->Set_AnimIndex(IDLE1);
		m_eCurState = IDLE1;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
	}
	else
	{
		m_pModelCom->Set_AnimIndex(IDLE2);
		m_eCurState = IDLE2;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
	}
	m_bVoid = false;
}

HRESULT CPlayer04::Change_Navigation(_uint iNaviNum)
{
	if (m_pNavigationCom == nullptr)
		return E_FAIL;	

	Safe_Release(m_pNavigationCom);

	CNavigation::NAVIGATIONDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = 0;

	switch (iNaviNum)
	{
	case 1:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage1Navigation"), TEXT("Stage1"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, -10.f, 1.f));
		m_vTargetLook = _float3{ 0.f,0.f,1.f };
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&m_vTargetLook));
		break;
	case 2:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage2Navigation"), TEXT("Stage2"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-29.3f, 0.f, 14.f, 1.f));
		m_vTargetLook = _float3{ 0.f,0.f,-1.f };
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&m_vTargetLook));
		break;
	case 3:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage3Navigation"), TEXT("Stage3"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(8.05f, 1.19f, -1.62f, 1.f));
		m_vTargetLook = _float3{ 1.f,0.f,0.f };
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&m_vTargetLook));
		break;
	case 4:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage4Navigation"), TEXT("Stage4"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(1.f, 0.f, 1.f, 1.f));
		m_vTargetLook = _float3{ 1.f,0.f,0.f };
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&m_vTargetLook));
		break;
	case 5:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage5Navigation"), TEXT("Stage5"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, -30.f, 1.f));
		m_vTargetLook = _float3{ 1.f,0.f,0.f };
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&m_vTargetLook));
		break;
	}

	_int iNaviIndex = m_pNavigationCom->Find_NaviIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pNavigationCom->Set_NaviIndex(iNaviIndex);

	Update_Parts();
	
	if (m_ePlayerMode == PMODE_BURST)
	{
		((CPlayer04HairBattle*)m_Parts[PARTS_HAIRBATTLE])->SetInitPos();
	}
	else
	{
		((CPlayer04Hair*)m_Parts[PARTS_HAIR])->SetInitPos();
	}

	return S_OK;
}

void CPlayer04::Change_Tag()
{
	Set_State(STEPBACK);
}

void CPlayer04::Create_TagEffect()
{
	CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
	CWall::WALLINFO* Info = new CWall::WALLINFO;
	Info->vSize = _float3{ 0.15f,6.f,0.15f };
	XMStoreFloat4(&Info->vWorldPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	Info->vWorldPos.y += 10.f;
	Info->vColor = _float4{ 0.6f,0.4f,1.f,1.f };
	Info->vColor2 = _float4{ 0.f,0.1f,1.f,1.f };
	Info->vRatio = _float2{ 1.f,1.f };
	Info->vUVFixSpeed = _float2{ 0.f,-0.5f };
	Info->vFixPosSpeed = _float3{ 0.f,-40.f,0.f };
	GI->Add_GameObjectToLayer(L"Wall", GI->Get_Cur_LevelIndex(), L"Layer_PlayerEffect", &Info);
}

void CPlayer04::Change_HairCut(_bool bOnOff)
{
	if (bOnOff)
	{
		Update_Parts();
		Get_Colliders("Player_HairCut")->Set_Awake();
		if (m_ePlayerMode == PMODE_BURST)
		{
			((CPlayer04HairBattle*)m_Parts[PARTS_HAIRBATTLE])->Set_Wake();
			((CPlayer04HairBattle*)m_Parts[PARTS_HAIRBATTLE])->SetInitPos();
		}
		else
		{
			((CPlayer04Hair*)m_Parts[PARTS_HAIR])->Set_Wake();
			((CPlayer04Hair*)m_Parts[PARTS_HAIR])->SetInitPos();
		}
		
	}
	
}

_bool CPlayer04::Get_BurstMode()
{
	if (m_ePlayerMode == PMODE_BURST)
		return true;
	else
		return false;
}

_bool CPlayer04::Get_IsIdle()
{
	if (m_eCurState == IDLE1 || m_eCurState == IDLE2)
		return true;

	return false;
}

_bool CPlayer04::Get_PreUp()
{
	if (m_fPreHp < m_fNowHp)
		return true;

	return false;
}

_bool CPlayer04::Get_PreDown()
{
	if (m_fPreHp > m_fNowHp)
		return true;

	return false;
}

void CPlayer04::Set_State(STATE eState)
{
	if (m_eNextState == eState)
		return;

	m_eNextState = eState;

	switch (m_eNextState)
	{
	case Client::CPlayer04::RUN:
		m_fRunSpeed = 5.f;
		break;
	case Client::CPlayer04::WALK:
		break;
	case Client::CPlayer04::AIRATTACK1:
		m_bAirAttack1Col = false;
		m_eHitType = TYPE_HIT;
		m_fAirPower = 1.5f;
		m_fPushPower = 1.f;
		Change_ColRange(_float3{ 1.5f,2.f,1.5f }, _float3{ 0.f,1.f,1.5f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER04) * 0.6f;
		break;
	case Client::CPlayer04::AIRATTACK2:
		m_bAirAttack2Col = false;
		m_eHitType = TYPE_HIT;
		m_fAirPower = 1.5f;
		m_fPushPower = 1.f;
		break;
	case Client::CPlayer04::AIRATTACK3:
		m_bAirAttack3Col = false;
		m_eHitType = TYPE_HIT;
		m_fAirPower = 1.5f;
		m_fPushPower = 1.f;
		break;
	case Client::CPlayer04::ATTACK1:
		m_bAttack1Col = false;
		m_eHitType = TYPE_HIT;
		m_fPushPower = 3.f;
		m_fAirPower = 0.f;
		Change_ColRange(_float3{ 1.5f,1.5f,1.5f }, _float3{ 0.f,1.f,1.5f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER04) * 0.6f;
		break;
	case Client::CPlayer04::ATTACK2:
		m_bAttack2Col = false;
		m_eHitType = TYPE_HIT;
		m_fPushPower = 3.f;
		m_fAirPower = 0.f;
		break;
	case Client::CPlayer04::ATTACK3:
		m_bAttack3Col = false;
		m_eHitType = TYPE_SPIN;
		m_fPushPower = 2.f;
		m_fAirPower = 5.f;
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER04) * 0.8f;
		break;
	case Client::CPlayer04::ATTACK4:
		m_bAttack4Col = false;
		m_eHitType = TYPE_UP;
		m_fPushPower = 3.f;
		m_fAirPower = 6.f;
		break;
	case Client::CPlayer04::ATTACK5:
		m_bAttack5Col = false;
		m_eHitType = TYPE_AWAY;
		m_fPushPower = 30.f;
		m_fAirPower = 1.f;
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER04) * 1.5f;
		break;
	case Client::CPlayer04::BOUNCE:
		m_bVoid = false;
		m_pTransformCom->ResetAndJump(1.f);
		break;
	case Client::CPlayer04::BROKEN:
		break;
	case Client::CPlayer04::BURST:
		m_bNormalChange = false;
		break;
	case Client::CPlayer04::DASH:
		break;
	case Client::CPlayer04::GROGGYEND:
		break;
	case Client::CPlayer04::GROGGYLOOF:
		break;
	case Client::CPlayer04::GROGGYSTART:
		Die_Sound();
		m_bVoid = true;
		break;
	case Client::CPlayer04::DOWN:
		break;
	case Client::CPlayer04::VOIDFRONT:
		break;
	case Client::CPlayer04::VOIDAIR:
		break;
	case Client::CPlayer04::VOIDATTACK:

		break;
	case Client::CPlayer04::FINALBURST:
		GI->PlaySoundW(L"UI_BP_full.wav", SD_UI, 0.3f);
		GI->PlaySoundW(L"Hero05_ba_54.wav", SD_PLAYER4_VO, 1.f);
		m_bBurstChange = false;
		break;
	case Client::CPlayer04::FINISHNORMAL:
		break;
	case Client::CPlayer04::FINISHWALL:
		break;
	case Client::CPlayer04::GETUP:
		m_bVoid = true;
		break;
	case Client::CPlayer04::SKILL1:
	{
		m_bSkill1Col = false;
		m_bSkill1TrailOn = false;
		m_eHitType = TYPE_HIT;
		m_fPushPower = 5.f;
		m_fAirPower = 1.f;
		Change_ColRange(_float3{ 2.f,2.f,2.5f }, _float3{ 0.f,2.f,2.5f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER04) * 1.4f;
		
	}
		break;
	case Client::CPlayer04::HURTBACK:
		Reset_Collider();
		break;
	case Client::CPlayer04::HURTFLY:
		break;
	case Client::CPlayer04::HURTFRONT:
		Reset_Collider();
		break;
	case Client::CPlayer04::IDLE1:
		m_bSwordRender = false;
		break;
	case Client::CPlayer04::IDLE2:
		m_bSwordRender = false;
		break;
	case Client::CPlayer04::JUMPBACK:
		break;
	case Client::CPlayer04::JUMPLOOF:
		break;
	case Client::CPlayer04::JUMPSTART:
		GI->PlaySoundW(L"se_heels_landing_asphalt_01.wav", SD_PLAYER4_SK, 0.8f);
		GI->PlaySoundW(L"Hero05_ba_16.wav", SD_PLAYER4_VO, 1.f);
		break;
	case Client::CPlayer04::JUMPEND:
		break;
	case Client::CPlayer04::QICKSTAND:
		break;
	case Client::CPlayer04::SKILL2:
		GI->PlaySoundW(L"se_ba_skill.wav", SD_PLAYER4_SK, 0.6f);
		m_bSkill2Col_1 = false;
		m_bSkill2Col_2 = false;
		m_eHitType = TYPE_SPIN;
		m_fAirPower = 6.f;
		m_fPushPower = 4.f;
		Change_ColRange(_float3{ 1.5f,1.2f,2.f }, _float3{ 0.f,1.2f,2.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER04) * 1.2f;
		break;
	case Client::CPlayer04::SKILL3START:
		GI->PlaySoundW(L"se_HE05_Specific_1.wav", SD_PLAYER4_SK, 0.8f);
		m_bSkill3StartCol = false;
		Input_Direction();
		m_bSkill3Jump = false;
		m_bSkill3TickPerSecond = false;
		m_eHitType = TYPE_FASTDOWN;
		m_fAirPower = -3.f;
		m_fPushPower = 0.f;
		Change_ColRange(_float3{ 1.5f,2.f,1.5f }, _float3{ 0.f,0.5f,1.5f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER04) * 0.6f;
		m_bSkill3Jump = true;
		Create_Particle(PT_VOID);
		PTM->Create_InstanceParticle(L"Player04VoidDistortion", m_pTransformCom->Get_WorldFloat4x4());
		m_bRenderPlayer = false;
		break;
	case Client::CPlayer04::SKILL3LOOF:
		GI->PlaySoundW(L"se_HE05_Specific_2.wav", SD_PLAYER4_SK, 0.8f);
		m_bSkill3LoofCol = false;
		m_eHitType = TYPE_HIT;
		m_fAirPower = -3.f;
		m_fPushPower = 0.f;
		Change_ColRange(_float3{ 1.5f,2.f,1.5f }, _float3{ 0.f,0.5f,1.5f });
		break;
	case Client::CPlayer04::SKILL3END:
		m_bSkill3EndCol = false;
		m_eHitType = TYPE_HIT;
		m_fAirPower = 0.f;
		m_fPushPower = 0.f;
		break;
	case Client::CPlayer04::SKILL4START:
		Input_Direction();
		GI->PlaySoundW(L"se_ba_skill.wav", SD_PLAYER4_SK, 0.6f);
		m_bSkill4Change = false;
		m_bSkill4StartCol = false;
		m_bSkill4EndCol = false;
		m_eHitType = TYPE_SPIN;
		m_fAirPower = 5.f;
		m_fPushPower = 3.f;
		Change_ColRange(_float3{ 2.f,2.f,2.5f }, _float3{ 0.f,2.f,2.5f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER04) * 1.2f;
		Create_Effect(ET_DASH);
		break;
	case Client::CPlayer04::SKILL4END:
		m_bSwordRender = false;
		break;
	case Client::CPlayer04::SKILL5:
		GI->PlaySoundW(L"se_ba_skill.wav", SD_PLAYER4_SK, 0.6f);
		m_bSkill5Col_1 = false;	m_bSkill5Col_2 = false;	m_bSkill5Col_3 = false;	m_bSkill5Col_4 = false;
		m_bSkill5Sound = false;
		m_eHitType = TYPE_HIT;
		m_fPushPower = 1.5f;
		m_fAirPower = 0.f;
		Change_ColRange(_float3{ 1.5f,1.2f,2.f }, _float3{ 0.f,1.2f,2.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER04) * 0.4f;
		break;
	case Client::CPlayer04::SKILL6:
		m_bSwordRender = false;
		m_bSkill6Col = false;
		m_bSkill6Effect = false;
		m_eHitType = TYPE_SPIN;
		m_fAirPower = 5.f;
		m_fPushPower = 3.f;
		Change_ColRange(_float3{ 7.f,3.f,6.f }, _float3{ 0.f,3.f,6.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER04) * 1.6f;
		break;
	case Client::CPlayer04::SKILL7START:
		//GI->PlaySoundW(L"se_HE05_Skill06_2.wav", SD_PLAYER4_SK, 0.8f); // 도약 시작할 때 넣고 지우기
		//GI->PlaySoundW(L"Hero05_ba_03.wav", SD_PLAYER4_VO, 1.f);
		GI->PlaySoundW(L"se_HE05_Specific_1.wav", SD_PLAYER4_SK, 0.8f);
		m_bSkill7Change = false;
		Change_ColRange(_float3{ 2.f,2.f,2.5f }, _float3{ 0.f,2.f,2.5f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER04) * 1.6f;
		Create_Particle(PT_VOID);
		PTM->Create_InstanceParticle(L"Player04VoidDistortion", m_pTransformCom->Get_WorldFloat4x4());
		m_bRenderPlayer = false;
		break;
	case Client::CPlayer04::SKILL7END:
		break;
	case Client::CPlayer04::SKILL8:
		GI->PlaySoundW(L"se_HE05_Specific_1.wav", SD_PLAYER4_SK, 0.8f);
		Create_Particle(PT_VOID);
		PTM->Create_InstanceParticle(L"Player04VoidDistortion", m_pTransformCom->Get_WorldFloat4x4());
		m_bRenderPlayer = false;
		m_bSkill8Effect = false;
		break;
	case Client::CPlayer04::SLAPDOWN:
		break;
	case Client::CPlayer04::SKILL9:
		Input_Direction();
		m_bSkill9Col = false;
		m_eHitType = TYPE_AWAY;
		m_fPushPower = 20.f;
		m_fAirPower = 6.f;
		Change_ColRange(_float3{ 1.5f,2.f,2.f }, _float3{ 0.f,2.f,2.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER04);
		break;
	case Client::CPlayer04::SKILL10:
		m_bSkill10Col = false;
		m_eHitType = TYPE_UP;
		m_fPushPower = 5.f;
		m_fAirPower = 12.f;
		Change_ColRange(_float3{ 1.5f,2.f,2.f }, _float3{ 0.f,2.f,2.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER04) * 0.6f;
		break;
	case Client::CPlayer04::SKILL11START:
		GI->PlaySoundW(L"Hero05_ba_18.wav", SD_PLAYER4_VO, 1.f);
		m_bSkill11EndCol = false;
		m_bSkill11Charge1 = false;
		m_bSkill11Charge2 = false;
		m_bSkill11Charge3 = false;
		m_bSkill11Charge4 = false;
		m_bSkill11PlayerLight = false;
		m_bSkill11ChargeEnd = false;
		m_fPlayerLightLimit = 0.15f;
		m_fSkill11ChargeEndAcc = 0.f;
		m_iChargeCount = 0;
		m_fChargeAcc = 0.f;
		m_eHitType = TYPE_SPIN;
		m_fPushPower = 3.f;
		m_fAirPower = 5.f;
		Change_ColRange(_float3{ 2.f,2.f,2.5f }, _float3{ 0.f,2.f,2.5f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER04) * 3.f;
		break;
	case Client::CPlayer04::SKILL11LOOF:
		break;
	case Client::CPlayer04::SKILL11END:
		break;
	case Client::CPlayer04::SKILL12START:
		m_bSkill12StartCol = false;
		m_bSkill12EndCol = false;
		m_eHitType = TYPE_HIT;
		m_fPushPower = 4.f;
		m_fAirPower = 0.f;
		Change_ColRange(_float3{ 2.f,2.f,2.5f }, _float3{ 0.f,2.f,2.5f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER04) * 0.8f;
		break;
	case Client::CPlayer04::SKILL12END:
		break;
	case Client::CPlayer04::SKILL13START:
		break;
	case Client::CPlayer04::SKILL13END:
		m_bSkill13End_Col = false;
		m_eHitType = TYPE_HIT;
		m_fAirPower = 3.f;
		m_fPushPower = 1.f;
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER04);
		break;
	case Client::CPlayer04::RUNFASTEND:
		break;
	case Client::CPlayer04::RUNFASTLOOF:
		break;
	case Client::CPlayer04::RUNFASTSTART:
		GI->PlaySoundW(L"UI_Dash.wav", SD_PLAYER4_SK, 0.4f);
		m_fRunSpeed = 8.f;
		Create_Effect(ET_DASH);
		break;
	case Client::CPlayer04::STEPBACK:
		m_bVoid = true;
		Get_Colliders("Player_HairCut")->Set_Sleep();
		m_vTargetLook = GI->Get_CamDir(CPipeLine::DIR_LOOK);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&m_vTargetLook));
		break;
	case Client::CPlayer04::WALKLEFT:
		break;
	case Client::CPlayer04::WALKRIGHT:
		break;
	case Client::CPlayer04::STUN:
		break;
	case Client::CPlayer04::STUNEND:
		break;
	case Client::CPlayer04::RAGESKILL1:
		UM->Set_FrontUI(false);
		GI->PlaySoundW(L"UI_Burst_st.wav", SD_UI, 0.3f);
		GI->PlaySoundW(L"Hero05_ba_56.wav", SD_PLAYER4_VO, 1.f);
		GI->PlaySoundW(L"se_HE05_Super01_1.wav", SD_PLAYER4_SK, 0.8f);
		Change_ColRange(_float3{ 12.f,4.f,16.f }, _float3{ 0.f,4.f,10.f });
		m_bRageSkill1Effect = false;
		m_bRageSkill1Effect2 = false;
		m_bBurstSkillOn = true;
		m_bVoid = true;
		m_bPatternSkill = true;
		m_bHeroTest_1 = false;
		m_bHeroTest_2 = false;
		m_bHeroTest_3 = false;

		break;
	case Client::CPlayer04::RAGESKILL2:
		GI->PlaySoundW(L"Hero05_ba_58.wav", SD_PLAYER4_VO, 1.f); //타이밍 잡아야함
		m_bBurstSkillOn = true;
		m_bVoid = false;
		m_bSwordRender = true;
		break;
	case Client::CPlayer04::FALLDOWN:
		break;
	case Client::CPlayer04::HITAWAY:
		break;
	case Client::CPlayer04::HITSPIN:
		Reset_Collider();
		break;
	case Client::CPlayer04::HITDOWN:
		Reset_Collider();
		break;
	case Client::CPlayer04::WALKBACK:
		Reset_Collider();
		break;
	}

	m_pModelCom->SetNextIndex(m_eNextState);
	m_pModelCom->SetChangeBool(true);
	m_eCurState = m_eNextState;

}

void CPlayer04::Set_Dir(DIR eDir)
{
	m_eDir = eDir;

	_matrix View = XMLoadFloat4x4(&GI->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW));
	_vector vCamLook = View.r[2];

	_vector vCamRight = XMVector3Normalize(XMVector3Cross(_vector{ 0.f,1.f,0.f }, vCamLook));

	vCamLook = XMVector3Normalize(XMVector3Cross(vCamRight, _vector{ 0.f,1.f,0.f }));

	switch (m_eDir)
	{
	case Client::CPlayer04::DIR_UP:
		XMStoreFloat3(&m_vTargetLook, (vCamLook));
		break;
	case Client::CPlayer04::DIR_DOWN:
		XMStoreFloat3(&m_vTargetLook, (vCamLook * -1.f));
		break;
	case Client::CPlayer04::DIR_RIGHT:
		XMStoreFloat3(&m_vTargetLook, (vCamRight));
		break;
	case Client::CPlayer04::DIR_LEFT:
		XMStoreFloat3(&m_vTargetLook, (vCamRight * -1.f));
		break;
	case Client::CPlayer04::DIR_LU:
		XMStoreFloat3(&m_vTargetLook, XMVector3Normalize(vCamLook - vCamRight));
		break;
	case Client::CPlayer04::DIR_RU:
		XMStoreFloat3(&m_vTargetLook, XMVector3Normalize(vCamLook + vCamRight));
		break;
	case Client::CPlayer04::DIR_LD:
		XMStoreFloat3(&m_vTargetLook, XMVector3Normalize(vCamLook * -1.f + vCamRight * -1.f));
		break;
	case Client::CPlayer04::DIR_RD:
		XMStoreFloat3(&m_vTargetLook, XMVector3Normalize(vCamLook * -1.f + vCamRight));
		break;
	}
}

void CPlayer04::End_Animation()
{
	if (m_pModelCom->GetAniEnd())
	{
		switch (m_eCurState)
		{
		case Client::CPlayer04::RUN:
			break;
		case Client::CPlayer04::WALK:
			break;
		case Client::CPlayer04::AIRATTACK1:
			if (!m_pTransformCom->Get_Air())
				Set_State(JUMPEND);
			else
				Set_State(JUMPLOOF);
			break;
		case Client::CPlayer04::AIRATTACK2:
			if (!m_pTransformCom->Get_Air())
				Set_State(JUMPEND);
			else
				Set_State(JUMPLOOF);
			break;
		case Client::CPlayer04::AIRATTACK3:
			if (!m_pTransformCom->Get_Air())
				Set_State(JUMPEND);
			else
				Set_State(JUMPLOOF);
			break;
		case Client::CPlayer04::ATTACK1:
			Set_State(IDLE2);
			break;
		case Client::CPlayer04::ATTACK2:
			Set_State(IDLE2);
			break;
		case Client::CPlayer04::ATTACK3:
			Set_State(IDLE2);
			break;
		case Client::CPlayer04::ATTACK4:
			Set_State(IDLE2);
			break;
		case Client::CPlayer04::ATTACK5:
			Set_State(IDLE2);
			break;
		case Client::CPlayer04::BOUNCE:
			Set_State(GETUP);
			break;
		case Client::CPlayer04::BROKEN:
			break;
		case Client::CPlayer04::BURST:
			Set_State(IDLE2);
			break;
		case Client::CPlayer04::DASH:
			break;
		case Client::CPlayer04::GROGGYEND:
			break;
		case Client::CPlayer04::GROGGYLOOF:
			break;
		case Client::CPlayer04::GROGGYSTART:
			Player_Die();
			break;
		case Client::CPlayer04::DOWN:
			break;
		case Client::CPlayer04::VOIDFRONT:
			GI->PlaySoundW(L"se_heels_landing_asphalt_01.wav", SD_PLAYER4_SK, 0.8f);
			m_bVoidOn = false;
			m_bVoid = false;
			Set_State(IDLE2);
			break;
		case Client::CPlayer04::VOIDAIR:
			break;
		case Client::CPlayer04::VOIDATTACK:
			Set_State(IDLE2);
			break;
		case Client::CPlayer04::FINALBURST:
			Set_State(IDLE2);
			m_vPlayerLightColor = { 1.f,1.f,1.f };
			((CPlayer04Hair*)m_Parts[PARTS_HAIR])->Set_PlayerLightColor(_float3{ 1.f,1.f,1.f });
			((CPlayer04HairBattle*)m_Parts[PARTS_HAIRBATTLE])->Set_PlayerLightColor(_float3{ 1.f,1.f,1.f });
			break;
		case Client::CPlayer04::FINISHNORMAL:
			break;
		case Client::CPlayer04::FINISHWALL:
			break;
		case Client::CPlayer04::GETUP:
			m_bVoid = false;
			Set_State(IDLE2);
			break;
		case Client::CPlayer04::SKILL1:
			m_bSkill1On = false;
			Set_State(IDLE2);
			break;
		case Client::CPlayer04::HURTBACK:
			Set_State(IDLE2);
			break;
		case Client::CPlayer04::HURTFLY:
			break;
		case Client::CPlayer04::HURTFRONT:
			Set_State(IDLE2);
			break;
		case Client::CPlayer04::IDLE1:
			break;
		case Client::CPlayer04::IDLE2:
			break;
		case Client::CPlayer04::JUMPBACK:
			break;
		case Client::CPlayer04::JUMPLOOF:
			break;
		case Client::CPlayer04::JUMPSTART:
			m_pModelCom->Set_AnimIndex(JUMPLOOF);
			m_eCurState = JUMPLOOF;
			break;
		case Client::CPlayer04::JUMPEND:
			GI->PlaySoundW(L"se_heels_landing_asphalt_02.wav", SD_PLAYER4_SK, 0.8f);
			Set_State(IDLE1);
			break;
		case Client::CPlayer04::QICKSTAND:
			break;
		case Client::CPlayer04::SKILL2:
			Set_State(IDLE2);
			m_ePushType = PUSH;
			break;
		case Client::CPlayer04::SKILL3START:
		{
			m_bSkill3LoofCol = false;
			m_eHitType = TYPE_HIT;
			m_fAirPower = -3.f;
			m_fPushPower = 0.f;
			m_pModelCom->Set_AnimIndex(SKILL3LOOF);
			m_eCurState = SKILL3LOOF;
			m_eNextState = m_eCurState;
			m_pModelCom->SetNextIndex(m_eNextState);
			m_pTransformCom->ResetAndJump(-20.f);
			Change_ColRange(_float3{ 1.5f,2.f,1.5f }, _float3{ 0.f,0.5f,1.5f });
			CPlayer04Slash2::SLASHINFO* Info = new CPlayer04Slash2::SLASHINFO;
			Info->vScale = { 0.8f,3.f,0.8f };
			Info->fBlurPower = 1.f;
			Info->fLimitTime = 2.f;
			Info->fTurnSpeed = -42.f;
			Info->vColor = { 0.f,0.f,0.f,1.f };
			Info->vColor2 = { 0.f,0.f,0.f, 1.f };
			Info->vRatio = { 0.8f,0.8f };
			Info->vRatioSpeed = { 5.f,5.f };
			Info->vScaleSpeed = { 0.f,0.f,0.f };
			Info->fAngle = 300.f;
			Info->Worldmatrix = m_pTransformCom->Get_WorldFloat4x4();
			GI->Add_GameObjectToLayer(L"Player04Slash2", TM->Get_CurLevel(), L"Layer_Player04Effect", &Info);
		}
			break;
		case Client::CPlayer04::SKILL3LOOF:
			m_bSkill3LoofCol = false;
			break;
		case Client::CPlayer04::SKILL3END:
			m_bSkill3On = false;
			Set_State(IDLE2);
			m_bSkill3EndCol = false;
			break;
		case Client::CPlayer04::SKILL4START:
			Set_State(IDLE2);
			break;
		case Client::CPlayer04::SKILL4END:
			Set_State(IDLE2);
			break;
		case Client::CPlayer04::SKILL5:
			Set_State(IDLE2);
			break;
		case Client::CPlayer04::SKILL6:
			Set_State(IDLE2);
			m_ePushType = PUSH;
			break;
		case Client::CPlayer04::SKILL7START:
			break;
		case Client::CPlayer04::SKILL7END:
			Set_State(IDLE2);
			break;
		case Client::CPlayer04::SKILL8:
			Set_State(IDLE2);
			m_bVoid = false;
			break;
		case Client::CPlayer04::SLAPDOWN:
			break;
		case Client::CPlayer04::SKILL9:
			Set_State(IDLE2);
			break;
		case Client::CPlayer04::SKILL10:
			Set_State(IDLE2);
			break;
		case Client::CPlayer04::SKILL11START:
			m_pModelCom->Set_AnimIndex(SKILL11LOOF);
			m_eCurState = SKILL11LOOF;
			m_eNextState = m_eCurState;
			m_pModelCom->SetNextIndex(m_eNextState);
			break;
		case Client::CPlayer04::SKILL11LOOF:
			break;
		case Client::CPlayer04::SKILL11END:
			Set_State(IDLE2);
			break;
		case Client::CPlayer04::SKILL12START:
			break;
		case Client::CPlayer04::SKILL12END:
			Set_State(IDLE2);
			break;
		case Client::CPlayer04::SKILL13START:
			m_pModelCom->Set_AnimIndex(SKILL13END);
			m_eCurState = SKILL13END;
			m_eNextState = m_eCurState;
			m_pModelCom->SetNextIndex(m_eNextState);
			break;
		case Client::CPlayer04::SKILL13END:
			Set_State(JUMPLOOF);
			m_fJumpSpeed = 0.f;
			break;
		case Client::CPlayer04::RUNFASTEND:
			if (m_ePlayerMode == PMODE_NORMAL)
				Set_State(IDLE1);
			else
				Set_State(IDLE2);
			break;
		case Client::CPlayer04::RUNFASTLOOF:
			break;
		case Client::CPlayer04::RUNFASTSTART:
			m_pModelCom->Set_AnimIndex(RUNFASTLOOF);
			m_eCurState = RUNFASTLOOF;
			break;
		case Client::CPlayer04::STEPBACK:
			m_bSelectPlayer = false;
			((CPlayer04Hair*)m_Parts[PARTS_HAIR])->Set_Sleep();
			((CPlayer04Hair*)m_Parts[PARTS_HAIRBATTLE])->Set_Sleep();
			break;
		case Client::CPlayer04::WALKLEFT:
			break;
		case Client::CPlayer04::WALKRIGHT:
			break;
		case Client::CPlayer04::STUN:
			break;
		case Client::CPlayer04::STUNEND:
			break;
		case Client::CPlayer04::RAGESKILL1:
			m_bRageSkill1On = false;
			m_bBurstSkillOn = false;
			m_bRangeSkill1On = false;
			m_bVoid = false;
			m_bRenderPlayer = true;
			m_bPatternSkill = false;
			m_bPointTrail = true;
			m_fPlayerLightLimit = 0.1f;
			m_bPlayerLight = true;
			Create_LightEffect();
			Create_Effect(ET_VOIDEND);
			m_eHE = HE_1;
			Set_State(IDLE2);
			UM->Set_FrontUI(true);
			break;
		case Client::CPlayer04::RAGESKILL2:
			UM->Set_FrontUI(true);
			m_bBurstSkillOn = false;
			m_bVoid = false;
			Set_State(IDLE2);
			break;
		case Client::CPlayer04::FALLDOWN:
			break;
		case Client::CPlayer04::HITAWAY:
			Set_State(FALLDOWN);
			break;
		case Client::CPlayer04::HITSPIN:
			Set_State(FALLDOWN);
			break;
		case Client::CPlayer04::HITDOWN:
			m_pModelCom->Set_AnimIndex(BOUNCE);
			m_eCurState = BOUNCE;
			m_eNextState = m_eCurState;
			m_pModelCom->SetNextIndex(m_eNextState);
			break;
		case Client::CPlayer04::WALKBACK:
			break;
		}
	}
}

void CPlayer04::Get_KeyInput(_float fTimeDelta)
{

	m_bDirKeyInput = false;
	if (!m_pModelCom->GetChangeBool())
	{
		Cancle_KeyInput(fTimeDelta);
		switch (m_eCurState)
		{
		case Client::CPlayer04::RUN:
			Run_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer04::WALK:
			break;
		case Client::CPlayer04::AIRATTACK1:
			AirAttack1_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer04::AIRATTACK2:
			AirAttack2_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer04::AIRATTACK3:
			AirAttack3_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer04::ATTACK1:
			Attack1_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer04::ATTACK2:
			Attack2_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer04::ATTACK3:
			Attack3_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer04::ATTACK4:
			Attack4_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer04::ATTACK5:
			Attack5_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer04::BOUNCE:
			break;
		case Client::CPlayer04::BROKEN:
			break;
		case Client::CPlayer04::BURST:
			break;
		case Client::CPlayer04::DASH:
			break;
		case Client::CPlayer04::GROGGYEND:
			break;
		case Client::CPlayer04::GROGGYLOOF:
			break;
		case Client::CPlayer04::GROGGYSTART:
			break;
		case Client::CPlayer04::DOWN:
			break;
		case Client::CPlayer04::VOIDFRONT:
			break;
		case Client::CPlayer04::VOIDAIR:
			break;
		case Client::CPlayer04::VOIDATTACK:
			break;
		case Client::CPlayer04::FINALBURST:
			break;
		case Client::CPlayer04::FINISHNORMAL:
			break;
		case Client::CPlayer04::FINISHWALL:
			break;
		case Client::CPlayer04::GETUP:
			break;
		case Client::CPlayer04::SKILL1:
			break;
		case Client::CPlayer04::HURTBACK:
			break;
		case Client::CPlayer04::HURTFLY:
			break;
		case Client::CPlayer04::HURTFRONT:
			break;
		case Client::CPlayer04::IDLE1:
			Idle_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer04::IDLE2:
			Idle_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer04::JUMPBACK:
			break;
		case Client::CPlayer04::JUMPLOOF:
			JumpLoof_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer04::JUMPSTART:
			JumpStart_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer04::JUMPEND:
			break;
		case Client::CPlayer04::QICKSTAND:
			break;
		case Client::CPlayer04::SKILL2:
			Skill2_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer04::SKILL3START:
			break;
		case Client::CPlayer04::SKILL3LOOF:
			break;
		case Client::CPlayer04::SKILL3END:
			break;
		case Client::CPlayer04::SKILL4START:
			break;
		case Client::CPlayer04::SKILL4END:
			break;
		case Client::CPlayer04::SKILL5:
			Skill5_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer04::SKILL6:
			break;
		case Client::CPlayer04::SKILL7START:
			break;
		case Client::CPlayer04::SKILL7END:
			break;
		case Client::CPlayer04::SKILL8:
			break;
		case Client::CPlayer04::SLAPDOWN:
			break;
		case Client::CPlayer04::SKILL9:
			Skill9_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer04::SKILL10:
			Skill10_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer04::SKILL11START:
			break;
		case Client::CPlayer04::SKILL11LOOF:
			Skill11Loof_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer04::SKILL11END:
			break;
		case Client::CPlayer04::SKILL12START:
			break;
		case Client::CPlayer04::SKILL12END:
			break;
		case Client::CPlayer04::SKILL13START:
			break;
		case Client::CPlayer04::SKILL13END:
			Skill13End_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer04::RUNFASTEND:
			break;
		case Client::CPlayer04::RUNFASTLOOF:
			Run_FastInput(fTimeDelta);
			break;
		case Client::CPlayer04::RUNFASTSTART:
			Run_FastStartInput(fTimeDelta);
			break;
		case Client::CPlayer04::STEPBACK:
			break;
		case Client::CPlayer04::WALKLEFT:
			break;
		case Client::CPlayer04::WALKRIGHT:
			break;
		case Client::CPlayer04::STUN:
			break;
		case Client::CPlayer04::STUNEND:
			break;
		case Client::CPlayer04::RAGESKILL1:
			break;
		case Client::CPlayer04::RAGESKILL2:
			break;
		case Client::CPlayer04::FALLDOWN:
			break;
		case Client::CPlayer04::HITAWAY:
			break;
		case Client::CPlayer04::HITSPIN:
			break;
		case Client::CPlayer04::HITDOWN:
			break;
		case Client::CPlayer04::WALKBACK:
			break;
		}		
	}

}

void CPlayer04::Update(_float fTimeDelta)
{
	m_bRangeCol = true;

	if (!m_pModelCom->GetChangeBool())
	{
		switch (m_eCurState)
		{
		case Client::CPlayer04::RUN:
			Run_Update(fTimeDelta);			
			break;
		case Client::CPlayer04::WALK:
			Walk_Update(fTimeDelta);
			break;
		case Client::CPlayer04::AIRATTACK1:
			AirAttack1_Update(fTimeDelta);
			break;
		case Client::CPlayer04::AIRATTACK2:
			AirAttack2_Update(fTimeDelta);
			break;
		case Client::CPlayer04::AIRATTACK3:
			AirAttack3_Update(fTimeDelta);
			break;
		case Client::CPlayer04::ATTACK1:
			Attack1_Update(fTimeDelta);
			break;
		case Client::CPlayer04::ATTACK2:
			Attack2_Update(fTimeDelta);
			break;
		case Client::CPlayer04::ATTACK3:
			Attack3_Update(fTimeDelta);
			break;
		case Client::CPlayer04::ATTACK4:
			Attack4_Update(fTimeDelta);
			break;
		case Client::CPlayer04::ATTACK5:
			Attack5_Update(fTimeDelta);
			break;
		case Client::CPlayer04::BOUNCE:
			Bounce_Update(fTimeDelta);
			break;
		case Client::CPlayer04::BROKEN:
			Broken_Update(fTimeDelta);
			break;
		case Client::CPlayer04::BURST:
			Burst_Update(fTimeDelta);
			break;
		case Client::CPlayer04::DASH:
			Dash_Update(fTimeDelta);
			break;
		case Client::CPlayer04::GROGGYEND:
			GroggyEnd_Update(fTimeDelta);
			break;
		case Client::CPlayer04::GROGGYLOOF:
			GroggyLoof_Update(fTimeDelta);
			break;
		case Client::CPlayer04::GROGGYSTART:
			GroggyStart_Update(fTimeDelta);
			break;
		case Client::CPlayer04::DOWN:
			Down_Update(fTimeDelta);
			break;
		case Client::CPlayer04::VOIDFRONT:
			VoidFront_Update(fTimeDelta);			
			break;
		case Client::CPlayer04::VOIDAIR:
			VoidAir_Update(fTimeDelta);
			break;
		case Client::CPlayer04::VOIDATTACK:
			VoidAttack_Update(fTimeDelta);
			break;
		case Client::CPlayer04::FINALBURST:
			FinalBurst_Update(fTimeDelta);		
			break;
		case Client::CPlayer04::FINISHNORMAL:
			FinishNormal_Update(fTimeDelta);
			break;
		case Client::CPlayer04::FINISHWALL:
			FinishWall_Update(fTimeDelta);
			break;
		case Client::CPlayer04::GETUP:
			GetUp_Update(fTimeDelta);
			break;
		case Client::CPlayer04::SKILL1:
			Skill1_Update(fTimeDelta);
			break;
		case Client::CPlayer04::HURTBACK:
			HurtBack_Update(fTimeDelta);
			break;
		case Client::CPlayer04::HURTFLY:
			HurtFly_Update(fTimeDelta);
			break;
		case Client::CPlayer04::HURTFRONT:
			HurtFront_Update(fTimeDelta);
			break;
		case Client::CPlayer04::IDLE1:
			Idle1_Update(fTimeDelta);
			break;
		case Client::CPlayer04::IDLE2:
			Idle2_Update(fTimeDelta);
			break;
		case Client::CPlayer04::JUMPBACK:
			JumpBack_Update(fTimeDelta);
			break;
		case Client::CPlayer04::JUMPLOOF:
			JumpLoof_Update(fTimeDelta);
			break;
		case Client::CPlayer04::JUMPSTART:
			JumpStart_Update(fTimeDelta);
			break;
		case Client::CPlayer04::JUMPEND:
			JumpEnd_Update(fTimeDelta);
			break;
		case Client::CPlayer04::QICKSTAND:
			QickStand_Update(fTimeDelta);
			break;
		case Client::CPlayer04::SKILL2:
			Skill2_Update(fTimeDelta);
			break;
		case Client::CPlayer04::SKILL3START:
			Skill3Start_Update(fTimeDelta);
			break;
		case Client::CPlayer04::SKILL3LOOF:
			Skill3Loof_Update(fTimeDelta);
			break;
		case Client::CPlayer04::SKILL3END:
			Skill3End_Update(fTimeDelta);
			break;
		case Client::CPlayer04::SKILL4START:
			Skill4Start_Update(fTimeDelta);
			break;
		case Client::CPlayer04::SKILL4END:
			Skill4End_Update(fTimeDelta);			
			break;
		case Client::CPlayer04::SKILL5:
			Skill5_Update(fTimeDelta);
			break;
		case Client::CPlayer04::SKILL6:
			Skill6_Update(fTimeDelta);			
			break;
		case Client::CPlayer04::SKILL7START:
			Skill7Start_Update(fTimeDelta);			
			break;
		case Client::CPlayer04::SKILL7END:
			Skill7End_Update(fTimeDelta);			
			break;
		case Client::CPlayer04::SKILL8:
			Skill8_Update(fTimeDelta);
			break;
		case Client::CPlayer04::SLAPDOWN:
			SlapDown_Update(fTimeDelta);
			break;
		case Client::CPlayer04::SKILL9:
			Skill9_Update(fTimeDelta);
			break;
		case Client::CPlayer04::SKILL10:
			Skill10_Update(fTimeDelta);
			break;
		case Client::CPlayer04::SKILL11START:
			Skill11Start_Update(fTimeDelta);
			break;
		case Client::CPlayer04::SKILL11LOOF:
			Skill11Loof_Update(fTimeDelta);
			break;
		case Client::CPlayer04::SKILL11END:
			Skill11End_Update(fTimeDelta);			
			break;
		case Client::CPlayer04::SKILL12START:
			Skill12Start_Update(fTimeDelta);			
			break;
		case Client::CPlayer04::SKILL12END:
			Skill12End_Update(fTimeDelta);			
			break;
		case Client::CPlayer04::SKILL13START:
			Skill13Start_Update(fTimeDelta);
			break;
		case Client::CPlayer04::SKILL13END:
			Skill13End_Update(fTimeDelta);			
			break;
		case Client::CPlayer04::RUNFASTEND:
			RunFastEnd_Update(fTimeDelta);
			break;
		case Client::CPlayer04::RUNFASTLOOF:
			RunFastLoof_Update(fTimeDelta);			
			break;
		case Client::CPlayer04::RUNFASTSTART:
			RunFastStart_Update(fTimeDelta);
			break;
		case Client::CPlayer04::STEPBACK:
			StepBack_Update(fTimeDelta);
			break;
		case Client::CPlayer04::WALKLEFT:
			break;
		case Client::CPlayer04::WALKRIGHT:
			break;
		case Client::CPlayer04::STUN:
			Stun_Update(fTimeDelta);
			break;
		case Client::CPlayer04::STUNEND:
			StunEnd_Update(fTimeDelta);
			break;
		case Client::CPlayer04::RAGESKILL1:
			RageSkill1_Update(fTimeDelta);			
			break;
		case Client::CPlayer04::RAGESKILL2:
			RageSkill2_Update(fTimeDelta);			
			break;
		case Client::CPlayer04::FALLDOWN:
			FallDown_Update(fTimeDelta);
			break;
		case Client::CPlayer04::HITAWAY:
			HitAway_Update(fTimeDelta);
			break;
		case Client::CPlayer04::HITSPIN:
			HitSpin_Update(fTimeDelta);
			break;
		case Client::CPlayer04::HITDOWN:
			HitDown_Update(fTimeDelta);
			break;
		case Client::CPlayer04::WALKBACK:
			break;
		}
	}
}

void CPlayer04::Change_Mode()
{
	if (K_DOWN(DIK_J))
		Set_State(BURST);

	if (K_DOWN(DIK_K))
	{
		Set_State(FINALBURST);
	}

}

void CPlayer04::LateTick_Func(_float fTimeDelta)
{
	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&m_vTargetLook), 0.2f);

	m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom, m_pNavigationCom);

	m_pTransformCom->Tick_Gravity(fTimeDelta, m_pNavigationCom, m_fGravity, true);

	Update_Parts();

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	End_Animation();

	Push_Colliders();


	if (m_bSwordTrail)
	{
		for (auto& iter : m_SwordTrails)
			iter->LateTick(fTimeDelta);
	}

	if (m_bPointTrail)
	{
		for (auto& iter : m_PointTrails)
			iter->LateTick(fTimeDelta);
	}

	if (m_bRenderPlayer)
	{
		for (auto& iter : m_MotionTrails)
			iter->LateTick(fTimeDelta);
		Render_Parts(fTimeDelta);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}
	Render_Col(m_pRendererCom);
}

void CPlayer04::Push_Colliders()
{
	if (m_bAttackCol)
		Choose_Col("Player_Attack");
	
	if (m_bHurt && !m_bVoid)
		Choose_Col("Player_Body");

	if(m_bRangeCol)
		Choose_Col("Player_Push_Range");
}

void CPlayer04::Change_ColRange(_float3 vSize, _float3 vCenter)
{
	Get_Colliders("Player_Attack")->Set_Size(vSize);
	Get_Colliders("Player_Attack")->Set_Center(vCenter);
}

void CPlayer04::Hit_State(CCreture * pOther)
{
	m_fPushPower = pOther->Get_PushPower();
	m_fAirPower = pOther->Get_AirPower();
	m_fAirPower = 8.f;
	Reset_Collider();

	m_fNowHp -= (pOther->Get_Damage() - STM->Get_FinalStamina(CStatus_Manager::PLAYER04));
	if (m_fNowHp <= 0.f)
	{
		m_fNowHp = 0.f;
		Set_State(GROGGYSTART);
		return;
	}

	m_bSwordRender = false;
	m_eShaderPass = SHADER_RIM;
	m_bRim = true;

	UM->Set_ShakeOn3(true);

	switch (pOther->Get_HitType())
	{
	case TYPE_HIT:
		if (m_pTransformCom->Get_Air())
		{
			CRM->Get_Cam()->Shake((m_fPushPower * 0.01f + m_fAirPower * 0.01f), 0.2f);
			
			return;
		}

		if (m_eCurState != HURTFLY)
		{
			if (m_pTransformCom->Check_TargetFront(XMLoadFloat4(&pOther->Get_WorldPos())))
				Set_State(HURTFRONT);
			else
				Set_State(HURTBACK);

			CRM->Get_Cam()->Shake(0.06f, 0.15f);
		}
		
		break;

	case TYPE_AWAY:
	{
		switch (pOther->Get_PushType())
		{
		case PUSH:
		{
			_vector vMonDir = XMVector3Normalize(XMLoadFloat3(&(pOther)->Get_Dir(_float3(0.f, 0.f, 1.f))));
			XMStoreFloat3(&m_vTargetLook, vMonDir * -1.f);
			XMStoreFloat3(&m_vPushLook, vMonDir);
		}
		break;
		case PULL:
		{
			_vector vMonDir = XMVector3Normalize(XMLoadFloat3(&(pOther)->Get_Dir(_float3(0.f, 0.f, 1.f))));
			XMStoreFloat3(&m_vTargetLook, vMonDir * -1.f);
			XMStoreFloat3(&m_vPushLook, vMonDir * -1.f);
		}
		break;
		case TARGET:
		{
			_vector vMonDir = XMVector3Normalize(XMLoadFloat3(&(pOther)->Get_Dir(_float3(0.f, 0.f, 1.f))));
			XMStoreFloat3(&m_vTargetLook, vMonDir * -1.f);
			XMStoreFloat3(&m_vPushLook, XMVector3Normalize(XMLoadFloat3(&pOther->Get_PushLook())));
		}
		break;
		}

		CRM->Get_Cam()->Shake((m_fPushPower * 0.01f + m_fAirPower * 0.01f), 0.2f);
		Set_State(HITAWAY);
		m_pTransformCom->ResetAndJump(m_fAirPower);

	}
	break;

	case TYPE_UP:
		Set_State(HITDOWN);
		break;

	case TYPE_SPIN:
	{
		switch (pOther->Get_PushType())
		{
		case PUSH:
		{
			_vector vMonDir = XMVector3Normalize(XMLoadFloat3(&(pOther)->Get_Dir(_float3(0.f, 0.f, 1.f))));
			XMStoreFloat3(&m_vTargetLook, vMonDir * -1.f);
			XMStoreFloat3(&m_vPushLook, vMonDir);
		}
		break;
		case PULL:
		{
			_vector vMonDir = XMVector3Normalize(XMLoadFloat3(&(pOther)->Get_Dir(_float3(0.f, 0.f, 1.f))));
			XMStoreFloat3(&m_vTargetLook, vMonDir * -1.f);
			XMStoreFloat3(&m_vPushLook, vMonDir * -1.f);
		}
		break;
		case TARGET:
		{
			_vector vMonDir = XMVector3Normalize(XMLoadFloat3(&(pOther)->Get_Dir(_float3(0.f, 0.f, 1.f))));
			XMStoreFloat3(&m_vTargetLook, vMonDir * -1.f);
			XMStoreFloat3(&m_vPushLook, XMVector3Normalize(XMLoadFloat3(&pOther->Get_PushLook())));
		}
		break;
		}

		CRM->Get_Cam()->Shake((m_fPushPower * 0.01f + m_fAirPower * 0.01f), 0.2f);
		Set_State(HITSPIN);
		m_pTransformCom->ResetAndJump(m_fAirPower);

	}
	break;
	}
}

void CPlayer04::Hit_Effect(CCreture * pOther)
{
	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	vPos.y += 1.f;
	pOther->Create_HitEffect(vPos);
}

void CPlayer04::Reset_Collider()
{
	m_bSwordRender = false;
	m_bSwordTrail = false;
	m_bAttackCol = false;
	m_bAirAttack1Col = false;
	m_bAirAttack2Col = false;
	m_bAirAttack3Col = false;
	m_bSkill13End_Col = false;
	m_bAttack1Col = false;
	m_bAttack2Col = false;
	m_bAttack3Col = false;
	m_bAttack4Col = false;
	m_bAttack5Col = false;
	m_bSkill3StartCol = false;
	m_bSkill3LoofCol = false;
	m_bSkill3EndCol = false;
	m_bVoidAttackCol_1 = false;
	m_bVoidAttackCol_2 = false;
	m_bVoidAttackCol_3 = false;
	m_bVoidAttackCol_4 = false;
	m_bVoidAttackCol_5 = false;
	m_bSkill1Col = false;
	m_bSkill2Col_1 = false;
	m_bSkill2Col_2 = false;
	m_bSkill4StartCol = false;
	m_bSkill4EndCol = false;
	m_bSkill5Col_1 = false;
	m_bSkill5Col_2 = false;
	m_bSkill5Col_3 = false;
	m_bSkill5Col_4 = false;
	m_bSkill6Col = false;
	m_bSkill7EndCol = false;
	m_bSkill9Col = false;
	m_bSkill10Col = false;
	m_bSkill11EndCol = false;
	m_bSkill12StartCol = false;
	m_bSkill12EndCol = false;
	m_bSwordTrail = false;
	m_bSkill3StartCol = false;
	m_bSkill3Jump = false;
	m_bSkill3On = false;
	m_bSkill3TickPerSecond = false;
}

void CPlayer04::Check_Burst(_float fTimeDelta, _float OriginTimeDelta)
{
	if (m_fPreEp < m_fNowEP)
	{
		m_fPreEp += 10.f * fTimeDelta;

		if (m_fPreEp >= m_fNowEP)
		{
			m_fPreEp = m_fNowEP;
		}
	}

	if (m_fPreEp == m_fNowEP)
	{
		if (m_ePlayerMode == PMODE_BURST)
		{
			if (m_bRenderPlayer)
			{
				m_fBurstParticleAcc += 1.f * OriginTimeDelta;
				if (m_fBurstParticleAcc >= m_fBurstParticleLimit)
				{
					Create_ParticleInstance(PTI_BURST);
					m_fBurstParticleAcc = 0.f;
				}
			}

			m_bMotionTrail = true;
			m_fNowEP -= 0.5f * fTimeDelta;
			m_fPreEp -= 0.5f * fTimeDelta;

			if (m_fNowEP <= 0.f)
			{
				m_fNowEP = 0.f;
				m_fPreEp = 0.f;
				m_fNowBP = 0.f;
				m_ePlayerMode = PMODE_BATTLE;

				Update_Parts();
				((CPlayer04Hair*)m_Parts[PARTS_HAIR])->Set_Wake();
				((CPlayer04Hair*)m_Parts[PARTS_HAIR])->SetInitPos();
				((CPlayer04HairBattle*)m_Parts[PARTS_HAIRBATTLE])->Set_Sleep();
				m_fBurstParticleAcc = 0.f;
				m_bPointTrail = false;
			}
		}
		else if (m_fNowEP < m_fMaxEp)
		{
			m_fNowEP += 5.f * fTimeDelta;
			m_fPreEp += 5.f * fTimeDelta;

			if (m_fNowEP >= m_fMaxEp)
			{
				m_fNowEP = m_fMaxEp;
				m_fPreEp = m_fMaxEp;
			}
		}
	}
}

void CPlayer04::Check_Rim(_float fTimeDelta)
{
	if (m_bRim)
	{
		m_fRimAcc += 1.f * fTimeDelta;
		if (m_fRimAcc >= 0.2f)
		{
			m_fRimAcc = 0.f;
			m_bRim = false;
			m_eShaderPass = SHADER_DEFAULT;
		}
	}
}

HRESULT CPlayer04::Set_RenderPass(SHADER eShader)
{
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	switch (eShader)
	{
	case Client::CPlayer04::SHADER_DEFAULT:
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
				return E_FAIL;
		}
		break;
	case Client::CPlayer04::SHADER_RIM:
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pShaderCom->Set_RawValue("g_CamPosition", &GI->Get_CamPosition(), sizeof(_float4))))
				return E_FAIL;
			_float g_RimPower = 1.5f;
			if (FAILED(m_pShaderCom->Set_RawValue("g_RimPower", &g_RimPower, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 1)))
				return E_FAIL;
		}
		break;
	case Client::CPlayer04::SHADER_LIGHT:
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			_float Alpha = 1.f;
			if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &Alpha, sizeof(_float))))
				return E_FAIL;

			_float Blur = 1.f;
			if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &Blur, sizeof(_float))))
				return E_FAIL;
			
			if (FAILED(m_pShaderCom->Set_RawValue("g_vPlayerLightColor", &m_vPlayerLightColor, sizeof(_float3))))
				return E_FAIL;

			if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 11)))
				return E_FAIL;
		}
	}
	}
	

	return S_OK;
}

void CPlayer04::Check_HP(_float fTimeDelta)
{
	if (m_bReHp)
	{
		m_fNowHp += 300.f * fTimeDelta;
		m_fPreHp += 300.f * fTimeDelta;

		if (m_fPreHp >= STM->Get_FinalHp(CStatus_Manager::PLAYER04))
		{
			m_fPreHp = STM->Get_FinalHp(CStatus_Manager::PLAYER04);
			m_fNowHp = m_fPreHp;
			m_bReHp = false;
		}
	}

	if (m_fPreHp < m_fNowHp)
	{
		m_fPreHp += 400.f * fTimeDelta;

		if (m_fPreHp >= m_fNowHp)
		{
			m_fPreHp = m_fNowHp;
		}
	}

	else if (m_fPreHp > m_fNowHp)
	{
		m_fPreHp -= 200.f * fTimeDelta;

		if (m_fPreHp <= m_fNowHp)
		{
			m_fPreHp = m_fNowHp;
		}
	}
}

void CPlayer04::Check_SP(_float fTimeDelta)
{
	if (m_fPreSp < m_fNowSP)
	{
		m_fPreSp += 15.f * fTimeDelta;

		if (m_fPreSp >= m_fNowSP)
		{
			m_fPreSp = m_fNowSP;
		}
	}

	if (!m_bSPUse)
	{
		if (CK_DOWN(DIK_X))
		{
			m_bSPUse = true;
			Set_SlowMotionTrail();
			return;
		}

		if (m_fPreSp == m_fNowSP)
		{
			m_fNowSP += 10.f * (fTimeDelta);
			m_fPreSp += 10.f * (fTimeDelta);
			if (m_fNowSP > m_fMaxSp)
			{
				m_fNowSP = m_fMaxSp;
				m_fPreSp = m_fMaxSp;
			}
		}
	}
	else
	{
		TIM->Fix_All(0.2f, 100.f);
		if (CK_DOWN(DIK_X))
		{
			m_bSPUse = false;
			TIM->Reset_All();
			Reset_SlowMotionTrail();
			return;
		}

		if (m_fPreSp == m_fNowSP)
		{
			m_bMotionTrail = true;
			m_fNowSP -= 10.f * fTimeDelta;
			m_fPreSp -= 10.f * fTimeDelta;
			if (m_fNowSP < 0.f)
			{
				m_bSPUse = false;
				m_fNowSP = 0.f;
				m_fPreSp = 0.f;
				TIM->Reset_All();
				Reset_SlowMotionTrail();
			}
		}
	}
}

void CPlayer04::Check_BP(_float fTimeDelta)
{
	if (m_ePlayerMode == PMODE_BURST && !m_bBurstSkillOn)
	{
		m_fNowBP += 5.f;

		if (m_fNowBP > 100.f)
			m_fNowBP = 100.f;
	}
}

HRESULT CPlayer04::Create_Effect(EFFECT eET)
{
	switch (eET)
	{
	case Client::CPlayer04::ET_DASH:
	{
		CWind::WINDINFO* Info = new CWind::WINDINFO;
		Info->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
		Info->vFixPosSpeed = { 0.f,0.f,-3.f };
		Info->vFixScaleSpeed = { 60.f,60.f,60.f };
		Info->vRatio = { 1.f,1.f };
		Info->vRatioSpeed = { 2.f,2.f };
		Info->vUVFixSpeed = { 1.f,-3.f };
		_matrix Effectmatrix;
		Effectmatrix.r[0] = { 0.1f, 0.f, 0.f, 0.f };
		Effectmatrix.r[1] = { 0.f, 0.f, 1.f, 0.f };
		Effectmatrix.r[2] = { 0.f, -1.f, 0.f, 0.f };
		Effectmatrix.r[3] = { 0.f, 1.f, 1.f, 1.f };
		XMStoreFloat4x4(&Info->WorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		if (FAILED(GI->Add_GameObjectToLayer(L"Wind", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
			return E_FAIL;

		CWind::WINDINFO* Info2 = new CWind::WINDINFO;
		Info2->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info2->vColor2 = { 0.7f,0.7f,0.7f,1.f };
		Info2->vFixPosSpeed = { 0.f,0.f,-5.f };
		Info2->vFixScaleSpeed = { 80.f,80.f,80.f };
		Info2->vRatio = { 1.f,1.f };
		Info2->vRatioSpeed = { 2.f,2.f };
		Info2->vUVFixSpeed = { 1.f,-3.f };
		Effectmatrix.r[0] = { 0.1f, 0.f, 0.f, 0.f };
		Effectmatrix.r[1] = { 0.f, 0.f, 1.f, 0.f };
		Effectmatrix.r[2] = { 0.f, -1.f, 0.f, 0.f };
		Effectmatrix.r[3] = { 0.f, 1.f, 1.f, 1.f };
		XMStoreFloat4x4(&Info2->WorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		if (FAILED(GI->Add_GameObjectToLayer(L"Wind", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info2)))
			return E_FAIL;

		break;
	}

	case Client::CPlayer04::ET_ATTACK1:
	{
		CPlayer01Trail_1::TRAILINFO* Info = new CPlayer01Trail_1::TRAILINFO;
		Info->vColor = { 0.2f,0.2f,0.8f,1.f };
		Info->vColor2 = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 5.f,1.f };
		Info->vRatioSpeed = { 30.f,0.f };
		_matrix Effectmatrix;
		Effectmatrix.r[0] = { -3.83f, -3.21f, 0.f, 0.f };
		Effectmatrix.r[1] = { -3.21f, 3.83f, 0.f, 0.f };
		Effectmatrix.r[2] = { 0.f, 0.f, -5.f, 0.f };
		Effectmatrix.r[3] = { -0.11f, 0.79f, 0.7f, 1.f };
		XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		Info->fBlurPower = 0.2f;
		if (FAILED(GI->Add_GameObjectToLayer(L"Player01Trail_1", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
			return E_FAIL;

		break;
	}
	case Client::CPlayer04::ET_ATTACK2:
	{
		CPlayer01Trail_1::TRAILINFO* Info = new CPlayer01Trail_1::TRAILINFO;
		Info->vColor = { 0.2f,0.2f,0.8f,1.f };
		Info->vColor2 = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 5.f,1.f };
		Info->vRatioSpeed = { 30.f,0.f };
		_matrix Effectmatrix;
		Effectmatrix.r[0] = { 3.21f, 3.83f, 0.f, 0.f };
		Effectmatrix.r[1] = { 3.83f, -3.21f, 0.f, 0.f };
		Effectmatrix.r[2] = { 0.f, 0.f, -5.f, 0.f };
		Effectmatrix.r[3] = { 0.02f, 1.f, 0.7f, 1.f };
		XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		Info->fBlurPower = 0.2f;
		if (FAILED(GI->Add_GameObjectToLayer(L"Player01Trail_1", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
			return E_FAIL;

		break;
	}
	case Client::CPlayer04::ET_ATTACK3:
	{
		CPlayer01Trail_1::TRAILINFO* Info = new CPlayer01Trail_1::TRAILINFO;
		Info->vColor = { 0.2f,0.2f,0.8f,1.f };
		Info->vColor2 = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 5.f,1.f };
		Info->vRatioSpeed = { 30.f,0.f };
		_matrix Effectmatrix;
		Effectmatrix.r[0] = { 8.f, -0.f, 0.f, 0.f };
		Effectmatrix.r[1] = { 0.f, -7.51f, 2.73f, 0.f };
		Effectmatrix.r[2] = { 0.f, -2.73f, -7.51f, 0.f };
		Effectmatrix.r[3] = { 0.f, 1.3f, 1.2f, 1.f };
		XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		Info->fBlurPower = 0.2f;
		if (FAILED(GI->Add_GameObjectToLayer(L"Player01Trail_1", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
			return E_FAIL;

		break;
	}
	case Client::CPlayer04::ET_ATTACK4:
	{
		CPlayer01Trail_1::TRAILINFO* Info = new CPlayer01Trail_1::TRAILINFO;
		Info->vColor = { 0.2f,0.2f,0.8f,1.f };
		Info->vColor2 = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 5.f,1.f };
		Info->vRatioSpeed = { 30.f,0.f };
		_matrix Effectmatrix;
		Effectmatrix.r[0] = { 0.f, 8.f, 0.f, 0.f };
		Effectmatrix.r[1] = { 8.f, 0.f, 0.f, 0.f };
		Effectmatrix.r[2] = { 0.f, 0.f, -8.f, 0.f };
		Effectmatrix.r[3] = { 0.f, 0.97f, 0.7f, 1.f };
		XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		Info->fBlurPower = 0.2f;
		if (FAILED(GI->Add_GameObjectToLayer(L"Player01Trail_1", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
			return E_FAIL;

		break;
	}

	case Client::CPlayer04::ET_ATTACK5:
	{
		CWind::WINDINFO* Info = new CWind::WINDINFO;
		Info->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
		Info->vFixPosSpeed = { 0.f,0.f,3.f };
		Info->vFixScaleSpeed = { 30.f,30.f,30.f };
		Info->vRatio = { 1.f,1.f };
		Info->vRatioSpeed = { 2.f,2.f };
		Info->vUVFixSpeed = { 1.f,-3.f };
		_matrix Effectmatrix;
		Effectmatrix.r[0] = { 0.1f, 0.f, 0.f, 0.f };
		Effectmatrix.r[1] = { 0.f, 0.f, 1.f, 0.f };
		Effectmatrix.r[2] = { 0.f, -1.f, 0.f, 0.f };
		Effectmatrix.r[3] = { 0.f, 1.f, 0.5f, 1.f };
		XMStoreFloat4x4(&Info->WorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		if (FAILED(GI->Add_GameObjectToLayer(L"Wind", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
			return E_FAIL;

		CWind::WINDINFO* Info2 = new CWind::WINDINFO;
		Info2->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info2->vColor2 = { 0.7f,0.7f,0.7f,1.f };
		Info2->vFixPosSpeed = { 0.f,0.f,5.f };
		Info2->vFixScaleSpeed = { 40.f,40.f,40.f };
		Info2->vRatio = { 1.f,1.f };
		Info2->vRatioSpeed = { 2.f,2.f };
		Info2->vUVFixSpeed = { 1.f,-3.f };
		Effectmatrix.r[0] = { 0.1f, 0.f, 0.f, 0.f };
		Effectmatrix.r[1] = { 0.f, 0.f, 1.f, 0.f };
		Effectmatrix.r[2] = { 0.f, -1.f, 0.f, 0.f };
		Effectmatrix.r[3] = { 0.f, 1.f, 0.5f, 1.f };
		XMStoreFloat4x4(&Info2->WorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		if (FAILED(GI->Add_GameObjectToLayer(L"Wind", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info2)))
			return E_FAIL;

		break;
	}

	case Client::CPlayer04::ET_AIRATTACK1:
	{
		CPlayer01Trail_1::TRAILINFO* Info = new CPlayer01Trail_1::TRAILINFO;
		Info->vColor = { 0.2f,0.2f,0.8f,1.f };
		Info->vColor2 = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 5.f,1.f };
		Info->vRatioSpeed = { 30.f,0.f };
		_matrix Effectmatrix;
		Effectmatrix.r[0] = { -0.86f, 4.92f, 0.f, 0.f };
		Effectmatrix.r[1] = { 4.92f, 0.86f, 0.f, 0.f };
		Effectmatrix.r[2] = { 0.f, 0.f, -5.f, 0.f };
		Effectmatrix.r[3] = { -0.1f, 1.51f, 1.f, 1.f };
		XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		Info->fBlurPower = 0.2f;
		if (FAILED(GI->Add_GameObjectToLayer(L"Player01Trail_1", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
			return E_FAIL;

		break;
	}

	case Client::CPlayer04::ET_AIRATTACK2:
	{
		CPlayer01Trail_1::TRAILINFO* Info = new CPlayer01Trail_1::TRAILINFO;
		Info->vColor = { 0.2f,0.2f,0.8f,1.f };
		Info->vColor2 = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 5.f,1.f };
		Info->vRatioSpeed = { 30.f,0.f };
		_matrix Effectmatrix;
		Effectmatrix.r[0] = { 4.7f, -1.71f, 0.f, 0.f };
		Effectmatrix.r[1] = { -1.71f, -4.7f, 0.f, 0.f };
		Effectmatrix.r[2] = { 0.f, 0.f, -5.f, 0.f };
		Effectmatrix.r[3] = { 0.1f, 1.2f, 1.f, 1.f };
		XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		Info->fBlurPower = 0.2f;
		if (FAILED(GI->Add_GameObjectToLayer(L"Player01Trail_1", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
			return E_FAIL;

		break;
	}

	case Client::CPlayer04::ET_AIRATTACK3:
	{
		CPlayer01Trail_1::TRAILINFO* Info = new CPlayer01Trail_1::TRAILINFO;
		Info->vColor = { 0.2f,0.2f,0.8f,1.f };
		Info->vColor2 = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 5.f,1.f };
		Info->vRatioSpeed = { 30.f,0.f };
		_matrix Effectmatrix;
		Effectmatrix.r[0] = { 1.29f, -4.82f, 0.f, 0.f };
		Effectmatrix.r[1] = { -4.82f, -1.29f, 0.f, 0.f };
		Effectmatrix.r[2] = { 0.f, 0.f, -5.f, 0.f };
		Effectmatrix.r[3] = { 0.f, 0.9f, 1.f, 1.f };
		XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		Info->fBlurPower = 0.2f;
		if (FAILED(GI->Add_GameObjectToLayer(L"Player01Trail_1", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
			return E_FAIL;

		break;
	}

	case Client::CPlayer04::ET_SKILL9:
	{
		CWind::WINDINFO* Info = new CWind::WINDINFO;
		Info->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
		Info->vFixPosSpeed = { 0.f,0.f,3.f };
		Info->vFixScaleSpeed = { 30.f,30.f,30.f };
		Info->vRatio = { 1.f,1.f };
		Info->vRatioSpeed = { 2.f,2.f };
		Info->vUVFixSpeed = { 1.f,-3.f };
		_matrix Effectmatrix;
		Effectmatrix.r[0] = { 0.1f, 0.f, 0.f, 0.f };
		Effectmatrix.r[1] = { 0.f, 0.034f, 0.09f, 0.f };
		Effectmatrix.r[2] = { 0.f, -0.09f, 0.034f, 0.f };
		Effectmatrix.r[3] = { 0.f, 1.3f, 0.53f, 1.f };
		XMStoreFloat4x4(&Info->WorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		if (FAILED(GI->Add_GameObjectToLayer(L"Wind", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
			return E_FAIL;

		CWind::WINDINFO* Info2 = new CWind::WINDINFO;
		Info2->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info2->vColor2 = { 0.7f,0.7f,0.7f,1.f };
		Info2->vFixPosSpeed = { 0.f,0.f,5.f };
		Info2->vFixScaleSpeed = { 40.f,40.f,40.f };
		Info2->vRatio = { 1.f,1.f };
		Info2->vRatioSpeed = { 2.f,2.f };
		Info2->vUVFixSpeed = { 1.f,-3.f };
		Effectmatrix.r[0] = { 0.1f, 0.f, 0.f, 0.f };
		Effectmatrix.r[1] = { 0.f, 0.034f, 0.09f, 0.f };
		Effectmatrix.r[2] = { 0.f, -0.09f, 0.034f, 0.f };
		Effectmatrix.r[3] = { 0.f, 1.3f, 0.53f, 1.f };
		XMStoreFloat4x4(&Info2->WorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		if (FAILED(GI->Add_GameObjectToLayer(L"Wind", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info2)))
			return E_FAIL;

		break;
	}

	case Client::CPlayer04::ET_SKILLCHARGE:
	{
		CEffectSpin::EFFECTSPINDESC EffectSpinDesc;
		EffectSpinDesc.iSpinType = 0;
		EffectSpinDesc.iPlayerType = CEffectSpin::PLAYERTYPE::PLAYERTYPE_DD;
		if (FAILED(GI->Add_GameObjectToLayer(TEXT("EffectSpin"), GI->Get_Cur_LevelIndex(), TEXT("Layer_KSEFFECT"), &EffectSpinDesc)))
			return E_FAIL;
		EffectSpinDesc.iSpinType = 1;
		EffectSpinDesc.iPlayerType = CEffectSpin::PLAYERTYPE::PLAYERTYPE_DD;
		if (FAILED(GI->Add_GameObjectToLayer(TEXT("EffectSpin"), GI->Get_Cur_LevelIndex(), TEXT("Layer_KSEFFECT"), &EffectSpinDesc)))
			return E_FAIL;
		break;
	}


	case Client::CPlayer04::ET_SKILL8:
	{
		break;
		
	}
	case Client::CPlayer04::ET_SKILL5:
	{
		if (FAILED(PTM->Create_AlphaParticle(L"Player04Sunder1", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;
		if (FAILED(PTM->Create_AlphaParticle(L"Player04Sunder2", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;
		if (FAILED(PTM->Create_AlphaParticle(L"Player04SunderA", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;
		if (FAILED(PTM->Create_AlphaParticle(L"Player04SunderB", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;

		CViewAlpha::VIEWALPHAINFO* Info2 = new CViewAlpha::VIEWALPHAINFO;
		Info2->fBlurPower = 1.f;
		Info2->vColor1 = { 1.f,1.f,1.f,1.f };
		Info2->vColor2 = { 0.f,0.2f,1.f,1.f };
		Info2->vRatio = { 0.7f,0.7f };
		Info2->vRatioSpeed = { 2.f,1.f };
		if (FAILED(GI->Add_GameObjectToLayer(L"ViewAlpha", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2)))
			return E_FAIL;

		CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
		Info3->fBlurPower = 5.f;
		Info3->vLocalPos = { 0.f,0.f,1.f,1.f };
		Info3->vColor = { 1.f,1.f,1.f,1.f };
		Info3->vColor2 = { 0.f,0.f,1.f,1.f };
		Info3->vRatio = { 1.f,2.f };
		Info3->vRatioSpeed = { 1.5f,1.5f };
		Info3->vScaleSpeed = { 70.f,70.f,70.f };
		Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3)))
			return E_FAIL;

		CPlayer01Wind2::WIND2INFO* Info4 = new CPlayer01Wind2::WIND2INFO;
		Info4->fBlurPower = 5.f;
		Info4->vLocalPos = { 0.f,0.f,1.f,1.f };
		Info4->vColor = { 1.f,1.f,1.f,1.f };
		Info4->vColor2 = { 0.f,0.f,1.f,1.f };
		Info4->vRatio = { 1.f,2.f };
		Info4->vRatioSpeed = { 1.5f,1.5f };
		Info4->vScaleSpeed = { 70.f,70.f,70.f };
		Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4)))
			return E_FAIL;

		CPlayer01Ball::BALLINFO* Info5 = new CPlayer01Ball::BALLINFO;
		Info5->fBlurPower = 5.f;
		Info5->vColor = { 0.f,0.f,0.f,1.f };
		Info5->vColor2 = { 0.f,0.f,0.f,1.f };
		Info5->vRatio = { 1.f,1.f };
		Info5->vRatioSpeed = { 1.5f,1.5f };
		Info5->vScaleSpeed = { 20.f,20.f,20.f };
		Info5->vLocalPos = { 0.f,0.f,1.f,1.f };
		Info5->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		if (FAILED(GI->Add_GameObjectToLayer(L"Player01Ball", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info5)))
			return E_FAIL;

		PTM->Create_InstanceParticle(L"Player04SunderEnd", m_pTransformCom->Get_WorldFloat4x4());
		break;
	}

	case Client::CPlayer04::ET_SKILL2_1:
	{
		CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
		Info3->fBlurPower = 2.f;
		Info3->vLocalPos = { 0.f,1.f,1.f,1.f };
		Info3->vColor = { 1.f,1.f,1.f,1.f };
		Info3->vColor2 = { 0.f,0.f,1.f,1.f };
		Info3->vRatio = { 2.f,3.f };
		Info3->vRatioSpeed = { 2.5f,2.5f };
		Info3->vScaleSpeed = { 50.f,50.f,50.f };
		Info3->vRotation = { 0.f,0.f,45.f };
		Info3->bRotation = true;
		Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3)))
			return E_FAIL;

		CPlayer01Wind2::WIND2INFO* Info4 = new CPlayer01Wind2::WIND2INFO;
		Info4->fBlurPower = 2.f;
		Info4->vLocalPos = { 0.f,1.f,1.f,1.f };
		Info4->vColor = { 1.f,1.f,1.f,1.f };
		Info4->vColor2 = { 0.f,0.f,1.f,1.f };
		Info4->vRatio = { 2.f,3.f };
		Info4->vRatioSpeed = { 2.5f,2.5f };
		Info4->vScaleSpeed = { 50.f,50.f,50.f };
		Info4->vRotation = { 0.f,0.f,45.f };
		Info4->bRotation = true;
		Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4)))
			return E_FAIL;
		break;
	}

	case Client::CPlayer04::ET_SKILL2_2:
	{
		CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
		Info3->fBlurPower = 2.f;
		Info3->vLocalPos = { 0.f,1.f,1.f,1.f };
		Info3->vColor = { 1.f,1.f,1.f,1.f };
		Info3->vColor2 = { 0.f,0.f,1.f,1.f };
		Info3->vRatio = { 2.f,3.f };
		Info3->vRatioSpeed = { 2.5f,2.5f };
		Info3->vScaleSpeed = { 50.f,50.f,50.f };
		Info3->vRotation = { 0.f,0.f,-60.f };
		Info3->bRotation = true;
		Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3)))
			return E_FAIL;

		CPlayer01Wind2::WIND2INFO* Info4 = new CPlayer01Wind2::WIND2INFO;
		Info4->fBlurPower = 2.f;
		Info4->vLocalPos = { 0.f,1.f,1.f,1.f };
		Info4->vColor = { 1.f,1.f,1.f,1.f };
		Info4->vColor2 = { 0.f,0.f,1.f,1.f };
		Info4->vRatio = { 2.f,3.f };
		Info4->vRatioSpeed = { 2.5f,2.5f };
		Info4->vScaleSpeed = { 50.f,50.f,50.f };
		Info4->vRotation = { 0.f,0.f,-60.f };
		Info4->bRotation = true;
		Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4)))
			return E_FAIL;
		break;
	}

	case Client::CPlayer04::ET_SKILL11:
	{
		switch (m_iChargeCount)
		{
		case 0:
			break;
		case 1:
		{
			CPlayer04SmokeBall::SMOKEBALLINFO* Info = new CPlayer04SmokeBall::SMOKEBALLINFO;
			Info->fBlurPower = 0.5f;
			Info->fSpeed = 2.f;
			Info->fTimeLimit = 4.5f;
			Info->vColor = { 0.f,0.f,0.f,0.f };
			Info->vColor2 = { 0.475f,0.f,1.f,0.f };
			Info->vLocalPos = { 0.f,1.f,2.f,1.f };
			Info->vRatio = { 3.f,1.f };
			Info->vRatioSpeed = { 1.f,1.f };
			Info->vScale = { 10.f,10.f,10.f };
			Info->vScaleSpeed = { 0.f,0.f,0.f };
			Info->vUvFixSpeed = { 0.f,0.7f };
			Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			GI->Add_GameObjectToLayer(L"Player04SmokeBall", TM->Get_CurLevel(), L"Layer_Player04Effect", &Info);

			CPlayer04SmokeBall::SMOKEBALLINFO* Info2 = new CPlayer04SmokeBall::SMOKEBALLINFO;
			Info2->fBlurPower = 0.5f;
			Info2->fSpeed = 2.f;
			Info2->fTimeLimit = 4.5f;
			Info2->vColor = { 0.f,0.f,0.f,0.f };
			Info2->vColor2 = { 0.475f,0.f,1.f,0.f };
			Info2->vLocalPos = { 0.f,1.f,2.f,1.f };
			Info2->vRatio = { 3.f,1.f };
			Info2->vRatioSpeed = { 1.f,1.f };
			Info2->vScale = { 7.f,7.f,7.f };
			Info2->vScaleSpeed = { 0.f,0.f,0.f };
			Info2->vUvFixSpeed = { 1.2f,0.f };
			Info2->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			GI->Add_GameObjectToLayer(L"Player04SmokeBall", TM->Get_CurLevel(), L"Layer_Player04Effect", &Info2);

			CPlayer04DarkBall::DARKBALLINFO* Info3 = new CPlayer04DarkBall::DARKBALLINFO;
			Info3->fBlurPower = 0.f;
			Info3->fSpeed = 2.f;
			Info3->fTimeLimit = 4.5f;
			Info3->iChargeCount = 2;
			Info3->vColor = { 0.f,0.f,0.f,1.f };
			Info3->vColor2 = { 0.f,0.f,0.f,1.f };
			Info3->vLocalPos = { 0.f,1.f,2.f,1.f };
			Info3->vRatio = { 1.f,1.f };
			Info3->vRatioSpeed = { 1.f,1.f };
			Info3->vScale = { 1.3f,1.3f,1.3f };
			Info3->vScaleSpeed = { 20.f,2.f };
			Info3->vScaleTime = { 0.1f,1.f };
			Info3->iChargeCount = 1;
			Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			GI->Add_GameObjectToLayer(L"Player04DarkBall", TM->Get_CurLevel(), L"Layer_Player04Effect", &Info3);
		}
			break;
		case 2:
		{
			CPlayer04SmokeBall::SMOKEBALLINFO* Info = new CPlayer04SmokeBall::SMOKEBALLINFO;
			Info->fBlurPower = 0.5f;
			Info->fSpeed = 2.f;
			Info->fTimeLimit = 4.5f;
			Info->vColor = { 0.f,0.f,0.f,0.f };
			Info->vColor2 = { 0.475f,0.f,1.f,0.f };
			Info->vLocalPos = { 0.f,1.f,2.f,1.f };
			Info->vRatio = { 3.f,1.f };
			Info->vRatioSpeed = { 1.f,1.f };
			Info->vScale = { 15.f,15.f,15.f };
			Info->vScaleSpeed = { 0.f,0.f,0.f };
			Info->vUvFixSpeed = { 0.f,0.7f };
			Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			GI->Add_GameObjectToLayer(L"Player04SmokeBall", TM->Get_CurLevel(), L"Layer_Player04Effect", &Info);

			CPlayer04SmokeBall::SMOKEBALLINFO* Info2 = new CPlayer04SmokeBall::SMOKEBALLINFO;
			Info2->fBlurPower = 0.5f;
			Info2->fSpeed = 2.f;
			Info2->fTimeLimit = 4.5f;
			Info2->vColor = { 0.f,0.f,0.f,0.f };
			Info2->vColor2 = { 0.475f,0.f,1.f,0.f };
			Info2->vLocalPos = { 0.f,1.f,2.f,1.f };
			Info2->vRatio = { 3.f,1.f };
			Info2->vRatioSpeed = { 1.f,1.f };
			Info2->vScale = { 12.f,12.f,12.f };
			Info2->vScaleSpeed = { 0.f,0.f,0.f };
			Info2->vUvFixSpeed = { 1.2f,0.f };
			Info2->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			GI->Add_GameObjectToLayer(L"Player04SmokeBall", TM->Get_CurLevel(), L"Layer_Player04Effect", &Info2);

			CPlayer04DarkBall::DARKBALLINFO* Info3 = new CPlayer04DarkBall::DARKBALLINFO;
			Info3->fBlurPower = 0.f;
			Info3->fSpeed = 2.f;
			Info3->fTimeLimit = 4.5f;
			Info3->iChargeCount = 2;
			Info3->vColor = { 0.f,0.f,0.f,1.f };
			Info3->vColor2 = { 0.f,0.f,0.f,1.f };
			Info3->vLocalPos = { 0.f,1.f,2.f,1.f };
			Info3->vRatio = { 1.f,1.f };
			Info3->vRatioSpeed = { 1.f,1.f };
			Info3->vScale = { 2.f,2.f,2.f };
			Info3->vScaleSpeed = { 30.f,3.f };
			Info3->vScaleTime = { 0.1f,1.f };
			Info3->iChargeCount = 2;
			Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			GI->Add_GameObjectToLayer(L"Player04DarkBall", TM->Get_CurLevel(), L"Layer_Player04Effect", &Info3);
		}
			break;
		case 3:
			CPlayer04SmokeBall::SMOKEBALLINFO* Info = new CPlayer04SmokeBall::SMOKEBALLINFO;
			Info->fBlurPower = 0.5f;
			Info->fSpeed = 2.f;
			Info->fTimeLimit = 4.5f;
			Info->vColor = { 0.f,0.f,0.f,0.f };
			Info->vColor2 = { 0.475f,0.f,1.f,0.f };
			Info->vLocalPos = { 0.f,1.5f,2.f,1.f };
			Info->vRatio = { 3.f,1.f };
			Info->vRatioSpeed = { 1.f,1.f };
			Info->vScale = { 25.f,25.f,25.f };
			Info->vScaleSpeed = { 0.f,0.f,0.f };
			Info->vUvFixSpeed = { 0.f,0.7f };
			Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			GI->Add_GameObjectToLayer(L"Player04SmokeBall", TM->Get_CurLevel(), L"Layer_Player04Effect", &Info);

			CPlayer04SmokeBall::SMOKEBALLINFO* Info2 = new CPlayer04SmokeBall::SMOKEBALLINFO;
			Info2->fBlurPower = 0.5f;
			Info2->fSpeed = 2.f;
			Info2->fTimeLimit = 4.5f;
			Info2->vColor = { 0.f,0.f,0.f,0.f };
			Info2->vColor2 = { 0.475f,0.f,1.f,0.f };
			Info2->vLocalPos = { 0.f,1.5f,2.f,1.f };
			Info2->vRatio = { 3.f,1.f };
			Info2->vRatioSpeed = { 1.f,1.f };
			Info2->vScale = { 20.f,20.f,20.f };
			Info2->vScaleSpeed = { 0.f,0.f,0.f };
			Info2->vUvFixSpeed = { 1.2f,0.f };
			Info2->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			GI->Add_GameObjectToLayer(L"Player04SmokeBall", TM->Get_CurLevel(), L"Layer_Player04Effect", &Info2);

			CPlayer04DarkBall::DARKBALLINFO* Info3 = new CPlayer04DarkBall::DARKBALLINFO;
			Info3->fBlurPower = 0.f;
			Info3->fSpeed = 2.f;
			Info3->fTimeLimit = 4.5f;
			Info3->iChargeCount = 2;
			Info3->vColor = { 0.f,0.f,0.f,1.f };
			Info3->vColor2 = { 0.f,0.f,0.f,1.f };
			Info3->vLocalPos = { 0.f,1.5f,2.f,1.f };
			Info3->vRatio = { 1.f,1.f };
			Info3->vRatioSpeed = { 1.f,1.f };
			Info3->vScale = { 4.f,4.f,4.f };
			Info3->vScaleSpeed = { 50.f,5.f };
			Info3->vScaleTime = { 0.05f,0.5f };
			Info3->iChargeCount = 3;
			Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			GI->Add_GameObjectToLayer(L"Player04DarkBall", TM->Get_CurLevel(), L"Layer_Player04Effect", &Info3);
			break;
		}
		
		break;
	}

	case Client::CPlayer04::ET_RAGESKILL1:
	{
		
	}
		break;

	case Client::CPlayer04::ET_RAGESKILL1_2:
	{
		CPlayer04WhiteBall::WHITEBALLINFO* Info = new CPlayer04WhiteBall::WHITEBALLINFO;
		Info->fBlurPower = 1.f;
		Info->vColor = { 1.f,1.f,1.f,1.f };
		Info->vColor2 = { 0.f,0.f,0.f,1.f };
		Info->vLocalPos = { 0.f,1.5f,0.0f,1.f };
		Info->vRatio = { 1.f,1.f };
		Info->vRatioSpeed = { 5.f,5.f };
		Info->vScale = { 0.1f,0.1f,0.1f };
		Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player04WhiteBall", TM->Get_CurLevel(), L"Layer_Player04Effect", &Info);
	}
		break;

	case Client::CPlayer04::ET_RAGESKILLCUT:
	{
		PTM->Create_InstanceParticle(L"Player04RageSkill1Cut_3", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player04RageSkill1Cut", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player04RageSkill1Cut_2", m_pTransformCom->Get_WorldFloat4x4());
		
	}
		break;

	case Client::CPlayer04::ET_SKILL10:
	{

		CWind::WINDINFO* Info = new CWind::WINDINFO;
		Info->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
		Info->vFixPosSpeed = { 0.f,0.f,3.f };
		Info->vFixScaleSpeed = { 30.f,30.f,30.f };
		Info->vRatio = { 1.f,1.f };
		Info->vRatioSpeed = { 2.f,2.f };
		Info->vUVFixSpeed = { 1.f,-3.f };
		_matrix Effectmatrix;
		Effectmatrix.r[0] = { 0.1f, 0.f, 0.f, 0.f };
		Effectmatrix.r[1] = { 0.f, 0.034f, 0.09f, 0.f };
		Effectmatrix.r[2] = { 0.f, -0.09f, 0.034f, 0.f };
		Effectmatrix.r[3] = { 0.f, 1.3f, 0.53f, 1.f };
		XMStoreFloat4x4(&Info->WorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		if (FAILED(GI->Add_GameObjectToLayer(L"Wind", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
			return E_FAIL;

		CWind::WINDINFO* Info2 = new CWind::WINDINFO;
		Info2->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info2->vColor2 = { 0.7f,0.7f,0.7f,1.f };
		Info2->vFixPosSpeed = { 0.f,0.f,5.f };
		Info2->vFixScaleSpeed = { 40.f,40.f,40.f };
		Info2->vRatio = { 1.f,1.f };
		Info2->vRatioSpeed = { 2.f,2.f };
		Info2->vUVFixSpeed = { 1.f,-3.f };
		Effectmatrix.r[0] = { 0.1f, 0.f, 0.f, 0.f };
		Effectmatrix.r[1] = { 0.f, 0.034f, 0.09f, 0.f };
		Effectmatrix.r[2] = { 0.f, -0.09f, 0.034f, 0.f };
		Effectmatrix.r[3] = { 0.f, 1.3f, 0.53f, 1.f };
		XMStoreFloat4x4(&Info2->WorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		if (FAILED(GI->Add_GameObjectToLayer(L"Wind", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info2)))
			return E_FAIL;
	}
	break;

	}

	return S_OK;
}

HRESULT CPlayer04::Create_Particle(PARTICLE ePT)
{
	switch (ePT)
	{
	case Client::CPlayer04::PT_VOID:
	{

		PTM->Create_AlphaParticle(L"Player04VoidEffect_1", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player04VoidEffect_2", m_pTransformCom->Get_WorldFloat4x4());
		
		
	}
		break;

	}

	return S_OK;
	
}

HRESULT CPlayer04::Create_ParticleInstance(PARTICLEINSTANCE ePT)
{
	switch (ePT)
	{
	case Client::CPlayer04::PTI_SKILL1:
		PTM->Create_InstanceParticle(L"Player04Skill1", m_pTransformCom->Get_WorldFloat4x4());
		//PTM->Create_InstanceParticle(L"Player04Skill1_3", m_pTransformCom->Get_WorldFloat4x4());
		break;

	case Client::CPlayer04::PTI_SKILL1_1:
		PTM->Create_InstanceParticle(L"Player04Skill1_1", m_pTransformCom->Get_WorldFloat4x4());
		break;

	case Client::CPlayer04::PTI_SKILL1_2:
		PTM->Create_InstanceParticle(L"Player04Skill1_2", m_pTransformCom->Get_WorldFloat4x4());
		break;

	case Client::CPlayer04::PTI_SKILL12:
		PTM->Create_InstanceParticle(L"Player04Skill12", m_pTransformCom->Get_WorldFloat4x4());
		break;

	case Client::CPlayer04::PTI_SKILL12_1:
		PTM->Create_InstanceParticle(L"Player04Skill12_1", m_pTransformCom->Get_WorldFloat4x4());
		break;

	case Client::CPlayer04::PTI_SKILL12_2:
		PTM->Create_InstanceParticle(L"Player04Skill12_2", m_pTransformCom->Get_WorldFloat4x4());
		//PTM->Create_InstanceParticle(L"Player04Skill12_3", m_pTransformCom->Get_WorldFloat4x4());
		break;

	case Client::CPlayer04::PTI_SKILL4:
		PTM->Create_InstanceParticle(L"Player04Skill4", m_pTransformCom->Get_WorldFloat4x4());
		break;

	case Client::CPlayer04::PTI_SKILL4_1:
		PTM->Create_InstanceParticle(L"Player04Skill4_1", m_pTransformCom->Get_WorldFloat4x4());
		break;

	case Client::CPlayer04::PTI_BURST:
		PTM->Create_InstanceParticle(L"Player04Burst", m_pTransformCom->Get_WorldFloat4x4());
		break;
	
	}
	return S_OK;
}

HRESULT CPlayer04::Ready_Trails()
{
	CSwordTrail::TRAILINFO Info;
	Info.HighAndLow.vHigh = { 0.f,0.f,0.6f };
	Info.HighAndLow.vLow = { 0.f,0.f,0.f };
	Info.TexName = L"SwordTrail_Right";
	Info.vColor1 = { 1.f,1.f,1.f,1.f };
	Info.vColor2 = { 1.f,0.2f,1.f,1.f };
	Info.vRatio = { 1.f,1.f };
	Info.bFixColor = true;
	Info.iShaderPass = 1;
	Info.fBlurPower = 1.f;
	CGameObject*		SwordTrail1 = GI->Clone_GameObject(TEXT("SwordTrailObj"), &Info);
	if (nullptr == SwordTrail1)
		return E_FAIL;
	m_SwordTrails.push_back((CSwordTrailObj*)SwordTrail1);

	CPointTrail::TRAILCREATEINFO Info2;
	Info2.vColor = { 1.f, 1.f, 1.f,1.f };
	Info2.vColor2 = { 0.f, 0.4f, 1.f,1.f };
	Info2.bFixColor = true;
	Info2.fBlurPower = 0.5f;
	Info2.iShaderPass = 1;
	Info2.vRatio = { 1.f,1.f };
	Info2.fSize = 0.07f;
	Info2.TexName = L"PointTrail03";
	Info2.iMaxIndex = 10;

	CGameObject*		LKick = GI->Clone_GameObject(TEXT("PointTrailObj"), &Info2);
	if (nullptr == LKick)
		return E_FAIL;
	m_PointTrails.push_back((CPointTrailObj*)LKick);

	CGameObject*		RKick = GI->Clone_GameObject(TEXT("PointTrailObj"), &Info2);
	if (nullptr == RKick)
		return E_FAIL;
	m_PointTrails.push_back((CPointTrailObj*)RKick);

	Info2.vColor = { 1.f, 1.f, 1.f,1.f };
	Info2.vColor2 = { 0.f, 0.4f, 1.f,1.f };
	Info2.bFixColor = true;
	Info2.fBlurPower = 0.5f;
	Info2.iShaderPass = 1;
	Info2.vRatio = { 1.f,1.f };
	Info2.fSize = 0.07f;
	Info2.TexName = L"Trail_Plasma_A";
	Info2.iMaxIndex = 15;
	CGameObject*		Body = GI->Clone_GameObject(TEXT("PointTrailObj"), &Info2);
	if (nullptr == Body)
		return E_FAIL;
	m_PointTrails.push_back((CPointTrailObj*)Body);

	CGameObject*		pMotionTrail1 = GI->Clone_GameObject(TEXT("Player04MotionTrail"));
	if (nullptr == pMotionTrail1)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail1);

	CGameObject*		pMotionTrail2 = GI->Clone_GameObject(TEXT("Player04MotionTrail"));
	if (nullptr == pMotionTrail2)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail2);

	CGameObject*		pMotionTrail3 = GI->Clone_GameObject(TEXT("Player04MotionTrail"));
	if (nullptr == pMotionTrail3)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail3);

	CGameObject*		pMotionTrail4 = GI->Clone_GameObject(TEXT("Player04MotionTrail"));
	if (nullptr == pMotionTrail4)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail4);

	CGameObject*		pMotionTrail5 = GI->Clone_GameObject(TEXT("Player04MotionTrail"));
	if (nullptr == pMotionTrail5)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail5);

	CGameObject*		pMotionTrail6 = GI->Clone_GameObject(TEXT("Player04MotionTrail"));
	if (nullptr == pMotionTrail6)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail6);

	CGameObject*		pMotionTrail7 = GI->Clone_GameObject(TEXT("Player04MotionTrail"));
	if (nullptr == pMotionTrail7)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail7);

	CGameObject*		pMotionTrail8 = GI->Clone_GameObject(TEXT("Player04MotionTrail"));
	if (nullptr == pMotionTrail8)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail8);

	CGameObject*		pMotionTrail9 = GI->Clone_GameObject(TEXT("Player04MotionTrail"));
	if (nullptr == pMotionTrail9)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail9);

	CGameObject*		pMotionTrail10 = GI->Clone_GameObject(TEXT("Player04MotionTrail"));
	if (nullptr == pMotionTrail10)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail10);

	return S_OK;
}

void CPlayer04::Create_MotionTrail(_float fTimeDelta)
{
	if (m_bMotionTrail)
	{
		if (m_MotionTrails.size() > 0)
			Set_MotionTrail(fTimeDelta);
	}

	for (auto& iter : m_MotionTrails)
	{
		iter->Tick(fTimeDelta);
	}
}

void CPlayer04::Create_PointTrail(_float fTimeDelta)
{
	Control_KickTrail(fTimeDelta);
	Control_BodyTrail(fTimeDelta);
}

void CPlayer04::Create_SwordTrail(_float fTimeDelta)
{
	if (m_SwordTrails[STR_SWORD]->Get_TrailOn())
	{
		_matrix TrailMatrix = ((CPlayer04Sword1*)m_Parts[PARTS_SWORD])->Get_EndMatrix();
		((CSwordTrailObj*)m_SwordTrails[0])->Set_Color(_float4{ 1.f,1.f,1.f,1.f });
		((CSwordTrailObj*)m_SwordTrails[0])->Set_Color2(_float4{ 0.f,0.2f,1.f,1.f });
		((CSwordTrailObj*)m_SwordTrails[0])->Set_BlurPower(1.f);
		((CSwordTrailObj*)m_SwordTrails[0])->Set_Ratio(_float2{ 1.f,1.f });
		((CSwordTrailObj*)m_SwordTrails[0])->Set_TimeLimit(0.015f);
		m_SwordTrails[STR_SWORD]->Set_HighLow(_float3{ 0.f,0.f,1.5f }, _float3{ 0.f,0.f,0.f });

		if (m_SwordTrails[STR_SWORD]->Get_TrailOn())
			m_SwordTrails[STR_SWORD]->Tick(fTimeDelta, TrailMatrix);
	}

}

void CPlayer04::Set_MotionTrail(_float fTimeDelta)
{
	m_fMotionTrailAcc += m_fMotionTrailSpeed * fTimeDelta;
	if (m_fMotionTrailAcc >= m_fMotionTrailLimit)
	{
		if(m_ePlayerMode != PMODE_BURST)
		{
			CPlayer04MotionTrail::PLAYER04MOTIONTRAILINFO Info;
			Info.WorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			Info.eHeadType = CPlayer04MotionTrail::HEADA;
			Info.Bodys = m_pModelCom->Get_HierarchyPointer();
			Info.HeadAMatrix = ((CPlayer04Hair*)m_Parts[PARTS_HAIR])->Get_ConbineMatrix();
			Info.Heads = ((CPlayer04Hair*)m_Parts[PARTS_HAIR])->Get_ModelCom()->Get_HierarchyPointer();
			((CPlayer04MotionTrail*)m_MotionTrails[m_iMotionTrailIndex])->Set_OnTrail(Info);
			m_fMotionTrailAcc = 0.f;
			++m_iMotionTrailIndex;
		}
		
		else
		{
			CPlayer04MotionTrail::PLAYER04MOTIONTRAILINFO Info;
			Info.WorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			Info.eHeadType = CPlayer04MotionTrail::HEADB;
			Info.Bodys = m_pModelCom->Get_HierarchyPointer();
			Info.HeadBMatrix = ((CPlayer04HairBattle*)m_Parts[PARTS_HAIRBATTLE])->Get_ConbineMatrix();
			Info.Heads2 = ((CPlayer04HairBattle*)m_Parts[PARTS_HAIRBATTLE])->Get_ModelCom()->Get_HierarchyPointer();
			((CPlayer04MotionTrail*)m_MotionTrails[m_iMotionTrailIndex])->Set_OnTrail(Info);
			m_fMotionTrailAcc = 0.f;
			++m_iMotionTrailIndex;
		}
	
		if (m_iMotionTrailIndex >= m_MotionTrails.size())
		{
			m_bMotionTrail = false;
			m_iMotionTrailIndex = 0;
		}
	}
}

void CPlayer04::Control_KickTrail(_float fTimeDelta)
{
	if (m_bPointTrail)
	{
		m_PointTrails[PTR_LKICK]->TrailOn();
		m_PointTrails[PTR_RKICK]->TrailOn();
		if (m_eCurState != IDLE2 && m_eCurState != IDLE1)
		{

			_matrix	LKickMatrix = m_Sockets[SOCKET_LKICK]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
			_float4 LKickPos;
			XMStoreFloat4(&LKickPos, LKickMatrix.r[3]);
			m_PointTrails[PTR_LKICK]->Add_Point(LKickPos);


			_matrix RKickMatrix = m_Sockets[SOCKET_RKICK]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
			_float4 RKickPos;
			XMStoreFloat4(&RKickPos, RKickMatrix.r[3]);
			m_PointTrails[PTR_RKICK]->Add_Point(RKickPos);
		}
		m_PointTrails[PTR_LKICK]->Tick(fTimeDelta);
		m_PointTrails[PTR_RKICK]->Tick(fTimeDelta);
	}
	else
	{
		m_PointTrails[PTR_LKICK]->TrailOff();
		m_PointTrails[PTR_RKICK]->TrailOff();
	}
}

void CPlayer04::Control_BodyTrail(_float fTimeDelta)
{
	if (m_bBodyTrail)
	{
		m_PointTrails[PTR_BODY]->TrailOn();
		_matrix	LKickMatrix = m_Sockets[SOCKET_BODY]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
		_float4 LKickPos;
		XMStoreFloat4(&LKickPos, LKickMatrix.r[3]);
		m_PointTrails[PTR_BODY]->Add_Point(LKickPos);

		m_PointTrails[PTR_BODY]->Tick(fTimeDelta);

	}
	else
	{
		m_PointTrails[PTR_BODY]->TrailOff();
	
	}
}

void CPlayer04::Player_Die()
{
	m_bDie = true;
	m_bSelectPlayer = false;
	PM->Change_Player(CPlayer::PLAYER_01);
}

void CPlayer04::Reset_SlowMotionTrail()
{
	for (auto& iter : m_MotionTrails)
	{
		((CPlayer04MotionTrail*)iter)->Set_AlphaLimit(0.1f);
		((CPlayer04MotionTrail*)iter)->Set_ShaderPass(8);
		((CPlayer04MotionTrail*)iter)->Set_BlurPower(0.3f);
		((CPlayer04MotionTrail*)iter)->Set_DownSpeed(1.f);
		((CPlayer04MotionTrail*)iter)->Set_UpSpeed(1.f);
	}
}

void CPlayer04::Set_SlowMotionTrail()
{
	for (auto& iter : m_MotionTrails)
	{
		((CPlayer04MotionTrail*)iter)->Set_AlphaLimit(1.f);
		((CPlayer04MotionTrail*)iter)->Set_ShaderPass(10);
		((CPlayer04MotionTrail*)iter)->Set_DownSpeed(20.f);
		((CPlayer04MotionTrail*)iter)->Set_UpSpeed(5.f);
	}
}

void CPlayer04::Check_PlayerLight(_float fTimeDelta)
{
	if (m_bPlayerLight)
	{
		m_eShaderPass = SHADER_LIGHT;
		((CPlayer04Hair*)m_Parts[PARTS_HAIR])->Set_PlayerLight(true);
		((CPlayer04HairBattle*)m_Parts[PARTS_HAIRBATTLE])->Set_PlayerLight(true);
		m_fPlayerLightAcc += 1.f * fTimeDelta;
		if (m_fPlayerLightAcc >= m_fPlayerLightLimit)
		{
			m_bPlayerLight = false;
			m_fPlayerLightAcc = 0.f;
			m_eShaderPass = SHADER_DEFAULT;
			((CPlayer04Hair*)m_Parts[PARTS_HAIR])->Set_PlayerLight(false);
			((CPlayer04HairBattle*)m_Parts[PARTS_HAIRBATTLE])->Set_PlayerLight(false);
			Create_LightEffect();
		}
	}
}

void CPlayer04::Change_Sound()
{
	
	_uint iRandomVoice = GI->Get_Random(1, 3);

	GI->PlaySoundW(L"UI_GodChange.wav", SD_UI, 0.6f);

	switch (iRandomVoice)
	{
	case 1:
		GI->PlaySoundW(L"Hero05_ba_60.wav", SD_PLAYER4_VO, 1.f);
		break;
	case 2:
		GI->PlaySoundW(L"Hero05_ba_61.wav", SD_PLAYER4_VO, 1.f);
		break;
	case 3:
		GI->PlaySoundW(L"Hero05_ba_62.wav", SD_PLAYER4_VO, 1.f);
	}
}

void CPlayer04::Die_Sound()
{
	_uint iRandomVoice = GI->Get_Random(0, 2);

	switch (iRandomVoice)
	{
	case 0:
		GI->PlaySoundW(L"Hero05_ba_78.wav", SD_PLAYER4_VO, 1.f);
		break;
	case 1:
		GI->PlaySoundW(L"Hero05_ba_79.wav", SD_PLAYER4_VO, 1.f);
		break;
	case 2:
		GI->PlaySoundW(L"Hero05_ba_80.wav", SD_PLAYER4_VO, 1.f);
		break;
	default:
		break;
	}
	
}

_bool CPlayer04::Get_IsJump()
{
	return m_pTransformCom->Get_Air();
}

void CPlayer04::Set_ScenePos(_float4 vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));
	m_pNavigationCom->Set_NaviIndex(m_pNavigationCom->Find_NaviIndex(XMLoadFloat4(&vPos)));
}

void CPlayer04::Set_SceneLook(_float4 vPos)
{
	Update_Parts();
	m_Parts[PARTS_HAIRBATTLE]->LateTick(1.f / 60.f);
	((CPlayer04HairBattle*)m_Parts[PARTS_HAIRBATTLE])->SetInitPos();
	m_Parts[PARTS_HAIR]->LateTick(1.f / 60.f);
	((CPlayer04Hair*)m_Parts[PARTS_HAIR])->SetInitPos();
	Tick_Col(m_pTransformCom->Get_WorldMatrix());
	m_pTransformCom->LookAt_ForLandObject(XMLoadFloat4(&vPos));
	XMStoreFloat3(&m_vTargetLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));


}

void CPlayer04::Set_SceneIdle()
{
	Reset_Collider();
	Set_State(IDLE1);
}

void CPlayer04::Set_SceneHit(HITTYPE eHitType, _float fPushPower, _float fAirPower, _float4 vPos, _float3 vDir)
{
	Reset_Collider();
	m_fPushPower = fPushPower;
	m_fAirPower = fAirPower;

	switch (eHitType)
	{
	case Client::CCreture::TYPE_HIT:
		if (m_pTransformCom->Check_TargetFront(XMLoadFloat4(&vPos)))
			Set_State(HURTFRONT);
		else
			Set_State(HURTBACK);
		break;
	case Client::CCreture::TYPE_SPIN:
		XMStoreFloat3(&m_vTargetLook, XMVector3Normalize(XMLoadFloat4(&vPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
		XMStoreFloat3(&m_vPushLook, XMVector3Normalize(XMLoadFloat3(&vDir)));
		Set_State(HITSPIN);
		m_pTransformCom->ResetAndJump(m_fAirPower);
		break;
	case Client::CCreture::TYPE_AWAY:
		XMStoreFloat3(&m_vTargetLook, XMVector3Normalize(XMLoadFloat4(&vPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
		XMStoreFloat3(&m_vPushLook, XMVector3Normalize(XMLoadFloat3(&vDir)));
		Set_State(HITAWAY);
		m_pTransformCom->ResetAndJump(m_fAirPower);
		break;
	case Client::CCreture::TYPE_UP:
		break;
	case Client::CCreture::TYPE_FASTDOWN:
		XMStoreFloat3(&m_vTargetLook, XMVector3Normalize(XMLoadFloat4(&vPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
		XMStoreFloat3(&m_vPushLook, XMVector3Normalize(XMLoadFloat3(&vDir)));
		Set_State(FALLDOWN);
		break;
	}
}

void CPlayer04::Set_SceneRealHit(HITTYPE eHitType, _float fPushPower, _float fAirPower, _float4 vPos, _float3 vDir, _float fDamage)
{
	Reset_Collider();
	m_fPushPower = fPushPower;
	m_fAirPower = fAirPower;
	m_bRim = true;
	m_fNowHp -= fDamage;
	switch (eHitType)
	{
	case Client::CCreture::TYPE_HIT:
		if (m_pTransformCom->Check_TargetFront(XMLoadFloat4(&vPos)))
			Set_State(HURTFRONT);
		else
			Set_State(HURTBACK);
		break;
	case Client::CCreture::TYPE_SPIN:
		XMStoreFloat3(&m_vTargetLook, XMVector3Normalize(XMLoadFloat4(&vPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
		XMStoreFloat3(&m_vPushLook, XMVector3Normalize(XMLoadFloat3(&vDir)));
		Set_State(HITSPIN);
		m_pTransformCom->ResetAndJump(m_fAirPower);
		break;
	case Client::CCreture::TYPE_AWAY:
		XMStoreFloat3(&m_vTargetLook, XMVector3Normalize(XMLoadFloat4(&vPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
		XMStoreFloat3(&m_vPushLook, XMVector3Normalize(XMLoadFloat3(&vDir)));
		Set_State(HITAWAY);
		m_pTransformCom->ResetAndJump(m_fAirPower);
		break;
	case Client::CCreture::TYPE_UP:
		break;
	case Client::CCreture::TYPE_FASTDOWN:
		XMStoreFloat3(&m_vTargetLook, XMVector3Normalize(XMLoadFloat4(&vPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
		XMStoreFloat3(&m_vPushLook, XMVector3Normalize(XMLoadFloat3(&vDir)));
		Set_State(FALLDOWN);
		break;
	}
}

void CPlayer04::Set_Rage1End()
{
	m_pModelCom->Set_TickPerSecond(30.f);
}

void CPlayer04::Default_KeyInput(_float fTimeDelta)
{
	if (CK_PRESS(DIK_CAPSLOCK))
	{
		m_bVoid = true;
		m_fVoidSpeed = 10.f;
		m_bSwordRender = false;
		m_bRenderPlayer = false;
		m_pModelCom->Set_AnimIndex(VOIDFRONT);
		m_eCurState = VOIDFRONT;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
		Create_Particle(PT_VOID);
		return;
	}

	Change_Mode();

	if (CK_DOWN(DIK_LSHIFT))
	{
		Set_State(RUNFASTSTART);
		return;
	}

	if (CM_PRESS(DIMK_LBUTTON))
	{
		Set_State(ATTACK1);
		return;
	}

	if (CK_PRESS(DIK_R))
	{
		Set_State(SKILL3START);
		return;
	}

	if (CK_PRESS(DIK_Q))
	{
		Set_State(SKILL4START);
		return;
	}

	if (CK_PRESS(DIK_E))
	{
		Set_State(SKILL5);
		return;
	}

	if (CK_PRESS(DIK_F))
	{
		Set_State(SKILL6);
		return;
	}

	if (CK_PRESS(DIK_T))
	{
		Set_State(SKILL7START);
		return;
	}

	if (CK_PRESS(DIK_G))
	{
		Set_State(SKILL12START);
		return;
	}


	if (CK_PRESS(DIK_2))
	{
		Set_State(SKILL9);
		return;
	}

	if (CK_PRESS(DIK_3))
	{
		Set_State(SKILL11START);
		return;
	}

	if (CK_PRESS(DIK_4))
	{
		Set_State(SKILL1);
		return;
	}

	if (CK_PRESS(DIK_5))
	{
		Set_State(SKILL2);
		return;
	}

	if (CK_PRESS(DIK_V))
	{
		Set_State(RAGESKILL1);
		return;
	}

	if (CK_PRESS(DIK_B))
	{
		Set_State(RAGESKILL2);
		return;
	}

	if (CK_PRESS(DIK_TAB))
	{
		Set_State(SKILL10);
		return;
	}

}

bool CPlayer04::Input_Direction()
{
	if (CK_PRESS(DIK_W))
	{
		Set_Dir(DIR_UP);
		m_bDirKeyInput = true;
		if (CK_PRESS(DIK_D))
		{
			Set_Dir(DIR_RU);
			m_bDirKeyInput = true;
		}
		if (CK_PRESS(DIK_A))
		{
			Set_Dir(DIR_LU);
			m_bDirKeyInput = true;
		}
	}
	else if (CK_PRESS(DIK_S))
	{
		Set_Dir(DIR_DOWN);
		m_bDirKeyInput = true;
		if (CK_PRESS(DIK_D))
		{
			Set_Dir(DIR_RD);
			m_bDirKeyInput = true;
		}
		if (CK_PRESS(DIK_A))
		{
			Set_Dir(DIR_LD);
			m_bDirKeyInput = true;
		}

	}
	else if (CK_PRESS(DIK_D))
	{
		Set_Dir(DIR_RIGHT);
		m_bDirKeyInput = true;
	}
	else if (CK_PRESS(DIK_A))
	{
		Set_Dir(DIR_LEFT);
		m_bDirKeyInput = true;
	}

	return m_bDirKeyInput;
}

void CPlayer04::Idle_KeyInput(_float fTimeDelta)
{
	Default_KeyInput(fTimeDelta);

	if (Input_Direction())
	{
		Set_State(RUN);
		return;
	}

	if (CK_DOWN(DIK_SPACE))
	{
		m_fJumpSpeed = 2.f;
		m_pTransformCom->Jump(7.f);
		Set_State(JUMPSTART);
		return;
	}


	
}

void CPlayer04::Run_KeyInput(_float fTimeDelta)
{
	Default_KeyInput(fTimeDelta);

	if (!Input_Direction())
	{
		if (m_ePlayerMode == PMODE_NORMAL)
			Set_State(IDLE1);
		else
			Set_State(IDLE2);
	}

	if (CK_DOWN(DIK_SPACE))
	{
		m_fJumpSpeed = 5.f;
		m_pTransformCom->Jump(9.f);
		Set_State(JUMPSTART);
		return;
	}


}

void CPlayer04::Run_FastStartInput(_float fTimeDelta)
{
	if (CK_DOWN(DIK_SPACE))
	{
		m_fJumpSpeed = 8.f;
		m_pTransformCom->Jump(10.f);
		Set_State(JUMPSTART);
		return;
	}
}

void CPlayer04::Run_FastInput(_float fTimeDelta)
{
	if (CK_PRESS(DIK_CAPSLOCK))
	{
		m_bVoid = true;
		m_fVoidSpeed = 10.f;
		m_bSwordRender = false;
		m_bRenderPlayer = false;
		m_pModelCom->Set_AnimIndex(VOIDFRONT);
		m_eCurState = VOIDFRONT;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
		Create_Particle(PT_VOID);
		return;
	}

	if (CK_DOWN(DIK_SPACE))
	{
		m_fJumpSpeed = 8.f;
		m_pTransformCom->Jump(10.f);
		Set_State(JUMPSTART);
		return;
	}

	if (!Input_Direction())
	{
		m_pModelCom->Set_AnimIndex(RUNFASTEND);
		m_eCurState = RUNFASTEND;
		GI->PlaySoundW(L"stop-land.wav", SD_PLAYER4_SK, 0.2f);
		return;
	}

	Change_Mode();

	if (CM_DOWN(DIMK_LBUTTON))
	{
		Set_State(ATTACK1);
		return;
	}

	if (CK_DOWN(DIK_R))
	{
		Set_State(SKILL3START);
		return;
	}

	if (CK_DOWN(DIK_Q))
	{
		Set_State(SKILL4START);
		return;
	}

	if (CK_DOWN(DIK_E))
	{
		Set_State(SKILL5);
		return;
	}

	if (CK_DOWN(DIK_F))
	{
		Set_State(SKILL6);
		return;
	}

	if (CK_DOWN(DIK_T))
	{
		Set_State(SKILL7START);
		return;
	}

	if (CK_DOWN(DIK_G))
	{
		Set_State(SKILL12START);
		return;
	}


	if (CK_DOWN(DIK_2))
	{
		Set_State(SKILL9);
		return;
	}

	if (CK_DOWN(DIK_3))
	{
		Set_State(SKILL11START);
		return;
	}

	if (CK_DOWN(DIK_4))
	{
		Set_State(SKILL1);

		return;
	}

	if (CK_DOWN(DIK_5))
	{
		Set_State(SKILL2);
		return;
	}

	if (CK_DOWN(DIK_V))
	{
		Set_State(RAGESKILL1);
		return;
	}

	if (CK_DOWN(DIK_B))
	{
		Set_State(RAGESKILL2);
		return;
	}

	if (CK_DOWN(DIK_TAB))
	{
		Set_State(SKILL10);
		return;
	}

	
}

void CPlayer04::Attack1_KeyInput(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(1))
	{
		if (CM_PRESS(DIMK_LBUTTON))
		{
			Input_Direction();
			Set_State(ATTACK2);
			return;
		}
	}

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(2))
	{
		Combo_KeyInput(fTimeDelta);
	}
}

void CPlayer04::Attack2_KeyInput(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(1))
	{
		if (CM_PRESS(DIMK_LBUTTON))
		{
			Input_Direction();
			Set_State(ATTACK3);
			return;
		}
	}

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(2))
	{
		Combo_KeyInput(fTimeDelta);
	}
}

void CPlayer04::Attack3_KeyInput(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(1))
	{
		if (CM_PRESS(DIMK_LBUTTON))
		{
			Input_Direction();
			Set_State(ATTACK4);
			return;
		}
	}

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(2))
	{
		Combo_KeyInput(fTimeDelta);
	}
}

void CPlayer04::Attack4_KeyInput(_float fTimeDelta)
{
	if (CK_PRESS(DIK_CAPSLOCK))
	{
		m_bVoid = true;
		m_fVoidSpeed = 10.f;
		m_bSwordRender = false;
		m_bRenderPlayer = false;
		m_pModelCom->Set_AnimIndex(VOIDFRONT);
		m_eCurState = VOIDFRONT;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
		Create_Particle(PT_VOID);
	}

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(1))
	{
		if (CM_PRESS(DIMK_LBUTTON))
		{
			Input_Direction();
			Set_State(ATTACK5);
			return;
		}
	}

	
}

void CPlayer04::Attack5_KeyInput(_float fTimeDelta)
{

	if (CK_PRESS(DIK_CAPSLOCK))
	{
		m_bVoid = true;
		m_fVoidSpeed = 10.f;
		m_bSwordRender = false;
		m_bRenderPlayer = false;
		m_pModelCom->Set_AnimIndex(VOIDFRONT);
		m_eCurState = VOIDFRONT;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
		Create_Particle(PT_VOID);
	}
}

void CPlayer04::JumpStart_KeyInput(_float fTimeDelta)
{
	if (CM_DOWN(DIMK_LBUTTON))
	{
		Input_Direction();
		Set_State(AIRATTACK1);
		m_pTransformCom->ResetAndJump(1.5f);
		return;
	}

}

void CPlayer04::JumpLoof_KeyInput(_float fTimeDelta)
{
	if (CM_DOWN(DIMK_LBUTTON))
	{
		Input_Direction();
		Set_State(AIRATTACK1);
		m_pTransformCom->ResetAndJump(1.5f);
		return;
	}

}

void CPlayer04::AirAttack1_KeyInput(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(1))
	{
		if (CM_PRESS(DIMK_LBUTTON))
		{
			Input_Direction();
			Set_State(AIRATTACK2);
			m_pTransformCom->ResetAndJump(1.5f);
			return;
		}
	}

}

void CPlayer04::AirAttack2_KeyInput(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(1))
	{
		if (CM_PRESS(DIMK_LBUTTON))
		{
			Input_Direction();
			Set_State(AIRATTACK3);
			m_pTransformCom->ResetAndJump(1.5f);
			return;
		}
	}

}

void CPlayer04::AirAttack3_KeyInput(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(1))
	{
		if (CM_PRESS(DIMK_LBUTTON))
		{
			Input_Direction();
			Set_State(SKILL13END);
			m_pTransformCom->ResetAndJump(1.5f);
			return;
		}
	}

}

void CPlayer04::Skill11Loof_KeyInput(_float fTimeDelta)
{
	Input_Direction();

	if (CM_DOWN(DIMK_LBUTTON))
	{	
		m_pModelCom->Set_AnimIndex(SKILL11END);
		m_eCurState = SKILL11END;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
		return;
	}
}

void CPlayer04::Skill13End_KeyInput(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(1))
	{
		
	}
}

void CPlayer04::Combo_KeyInput(_float fTimeDelta)
{
	if (CK_PRESS(DIK_CAPSLOCK))
	{
		m_bVoid = true;
		m_fVoidSpeed = 10.f;
		m_bSwordRender = false;
		m_bRenderPlayer = false;
		m_pModelCom->Set_AnimIndex(VOIDFRONT);
		m_eCurState = VOIDFRONT;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
		Create_Particle(PT_VOID);
		return;
	}

	Change_Mode();

	if (CK_DOWN(DIK_LSHIFT))
	{
		Set_State(RUNFASTSTART);
		return;
	}

	if (CK_PRESS(DIK_R))
	{
		Set_State(SKILL3START);
		return;
	}

	if (CK_PRESS(DIK_Q))
	{
		Set_State(SKILL4START);
		return;
	}

	if (CK_PRESS(DIK_E))
	{
		Set_State(SKILL5);
		return;
	}

	if (CK_PRESS(DIK_F))
	{
		Set_State(SKILL6);
		return;
	}

	if (CK_PRESS(DIK_T))
	{
		Set_State(SKILL7START);
		return;
	}

	if (CK_PRESS(DIK_G))
	{
		Set_State(SKILL12START);
		return;
	}


	if (CK_PRESS(DIK_2))
	{
		Set_State(SKILL9);
		return;
	}

	if (CK_PRESS(DIK_3))
	{
		Set_State(SKILL11START);
		return;
	}

	if (CK_PRESS(DIK_4))
	{
		Set_State(SKILL1);
		return;
	}

	if (CK_PRESS(DIK_5))
	{
		Set_State(SKILL2);
		return;
	}

	if (CK_PRESS(DIK_V))
	{
		Set_State(RAGESKILL1);
		return;
	}

	if (CK_PRESS(DIK_B))
	{
		Set_State(RAGESKILL2);
		return;
	}

	if (CK_PRESS(DIK_TAB))
	{
		Set_State(SKILL10);
		return;
	}

	
}

void CPlayer04::Skill10_KeyInput(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(1))
	{
		Default_KeyInput(fTimeDelta);
	}
}

void CPlayer04::Skill9_KeyInput(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(1))
	{
		Default_KeyInput(fTimeDelta);
	}
}

void CPlayer04::Skill2_KeyInput(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(2))
	{
		Default_KeyInput(fTimeDelta);
	}
}

void CPlayer04::Skill5_KeyInput(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(4))
	{
		Default_KeyInput(fTimeDelta);
	}
}

void CPlayer04::Cancle_KeyInput(_float fTimeDelta)
{
	if (m_eCurState != RAGESKILL1 && m_eCurState != RAGESKILL2 && m_eCurState != GROGGYSTART && m_eCurState != STEPBACK)
	{
		if (CK_DOWN(DIK_CAPSLOCK))
		{
			GI->PlaySoundW(L"se_HE05_Specific_1.wav", SD_PLAYER4_SK, 0.8f);
			Input_Direction();
			Reset_Collider();
			m_bVoid = true;
			m_fVoidSpeed = 10.f;
			m_bSwordRender = false;
			m_bRenderPlayer = false;
			m_pModelCom->Set_AnimIndex(VOIDFRONT);
			m_eCurState = VOIDFRONT;
			m_eNextState = m_eCurState;
			m_pModelCom->SetNextIndex(m_eNextState);
			Create_Particle(PT_VOID);
			return;
		}

		if (CK_DOWN(DIK_R))
		{
			Input_Direction();
			m_bVoid = true;
			Reset_Collider();
			Set_State(SKILL3START);
			return;
		}

		if (CK_DOWN(DIK_T))
		{
			Input_Direction();
			m_bVoid = true;
			Reset_Collider();
			Set_State(SKILL7START);
			return;
		}

		if (CK_DOWN(DIK_1))
		{
			Input_Direction();
			m_bSkill8Effect = false;
			m_bVoid = true;
			Reset_Collider();
			Set_State(SKILL8);
			return;
		}
	}

}

void CPlayer04::Run_Update(_float fTimeDelta)
{
	m_pTransformCom->Go_Dir(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_fRunSpeed, fTimeDelta, m_pNavigationCom);

	m_fRunSoundAcc += 1.f * fTimeDelta;

	if (!m_bOtherFoot)
	{
		if (m_fRunSoundAcc >= 0.3f)
		{
			GI->PlaySoundW(L"se_flat_run_s_asphalt_01.wav", SD_PLAYER4_SK, 0.7f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = true;
		}
	}

	else
	{
		if (m_fRunSoundAcc >= 0.3f)
		{
			GI->PlaySoundW(L"se_flat_run_s_asphalt_02.wav", SD_PLAYER4_SK, 0.7f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = false;
		}
	}
}

void CPlayer04::Walk_Update(_float fTimeDelta)
{
}

void CPlayer04::AirAttack1_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (!m_pTransformCom->Get_Air())
	{
		m_pModelCom->Set_AnimIndex(JUMPEND);
		m_eCurState = JUMPEND;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
		return;
	}
	
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bAirAttack1Col)
	{
		GI->PlaySoundW(L"se_HE01_Nattack_1.wav", SD_PLAYER4_SK, 0.8f);
		GI->PlaySoundW(L"Hero05_ba_03.wav", SD_PLAYER4_VO, 1.f);
		CRM->Get_Cam()->Shake(0.05f, 0.15f);
		m_bAttackCol = true;
		m_bAirAttack1Col = true;
		Create_Effect(ET_AIRATTACK1);
	}



}

void CPlayer04::AirAttack2_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (!m_pTransformCom->Get_Air())
	{
		m_pModelCom->Set_AnimIndex(JUMPEND);
		m_eCurState = JUMPEND;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
		return;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bAirAttack2Col)
	{
		GI->PlaySoundW(L"se_HE01_Nattack_2.wav", SD_PLAYER4_SK, 0.8f);
		CRM->Get_Cam()->Shake(0.05f, 0.15f);
		m_bAttackCol = true;
		m_bAirAttack2Col = true;
		Create_Effect(ET_AIRATTACK2);
	}


}

void CPlayer04::AirAttack3_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (!m_pTransformCom->Get_Air())
	{
		m_pModelCom->Set_AnimIndex(JUMPEND);
		m_eCurState = JUMPEND;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
		return;
	}

	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(1))
		m_pTransformCom->ResetGravity();

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bAirAttack3Col)
	{
		GI->PlaySoundW(L"se_HE01_Nattack_3_01.wav", SD_PLAYER4_SK, 0.8f);
		GI->PlaySoundW(L"Hero05_ba_04.wav", SD_PLAYER4_VO, 1.f);
		CRM->Get_Cam()->Shake(0.05f, 0.15f);
		m_bAttackCol = true;
		m_bAirAttack3Col = true;
		Create_Effect(ET_AIRATTACK3);
	}

}

void CPlayer04::Attack1_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bAttack1Col)
	{
		GI->PlaySoundW(L"se_HE01_Nattack_1.wav", SD_PLAYER4_SK, 0.8f);
		GI->PlaySoundW(L"Hero05_ba_02.wav", SD_PLAYER4_VO, 1.f);
		m_bAttackCol = true;
		m_bAttack1Col = true;
		CRM->Get_Cam()->Shake(0.05f, 0.15f);
		Create_Effect(ET_ATTACK1);
	}



}

void CPlayer04::Attack2_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bAttack2Col)
	{
		GI->PlaySoundW(L"se_HE01_Nattack_2.wav", SD_PLAYER4_SK, 0.8f);
		m_bAttackCol = true;
		m_bAttack2Col = true;
		CRM->Get_Cam()->Shake(0.05f, 0.15f);
		Create_Effect(ET_ATTACK2);
	}
}

void CPlayer04::Attack3_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bAttack3Col)
	{
		GI->PlaySoundW(L"se_HE01_Nattack_3_01.wav", SD_PLAYER4_SK, 0.8f);
		GI->PlaySoundW(L"Hero05_ba_01.wav", SD_PLAYER4_VO, 1.f);
		m_bAttackCol = true;
		m_bAttack3Col = true;
		CRM->Get_Cam()->Shake(0.06f, 0.15f);
		Create_Effect(ET_ATTACK3);
	}
}

void CPlayer04::Attack4_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bAttack4Col)
	{
		GI->PlaySoundW(L"se_HE01_Nattack_3_02.wav", SD_PLAYER4_SK, 0.8f);
		GI->PlaySoundW(L"Hero05_ba_03.wav", SD_PLAYER4_VO, 1.f);
		m_bAttackCol = true;
		m_bAttack4Col = true;
		CRM->Get_Cam()->Shake(0.06f, 0.15f);
		Create_Effect(ET_ATTACK4);
	}
}

void CPlayer04::Attack5_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bAttack5Col)
	{
		GI->PlaySoundW(L"se_HE01_Skill02_3.wav", SD_PLAYER4_SK, 0.7f);
		GI->PlaySoundW(L"Hero05_ba_06.wav", SD_PLAYER4_VO, 1.f);
		m_bAttackCol = true;
		m_bAttack5Col = true;
		CRM->Get_Cam()->Shake(0.3f, 0.2f, true, 0.01f);
		CRM->Get_Cam()->Fov(-3.f, 0.1f, 0.3f, true);
		Create_Effect(ET_ATTACK5);
		PTM->Create_InstanceParticle(L"Player04AttackEnd", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player04Skill9_2", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player04Distortion", m_pTransformCom->Get_WorldFloat4x4());
		m_eShaderPass = SHADER_LIGHT;
		m_bPlayerLight = true;
		m_fPlayerLightLimit = 0.1f;

	}
}

void CPlayer04::Bounce_Update(_float fTimeDelta)
{
}

void CPlayer04::Broken_Update(_float fTimeDelta)
{
}

void CPlayer04::Burst_Update(_float fTimeDelta)
{
	if (!m_bNormalChange)
	{
		if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0))
		{
			m_bNormalChange = true;
			m_ePlayerMode = PMODE_BATTLE;
			Update_Parts();
			m_bPointTrail = false;
			((CPlayer04Hair*)m_Parts[PARTS_HAIR])->Set_Wake();
			((CPlayer04HairBattle*)m_Parts[PARTS_HAIRBATTLE])->Set_Sleep();
			((CPlayer04Hair*)m_Parts[PARTS_HAIR])->SetInitPos();
		}
	}
}

void CPlayer04::Dash_Update(_float fTimeDelta)
{
}

void CPlayer04::GroggyEnd_Update(_float fTimeDelta)
{
}

void CPlayer04::GroggyLoof_Update(_float fTimeDelta)
{
}

void CPlayer04::GroggyStart_Update(_float fTimeDelta)
{
}

void CPlayer04::Down_Update(_float fTimeDelta)
{
}

void CPlayer04::VoidFront_Update(_float fTimeDelta)
{
	if (!m_bVoidOn)
	{
		GI->PlaySoundW(L"se_HE05_Specific_1.wav", SD_PLAYER4_SK, 0.8f);
		m_bVoidOn = true;
		Input_Direction();
		PTM->Create_InstanceParticle(L"Player04VoidDistortion", m_pTransformCom->Get_WorldFloat4x4());
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0))
	{
		if (CM_PRESS(DIMK_LBUTTON))
		{
			m_bVoid = false;
			Input_Direction();
			m_bVoidAttackCol_1 = false; m_bVoidAttackCol_2 = false; m_bVoidAttackCol_3 = false;
			m_bVoidAttackCol_4 = false; m_bVoidAttackCol_5 = false;
			Update_Parts();
			((CPlayer04HairBattle*)m_Parts[PARTS_HAIRBATTLE])->SetInitPos();
			((CPlayer04Hair*)m_Parts[PARTS_HAIR])->SetInitPos();
			m_eHitType = TYPE_HIT;
			m_fPushPower = 3.f;
			m_fAirPower = 0.f;
			m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER04) * 0.3f;
			m_pModelCom->Set_AnimIndex(VOIDATTACK);
			m_eCurState = VOIDATTACK;
			m_eNextState = m_eCurState;
			m_pModelCom->SetNextIndex(m_eNextState);
			Change_ColRange(_float3{ 2.f,1.5f,2.f }, _float3{ 0.f,1.5f,1.f });
		}
	}

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(1)
		&& m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(2))
	{
		if (m_fVoidSpeed > 0.f)
		{
			m_fVoidSpeed -= 20.f * fTimeDelta;
			if (m_fVoidSpeed < 0.f)
				m_fVoidSpeed = 0.f;
		}

		m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), m_fVoidSpeed, fTimeDelta, m_pNavigationCom);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(3) + 2)
	{
		m_bRenderPlayer = true;
		m_bVoid = false;
		m_fPlayerLightLimit = 0.1f;
		m_bPlayerLight = true;
		Update_Parts();
		((CPlayer04HairBattle*)m_Parts[PARTS_HAIRBATTLE])->SetInitPos();
		((CPlayer04Hair*)m_Parts[PARTS_HAIR])->SetInitPos();
	}
}

void CPlayer04::VoidAir_Update(_float fTimeDelta)
{
}

void CPlayer04::VoidAttack_Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	m_bVoidOn = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0))
	{
		m_bSwordRender = true;
		
	}
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1))
	{
		GI->PlaySoundW(L"se_HE05_Hattack_3.wav", SD_PLAYER4_SK, 0.4f);
		m_bSwordRender = false;
		
	}
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bVoidAttackCol_1)
	{
		GI->PlaySoundW(L"se_HE05_Skill04_1.wav", SD_PLAYER4_SK, 0.8f);
		GI->PlaySoundW(L"Hero05_ba_31.wav", SD_PLAYER4_VO, 1.f);
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		m_bAttackCol = true;
		m_bVoidAttackCol_1 = true;
		m_bSwordTrail = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(3) && !m_bVoidAttackCol_2)
	{
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		m_bAttackCol = true;
		m_bVoidAttackCol_2 = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(4) && !m_bVoidAttackCol_3)
	{
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		m_bAttackCol = true;
		m_bVoidAttackCol_3 = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(5) && !m_bVoidAttackCol_4)
	{
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		m_bAttackCol = true;
		m_bVoidAttackCol_4 = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(6) && !m_bVoidAttackCol_5)
	{
		GI->PlaySoundW(L"se_HE05_Specific_2.wav", SD_PLAYER4_SK, 0.8f);
		CRM->Get_Cam()->Shake(0.2f, 0.1f, false, 0.1f);
		m_eHitType = TYPE_SPIN;
		m_fAirPower = 6.f;
		m_fPushPower = 5.f;
		m_bAttackCol = true;
		m_bVoidAttackCol_5 = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(6) + 3)
	{
		m_bSwordTrail = false;
	}

}

void CPlayer04::FinalBurst_Update(_float fTimeDelta)
{
	if (!m_bBurstChange)
	{
		if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0))
		{
			m_bPlayerLight = true;
			m_fPlayerLightLimit = 0.1f;
			m_vPlayerLightColor = { 0.f,0.5f,1.f };
			PTM->Create_InstanceParticle(L"Player04BurstParticle", m_pTransformCom->Get_WorldFloat4x4());

			CPlayer04VoidParticle::COLORINFO* Info = new CPlayer04VoidParticle::COLORINFO;
			Info->vColor = { 1.f,1.f,1.f,1.f };
			Info->vColor2 = { 0.f,0.2f,1.f,1.f };
			Info->vSize = { 1.f,1.f };
			GI->Add_GameObjectToLayer(L"Player04VoidParticle", TM->Get_CurLevel(), L"Layer_Particle", &Info);

			m_bBurstChange = true;
			m_ePlayerMode = PMODE_BURST;
			Update_Parts();
			m_bPointTrail = true;
			GI->PlaySoundW(L"HSChangeSound.ogg", SD_PLAYER4_SK, 1.f);
			((CPlayer04Hair*)m_Parts[PARTS_HAIR])->Set_Sleep();
			((CPlayer04HairBattle*)m_Parts[PARTS_HAIRBATTLE])->Set_Wake();
			((CPlayer04HairBattle*)m_Parts[PARTS_HAIRBATTLE])->SetInitPos();
			((CPlayer04Hair*)m_Parts[PARTS_HAIR])->Set_PlayerLightColor(_float3{ 0.f,0.5f,1.f });
			((CPlayer04HairBattle*)m_Parts[PARTS_HAIRBATTLE])->Set_PlayerLightColor(_float3{ 0.f,0.5f,1.f });
		}	
	}
}

void CPlayer04::FinishNormal_Update(_float fTimeDelta)
{
}

void CPlayer04::FinishWall_Update(_float fTimeDelta)
{
}

void CPlayer04::GetUp_Update(_float fTimeDelta)
{
}

void CPlayer04::Skill1_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bSkill1On)
	{
		GI->PlaySoundW(L"Hero05_ba_19.wav", SD_PLAYER4_VO, 1.f);
		m_bSwordRender = true;
		m_bSkill1On = true;
	}
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1))
	{
		GI->PlaySoundW(L"se_HE05_Hattack_3.wav", SD_PLAYER4_SK, 0.4f);
		m_bSwordRender = false;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bSkill1Col)
	{
		GI->PlaySoundW(L"CHR_AssassinBlades_Swish_01_A.ogg", SD_PLAYER4_SK, 0.8f);
		CRM->Get_Cam()->Shake(0.2f, 0.2f, false, 0.2f);
		m_bAttackCol = true;
		m_bSkill1Col = true;
		Create_ParticleInstance(PTI_SKILL1);
		Create_ParticleInstance(PTI_SKILL1_1);
		Create_ParticleInstance(PTI_SKILL1_2);
		
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(3)+3)
	{
		m_bSwordTrail = true;
		
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(4)+2)
	{
		m_bSwordTrail = false;	
	
	}


	
	
}

void CPlayer04::HurtBack_Update(_float fTimeDelta)
{
	if (CK_PRESS(DIK_CAPSLOCK))
	{
		m_bVoid = true;
		m_fVoidSpeed = 10.f;
		m_bSwordRender = false;
		m_bRenderPlayer = false;
		m_pModelCom->Set_AnimIndex(VOIDFRONT);
		m_eCurState = VOIDFRONT;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
		Create_Particle(PT_VOID);
	}
}

void CPlayer04::HurtFly_Update(_float fTimeDelta)
{
}

void CPlayer04::HurtFront_Update(_float fTimeDelta)
{
	if (CK_PRESS(DIK_CAPSLOCK))
	{
		m_bVoid = true;
		m_fVoidSpeed = 10.f;
		m_bSwordRender = false;
		m_bRenderPlayer = false;
		m_pModelCom->Set_AnimIndex(VOIDFRONT);
		m_eCurState = VOIDFRONT;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
		Create_Particle(PT_VOID);
	}
}

void CPlayer04::Idle1_Update(_float fTimeDelta)
{
}

void CPlayer04::Idle2_Update(_float fTimeDelta)
{
}

void CPlayer04::JumpBack_Update(_float fTimeDelta)
{
}

void CPlayer04::JumpLoof_Update(_float fTimeDelta)
{
	Input_Direction();
	if (!m_pTransformCom->Get_Air())
	{
		m_pModelCom->Set_AnimIndex(JUMPEND);
		m_eCurState = JUMPEND;
	}
	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), m_fJumpSpeed, fTimeDelta, m_pNavigationCom);
}

void CPlayer04::JumpStart_Update(_float fTimeDelta)
{
	Input_Direction();
	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), m_fJumpSpeed, fTimeDelta, m_pNavigationCom);
}

void CPlayer04::JumpEnd_Update(_float fTimeDelta)
{
}

void CPlayer04::QickStand_Update(_float fTimeDelta)
{
}

void CPlayer04::Skill2_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bSkill2Col_1)
	{
		GI->PlaySoundW(L"Hero05_ba_27.wav", SD_PLAYER4_VO, 1.f);
		/*GI->PlaySoundW(L"se_HE05_Skill06_1.wav", SD_PLAYER4_SK, 0.8f);
		GI->PlaySoundW(L"se_HE05_Skill01_5.wav", SD_PLAYER03_RE, 0.8f);*/
		GI->PlaySoundW(L"se_HE01_Nattack_3_01.wav", SD_PLAYER1_SK, 0.8f);
		m_bAttackCol = true;
		m_bSkill2Col_1 = true;
		Create_Effect(ET_SKILL2_1);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bSkill2Col_2)
	{
		/*GI->PlaySoundW(L"se_HE05_Skill06_1.wav", SD_PLAYER4_SK, 0.8f);
		GI->PlaySoundW(L"se_HE05_Skill01_5.wav", SD_PLAYER03_RE, 0.8f);*/
		GI->PlaySoundW(L"se_HE01_Hattack_1.wav", SD_PLAYER1_SK, 0.9f);
		m_eHitType = TYPE_FASTDOWN;
		m_fAirPower = -10.f;
		m_fPushPower = 4.f;
		m_bAttackCol = true;
		m_bSkill2Col_2 = true;
		Create_Effect(ET_SKILL2_2);
	}

}

void CPlayer04::Skill3Start_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	m_pTransformCom->ResetGravity();
	//점프 시작할때 
	//
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bSkill3On)
	{
		GI->PlaySoundW(L"se_HE05_Specific_2.wav", SD_PLAYER4_SK, 0.8f);
		GI->PlaySoundW(L"Hero05_ba_40.wav", SD_PLAYER4_VO, 1.f); 
		m_pModelCom->Set_TickPerSecond(42.f);

		m_bRenderPlayer = true;
		m_bVoid = false;
		m_fPlayerLightLimit = 0.1f;
		m_bPlayerLight = true;
		m_bSwordRender = true;
		m_bSkill3On = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bSkill3StartCol)
	{
		GI->PlaySoundW(L"se_HE05_Skill02_1.wav", SD_PLAYER4_SK, 0.8f);
		CRM->Get_Cam()->Shake(0.2f, 0.2f, false, 0.2f);
		m_bAttackCol = true;
		m_bSkill3StartCol = true;

		CPlayer04Slash2::SLASHINFO* Info = new CPlayer04Slash2::SLASHINFO;
		Info->vScale = { 0.8f,3.f,0.8f };
		Info->fBlurPower = 1.f;
		Info->fLimitTime = 2.f;
		Info->fTurnSpeed = -55.f;
		Info->vColor = { 1.f,1.f,1.f,1.f };
		Info->vColor2 = { 1.f,1.f,1.f, 1.f };
		Info->vRatio = { 1.f,1.f };
		Info->vRatioSpeed = { 3.f,3.f };
		Info->vScaleSpeed = { 0.f,0.f,0.f };
		Info->fAngle = 300.f;
		Info->Worldmatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player04Slash2", TM->Get_CurLevel(), L"Layer_Player04Effect", &Info);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bSkill3Jump)
	{
		m_bSkill3Jump = true;
		GI->PlaySoundW(L"se_HE05_Skill01_5.wav", SD_PLAYER4_SK, 0.8f);
	}
	
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(3) && !m_bSkill3TickPerSecond)
	{
		m_bSkill3TickPerSecond = true;
		m_pModelCom->Set_TickPerSecond(84.f);
	}
}

void CPlayer04::Skill3Loof_Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	m_pTransformCom->ResetGravity();
	

	if (!m_pTransformCom->Get_Air())
	{
		m_bSkill3EndCol = false;
		m_eHitType = TYPE_HIT;
		m_fAirPower = 0.f;
		m_fPushPower = 0.f;
		m_pModelCom->Set_AnimIndex(SKILL3END);
		m_eCurState = SKILL3END;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
	}

	if (m_pModelCom->Get_KeyFrame() == 0 && !m_bSkill3LoofCol)
	{
		m_bAttackCol = true;
		m_bSkill3LoofCol = true;
	}
}

void CPlayer04::Skill3End_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0))
	{
		GI->PlaySoundW(L"se_HE05_Hattack_3.wav", SD_PLAYER4_SK, 0.4f);
		m_bSwordRender = false;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bSkill3EndCol)
	{
		CRM->Get_Cam()->Shake(0.2f, 0.2f, false, 0.2f);
		m_bAttackCol = true;
		m_bSkill3EndCol = true;
	}
}

void CPlayer04::Skill4Start_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (!m_bSkill4Change)
	{
		if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0))
		{
			m_eHitType = TYPE_HIT;
			m_fPushPower = 3.f;
			m_fAirPower = 2.f;
			m_pModelCom->Set_AnimIndex(SKILL4END);
			m_eCurState = SKILL4END;
			m_eNextState = m_eCurState;
			m_pModelCom->SetNextIndex(m_eNextState);
			m_bSkill4Change = true;
			m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER04) * 1.5f;
		}
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bSkill4StartCol)
	{
		GI->PlaySoundW(L"se_HE02_Skill03_3.wav", SD_PLAYER4_SK, 0.8f);
		GI->PlaySoundW(L"Hero05_ba_33.wav", SD_PLAYER4_VO, 1.f);
		CRM->Get_Cam()->Shake(0.2f, 0.2f, false, 0.2f);
		CRM->Get_Cam()->Fov(-4.f, 0.15f, 0.f);
		m_bAttackCol = true;
		m_bSkill4StartCol = true;
		PTM->Create_InstanceParticle(L"Player04Skill4_2", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player04Skill4_3", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player04Skill4_4", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player04Skill4_5", m_pTransformCom->Get_WorldFloat4x4());		
		PTM->Create_InstanceParticle(L"Player04Skill4_7", m_pTransformCom->Get_WorldFloat4x4());
		m_eShaderPass = SHADER_LIGHT;
		m_bPlayerLight = true;
		m_fPlayerLightLimit = 0.1f;

		CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
		Info->fBlurPower = 0.5f;
		Info->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
		Info->vLocalPos = { 0.f,1.f,1.f,1.f };
		Info->vRatio = { 1.f,1.f };
		Info->vRatioSpeed = { 1.f,1.f };
		Info->vScale = { 0.1f,0.1f,0.1f };
		Info->vScaleSpeed = { 80.f,80.f,80.f };
		Info->vUvFixSpeed = { 1.f,3.f };
		Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
		
	}
	
}

void CPlayer04::Skill4End_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0))
	{
		m_bSwordRender = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && m_bSwordRender)
	{
		GI->PlaySoundW(L"se_HE05_Hattack_3.wav", SD_PLAYER4_SK, 0.4f);
		m_bSwordRender = false;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bSkill4EndCol)
	{
		GI->PlaySoundW(L"se_HE05_Special05.wav", SD_PLAYER4_SK, 0.8f);
		GI->PlaySoundW(L"Hero05_ba_46.wav", SD_PLAYER4_VO, 1.f);
		CRM->Get_Cam()->Shake(0.2f, 0.2f, false, 0.2f);
		m_bAttackCol = true;
		m_bSkill4EndCol = true;
		Create_ParticleInstance(PTI_SKILL1);
		Create_ParticleInstance(PTI_SKILL1_1);
		Create_ParticleInstance(PTI_SKILL1_2);		
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) - 2)
		m_bSwordTrail = true;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) + 4)
		m_bSwordTrail = false;


}

void CPlayer04::Skill5_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bSkill5Col_1)
	{
		
		m_bAttackCol = true;
		m_bSkill5Col_1 = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bSkill5Col_2)
	{
		m_bAttackCol = true;
		m_bSkill5Col_2 = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bSkill5Col_3)
	{
		m_bAttackCol = true;
		m_bSkill5Col_3 = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(3) && !m_bSkill5Col_4)
	{
		GI->PlaySoundW(L"se_HE01_Skill02_4.wav", SD_PLAYER4_SK, 0.8f);
		CRM->Get_Cam()->Shake(0.3f, 0.2f, false, 0.2f);
		m_eHitType = TYPE_UP;
		m_fAirPower = 6.f;
		m_fPushPower = 2.f;
		m_bAttackCol = true;
		m_bSkill5Col_4 = true;
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER04) * 1.2f;
		Create_Effect(ET_SKILL5);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(5) && !m_bSkill5Sound)
	{
		m_bSkill5Sound = true;
		GI->PlaySoundW(L"se_HE05_Skill04_1.wav", SD_PLAYER4_SK, 0.8f); // 다리 돌리기 시작할때 넣으면 지우기
		GI->PlaySoundW(L"Hero05_ba_36.wav", SD_PLAYER4_VO, 1.f); // 다리 돌리기 시작할때 넣으면 지우기
	}
	
}

void CPlayer04::Skill6_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0))	
		m_bSwordRender = true;
		
		

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1))	
		m_bSwordRender = false;
		
	
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) - 1)
	{
		m_bPlayerLight = true;
		m_fPlayerLightLimit = 0.1f;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bSkill6Col)
	{
		GI->PlaySoundW(L"CHR_AssassinBlades_Swish_06_C.ogg", SD_PLAYER4_SK, 0.5f);
		GI->PlaySoundW(L"Hero05_ba_41.wav", SD_PLAYER4_VO, 1.2f);
		CRM->Get_Cam()->Shake(0.2f, 0.2f, false, 0.2f);
		CRM->Get_Cam()->Fov(-4.f, 0.1f, 0.f);
		m_bAttackCol = true;
		m_bSkill6Col = true;
		
		m_ePushType = PULL;
		m_eHitType = TYPE_SPIN;
		m_fPushPower = 20.f;
		m_fAirPower = 3.f;
		
		PTM->Create_InstanceParticle(L"Player04Skill6", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player04Skill6_2", m_pTransformCom->Get_WorldFloat4x4());	
		PTM->Create_InstanceParticle(L"Player04Skill6_4", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player04Skill6_5", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player04Skill6_3", m_pTransformCom->Get_WorldFloat4x4());

		
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) + 1 && !m_bSkill6Effect)
	{
		
		m_bSkill6Effect = true;
		CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
		Info->fBlurPower = 0.5f;
		Info->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
		Info->vLocalPos = { 0.f,1.f,2.f,1.f };
		Info->vRatio = { 1.f,1.f };
		Info->vRatioSpeed = { 2.f,2.f };
		Info->vScale = { 0.1f,0.1f,0.1f };
		Info->vScaleSpeed = { 150.f,150.f,150.f };
		Info->vUvFixSpeed = { 1.f,3.f };
		Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);

		CPlayer04Col::COLINFO* Info2 = new CPlayer04Col::COLINFO;
		Info2->fLifeTime = 0.5f;
		Info2->vLocalPos = { 0.f,0.f,2.f,1.f };
		Info2->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player04Col", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2);

	}
}

void CPlayer04::Skill7Start_Update(_float fTimeDelta)
{
	// 도약 시작할때
	//GI->PlaySoundW(L"se_HE05_Skill06_2.wav", SD_PLAYER4_SK, 0.8f);

	if (!m_bSkill7Change)
	{
		if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0))
		{
			m_bSkill7Change = true;
			m_pModelCom->Set_AnimIndex(SKILL7END);
			m_eCurState = SKILL7END;
			m_eNextState = m_eCurState;
			m_pModelCom->SetNextIndex(m_eNextState);
			m_bSkill7EndCol = false;
			m_eHitType = TYPE_HIT;
			m_fAirPower = 0.f;
			m_fPushPower = 3.f;	
			m_bRenderPlayer = true;
			m_bVoid = false;
			m_bPlayerLight = true;
			m_fPlayerLightLimit = 0.1f;
		}
	}
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0))
		m_pModelCom->Set_TickPerSecond(1.f);

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(0) + 2)
		m_pModelCom->Set_TickPerSecond(100.f);

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(2) + 1 && m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(0))
	{
		m_pModelCom->Set_TickPerSecond(30.f);
		m_pTransformCom->Go_Dir(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 100.f, fTimeDelta, m_pNavigationCom);
	}
}

void CPlayer04::Skill7End_Update(_float fTimeDelta)
{

	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0))
	{
		m_bSwordRender = true;
		
	}
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1))
	{
		GI->PlaySoundW(L"se_HE05_Hattack_3.wav", SD_PLAYER4_SK, 0.4f);
		m_bSwordRender = false;
	
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bSkill7EndCol)
	{
		GI->PlaySoundW(L"Hero05_ba_39.wav", SD_PLAYER4_VO, 1.f);
		GI->PlaySoundW(L"CHR_AssassinBlades_Swish_01_G.ogg", SD_PLAYER4_SK, 0.8f);
		CRM->Get_Cam()->Shake(0.2f, 0.2f, false, 0.2f);
		m_bAttackCol = true;
		m_bSkill7EndCol = true;
		Create_ParticleInstance(PTI_SKILL1);
		Create_ParticleInstance(PTI_SKILL1_1);
		Create_ParticleInstance(PTI_SKILL1_2);
	
		
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) - 1)
		m_bSwordTrail = true;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) + 3)
		m_bSwordTrail = false;

	//베고 칼 돌릴때
	//GI->PlaySoundW(L"se_HE05_Special03.wav", SD_PLAYER4_SK, 0.8f);
}

void CPlayer04::Skill8_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	// 손 움직이기 시작할때
	//GI->PlaySoundW(L"se_HE05_Skill04_1.wav", SD_PLAYER4_SK, 0.8f);

	// 손 움직이는거 끝날때쯤(불기둥 돌아가기 전에 대사가 끝나도록)
	//GI->PlaySoundW(L"Hero05_ba_25.wav", SD_PLAYER4_VO, 1.f);

	// 불기둥 돌아갈때
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bSkill8Effect)
	{
		m_bSkill8Effect = true;
		m_bRenderPlayer = true;
		m_bVoid = false;
		m_fPlayerLightLimit = 0.1f;
		m_bPlayerLight = true;
		Default_KeyInput(fTimeDelta);
		
	}

	//불기둥 터질때
	//
}

void CPlayer04::SlapDown_Update(_float fTimeDelta)
{
}

void CPlayer04::Skill9_Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bSkill9Col)
	{
		GI->PlaySoundW(L"se_HE05_Skill07_2.wav", SD_PLAYER4_SK, 0.6f);
		GI->PlaySoundW(L"se_HE05_Special01_2.wav", SD_PLAYER03_RE, 0.6f);
		GI->PlaySoundW(L"Hero05_ba_26.wav", SD_PLAYER4_VO, 1.f);
		CRM->Get_Cam()->Shake(0.2f, 0.2f, false, 0.2f);
		m_bAttackCol = true;
		m_bSkill9Col = true;
		Create_Effect(ET_SKILL9);
		PTM->Create_InstanceParticle(L"Player04Skill9", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player04Skill9_2", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player04Distortion_2", m_pTransformCom->Get_WorldFloat4x4());
		m_eShaderPass = SHADER_LIGHT;
		m_bPlayerLight = true;
		m_fPlayerLightLimit = 0.1f;
	}
	
}

void CPlayer04::Skill10_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bSkill10Col)
	{
		//GI->PlaySoundW(L"se_HE05_Skill01_4.wav", SD_PLAYER4_SK, 0.6f);
		GI->PlaySoundW(L"se_HE01_Skill02_3.wav", SD_PLAYER1_SK, 0.8f);
		GI->PlaySoundW(L"Hero05_ba_48.wav", SD_PLAYER4_VO, 1.f);
		CRM->Get_Cam()->Shake(0.2f, 0.2f, false, 0.2f);
		CRM->Get_Cam()->Fov(3.f, 0.2f, 0.f);
		m_bAttackCol = true;
		m_bSkill10Col = true;
		Create_Effect(ET_SKILL10);
		PTM->Create_InstanceParticle(L"Player04Skill9", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player04Skill9_2", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player04Distortion_2", m_pTransformCom->Get_WorldFloat4x4());
		m_eShaderPass = SHADER_LIGHT;
		m_bPlayerLight = true;
		m_fPlayerLightLimit = 0.1f;
	}
}

void CPlayer04::Skill11Start_Update(_float fTimeDelta)
{
	//칼 휘릭 할때
	//GI->PlaySoundW(L"se_HE05_Special03.wav", SD_PLAYER4_SK, 0.8f);
}

void CPlayer04::Skill11Loof_Update(_float fTimeDelta)
{
	if (m_bSkill11ChargeEnd)
	{
		m_fSkill11ChargeEndAcc += 1.f * fTimeDelta;
		if (m_fSkill11ChargeEndAcc >= 0.7f)
		{
			m_fSkill11ChargeEndAcc = 0.f;
			m_bSkill11ChargeEnd = false;
		}

	}

	else
	{
		if (m_iChargeCount < 3)
		{
			m_fChargeAcc += 1.f * fTimeDelta;
			if (m_fChargeAcc >= 0.3f && !m_bSkill11Charge1)
			{
				m_bSkill11Charge1 = true;
				PTM->Create_InstanceParticle(L"Player04BallCharge", m_pTransformCom->Get_WorldFloat4x4());
				PTM->Create_InstanceParticle(L"Player04ChargeDistortion", m_pTransformCom->Get_WorldFloat4x4());
				GI->PlaySoundW(L"SE_BTL_103.wav", SD_PLAYER4_SK, 0.8f);
			}

			else if (m_fChargeAcc >= 0.6f && !m_bSkill11Charge2)
			{
				m_bSkill11Charge2 = true;
				PTM->Create_InstanceParticle(L"Player04BallCharge_2", m_pTransformCom->Get_WorldFloat4x4());
				PTM->Create_InstanceParticle(L"Player04ChargeDistortion", m_pTransformCom->Get_WorldFloat4x4());
				GI->PlaySoundW(L"SE_BTL_103.wav", SD_PLAYER4_SK, 0.8f);
			}

			else if (m_fChargeAcc >= 0.8f && !m_bSkill11Charge3)
			{
				m_bSkill11Charge3 = true;
				PTM->Create_InstanceParticle(L"Player04BallCharge", m_pTransformCom->Get_WorldFloat4x4());
				PTM->Create_InstanceParticle(L"Player04ChargeDistortion", m_pTransformCom->Get_WorldFloat4x4());
				GI->PlaySoundW(L"SE_BTL_103.wav", SD_PLAYER4_SK, 0.8f);
			}

			else if (m_fChargeAcc >= 1.2f && !m_bSkill11Charge4)
			{
					
				m_bSkill11Charge4 = true;	
				m_bPlayerLight = true;
			
				switch (m_iChargeCount + 1)
				{
				case 1:
				{
					GI->PlaySoundW(L"se_HE05_Skill01_3.wav", SD_PLAYER4_SK, 0.8f);
					CRM->Get_Cam()->Shake(0.15f, 0.2f, false, 0.1f);
					CRM->Get_Cam()->Fov(1.5f, 0.1f, 0.f);
					CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
					Info->fBlurPower = 0.5f;
					Info->vColor = { 0.f,0.f,0.f,1.f };
					Info->vColor2 = { 0.f,0.f,0.f,1.f };
					Info->vLocalPos = { 0.f,1.f,0.f,1.f };
					Info->vRatio = { 1.f,1.f };
					Info->vRatioSpeed = { 2.f,2.f };
					Info->vScale = { 0.1f,0.1f,0.1f };
					Info->vScaleSpeed = { 150.f,150.f,150.f };
					Info->vUvFixSpeed = { 1.f,3.f };
					Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
					GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
					PTM->Create_AlphaParticle(L"Player04Charge", m_pTransformCom->Get_WorldFloat4x4());

					CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
					Info3->fBlurPower = 5.f;
					Info3->vScale = { 0.1f,1.f,0.1f };
					Info3->vLocalPos = { 0.f,0.f,0.f,1.f };
					Info3->vColor = { 0.f,0.f,0.f,1.f };
					Info3->vColor2 = { 0.f,0.f,0.f,1.f };
					Info3->vRatio = { 1.f,1.f };
					Info3->vRatioSpeed = { 1.5f,1.5f };
					Info3->vScaleSpeed = { 60.f,60.f,60.f };
					Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
					GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3);
					

					CPlayer01Wind2::WIND2INFO* Info4 = new CPlayer01Wind2::WIND2INFO;
					Info4->fBlurPower = 5.f;
					Info3->vScale = { 0.1f,1.f,0.1f };
					Info4->vLocalPos = { 0.f,0.f,0.f,1.f };
					Info4->vColor = { 0.f,0.f,0.f,1.f };
					Info4->vColor2 = { 0.f,0.f,0.f,1.f };
					Info4->vRatio = { 1.f,1.f };
					Info4->vRatioSpeed = { 1.5f,1.5f };
					Info4->vScaleSpeed = { 60.f,60.f,60.f };
					Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
					GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4);
			
				}
					break;
				case 2:
				{
					GI->PlaySoundW(L"se_HE05_Skill01_3.wav", SD_PLAYER4_SK, 0.8f);
					CRM->Get_Cam()->Shake(0.2f, 0.2f, false, 0.1f);
					CRM->Get_Cam()->Fov(2.f, 0.1f, 0.f);
					CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
					Info->fBlurPower = 0.5f;
					Info->vColor = { 0.f,0.f,0.f,1.f };
					Info->vColor2 = { 0.f,0.f,0.f,1.f };
					Info->vLocalPos = { 0.f,1.f,0.f,1.f };
					Info->vRatio = { 1.f,1.f };
					Info->vRatioSpeed = { 2.f,2.f };
					Info->vScale = { 0.1f,0.1f,0.1f };
					Info->vScaleSpeed = { 200.f,200.f,200.f };
					Info->vUvFixSpeed = { 1.f,3.f };
					Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
					GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
					PTM->Create_AlphaParticle(L"Player04Charge_2", m_pTransformCom->Get_WorldFloat4x4());

					CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
					Info3->fBlurPower = 5.f;
					Info3->vScale = { 0.1f,3.f,0.1f };
					Info3->vLocalPos = { 0.f,0.f,0.f,1.f };
					Info3->vColor = { 0.f,0.f,0.f,1.f };
					Info3->vColor2 = { 0.f,0.f,0.f,1.f };
					Info3->vRatio = { 1.f,1.f };
					Info3->vRatioSpeed = { 1.5f,1.5f };
					Info3->vScaleSpeed = { 80.f,80.f,80.f };
					Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
					GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3);


					CPlayer01Wind2::WIND2INFO* Info4 = new CPlayer01Wind2::WIND2INFO;
					Info4->fBlurPower = 5.f;
					Info4->vScale = { 0.1f,3.f,0.1f };
					Info4->vLocalPos = { 0.f,0.f,0.f,1.f };
					Info4->vColor = { 0.f,0.f,0.f,1.f };
					Info4->vColor2 = { 0.f,0.f,0.f,1.f };
					Info4->vRatio = { 1.f,1.f };
					Info4->vRatioSpeed = { 1.5f,1.5f };
					Info4->vScaleSpeed = { 80.f,80.f,80.f };
					Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
					GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4);

					break;
				}
				case 3:
				{
					GI->PlaySoundW(L"se_HE05_Skill01_4.wav", SD_PLAYER4_SK, 0.8f);
					CRM->Get_Cam()->Shake(0.25f, 0.2f, false, 0.1f);
					CRM->Get_Cam()->Fov(2.5f, 0.1f, 0.f);
					CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
					Info->fBlurPower = 0.5f;
					Info->vColor = { 0.f,0.f,0.f,1.f };
					Info->vColor2 = { 0.f,0.f,0.f,1.f };
					Info->vLocalPos = { 0.f,1.f,0.f,1.f };
					Info->vRatio = { 1.f,1.f };
					Info->vRatioSpeed = { 2.f,2.f };
					Info->vScale = { 0.1f,0.1f,0.1f };
					Info->vScaleSpeed = { 250.f,250.f,250.f };
					Info->vUvFixSpeed = { 1.f,3.f };
					Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
					GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
					PTM->Create_AlphaParticle(L"Player04Charge_3", m_pTransformCom->Get_WorldFloat4x4());

					CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
					Info3->fBlurPower = 5.f;
					Info3->vScale = { 0.1f,4.f,0.1f };
					Info3->vLocalPos = { 0.f,0.f,0.f,1.f };
					Info3->vColor = { 0.f,0.f,0.f,1.f };
					Info3->vColor2 = { 0.f,0.f,0.f,1.f };
					Info3->vRatio = { 1.f,1.f };
					Info3->vRatioSpeed = { 1.5f,1.5f };
					Info3->vScaleSpeed = { 100.f,100.f,100.f };
					Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
					GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3);


					CPlayer01Wind2::WIND2INFO* Info4 = new CPlayer01Wind2::WIND2INFO;
					Info4->fBlurPower = 5.f;
					Info3->vScale = { 0.1f,4.f,0.1f };
					Info4->vLocalPos = { 0.f,0.f,0.f,1.f };
					Info4->vColor = { 0.f,0.f,0.f,1.f };
					Info4->vColor2 = { 0.f,0.f,0.f,1.f };
					Info4->vRatio = { 1.f,1.f };
					Info4->vRatioSpeed = { 1.5f,1.5f };
					Info4->vScaleSpeed = { 100.f,100.f,100.f };
					Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
					GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4);

					break;
				}
			
				}
			}

			/*else if (m_fChargeAcc >= 1.55f && !m_bSkill11PlayerLight)
			{
				m_bSkill11PlayerLight = true;
				m_bPlayerLight = true;
				
			}*/
			else if (m_fChargeAcc >= 1.3f)
			{
				m_fChargeAcc = 0.f;
				m_fSkill11ChargeEndAcc = 0.f;
				m_bSkill11ChargeEnd = true;
				m_bSkill11Charge1 = false;
				m_bSkill11Charge2 = false;
				m_bSkill11Charge3 = false;
				m_bSkill11Charge4 = false;
				m_bSkill11PlayerLight = false;
				++m_iChargeCount;
				
				switch (m_iChargeCount)
				{
				case 1:
				{					
					CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
					Info->fBlurPower = 0.5f;
					Info->vColor = { 0.f,0.f,0.f,1.f };
					Info->vColor2 = { 0.f,0.f,0.f,1.f };
					Info->vLocalPos = { 0.f,1.f,0.f,1.f };
					Info->vRatio = { 1.f,1.f };
					Info->vRatioSpeed = { 2.f,2.f };
					Info->vScale = { 0.1f,0.1f,0.1f };
					Info->vScaleSpeed = { 200.f,200.f,200.f };
					Info->vUvFixSpeed = { 1.f,-3.f };
					Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
					GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
				
				}
				break;
				case 2:
				{			
					CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
					Info->fBlurPower = 0.5f;
					Info->vColor = { 0.f,0.f,0.f,1.f };
					Info->vColor2 = { 0.f,0.f,0.f,1.f };
					Info->vLocalPos = { 0.f,1.f,0.f,1.f };
					Info->vRatio = { 1.f,1.f };
					Info->vRatioSpeed = { 2.f,2.f };
					Info->vScale = { 0.1f,0.1f,0.1f };
					Info->vScaleSpeed = { 250.f,250.f,250.f };
					Info->vUvFixSpeed = { 1.f,-3.f };
					Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
					GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
					
					break;
				}
				case 3:
				{				
					CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
					Info->fBlurPower = 0.5f;
					Info->vColor = { 0.f,0.f,0.f,1.f };
					Info->vColor2 = { 0.f,0.f,0.f,1.f };
					Info->vLocalPos = { 0.f,1.f,0.f,1.f };
					Info->vRatio = { 1.f,1.f };
					Info->vRatioSpeed = { 2.f,2.f };
					Info->vScale = { 0.1f,0.1f,0.1f };
					Info->vScaleSpeed = { 300.f,300.f,300.f };
					Info->vUvFixSpeed = { 1.f,-3.f };
					Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
					GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
				
					break;
				}				
				}

			}
		}
	}
}

void CPlayer04::Skill11End_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0))
	{
		m_bSwordRender = true;
	}
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1))
	{
		GI->PlaySoundW(L"se_HE05_Hattack_3.wav", SD_PLAYER4_SK, 0.4f);
		m_bSwordRender = false;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bSkill11EndCol)
	{
		GI->PlaySoundW(L"Hero05_ba_08.wav", SD_PLAYER4_VO, 1.f);
		GI->PlaySoundW(L"CHR_AssassinBlades_Swish_01_E.ogg", SD_PLAYER4_SK, 0.8f);
		CRM->Get_Cam()->Shake(0.4f, 0.2f, false, 0.1f);
		m_bAttackCol = true;
		m_bSkill11EndCol = true;
		Create_ParticleInstance(PTI_SKILL1);
		Create_ParticleInstance(PTI_SKILL1_1);
		Create_ParticleInstance(PTI_SKILL1_2);
		Create_Effect(ET_SKILL11);
	
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) - 2)
		m_bSwordTrail = true;
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) + 3)
		m_bSwordTrail = false;
}

void CPlayer04::Skill12Start_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0))
	{
		m_bSwordRender = true;
		m_bSwordTrail = true;
	}
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1))
	{
		Input_Direction();
		m_pModelCom->Set_AnimIndex(SKILL12END);
		m_eCurState = SKILL12END;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
		m_fSkill12Speed = 10.f;
		m_eHitType = TYPE_AWAY;
		m_fPushPower = 20.f;
		m_fAirPower = 0.f;
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER04) * 1.2f;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) - 3)
		m_bSwordTrail = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bSkill12StartCol)
	{
		GI->PlaySoundW(L"CHR_AssassinBlades_Swish_05_A.ogg", SD_PLAYER4_SK, 0.8f);
		GI->PlaySoundW(L"Hero05_ba_40.wav", SD_PLAYER4_VO, 1.f);
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		m_bSkill12StartCol = true;
		CPlayer04Slash::SLASHINFO* Info = new CPlayer04Slash::SLASHINFO;
		Info->vScale = { 0.5f,1.f,0.5f };
		Info->Worldmatrix = m_pTransformCom->Get_WorldFloat4x4();
		Info->Worldmatrix._42 += 1.f;
		GI->Add_GameObjectToLayer(L"Player04Slash", TM->Get_CurLevel(), L"Layer_Player04Effect", &Info);
		Create_ParticleInstance(PTI_SKILL12);
		Create_ParticleInstance(PTI_SKILL12_1);	
	}
}

void CPlayer04::Skill12End_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() < m_pModelCom->Get_TimeLimit(0))
	{
		if (m_fSkill12Speed > 0.f)
			m_fSkill12Speed -= 30.f * fTimeDelta;

		if (m_fSkill12Speed <= 0.f)
			m_fSkill12Speed = 0.f;
		m_pTransformCom->Go_Dir(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_fSkill12Speed, fTimeDelta, m_pNavigationCom);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) - 4)
		m_bSwordTrail = true;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) + 5)
		m_bSwordTrail = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bSkill12EndCol)
	{
		GI->PlaySoundW(L"CHR_AssassinBlades_Swish_05_B.ogg", SD_PLAYER4_SK, 0.8f);
		GI->PlaySoundW(L"Hero05_ba_54.wav", SD_PLAYER4_VO, 1.f);
		CRM->Get_Cam()->Shake(0.2f, 0.2f, false, 0.2f);
		CRM->Get_Cam()->Fov(-3.f, 0.1f, 0.f);
		m_bSkill12EndCol = true;
		CPlayer04Slash::SLASHINFO* Info = new CPlayer04Slash::SLASHINFO;
		Info->vScale = { 1.5f,3.f,1.5f };
		Info->Worldmatrix = m_pTransformCom->Get_WorldFloat4x4();
		Info->Worldmatrix._42 += 1.f;
		GI->Add_GameObjectToLayer(L"Player04Slash", TM->Get_CurLevel(), L"Layer_Player04Effect", &Info);
		Create_ParticleInstance(PTI_SKILL12);
		Create_ParticleInstance(PTI_SKILL12_1);
	
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1))
	{
		GI->PlaySoundW(L"se_HE05_Hattack_3.wav", SD_PLAYER4_SK, 0.4f);
		m_bSwordRender = false;
	}
}

void CPlayer04::Skill13Start_Update(_float fTimeDelta)
{
}

void CPlayer04::Skill13End_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() == 1)
		m_bSwordRender = true;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bSkill13End_Col)
	{
		GI->PlaySoundW(L"se_HE05_Special05.wav", SD_PLAYER4_SK, 0.6f);
		GI->PlaySoundW(L"Hero05_ba_05.wav", SD_PLAYER4_VO, 1.f);
		m_bAttackCol = true;
		m_bSkill13End_Col = true;
	}

	if (m_pModelCom->Get_KeyFrame() < m_pModelCom->Get_TimeLimit(0))
		m_pTransformCom->ResetGravity();

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0))
	{
		m_bSwordRender = false;
	}
}

void CPlayer04::RunFastEnd_Update(_float fTimeDelta)
{
}

void CPlayer04::RunFastLoof_Update(_float fTimeDelta)
{
	m_pTransformCom->Go_Dir(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_fRunSpeed, fTimeDelta, m_pNavigationCom);

	m_fRunSoundAcc += 1.f * fTimeDelta;

	if (!m_bOtherFoot)
	{
		if (m_fRunSoundAcc >= 0.23f)
		{
			GI->PlaySoundW(L"se_flat_run_s_asphalt_01.wav", SD_PLAYER4_SK, 0.7f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = true;
		}
	}

	else
	{
		if (m_fRunSoundAcc >= 0.23f)
		{
			GI->PlaySoundW(L"se_flat_run_s_asphalt_02.wav", SD_PLAYER4_SK, 0.7f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = false;
		}
	}
}

void CPlayer04::RunFastStart_Update(_float fTimeDelta)
{
}

void CPlayer04::StepBack_Update(_float fTimeDelta)
{
}

void CPlayer04::Stun_Update(_float fTimeDelta)
{
}

void CPlayer04::StunEnd_Update(_float fTimeDelta)
{
}

void CPlayer04::RageSkill1_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	// GI->PlaySoundW(L"se_HE05_Super01_1.wav", SD_PLAYER4_SK, 0.8f); //시작하자마자 작게 돌때

	if (m_pModelCom->Get_KeyFrame() == 0 && !m_bHeroTest_1)
	{
		m_bHeroTest_1 = true;

		CRM->Get_Cam()->Save_World();
		CRM->Play_CutScene(13, true, false, m_pTransformCom);
	}
	if (m_pModelCom->Get_KeyFrame() == 63 && !m_bHeroTest_2)
	{
		m_bHeroTest_2 = true;

		CRM->End_Cut();

		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(0.f, 0.f, 0.f);
		CamDesc.vLook = _float3(0.f, 0.f, 0.f);
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 10.f;
		CamDesc.fLookLerpSpeed = 10.f;
		CamDesc.vPivotLookPos = _float3(0.f, 1.f, 5.f);
		CamDesc.pTran = m_pTransformCom;
		CamDesc.vPosDis = _float3(0.f, 3.5f, -4.f);
		CamDesc.bPixLocal = true;
		CRM->Get_Cam()->Show_Target(CamDesc);
		
	}
	if (m_pModelCom->Get_KeyFrame() == 113 && !m_bHeroTest_3)
	{
		m_bHeroTest_3 = true;


		CRM->Get_Cam()->Load_World();
		CRM->Get_Cam()->End_Target();
	}
	//63 칼뽑기  113 끝



	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0))
	{
		if (!m_bRangeSkill1On)
		{
			GI->PlaySoundW(L"se_HE05_Super01_2.wav", SD_PLAYER4_SK, 0.8f);
			GI->PlaySoundW(L"Hero05_ba_57.wav", SD_PLAYER4_VO, 1.f);
			m_bRangeSkill1On = true;
		}
		m_bSwordRender = true;
		m_fNowBP = 0.f;
	}
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bRageSkill1Effect2)
	{
		GI->PlaySoundW(L"se_HE05_Specific_2.wav", SD_PLAYER4_SK, 0.8f);
		m_bRageSkill1Effect2 = true;
		m_bSwordRender = false;
		Create_Particle(PT_VOID);
		m_bRenderPlayer = false;
		m_bPointTrail = false;
	
		m_eHE = HE_RAGE1;
		m_bAttackCol = true;
		Create_Effect(ET_RAGESKILL1_2);
		Create_Effect(ET_RAGESKILLCUT);
		m_pModelCom->Set_TickPerSecond(0.f);
	
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) + 1)
	{
		m_eHE = HE_1;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bRageSkill1Effect)
	{
		PTM->Create_AlphaParticle(L"Player04RageSkill1", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player04RageSkill1_2", m_pTransformCom->Get_WorldFloat4x4());
		GI->PlaySoundW(L"se_enemy_warning.wav", SD_PLAYER3_SK, 0.5f);
	}
	
}

void CPlayer04::RageSkill2_Update(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0))
	{
		m_bSwordRender = false;
		m_fNowBP = 0.f;
	}
}

void CPlayer04::FallDown_Update(_float fTimeDelta)
{
	if (!m_pTransformCom->Get_Air())
	{
		m_pModelCom->Set_AnimIndex(BOUNCE);
		m_eCurState = BOUNCE;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
		m_pTransformCom->ResetAndJump(1.f);
	}

	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vPushLook), m_fPushPower, fTimeDelta, m_pNavigationCom);
}

void CPlayer04::HitAway_Update(_float fTimeDelta)
{
	if (!m_pTransformCom->Get_Air())
	{
		Set_State(BOUNCE);
	}

	if (m_fPushPower > 5.f)
		m_fPushPower -= 5.f * fTimeDelta;

	if (m_fPushPower <= 5.f)
		m_fPushPower = 5.f;

	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vPushLook), m_fPushPower, fTimeDelta, m_pNavigationCom);
}

void CPlayer04::HitSpin_Update(_float fTimeDelta)
{
	if (!m_pTransformCom->Get_Air())
	{
		Set_State(BOUNCE);
	}

	if (m_fPushPower >= 0.f)
		m_fPushPower -= 3.f * fTimeDelta;

	if (m_fPushPower <= 0.f)
		m_fPushPower = 0.f;

	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vPushLook), m_fPushPower, fTimeDelta, m_pNavigationCom);
}

void CPlayer04::HitDown_Update(_float fTimeDelta)
{
}

void CPlayer04::Create_HE_1(_float4 vWorldPos)
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
	HitEffect->vColor2 = { 1.f,0.5f,0.f,1.f };
	GI->Add_GameObjectToLayer(L"Player01HitEffect_1", GI->Get_Cur_LevelIndex(), L"Layer_PlayerEffect", &HitEffect);



	_matrix Matrix = XMMatrixIdentity();
	Matrix.r[3] = XMLoadFloat4(&vPos);
	_float4x4 Matrix2;
	XMStoreFloat4x4(&Matrix2, Matrix);
	PTM->Create_InstanceParticle(L"Player01Hit1", Matrix2);
	PTM->Create_InstanceParticle(L"Player01Hit1_2", Matrix2);
}

void CPlayer04::Create_HE_RAGE1(_float4 vWorldPos)
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
	HitEffect->vColor2 = { 1.f,0.5f,0.f,1.f };
	GI->Add_GameObjectToLayer(L"Player01HitEffect_1", GI->Get_Cur_LevelIndex(), L"Layer_PlayerEffect", &HitEffect);

	_matrix Matrix = XMMatrixIdentity();
	Matrix.r[3] = XMLoadFloat4(&vPos);
	_float4x4 Matrix2;
	XMStoreFloat4x4(&Matrix2, Matrix);
	PTM->Create_InstanceParticle(L"Player01Hit1", Matrix2);
	PTM->Create_InstanceParticle(L"Player01Hit1_2", Matrix2);

	CPlayer04Rage1Particle::RAGEPARTICLEINFO* Info = new CPlayer04Rage1Particle::RAGEPARTICLEINFO;
	Info->vCreatePos = vWorldPos;
	GI->Add_GameObjectToLayer(L"Player04Rage1Particle", TM->Get_CurLevel(), L"Layer_Player04Effect", &Info);
	EM->Set_RageParticleOn();
	EM->Add_RageParticleCount();

}

void CPlayer04::Create_LightEffect()
{
	switch (m_eCurState)
	{
	case Client::CPlayer04::VOIDFRONT:
		Create_PlayerVoidParticle();
		PTM->Create_InstanceParticle(L"Player04VoidDistortion", m_pTransformCom->Get_WorldFloat4x4());
		break;
	case Client::CPlayer04::FINALBURST:
		break;
	case Client::CPlayer04::SKILL9:
		Create_PlayerVoidParticle();
		break;
	case Client::CPlayer04::SKILL4START:
		Create_PlayerVoidParticle();
		break;
	case Client::CPlayer04::SKILL10:
		Create_PlayerVoidParticle();
		break;
	case Client::CPlayer04::ATTACK5:
		Create_PlayerVoidParticle();
		break;
	case Client::CPlayer04::SKILL8:
		Create_PlayerVoidParticle();
		break;
	case Client::CPlayer04::SKILL1:
		break;
	case Client::CPlayer04::SKILL2:
		break;
	case Client::CPlayer04::SKILL3START:
		Create_PlayerVoidParticle();
		PTM->Create_InstanceParticle(L"Player04VoidDistortion", m_pTransformCom->Get_WorldFloat4x4());
		break;
	case Client::CPlayer04::SKILL3LOOF:
		break;
	case Client::CPlayer04::SKILL3END:
		break;
	case Client::CPlayer04::SKILL6:
		Create_PlayerVoidParticle();
		PTM->Create_InstanceParticle(L"Player04VoidDistortion", m_pTransformCom->Get_WorldFloat4x4());
		break;
	case Client::CPlayer04::SKILL7END:
		Create_PlayerVoidParticle();
		PTM->Create_InstanceParticle(L"Player04VoidDistortion", m_pTransformCom->Get_WorldFloat4x4());
		break;
	case Client::CPlayer04::SKILL11LOOF:
		Create_PlayerVoidParticle();
		PTM->Create_InstanceParticle(L"Player04ChargeParticle", m_pTransformCom->Get_WorldFloat4x4());
		break;
	case Client::CPlayer04::RAGESKILL1:
		Create_PlayerVoidParticle();
		PTM->Create_InstanceParticle(L"Player04VoidDistortion", m_pTransformCom->Get_WorldFloat4x4());
		break;
	case Client::CPlayer04::RAGESKILL2:
		break;
	}
}

void CPlayer04::Create_PlayerVoidParticle()
{
	CPlayer04VoidParticle::COLORINFO* Info = new CPlayer04VoidParticle::COLORINFO;
	Info->fY = m_fVoidParticleY;
	Info->vColor = m_vVoidParticleColor;
	Info->vColor2 = m_vVoidParticleColor2;
	GI->Add_GameObjectToLayer(L"Player04VoidParticle", TM->Get_CurLevel(), L"Layer_Particle", &Info);
		
}

HRESULT CPlayer04::Ready_Components()
{

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Player04Body"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	m_pModelCom->Set_RootHierarchy(1);

	/* For.Com_Navigation */
	CNavigation::NAVIGATIONDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = 0;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage1Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vCenter = _float3(0.f, 1.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(30.f, 30.f, 30.f);
	strcpy(ColDesc.sTag, "Player_Attack");
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;

	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.8f, 0.f);
	ColDesc.vSize = _float3(0.7f, 1.6f, 0.7f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Player_Body");
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;

	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.3f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "Player_Push_Range");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	
	CHierarchyNode*		pPlayer_Body = m_pModelCom->Get_HierarchyNode("Chest_M");
	if (nullptr == pPlayer_Body)
		return E_FAIL;

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(2.f, 2.f, 2.f);
	ColDesc.vCenter = _float3(0.f, 1.f, 0.f);
	ColDesc.pSocket = pPlayer_Body;
	XMStoreFloat4x4(&ColDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	ColDesc.pOwner = this;
	ColDesc.iFlag = 10;
	strcpy(ColDesc.sTag, "Player_HairCut");
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;



	return S_OK;
}

HRESULT CPlayer04::Ready_Sockets()
{
	CHierarchyNode*		pHead = m_pModelCom->Get_HierarchyNode("Head_M");
	if (nullptr == pHead)
		return E_FAIL;
	m_Sockets.push_back(pHead);

	CHierarchyNode*		pWeaponR = m_pModelCom->Get_HierarchyNode("Weapon_R");
	if (nullptr == pWeaponR)
		return E_FAIL;
	m_Sockets.push_back(pWeaponR);

	CHierarchyNode*		pWeaponBack = m_pModelCom->Get_HierarchyNode("WeaponPos_M");
	if (nullptr == pWeaponBack)
		return E_FAIL;
	m_Sockets.push_back(pWeaponBack);

	CHierarchyNode*		pKickL = m_pModelCom->Get_HierarchyNode("Toes_L");
	if (nullptr == pKickL)
		return E_FAIL;
	m_Sockets.push_back(pKickL);

	CHierarchyNode*		pKickR = m_pModelCom->Get_HierarchyNode("Toes_R");
	if (nullptr == pKickR)
		return E_FAIL;
	m_Sockets.push_back(pKickR);

	CHierarchyNode*		pBody = m_pModelCom->Get_HierarchyNode("RootPart2_M");
	if (nullptr == pBody)
		return E_FAIL;
	m_Sockets.push_back(pBody);

	return S_OK;
}

HRESULT CPlayer04::Ready_PlayerParts()
{

	CGameObject*		pPlayer04Hair = GI->Clone_GameObject(TEXT("Player04Hair"));
	if (nullptr == pPlayer04Hair)
		return E_FAIL;
	m_Parts.push_back((CParts*)pPlayer04Hair);

	CGameObject*		pPlayer04HairBattle = GI->Clone_GameObject(TEXT("Player04HairBattle"));
	if (nullptr == pPlayer04HairBattle)
		return E_FAIL;
	m_Parts.push_back((CParts*)pPlayer04HairBattle);

	CGameObject*		pPlayer04Sword1 = GI->Clone_GameObject(TEXT("Player04Sword1"));
	if (nullptr == pPlayer04Sword1)
		return E_FAIL;
	m_Parts.push_back((CParts*)pPlayer04Sword1);

	CGameObject*		pPlayer04Sword2 = GI->Clone_GameObject(TEXT("Player04Sword2"));
	if (nullptr == pPlayer04Sword2)
		return E_FAIL;
	m_Parts.push_back((CParts*)pPlayer04Sword2);

	CGameObject*		pPlayer04SwordBack = GI->Clone_GameObject(TEXT("Player04SwordBack"));
	if (nullptr == pPlayer04SwordBack)
		return E_FAIL;
	m_Parts.push_back((CParts*)pPlayer04SwordBack);



	return S_OK;
}

HRESULT CPlayer04::Update_Parts()
{

	XMStoreFloat4x4(&m_HeadMatrix, m_Sockets[SOCKET_HEAD]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix());

	if (m_ePlayerMode == PMODE_BURST)
		m_Parts[PARTS_HAIRBATTLE]->SetUp_State(XMLoadFloat4x4(&m_HeadMatrix));
	else
		m_Parts[PARTS_HAIR]->SetUp_State(XMLoadFloat4x4(&m_HeadMatrix));


	XMStoreFloat4x4(&m_WeaponBack, m_Sockets[SOCKET_WEAPONBACK]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix());
	m_Parts[PARTS_SWORDBACK]->SetUp_State(XMLoadFloat4x4(&m_WeaponBack));


	XMStoreFloat4x4(&m_Weapon_R, m_Sockets[SOCKET_WEAPON_R]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix());


	m_Parts[PARTS_SWORD]->SetUp_State(XMLoadFloat4x4(&m_Weapon_R));
	m_Parts[PARTS_SWORD2]->SetUp_State(XMLoadFloat4x4(&m_WeaponBack));

	return S_OK;
}

void CPlayer04::Render_Parts(_float fTimeDelta)
{
	switch (m_ePlayerMode)
	{
	case Client::CPlayer04::PMODE_BURST:
		m_Parts[PARTS_HAIRBATTLE]->LateTick(fTimeDelta);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_Parts[PARTS_HAIRBATTLE]);
		break;

	default:
		m_Parts[PARTS_HAIR]->LateTick(fTimeDelta);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_Parts[PARTS_HAIR]);
		break;
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_Parts[PARTS_SWORDBACK]);

	if (m_bSwordRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_Parts[PARTS_SWORD]);
	else
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_Parts[PARTS_SWORD2]);

}

CPlayer04 * CPlayer04::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer04*		pInstance = new CPlayer04(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer04"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer04::Clone(void * pArg)
{
	CPlayer04*		pInstance = new CPlayer04(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer04"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer04::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	
	for (auto& iter : m_SwordTrails)
		Safe_Release(iter);

	for (auto& iter : m_PointTrails)
		Safe_Release(iter);

	for (auto& iter : m_MotionTrails)
		Safe_Release(iter);

	for (auto& pPart : m_Parts)
		Safe_Release(pPart);
	

	m_Parts.clear();

	
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pShaderCom);
}
