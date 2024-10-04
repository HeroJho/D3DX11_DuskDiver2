#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
END

BEGIN(Client)
class CSwordTrail;

class CSwordTrailObj : public CGameObject
{

public:
	CSwordTrailObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSwordTrailObj(const CSwordTrailObj& rhs);
	virtual ~CSwordTrailObj() = default;

public:
	virtual HRESULT Initialize_Prototype()override { return S_OK; }
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Tick(_float fTimeDelta, _matrix Matrix);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() { return S_OK; }
	virtual void Render_Debug() override {};
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override {};

public:
	_bool Get_TrailOn();
	void TrailOn(_matrix Matrix);
	void TrailOff();
	void Set_Color(_float4 vColor);
	void Set_Color2(_float4 vColor);
	void Set_Ratio(_float2 vRatio);
	void Set_BlurPower(_float fBlurPower);
	void Set_TimeLimit(_float fTimeLimit);
	void Set_HighLow(_float3 vHigh, _float3 vLow);

private:
	CRenderer* m_pRendererCom = nullptr;
	CSwordTrail* m_pTrail = nullptr;

public:
	static CSwordTrailObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END
