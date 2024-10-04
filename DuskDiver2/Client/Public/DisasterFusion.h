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
class CSwordTrailObj;

class CDisasterFusion final : public CCreture
{
public:
	enum STATE {
		SPAWN, IDLE, RUN, WALKBACK, WALKFRONT, STRAFELEFT, STRAFERIGHT, 
		ATTACK1,  ATTACK3_1, ATTACK3_2, ATTACK3_3, ATTACK2,
		HITBACK, HITFLY, HITFRONT, HITDOWN, HITHORI, HITSPIN, HITUP, HITSLAPDOWN, BOUNCE,
		DOWN, GETUP, STUNBEGIN, STUN, STUNRECOVER
	};
	enum EFFECT { EFFECT_THUNDER, EFFECT_TRAIL, EFFECT_PATTERNGAUGE, EFFCT_END };
	enum Socket { SOCKET_RHAND, SOCKET_LHAND, SOCKET_RTHUMB, SOCKET_LTHUMB, SOCKET_RTUMBPOINT, SOCKET_RTUMBPOINT2, SOCKET_SPINE, SOKET_PG
	};
	enum SWORDTRAIL { STR_RHAND, STR_LHAND, STR_END };

private:
	CDisasterFusion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDisasterFusion(const CDisasterFusion& rhs);
	virtual ~CDisasterFusion() = default;

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
	void Control_DisasterFusion();
	_bool Check_Move() { return m_bSpawn; }

private:
	void Set_State(STATE eState); // 상태를 설정 보간을 하는 애니메이션은 여기서 애니메이션 셋팅
	void Hit_State(CCreture* pOther);
	void End_Animation(); // 애니메이션이 끝났을 때 작업을 수행 보간을 안할 애니메이션은 여기서 애니메이션 셋팅
	void Update(_float fTimeDelta); // 현재 상태에 따라 작업수행
	void Set_Dir(); // 방향을 설정, 플레이어의 포스에서 내꺼 포스 뺀거를 바라보게 함, 제어사용
	void LookDirPlayer();
	void LookAtPlayer(_float fLiearTime = 0.1f); //플레이어 바라보고 turn하는거까지 합친함수, 매개변수는 turn 하는 속도 보간하기위함
	void Distance_BattlePattern(_float fTimeDelta); //거리에 따른 배틀 패턴
	void AttackPattern(_float fTimeDelta);
	void ActionToPlayer(_float fTimeDelta); // 플레이어를 향한 행동
	void RegretToPlayer(_float fTimeDelta); // 플레이어랑 관련없는 행동
	void Check_Rim(_float fTimeDelta);
	void Check_Death(_float fTimeDelta);
	void Check_Avoid();
	void HitAwayPower_Cehck();
	void Reset_Collider();
	void Event_Stage();


	void CloseAttack(_float fTimeDelta);

	void Attack1(_float fTimeDelta);
	void Attack2(_float fTimeDelta);
	void Attack3(_float fTimeDelta);

	void SpawnRoar_Update(_float fTimeDelta);

	void Attack1_Update(_float fTimeDelta);
	void Attack2_Update(_float fTimeDelta);
	void Attack3_1Update(_float fTimeDelta);
	void Attack3_2Update(_float fTimeDelta);
	void Attack3_3Update(_float fTimeDelta);

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
	
	HRESULT Create_Effect(EFFECT eET);
	void Create_Particle(_float fTimeDelta);

	void Create_SwordTrail(_float fTimeDelta);
	void Control_RHandTrail(_float fTimeDelta);
	void Control_LHandTrail(_float fTimeDelta);

	void On_LHandTrail();
	void On_RHandTrail();

	void Create_Item();
	void Delay_Spawn(_float fTimeDelta);

private://컴포넌트, 파츠관련 함수들
	HRESULT Ready_Components();
	HRESULT Create_Navigation(_uint iNaviNum);
	HRESULT Ready_Sockets();
	HRESULT Ready_Trails();

private: //플레이어 변수
	STATE m_eCurState = SPAWN; // 현재 상태
	STATE m_eNextState = SPAWN; // 바꿔야할 상태
	_float3 m_vTargetLook; //바라볼 방향, 보간하므로 필요함


private: //컴포넌트
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
	CTexture*				m_pDissolveTexture = nullptr;
	CTexture*				m_pEmissiveCom = nullptr;
	vector<CSwordTrailObj*>	m_SwordTrails;
	vector<CHierarchyNode*>	m_pSockets;
	CGameObject*				m_pTrailEffect = nullptr;

private:
	_float4					m_fPlayerPos;
	_float3					m_vPlayerLook;

	_uint					m_iRandomAttack = 0;
	_uint					m_iIndex = 0;
	_float					m_fDistance = 0.f;
	_float					m_fAttackTime = 0.f; //공격 텀 주기 위한 변수
	_float					m_fSpeed = 0.f;
	_float					m_fPlusDamage = 0.f;

	_bool					m_bOtherFoot = false;
	_float					m_fRunSoundAcc = 0.f;

	_bool					m_bOneCheck = false;
	_bool					m_bAttack = false;
	_bool					m_bWalkBack = false;
	_bool					m_bHitAble = false;
	_bool					m_bAction = false;
	_bool					m_bFly = false;
	_bool					m_bMove = false;
	_bool					m_bAttack1On = false; // attack1 한번만
	_bool					m_bAttack2On = false; // attack2 한번만
	_bool					m_bAttack3On = false; // attack3 한번만
	//충돌처리용
	_bool					m_bRHandCol = false;
	_bool					m_bLHandCol = false;
	_bool					m_bRangeCol = false;
	_bool					m_bBodyCol = false;
	_bool					m_bPush = false;
	_bool					m_bShake = false;
	_bool					m_bSpawnEffect = false;

	_bool					m_bSameAttack = false; //hit이랑 동일, 공격받는상태인지아닌지 체크

	_bool					m_bAttackCol = false;
	_bool					m_bOneRandom = false;

	_bool					m_bRim = false;
	_float					m_fRimAcc = 0.f;

	_uint					m_iShaderPath = 0;

	_bool					m_bDeadStart = false;
	_bool					m_bDissolve = false;
	_float					m_fDissolveAcc = 0.f;

	_float					m_fHAirPower = 0.f; // 상대방의 공중파워
	_float					m_fHPushPower = 0.f; //상대방의 미는파워

	_float					m_fHPushTime = 0.f;

	_bool					m_bAvoidAttack = false;
	_bool					m_bReGauge = false;
	_bool					m_bMinusCount = false;
	//트레일 & 이펙트
	_bool					m_bAttack2Trail = false;
	_bool					m_bAttack3_3Trail = false;
	_float					m_fEffectSpeed = 0.f;
	_float					m_fEffectScale = 0.f;

	_uint					m_iEffectIndex = 0; //0일때 오른손 중앙, 1일때 왼손 중앙, 2일때 오른손 끝

	_bool					m_bAttack1Effect = false;
	_bool					m_bAttack3Effect_1 = false;
	_bool					m_bAttack3Effect_2 = false;

	_bool					m_bAttack2Effect_1 = false;
	_bool					m_bAttack2Effect_2 = false;
	_bool					m_bAttack2Effect_3 = false;

	_float					m_fParticleAcc = 0.f;
	_bool					m_bParticle = true;

	_bool					m_bSpawnSound = false;
	//디버그용
	_float					m_fTest = 0.f;
	_float					m_fBlurPower = 0.f;
	_float					m_fRimPower = 1.f;
	_float					m_fGama = 0.f;
	_bool m_bCreate = false;
	_float m_fCreateAcc = 0.f;
	_float m_fDelayTime = 0.f;
	_bool					m_bCheckFront = false;

private:
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	void	Set_PreBar(_float fTimeDelta);

public:
	static CDisasterFusion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END