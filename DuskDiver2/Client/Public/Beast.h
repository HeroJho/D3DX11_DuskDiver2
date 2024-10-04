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
class CBeastStoneBody;
class CBeastBreath;
class CBeastMarvelPush;
class CBeastMarvelBall;
class CBeastMarvelObstacle;
class CMarvelReserveTaril;
class CBeastBreath2;

class CBeast final : public CCreture
{
public:
	enum STATE { //attack1 - �ٰŸ� ���� ������, attack2 - �ٰŸ� �ٴ� ��� attack3 - �� ��ȯ, attack4_1 �극��, attack5 - �Ͽ︵(���� ������ ����)
		IDLE, ATTACK1, ATTACK2, ATTACK3, ATTACK4_1, ATTACK4_2, ATTACK4_3, ATTACK5,
		STUNBEGIN, STUN, STUNRECOVER, DEATH
	};

	enum BREATHTYPE {BREATH_NORMAL, BREATH_RED, BREATH_WHITE, BREATH_END};

	enum BEAST_ATTACK {
		CLOSECLAW, 
	};
	enum EFFECT {
		ATTACK1_RIGHT, HOWLING_ATTACK3, HOWLING_ATTACK5 , ET_ATTACK2, 
	};

	enum POSITION {
		BEAST_HEAD, BEAST_RIGHT, BEAST_LEFT, POSITION_END
	};

private:
	CBeast(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBeast(const CBeast& rhs);
	virtual ~CBeast() = default;

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
	void Control_Beast();
	void Set_BeastCry();
	void Spawn_Beast() { m_bSpawn = true; }
	void Plus_BeastHP(_float fHp) { m_fNowHp += fHp; m_fPreHp += fHp; }


private:

	void Key_InputAttack();
	void Set_State(STATE eState);
	void End_Animation(); 
	void Update(_float fTimeDelta); 
	void Push_Collider();
	void Set_Dir(); 
	void LookAtPlayer(_float fLiearTime = 0.1f); 
	void Distance_BattlePattern(_float fTimeDelta); 
	void Change_AttackColRange(_float3 vSize, _float3 vCenter);
	void AttackPattern(_float fTimeDelta);
	void BasicAttack(_float fTimeDelta);
	void CloseAttack(_float fTimeDelta);
	void ActionToPlayer(_float fTimeDelta); 
	void RegretToPlayer(_float fTimeDelta); 
	void Check_Rim(_float fTimeDelta);
	void Check_Death(_float fTimeDelta);
	void Check_DeadState();
	void Check_BreathState();
	void Check_Stun();
	void Check_Recover(_float fTimeDelta);
	void Set_IDleState();
	void Reset_Collider();
	void Reset_AttackState();
	void Reset_HowlingAttack();
	void End_Receover();
	void Create_Effect(EFFECT eEffect);
	void Create_Particle(_float4 fParticle);

	void Attack1(_float fTimeDelta);
	void Attack2(_float fTimeDelta);
	void Attack3(_float fTimeDelta);
	void Attack4(_float fTimeDelta);
	void Attack5(_float fTimeDelta);

	void Idle_Update(_float fTimeDelta);

	void Attack1_Update(_float fTimeDelta);
	void Attack2_Update(_float fTimeDelta);
	void Attack3_Update(_float fTimeDelta);
	void Attack4_1Update(_float fTimeDelta);
	void Attack4_2Update(_float fTimeDelta);
	void Attack4_2MarvelUpdate(_float fTimeDelta);
	void Attack4_3Update(_float fTimeDelta);	
	void Attack5_Update(_float fTimeDelta);
	void StunBegin_Update(_float fTimeDelta);
	void StunRecover_Update(_float fTimeDelta);

	void HowlingAttack1(_float fTimeDelta); //�Ͽ︵���� �÷��̾� �о��
	void HowlingAttack2(_float fTimeDelta); //���� �ϳ� �߻� - ������ �پ�Ѿ����
	void HowlingAttack3(_float fTimeDelta); //�ұ�� ��ȯ
	void HowlingAttack4(_float fTimeDelta); //ȸ���� ��ȯ
	void HowlingAttack5(_float fTimeDelta); //�տ���
	void HowlingAttack6(_float fTimeDelta); //����ġ��

	void Idle_HowlingAttack3(_float fTimeDelta);
	void Idle_HowlingAttack4(_float fTimeDelta);
	void Idle_HowlingAttack5(_float fTimeDelta);
	void Marvel_HowlingAttack6(_float fTimeDelta);

	void Create_ReserveTrail(_uint iTrailIndex); //�տ��� - ���ݹ��� �ߴ°�
	void Play_Game(_uint iTurnIndex); //�տ��� ���� ����

	//����ġ��
	void Off_MarvelObj();
	void Render_MarvelTrail();
	void Create_MarvelObj();
	void Create_Obstacle();
	
	void Stun_Update(_float fTimeDelta);

	

private://������Ʈ, �������� �Լ���
	HRESULT Ready_Components();
	HRESULT Create_StoneBody();
	HRESULT Create_Breath();
	HRESULT Create_Navigation(_uint iNaviNum);

	//����ġ��
	HRESULT Create_MarvelPush(); //�м��ִ°�
	HRESULT Create_MarvelObstacle(); //���ع�
	HRESULT Create_MarvelBall(); //����
	HRESULT Create_MarvelReserveTrail(); 

	void	SetUp_Breath();
	void	Find_NearstStoneShieldBody(_float fTimeDelta);
	void	Find_NearstStoneAttackBody(_float fTimeDelta);


	//HS
	void Fix_BreathEffectInfo(BREATHTYPE eType);
	void Update_Breath2Matrix(_float fTimeDelta);
	void Create_BreathChargeEffect(BREATHTYPE eType, _float fTimeDelta);
	void Create_BreathEffect(BREATHTYPE eType, _float fTimeDelta);


	CBeastBreath*			m_pBeastBreath = nullptr;
	_float m_fWaveAcc = 0.f;
	_bool m_bCry1Col = false;
	_float m_fAttack3CreateAcc = 0.f;
	_float m_fBreathParticleAcc = 0.f;
	_bool m_bBreathNormalCircle = false;
	_float m_fBreathNormalAcc = 0.f;
	vector<CGameObject*> m_BeastBreath2;
	BREATHTYPE m_eBreathType = BREATH_NORMAL;

private: 
	POSITION	m_eHitPostion = POSITION_END;
	STATE		m_eCurState = IDLE; 
	STATE		m_eNextState = IDLE; 
	_float3		m_vTargetLook; 
	_float3		m_vPlayerLook; 
	_float		m_fDissolveAcc = 0.f;
	_bool		m_bDissolve = false;

	_bool		m_bAttack1Voice = false;
	_bool		m_bAttack2Voice = false;
	_bool		m_bAttack2Sound = false;
	_bool		m_bAttack3Voice = false;
	_bool		m_bAttack5Voice = false;
	_float		m_fBreathSoundAcc = 0.f;
	_bool		m_bPillarSound = false;
	_float		m_fPillarSoundAcc = 0.f;
	_float		m_fStunSoundAcc = 0.f;

private: 
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
	CTexture*				m_pDissolveTexture = nullptr;
	CTexture*				m_pEmissiveCom = nullptr;
	CHierarchyNode*			m_pClawNode;
	CHierarchyNode*			m_pBreathNode;
	vector<CBeastStoneBody*> m_pStoneBodys;
	


private:
	_float4x4				m_BreathMatrix;
	_float4x4				m_BreathFixMatrix;
	_float4					m_fPlayerPos;
	_float4					m_fPlayerStartPos;

	_uint					m_iCloseAttack = 0;
	_uint					m_iBasicAttack = 0;
	_uint					m_iAttackCount = 0;
	_uint					m_iStunCont = 0;
	_float					m_fAttackTime = 0.f; 
	_float					m_fAttackDelayT = 0.f;
	_float					m_fStunRecoverAcc = 0.f;


	_bool					m_bPase2On = false;
	_bool					m_bPase2Hp = false;
	_bool					m_bBeastDie = false;
	_bool					m_bStunRecoverSound = false;
	_bool					m_bStunBeginDownSound = false;
	_bool					m_bOneRandom = true;
	_bool					m_bAttack = false;
	_bool					m_bOneCheck = false;
	_bool					m_bAction = false;
	_bool					m_bHitAble = false;
	_bool					m_bMove = false;
	_bool					m_bAddTime = true;
	_bool					m_bPlayerHit = true;
	_bool					m_bDelayAttack = false;
	_bool					m_bStun = false;
	_bool					m_bStunCol = false;
	_bool					m_bBeastEndEnd = false;

	_bool					m_bStoneSound = false;
	_bool					m_bOneCheckPos = false;
	_bool					m_bAttack2Effect = false;
	_bool					m_bMonkeyParttern = false;


	//�浹ó��
	_bool					m_bLHandCol = false;
	_bool					m_bRangeCol = false;
	_bool					m_bRHandCol = false;
	_bool					m_bAttackCol = false;
	_bool					m_bHurtCol = false;
	_float					m_fAttackRange = 0.f; //�浹ü ũ�� ������

	//�극�� ����
	_uint					m_iBreathCount = 0;
	_uint					m_iBreatLimtNum = 0;
	_uint					m_iMaxCount = 0; //�극�� �մ� �ִϸ��̼� �󸶳� �ݺ��Ұ���
	_uint					m_iChargeMaxCount = 0; //�극�� �մ� �ִϸ��̼� �󸶳� �ݺ��Ұ���

	_bool					m_bFix = false;
	_bool					m_bFireBreath = false;
	_bool					m_bOneMatrix = true;
	_bool					m_bBreath = false;
	_bool					m_bChasePlayer = false;
	_bool					m_bCreateStone = false;
	_float					m_fFixAcc = 0.f;
	_float					m_fBreathSize = 0.f;
	_float					m_fDistance = 0.f;
	_float					m_fFixY = 0.f;
	_float					m_fBreathDistance = 0.f;

	_uint					m_iChargeCount = 0;
	//�Ͽ︵

	//1. ���� ������ - ������ ���ؾ��ϴ� ����
	_uint					m_iHowlingAttack = 0;
	_bool					m_bCreateClaw = true;

	//2. �ұ�� - �װ��� ���� ��ġ�� ���� �߰� ��� ������ ����
	_bool					m_bCreateRange = true;
	_bool					m_bHowlingAttack = false; //�ұ�� ���� ������ �Ǵ�
	_uint					m_iHowlAttack3Count = 0;
	_float					m_fHowlDelayAcc = 0.f;
	_bool					m_bDleay = true;

	//3. ȸ����
	_bool					m_bCreateTornado = false;

	//4. �տ���

	_float					m_fCreateReserveAcc = 0.f;
	_float					m_fReserveLimitTime = 0.f;
	_float					m_fPrePos = 0.f;
	_float3					m_fReserveTarget = { 0.f, 0.f, 0.f };
	_float3					m_fReservePos = { 0.f, 0.f, 0.f };
	_uint					m_iTurnIndex = 0;
	_bool					m_bProgress = false;
	_bool					m_bReserveGameStart = false;
	
	//����ġ��

	//������Ʈ - ���ع�, ũ����Ż, ���� ���±�, ����
	vector<CBeastMarvelObstacle*> m_pMarvelObstacle;
	vector<CBeastMarvelPush*> m_pMarvelPush;
	vector<CMarvelReserveTaril*> m_pMarvelTrail;
	CBeastMarvelBall*		m_pMarvelBall;
	void Set_PushNewPos();

	_uint	m_iTrailIndex = 0;
	_float	m_fMarvelGameAcc = 0.f;
	_bool	m_bCreateObstacle = true;
	_bool	m_bCreatePush = true;
	_bool	m_bCreateMarvelBall = true;
	_bool	m_bExplosion = false;
	_bool	m_bOneSet = true;
	_bool	m_bMarvelGame = false;
	_bool	m_bChaseTrail = false;

	//���ع� ��ġ
	_float4 vRouteObstaclePos[2][11] = 
	{
		//1��° ���
		{ { -5.916f, 0.02f, -61.914f, 1.f }, { -9.364f, 0.02f, -52.201f, 1.f }, { -0.655f, 0.02f, -50.579f, 1.f },
		{ 5.2f, 0.02f, -57.f, 1.f }, { 6.473f, 0.02f, -45.256f, 1.f }, { -5.822f, 0.02f, -41.119f, 1.f },
		{ -0.886f, 0.02f, -59.460f, 1.f },{ 4.321f, 0.02f, -38.460f, 1.f },{ 5.2f, 0.02f, -51.199f, 1.f },
		{ -2.f, 0.02f, -45.f, 1.f },{ -3.f, 0.02f, -36.f, 1.f }
		},
		
		
	};



	//����Ʈ
	_bool					m_bEffect_Attack1 = false;

	//�� & ���̴�
	_uint					m_iShaderPath = 0;
	_bool					m_bRim = false;
	_float					m_fRimAcc = 0.f;


	_float					m_fRecoverAcc = 0.f;
	_bool					m_bDamageAble = true;
	_bool					m_bDeadStart = false;
	_bool					m_bDeadOne = true;


	_bool					m_bReGauge = false;
	_float					m_fMinusHp = 0.f;
	_float					m_fMaxPg = 0.f;
	_float					m_fMinusPg = 0.f;


	_float					m_fHAirPower = 0.f; // ������ �����Ŀ�
	_float					m_fHPushPower = 0.f; //������ �̴��Ŀ�
	_float					m_fHPushTime = 0.f;
	

	//����׿�
	_bool m_bGara = true;

	_float4 ShakeTest = { 0.f,0.f,0.f,0.f };
	_float4 FowTest = { 0.f,0.f,0.f,0.f };
	_float					m_fTest = 0.f;
	_float					m_fBlurPower = 1.f;
	_bool					m_bCheckFront = false;
	_bool m_bLineTrailOn = false;
	_bool	m_bSuccessGame = false;
	_bool m_bMarbleAttack = false;
	_float m_fPillarPattern = 0.f;
	_bool m_bAttack5EffectOn = false;
	void Create_Attack5OnEffect();

private:
	HRESULT Ready_Layer_UI(const _tchar* pLayerTag);
	void	Set_PreBar(_float fTimeDelta);

public:
	static CBeast* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END