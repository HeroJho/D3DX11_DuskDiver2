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


class CRing final : public CEffect
{
public:
	typedef struct tagRingInfo
	{
		_float4x4 vWorldMatrix;
		_float4 vLocalPos;
		_float3 vScale;
		_float4 vColor;
		_float4 vColor2;
		_float3 vFixScaleSpeed;
		_float2 vRatio;
		_float2 vRatioSpeed;
		_float fBlurPower = 0.f;
		_bool bFixPos = false;
		_float fUpSpeed = 0.f;
	}RINGINFO;		 

private:
	CRing(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CRing(const CRing& rhs);
	virtual ~CRing() = default;

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
	RINGINFO* m_pInfo = nullptr;
	_float m_fLifeTimeAcc = 0.f;
public:
	static CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END