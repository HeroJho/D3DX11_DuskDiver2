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


class CPlayer01Rage1Ball final : public CEffect
{
public:
	typedef struct tagPlayer01Rage1BallInfo
	{
		_float4x4 vWorldMatrix;
		_float4 vColor;
		_float4 vColor2;
		_float2 vRatio;
		_float2 vRatioSpeed;
		_float fLimitTime;
		_float fBlurPower = 1.f;
		_float3 vScaleSpeed;
		_float4 vLocalPos;
		_float3 vScale = { 0.1f,0.1f,0.1f };
		_uint iShaderPass = 3;
	}RAGE1BALLINFO;		 

private:
	CPlayer01Rage1Ball(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CPlayer01Rage1Ball(const CPlayer01Rage1Ball& rhs);
	virtual ~CPlayer01Rage1Ball() = default;

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
	_bool m_bEffect1 = false;
	_bool m_bEffect2 = false;
	_bool m_bEffect3 = false;
	_bool m_bEffect4 = false;
	_bool m_bEffect5 = false;
	_bool m_bEffect6 = false;
	_bool m_bScaleFix = true;
	_float3 m_vOriginScale1;
	_float3 m_vOriginScale2;
	_float3 m_vOriginScale3;
	_float4 m_vOriginColor;
	_float4 m_vOriginColor2;
	CShader*				m_pShaderCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel* m_pModelCom = nullptr;
	RAGE1BALLINFO* m_pInfo = nullptr;
public:
	static CEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END