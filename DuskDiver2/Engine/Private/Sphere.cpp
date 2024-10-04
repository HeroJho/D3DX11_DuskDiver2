#include "..\Public\Sphere.h"
#include "AABB.h"
#include "OBB.h"
#include "HierarchyNode.h"
#include "PhysX_Manager.h"

CSphere::CSphere(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCollider(pDevice, pContext)
{

}

CSphere::CSphere(const CSphere & rhs)
	: CCollider(rhs)
{

}

HRESULT CSphere::Initialize_Prototype(CCollider::TYPE eColliderType)
{
	if (FAILED(__super::Initialize_Prototype(eColliderType)))
		return E_FAIL;

	m_eColliderType = CCollider::TYPE_SPHERE;

	return S_OK;
}

HRESULT CSphere::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pOriginal_Sphere = new BoundingSphere(m_ColliderDesc.vCenter, m_ColliderDesc.vSize.x);
	m_pSphere = new BoundingSphere(*m_pOriginal_Sphere);

	m_pXBody = CPhysX_Manager::Get_Instance()->Create_Sphere(m_ColliderDesc.vCenter, m_ColliderDesc.vSize.x, m_ColliderDesc.iFlag);
	m_pXBody->userData = this;

	return S_OK;
}

void CSphere::Update(_fmatrix TransformMatrix)
{
	if (m_ColliderDesc.pSocket == nullptr)
		m_pOriginal_Sphere->Transform(*m_pSphere, TransformMatrix);
	else
	{
		_matrix SocketMatrix = m_ColliderDesc.pSocket->Get_CombinedTransformation() * XMLoadFloat4x4(&m_ColliderDesc.PivotMatrix) * TransformMatrix;
		SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
		SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
		SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);
		m_pOriginal_Sphere->Transform(*m_pSphere, SocketMatrix);
	}

	Set_Px(m_pSphere->Center);

}

_bool CSphere::Collision(CCollider * pTargetCollider)
{
	CCollider::TYPE		eType = pTargetCollider->Get_ColliderType();

	m_isColl = false;

	switch (eType)
	{
	case CCollider::TYPE_AABB:
		m_isColl = m_pSphere->Intersects(((CAABB*)pTargetCollider)->Get_Collider());
		break;

	case CCollider::TYPE_OBB:
		m_isColl = m_pSphere->Intersects(((COBB*)pTargetCollider)->Get_Collider());
		break;

	case CCollider::TYPE_SPHERE:
		m_isColl = m_pSphere->Intersects(((CSphere*)pTargetCollider)->Get_Collider());
		break;
	}

	return m_isColl;
}

void CSphere::Set_Size(_float3 vSize)
{
	m_ColliderDesc.vSize = vSize;
	m_pOriginal_Sphere->Radius = m_ColliderDesc.vSize.x;
	PxShape* pShape;
	m_pXBody->getShapes(&pShape, sizeof(PxShape));
	PxSphereGeometry PxG;
	pShape->getSphereGeometry(PxG);
	PxG.radius = vSize.x;
	pShape->setGeometry(PxG);
}

void CSphere::Set_Center(_float3 vCenter)
{
	m_ColliderDesc.vCenter = vCenter;
	m_pOriginal_Sphere->Center = m_ColliderDesc.vCenter;
}

HRESULT CSphere::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pBatch->Begin();	

	DX::Draw(m_pBatch, *m_pSphere, XMLoadFloat4(&m_vColor));

	m_pBatch->End();


	return S_OK;
}

CSphere * CSphere::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CCollider::TYPE eColliderType)
{
	CSphere*			pInstance = new CSphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eColliderType)))
	{
		MSG_BOX(TEXT("Failed To Created : CSphere"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CSphere::Clone(void * pArg)
{
	CSphere*			pInstance = new CSphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CSphere"));
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CSphere::Free()
{
	__super::Free();

	Safe_Delete(m_pOriginal_Sphere);
	Safe_Delete(m_pSphere);

}
