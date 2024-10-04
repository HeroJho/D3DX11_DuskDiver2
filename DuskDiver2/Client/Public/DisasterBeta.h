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

class CDisasterBeta final : public CCreture
{
public:
	enum STATE {
		SPAWN, IDLE, RUN, WALKBACK, WALKFRONT, STRAFELEFT, STRAFERIGHT, 
		ATTACK1, ATTACK2, ATTACK3, ATTACK4_1, ATTACK4_2, ATTACK4_3, AVOIDATTACK,
		HITBACK, HITFLY, HITFRONT, HITDOWN, HITHORI, HITSPIN, HITUP, HITSLAPDOWN, BOUNCE,
		DOWN, GETUP, STUNBEGIN, STUN, STUNRECOVER
	};
	enum EFFECT {
		ATTACK1_RIGHT,ATTACK1_LEFT, ATTACK3_LEFT, ATTACK3_RIGHT, ATTACK4_2LEFT, ATTACK4_2RIGHT, ATTACK4_3_RIGHT,
		AVOIDATTACK_LEFT
	};
	enum DIE {
		DIE_BOUNCE, DIE_STUN
	};
private:
	CDisasterBeta(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDisasterBeta(const CDisasterBeta& rhs);
	virtual ~CDisasterBeta() = default;

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
	void Control_DisasterBeta();
	_bool Check_Move() { return m_bSpawn; }

private:
	void Delay_Spawn(_float fTimeDelta);
	void Set_State(STATE eState); // 상태를 설정 보간을 하는 애니메이션은 여기서 애니메이션 셋팅
	void Hit_State(CCreture* pOther);
	void End_Animation(); // 애니메이션이 끝났을 때 작업을 수행 보간을 안할 애니메이션은 여기서 애니메이션 셋팅
	void Update(_float fTimeDelta); // 현재 상태에 따라 작업수행
	void Set_Dir(); // 방향을 설정, 플레이어의 포스에서 내꺼 포스 뺀거를 바라보게 함, 제어사용
	void LookDirPlayer();
	void LookAtPlayer(_float fLiearTime = 0.15f); //플레이어 바라보고 turn하는거까지 합친함수, 매개변수는 turn 하는 속도 보간하기위함

	void Distance_BattlePattern(_float fTimeDelta); //거리에 따른 배틀 패턴
	void AttackPattern(_float fTimeDelta);
	void RandomWalk();
	void ActionToPlayer(_float fTimeDelta); // 플레이어를 향한 행동
	void RegretToPlayer(_float fTimeDelta); // 플레이어랑 관련없는 행동
	void Check_Rim(_float fTimeDelta);
	void Check_Death(_float fTimeDelta);
	void Check_Avoid();
	void HitAwayPower_Cehck();
	void Reset_Collider();
	void Event_Stage();
	void Create_Effect(EFFECT eEffect);
	void Create_Item(DIE eDie);
	//근거리 공격 3
	void CloseAttack(_float fTimeDelta);
	
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
	void Stun_Update(_float fTimeDelta);

	
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
	_uint					m_iShaderPath = 0;
	_uint					m_iIndex = 0;
	_float4					m_fPlayerPos;
	_float					m_fDistance = 0.f;
	_float					m_fSpeed = 0.f;
	_float					m_fAttackTime = 0.f; //공격 텀 주기 위한 변수
	_float					m_fPlusDamage = 0.f;

	_bool					m_bOneCheck = false;
	_bool					m_bAction = false;
	_bool					m_bMove = false;
	_bool					m_bAttack = false;
	_bool					m_bHitAble = false;
	_bool					m_bFly = false;
	_bool					m_bSpawnEffect = false;
	//충돌처리
	_bool					m_bRHandCol = false;
	_bool					m_bLHandCol = false;
	_bool					m_bRangeCol = false;
	_bool					m_bBodyCol = false;
	_bool					m_bPush = false;
	_bool					m_bSameAttack = false; //hit이랑 동일, 공격받는상태인지아닌지 체크

	_bool					m_bEffectTime = false;
	_bool					m_bEffectTime3 = false;
	_bool					m_bEffectTime4_1 = false;
	_bool					m_bEffectTime4_2 = false;
	_bool					m_bEffectTime4_3 = false;
	_bool					m_bEffectTime5 = false;

	_bool					m_bAttackCol = false;
	_bool					m_bAttack2Col = false;
	_bool					m_bAttack3Col_1 = false;
	_bool					m_bAttack3Col_2 = false;
	
	_bool					m_bRim = true;
	_float					m_fRimAcc = 0.f;

	_bool					m_bBackWalk = false;
	_bool					m_bDeadStart = false;
	_bool					m_bDissolve = false;
	_float					m_fDissolveAcc = 0.f;

	_float					m_fHAirPower = 0.f; // 상대방의 공중파워
	_float					m_fHPushPower = 0.f; //상대방의 미는파워

	_float					m_fHPushTime = 0.f;
	_bool					m_bOneRandom = false;
	_bool					m_bSide = false; //0이 왼쪽 1이 오른쪽

	_bool					m_bReGauge = false;

	_bool					m_bMinusCount = false;
	_bool					m_bWafer1 = false;
	_float4					m_fHitEffectPos; //히트 이펙트 위치
	_bool					m_bDelayHit = false;
	_bool m_bHitEffect = false;
	_float m_fHitEffectDelAcc = 0.f; //이펙트 딜레이 시간

	_bool					m_bAttack4_1Sound = false;
	_bool					m_bRoarSound1 = false;
	_bool					m_bRoarSound2 = false;
	_bool					m_bAvoidAttackSound = false;

	//테스트용 나중에 게이지로
	_bool					m_bAvoidAttack = false;


	//스폰
	_bool m_bCreate = false;
	_float m_fCreateAcc = 0.f;
	_float m_fDelayTime = 0.f;

	//디버그용
	_float					m_fGama = 0.f;
	_bool					m_bCheckFront = false;
	_float					m_fTest = 0.f;
	_float					m_fBlurPower = 1.f;
	_float4 ShakeTest = { 0.f,0.f,0.f,0.f };
	_float4 FowTest = { 0.f,0.f,0.f,0.f };

private:
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	void	Set_PreBar(_float fTimeDelta);

public:
	static CDisasterBeta* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END