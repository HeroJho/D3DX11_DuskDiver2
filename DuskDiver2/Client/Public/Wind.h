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


class CWind final : public CEffect
{
public:
	typedef struct tagWindInfo
	{
		_float4x4 WorldMatrix;
		_float2 vUVFixSpeed;
		_float4 vColor;
		_float4 vColor2;
		_float3 vFixPosSpeed;
		_float3 vFixScaleSpeed;
		_float2 vRatio;
		_float2 vRatioSpeed;
		_float fBlurPower = 0.f;
		_bool bDistortion = false;
		_bool bRotation = false;
		_float3 vRotation;
	}WINDINFO;		 

private:
	CWind(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CWind(const CWind& rhs);
	virtual ~CWind() = default;

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
	WINDINFO* m_WindInfo = nullptr;
	_float m_fLifeTimeAcc = 0.f;
public:
	static CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END