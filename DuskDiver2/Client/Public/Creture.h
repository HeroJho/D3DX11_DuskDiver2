#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CCreture : public CGameObject
{
public:
	typedef struct tagMonsterCreateInfo
	{
		int iIndex;
		int iNaviNum = 1;
		_float4 vPosition;
		_float3 vDir;
	}CREATEINFO;

public:
	enum HITTYPE {TYPE_HIT, TYPE_SPIN, TYPE_AWAY, TYPE_UP, TYPE_FASTDOWN, TYPE_END};
	enum PUSHTYPE {PUSH, PULL, TARGET, PUSHTYPE_END};
public:
	CCreture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCreture(const CCreture& rhs);
	virtual ~CCreture() = default;

public:
	virtual HRESULT Initialize_Prototype()override;
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Tick(_float fTimeDelta)override {};
	virtual void LateTick(_float fTimeDelta)override {};
	virtual HRESULT Render() override { return S_OK; }
	virtual HRESULT Render_Shadow() { return S_OK; }
	virtual void Render_Debug() override {};
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override {};
	virtual void On_Hit(CCreture* pOther) {};
	virtual _float4 Get_WorldPos();


	CTransform* Get_Transform() { return m_pTransformCom; }
	_float4x4 Get_WorldMatrix();
	void Set_WorldPos(_float4 vWorldPos);
	_float3 Get_Dir(_float3 vAxis);
	void Set_Dir(_float3 vAxis, _float3 vDir);
	HITTYPE Get_HitType() { return m_eHitType; }
	PUSHTYPE Get_PushType() { return m_ePushType; }
	_float Get_AirPower() { return m_fAirPower; }
	_float Get_PushPower() { return m_fPushPower; }
	_float Get_PatternGauge() { return m_fPatternGauge; }
	_float Get_PreHp() { return m_fPreHp; }
	_float Get_PreSp() { return m_fPreSp; }
	_float Get_PrePatternGauge() { return m_fPrePatternGauge; }
	_float3 Get_PushLook() { return m_vPushLook; }
	_float Get_MaxHp() { return m_fMaxHp; }
	void Set_MaxHp(_float fMaxHp) { m_fMaxHp = fMaxHp; }

	_float Get_NowHp() { return m_fNowHp; }
	void Set_NowHp(_float fNowHp) { m_fNowHp = fNowHp; }

	_float Get_Damage() { return m_fDamage; }
	void Set_Damage(_float fDamage) { m_fDamage = fDamage; }

	_float Get_Stamina() { return m_fStamina; }
	void Get_Stamina(_float fStamina) { m_fStamina = fStamina; }

	_bool Get_Hurt() { return m_bHurt; }

	_bool Get_Spawn() { return m_bSpawn; }

protected:
	HRESULT Ready_Components();
	void Check_Hurt(_float fTimeDelta);

protected:
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	HITTYPE					m_eHitType = TYPE_END; // 피격 타입
	PUSHTYPE				m_ePushType = PUSH;
	_float					m_fAirPower = 0.f; //공중에서 영향을 주는 피격힘
	_float					m_fPushPower = 0.f; //평지에서 영향을 주는 피격힘
	_float					m_fHurtTime = 0.f; //피격 콜라이더 쿨타임
	_float					m_fHurtAcc = 0.f; //피격 콜라이더 쿨타임 누적값
	_bool					m_bHurt = true; // 피격 콜라이더가 그룹에 포함되는지 안되는지
	_bool					m_bSpawn = false;
	_float					m_fDamage = 0.f;
	_float					m_fPlusDamage = 0.f;
	_float					m_fStamina;
	_float3					m_vPushLook = { 0.f,0.f,0.f };

	_float					m_fMaxHp;
	_float					m_fNowHp;
	_float					m_fPreHp = 0.f;
	_float					m_fMinusHp = 0.f;

	_float					m_fMaxPg = 0.f;
	_float					m_fPatternGauge = 0.f;
	_float					m_fPrePatternGauge = 0.f;
	_float					m_fMinusPg = 0.f;

	_float					m_fMaxSp = 0.f;
	_float					m_fPreSp = 0.f;

	_float					m_fMaxEp = 0.f;
	_float					m_fPreEp = 0.f;

	_bool					m_bFightEnd = false;

public:
	static CCreture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END