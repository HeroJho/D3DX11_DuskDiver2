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

class CBeastMarvelBall : public CCreture
{
private:
	CBeastMarvelBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBeastMarvelBall(const CBeastMarvelBall& rhs);
	virtual ~CBeastMarvelBall() = default;

public:
	typedef struct tagBallInfo
	{
		_float fBlurPower;
		_float4 vColor;
		_float4 vColor2;
		_float2 vRatio;
		_float2 vRatioSpeed;
		_float3 vScale;
		_float3 vScaleSpeed;
		_float4 vPos;
	}BALLINFO;

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
	void	Render_OnOff(_bool	bOnOff) { m_bRender = bOnOff; m_bStartEffect = bOnOff; }
	_bool	Get_RenderOnOff() { return m_bRender; }
	_bool	Get_ExplosionState() { return m_bExplosion; }
	_bool	Get_SuccessState() { return m_bSuccess; }

	void	Chase_Start() { m_bChaseTarget = true; }
	void	Attack_On(_bool bOnOff) { m_bAttackCol = bOnOff; }

	void	Set_TargetPos(_float4 vTargetPos, _uint iIndex);
	void	Reset_State();
	void	Reset_Position();

private:
	HRESULT Ready_Components();
	void Create_ColEffect();
	void Create_EndEffect();
	void Create_AppearEffect();
	void Create_Effect();

private:
	_bool Move_MarvelBall(_vector vTargetPos, _float fSpeed, _float fTimeDelta, _float fLimitDistance = 0.1f);

private:
	BALLINFO* m_pInfo = nullptr;

	_float4			m_vTargetPos[5];

	_float			m_fEffectAcc = 0.f;
	_float			m_fSize = 0.f;
	_float			m_fBlurPower = 0.f;
	_uint			m_iTargetIndex = 0;
	_bool			m_bChaseTarget = false;
	_bool			m_bGoDir = false;
	_bool			m_bRender = false;
	_bool			m_bAttackCol = true;
	_bool			m_bExplosion = false;
	_bool			m_bOneHit = true;
	_bool			m_bSuccess = false;

	_bool			m_bStartEffect = false;

public:
	static CBeastMarvelBall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCreture* Clone(void* pArg);
	virtual void Free() override;
};

END