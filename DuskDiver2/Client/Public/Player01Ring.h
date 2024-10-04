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


class CPlayer01Ring final : public CEffect
{
public:
	typedef struct tagPlayer01Wind1Info
	{
		_float4x4 vWorldMatrix;
		_float4 vLocalPos;
		_float4 vColor;
		_float4 vColor2;
		_float2 vRatio;
		_float2 vRatioSpeed;
		_float fBlurPower = 0.f;
		_float3 vScale;
		_float3 vScaleSpeed;
	}RINGINFO;		 

private:
	CPlayer01Ring(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CPlayer01Ring(const CPlayer01Ring& rhs);
	virtual ~CPlayer01Ring() = default;

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
public:
	static CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END