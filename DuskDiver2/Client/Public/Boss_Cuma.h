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


class CBoss_Cuma final : public CCreture
{
public:
	enum FACE { FACE_IDLE, FACE_SMILE, FACE_HURT, FACE_END };
	enum STATE { STATE_IDLE, STATE_ONING, STATE_ONIDLE, STATE_DIE, STATE_END };

private:
	CBoss_Cuma(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss_Cuma(const CBoss_Cuma& rhs);
	virtual ~CBoss_Cuma() = default;

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
	void Set_State(STATE eState);
	void Start_On();
	void Shake();
	void Set_Boss(class CMechanicalOctopus* pBoss) { m_pBoss = pBoss; };



private:
	void Tick_Idle(_float fTimeDelta);
	void Tick_Oning(_float fTimeDelta);
	void Tick_OnIdle(_float fTimeDelta);
	void Tick_Die(_float fTimeDelta);

	void Tick_Shake(_float fTimeDelta);


private://컴포넌트, 파츠관련 함수들
	HRESULT Ready_Components();




private:
	FACE m_eFace = FACE_IDLE;
	STATE m_eState = STATE_END;
	class CMechanicalOctopus* m_pBoss = nullptr;

	_float4 m_vLocalPos;

	_float m_fTimeAcc = 0.f;
	_float m_fRealShakeTimeAcc = 0.f;

	_bool m_bJump = false;

	_bool m_bRealShake = true;

	// For. Shake
	_bool m_bShake = false;
	_float3 m_vInitPos;
	_bool m_bShakeTurn = false;
	_float m_fShakeTimeAcc = 0.f;
	_float m_fShakeEndTimeAcc = 0.f;

	_bool m_bStart = false;
	_bool m_bRender = true;

	// For. Die
	_float m_fCutSceneAcc = 0.f;
	_bool		m_bBoome = false;
	_float			m_fBoomeTimeAcc = 0.f;
	_bool		m_bCry = false;
	_float		m_fCryTimeAcc = 0.f;
	_float m_fCry2TimeAcc = 0.f;

private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CModel*					m_pModelCom1 = nullptr;
	CModel*					m_pModelCom2 = nullptr;


	
public:
	static CBoss_Cuma* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END