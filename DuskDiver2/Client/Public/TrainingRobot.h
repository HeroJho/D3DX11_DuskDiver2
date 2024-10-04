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
	void Set_State(STATE eState); // ���¸� ���� ������ �ϴ� �ִϸ��̼��� ���⼭ �ִϸ��̼� ����
	void Hit_State(CCreture* pOther);
	void End_Animation(); // �ִϸ��̼��� ������ �� �۾��� ���� ������ ���� �ִϸ��̼��� ���⼭ �ִϸ��̼� ����
	void Update(_float fTimeDelta); // ���� ���¿� ���� �۾�����
	void Set_Dir(); // ������ ����, �÷��̾��� �������� ���� ���� ���Ÿ� �ٶ󺸰� ��, ������
	void LookDirPlayer();
	void LookAtPlayer(_float fLiearTime = 0.1f); //�÷��̾� �ٶ󺸰� turn�ϴ°ű��� ��ģ�Լ�, �Ű������� turn �ϴ� �ӵ� �����ϱ�����
	void Distance_Pattern(_float fTimeDelta); // �Ÿ��� ���� ��Ʋ ����
	void AttackPattern(_float fTimeDelta);
	void RandomWalk();
	void ActionToPlayer(_float fTimeDelta); // �÷��̾ ���� �ൿ
	void RegretToPlayer(_float fTimeDelta); // �÷��̾�� ���þ��� �ൿ
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


private://������Ʈ, �������� �Լ���
	HRESULT Ready_Components();
	HRESULT Create_Navigation(_uint iNaviNum);

private:
	STATE m_eCurState = SPAWN1; // ���� ����
	STATE m_eNextState = SPAWN1; // �ٲ���� ����
	SHADER m_eShaderPass = SHADER_DEFAULT;
	_float3 m_vTargetLook; //�ٶ� ����, �����ϹǷ� �ʿ���

	_bool m_bMonsterLight = false;
	_float m_fMonsterLightAcc = 0.f;
	_float m_fMonsterLightLimit = 0.15f;
	_float3 m_vMonsterLightColor = { 1.f, 1.f, 1.f };

private: //������Ʈ
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
	CTexture*				m_pDissolveTexture = nullptr;
private:
	_float4					m_fPlayerPos;
	_float3					m_vPlayerLook;
	_float					m_fDistance = 0.f;
	_float					m_fAttackTime = 0.f; //���� �� �ֱ� ���� ����
	_float					m_fSpeed = 0.f;

	_uint					m_iRandomNum = 0;
	_uint					m_iRandomAttack = 0;

	_bool					m_bMove = false;
	_bool					m_bAttack = false;
	_bool					m_bOneCheck = false;
	_bool					m_bFly = false; // ���߿��ִ��� �ƴ��� -> �Լ��� �Ǵܰ����ϹǷ� �����ص���
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
	//�浹ó��

	//�ݶ��̴� Ű�� ����뵵
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

	_bool					m_bHandCol = false; //��
	_bool					m_bAttackCol = false; //���� ����
	_bool					m_bBodyCol = false;
	_bool					m_bSameAttack = false; //hit�̶� ����, ���ݹ޴»��������ƴ��� üũ
	_bool					m_bHitAble = false;
	_bool					m_bMinusCount = false;
	_float					m_fHAirPower = 0.f; // ������ �����Ŀ�
	_float					m_fHPushPower = 0.f; //������ �̴��Ŀ�
	
	_float					m_fHPushTime = 0.f;

	//����

	_bool					m_bGetUpSound = false;
	_bool					m_bSpawnSound = false;
	_bool					m_bSpawn2Sound = false;

	_bool					m_bStrafe1Sound = false;
	_bool					m_bStrafe2Sound = false;
	_bool					m_bAttack2Sound = false;
	_bool					m_bWalkback1Sound = false;
	_bool					m_bWalkback2Sound = false;

	//Ʃ�丮�󿡼� ������ ����
	_bool					m_bAction = false;
	_bool					m_bWakeUP = false;
	_bool					m_bSpawnHp = false;
	//����׿�
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