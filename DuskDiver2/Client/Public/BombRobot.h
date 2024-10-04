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

class CBombRobot final : public CCreture
{
public:
	enum STATE {
		SPAWN, IDLE, RUN, WALKBACK, WALKFRONT, STRAFELEFT, STRAFERIGHT,
		ATTACK1, ATTACK2, ATTACK3, ATTACK4,
		HITBACK, HITFLY, HITFRONT, HITDOWN, HITHORI, HITSPIN, HITUP, HITSLAPDOWN, BOUNCE,
		DOWN, GETUP, STUN, STUNRECOVER, TURNLEFT, TURNRIGHT
	};

	enum ATTACKSTATE { ATTACK_STRAIGHT, ATTACK_PARABOLA, ATTACK_BACKSTEP, ATTACK_UP, ATTACK_END };

private:
	CBombRobot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBombRobot(const CBombRobot& rhs);
	virtual ~CBombRobot() = default;

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
	void Control_BombRobot();

private:
	void Delay_Spawn(_float fTimeDelta);
	void Set_State(STATE eState);
	void Hit_State(CCreture* pOther);
	void End_Animation();
	void Update(_float fTimeDelta);
	void Set_Dir();
	void LookDirPlayer();
	void LookAtPlayer(_float fLiearTime = 0.15f);
	void Distance_BattlePattern(_float fTimeDelta);
	void AttackPattern(_float fTimeDelta);
	void ActionToPlayer(_float fTimeDelta);
	void RegretToPlayer(_float fTimeDelta);
	void Check_Rim(_float fTimeDelta);
	void Check_Death(_float fTimeDelta);
	void Check_Avoid();
	void Reset_Collider();
	void Event_Stage();
	void Create_Particle(_float fTimeDelta);

	void Attack1(_float fTimeDelta);
	void Attack2(_float fTimeDelta);
	void Attack3(_float fTimeDelta);
	void Attack4(_float fTimeDelta);
	void EscapePlayer(_float fTimeDelta);


	void Spawn_Update(_float fTimeDelta);

	void Attack1_Update(_float fTimeDelta);
	void Attack2_Update(_float fTimeDelta);
	void Attack3_Update(_float fTimeDelta);
	void Attack4_Update(_float fTimeDelta);


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
	void Create_DeadEffect();


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
	CHierarchyNode*			m_pMuzzleNode = nullptr;
private:
	ATTACKSTATE				m_eAttackState = ATTACK_END;
	ATTACKSTATE				m_eNextAttackState = ATTACK_END;
	_uint					m_iAttackNum = 0;
	_uint					m_iRandomAttack = 0;
	_uint					m_iShaderPath = 0;
	_uint					m_iIndex = 0;

	_float4x4				m_fMatrix;
	_float4					m_fPlayerPos;
	_float					m_fDistance = 0.f;
	_float					m_fSpeed = 0.f;
	_float					m_fAttackAcc = 0.f; //공격 텀 주기 위한 변수

	_bool					m_bOneCheck = false;
	_bool					m_bAction = false;
	_bool					m_bAttack = false;
	_bool					m_bRun = false;
	_bool					m_bHitAble = false;
	_bool					m_bFly = false;
	_bool					m_bEscape = false;
	_bool					m_bMove = false;
	_bool					m_bOneRandom = false;
	_bool					m_bWalkBack = false;


	//충돌처리
	_bool					m_bOnePatternEffect = false;
	_bool					m_bRangeCol = false;
	_bool					m_bBodyCol = false;
	_bool					m_bPush = false;
	_bool					m_bDefendMode = false;
	_bool					m_bSameAttack = false; //hit이랑 동일, 공격받는상태인지아닌지 체크
	
	_bool					m_bShootBullet1 = false;
	_bool					m_bShootBullet2 = false;
	_bool					m_bShootBullet3 = false;
	_bool					m_bShootBullet4 = false;

	_bool					m_bRim = false;
	_float					m_fRimAcc = 0.f;
	_bool					m_bSpawnEffect = false;

	_bool					m_bOtherAttack = false;
	_bool					m_bDeadStart = false;
	_bool					m_bDissolve = false;
	_float					m_fDissolveAcc = 0.f;

	_bool					m_bDontWalk = false;
	_float					m_fEscapeAcc = 0.f;
	_bool					m_bEscapeStart = false;

	_float					m_fHAirPower = 0.f; // 상대방의 공중파워
	_float					m_fHPushPower = 0.f; //상대방의 미는파워

	_float					m_fHPushTime = 0.f;
	_bool					m_bSpawnSound = false;
	_bool					m_bAvoidAttack = false;

	_bool					m_bReGauge = false;
	_bool					m_bMinusCount = false;
	_bool					m_bDeadEffect = false;
	_bool m_bCreate = false;
	_float m_fCreateAcc = 0.f;
	_float m_fDelayTime = 0.f;
	//디버그용
	_bool					m_bCheckFront = false;
	_float					m_fTest = 0.f;
	_float					m_fBlurPower = 0.f;
	_float					m_fRimPower = 0.f;

private:
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	void	Set_PreBar(_float fTimeDelta);

public:
	static CBombRobot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END