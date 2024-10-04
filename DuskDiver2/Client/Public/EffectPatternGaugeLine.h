#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Particle;
END

BEGIN(Client)

class CEffectPatternGaugeLine : public CGameObject
{
public:
	typedef struct tagEffectRangeInfo
	{
		_float2 vSize;
		_float4x4 WorldMatrix;
		_float fBlurPower = 0.f;
		_float2 vScaleSpeed = _float2{ 0.f,0.f };
		_float4 vColor;
		_float4 vColor2;
		_bool bFixColor = true;
		_float2 vRatio;
		_float2 vRatioSpeed;
		_float fLifeTime;
	}EFFECTRANGEINFO;
private:
	CEffectPatternGaugeLine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffectPatternGaugeLine(const CEffectPatternGaugeLine& rhs);
	virtual ~CEffectPatternGaugeLine() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;


public:
	void	Set_RatioSpeed(_float2 fSpeed) {  m_fOriginSpeed = fSpeed; }
	void	SetUp_Position(_float4 vPostion) { m_vPosition = vPostion; }
	_bool	Get_RenderOnOff() { return m_bRender; }
	void	Rnder_OnOff(_bool bOnOff) { m_bRender = bOnOff; }
	
private:
	EFFECTRANGEINFO* m_EffectRangeInfo = nullptr;
	EFFECTRANGEINFO* m_EffectInitInfo = nullptr;
	_float m_fLifeTimeAcc = 0.f;
	_float m_fShaderUVAcc = 0.f;
	_float m_fShaderUVIndexX = 0;
	_float m_fShaderUVIndexY = 0;


private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Particle*			m_pVIBufferCom = nullptr;

private:
	HRESULT Ready_Components();
	HRESULT SetUp_State(_fmatrix StateMatrix);
	void	Reset_State();
private:
	_bool	m_bRender = false;
	_bool	m_bChange = false;
	_bool	m_bSizeChnage = false;
	_float2 fTemp = { 5.f, 5.f };
	_float2 m_fOrigin = { 0.f, 0.f };
	_float2 m_fOriginSpeed = { 0.f, 0.f };
	_float4 m_vPosition = { 0.f, 0.f, 0.f, 1.f };
	_float	m_fScale = 0.f;
	_float m_fLimitSacle = 0.f;
	_float	m_fChnageScale = 0.f;
	_float2 m_vUVFixAcc = { 0.f,0.f };
	_bool	m_bOneReset = false;
public:
	static CEffectPatternGaugeLine* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END