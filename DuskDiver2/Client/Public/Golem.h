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
class CPointTrailObj;

class CGolem final : public CCreture
{
public:
	enum STATE {
		SPAWN1, SPAWN2,IDLE, RUN, WALKBACK, WALKFRONT, STRAFELEFT, STRAFERIGHT, 
		ATTACK1, ATTACK2, ATTACK3, ATTACK4, ATTACK5, ATTACK6_1, ATTACK6_2, ATTACK7,
		HITBACK, DOWN, GETUP, RECOVER, STUN, STUNRECOVER, STUNDEATH, TURNLEFT, TURNRIGHT
	};
	enum Socket {
		 SOKET_WEAPON, SOCKET_HEAD, SOCKET_END
	};
	enum EFFECT { ATTACK5_EFFECT, ATTACK5_ROCKEFFECT, ATTAK6_1EFFECT, ATTAK6_1EFFECT2, EFFECT_END };
	enum POINTTRAIL { PTR_WEAPON, PTR_REYE, PRT_LEYE, PTR_END };
	enum SHADER { SHADER_DEFAULT, SHADER_RIM, SHADER_LIGHT, SHADER_DISSOLVE, SHADER_END };

private:
	CGolem(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGolem(const CGolem& rhs);
	virtual ~CGolem() = default;

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
	void Control_Golem();
private:
	HRESULT Set_RenderPass(SHADER eShader);
	void KeyInput_Attack();
	void Set_State(STATE eState);
	void Hit_State(CCreture* pOther);
	void End_Animation(); 
	void Update(_float fTimeDelta); 
	void Set_Dir(); 
	void LookAtPlayer(_float fLiearTime = 0.1f); 
	void Distance_BattlePattern(_float fTimeDelta); 
	void Change_WeaponColRange(_float3 vSize, _float3 vCenter);
	void Change_AttackColRange(_float3 vSize, _float3 vCenter);
	void AttackPattern(_float fTimeDelta);
	void BasicAttack(_float fTimeDelta);
	void CloseAttack(_float fTimeDelta);
	void ActionToPlayer(_float fTimeDelta); 
	void RegretToPlayer(_float fTimeDelta); 
	void Check_Rim(_float fTimeDelta);
	void Check_Death(_float fTimeDelta);
	void Check_Stun();
	void Check_Recover(_float fTimeDelta);
	void Reset_Collider();

	void Create_Particle(_float4 fParticle);
	void Create_Effect(EFFECT eEffect);


	void Attack1(_float fTimeDelta);
	void Attack2(_float fTimeDelta);
	void Attack3(_float fTimeDelta);
	void Attack4(_float fTimeDelta);
	void Attack5(_float fTimeDelta);
	void Attack6_1(_float fTimeDelta);
	void Attack6_2(_float fTimeDelta);
	void Attack7(_float fTimeDelta);

	void Attack1_Update(_float fTimeDelta);
	void Attack2_Update(_float fTimeDelta);
	void Attack3_Update(_float fTimeDelta);
	void Attack4_Update(_float fTimeDelta);
	void Attack5_Update(_float fTimeDelta);
	void Attack6_1Update(_float fTimeDelta);
	void Attack6_2Update(_float fTimeDelta);
	void Attack7_Update(_float fTimeDelta);

	void Spawn1_Update(_float fTimeDelta);
	void Spawn2_Update(_float fTimeDelta);
	void Idle_Update(_float fTimeDelta);
	void HitBack_Update(_float fTimeDelta);
	void StunRecover_Update(_float fTimeDelta);

	void Run_Update(_float fTimeDelta);
	void Walk_Update(_float fTimeDelta);
	void WalkBack_Update(_float fTimeDelta);
	void Strafe_Update(_float fTimeDelta);
	
	//트레일
	HRESULT Ready_Trails();
	HRESULT Ready_Socket();
	void Create_PointTrail(_float fTimeDelta);
	void Control_WeaponTrail(_float fTimeDelta);
	void Control_RightEyeTrail(_float fTimeDelta);
	void Control_LeftEyeTrail(_float fTimeDelta);
	void Check_MonsterLight(_float fTimeDelta);

	void Create_Item();

private://컴포넌트, 파츠관련 함수들
	HRESULT Ready_Components();
	HRESULT Create_Navigation(_uint iNaviNum);
private: 
	SHADER m_eShaderPass = SHADER_DEFAULT;
	STATE m_eCurState = SPAWN1; // 현재 상태
	STATE m_eNextState = SPAWN1; // 바꿔야할 상태
	_float3 m_vTargetLook; //바라볼 방향, 보간하므로 필요함
	_float3 m_vPlayerLook; //플레이어가 바라보는 방향
	_float m_fDissolveAcc = 0.f;
	_bool m_bDissolve = false;

	_float m_fCutEffectAcc = 0.f;
	_float m_fCutEffect2Acc = 0.f;
	_bool m_bMonsterLight = false;
	_float m_fMonsterLightAcc = 0.f;
	_float m_fMonsterLightLimit = 0.15f;
	_float3 m_vMonsterLightColor = { 1.f, 1.f, 1.f };


private: //컴포넌트
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
	CTexture*				m_pDissolveTexture = nullptr;
	CTexture*				m_pEmissiveCom[2] = { nullptr };
	vector<class CHierarchyNode*>	m_Sockets;
	vector<CPointTrailObj*>	m_PointTrails;

private:
	_uint					m_iRandomAttack = 0;
	_uint					m_iCloseAttackNum = 0;
	_uint					m_iRandom = 0;
	_uint					m_iIndex = 0;
	_float4					m_fPlayerPos;
	_float					m_fAttack1EffectAcc = 0.f;

	_float					m_fDistance = 0.f;
	_float					m_fAttackTime = 0.f; //공격 텀 주기 위한 변수
	_float					m_fSpeed = 0.f;
	_float					m_fRimPower = 0.f;
	_bool					m_bOneRandom = true;
	_bool					m_bAttack = false;
	_bool					m_bOneCheck = false;
	_bool					m_bWalkBack = false;
	_bool					m_bFly = false;
	_bool					m_bAction = false;
	_bool					m_bHitAble = false;
	_bool					m_bMove = false;
	_bool					m_bAttack4Effect = false;
	_bool					m_bAttack7Effect = false;
	_bool					m_bAttack7Effect_2 = false;
	_bool					m_bAttack7Effect_3 = false;
	_bool					m_bAttack6Sound = false;
	_bool					m_bAttack6Sound_2 = false;

	_bool					m_bPlayerHit = true;

	//충돌처리
	_bool					m_bWeaponCol = false;
	_bool					m_bRangeCol = false;
	_bool					m_bAttackCol = false;
	_bool					m_bSameAttack = false; //hit이랑 동일, 공격받는상태인지아닌지 체크

	_bool					m_bAttack2Col_0 = false;
	_bool					m_bAttack3Col_0 = false;
	_bool					m_bAttack4Col_0 = false;
	_bool					m_bAttack4Col_1 = false;
	_bool					m_bAttack4Col_2 = false;
	_bool					m_bAttack4Col_3 = false;
	_bool					m_bAttack6_1Col_0 = false;

	_bool					m_bAttack6_2Col_0 = false;
	_bool					m_bAvoidAttackCol = false;

	_bool					m_bColDelay = false;
	_float					m_fColDelayTime = 0.f;

	_float4					m_fParticle = { 0.f, 0.f, 0.f, 0.f }; //1: 인덱스 2: 사이즈 3: 앞뒤이동 4: 좌우이동

	_float					m_fHAirPower = 0.f; // 상대방의 공중파워
	_float					m_fHPushPower = 0.f; //상대방의 미는파워
	_float					m_fHPushTime = 0.f;

	_uint					m_iShaderPath = 0;

	_float					m_fDelayTime = 0.f;

	_bool					m_bRim = false;
	_float					m_fRimAcc = 0.f;
	_float					m_fPosY = 0.f;
	_float					m_fRecoverAcc = 0.f;
	_bool					m_bStun = false;
	_bool					m_bClose = false;
	_bool					m_bHitMove = true;
	_bool					m_bDamageAble = true;
	_bool					m_bDeadStart = false;
	_bool					m_bDeadOne = true;

	_bool					m_bReGauge = false;

	_bool					m_bAttack5Effect = false;
	_bool					m_bAttack6_1Effect = false;

	_bool					m_bSpawn1Walk = false;
	_bool					m_bSpawn2Shout = false;
	_bool					m_bAttack1On = false;
	_bool					m_bAttack1Fire = false;
	_bool					m_bAttack2Fire = false;
	_bool					m_bAttack3Off = false;
	_bool					m_bAttack4On = false;
	_bool					m_bAttack5On = false;
	_bool					m_bAttack6On = false;
	_bool					m_bAttack7On = false;
	_bool					m_bRecoverOn = false;
	_bool					m_bOtherFoot = false;
	_float					m_fRunSoundAcc = 0.f;

	_float					m_fRemoveAcc = 0.f;
	_bool					m_bRemove = false;
	_bool					m_bOneMove = true;
	//트레일 변수
	_bool					m_bEyeTrail = false;
	_bool					m_bWeaponTrail = false;


	//디버그용
	_bool m_bGara = true;

	_float4 ShakeTest = { 0.f,0.f,0.f,0.f };
	_float4 FowTest = { 0.f,0.f,0.f,0.f };
	_float					m_fTest = 0.f;
	_float					m_fBlurPower = 1.f;
	_bool					m_bCheckFront = false;

private:
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	void	Set_PreBar(_float fTimeDelta);

public:
	static CGolem* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END