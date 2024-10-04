#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END

BEGIN(Client)
class CBeastStonePart;

class CBeastStoneBody : public CCreture
{
private:
	CBeastStoneBody(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBeastStoneBody(const CBeastStoneBody& rhs);
	virtual ~CBeastStoneBody() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	virtual void On_Hit(CCreture* pOther) override;

public:
	void	Render_OnOff(_bool	bOnOff) { m_bRender = bOnOff; }
	_bool	Get_RenderOnOff() { return m_bRender; }
	void	Reset_InitState();
	void	Create_Effect();
	void	Fallen_Start() { m_bFallen = true; }
	_bool	Get_Fallen() { return m_bFallen; }

	void	Set_InitState();
	void	Shield_OnOff(_bool bOnOff) { m_bShieldCol = bOnOff; }
	void	BodyCol_OnOff(_bool bOnOff) { m_bBodyCol = bOnOff; }
	void	Set_RandomIndex(_uint iIndex) { m_iShieldBody = iIndex; }

	void	On_HeatingNotShield(CCreture* pOther);
	void	On_Heating(CCreture* pOther);

private:
	HRESULT Ready_ModelCom();
	void	Check_Rim(_float fTimeDelta);
private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	//vector<class CBeastStonePart*>	m_PartALayer; 


private:
	//HS
	_bool m_bParticleOn = false;
	_float m_fParticleAcc = 0.f;


	CREATEINFO				m_pCreateInfo;

	_bool					m_bAttackCol = true; //플레이어 공격용
	_bool					m_bShieldCol = false; //브레스와 충돌용
	_bool					m_bBodyCol = true; //플레이어 어택과 충돌용
	_bool					m_bHurtCol = false;
	_bool					m_bOneCol = true;
	_bool					m_bOneShake = true;

	_uint					m_iShieldBody = 0;
	//rim
	_uint					m_iShaderPath = 0;
	_float					m_fRimPower = 0.f;
	_float4 m_fRimColor =	{ 0.f,0.f,0.f,1.f };

	_bool					m_bRim = false;
	_bool					m_bAttackRim = false;

	_float					m_fRimAcc = 0.f;

	_float					m_fLimitY = 0.f;
	_float					m_fSpeed = 0.f;

	_bool					m_bFallen = false;
	_bool					m_bOneFallen = true;

	_bool					m_bDeadStart = false;
	_float					m_fDeadAcc = 0.f;


	_float					m_fHeatingAcc = 0.f;
	_float					m_fHeatRim = 15.f;

	_bool					m_bOnceRim = false;
	//==============================================for Debug & IMGUI
	_bool					m_bRender = false;

	_int					m_iSelectIndex = 0; // 현재 타입의 몇번째 블럭을 edit할것인가
	_float4					m_SelectedPosition = { 0.f,0.f,0.f,1.f };
	_float					m_SelectedAngle = 0.f;
	_float					m_SelectedSize = 1.f;


private:
	HRESULT Ready_Components();
	HRESULT Ready_Collider();

	void Initialize_Parts();

public:
	static CBeastStoneBody* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END