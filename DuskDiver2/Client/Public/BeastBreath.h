#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CBeastBreath : public CCreture
{
	
public:
	enum BREATHTYPE {BREATH_NORMAL, BREATH_RED, BREATH_WHITE, BREATH_END};
	typedef struct BreathInfo
	{
		_float4 vColor1 = { 1.f,0.f,0.f,1.f };
		_float4 vColor2 = { 0.f,0.f,0.f,1.f };
		_float2 vRatio = { 2.f,10.f };
		_float	fBlurPower = 10.f;
		_float2 vUVFixSpeed = { 1.f,3.f };
		_float2 vUVFixAcc = { 0.f,0.f };	
		_float4 vColor1_2 = { 0.f,0.f,0.f,1.f };
		_float4 vColor2_2 = { 0.f,0.f,0.f,1.f };
		_float2 vRatio_2 = { 1.f,3.f };
		_float	fBlurPower_2 = 2.f;
		_float2 vUVFixSpeed_2 = { -1.f,3.f };
		_float2 vUVFixAcc_2 = { 0.f,0.f };
		BREATHTYPE eBreathType = BREATH_NORMAL;
		_float3 vScale = { 1.f,1.f,1.f };
	}BREATHINFO;
private:
	CBeastBreath(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBeastBreath(const CBeastBreath& rhs);
	virtual ~CBeastBreath() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

	virtual void Render_Debug() override;
private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CModel*					m_pBreathCom = nullptr;
	CCreture*				m_pColCreture = nullptr;

public:
	HRESULT SetUp_State(_fmatrix StateMatrix); //입에있는 뼈 계속 따라다닐수있도록
	void	Render_OnOff(_bool	bOnOff) 
	{ 
		if (!bOnOff)
			m_bRatioFix = true;
		else
			m_bRender = bOnOff;
	}
	_bool	Get_RenderOnOff() { return m_bRender; }
	void	Change_ColRange(_float3 vSize, _float3 vCenter);
	void	Change_Size(_float vSize);
	void	Change_BreathSize();

	void	Attack_OnOff(_bool bOnOff) { m_bAttackCol = bOnOff; }
	void	PlayerHit_On();
	void Set_BreathInfo(BREATHINFO Info);
public:
	void	Set_ColStoneOnOff(_bool bOnOff) { m_bColStone = bOnOff; }
	_bool	Get_ColStone() { return m_bColStone; }

private:
	HRESULT Ready_Components();

private:
	CCreture::CREATEINFO m_CretureInfo;

	_float4			m_vTargetPos = {0.f, 0.f, 0.f, 1.f};
	_float			m_fSize = 0.f;

	_float			m_fBreathDistance = 0.f;
	_float			m_fColDelayTime = 0.f;
	_bool			m_bRender = false;
	_bool			m_bRatioFix = false;
	_bool			m_bHurtCol = false;
	_bool			m_bColStone = false;
	_bool			m_bAttackCol = false;
	_bool			m_bPlayerHit = true;
	_bool			m_bColDelay = false;
	
	


	//HS
	_float4 m_vColor1 = { 1.f,0.f,0.f,1.f };
	_float4 m_vColor2 = { 0.f,0.f,0.f,1.f };
	_float2 m_vRatio = { 2.f,10.f };
	_float	m_fBlurPower = 10.f;
	_float2 m_vUVFixSpeed = { 1.f,3.f };
	_float2 m_vUVFixAcc = { 0.f,0.f };
	_float3 m_vScale = { 1.f,1.f,1.f };
	_float4 m_vColor1_2 = { 0.f,0.f,0.f,1.f };
	_float4 m_vColor2_2 = { 0.f,0.f,0.f,1.f };
	_float2 m_vRatio_2 = { 1.f,3.f };
	_float	m_fBlurPower_2 = 2.f;
	_float2 m_vUVFixSpeed_2 = { -1.f,3.f };
	_float2 m_vUVFixAcc_2 = { 0.f,0.f };
	BREATHTYPE m_eBreathType = BREATH_NORMAL;


public:
	static CBeastBreath* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCreture* Clone(void* pArg);
	virtual void Free() override;
};

END