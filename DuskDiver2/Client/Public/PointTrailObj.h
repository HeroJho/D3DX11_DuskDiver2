#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
END

BEGIN(Client)
class CPointTrail;

class CPointTrailObj : public CGameObject
{

public:
	CPointTrailObj(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPointTrailObj(const CPointTrailObj& rhs);
	virtual ~CPointTrailObj() = default;

public:
	virtual HRESULT Initialize_Prototype()override { return S_OK; }
	virtual HRESULT Initialize(void* pArg)override;
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render() override;
	virtual HRESULT Render_Shadow() { return S_OK; }
	virtual void Render_Debug() override {};
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override {};

public:
	_bool Get_TrailOn();
	void TrailOn();
	void TrailEnd();
	void TrailOff();
	void Add_Point(_float4 vPos);
	void Set_Color(_float4 Color);
	void Set_Color2(_float4 Color2);
	void Set_Ratio(_float2 vRatio);
	void Set_BlurPower(_float fBlurPower);
	void Set_Size(_float fSize);
	void Set_MaxIndex(_uint iMaxIndex);
	void Set_TimeLimit(_float fTimeLimit);

private:
	CRenderer* m_pRendererCom = nullptr;
	CPointTrail* m_pTrail = nullptr;
public:
	static CPointTrailObj* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};

END
