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

	_float TargetDistance(_fvector vTargetPos); //Ÿ�ٰ� ���� �Ÿ� ��ȯ
	_bool Check_LimitDistance(_fvector vTargetPos, _float fDistance); //Ÿ�ٰ� ������ �Ÿ� �����ִ� �Լ�, �־��� distance���� �Ÿ��� �۾����� true ����
	_bool Check_TargetFront(_fvector vTargetPos); //Ÿ����ġ�� �� ��ġ ���� �����ִ� �Լ��ε� �ϴ� �տ� �ִ��� �ڿ��ִ��� �񱳸� ��

	//���� ȣ���ϳİ� �߿�
	void Push(_fvector vMyPos, _float fMyRad, _fvector vOtherPos, _float fOtherRad, _float fTimeDelta, CNavigation* pNavigation); //y����� ��
	void PushXZ(_fvector vMyPos, _float fMyRad, _fvector vOtherPos, _float fOtherRad, _float fTimeDelta, CNavigation * pNavigation); //y�� ���� x,z��
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
	/* �߷� */
	void Tick_Gravity(_float fTimeDelta, class CNavigation* pNavigation = nullptr, _float fGravity = 7.5f, _bool bCheckNano = false);
	/* ���� �մϴ�. */
	void Jump(_float fPower);
	/* �߷µ����͸� �����մϴ�.(�߷� ���ӵ� 0) */
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