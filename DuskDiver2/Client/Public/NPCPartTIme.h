#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END

BEGIN(Client)

class CNPCPartTime : public CCreture
{
private:
	enum STATE {
		STATE_IDLE, STATE_TURNL, STATE_TURNR, STATE_WALK, STATE_END
	};
private:
	CNPCPartTime(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPCPartTime(const CNPCPartTime& rhs);
	virtual ~CNPCPartTime() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	_bool					m_bAction = false; // 플레이어의 위치에 따라 거리가 멀면 false로 하고 tick,latetick,render를 안한다.

	_float					m_fFirstRotationAngle = 0.f; // 시작할때 회전한 각도
	_bool					m_bTurnable = false; // 회전을 할수 있는가
	_bool					m_bTurned = false; // 왼쪽으로 회전을 했는가
	STATE					m_eCurState = STATE_IDLE; // 현재 상태
	STATE					m_eNextState = STATE_IDLE;

	//================================
	_float  NewX = 0.f;
	_float  NewZ = 0.f;
	//================================

private:
	HRESULT Ready_Components();
	_bool PlayerNearby();

	void Update_NPC(_float fTimeDelta);
	void Update_Ani_Idle(_float fTimeDelta);
	void Update_Ani_TurnL(_float fTimeDelta);
	void Update_Ani_TurnR(_float fTimeDelta);

	_bool On_LeftSide(); // 대화를 걸었을때 NPC알바의 왼쪽에 플레이어가 있었는가

	//================================
	void Move(_float fTimeDelta);
	//================================

public:
	static CNPCPartTime* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END