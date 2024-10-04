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


class CPlayer04SmokeBall final : public CCreture
{
public:
	typedef struct tagCPlayer04SmokeBallInfo
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
		_float fSpeed;
		_float fTimeLimit;
	}SMOKEBALLINFO;		 

private:
	CPlayer04SmokeBall(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CPlayer04SmokeBall(const CPlayer04SmokeBall& rhs);
	virtual ~CPlayer04SmokeBall() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Components();
	_float2 m_vUVFixAcc = { 0.f,0.f };
	_float m_fTimeLimitAcc = 0.f;

private:
	CShader*				m_pShaderCom = nullptr;
	CModel* m_pModelCom = nullptr;
	SMOKEBALLINFO* m_pInfo = nullptr;
public:
	static CCreture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END