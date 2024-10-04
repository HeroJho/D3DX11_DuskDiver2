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

class CMechanicalOctopus final : public CCreture
{
public:
	enum STATE {
		IDLE1,IDLE2, MOVE, BRAKE, ATTACK1_1, ATTACK1_2, ATTACK1_3, ATTACK2,
		STUNBEGIN, STUN, DEATH
	};
	enum SOKET {
		LEG_A, LEG_B, LEG_C, LEG_D
	};

	enum EFFECT {
		RANGE
	};
private:
	CMechanicalOctopus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMechanicalOctopus(const CMechanicalOctopus& rhs);
	virtual ~CMechanicalOctopus() = default;

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
	void Control_MechanicalOctopus();
private:
	void Set_State(STATE eState);
	void End_Animation(); 
	void Update(_float fTimeDelta); 
	void Set_Dir(); 
	void LookAtPlayer(_float fLiearTime = 0.1f); 
	void Distance_BattlePattern(_float fTimeDelta); 
	void Change_AttackColRange(_float3 vSize, _float3 vCenter);
	void AttackPattern(_float fTimeDelta);
	void BasicAttack(_float fTimeDelta);
	void Moving(_float fTimeDelta);
	void ActionToPlayer(_float fTimeDelta); 
	void RegretToPlayer(_float fTimeDelta); 
	void Check_Rim(_float fTimeDelta);
	void Check_Death(_float fTimeDelta);
	void Check_PatternGauge(_float fTimeDelta);
	void Reset_Collider();

	void Check_ShootAttack(_float fTimeDelta);
	void Find_NearstMonster(_float fTimeDelta);
	void Shoot_AllLeg();
	void Create_Particle(_float4 fParticle);

	void Attack1(_float fTimeDelta);
	void Attack2(_float fTimeDelta);

	void Attack1_1Update(_float fTimeDelta);
	void Attack1_2Update(_float fTimeDelta);
	void Attack1_3Update(_float fTimeDelta);
	void Attack2_Update(_float fTimeDelta);

	void Break_Update(_float fTimeDelta);
	void Move_Update(_float fTimeDelta);
	void Idle_Update(_float fTimeDelta);

	void Stun_Update(_float fTimeDelta);
	void Death_Update(_float fTimeDelta);


	void Create_Effect(EFFECT eEffect);

	
private://컴포넌트, 파츠관련 함수들
	HRESULT Ready_Sockets();
	HRESULT Ready_LegParts(_uint iNaviNum);
	HRESULT Update_Parts();
	HRESULT Ready_Components();
	HRESULT Create_Navigation(_uint iNaviNum);
private: 
	STATE m_eCurState = IDLE1; // 현재 상태
	STATE m_eNextState = IDLE1; // 바꿔야할 상태
	_float3 m_vTargetLook; //바라볼 방향, 보간하므로 필요함
	_float3 m_vPlayerLook; //플레이어가 바라보는 방향
	_float m_fDissolveAcc = 0.f;
	_bool m_bDissolve = false;

private:// STL 컨테이너
	vector<CCreture*>				m_Parts;
	vector<class CHierarchyNode*>		m_Sockets;


	_float4x4 m_LegAMatrix;
	_float4x4 m_LegBMatrix;
	_float4x4 m_LegCMatrix;
	_float4x4 m_LegDMatrix;

private: //컴포넌트
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
	CTexture*				m_pDissolveTexture = nullptr;
	CTexture*				m_pEmissiveCom = nullptr;
	CGameObject*			m_pRangeEffect = nullptr;
private:
	_uint					m_iRandomAttack = 0;
	_uint					m_iRandomAttack2 = 0;
	_uint					m_iRandom = 0;
	_uint					m_iIndex = 0;
	_float4					m_fPlayerPos;

	_float					m_fDistance = 0.f;
	_float					m_fAttackTime = 0.f; //공격 텀 주기 위한 변수
	_float					m_fSpeed = 0.f;
	_float					m_fShootAcc = 0.6f;

	_bool					m_bShoot = false;
	_bool					m_bMoveShoot = true;
	_bool					m_bBreak = false;
	_bool					m_bOneTime = false;
	_bool					m_bAddShoot = true;
	_float					m_fBreakAcc = 0.f;


	_bool					m_bOneRandom = true;
	_bool					m_bAttack = false;
	_bool					m_bOneCheck = false;
	_bool					m_bWalkBack = false;
	_bool					m_bFly = false;
	_bool					m_bAction = false;
	_bool					m_bHitAble = false;
	_bool					m_bMove = true;
	_bool					m_bAddTime = true;
	_bool					m_bPlayerHit = true;
	_bool					m_bPatternStart = false;
	_bool					m_bPour = false;

	_bool					m_bAvoidAttack = false;
	_bool					m_bAvoidCheck = false;
	_bool					m_bShootMissile = true;
	_float					m_fLegTotalHp = 0.f;
	_float					m_fLegHp = 0.f;

	_bool					m_bPattern1 = false;
	_bool					m_bPattern2 = false;
	_bool					m_bPattern3 = false;

	_bool					m_bAttack2Shake = false;

	_bool					m_bJumpSound = false;
	_bool					m_bDownSound = false;

	_bool					m_bSpinSound = false;
	_float					m_fSpinAcc = 0.f;
	//충돌처리
	_bool					m_bLHandCol = false;
	_bool					m_bRangeCol = false;
	_bool					m_bAttackCol = false;

	_bool					m_bHurtCol = false;

	_bool					m_bAvoidAttackCol = false;
	_bool					m_bAttack1_3Sound = false;
	_float					m_fMovingGuage = 0.f;
	_bool					m_bMoving = false;
	_bool					m_bClose = false;

	_bool					m_bColDelay = false;
	_float					m_fColDelayTime = 0.f;

	//_float4					m_fParticle = { 0.f, 0.f, 0.f, 0.f }; //1: 인덱스 2: 사이즈 3: 앞뒤이동 4: 좌우이동

	_float					m_fHAirPower = 0.f; // 상대방의 공중파워
	_float					m_fHPushPower = 0.f; //상대방의 미는파워
	_float					m_fHPushTime = 0.f;

	_uint					m_iShaderPath = 0;

	_float					m_fRange = 0;
	_float					m_fDelayTime = 0.f;

	_bool					m_bRim = false;
	_float					m_fRimAcc = 0.f;
	_float					m_fPosY = 0.f;
	_float					m_fRecoverAcc = 0.f;
	_bool					m_bStun = false;
	_bool					m_bHitMove = true;
	_bool					m_bDamageAble = true;
	_bool					m_bDeadStart = false;
	_bool					m_bStunOne = true;
	_bool					m_bOneDead = true;
	
	_bool					m_bReGauge = false;
	_float					m_fMinusHp = 0.f;
	_float					m_fMaxPg = 0.f;
	_float					m_fMinusPg = 0.f;

	_float					m_fStunSoundAcc = 0.f;
	_float					m_fGasSoundAcc = 0.f;
	_bool					m_bOneSound = false;
	_bool					m_bTwoSound = false;
	_bool					m_bStunOn = false;
	_bool					m_bStunGas = false;
	_bool					m_bIdleOn = false;
	_bool					m_bIdleStop = false;
	_bool					m_bBreakOn = false;
	_bool					m_bAttack1On = false;
	_bool					m_bMoveOn = false;
	_bool					m_bRangeOn = false;
	//이펙트
	_bool					m_bOnePos = false;
	_bool					m_bRangeEffect1 = false;
	_bool					m_bAttack1Effect = false;
	_float					m_bAttack1EffectAcc = 0.f;

	_bool					m_bAttack2Effect = false;
	_bool					m_bAttack2Effect_2 = false;
	//디버그용
	_bool m_bGara = true;

	_float4 ShakeTest = { 0.f,0.f,0.f,0.f };
	_float4 FowTest = { 0.f,0.f,0.f,0.f };
	_float					m_fTest = 0.f;
	_float					m_fBlurPower = 1.f;
	_bool					m_bCheckFront = false;


	_float					m_fCutSceneAcc = 0.f;

	// For.  CutScene
public: 
	void Set_OctopusCutSceneState(_float fPosX, _float fPoZ, _float fSpeed);
	_float4x4 Get_BoneMatrix();
	void Set_CutScene(_bool bCutScene) { m_bCutScene = bCutScene; }
	
	void Die();

private:
	void Tick_Game(_float fTimeDelta);
	void Tick_Scene(_float fTimeDelta);
	void CutScene_Update(_float fTimeDelta);

private:
	_bool				m_bCutScene = true;
	CHierarchyNode*			m_pHead;
	_bool m_bRender = false;
	_float m_fAnimSpeed = 0.f;



private:
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	void	Set_PreBar(_float fTimeDelta);

public:
	static CMechanicalOctopus* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END