#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)

class CParticleTool final : public CBase
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

	typedef struct tagParticleCreateInfo2
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
	}PARTICLECREATE2;

	typedef struct TexInfo
	{
		wstring TexPath;
		wstring TexName;
	}TEXINFO;

public:
	CParticleTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CParticleTool() = default;

public:
	HRESULT Initialize();

#pragma region ParticleWindow
public:
	void ShowParticleToolWindow(_float fTimeDelta);
#pragma endregion ParticleWindow

#pragma region SetParticle

private:
	void SetParticle(_float fTimeDelta);
	void ResetInfo();
	void SetParticleInfo(char* ParticleName);
private:
	_float m_fMinPosX = 0.f;
	_float m_fMinPosY = 0.f;
	_float m_fMinPosZ = 0.f;
	_float m_fMaxPosX = 0.f;
	_float m_fMaxPosY = 0.f;
	_float m_fMaxPosZ = 0.f;
	_float m_fMinSizeX = 0.f;
	_float m_fMinSizeY = 0.f;
	_float m_fMaxSizeX = 0.f;
	_float m_fMaxSizeY = 0.f;
	_float m_fMinSpeed = 0.f;
	_float m_fMaxSpeed = 0.f;
	_float m_fMinLifeTime = 0.f;
	_float m_fMaxLifeTime = 0.f;
	_float m_fDirectionX = 0.f;
	_float m_fDirectionY = 0.f;
	_float m_fDirectionZ = 0.f;
	_float m_fGravity = 0.f;
	_float4 m_vMinPos = { 0.f,0.f,0.f,1.f };
	_float4 m_vMaxPos = { 0.f,0.f,0.f,1.f };
	_float2 m_vMinSize;
	_float2 m_vMaxSize;
	_float3 m_vDir{ 0.f,0.f,0.f };
	_float2 m_vScaleSpeed = _float2{ 0.f,0.f };
	int m_iParticleNum = 1;
	_bool m_bUVImage = false;
	_bool m_bUV = false;
	int m_iShaderPass = 0;
	_float m_fUVMaxIndexX = 0.f;
	_float m_fUVMaxIndexY = 0.f;
	_float m_fUVSpeed = 0.f;
	_bool m_bUVFix = false;;
	_bool m_bSetParticle = true;
	_float4 m_vColor1 = { 0.f,0.f,0.f,1.f };
	_float4 m_vColor2 = { 0.f,0.f,0.f,1.f };
	_float2 m_vRatio = { 1.f,1.f };
	_float2 m_vRatioSpeed = { 0.f,0.f };
	_float m_fBlurPower = 0.f;
	_bool m_bFixColor = false;

	list<const char*> m_ParticleTabName;
#pragma endregion SetParticle

#pragma region FixParticleInfo
private:
	void FixParticle();
	void Play();
	void ChangeToSelectTexture();

	_float4 m_vPos = { 0.f,0.f,0.f,1.f };
	_float2 m_vSize = { 0.f,0.f };
	_float m_fLifeTime = 0.f;
	_float m_fSpeed = 0.f;
	_bool m_bUVFix2 = false;


private:

	vector<PARTICLECREATE> m_ParticleCreateInfo;

	_bool m_bFixParticle = false;

	string m_ParticleName;
	string m_TextureName;
#pragma endregion FixParticleInfo

#pragma region ParticleInfo
private:
	void ShowParticleInfoWindow(_float fTimeDelta);
	void CreateParticle(char* ParticleName);
	void DeleteParticle();
	void SaveParticle(char* SaveName);
	void LoadParticle(char* LoadName);
	void ChangeName(char* ParticleName);
private:
	vector<const char*> m_ParticleList;
	int m_iSelectedParticle = 0;
	string m_NowParticles;
#pragma endregion ParticleInfo

#pragma region TextureInfo
private:
	void ShowTextureInfoWindow(_float fTimeDelta);
	TEXINFO* GetTexInfo(wstring TexName);
	HRESULT CreateResourceView(wstring TextureTag, const _tchar * pTextureFilePath, _uint iNumTextures);
	vector<ID3D11ShaderResourceView*> FindResourceView(wstring TextureName);
	HRESULT CheckViewName(wstring TextureName);
	void Delete_ResourceView();
	void LoadTexture();

private:
	bool m_bAnimation = false;
	int m_iFrameNum = 0;
	float m_fAniAcc = 0.f;
	float m_fAniSpeed = 1.f;

	map<wstring, vector<ID3D11ShaderResourceView*>> m_ResourceView;

	vector<const char*> m_TextureList;
	int m_iSelectedTexture = 0;

	list<TEXINFO*> m_TexInfo;

	string m_NowTextures;
#pragma endregion TextureInfo

#pragma region InfoWindow
public:
	void ShowInfoWindow(_float fTimeDelta);
private:
	list<const char*> m_TabName;
#pragma endregion InfoWindow

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;

public:
	virtual void Free() override;
};

END

