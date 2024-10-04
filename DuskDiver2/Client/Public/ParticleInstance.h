#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Particle_Instance;
END

BEGIN(Client)

class CParticleInstance : public CGameObject
{
public:
	typedef struct tagParticleInstanceInfo
	{
		_float4 vMinPos;
		_float4 vMaxPos;
		_float2 vMinSize;
		_float2 vMaxSize;
		_float fMinSpeed = 0.f;
		_float fMaxSpeed = 0.f;
		_float fMinLifeTime = 0.f;
		_float fMaxLifeTime = 0.f;
		_float fGravity = 0.f;
		_float4x4 WorldMatrix;
		_float3 vDirAngle;
		_float3 vRangeAngle1;
		_float3 vRangeAngle2;
		int iParticleNum;
		const _tchar* TexPath = L"";
		const _tchar* TexName = L"";
		_bool bUV;
		_bool bUVLoof;
		int iShaderPass;
		_float fUVMaxIndexX;
		_float fUVMaxIndexY;
		_float fUVSpeed;
		_float2 vReleaseSpeed;
		_bool bBlend = false;
		_float fBlurPower = 0.f;
		_float fAlphaTest = 0.f;
		_bool bFixColor = false;
		_float4 vColor = { 0.f,0.f,0.f,1.f };
		_float4 vColor2 = { 0.f,0.f,0.f,1.f };
		_float2 vRatio = { 1.f,1.f };
		_float2 vRatioSpeed = { 0.f,0.f };
		_bool bCharge = false;
		_float4 vChargePos = { 0.f,0.f,0.f,1.f };
		_float fChargeRange = 0.1f;
		_bool bDistortion = false;
		_float fDistortionPower = 0.f;
		_bool bFollow = false;
		_float4 vLocalPos = { 0.f,0.f,0.f,1.f };
	}PARTICLEINSTANCEINFO;
private:
	CParticleInstance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticleInstance(const CParticleInstance& rhs);
	virtual ~CParticleInstance() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


private:
	PARTICLEINSTANCEINFO* m_ParticleInfo = nullptr;
	int m_iShaderPass = 0;
	_float m_fLifeTimeAcc = 0.f;
	_float m_fGravityAcc = 0.f;
	_float m_fShaderUVAcc = 0.f;
	_float m_fShaderUVIndexX = 0;
	_float m_fShaderUVIndexY = 0;


private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Particle_Instance*			m_pVIBufferCom = nullptr;

private:
	HRESULT Ready_Components();

public:
	static CParticleInstance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END