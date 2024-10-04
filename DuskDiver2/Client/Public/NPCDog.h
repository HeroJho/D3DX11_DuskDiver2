#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
class CNavigation;
END

BEGIN(Client)

class CNPCDog : public CCreture
{
private:
	enum DOG_STATE {TYPE_BARKING, TYPE_HAPPY, TYPE_WALK, TYPE_END};
private:
	CNPCDog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNPCDog(const CNPCDog& rhs);
	virtual ~CNPCDog() = default;


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
	CNavigation*			m_pNavigationCom = nullptr;

	class CDogbox*			m_Dogbox = nullptr;

	_bool					m_bAction = true; // Tick, LateTick, Render�� �Ѱ�

	_bool					m_bRenderState = true; // ��ġ ���� �̵��� ��� ������ ���� �뵵
	_bool					m_bFindCrack = false; // �տ��� �������� �ִ°�
	_bool					m_bChase = false; // �÷��̾ ���󰡳�

	_float					m_fSpeed = 3.f;
	_uint					m_iLastAnimIndex = 0;
	_float					m_fCrackLength = 0.f;// �������� �տ������� �Ÿ��� ��� �뵵

	_bool					m_bOn = false;
	_bool					m_bFirstMeet = false;
	_bool					m_bWhimperOn = false;
	_bool					m_bBark = false;
	_bool					m_bIdle = false;
	_float					m_fBarkAcc = 0.f;
	_float					m_fIdleAcc = 0.f;
	_float					m_fBeNormalAcc = 0.f;
	_float4					m_fPlayerPos = { 0.f, 0.f, 0.f, 0.f };

	//===========================
	_bool					m_bBurstUIOn = false;
	_bool					m_bMiddleBoss = false;
	//===========================
private:
	HRESULT Ready_Components();
	_bool Check_CrackNearby();
	void Update_Dog(_float fTimeDelta); // ������Ʈ�� å������ �Լ�
	void Chase_Player(_float fTimeDelta); // �÷��̾ ���󰡴� �Լ�
	void Barking(_float fTimeDelta); // �տ��� ���� ¢�� �Լ�

	void Fix_State(); // Local������ Look������ �̻��Ͽ� Look�� �������ִ� �Լ�

public:
	static CNPCDog* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END