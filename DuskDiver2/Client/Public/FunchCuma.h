#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CNavigation;
class CHierarchyNode;
class CLight;
END

BEGIN(Client)


class CFunchCuma final : public CCreture
{
public:
	enum FACE { FACE_IDLE, FACE_SMILE, FACE_HURT, FACE_END };
	enum STATE { STATE_CUT, STATE_IDLE, STATE_ENDCUT, STATE_END };

private:
	CFunchCuma(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFunchCuma(const CFunchCuma& rhs);
	virtual ~CFunchCuma() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	virtual void On_Hit(CCreture* pOther) override;




private:
	void Tick_Cut(_float fTimeDelta);
	void Tick_Idle(_float fTimeDelta);
	void Tick_EndCut(_float fTimeDelta);


	void Tick_Shake(_float fTimeDelta);


private://컴포넌트, 파츠관련 함수들
	HRESULT Ready_Components();



public:
	void Set_State(STATE eState);

	void Shake();

	void Start();

	void Check_Score();

	_float Get_Score() { return m_fDamageScore; }

private:
	FACE m_eFace = FACE_IDLE;
	STATE m_eState = STATE_END;

	_float m_fTImeAcc = 0.f;
	_bool m_bJump = false;
	_bool m_bDonw = false;
	_bool m_bCutIdle = false;
	_float m_fTimeCutIdleAcc = 0.f;
	_bool m_bUpLookat = false;

	_bool m_bCutEnd = false;
	_float m_fTimeCutCut = 0.f;

	_bool m_bEndSound = false;
	_bool m_bGoBalling = false;

	// For. Shake
	_bool m_bShake = false;
	_float3 m_vInitPos;
	_bool m_bShakeTurn = false;
	_float m_fShakeTimeAcc = 0.f;
	_float m_fShakeEndTimeAcc = 0.f;


	// For. Game
	_bool m_bStart = false;
	_float m_fSussDamageScore = 0.f;
	_float m_fMScore = 100.f;
	_bool m_bCutShake = false;
	_float m_fCutShakeTimeAcc = 0.f;

	// UI 표시해야할 것들
	_float m_fGameTimeAcc = 0.f; // 제한 시간
	_float m_fDamageScore = 0.f;
	

	_bool m_bBoom = false;
	_float m_fBoomTimeAcc = 0.f;


	CLight*					m_pLight = nullptr;


private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CModel*					m_pModelCom1 = nullptr;
	CModel*					m_pModelCom2 = nullptr;


public:
	static CFunchCuma* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END