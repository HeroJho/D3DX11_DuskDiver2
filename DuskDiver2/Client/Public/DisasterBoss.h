#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CNavigation;
class CHierarchyNode;
END

BEGIN(Client)
class CParts;

class CDisasterBoss final : public CCreture
{
public:
	enum STATE {
		SPAWN, IDLE, RUN, WALKBACK, WALKFRONT, STRAFELEFT, STRAFERIGHT, 
		ATTACK1, ATTACK2, ATTACK3, ATTACK4_1, ATTACK4_2, ATTACK4_3, AVOIDATTACK,
		HITBACK, HITFLY, HITFRONT, HITDOWN, HITHORI, HITSPIN, HITUP, HITSLAPDOWN, BOUNCE,
		DOWN, GETUP, STUN, STUNRECOVER, TURNLEFT, TURNRIGHT, DEATHSTART
	};

	enum EFFECT {
		ATTACK1_RIGHT, ATTACK1_LEFT, ATTACK3_LEFT, ATTACK3_RIGHT, AVOIDATTACK_LEFT
	};
private:
	CDisasterBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDisasterBoss(const CDisasterBoss& rhs);
	virtual ~CDisasterBoss() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	virtual void On_Hit(CCreture* pOther) override;
	virtual void Create_HitEffect(_float4 vWorldPos)override;

public:
	void Control_DisasterBoss();

private:
	void Delay_Spawn(_float fTimeDelta);
	void Set_State(STATE eState); 
	void Hit_State(CCreture* pOther);
	void End_Animation(); 
	void Update(_float fTimeDelta); 
	void Set_Dir();
	void LookAtPlayer(_float fLiearTime = 0.1f);
	void Distance_BattlePattern(_float fTimeDelta);
	void AttackPattern(_float fTimeDelta);
	void ActionToPlayer(_float fTimeDelta); // 플레이어를 향한 행동
	void RegretToPlayer(_float fTimeDelta); // 플레이어랑 관련없는 행동
	void Check_Rim(_float fTimeDelta);
	void Check_Death(_float fTimeDelta);
	void Check_Avoid();
	void HitAwayPower_Cehck();
	void CloseAttack(_float fTimeDelta);
	void Reset_Collider();
	void RandomWalk();
	void Create_Effect(EFFECT eEffect);
	void Create_Particle(_float fTimeDelta);
	HRESULT Create_ParticleRock();

	void Attack1(_float fTimeDelta);
	void Attack2(_float fTimeDelta);
	void Attack3(_float fTimeDelta);
	void Attack4(_float fTimeDelta); //원거리
	void AvoidAttack(); //뒤로 피하고 공격

	void Spawn_Update(_float fTimeDelta);

	void Attack1_Update(_float fTimeDelta);
	void Attack2_Update(_float fTimeDelta);
	void Attack3_Update(_float fTimeDelta);
	void Attack4_1Update(_float fTimeDelta);
	void Attack4_2Update(_float fTimeDelta);
	void Attack4_3Update(_float fTimeDelta);
	void AvoidAttack_Update(_float fTimeDelta);

	void HitBack_Update(_float fTimeDelta);
	void HitFly_Update(_float fTimeDelta);
	void HitFront_Update(_float fTimeDelta);
	void HitDown_Update(_float fTimeDelta);
	void HitHori_Update(_float fTimeDelta);
	void HitSpin_Update(_float fTimeDelta);
	void HitUp_Update(_float fTimeDelta);
	void HitSlapdown_Update(_float fTimeDelta);

	void Run_Update(_float fTimeDelta);
	void Walk_Update(_float fTimeDelta);
	void WalkBack_Update(_float fTimeDelta);
	void Strafe_Update(_float fTimeDelta);

	void Create_Item();
	
private://컴포넌트, 파츠관련 함수들
	HRESULT Ready_Components();
	HRESULT Create_Navigation(_uint iNaviNum);

private: //플레이어 변수
	STATE m_eCurState = SPAWN; // 현재 상태
	STATE m_eNextState = SPAWN; // 바꿔야할 상태
	_float3 m_vTargetLook; //바라볼 방향, 보간하므로 필요함
	_float3 m_vPlayerLook; //플레이어가 바라보는 방향

private: //컴포넌트
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
	CTexture*				m_pDissolveTexture = nullptr;
	CTexture*				m_pEmissiveCom = nullptr;

private:
	_uint					m_iRandom = 0;
	_uint					m_iRandomAttack = 0;
	_uint					m_iIndex = 0;
	_float4					m_fPlayerPos;
	_float					m_fDistance = 0.f;
	_float					m_fSpeed = 0.f;
	_float					m_fAttackTime = 0.f; //공격 텀 주기 위한 변수

	_bool					m_bAttack = false;
	_bool					m_bOneCheck = false;
	_bool					m_bFly = false;
	_bool					m_bAction = false;
	_bool					m_bHitAble = false;
	_bool					m_bMove = false;

	//충돌처리
	_bool					m_bRHandCol = false;
	_bool					m_bLHandCol = false;
	_bool					m_bRangeCol = false;
	_bool					m_bBodyCol = false;
	_bool					m_bRollCol = false;
	_bool					m_bAddTime = true;

	_bool					m_bSameAttack = false; //hit이랑 동일, 공격받는상태인지아닌지 체크
	_bool					m_bPush = false;

	_bool					m_bAttack1Col = false;
	_bool					m_bAttack2Col = false;
	_bool					m_bAttack3Col_1 = false;
	_bool					m_bAttack3Col_2 = false;
	_bool					m_bAttack4_1Col = false;
	_bool					m_bAttack4_Effect = false;
	_bool					m_bSpawnEffect = false;

	
	_bool					m_bMinusCount = false;
	_bool					m_bOneRandom = false;
	_bool					m_bAvoidAttack = false;
	_bool					m_bBackWalk = false;

	_bool					m_bPlayerHit = true;

	_bool					m_bColDelay = false;
	_float					m_fColDelayTime = 0.f;
	_uint					m_iNaviNum = 0;

	_float					m_fHAirPower = 0.f; // 상대방의 공중파워
	_float					m_fHPushPower = 0.f; //상대방의 미는파워

	_float					m_fHPushTime = 0.f;

	_uint					m_iShaderPath = 0;

	_bool					m_bRim = false;
	_float					m_fRimAcc = 0.f;
	_bool					m_bAttackCol = false;

	_bool					m_bDeadStart = false;
	_bool					m_bDissolve = false;
	_float					m_fDissolveAcc = 0.f;
	_bool					m_bCreateItem = false;
	_float					m_fParticleAcc = 0.f;
	_bool					m_bParticle = true;
	_float					m_fParticleRockAcc = 0.f;
	_bool					m_bParticleRock = false;

	_bool					m_bReGauge = false;

	_bool					m_bAttack1On = false;
	_bool					m_bAttack2On = false;
	_bool					m_bAttack4On = false;
	_bool					m_bAvoidAttackOn = false;
	_bool					m_bOtherFoot = false;
	_float					m_fRunSoundAcc = 0.f;
	
	_bool					m_bRoarSound1 = false;
	_bool					m_bRoarSound2 = false;

	_bool					m_bWalkSound1 = false;
	_bool					m_bWalkSound2 = false;
	_bool					m_bBackWalkSound1 = false;
	_bool					m_bBackWalkSound2 = false;
	_bool					m_bStrafeSound1 = false;
	_bool					m_bStrafeSound2 = false;
	_bool					m_bRunSound1 = false;
	_bool					m_bRunSound2 = false;

	_bool					m_bAvoidSound = false;


	_bool m_bCreate = false;
	_float m_fCreateAcc = 0.f;
	_float m_fDelayTime = 0.f;

	//디버그용
	_float					m_fTest = 0.f;
	_float					m_fBlurPower = 1.f;
	_bool					m_bCheckFront = false;

private:
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	void	Set_PreBar(_float fTimeDelta);

public:
	static CDisasterBoss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END