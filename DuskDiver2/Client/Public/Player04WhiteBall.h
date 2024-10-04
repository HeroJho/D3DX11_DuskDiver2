#pragma once
#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CTransform;
END

BEGIN(Client)


class CPlayer04WhiteBall final : public CCreture
{
public:
	typedef struct tagCPlayer04WhiteBallInfo
	{
		_float4x4 vWorldMatrix;
		_float4 vColor;
		_float4 vColor2;
		_float2 vRatio;
		_float2 vRatioSpeed;
		_float fBlurPower;
		_float4 vLocalPos;	
		_float3 vScale;
	}WHITEBALLINFO;		 

private:
	CPlayer04WhiteBall(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	CPlayer04WhiteBall(const CPlayer04WhiteBall& rhs);
	virtual ~CPlayer04WhiteBall() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual void LateTick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Render_Debug() override;

	_float4 Get_BallPos();
	void Create_RageParticleEffect(_uint iIndex);
private:
	HRESULT Ready_Components();

private:
	_float2 m_vScaleTimeAcc = { 0.f,0.f };
	_float2 m_vUVFixAcc = { 0.f,0.f };
	_float m_fTimeLimitAcc = 0.f;
	//Phase1 처음등장
private:
	void Phase1(_float fTimeDelta);
	void Phase2(_float fTimeDelta);
	void Phase3(_float fTimeDelta);
	void Phase4(_float fTimeDelta);
	void Create_BlackBall(_float fTimeDelta);
	void Create_Tornado(_float fTimeDelta);
	void Create_LightEffect(_float fTimeDelta);
	void Phase3_Update(_float fTimeDelta);
	void BlackBallOn(_float fTimeDelta);
	void Create_BlackBallEffect(_uint iIndex, _float fTimeDelta);

private:	
	
	_float m_fP1ScaleUpAcc = 0.f;
	_float m_fP1ScaleUpSpeed = 15.f;
	_float m_fP1ScaleDownAcc = 0.f;
	_float m_fP1ScaleDownSpeed = 5.f;
	_uint m_iPhase = 1;
	_bool m_bP1ScaleUp = true;
	_bool m_bP1End = false;



	//Phase2
private:
	_float m_fP2ScaleFixAcc = 0.f;
	_bool m_bP2ScaleFix = false;
	_bool m_bP2ScaleUp = true;
	_float m_fP2ScaleUpAcc = 0.f;
	_float m_fP2ScaleUpSpeed = 0.f;
	_float m_fP2ScaleDownAcc = 0.f;
	_float m_fP2ScaleDownSpeed = 0.f;

	//Phase3
private:
	_bool m_bP3ScaleFix = false;
	_bool m_bP3ShakeEnd = false;
	_float m_fP3ScaleFixLimit = 0.6f;
	_float m_fP3ScaleFixAcc = 0.6f;
	_bool m_bP3ScaleUp = true;
	_float m_fP3ScaleUpAcc = 0.f;
	_float m_fP3ScaleUpSpeed = 0.f;
	_float m_fP3ScaleDownAcc = 0.f;
	_float m_fP3ScaleDownSpeed = 0.f;
	_bool m_bBalckBall = false;
	_float m_fP3ShakePower = 0.01f;
	_uint m_iSmoke2Count = 0;

	
	//Phase4
	_bool m_bLightEffect = false;
	_bool m_bTornado = false;
	_bool m_bTornadoCreate = false;
	
	

private:
	CShader*				m_pShaderCom = nullptr;
	CModel* m_pModelCom = nullptr;
	WHITEBALLINFO* m_pInfo = nullptr;
public:
	static CCreture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;

};
END