#pragma once
#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CModel;
class CShader;
END

BEGIN(Client)

class CPlayer03CannonBullet final : public CCreture
{
public:
	typedef struct tagCannonBulletInfo
	{
		_float3 vScale;
		_float4x4 Worldmatrix;
		_bool bRage = false;
	}BULLETINFO;		 

private:
	CPlayer03CannonBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CPlayer03CannonBullet(const CPlayer03CannonBullet& rhs);
	virtual ~CPlayer03CannonBullet() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

private:
	HRESULT Ready_Components();

private:
	_float4 m_vColor1 = { 0.6f,0.6f,1.f,1.f };
	_float4 m_vColor2 = { 0.f,0.f,1.f,1.f };
	_float2 m_vRatio = { 1.f,1.f };
	_float m_fBlurPower = 10.f;
	_float m_fLimitAcc = 0.f;
	_float m_fMaxUVIndexX;
	_float m_fMaxUVIndexY;
	_float2 m_vUVFixAcc;
	_float2 m_vUVFixSpeed;
	_float m_fLifeTimeAcc = 0.f;
	_float3 m_vTargetLook;
	_float m_fColAcc = 0.f;
	_bool m_bAttackCol = true;
	_bool m_bRender = true;
	_bool m_bEffect1 = false;
	_bool m_bEffect2 = false;
	_bool m_bEffect3 = false;

private:
	CShader*				m_pShaderCom = nullptr;
	CModel* m_pModelCom = nullptr;
	CTransform* m_pTransformCom2 = nullptr;
	BULLETINFO* m_pInfo = nullptr;
public:
	static CCreture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END