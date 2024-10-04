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

class CPlayer04VoidParticle : public CGameObject
{
public:
	typedef struct tagParticleInfo
	{
		_float fSpeed;
		_float fLifeTime;
		_float3 vDirection;
		_float fGravity;
		_float4x4 WorldMatrix;
		const _tchar* TexPath = L"";
		const _tchar* TexName = L"";
		_bool bUV = false;
		int iShaderPass;
		_float fUVSpeed;
		_float fMaxUVIndexX;
		_float fMaxUVIndexY;
		_bool bUVLoof = false;
		_float2 vScaleSpeed = _float2{ 0.f,0.f };
		_bool bBlend = false;
		_float2 vUVFixSpeed = { 0.f,0.f };
		_float fBlurPower = 0.f;		
		_bool bFixColor;
		_float2 vRatio;
		_float2 vRatioSpeed;
		_bool bUVFix;
		_float fRotationSpeed = 0.f;
		_float fRotationRatio = 0.f;
		_float fRotationTime = 0.f;
	}PARTICLEINFO;
	typedef struct tagColorInfo
	{
		_float fY = 1.f;
		_float2 vSize = { 1.f,1.f };
		_float4 vColor = { 1.f,1.f,1.f,1.f };
		_float4 vColor2 = { 1.f,1.f,1.f,1.f };
	}COLORINFO;

private:
	CPlayer04VoidParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer04VoidParticle(const CPlayer04VoidParticle& rhs);
	virtual ~CPlayer04VoidParticle() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;


private:
	PARTICLEINFO* m_ParticleInfo = nullptr;
	_float m_fLifeTimeAcc = 0.f;
	_float m_fGravityAcc = 0.f;
	_float m_fShaderUVAcc = 0.f;
	_float m_fShaderUVIndexX = 0;
	_float m_fShaderUVIndexY = 0;
	const _tchar* TexPath;
	const _tchar* TexName;
	_float2 m_vUVFixAcc = { 0.f,0.f };
	_matrix m_RotationMatrix;
	_float3 m_vCamLook;
	_float m_fRotationTimeAcc = 0.f;
	_float m_fRotationAngle = 0.f;


private:
	COLORINFO*				m_pInfo = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Particle*			m_pVIBufferCom = nullptr;

private:
	HRESULT Ready_Components();

public:
	static CPlayer04VoidParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END