#pragma once

#include "Component.h"
#include "DebugDraw.h"

BEGIN(Engine)

class ENGINE_DLL CRigid_Sphere final : public CComponent
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
	}RIGIDEDESC;


private:
	CRigid_Sphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRigid_Sphere(const CRigid_Sphere& rhs);
	virtual ~CRigid_Sphere() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);


public:
	void Update_Transform(class CTransform* pTran);

private:
	PxRigidDynamic* m_pXBody = nullptr;
	RIGIDEDESC		m_Desc;

private:
	BoundingSphere*	m_pSphere = nullptr;
	BoundingSphere*	m_pOriginal_Sphere = nullptr;



#ifdef _DEBUG
public:
	virtual HRESULT Render() override;

	RIGIDEDESC* Get_Desc() { return &m_Desc; }
	PxRigidDynamic* Get_Rigid() { return m_pXBody; }
	void Edit_Px();
	void Reset_Pos();

#endif // _DEBUG

#ifdef _DEBUG
protected:
	PrimitiveBatch<VertexPositionColor>*		m_pBatch = nullptr;
	BasicEffect*								m_pEffect = nullptr;
	ID3D11InputLayout*							m_pInputLayout = nullptr;
#endif



public:
	static CRigid_Sphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END