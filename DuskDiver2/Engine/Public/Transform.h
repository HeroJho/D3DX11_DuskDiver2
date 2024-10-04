#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };
	typedef struct tagTransformDesc
	{
		_float			fSpeedPerSec;
		_float			fRotationPerSec;
	}TRANSFORMDESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	virtual ~CTransform() = default;

public:
	_vector Get_State(STATE eState) {
		return XMLoadFloat4((_float4*)&m_WorldMatrix.m[eState][0]);
	}

	void Set_State(STATE eState, _fvector vState);

	_matrix Get_WorldMatrix() const {
		return XMLoadFloat4x4(&m_WorldMatrix);
	}
	void Set_WorldMatrix(_float4x4 mWorld) {
		m_WorldMatrix = mWorld;
	}

	_float4x4 Get_WorldFloat4x4() const {
		return m_WorldMatrix;
	}

	_float4x4 Get_WorldFloat4x4_TP() const {
		_float4x4	WorldMatrix;
		XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(Get_WorldMatrix()));
		return WorldMatrix;
	}

	_matrix Get_WorldMatrixInverse() const {
		return XMMatrixInverse(nullptr, Get_WorldMatrix());
	}

	void Set_Look(_fvector vLook);

	_float3 Get_Rotation_Debug() { return m_vRotation; }
	void Set_Rotation_Debug(_float3 vRot) { m_vRotation = vRot; }
	_float3 Get_Scale_Debug() { return m_vScale; }
	void Set_Scale_Debug(_float3 vScale) { m_vScale = vScale; }

	_matrix Get_WorldMatrix_NoScale();

public:
	virtual HRESULT Initialize_Prototype(); 
	virtual HRESULT Initialize(void* pArg);

public:
	void Go_Straight(_float fTimeDelta);
	void Go_Backward(_float fTimeDelta);
	void Go_Left(_float fTimeDelta);
	void Go_Right(_float fTimeDelta);

	void RotationThree(_float3 vAxis, _float fRadian, _float3 vAxis2, _float fRadian2, _float3 vAxis3, _float fRadian3);
	void Go_Dir(_fvector vDir, _float fSpeed, _float fTimeDelta);
	void Go_Dir(_fvector vDir, _float fSpeed, _float fTimeDelta, class CNavigation* pNavigation);
	void Set_Scale(_fvector vScaleInfo);
	void Set_ScaleX(_fvector vScaleInfo);
	void Set_ScaleY(_fvector vScaleInfo);
	void Set_ScaleZ(_fvector vScaleInfo);
	_float3 Get_Scale();

	void TurnAngle(_fvector vAxis, _float _fAngle);
	void Turn(_fvector vAxis, _float fTimeDelta);
	void Turn(_fvector vAxis, _float fSpeed, _float fTimeDelta);
	void Turn(_fvector StartLook, _fvector EndLook, _float _fRatio);

	void Rotation(_fvector vAxis, _float fRadian);

	_float Get_TurnSpeed() { return m_TransformDesc.fRotationPerSec; }
	void Set_TurnSpeed(_float fSpeed) { m_TransformDesc.fRotationPerSec = fSpeed; }

	void LookAt(_fvector vAt, _bool bInv = false);
	void LookAt_ForLandObject(_fvector vAt, _bool bInv = false);
	_bool Move(_fvector vTargetPos, _float fSpeed, _float fTimeDelta, _float fLimitDistance = 0.1f, _bool bLimitPass = false);

	_float TargetDistance(_fvector vTargetPos); //타겟과 나의 거리 반환
	_bool Check_LimitDistance(_fvector vTargetPos, _float fDistance); //타겟과 나와의 거리 비교해주는 함수, 넣어준 distance보다 거리가 작아질때 true 리턴
	_bool Check_TargetFront(_fvector vTargetPos); //타겟위치와 내 위치 각도 구해주는 함수인데 일단 앞에 있는지 뒤에있는지 비교만 됨

	//누가 호출하냐가 중요
	void Push(_fvector vMyPos, _float fMyRad, _fvector vOtherPos, _float fOtherRad, _float fTimeDelta, CNavigation* pNavigation); //y축까지 밈
	void PushXZ(_fvector vMyPos, _float fMyRad, _fvector vOtherPos, _float fOtherRad, _float fTimeDelta, CNavigation * pNavigation); //y축 제외 x,z만
	void PushMe(_fvector vMyPos, _float fMyRad, _fvector vOtherPos, _float fOtherRad, _float fTimeDelta, CNavigation * pNavigation); //
	void PushOther(_fvector vMyPos, _float fMyRad, _fvector vOtherPos, _float fOtherRad, _float fTimeDelta, CNavigation * pNavigation);
	void PushMeXZ(_fvector vMyPos, _float fMyRad, _fvector vOtherPos, _float fOtherRad, _float fTimeDelta, CNavigation * pNavigation);
	void PushOtherXZ(_fvector vMyPos, _float fMyRad, _fvector vOtherPos, _float fOtherRad, _float fTimeDelta, CNavigation * pNavigation);



private:
	_float4x4				m_WorldMatrix;
	TRANSFORMDESC			m_TransformDesc;
	_float3					m_vRotation;
	_float3					m_vScale;


	// ======================
	//		For. Gravity
	// ======================
public:
	/* 중력 */
	void Tick_Gravity(_float fTimeDelta, class CNavigation* pNavigation = nullptr, _float fGravity = 7.5f, _bool bCheckNano = false);
	/* 점프 합니다. */
	void Jump(_float fPower);
	/* 중력데이터를 리셋합니다.(중력 가속도 0) */
	void ResetGravity();
	void ResetAndJump(_float fPower);

	_bool Get_Air() { return m_bAir; }

private:
	_float					m_fGravity = 10.f;
	_float					m_fGravityAcc = 0.f;
	_float					m_fVelocity = 0.f;
	_bool					m_bAir = false;

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END