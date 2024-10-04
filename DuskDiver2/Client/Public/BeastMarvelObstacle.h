#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CTexture;
class CRenderer;
class CTransform;
END

BEGIN(Client)
class CBeastAura;

class CBeastMarvelObstacle : public CCreture
{
private:
	CBeastMarvelObstacle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBeastMarvelObstacle(const CBeastMarvelObstacle& rhs);
	virtual ~CBeastMarvelObstacle() = default;

private:

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	virtual void On_Hit(CCreture* pOther) override;
	virtual void Render_Debug() override;


private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

public:
	void	Appear_On() { m_bAppear = true; }
	void	Render_OnOff(_bool	bOnOff) { m_bRender = bOnOff; }
	_bool	Get_RenderOnOff() { return m_bRender; }
	void	Reset_State(_float4 vPos);
private:
	HRESULT Ready_Components();
	void	SetUp_AppearLimitTime();
	void	Change_ObstacleSize(_float fTimeDelta);
private:
	CCreture::CREATEINFO m_CretureInfo;

	_float			m_fObstacleScale = 0.1f;
	_float			m_fPlusScale = 0.f;
	_float			m_fTotalScale = 0.f;
	_float			m_fLimitScale = 0.f;
	_float			m_fDelayAcc = 0.f;
	_float			m_fBlurPower = 0.f;
	_float			m_fAppearAcc = 0.f;
	_float			m_fDelayAppear = 0.f;


	_bool			m_bAppear = false;
	_bool			m_bRender = false;
	_bool			m_bChangeScale = false;
	_bool			m_bSizeUp = false;
	_bool			m_bOneHit = true;
	_bool			m_bMarbleColEffect = false;
	_bool			m_bMarbleEnd = false;

	_float4 m_vColor = { 0.f,0.f,0.f,1.f };
	_float4 m_vColor2 = { 0.f,0.f,0.f,1.f };
	_float2 m_vUVFixAcc = { 0.f,0.f };
	_float2 m_vRatio = { 1.f,1.f };

	vector<CGameObject*> m_Aura;

public:
	static CBeastMarvelObstacle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCreture* Clone(void* pArg);
	virtual void Free() override;
};

END