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


class CPlayer01Metamor final : public CEffect
{
public:
	typedef struct tagPlayer01MetamorInfo
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
		_float fTurnSpeed = 0.f;
		_bool bPosFix = false;
		_float fUpSpeed = 0.f;
		_float2 vUVFixSpeed = { 0.f,0.f };
	}METAMORINFO;		 

private:
	CPlayer01Metamor(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CPlayer01Metamor(const CPlayer01Metamor& rhs);
	virtual ~CPlayer01Metamor() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
private:
	CShader*				m_pShaderCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel* m_pModelCom = nullptr;
	METAMORINFO* m_pInfo = nullptr;
public:
	static CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END