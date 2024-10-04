#pragma once
#include "Client_Defines.h"
#include "Effect.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTransform;
END

BEGIN(Client)


class CPlayer01Wind2 final : public CEffect
{
public:
	typedef struct tagPlayer02Wind1Info
	{
		_float4x4 vWorldMatrix;
		_float4 vLocalPos;
		_float4 vColor;
		_float4 vColor2;
		_float2 vRatio;
		_float2 vRatioSpeed;
		_float fBlurPower = 1.f;
		_float3 vScaleSpeed;
		_bool bRotation = false;
		_float3 vRotation;
		_float3 vScale = { 0.1f,0.1f,0.1f };
		_float fTurnSpeed = 10.f;
		_bool bLookFix = false;
		_float fLookSpeed = 0.f;
		_float fLifeTime = 0.f;
	}WIND2INFO;		 

private:
	CPlayer01Wind2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CPlayer01Wind2(const CPlayer01Wind2& rhs);
	virtual ~CPlayer01Wind2() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
	
private:
	_float m_fLifeTimeAcc = 0.f;
	CShader*				m_pShaderCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel* m_pModelCom = nullptr;
	WIND2INFO* m_pInfo = nullptr;
public:
	static CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END