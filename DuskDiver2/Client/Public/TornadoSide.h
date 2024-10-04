#pragma once
#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CModel;
class CShader;
END

BEGIN(Client)

class CTornadoSide final : public CCreture
{
public:
	typedef struct tagTornadoSideInfo
	{
		_float4x4 vWorldMatrix;	
		_float4 vLocalPos = { 0.f,0.f,0.f,1.f };
		_float3 vScale = { 1.f,1.f,1.f };
		_float4 vColor;
		_float4 vColor2;
		_float2 vRatio;
		_float2 vRatioSpeed;
		_float fBlurPower;
		_float2 vUVFixSpeed;
		_float fLifeTime;
		_float2 vUVFixAcc = { 0.f,0.f };
		_float fAngle = 0.f;
	}TORNADOSIDEINFO;		 

private:
	CTornadoSide(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CTornadoSide(const CTornadoSide& rhs);
	virtual ~CTornadoSide() = default;

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
	_float m_fMaxUVIndexX;
	_float m_fMaxUVIndexY;
	_float2 m_vUVFixAcc;
	_float m_fLifeTimeAcc = 0.f;
	_bool m_bEnd = false;

private:
	CShader*				m_pShaderCom = nullptr;
	CModel* m_pModelCom = nullptr;
	TORNADOSIDEINFO* m_pInfo = nullptr;
public:
	static CCreture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END