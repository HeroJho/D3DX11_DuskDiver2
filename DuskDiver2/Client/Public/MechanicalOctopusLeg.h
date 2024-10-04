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

class CMechanicalOctopusLeg final : public CCreture
{
public:
	enum STATE {
		ATTACK1_1, ATTACK1_2, ATTACK1_3, ATTACK2_1, ATTACK2_2, ATTACK2_3, ATTACK3
	};
	enum ATTACKSTATE {
		ATTACKPATTERN1, ATTACKPATTERN2, ATTACKPATTERN3
	};
	typedef struct LegInfo
	{
		_uint iHitType; //hit 타입

	}LEGINFO;
private:
	CMechanicalOctopusLeg(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMechanicalOctopusLeg(const CMechanicalOctopusLeg& rhs);
	virtual ~CMechanicalOctopusLeg() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	virtual void On_Hit(CCreture* pOther) override;
	virtual  _float4 Get_WorldPos() override;
public:
	HRESULT SetUp_State(_fmatrix StateMatrix);
	void Control_MechanicalOctopus();
	void Attack_On() { m_bMove = true; m_bOneRandom = true; };
	void Pour_Missile() { m_bPour = true; m_bOneCheck = false; }
	void Pour_MissilePlus() { m_bPlus = true; };
	void Spawn_On() { m_bSpawn = true; }
	void Render_On() { m_bRender = true; }

	_float4x4 Get_WorldMatrix();
private:
	void Set_State(STATE eState);
	void End_Animation(); 
	void Update(_float fTimeDelta); 
	void Set_Dir(); 
	void LookAtPlayer(_float fLiearTime = 0.1f); 
	void Distance_BattlePattern(_float fTimeDelta); 
	void BasicAttack(_float fTimeDelta);
	void Check_Rim(_float fTimeDelta);
	void Check_Death(_float fTimeDelta);
	void Check_Stun();
	void Check_Recover(_float fTimeDelta);
	void Reset_Collider();
	void Create_Particle(_float4 fParticle);
	void Missile_Sound();


	void Launch_Missile1(); // 카이사 배지어
	void Launch_Missile2(); //배지어 새로운거
	void Launch_Missile3(); //유도탄

	void Attack1(_float fTimeDelta);
	void Attack2(_float fTimeDelta);
	void Attack3(_float fTimeDelta);

	void Attack1_1Update(_float fTimeDelta);
	void Attack1_2Update(_float fTimeDelta);
	void Attack1_3Update(_float fTimeDelta);
	void Attack2_1Update(_float fTimeDelta);
	void Attack2_2Update(_float fTimeDelta);
	void Attack2_3Update(_float fTimeDelta);
	void Attack3_Update(_float fTimeDelta);

	_float TargetDistance(_fvector vTargetPos);
private://컴포넌트, 파츠관련 함수들
	HRESULT Ready_Components();
	HRESULT Create_Navigation(_uint iNaviNum);
	void	Ready_HierarchyNode();
private: 
	STATE m_eCurState = ATTACK1_1; // 현재 상태
	STATE m_eNextState = ATTACK1_1; // 바꿔야할 상태
	_float3 m_vTargetLook; //바라볼 방향, 보간하므로 필요함
	_float3 m_vPlayerLook; //플레이어가 바라보는 방향
	_float m_fDissolveAcc = 0.f;
	_bool m_bDissolve = false;
private: //컴포넌트
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTransform*				m_pParentTransformCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;
	CTexture*				m_pDissolveTexture = nullptr;
	CTexture*				m_pEmissiveCom = nullptr;

	//폭탄 발사하는 뼈
	CHierarchyNode*			m_pCoreNode;

	//미사일 발사하는 뼈
	CHierarchyNode*			m_pMissileNode1;
	CHierarchyNode*			m_pMissileNode2;
	CHierarchyNode*			m_pMissileNode3;
	CHierarchyNode*			m_pMissileNode4;
	CHierarchyNode*			m_pMissileNode5;
	CHierarchyNode*			m_pMissileNode6;
	CHierarchyNode*			m_pMissileNode7;
	CHierarchyNode*			m_pMissileNode8;
	CHierarchyNode*			m_pMissileNode9;
	CHierarchyNode*			m_pMissileNode10;
	CHierarchyNode*			m_pMissileNode11;
	CHierarchyNode*			m_pMissileNode12;
	CHierarchyNode*			m_pBombNode;


private:
	LEGINFO					m_LegInfo;
	_uint					m_iRandomAttack = 0;
	_uint					m_iRandomAttack2 = 0;
	_uint					m_iRandom = 0;
	_uint					m_iIndex = 0;
	_float4					m_fPlayerPos;

	_float					m_fDistance = 0.f;
	_float					m_fSpeed = 0.f;

	_bool					m_bRender = false;
	_bool					m_bShootBullet2 = false;
	_bool					m_bShootBullet3 = false;

	_bool					m_bShootMissile1_1 = false;
	_bool					m_bShootMissile1_2 = false;
	_bool					m_bShootMissile1_3 = false;
	_bool					m_bShootMissile1_4 = false;

	_bool					m_bShootMissile2_1 = false;
	_bool					m_bShootMissile2_2 = false;

	_bool					m_bAttack = false;
	_bool					m_bOneCheck = false;
	_bool					m_bAction = false;
	_bool					m_bHitAble = true;
	_bool					m_bMove = false;
	_bool					m_bPour = false;
	_bool					m_bPlus = false;
	//충돌처리
	_bool					m_bLHandCol = false;
	_bool					m_bRangeCol = false;
	_bool					m_bAttackCol = false;

	_bool					m_bHurtCol = false;

	//_float4					m_fParticle = { 0.f, 0.f, 0.f, 0.f }; //1: 인덱스 2: 사이즈 3: 앞뒤이동 4: 좌우이동
	_float4					m_fWorldPos = {0.f, 0.f, 0.f, 1.f};
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
	_bool					m_bHitMove = true;
	_bool					m_bDamageAble = true;
	_bool					m_bDeadStart = false;
	_bool					m_bDeadOne = true;
	_bool					m_bReGauge = false;
	_float					m_fMinusHp = 0.f;
	_float					m_fMaxPg = 0.f;
	_float					m_fMinusPg = 0.f;

	_bool					m_bOneRandom = true;
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
	static CMechanicalOctopusLeg* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END