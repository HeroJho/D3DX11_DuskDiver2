#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CTexture;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CBeastClaw : public CCreture
{
private:
	CBeastClaw(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBeastClaw(const CBeastClaw& rhs);
	virtual ~CBeastClaw() = default;

public:
	typedef struct tagClawInfo
	{
		_float4x4 vWorldMatrix;
		_float4 vColor;
		_float4 vColor2;
		_float2 vRatio;
		_float2 vRatioSpeed;
		_float fBlurPower;
		_float4 vLocalPos;
	}CLAWINFO;

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
	CTexture*				m_pTextureCom = nullptr;

public:
	void	Render_OnOff(_bool	bOnOff) { m_bRender = bOnOff; }
	_bool	Get_RenderOnOff() { return m_bRender; }
	void	Set_ColStoneOnOff(_bool bOnOff) { m_bColStone = bOnOff; }
	_bool	Get_ColStone() { return m_bColStone; }

private:
	HRESULT Ready_Components();
	void	Change_ClawSize(_float fTimeDelta);
	void	Change_ColRange(_float3 vSize, _float3 vCenter);

private:
	CLAWINFO* m_pInfo = nullptr;

	_float4			m_vTargetPos = {0.f, 0.f, 0.f, 1.f};
	_float3			m_vScale = { 1.f, 1.f, 1.f };
	_float3			m_vColScale = { 0.f, 0.f, 0.f };

	_float			m_fColZSize = 0.f;
	_float			m_fSize = 0.f;
	_float			m_fBlurPower = 0.f;
	_float			m_fColDelayTime = 0.f;
	_float			m_fBreathDistance = 0.f;

	_bool			m_bRender = false;
	_bool			m_bHurtCol = false;
	_bool			m_bColStone = false;
	_bool			m_bAttackCol = false;
	_bool			m_bPlayerHit = true;

public:
	static CBeastClaw* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCreture* Clone(void* pArg);
	virtual void Free() override;
};

END