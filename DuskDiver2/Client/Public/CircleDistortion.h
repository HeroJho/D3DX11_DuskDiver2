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


class CCircleDistortion final : public CEffect
{
public:
	typedef struct tagCircleDistortionInfo
	{
		_float4x4 vWorldMatrix;
		_float2 vRatio;
		_float2 vRatioSpeed;
		_float fLimitTime;
		_float fBlurPower = 1.f;
		_float3 vScaleSpeed;
		_float3 vScale;
		_float4 vLocalPos;
		_float3 vScaleAccSpeed;
	}CIRCLEDISTORTIONINFO;		 

private:
	CCircleDistortion(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CCircleDistortion(const CCircleDistortion& rhs);
	virtual ~CCircleDistortion() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
private:
	_float m_fLimitAcc = 0.f;
	_float4 m_vColor = { 0.f,0.f,0.f,1.f };
	_float4 m_vColor2 = { 0.f,0.f,0.f,1.f };
	CShader*				m_pShaderCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel* m_pModelCom = nullptr;
	CIRCLEDISTORTIONINFO* m_pInfo = nullptr;
public:
	static CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END