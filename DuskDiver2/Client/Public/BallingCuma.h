#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CNavigation;
class CHierarchyNode;
END

BEGIN(Client)


class CBallingCuma final : public CCreture
{
public:
	enum FACE { FACE_IDLE, FACE_SMILE, FACE_HURT, FACE_END };
	enum STATE { STATE_IDLE, STATE_RUN, STATE_HAKI, STATE_HAKI_RUN, STATE_END };

private:
	CBallingCuma(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBallingCuma(const CBallingCuma& rhs);
	virtual ~CBallingCuma() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

public:
	void Tick_Idle(_float fTimeDelta);
	void Tick_Run(_float fTimeDelta);
	void Tick_Haki(_float fTimeDelta);
	void Tick_Haki_Run(_float fTimeDelta);

	void Shake();
	void Tick_Shake(_float fTimeDelta);

	void Hit();
	void End_Game();

	void Set_HakiEnd() { m_bHakiEnd = true; }

private://컴포넌트, 파츠관련 함수들
	HRESULT Ready_Components();


private:
	STATE m_eState = STATE_IDLE;
	FACE m_eFace = FACE_IDLE;
	_float m_fTimeAcc = 0.f;
	_bool m_bJump = false;
	_bool m_bDown = false;
	_bool m_bRealShake = true;
	_bool m_bCutEnd = false;

	_float m_fEndTimeAcc = 0.f;

	_float m_fTimeShakeAcc = 0.f;

	// For. Shake
	_bool m_bShake = false;
	_float3 m_vInitPos;
	_bool m_bShakeTurn = false;
	_float m_fShakeTimeAcc = 0.f;
	_float m_fShakeEndTimeAcc = 0.f;

	_bool m_bHoldCam = false;

	_bool	m_bOneControl = true;
	// For. Haki
	_float m_fTurnAngle = 0.f;
	_float m_fHakiTimeAcc = 0.f;

	_bool m_bOneSpawn = false;

	_bool m_bHakiEnd = false;
	_float m_fHakiEndTimeAcc = 0.f;

	_bool m_bBoom = false;
	_float m_fBoomTimeAcc = 0.f;
	_bool m_bEndSound = false;

private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CModel*					m_pModelCom1 = nullptr;
	CModel*					m_pModelCom2 = nullptr;


public:
	static CBallingCuma* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END