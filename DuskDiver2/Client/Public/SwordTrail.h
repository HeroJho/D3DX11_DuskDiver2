#pragma once
#include "VIBuffer.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
END

BEGIN(Client)
class CSwordTrail :	public CVIBuffer
{
public: 
	typedef struct tagTrailPos
	{
		_float3 vHigh;
		_float3 vLow;
	}TRAILPOS;

	struct TRAILINFO {
		TRAILPOS	HighAndLow;
		_float4		vColor1 = { 1.f,1.f,1.f,1.f };
		_float4		vColor2 = { 1.f,1.f,1.f,1.f };
		_float2		vRatio = { 1.f,1.f };
		_bool		bFixColor = false;
		_float		fBlurPower = 0.f;
		_uint		iShaderPass = 0;
		_tchar*		TexName;
	};

private:
	CSwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSwordTrail(const CSwordTrail& rhs);
	virtual ~CSwordTrail() = default;

public:
	virtual HRESULT			Initialize_Prototype() override;
	virtual HRESULT			Initialize(void* pArg) override;
	HRESULT Render();

	void Tick(const _float& _fTimeDelta, _matrix _matWeapon);
	void			TrailOn(_matrix _matWeapon);
	
	void			TrailOff() { m_bTrailOn = false; } 

	const _bool&	Get_TrailOn() { return m_bTrailOn; }

	void Set_Color(_float4 vColor) { m_vColor = vColor; }
	void Set_Color2(_float4 vColor) { m_vColor2 = vColor; }
	void Set_Ratio(_float2 vRatio) { m_vRatio = vRatio; }
	void Set_BlurPower(_float fBlurPower) { m_fBlurPower = fBlurPower; }
	void Set_TimeLimit(_float fTimeLimit) { m_fTimeLimit = fTimeLimit; }
	void Set_HighLow(_float3 vHigh, _float3 vLow) { m_HighAndLow.vHigh = vHigh; m_HighAndLow.vLow = vLow; }
	
private:
	TRAILPOS				m_HighAndLow;
	VTXTEX					m_RealData[22];
	_float4					m_vColor;
	_float4					m_vColor2;
	_float2					m_vRatio;
	_bool					m_bFixColor = false;
	_float					m_fBlurPower = 0.f;
	_uint					m_iShaderPass = 0;
	_float					m_fTimeAcc = 0.f;
	_float					m_fTimeLimit = 0.01f;
	_bool					m_bTrailOn = false;
	_int					m_iVtxCount = 0;

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
public:
	static	CSwordTrail*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent*		Clone(void* pArg);
	virtual void			Free() override;


};
END