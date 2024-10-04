#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CRenderer;
class CNavigation;
class CTransform;
END

BEGIN(Client)

class CBeastMarvelPush : public CCreture
{
public:
	typedef struct tagPushInfo
	{
		int iIndex;
		int iNaviNum;
		_float fBlurPower;
		_float4 vPosition;
		_float4 vColor;
		_float4 vColor2;
		_float2 vRatio;
		_float2 vRatioSpeed;
		_float3 vScale;
		_float3 vScaleSpeed;

	}PUSHINFO;

private:
	CBeastMarvelPush(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBeastMarvelPush(const CBeastMarvelPush& rhs);
	virtual ~CBeastMarvelPush() = default;

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
	CNavigation*			m_pNavigationCom = nullptr;

public:
	void	Reset_Position();
	void	Set_On();
	void	Set_Off();
	void	Set_NewPos(_float4 vPos) { m_pInfo->vPosition = vPos; }
	_bool	Get_RenderOnOff() { return m_bRender; }

private:
	void Create_Particle();
	void Create_OnParticle();
	HRESULT Ready_Components();
	HRESULT Create_Navigation(_uint iNaviNum);
	void	Change_ColRange(_float3 vSize, _float3 vCenter);
	_float m_fParticleAcc = 0.f;

private:
	_bool m_bStartFix = false;
	PUSHINFO* m_pInfo = nullptr;
	_bool	m_bRender = false;
	_bool	m_bColOff = false;
	_bool	m_bEndOff = false;

public:
	static CBeastMarvelPush* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCreture* Clone(void* pArg);
	virtual void Free() override;
};

END