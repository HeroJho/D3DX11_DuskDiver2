#pragma once
#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
END

BEGIN(Client)

class CBeastTornado final : public CCreture
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
		_float fLifeTime;
		_bool bOrigin = false;
	}TORNADOINFO;

private:
	CBeastTornado(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CBeastTornado(const CBeastTornado& rhs);
	virtual ~CBeastTornado() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	virtual void Create_HitEffect(_float4 vWorldPos)override;
	_bool Get_TornadoEnd() {return m_bTorandoEnd;}

private:
	HRESULT Ready_Components();
	void	Change_ColRange(_float3 vSize, _float3 vCenter);
	void	Check_Death(_float fTimeDelta);
	void	Check_Hurt(_float fTimeDelta);
	void	Change_TornadoSize(_float fTimeDelta);
	void	Instant_ChangeSize(_float fTimeDelta);
	void	Chase_Player();

private:
	TORNADOINFO*	m_pInfo = nullptr;
	_float4		m_vPlayerPos = { 0.f, 0.f, 0.f, 0.f };
	_float3		m_vTargetLook = { 0.f, 0.f, 0.f };
	_float3		m_vTornadoScale = { 1.f, 1.f, 1.f };
	_float3		m_vColScale = { 0.f, 0.f, 0.f };

	_float		m_fSoundAcc = 0.f;
	_float		m_fChaseAcc = 0.f;
	_float		m_fSpeed = 1.2f;
	_float		m_fPlusScale = 0.f;
	_float		m_fTurnSpeed = 1.5f;
	_float		m_fDeadAcc = 0.f;
	_float		m_fDeadLimitTime = 0.f;
	_float		m_fColDelayTime = 0.f;

	_float		m_fColLimitAcc = 0.f;

	_bool		m_bPlayerChase = true;
	_bool		m_bInstantSize = true;
	_bool		m_bChange = false;
	_bool		m_bDeadStart = false;
	_bool		m_bOneCol = true;
	_bool		m_bTorandoEnd = false;
	_bool		m_bPlayerHit = true;
	_bool		m_bColDelay = false;
	_float m_fLifeTimeAcc = 0.f;
	_float m_fScaleTimeAcc = 0.f;
	_bool m_bScaleEnd = false;
	_float m_fMaxUVIndexX = 0.f;
	_float m_fMaxUVIndexY = 0.f;
	_float2 m_vUVFixAcc = { 0.f,0.f };
	_float m_fParticleAcc = 0.f;
private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;

public:
	static CCreture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END