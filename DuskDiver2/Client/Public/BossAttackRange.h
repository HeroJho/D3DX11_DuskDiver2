#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Client)

class CBossAttackRange : public CCreture
{
public:
	typedef struct tagEffectRangeInfo
	{
		_uint	iIndex;
		_float4 vPosition;
		_bool bSound = false;

	}EFFECTRANGEINFO;
private:
	CBossAttackRange(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBossAttackRange(const CBossAttackRange& rhs);
	virtual ~CBossAttackRange() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

private:
	void	Random_InitPos();

private:
	EFFECTRANGEINFO			m_EffectRangeInfo;
	_bool m_bSound;
	_float m_fRandomX = 0.f;
	_float m_fStaticY = 0.f;
	_float m_fRandomZ = 0.f;


private:
	HRESULT Ready_Components();
	_bool m_bAttackCol = false;

private:
	_bool		m_bCreatePillar = false;
	_float		m_fCreateAcc = 0.f;

public:
	static CBossAttackRange* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCreture* Clone(void* pArg);
	virtual void Free() override;
};

END