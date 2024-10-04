#pragma once

#include "Client_Defines.h"
#include "Yabawe.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CTexture;
class CRenderer;
class CTransform;
END

BEGIN(Client)

class CRealYabawe : public CYabawe
{
public:
	enum FACE { FACE_IDLE, FACE_SMILE, FACE_HURT, FACE_END };

private:
	CRealYabawe(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRealYabawe(const CRealYabawe& rhs);
	virtual ~CRealYabawe() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	virtual void On_Hit(CCreture* pOther) override;

	virtual void Render_Debug() override;


public:
	void Start_CutScene();
	void Shake();

public:
	void Game_Tick(_float fTimeDelta);
	void End_Tick(_float fTimeDelta);


	void Tick_Shake(_float fTimeDelta);

protected:
	HRESULT Ready_Components();


private:
	FACE m_eFace = FACE_IDLE;

	_bool m_bTurnChange = false;
	_float m_fAngle = 0.f;
	_bool m_bTempOne = false;
	_bool m_bJump = false;
	_bool m_bInvJump = false;

	_float3 m_vInitPos;
	_float m_fSpinTimeAcc = 0.f;
	_bool m_bStartShakeJump = false;
	_bool m_bStartShake = false;
	_float m_fTimeAccTemp = 0.f;
	_bool m_bAttackCol = false;

	_bool m_bNext = false;
	_float m_fNextTimeAcc = 0.f;
	_bool m_bOnce_1 = false;

	_bool m_bEndSound = false;
	_bool m_bMoveSound = false;
	_bool m_bStartEndSound = false;

	// For. Shake
	_bool m_bShake = false;
	_float3 m_vInitShakePos;
	_bool m_bShakeTurn = false;
	_float m_fShakeTimeAcc = 0.f;
	_float m_fShakeEndTimeAcc = 0.f;

	// For. EndCut
	_float m_fTimeAcc = 0.f;
	_bool m_bCutShake = false;
	_float m_fCutShakeTimeAcc = 0.f;
	_bool m_bEndJump = false;
	_bool m_bDonw = false;
	_bool m_bCutEnd = false;
	_float m_fTimeCutCut = 0.f;


	_bool m_bBoom = false;
	_float m_fBoomTimeAcc = 0.f;

private:
	CModel*					m_pModelCom1 = nullptr;
	CModel*					m_pModelCom2 = nullptr;


public:
	static CRealYabawe* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCreture* Clone(void* pArg);
	virtual void Free() override;
};

END