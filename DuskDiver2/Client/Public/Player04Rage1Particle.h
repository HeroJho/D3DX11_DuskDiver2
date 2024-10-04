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

class CPlayer04Rage1Particle : public CGameObject
{
public:
	typedef struct tagParticleInfo
	{
		_float2 vSize;
		_float fSpeed;
		_float fLifeTime;
		const _tchar* TexPath = L"";
		const _tchar* TexName = L"";
		int iShaderPass;
		_float fUVSpeed;
		_float fMaxUVIndexX;
		_float fMaxUVIndexY;
		_float fBlurPower = 0.f;
		_float4 vColor;
		_float4 vColor2;
		_bool bFixColor;
		_float2 vRatio;
		_float2 vRatioSpeed;
		_float fRotationSpeed = 0.f;
		_float fRotationRatio = 0.f;
		_float fRotationTime = 0.f;
	}PARTICLEINFO;

	typedef struct tagCreateInfo
	{
		_float4 vCreatePos;
	}RAGEPARTICLEINFO;
private:
	CPlayer04Rage1Particle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer04Rage1Particle(const CPlayer04Rage1Particle& rhs);
	virtual ~CPlayer04Rage1Particle() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;


private:
	PARTICLEINFO* m_ParticleInfo = nullptr;
	RAGEPARTICLEINFO* m_CreateInfo = nullptr;
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
	_float3 m_vTargetLook;
	_uint m_iIndex;
	_bool m_bCombine = false;

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CTransform*				m_pTransformCom = nullptr;
	CVIBuffer_Particle*		m_pVIBufferCom = nullptr;

private:
	HRESULT Ready_Components();

public:
	static CPlayer04Rage1Particle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END