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

class CTrainingRobot final : public CCreture
{
public:
	enum STATE {
		ATTACK1, ATTACK2, ATTACK3, ATTACK4_1, ATTACK4_2, ATTACK4_3,
		BOUNCE, HITBACK, HITFLY, HITFRONT, HITDOWN, HITSPIN, HITUP, HITSLAPDOWN, DOWN,GETUP,
		SPAWN1, SPAWN2,IDLE, STUN, STUNRECOVER, STRAFELEFT, STRAFERIGHT, TURNLEFT, TURNRIGHT,RUN, WALKBACK, WALK				
	};

	enum SHADER { SHADER_DEFAULT, SHADER_RIM, SHADER_LIGHT, SHADER_DISSOLVE, SHADER_END };

private:
	CTrainingRobot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrainingRobot(const CTrainingRobot& rhs);
	virtual ~CTrainingRobot() = default;

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
	void Set_WakeUP() { m_bWakeUP = true; }
	_uint Get_Index() { return m_iIndex; }
	
private:
	HRESULT Set_RenderPass(SHADER eShader);
	void Set_State(STATE eState); // 상태를 설정 보간을 하는 애니메이션은 여기서 애니메이션 셋팅
	void Hit_State(CCreture* pOther);
	void End_Animation(); // 애니메이션이 끝났을 때 작업을 수행 보간을 안할 애니메이션은 여기서 애니메이션 셋팅
	void Update(_float fTimeDelta); // 현재 상태에 따라 작업수행
	void Set_Dir(); // 방향을 설정, 플레이어의 포스에서 내꺼 포스 뺀거를 바라보게 함, 제어사용
	void LookDirPlayer();
	void LookAtPlayer(_float fLiearTime = 0.1f); //플레이어 바라보고 turn하는거까지 합친함수, 매개변수는 turn 하는 속도 보간하기위함
	void Distance_Pattern(_float fTimeDelta); // 거리에 따른 배틀 패턴
	void AttackPattern(_float fTimeDelta);
	void RandomWalk();
	void ActionToPlayer(_float fTimeDelta); // 플레이어를 향한 행동
	void RegretToPlayer(_float fTimeDelta); // 플레이어랑 관련없는 행동
	void Check_Rim(_float fTimeDelta);
	void Check_Death(_float fTimeDelta);
	void WakeUpCheck(_float fTimeDelta);
	void Reset_Collider();
	void HitAwayPower_Cehck();
	void Create_DeadEffect();

	void Attack1(_float fTimeDelta);
	void Attack2(_float fTimeDelta);
	void Attack3(_float fTimeDelta);
	void Attack4(_float fTimeDelta);

	void Attack1_Update();
	void Attack2_Update();
	void Attack3_Update();
	void Attack4_1Update();
	void Attack4_2Update();
	void Attack4_3Update();

	void HitBack_Update(_float fTimeDelta);
	void HitFly_Update(_float fTimeDelta);
	void HitFront_Update(_float fTimeDelta);
	void HitDown_Update(_float fTimeDelta);
	void HitSpin_Update(_float fTimeDelta);
	void HitUp_Update(_float fTimeDelta);
	void HitSlapdown_Update(_float fTimeDelta);

	void Run_Update(_float fTimeDelta);
	void Walk_Update(_float fTimeDelta);
	void WalkBack_Update(_float fTimeDelta);
	void Strafe_Update(_float fTimeDelta);
	void GetUp_Update(_float fTimeDelta);
	void Spawn1_Update(_float fTimeDelta);
	void Spawn2_Update(_float fTimeDelta);

	void Check_MonsterLight(_float fTimeDelta);


private://컴포넌트, 파츠관련 함수들
	HRESULT Ready_Components();
	HRESULT Create_Navigation(_uint iNaviNum);

private:
	STATE m_eCurState = SPAWN1; // 현재 상태
	STATE m_eNextState = SPAWN1; // 바꿔야할 상태
	SHADER m_eShaderPass = SHADER_DEFAULT;
	_float3 m_vTargetLook; //바라볼 방향, 보간하므로 필요함

	_bool m_bMonsterLight = false;
	_float m_fMonsterLightAcc = 0.f;
	_float m_fMonsterLightLimit = 0.15f;
	_float3 m_vMonsterLightColor = { 1.f, 1.f, 1.f };

private: //컴포넌트
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
	CTexture*				m_pDissolveTexture = nullptr;
private:
	_float4					m_fPlayerPos;
	_float3					m_vPlayerLook;
	_float					m_fDistance = 0.f;
	_float					m_fAttackTime = 0.f; //공격 텀 주기 위한 변수
	_float					m_fSpeed = 0.f;

	_uint					m_iRandomNum = 0;
	_uint					m_iRandomAttack = 0;

	_bool					m_bMove = false;
	_bool					m_bAttack = false;
	_bool					m_bOneCheck = false;
	_bool					m_bFly = false; // 공중에있는지 아닌지 -> 함수로 판단가능하므로 삭제해도됨
	_bool					m_bWalkBack = false;
	_bool					m_bPush = false;
	_bool					m_bAway = false;
	_bool					m_bOneRandom = false;

	_bool					m_bOtherFoot = false;
	_float					m_fRunSoundAcc = 0.f;

	_uint					m_iIndex = 0;
	_bool					m_bDeadStart = false;
	_bool					m_bDissolve = false;
	_float					m_fDissolveAcc = 0.f;

	_bool					m_bRim = false;
	_float					m_fRimAcc = 0.f;
	_float					m_fRim = 0.f;

	_uint					m_iShaderPath = 0;
	//충돌처리

	//콜라이더 키고 끄기용도
	_bool					m_bAttack1Col = false;
	_bool					m_bAttack2Col = false;

	_bool					m_bAttack3Col_0 = false;
	_bool					m_bAttack3Col_1 = false;
	_bool					m_bAttack3Col_2 = false;
	_bool					m_bAttack3Col_3 = false;
	_bool					m_bAttack3Col_4 = false;
	_bool					m_bAttack3Col_5 = false;
	_bool					m_bAttack3Col_6 = false;

	_bool					m_bAttack4_1Col = false;

	_bool					m_bAttack4_2Col_0 = false;
	_bool					m_bAttack4_2Col_1 = false;
	_bool					m_bAttack4_2Col_2 = false;
	_bool					m_bAttack4_2Col_3 = false;

	_bool					m_bAttack4_3Col_0 = false;
	_bool					m_bAttack4_3Col_1 = false;
	_bool					m_bAttack4_3Col_2 = false;

	_bool					m_bHandCol = false; //손
	_bool					m_bAttackCol = false; //범위 공격
	_bool					m_bBodyCol = false;
	_bool					m_bSameAttack = false; //hit이랑 동일, 공격받는상태인지아닌지 체크
	_bool					m_bHitAble = false;
	_bool					m_bMinusCount = false;
	_float					m_fHAirPower = 0.f; // 상대방의 공중파워
	_float					m_fHPushPower = 0.f; //상대방의 미는파워
	
	_float					m_fHPushTime = 0.f;

	//사운드

	_bool					m_bGetUpSound = false;
	_bool					m_bSpawnSound = false;
	_bool					m_bSpawn2Sound = false;

	_bool					m_bStrafe1Sound = false;
	_bool					m_bStrafe2Sound = false;
	_bool					m_bAttack2Sound = false;
	_bool					m_bWalkback1Sound = false;
	_bool					m_bWalkback2Sound = false;

	//튜토리얼에서 움직임 제어
	_bool					m_bAction = false;
	_bool					m_bWakeUP = false;
	_bool					m_bSpawnHp = false;
	//디버그용
	_bool m_bGara = true;
	_bool					m_bCheckFront = false;
	_float					m_fTest = 0.f;
	_float					m_fAttackTest = 0.f;
	_float					m_fBlurPower = 0.f;
	_float					m_fAngleY = 0.f;
	_float4 ShakeTest = { 0.f,0.f,0.f,0.f };
	_float4 FowTest = { 0.f,0.f,0.f,0.f };

private:
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	void	Set_PreBar(_float fTimeDelta);

public:
	static CTrainingRobot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END