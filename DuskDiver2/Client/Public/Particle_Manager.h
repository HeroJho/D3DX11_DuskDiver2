#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "AlphaParticle.h"
BEGIN(Client)
class CAlphaParticle;

class CParticle_Manager final : public CBase
{

public:
	typedef struct tagParticleCreateInfo
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
		_float4 vWorldPos = { 0.f,0.f,0.f,1.f };
		_float3 vDir;
		int iParticleNum;
		const _tchar* TexPath = L"";
		const _tchar* TexName = L"";
		_bool bUV;
		_bool bUVLoof;
		int iShaderPass;
		_float fUVMaxIndexX;
		_float fUVMaxIndexY;
		_float fUVSpeed;
		_float2 vScaleSpeed = _float2{ 0.f,0.f };
		_bool bBlend = false;
		_float2 vUVFixSpeed = { 0.f,0.f };
		_bool bFixColor = false;
		_float4 vColor1 = { 0.f,0.f,0.f,1.f };
		_float4 vColor2 = { 0.f,0.f,0.f,1.f };
		_float2 vRatio = { 1.f,1.f };
		_float2 vRatioSpeed = { 0.f,0.f };
		_float fBlurPower = 0.f;
		_bool bUVFix = false;
		_float fRotationSpeed = 0.f;
		_float fRotationRatio = 0.f;
		_float fRotationTime = 0.f;
		_bool bFollow = false;
	}PARTICLECREATE;

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
		_float4 vWorldPos = { 0.f,0.f,0.f,1.f };
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
	}PARTICLEINSTANCEINFO;

	DECLARE_SINGLETON(CParticle_Manager)
private:
	CParticle_Manager();
	virtual ~CParticle_Manager() = default;

public:
	void Initalize();
	void LoadAll();
	void LoadParticle(char* LoadName);
	void LoadParticleInstance(char* LoadName);
	PARTICLECREATE SearchParticle(_tchar* ParticleName);
	PARTICLEINSTANCEINFO SearchParticleInstance(_tchar* ParticleName);
	HRESULT Create_AlphaParticle(_tchar* ParticleName, _float4x4 WorldMatrix);
	HRESULT Create_InstanceParticle(_tchar* ParticleName, _float4x4 WorldMatrix);
private:
	_float4 m_vPos = { 0.f,0.f,0.f,1.f };
	_float2 m_vSize;
	_float m_fLifeTime;
	_float m_fSpeed;
	map<_tchar*, PARTICLECREATE> m_ParticleCreateInfo;
	map<_tchar*, PARTICLEINSTANCEINFO> m_ParticleInstanceCreateInfo;

public:
	virtual void Free() override;
};
END

