#pragma once

#include "Collider.h"

BEGIN(Engine)

class ENGINE_DLL COBB final : public CCollider
{
public:
	typedef struct tagOBBDesc
	{	
		_float3		vCenter;
		_float3		vCenterAxis[3];
		_float3		vAlignAxis[3];
	}OBBDESC;
protected:
	COBB(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COBB(const COBB& rhs);
	virtual ~COBB() = default;

public:
	BoundingOrientedBox Get_Collider() {
		return *m_pOBB;
	}

public:
	virtual HRESULT Initialize_Prototype(CCollider::TYPE eColliderType);
	virtual HRESULT Initialize(void* pArg);
	virtual void Update(_fmatrix TransformMatrix);
	virtual _bool Collision(CCollider* pTargetCollider);
	virtual void Set_Size(_float3 vSize)override;
	virtual void Set_Center(_float3 vCenter)override;
	virtual void Set_Rot(_float3 vRot)override;

	void Set_SizeOBB(_float3 vSize);

#ifdef _DEBUG
public:
	virtual HRESULT Render();
#endif // _DEBUG

private:
	BoundingOrientedBox*	m_pOBB = nullptr;
	BoundingOrientedBox*	m_pOriginal_OBB = nullptr;



public:
	static COBB* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CCollider::TYPE eColliderType);
	virtual CComponent* Clone(void* pArg);
	virtual void Free() override;


};

END