#pragma once
#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CModel;
class CShader;
END

BEGIN(Client)

class CTornado final : public CCreture
{
public:
	typedef struct tagTornadoInfo
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
		_float fTurnSpeed;
		_float fDamage;
		_float fLifeTime;
		_bool bOrigin = false;
	}TORNADOINFO;		 

private:
	CTornado(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CTornado(const CTornado& rhs);
	virtual ~CTornado() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	virtual void Create_HitEffect(_float4 vWorldPos)override;

private:
	HRESULT Ready_Components();

private:
	_float m_fLimitAcc = 0.f;
	_float m_fMaxUVIndexX;
	_float m_fMaxUVIndexY;
	_float2 m_vUVFixAcc;
	_float m_fLifeTimeAcc = 0.f;
	_bool m_bAttackCol = true;
	_bool m_bEnd = false;
	_float m_fColAcc = 0.f;
	_float m_fParticleAcc = 0.f;
	_bool m_bDistortion = false;
	_bool m_bHitFix = false;
private:
	CShader*				m_pShaderCom = nullptr;
	CModel* m_pModelCom = nullptr;
	TORNADOINFO* m_pInfo = nullptr;
public:
	static CCreture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END