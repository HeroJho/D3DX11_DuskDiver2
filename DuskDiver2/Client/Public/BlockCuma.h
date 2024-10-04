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
class CLight;
END

BEGIN(Client)

class CBlockCuma : public CCreture
{
public:
	typedef struct BlockCumaDesc
	{
		_float3 vPos;
		_float3 vRot;
	}BLOCKCMADESC;

public:
	enum FACE { FACE_IDLE, FACE_SMILE, FACE_HURT, FACE_END };
	enum STATE { STATE_CUT, STATE_PUSE, STATE_IDLE, STATE_HIDE, STATE_ENDCUT, STATE_END};

private:
	CBlockCuma(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBlockCuma(const CBlockCuma& rhs);
	virtual ~CBlockCuma() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();


	virtual void Render_Debug() override;



public:
	void Set_State(STATE eState);
	void Set_Puse() { m_eState = STATE_PUSE; }
	void Set_End() { m_eState = STATE_ENDCUT; }

	void Shake();

private:
	void Tick_Cut(_float fTimeDelta);
	void Tick_Puse(_float fTimeDelta);
	void Tick_Idle(_float fTimeDelta);
	void Tick_Hide(_float fTimeDelta);
	void Tick_EndCut(_float fTimeDelta);

	void Tick_Shake(_float fTimeDelta);

	void Peek_A_Boo();

private:
	HRESULT Ready_Components();
	

private:
	FACE m_eFace = FACE_IDLE;
	STATE m_eState = STATE_END;
	BLOCKCMADESC m_Desc;

	_bool m_bHoverTurn = false;

	_float3 m_vHomePos;
	_float m_fSpeed = 0.f;

	_float m_fGoHomeTimeAcc = 0.f;

	_float m_fGoHideTimeAcc = 0.f;
	_bool m_bOutHide = false;
	_bool m_bUpHide = false;
	_bool m_bDownHide = false;
	_bool m_bStayHide = false;

	_float m_fHideTime = 10.f;
	
	_bool m_bLeftEndHide = false;
	_float3 m_vLeftPos;

	_bool m_bTemm = false;
	

	// For. Shake
	_bool m_bShake = false;
	_float3 m_vInitPos;
	_bool m_bShakeTurn = false;
	_float m_fShakeTimeAcc = 0.f;
	_float m_fShakeEndTimeAcc = 0.f;


	// For. EndCut
	_float m_fTimeAcc = 0.f;
	_bool m_bCutShake = false;
	_float m_fCutShakeTimeAcc = 0.f;
	_bool m_bJump = false;
	_bool m_bDonw = false;
	_bool m_bCutEnd = false;
	_float m_fTimeCutCut = 0.f;


	_bool m_bBoom = false;
	_float m_fBoomTimeAcc = 0.f;


	CLight*					m_pLight = nullptr;


private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CModel*					m_pModelCom1 = nullptr;
	CModel*					m_pModelCom2 = nullptr;


public:
	static CBlockCuma* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCreture* Clone(void* pArg);
	virtual void Free() override;
};

END