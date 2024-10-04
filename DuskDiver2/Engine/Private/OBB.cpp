#include "..\Public\OBB.h"
#include "AABB.h"
#include "Sphere.h"
#include "HierarchyNode.h"
#include "PhysX_Manager.h"

COBB::COBB(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCollider(pDevice, pContext)
{

}

COBB::COBB(const COBB & rhs)
	: CCollider(rhs)
{

}

HRESULT COBB::Initialize_Prototype(CCollider::TYPE eColliderType)
{
	if (FAILED(__super::Initialize_Prototype(eColliderType)))
		return E_FAIL;

	m_eColliderType = CCollider::TYPE_OBB;

	return S_OK;
}

HRESULT COBB::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	/* 복제될때 셋팅하고자하는 상태로 갱신한다. */
	m_pOriginal_OBB = new BoundingOrientedBox(m_ColliderDesc.vCenter, _float3(m_ColliderDesc.vSize.x * 0.5f,
		m_ColliderDesc.vSize.y * 0.5f,
		m_ColliderDesc.vSize.z * 0.5f), _float4(0.f, 0.f, 0.f, 1.f));

	/* 회전에 대한 상태. */
	_matrix		RotationMatrix = XMMatrixRotationX(m_ColliderDesc.vRotation.x) *
		XMMatrixRotationY(m_ColliderDesc.vRotation.y) *
		XMMatrixRotationZ(m_ColliderDesc.vRotation.z);

	m_pOriginal_OBB->Transform(*m_pOriginal_OBB, RotationMatrix);

	m_pOBB = new BoundingOrientedBox(*m_pOriginal_OBB);

	m_pXBody = CPhysX_Manager::Get_Instance()->Create_Box(m_ColliderDesc.vCenter, m_ColliderDesc.vSize, m_ColliderDesc.iFlag);
	m_pXBody->userData = this;
	Set_Px(m_pOBB->Center, m_pOBB->Orientation);

	return S_OK;
}

void COBB::Update(_fmatrix TransformMatrix)
{
	// 소켓 뼈가 없다면 그대로 월드행렬로 갱신
	if(m_ColliderDesc.pSocket == nullptr)
		m_pOriginal_OBB->Transform(*m_pOBB, TransformMatrix);
	else 
	{
		// 소켓 뼈가 있다면 해당 계층 뼈의 행렬 값도 적용해서 갱신
		_matrix SocketMatrix = m_ColliderDesc.pSocket->Get_CombinedTransformation() * 
			XMLoadFloat4x4(&m_ColliderDesc.PivotMatrix) * TransformMatrix;
		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);
		m_pOriginal_OBB->Transform(*m_pOBB, SocketMatrix);
	}

	// 최종적으로 적용된 행렬 값으로 Actor 갱신
	Set_Px(m_pOBB->Center, m_pOBB->Orientation);
}

_bool COBB::Collision(CCollider * pTargetCollider)
{
	CCollider::TYPE		eType = pTargetCollider->Get_ColliderType();

	m_isColl = false;

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		m_isColl = m_pOBB->Intersects(((CAABB*)pTargetCollider)->Get_Collider());
		break;

	case CCollider::TYPE_OBB:
		m_isColl = m_pOBB->Intersects(((COBB*)pTargetCollider)->Get_Collider());
		break;

	case CCollider::TYPE_SPHERE:
		m_isColl = m_pOBB->Intersects(((CSphere*)pTargetCollider)->Get_Collider());
		break;
	}

	return m_isColl;
}

void COBB::Set_Size(_float3 vSize)
{
	m_ColliderDesc.vSize = { vSize.x, vSize.y, vSize.z };
	m_pOriginal_OBB->Extents = m_ColliderDesc.vSize;
	PxShape* pShape;
	m_pXBody->getShapes(&pShape, sizeof(PxShape));
	PxBoxGeometry PxG;
	pShape->getBoxGeometry(PxG);
	PxG.halfExtents = PxVec3(vSize.x, vSize.y, vSize.z);
	pShape->setGeometry(PxG);
}

void COBB::Set_Center(_float3 vCenter)
{
	m_ColliderDesc.vCenter = vCenter;
	m_pOriginal_OBB->Center = m_ColliderDesc.vCenter;
}

void COBB::Set_Rot(_float3 vRot)
{
	m_ColliderDesc.vRotation = vRot;
	
	m_pOriginal_OBB->Orientation = _float4(0.f, 0.f, 0.f, 1.f);

	_matrix		RotationMatrix = XMMatrixRotationX(m_ColliderDesc.vRotation.x) *
		XMMatrixRotationY(m_ColliderDesc.vRotation.y) *
		XMMatrixRotationZ(m_ColliderDesc.vRotation.z);

	m_pOriginal_OBB->Transform(*m_pOriginal_OBB, RotationMatrix);

}

void COBB::Set_SizeOBB(_float3 vSize)
{
	//m_ColliderDesc.vSize = { vSize.x * 0.5f, vSize.y * 0.5f, vSize.z * 0.5f };
	//m_pOriginal_OBB->Extents = m_ColliderDesc.vSize;
	//PxShape* pShape;
	//m_pXBody->getShapes(&pShape, sizeof(PxShape));
	//PxBoxGeometry PxG;
	//pShape->getBoxGeometry(PxG);
	//PxG.halfExtents = PxVec3(vSize.x * 0.5f, vSize.y * 0.5f, vSize.z * 0.5f);
	//pShape->setGeometry(PxG);

	Safe_Delete(m_pOriginal_OBB);
	Safe_Delete(m_pOBB);
	CPhysX_Manager::Get_Instance()->Remove_Actor(m_pXBody);

	m_ColliderDesc.vSize = vSize;

	m_pOriginal_OBB = new BoundingOrientedBox(m_ColliderDesc.vCenter, _float3(m_ColliderDesc.vSize.x * 0.5f,
		m_ColliderDesc.vSize.y * 0.5f,
		m_ColliderDesc.vSize.z * 0.5f), _float4(0.f, 0.f, 0.f, 1.f));

	/* 회전에 대한 상태. */
	_matrix		RotationMatrix = XMMatrixRotationX(m_ColliderDesc.vRotation.x) *
		XMMatrixRotationY(m_ColliderDesc.vRotation.y) *
		XMMatrixRotationZ(m_ColliderDesc.vRotation.z);

	m_pOriginal_OBB->Transform(*m_pOriginal_OBB, RotationMatrix);

	m_pOBB = new BoundingOrientedBox(*m_pOriginal_OBB);

	m_pXBody = CPhysX_Manager::Get_Instance()->Create_Box(m_ColliderDesc.vCenter, m_ColliderDesc.vSize, m_ColliderDesc.iFlag);
	m_pXBody->userData = this;
	Set_Px(m_pOBB->Center, m_pOBB->Orientation);
}

HRESULT COBB::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pBatch->Begin();

	DX::Draw(m_pBatch, *m_pOBB, XMLoadFloat4(&m_vColor));

	m_pBatch->End();

	return S_OK;
}



COBB * COBB::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CCollider::TYPE eColliderType)
{
	COBB*			pInstance = new COBB(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eColliderType)))
	{
		MSG_BOX(TEXT("Failed To Created : COBB"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * COBB::Clone(void * pArg)
{
	COBB*			pInstance = new COBB(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : COBB"));
		Safe_Release(pInstance);
	}
	return pInstance;
}


void COBB::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginal_OBB);
	Safe_Delete(m_pOBB);
}
