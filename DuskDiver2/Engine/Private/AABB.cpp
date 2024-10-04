#include "..\Public\AABB.h"
#include "PipeLine.h"

#include "OBB.h"
#include "Sphere.h"

CAABB::CAABB(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCollider(pDevice, pContext)
{

}

CAABB::CAABB(const CAABB & rhs)
	: CCollider(rhs)
	, m_pAABB(rhs.m_pAABB)
{

}

HRESULT CAABB::Initialize_Prototype(CCollider::TYPE eColliderType)
{
	if (FAILED(__super::Initialize_Prototype(eColliderType)))
		return E_FAIL;
	m_eColliderType = CCollider::TYPE_AABB;
	return S_OK;
}

HRESULT CAABB::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;



	/* �����ɶ� �����ϰ����ϴ� ���·� �����Ѵ�. */
	m_pOriginal_AABB = new BoundingBox(m_ColliderDesc.vCenter, _float3(m_ColliderDesc.vSize.x * 0.5f,
		m_ColliderDesc.vSize.y * 0.5f, 
		m_ColliderDesc.vSize.z * 0.5f));

	m_pAABB = new BoundingBox(*m_pOriginal_AABB);

	return S_OK;
}

void CAABB::Update(_fmatrix TransformMatrix)
{
	m_pOriginal_AABB->Transform(*m_pAABB, Remove_Rotation(TransformMatrix));
}

_bool CAABB::Collision(CCollider * pTargetCollider)
{
	CCollider::TYPE		eType = pTargetCollider->Get_ColliderType();

	m_isColl = false;

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		m_isColl = m_pAABB->Intersects(((CAABB*)pTargetCollider)->Get_Collider());
		break;

	case CCollider::TYPE_OBB:
		m_isColl = m_pAABB->Intersects(((COBB*)pTargetCollider)->Get_Collider());
		break;

	case CCollider::TYPE_SPHERE:
		m_isColl = m_pAABB->Intersects(((CSphere*)pTargetCollider)->Get_Collider());
		break;
	}	

	return m_isColl;
}

void CAABB::Set_Size(_float3 vSize)
{
	m_ColliderDesc.vSize = { vSize.x * 0.5f, vSize.y * 0.5f, vSize.z * 0.5f };
	m_pOriginal_AABB->Extents = m_ColliderDesc.vSize;
}

void CAABB::Set_Center(_float3 vCenter)
{
	m_ColliderDesc.vCenter = vCenter;
	m_pOriginal_AABB->Center = m_ColliderDesc.vCenter;
}

#ifdef _DEBUG

HRESULT CAABB::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pBatch->Begin();
	
	DX::Draw(m_pBatch, *m_pAABB, XMLoadFloat4(&m_vColor));

	m_pBatch->End();

	return S_OK;
}

#endif // _DEBUG

_float3 CAABB::Compute_Min()
{
	return _float3(m_pAABB->Center.x - m_pAABB->Extents.x, 
		m_pAABB->Center.y - m_pAABB->Extents.y, 
		m_pAABB->Center.z - m_pAABB->Extents.z);
}

_float3 CAABB::Compute_Max()
{
	return _float3(m_pAABB->Center.x + m_pAABB->Extents.x,
		m_pAABB->Center.y + m_pAABB->Extents.y,
		m_pAABB->Center.z + m_pAABB->Extents.z);
}

CAABB * CAABB::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CCollider::TYPE eColliderType)
{
	CAABB*			pInstance = new CAABB(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eColliderType)))
	{
		MSG_BOX(TEXT("Failed To Created : CAABB"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CAABB::Clone(void * pArg)
{
	CAABB*			pInstance = new CAABB(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CAABB"));
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CAABB::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginal_AABB);
	Safe_Delete(m_pAABB);
}
