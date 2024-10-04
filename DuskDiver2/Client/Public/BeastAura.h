#pragma once
#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CModel;
class CShader;
END

BEGIN(Client)

class CBeastAura final : public CCreture
{
public:
	typedef struct tagPlayer04AuraInfo
	{
		_float4x4 vWorldMatrix;	
		_float4 vLocalPos = { 0.f,0.f,0.f,1.f };
		_float3 vScale = { 1.f,1.f,1.f };
		_float4 vColor;
		_float4 vColor2;
		_float2 vRatio;
		_float2 vRatioSpeed;
		_float3 vScaleSpeed;
		_float fBlurPower;
		_float2 vUVFixSpeed;
		_float fAngle = 0.f;
		_float fTurnSpeed;
		_float2 vOriginRatio;
		_float3 vOriginScale;
		
	}AURAINFO;		 

private:
	CBeastAura(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CBeastAura(const CBeastAura& rhs);
	virtual ~CBeastAura() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

	void Set_On(_float4 vPos);
	void Set_Off();
	void Create_Effect();

private:
	HRESULT Ready_Components();

private:
	_bool m_bRender = false;
	_bool m_bStartFix = false;
	_bool m_bOff = false;
	_float m_fMaxUVIndexX;
	_float m_fMaxUVIndexY;
	_float2 m_vUVFixAcc;
	_float m_fParticleAcc = 0.f;

private:
	CShader*				m_pShaderCom = nullptr;
	CModel* m_pModelCom = nullptr;
	AURAINFO* m_pInfo = nullptr;
public:
	static CCreture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END