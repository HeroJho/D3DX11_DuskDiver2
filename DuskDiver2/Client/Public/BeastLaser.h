#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
END

BEGIN(Client)

class CBeastLaser : public CCreture
{
public:
	typedef struct BeastLaserInfo
	{
		_float4 vPosition;
		_float4 vTargetLook;
		_float4 vColor1 = { 1.f,0.f,0.f,1.f };
		_float4 vColor2 = { 0.f,0.f,0.f,1.f };
		_float2 vRatio = { 2.f,10.f };
		_float	fBlurPower = 10.f;
		_float4 vColor1_2 = { 0.f,0.f,0.f,1.f };
		_float4 vColor2_2 = { 0.f,0.f,0.f,1.f };
		_float2 vRatio_2 = { 1.f,3.f };
		_float	fBlurPower_2 = 2.f;
		_float2 vUVFixSpeed = { 0.f,0.f };
		_float2 vUVFixAcc = { 0.f,0.f };
		_float2 vUVFixSpeed_2 = { -1.f,3.f };
		_float2 vUVFixAcc_2 = { 0.f,0.f };
		_float3 vScaleSpeed;
		_float3 vScale = { 1.f,1.f,1.f };


	}LASERINFO;

private:
	CBeastLaser(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBeastLaser(const CBeastLaser& rhs);
	virtual ~CBeastLaser() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

	virtual void Render_Debug() override;


private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CModel*					m_pBreathCom = nullptr;
private:
	HRESULT Ready_Components();
	void	Change_ColRange(_float3 vSize, _float3 vCenter);

private:
	LASERINFO*		m_pInfo = nullptr;
	
	_float			m_fDeadAcc = 0.f;
	_bool			m_bPlayerHit = true; //플레이어 한번만 충돌

	//스케일 조정
	_float			m_vScale = 1.f;
	_float			m_vColScale = 0.f;
	


	
public:
	static CBeastLaser* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCreture* Clone(void* pArg);
	virtual void Free() override;
};

END