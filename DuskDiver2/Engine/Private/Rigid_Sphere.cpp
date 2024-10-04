#include "..\Public\Rigid_Sphere.h"

#include "PhysX_Manager.h"
#include "Transform.h"
#include "PipeLine.h"

CRigid_Sphere::CRigid_Sphere(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CRigid_Sphere::CRigid_Sphere(const CRigid_Sphere & rhs)
	: CComponent(rhs)
#ifdef _DEBUG
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif // _DEBUG
{
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif // _DEBUG
}


HRESULT CRigid_Sphere::Initialize_Prototype()
{

#ifdef _DEBUG

	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);
	m_pEffect = new BasicEffect(m_pDevice);

	m_pEffect->SetVertexColorEnabled(true);

	const void*		pShaderByteCodes = nullptr;
	size_t			iLength = 0;

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCodes, &iLength);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCodes, iLength, &m_pInputLayout)))
		return E_FAIL;
#endif

	return S_OK;
}

HRESULT CRigid_Sphere::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	memcpy(&m_Desc, pArg, sizeof(RIGIDEDESC));

	PxMaterial* pMaterial = CPhysX_Manager::Get_Instance()->Create_Material(m_Desc.fStaticFriction, m_Desc.fDynamicFriction, m_Desc.fRestitution);


	m_pXBody = CPhysX_Manager::Get_Instance()->Create_PxSphere(m_Desc.vExtense, m_Desc.fWeight, m_Desc.fAngleDump, pMaterial);
	pMaterial->release();

	m_pXBody->setGlobalPose(PxTransform(m_Desc.vStartPos.x, m_Desc.vStartPos.y, m_Desc.vStartPos.z));
	/* 복제될때 셋팅하고자하는 상태로 갱신한다. */
	m_pOriginal_Sphere = new BoundingSphere(_float3(0.f, 0.f, 0.f), m_Desc.vExtense.x * 0.5f);



	/* 회전에 대한 상태. */
	//_matrix		RotationMatrix = XMMatrixRotationX(m_ColliderDesc.vRotation.x) *
	//	XMMatrixRotationY(m_ColliderDesc.vRotation.y) *
	//	XMMatrixRotationZ(m_ColliderDesc.vRotation.z);

	// m_pOriginal_OBB->Transform(*m_pOriginal_OBB, RotationMatrix);

	m_pSphere = new BoundingSphere(*m_pOriginal_Sphere);

	return S_OK;
}

void CRigid_Sphere::Edit_Px()
{
	PxMaterial* pMaterial = CPhysX_Manager::Get_Instance()->Create_Material(m_Desc.fStaticFriction, m_Desc.fDynamicFriction, m_Desc.fRestitution);

	m_pXBody->release();
	m_pXBody = CPhysX_Manager::Get_Instance()->Create_PxSphere(m_Desc.vExtense, m_Desc.fWeight, m_Desc.fAngleDump, pMaterial);
	m_pOriginal_Sphere->Radius = m_Desc.vExtense.x;

	pMaterial->release();
}

void CRigid_Sphere::Reset_Pos()
{
	m_pXBody->setGlobalPose(PxTransform(0.f, 3.f, 0.f));
}

void CRigid_Sphere::Update_Transform(CTransform * pTran)
{
	PxTransform PxTran = m_pXBody->getGlobalPose();
	_float4x4 mWorldMtx;

	_float3 vScale = pTran->Get_Scale();


	_vector vRight = XMVector3Normalize(XMVectorSet(PxTran.q.getBasisVector0().x, PxTran.q.getBasisVector0().y, PxTran.q.getBasisVector0().z, 0.f)) * vScale.x;
	_vector vUp = XMVector3Normalize(XMVectorSet(PxTran.q.getBasisVector1().x, PxTran.q.getBasisVector1().y, PxTran.q.getBasisVector1().z, 0.f)) * vScale.y;
	_vector vLook = XMVector3Normalize(XMVectorSet(PxTran.q.getBasisVector2().x, PxTran.q.getBasisVector2().y, PxTran.q.getBasisVector2().z, 0.f)) * vScale.z;


	XMStoreFloat4((_float4*)&mWorldMtx._11, vRight);
	XMStoreFloat4((_float4*)&mWorldMtx._21, vUp);
	XMStoreFloat4((_float4*)&mWorldMtx._31, vLook);

	mWorldMtx._41 = PxTran.p.x;
	mWorldMtx._42 = PxTran.p.y;
	mWorldMtx._43 = PxTran.p.z;
	mWorldMtx._44 = 1.f;

	// 실제 Ph 위치
	m_pOriginal_Sphere->Transform(*m_pSphere, XMLoadFloat4x4(&mWorldMtx));

	// 그리기 위한 용도 -> Pivot 적용해서
	_vector vPivotPos = XMVector3TransformCoord(XMLoadFloat3(&m_Desc.vPos), XMLoadFloat4x4(&mWorldMtx));
	mWorldMtx._41 = XMVectorGetX(vPivotPos);
	mWorldMtx._42 = XMVectorGetY(vPivotPos);
	mWorldMtx._43 = XMVectorGetZ(vPivotPos);
	mWorldMtx._44 = 1.f;
	pTran->Set_WorldMatrix(mWorldMtx);
}



CRigid_Sphere * CRigid_Sphere::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRigid_Sphere*			pInstance = new CRigid_Sphere(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CRigid_Sphere"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CRigid_Sphere::Clone(void * pArg)
{
	CRigid_Sphere*			pInstance = new CRigid_Sphere(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CRigid_Sphere"));
		Safe_Release(pInstance);
	}
	return pInstance;
}


#ifdef _DEBUG

HRESULT CRigid_Sphere::Render()
{


	m_pEffect->SetWorld(XMMatrixIdentity());

	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	m_pEffect->SetView(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

	RELEASE_INSTANCE(CPipeLine);

	m_pEffect->Apply(m_pContext);

	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pBatch->Begin();

	DX::Draw(m_pBatch, *m_pSphere, XMLoadFloat4(&_float4(1.f, 0.f, 0.f, 1.f)));

	m_pBatch->End();

	return S_OK;
}
#endif // _DEBUG



void CRigid_Sphere::Free()
{
	__super::Free();


	//if(m_Material)
	//	m_Material->release();

	if (m_pXBody)
		m_pXBody->release();

#ifdef _DEBUG
	if (false == m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}

	Safe_Release(m_pInputLayout);

#endif // _DEBUG

	Safe_Delete(m_pOriginal_Sphere);
	Safe_Delete(m_pSphere);

}


