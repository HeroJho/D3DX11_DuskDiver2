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


class CDGEndCuma final : public CCreture
{
public:
	enum FACE { FACE_IDLE, FACE_SMILE, FACE_HURT, FACE_END };


private:
	CDGEndCuma(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDGEndCuma(const CDGEndCuma& rhs);
	virtual ~CDGEndCuma() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;

public:
	void Shake();
	void Tick_Shake(_float fTimeDelta);

private://컴포넌트, 파츠관련 함수들
	HRESULT Ready_Components();


private:
	FACE m_eFace = FACE_HURT;
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


	_float m_fCraneTimeAcc = 0.f;
	_bool m_bCrane = false;

	_bool m_bEndSound = false;
	_bool m_bJumpEndSound = false;
private:
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CModel*					m_pModelCom1 = nullptr;
	CModel*					m_pModelCom2 = nullptr;


public:
	static CDGEndCuma* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END