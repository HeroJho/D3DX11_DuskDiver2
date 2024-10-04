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


class CPlayer01SmokeBall final : public CEffect
{
public:
	typedef struct tagCPlayer01SmokeBallInfo
	{
		_float4x4 vWorldMatrix;
		_float4 vColor;
		_float4 vColor2;
		_float2 vRatio;
		_float2 vRatioSpeed;
		_float fBlurPower;
		_float3 vScaleSpeed;
		_float4 vLocalPos;
		_float3 vScale;
		_float2 vUvFixSpeed;
		_bool bFixPos = false;
		_float fLookSpeed = 0.f;
		_float fAngle = 0.f;
	}SMOKEBALLINFO;		 

private:
	CPlayer01SmokeBall(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CPlayer01SmokeBall(const CPlayer01SmokeBall& rhs);
	virtual ~CPlayer01SmokeBall() = default;

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
	SMOKEBALLINFO* m_pInfo = nullptr;
public:
	static CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END