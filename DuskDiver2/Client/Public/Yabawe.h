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

class CYabawe : public CCreture
{
public:
	typedef struct tagYabaweDesc
	{
		_float3 vPos;
		_float4x4 mMatrix;

	}YABAWEDESC;

protected:
	CYabawe(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CYabawe(const CYabawe& rhs);
	virtual ~CYabawe() = default;


public:
	void Set_Root(_float3* vRoot, _bool bInv, _float fSpeed, _float fSens, _float fStopDis);


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
	void Tick_Game(_float fTimeDelta);
	void Tick_End(_float fTimeDelta);


protected:
	_bool m_bMove = false; // 움직이냐
	list<_float3> m_BasiRoots;
	_float m_fT = 0.f;

	_float3 m_vDestPos;
	_float3 m_vRealDestPos;
	_float m_fSpeed;
	_float m_fSens;
	_float m_fStopDis;

	_bool m_bCheckRealDestPos = false;
	_bool m_bRealYa = false;


	_bool m_bHit = false;

	_bool m_bHitTurn = false;
	_float m_fHitTimeAcc = 0.f;
	_float m_fHitTimeTurnAcc = 0.f;
	_bool m_bAttackCol = false;

	_float3 m_vInitPos;
	_float m_fShakeSpeed = 0.f;

	_bool m_bMoveSound = false;

protected:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;


protected:
	HRESULT Ready_Components();

protected:
	YABAWEDESC m_Desc;


public:
	static CYabawe* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCreture* Clone(void* pArg);
	virtual void Free() override;
};

END