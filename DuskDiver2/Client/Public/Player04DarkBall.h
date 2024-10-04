#pragma once
#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTransform;
END

BEGIN(Client)


class CPlayer04DarkBall final : public CCreture
{
public:
	typedef struct tagCPlayer04DarkBallInfo
	{
		_float4x4 vWorldMatrix;
		_float4 vColor;
		_float4 vColor2;
		_float2 vRatio;
		_float2 vRatioSpeed;
		_float fBlurPower;
		_float4 vLocalPos;
		_float3 vScale;
		_float2 vScaleSpeed;
		_float2 vScaleTime;
		_float fSpeed;
		_float fTimeLimit;
		_uint iChargeCount;
	}DARKBALLINFO;		 

private:
	CPlayer04DarkBall(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CPlayer04DarkBall(const CPlayer04DarkBall& rhs);
	virtual ~CPlayer04DarkBall() = default;

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
	_float2 m_vUVFixAcc = { 0.f,0.f };
	_float m_fTimeLimitAcc = 0.f;
	_bool m_bScaleUp = true;
	_float2 m_vScaleTimeAcc = { 0.f,0.f };
	_bool m_bAttackCol = false;
	_float m_fAttackColAcc = 0.f;
private:
	CShader*				m_pShaderCom = nullptr;
	CModel* m_pModelCom = nullptr;
	DARKBALLINFO* m_pInfo = nullptr;
public:
	static CCreture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END