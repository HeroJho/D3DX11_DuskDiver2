#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CEffect : public CGameObject
{

public:
	CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect(const CEffect& rhs);
	virtual ~CEffect() = default;

public:
	virtual HRESULT Initialize_Prototype()override { return S_OK; }
	virtual HRESULT Initialize(void* pArg)override { return S_OK; }
	virtual void Tick(_float fTimeDelta)override {};
	virtual void LateTick(_float fTimeDelta)override {};
	virtual HRESULT Render() override { return S_OK; }
	virtual HRESULT Render_Shadow() { return S_OK; }
	virtual void Render_Debug() override {};
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override {};

public:
	_bool Set_UVIndex(_float fTimeDelta);
	_bool Set_UVReverse(_float fTimeDelta);
	void Set_UVFix(_float fTimeDelta);
protected:
	_float m_fUVIndexAcc = 0.f;
	_float m_fUVIndexX = 0.f;
	_float m_fUVIndexY = 0.f;
	_float m_fMaxUVIndexX = 0.f;
	_float m_fMaxUVIndexY = 0.f;
	_float m_fLifeTimeAcc = 0.f;
	_float m_fUVSpeed = 0.f;
	_float2 m_vUVFixSpeed = { 0.f,0.f };
	_float2 m_vUVFixAcc = { 0.f,0.f };

public:
	static CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END
