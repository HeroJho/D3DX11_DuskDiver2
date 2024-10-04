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
		_uint iLaneIndex; // 몇번레인을 탈것인가? 0,5 : 버스 1~4 : 일반 차도 ,,,,, 0~2 : 가는길 3~5 : 오는길
		_float fDestinationX; // 중간역 좌표는 어디인가
		_float fResetX; // 초기화 좌표
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
	_float					m_fAcceleration = 0.f; // 가속도
	_bool					m_bAttackCol = true;
	_float					m_fAttackColAcc = 0.f;

	_float					m_fWaitingTime = 0.f;
	_float					m_fDecliningTime = 0.f; // 감속때 누적된 시간
	_bool					m_bStopped = false; // 정차를 한적이 있는가
	_bool					m_bDecline = false; // 감속시작을 했는가

	_float m_fDissolveAcc = 0.f;
	_bool m_bRender = false;

private:
	HRESULT Ready_Components();
	void Set_InitialState();

	void Update_Cars(_float fTimeDelta);

	void MoveBack(_float fTimeDelta); // 뒤로 가는 차 
	void MoveFront(_float fTimeDelta); // 앞으로 가는 차

public:
	static CStage3Bus* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CCreture* Clone(void* pArg);
	virtual void Free() override;
};

END