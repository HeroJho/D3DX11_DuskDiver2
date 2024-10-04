#pragma once
#include "VIBuffer.h"
#include "Client_Defines.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CTransform;
END

BEGIN(Client)
class CPointTrail :	public CVIBuffer
{
public: 
	typedef struct tagTrailCreateInfo {
		_float		fSize;
		_float4		vColor;
		_float4		vColor2;
		_float2		vRatio;
		_bool		bFixColor = false;
		_float		fBlurPower = 0.f;
		_tchar*		TexName;
		_uint		iShaderPass = 0;
		_uint iMaxIndex = 20;
	}TRAILCREATEINFO;

	typedef struct tagTrailInfo {
		_float4 vPos;
		_float3 vRight;
		_float3 vUp = { 0.f,1.f,0.f };
		_float fUV;
		
	}TRAILINFO;

private:
	CPointTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPointTrail(const CPointTrail& rhs);
	virtual ~CPointTrail() = default;

public:
	virtual HRESULT	Initialize_Prototype() override;
	virtual HRESULT	Initialize(void* pArg) override;
	HRESULT Render();
	void Tick(const _float& _fTimeDelta);
	void TrailOn() { m_bTrailOn = true; }
	void TrailOff();
	const _bool& Get_TrailOn() { return m_bTrailOn; }
	void Set_TrailOff() { m_bTrailOn = false; }
	void Set_Color(_float4 Color) { m_vColor = Color; }
	void Add_Point(_float4 vPos);
	void Set_Color2(_float4 Color2) { m_vColor2 = Color2; }
	void Set_Ratio(_float2 vRatio) { m_vRatio = vRatio; }
	void Set_BlurPower(_float fBlurPower) { m_fBlurPower = fBlurPower; }
	void Set_Size(_float fSize) { m_fSize = fSize; }
	void Set_MaxIndex(_uint iMaxIndex) { m_iMaxIndex = iMaxIndex; }
	void Set_TimeLimit(_float fTimeLimit) { m_fTimeLimit = fTimeLimit; }

private:
	_float4					m_vColor;
	_float4					m_vColor2;
	_float2					m_vRatio;
	_float					m_fBlurPower;
	_bool					m_bFixColor;
	_uint					m_iShaderPass = 0;
	_float					m_fSize;
	_float					m_fTimeAcc = 0.f;
	_float					m_fTimeLimit = 0.017f;
	_uint					m_iMaxIndex = 20;
	_bool					m_bTrailOn = false;
	_bool					m_bStart = false;
	vector<TRAILINFO> m_TrailInfos;

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
public:
	static	CPointTrail*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent*		Clone(void* pArg);
	virtual void			Free() override;
};
END

/*
Tick
if (m_bDead)
{

return E_FAIL;
}


AUTOINSTANCE(CGameInstance, _pGameInstance);

m_fCurTime += _fTimeDelta;
if (m_fCurTime > m_fMaxTime)
{
m_vPos.erase(m_vPos.begin());
m_vRight.erase(m_vRight.begin());
m_vUp.erase(m_vUp.begin());
m_vUV.erase(m_vUV.begin());
}

if (m_vPos.size() < 1)
{
m_bDead = true;
}

if (m_bAlpha)
{
for (_uint i = 0; i < m_vUV.size(); ++i)
{
_float _iVtxCount = 0.f;
if (m_vUV.size() < 1)
{
_iVtxCount = 1.f;
}
else
{
_iVtxCount = (_float)m_vUV.size() - 1.f;
}
m_vUV[i] = _float(i) / ((_float)_iVtxCount);
}
}



return S_OK;

*/


/*
GS_OUT         Out[8];
matrix         matVP = mul(g_ViewMatrix, g_ProjMatrix);

Out[0].vPosition = mul(vector(g_vPos_2 - (g_Right2 * g_Width), 1.f), matVP);
Out[0].vTexUV = float2(g_Uv2, 0.f);

Out[1].vPosition = mul(vector(g_vPos_2 + (g_Right2 * g_Width), 1.f), matVP);
Out[1].vTexUV = float2(g_Uv2, 1.f);

Out[2].vPosition = mul(vector(g_vPos_1 + (g_Right1 * g_Width), 1.f), matVP);
Out[2].vTexUV = float2(g_Uv1, 1.f);

Out[3].vPosition = mul(vector(g_vPos_1 - (g_Right1 * g_Width), 1.f), matVP);
Out[3].vTexUV = float2(g_Uv1, 0.f);

GeometryStream.Append(Out[0]);
GeometryStream.Append(Out[1]);
GeometryStream.Append(Out[2]);
GeometryStream.RestartStrip();

GeometryStream.Append(Out[0]);
GeometryStream.Append(Out[2]);
GeometryStream.Append(Out[3]);
GeometryStream.RestartStrip();



Out[4].vPosition = mul(vector(g_vPos_1 + (g_Up1 * g_Width), 1.f), matVP);
Out[4].vTexUV = float2(g_Uv1, 1.f);

Out[5].vPosition = mul(vector(g_vPos_2 + (g_Up2 * g_Width), 1.f), matVP);
Out[5].vTexUV = float2(g_Uv2, 1.f);

Out[6].vPosition = mul(vector(g_vPos_2 - (g_Up2 * g_Width), 1.f), matVP);
Out[6].vTexUV = float2(g_Uv2, 0.f);

Out[7].vPosition = mul(vector(g_vPos_1 - (g_Up1 * g_Width), 1.f), matVP);
Out[7].vTexUV = float2(g_Uv1, 0.f);


GeometryStream.Append(Out[4]);
GeometryStream.Append(Out[5]);
GeometryStream.Append(Out[6]);
GeometryStream.RestartStrip();

GeometryStream.Append(Out[4]);
GeometryStream.Append(Out[6]);
GeometryStream.Append(Out[7]);
GeometryStream.RestartStrip();
*/