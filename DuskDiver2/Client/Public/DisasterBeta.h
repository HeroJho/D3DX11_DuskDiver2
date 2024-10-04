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
	void Set_State(STATE eState); // ���¸� ���� ������ �ϴ� �ִϸ��̼��� ���⼭ �ִϸ��̼� ����
	void Hit_State(CCreture* pOther);
	void End_Animation(); // �ִϸ��̼��� ������ �� �۾��� ���� ������ ���� �ִϸ��̼��� ���⼭ �ִϸ��̼� ����
	void Update(_float fTimeDelta); // ���� ���¿� ���� �۾�����
	void Set_Dir(); // ������ ����, �÷��̾��� �������� ���� ���� ���Ÿ� �ٶ󺸰� ��, ������
	void LookDirPlayer();
	void LookAtPlayer(_float fLiearTime = 0.15f); //�÷��̾� �ٶ󺸰� turn�ϴ°ű��� ��ģ�Լ�, �Ű������� turn �ϴ� �ӵ� �����ϱ�����

	void Distance_BattlePattern(_float fTimeDelta); //�Ÿ��� ���� ��Ʋ ����
	void AttackPattern(_float fTimeDelta);
	void RandomWalk();
	void ActionToPlayer(_float fTimeDelta); // �÷��̾ ���� �ൿ
	void RegretToPlayer(_float fTimeDelta); // �÷��̾�� ���þ��� �ൿ
	void Check_Rim(_float fTimeDelta);
	void Check_Death(_float fTimeDelta);
	void Check_Avoid();
	void HitAwayPower_Cehck();
	void Reset_Collider();
	void Event_Stage();
	void Create_Effect(EFFECT eEffect);
	void Create_Item(DIE eDie);
	//�ٰŸ� ���� 3
	void CloseAttack(_float fTimeDelta);
	
	void Attack1(_float fTimeDelta);
	void Attack2(_float fTimeDelta);
	void Attack3(_float fTimeDelta);

	void Attack4(_float fTimeDelta); //���Ÿ�
	void AvoidAttack(); //�ڷ� ���ϰ� ����

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

	
private://������Ʈ, �������� �Լ���
	HRESULT Ready_Components();
	HRESULT Create_Navigation(_uint iNaviNum);

private: //�÷��̾� ����
	STATE m_eCurState = SPAWN; // ���� ����
	STATE m_eNextState = SPAWN; // �ٲ���� ����
	_float3 m_vTargetLook; //�ٶ� ����, �����ϹǷ� �ʿ���
	_float3 m_vPlayerLook; //�÷��̾ �ٶ󺸴� ����

private: //������Ʈ
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
	_float					m_fAttackTime = 0.f; //���� �� �ֱ� ���� ����
	_float					m_fPlusDamage = 0.f;

	_bool					m_bOneCheck = false;
	_bool					m_bAction = false;
	_bool					m_bMove = false;
	_bool					m_bAttack = false;
	_bool					m_bHitAble = false;
	_bool					m_bFly = false;
	_bool					m_bSpawnEffect = false;
	//�浹ó��
	_bool					m_bRHandCol = false;
	_bool					m_bLHandCol = false;
	_bool					m_bRangeCol = false;
	_bool					m_bBodyCol = false;
	_bool					m_bPush = false;
	_bool					m_bSameAttack = false; //hit�̶� ����, ���ݹ޴»��������ƴ��� üũ

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

	_float					m_fHAirPower = 0.f; // ������ �����Ŀ�
	_float					m_fHPushPower = 0.f; //������ �̴��Ŀ�

	_float					m_fHPushTime = 0.f;
	_bool					m_bOneRandom = false;
	_bool					m_bSide = false; //0�� ���� 1�� ������

	_bool					m_bReGauge = false;

	_bool					m_bMinusCount = false;
	_bool					m_bWafer1 = false;
	_float4					m_fHitEffectPos; //��Ʈ ����Ʈ ��ġ
	_bool					m_bDelayHit = false;
	_bool m_bHitEffect = false;
	_float m_fHitEffectDelAcc = 0.f; //����Ʈ ������ �ð�

	_bool					m_bAttack4_1Sound = false;
	_bool					m_bRoarSound1 = false;
	_bool					m_bRoarSound2 = false;
	_bool					m_bAvoidAttackSound = false;

	//�׽�Ʈ�� ���߿� ��������
	_bool					m_bAvoidAttack = false;


	//����
	_bool m_bCreate = false;
	_float m_fCreateAcc = 0.f;
	_float m_fDelayTime = 0.f;

	//����׿�
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