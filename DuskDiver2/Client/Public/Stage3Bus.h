#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CModel;
END

BEGIN(Client)

class CStage3Bus : public CCreture
{
public:
	typedef struct tagBusDesc {
		_uint iLaneIndex; // ��������� Ż���ΰ�? 0,5 : ���� 1~4 : �Ϲ� ���� ,,,,, 0~2 : ���±� 3~5 : ���±�
		_float fDestinationX; // �߰��� ��ǥ�� ����ΰ�
		_float fResetX; // �ʱ�ȭ ��ǥ
	}BUSDESC;
private:
	CStage3Bus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CStage3Bus(const CStage3Bus& rhs);
	virtual ~CStage3Bus() = default;


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
	CTexture*				m_pDissolveTexture = nullptr;

	BUSDESC					m_BusDesc;
	_float					m_fStartZ = 0.f;
	_float					m_fCurPosX = 0.f;
	_float					m_fCurSpeed = 0.f;

	_float					m_fSlideLength = 100.f;
	_float					m_fMaxSpeed = 20.f;
	_float					m_fSlideTime = 5.f;
	_float					m_fAcceleration = 0.f; // ���ӵ�
	_bool					m_bAttackCol = true;
	_float					m_fAttackColAcc = 0.f;

	_float					m_fWaitingTime = 0.f;
	_float					m_fDecliningTime = 0.f; // ���Ӷ� ������ �ð�
	_bool					m_bStopped = false; // ������ ������ �ִ°�
	_bool					m_bDecline = false; // ���ӽ����� �ߴ°�

	_float m_fDissolveAcc = 0.f;
	_bool m_bRender = false;

private:
	HRESULT Ready_Components();
	void Set_InitialState();

	void Update_Cars(_float fTimeDelta);

	void MoveBack(_float fTimeDelta); // �ڷ� ���� �� 
	void MoveFront(_float fTimeDelta); // ������ ���� ��

public:
	static CStage3Bus* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCreture* Clone(void* pArg);
	virtual void Free() override;
};

END