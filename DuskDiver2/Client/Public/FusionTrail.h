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


class CFusionTrail final : public CEffect
{
public:
	typedef struct tagPlayer01TrailInfo
	{
		_float4x4 vWorldMatrix;
		_float4 vColor;
		_float4 vColor2;
		_float2 vRatio;
		_float2 vRatioSpeed;
		_float3 fScale;
		_float fBlurPower = 1.f;
	}TRAILINFO;		 

private:
	CFusionTrail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CFusionTrail(const CFusionTrail& rhs);
	virtual ~CFusionTrail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Decrease_Start() { m_bDecrease = true; }
	HRESULT SetUp_State(_fmatrix StateMatrix);

private:
	HRESULT Ready_Components();
private:
	CShader*				m_pShaderCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel* m_pModelCom = nullptr;
	TRAILINFO* m_TrailInfo = nullptr;

private:
	_bool		m_bDecrease = false;

public:
	static CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END