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

	_bool					m_bAction = false; // �÷��̾��� ��ġ�� ���� �Ÿ��� �ָ� false�� �ϰ� tick,latetick,render�� ���Ѵ�.

	_float					m_fFirstRotationAngle = 0.f; // �����Ҷ� ȸ���� ����
	_bool					m_bTurnable = false; // ȸ���� �Ҽ� �ִ°�
	_bool					m_bTurned = false; // �������� ȸ���� �ߴ°�
	STATE					m_eCurState = STATE_IDLE; // ���� ����
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

	_bool On_LeftSide(); // ��ȭ�� �ɾ����� NPC�˹��� ���ʿ� �÷��̾ �־��°�

	//================================
	void Move(_float fTimeDelta);
	//================================

public:
	static CNPCPartTime* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END