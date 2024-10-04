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

class CBear4 : public CCreture
{
private:
	CBear4(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBear4(const CBear4& rhs);
	virtual ~CBear4() = default;

private:
	enum BEARSTATE
	{
		STATE_UP, STATE_DOWN, STATE_HIDE, STATE_HIT, STATE_DIE, STATE_END
	};

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	virtual void On_Hit(CCreture* pOther) override;

	virtual void Render_Debug() override;

public:
	void Pattern_Up(_float fTimeDelta);
	void Pattern_Down(_float fTimeDelta);
	void Pattern_Hit(_float fTimeDelta);
	void Pattern_Hide(_float fTimeDelta);
	void Pattern_Die(_float fTimeDelta);

private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	

protected:
	HRESULT Ready_Components();

private:
	XMVECTOR m_OriginPos;
	XMVECTOR m_Pos;

	_bool m_bDie = false;
	XMVECTOR m_DiePos;

	_bool m_bDeadRight = false;
	_float m_fDeadTime = 0.f;

	_bool m_bGoUp = false;

	_float m_fHitTime = 0.f;
	_float m_fHideTime = 0.f;

	_float m_fPlayerHit = 0.f;

	_bool m_bRandHide = false;
	_int m_iRandHide = 0;

	BEARSTATE m_BearState = STATE_HIDE;

	_int m_iNum = 0;

	_bool					m_bRim = false;
	_float					m_fRimAcc = 0.f;
	_float					m_fPosY = 0.f;
	_float					m_fRecoverAcc = 0.f;

	_bool					m_bAction = false;
	_bool					m_bHitAble = false;
	
	_bool					m_bOneHitPlayer = true;
	_bool					m_bOneHit = true;
	_bool					m_bAttackStart = false;
	_bool					m_bAttackCol = false;
public:
	static CBear4* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCreture* Clone(void* pArg);
	virtual void Free() override;
};

END