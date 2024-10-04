#pragma once

#include "Component.h"
#include "DebugDraw.h"

BEGIN(Engine)

class ENGINE_DLL CRigid final : public CComponent
{
public:
	typedef struct tagRigidDesc
	{
		_bool			bBox = true;

		_float3			vStartPos;

		_float3			vPos;
		_float3			vRotation;
		_float3			vExtense;

		_float			fWeight;			// ¹«°Ô
		_float			fStaticFriction;
		_float			fDynamicFriction;
		_float			fRestitution;
		_float			fAngleDump;
		_float			fMaxVel = 0.f;

	}RIGIDEDESC;


private:
	CRigid(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRigid(const CRigid& rhs);
	virtual ~CRigid() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);

public:
	void Set_GreenColor(_bool bGreen) { m_bGreenColor = bGreen; }

public:
	void Update_Transform(class CTransform* pTran);
	void Set_Sleep(_bool bIsSleep);
	void Add_Force(_float3 vForce);
	void Add_Torque(_float3 vTorque);

	void Edit_PosRot();

private:
	PxRigidDynamic* m_pXBody = nullptr;
	RIGIDEDESC		m_Desc;
	_bool m_bFirst = false;

	_bool m_bGreenColor = false;

private:
	BoundingOrientedBox*	m_pOBB = nullptr;
	BoundingOrientedBox*	m_pOriginal_OBB = nullptr;



#ifdef _DEBUG
public:
	virtual HRESULT Render() override;

	RIGIDEDESC* Get_Desc() { return &m_Desc; }
	PxRigidDynamic* Get_PxBody() { return m_pXBody; }
	void Edit_Px();
	

#endif // _DEBUG

#ifdef _DEBUG
protected:
	PrimitiveBatch<VertexPositionColor>*		m_pBatch = nullptr;
	BasicEffect*								m_pEffect = nullptr;
	ID3D11InputLayout*							m_pInputLayout = nullptr;
#endif



public:
	static CRigid* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END