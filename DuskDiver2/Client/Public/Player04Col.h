#pragma once
#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)

END

BEGIN(Client)

class CPlayer04Col final : public CCreture
{
public:
	typedef struct tagSlashInfo
	{
		_float4 vLocalPos;
		_float fLifeTime;
		_float4x4 vWorldMatrix;
	}COLINFO;		 

private:
	CPlayer04Col(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CPlayer04Col(const CPlayer04Col& rhs);
	virtual ~CPlayer04Col() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

private:
	HRESULT Ready_Components();

private:
	_float m_fLifeTimeAcc = 0.f;
	COLINFO* m_pInfo = nullptr;
public:
	static CCreture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END