#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CTexture;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CBear1 : public CCreture
{
private:
	CBear1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBear1(const CBear1& rhs);
	virtual ~CBear1() = default;

	enum BEARSTATE
	{STATE_UP,STATE_DOWN,STATE_HIDE, STATE_HIT,STATE_END};

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	virtual void On_Hit(CCreture* pOther) override;

	virtual void Render_Debug() override;
	virtual void Create_HitEffect(_float4 vWorldPos)override;

public:
	_float Get_BearDamage() { return m_fInsteadDamage; }
	_uint Get_Index() { return m_CretureInfo.iIndex; }
	void Dead_On() { m_bDeadOn = true; }
	void Render_OnOff(_bool bOnOff) { m_bRender = bOnOff; }
	_bool Get_DeadOn() { return m_bDeadOn;}
	void Hit_OnOff(_bool bOnOff) { m_bHitAble = bOnOff; }
	void AllDead_Monster();
	_bool	Get_MonsterSpawn() { return m_bCreateMonster; }

private:
	void Check_Rim(_float fTimeDelta);
	void CreateMonster();
	void ReflectPlayer();
	void Check_ShakeBear(_float fTimeDelta);
	void ShakeBear();
	_float m_fShakePow = 0.f;
	_bool	m_bShake = false;
	_float	m_fShakeTimeAcc = 0.f;
	_float m_fShakeTime = 0.f;
	void Pattern_Up(_float fTimeDelta);
	void Pattern_Down(_float fTimeDelta);
	void Pattern_Hit(_float fTimeDelta);
	void Pattern_Hide(_float fTimeDelta);
	void LookAtPlayer();

private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CCreture*				m_pColPlayer = nullptr;

protected:
	HRESULT Ready_Components();

private:
	CCreture::CREATEINFO	m_CretureInfo;
	BEARSTATE m_BearState = STATE_HIDE;
	
	XMVECTOR m_OriginPos;
	XMVECTOR m_Pos;

	_bool m_bGoUp = false;
	list<CCreture*>			m_pMonster;
	_float					m_fHitTime = 0.f;
	_float					m_fHideTime = 0.f;

	_float					m_fPlayerHit = 0.f;
	_bool					m_bRandHide = false;
	_int					m_iRandHide = 0;
	_bool					m_bHitAble = false;

	_bool					m_bCreateEffect = false;
	_bool					m_bOneHit = false;
	_bool					m_bFalseBear = false;
	_int					m_iNum = 0;

	_uint					m_iShaderPass = 0;
	_float4					m_vPrePos;
	_float4					m_fRimColor = { 0.f,0.f,0.f,1.f };
	_float3					m_vTargetLook = { 0.f, 0.f, 0.f };
	_bool					m_bRim = false;
	_float					m_fRimAcc = 0.f;
	_uint					m_iShaderPath = 0;
	_float					m_fBlurPower = 0.f;

	_bool					m_bAttackCol = false;
	_bool					m_bMonsterSpawn = false;

	_bool					m_bRender = true;
	_float					m_fRimLimitTime = 0.f;
	_float					m_fInsteadDamage = 0.f;
	_bool					m_bDeadOn = false;
	_bool					m_bCreateMonster = false;

	_bool					m_bCheck = false;

	_bool					m_bUp = false;
	_bool					m_bDown = false;

public:
	static CBear1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCreture* Clone(void* pArg);
	virtual void Free() override;
};

END