#pragma once

#include "Component.h"
#include "DebugDraw.h"


BEGIN(Engine)
class CHierarchyNode;

class ENGINE_DLL CCollider abstract : public CComponent
{
public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };
public:
	typedef struct tagColliderDesc
	{
		char			sTag[MAX_PATH];	// 콜라이더 태그
		_float3			vCenter;
		_float3			vSize;
		_float3			vRotation;
		CHierarchyNode* pSocket = nullptr;
		_float4x4 PivotMatrix;
		_uint			iFlag = 0;

		class CGameObject* pOwner = nullptr;

	}COLLIDERDESC;

	typedef struct tagOtherToMeColDesc
	{
		class CGameObject* pOther = nullptr;
		const COLLIDERDESC* pOtherDesc;
		const COLLIDERDESC* pMyDesc;

	}OTHERTOMECOLDESC;


protected:
	CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	TYPE Get_ColliderType() const {
		return m_eColliderType;
	}
	const COLLIDERDESC* Get_Desc() { return &m_ColliderDesc; }

	virtual void Set_Size(_float3 vSize) {};
	virtual void Set_Center(_float3 vCenter) {};
	virtual void Set_Rot(_float3 vRot) {};

	_bool Get_IsCurTrigger() { return m_bIsCurTrigger; }
	void Set_IsCurTrigger(_bool bIsCurTrigger) { m_bIsCurTrigger = bIsCurTrigger; }

	void Set_Sleep();
	void Set_Awake();

public:
	HRESULT Change_Socket(CHierarchyNode* pNode);

public:
	virtual HRESULT Initialize_Prototype(TYPE eColliderType);
	virtual HRESULT Initialize(void* pArg);
	virtual void Update(_fmatrix TransformMatrix) = 0;
	virtual _bool Collision(CCollider* pTargetCollider) = 0;


#ifdef _DEBUG
public:
	virtual HRESULT Render() override;
#endif // _DEBUG

protected:
	TYPE					m_eColliderType = TYPE_END;
	COLLIDERDESC			m_ColliderDesc;
	_bool					m_isColl = false;
	_bool					m_bIsCurTrigger = false;


#ifdef _DEBUG
protected:
	PrimitiveBatch<VertexPositionColor>*		m_pBatch = nullptr;
	BasicEffect*								m_pEffect = nullptr;	
	ID3D11InputLayout*							m_pInputLayout = nullptr;
	_float4										m_vColor = _float4(0.f, 1.f, 0.f, 1.f);
#endif

protected:
	_matrix Remove_Rotation(_fmatrix Matrix);

public:	
	virtual CComponent* Clone(void* pArg) = 0;
	virtual void Free() override;



	/* For. PhysX */
public:
	CGameObject* Get_Owner() { return m_ColliderDesc.pOwner; }

protected:
	void Set_Px(_float3 vPos, _float4 vRot);
	void Set_Px(_float3 vPos);

protected:
	PxRigidDynamic* m_pXBody = nullptr;

};

END