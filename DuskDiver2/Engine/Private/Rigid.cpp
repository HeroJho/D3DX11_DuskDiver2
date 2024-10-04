#include "..\Public\Rigid.h"

#include "PhysX_Manager.h"
#include "Transform.h"
#include "PipeLine.h"

CRigid::CRigid(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CRigid::CRigid(const CRigid & rhs)
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


HRESULT CRigid::Initialize_Prototype()
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

HRESULT CRigid::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	memcpy(&m_Desc, pArg, sizeof(RIGIDEDESC));

	PxMaterial* pMaterial = CPhysX_Manager::Get_Instance()->Create_Material(m_Desc.fStaticFriction, m_Desc.fDynamicFriction, m_Desc.fRestitution);

	m_Desc.vExtense.x = abs(m_Desc.vExtense.x);
	m_Desc.vExtense.y = abs(m_Desc.vExtense.y);
	m_Desc.vExtense.z = abs(m_Desc.vExtense.z);
	m_pXBody = CPhysX_Manager::Get_Instance()->Create_PxBox(m_Desc.vExtense, m_Desc.fWeight, m_Desc.fAngleDump, pMaterial, m_Desc.fMaxVel);
	pMaterial->release();
	
	//XMMatrixRotationX(m_Desc.vRotation.x);
	//XMMatrixRotationX(m_Desc.vRotation.y);
	//XMMatrixRotationX(m_Desc.vRotation.x);
	// XMQuaternionRotationMatrix()
	_float4 vQuar; XMStoreFloat4(&vQuar, XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&m_Desc.vRotation)));
	m_pXBody->setGlobalPose(PxTransform(m_Desc.vStartPos.x, m_Desc.vStartPos.y, m_Desc.vStartPos.z, PxQuat(vQuar.x, vQuar.y, vQuar.z, vQuar.w)));
	/* 복제될때 셋팅하고자하는 상태로 갱신한다. */
	m_pOriginal_OBB = new BoundingOrientedBox(_float3(0.f, 0.f, 0.f), _float3(m_Desc.vExtense.x * 0.5f,
		m_Desc.vExtense.y* 0.5f,
		m_Desc.vExtense.z* 0.5f), _float4(0.f, 0.f, 0.f, 1.f));

	/* 회전에 대한 상태. */
	//_matrix		RotationMatrix = XMMatrixRotationX(m_ColliderDesc.vRotation.x) *
	//	XMMatrixRotationY(m_ColliderDesc.vRotation.y) *
	//	XMMatrixRotationZ(m_ColliderDesc.vRotation.z);

	// m_pOriginal_OBB->Transform(*m_pOriginal_OBB, RotationMatrix);

	m_pOBB = new BoundingOrientedBox(*m_pOriginal_OBB);

	m_bFirst = true;

	return S_OK;
}

void CRigid::Edit_Px()
{
	PxMaterial* pMaterial = CPhysX_Manager::Get_Instance()->Create_Material(m_Desc.fStaticFriction, m_Desc.fDynamicFriction, m_Desc.fRestitution);

	m_pXBody->release();
	m_pXBody = CPhysX_Manager::Get_Instance()->Create_PxBox(m_Desc.vExtense, m_Desc.fWeight, m_Desc.fAngleDump, pMaterial, m_Desc.fMaxVel);
	XMStoreFloat3(&m_pOriginal_OBB->Extents, XMLoadFloat3(&m_Desc.vExtense) * 0.5f);

	m_pXBody->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);
	m_pXBody->setLinearVelocity(PxVec3(m_Desc.fMaxVel, m_Desc.fMaxVel, m_Desc.fMaxVel));
	Edit_PosRot();

	pMaterial->release();

	m_bFirst = true;
}


void CRigid::Edit_PosRot()
{
	_float4 vQuar; XMStoreFloat4(&vQuar, XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&m_Desc.vRotation)));
	m_pXBody->setGlobalPose(PxTransform(m_Desc.vStartPos.x, m_Desc.vStartPos.y, m_Desc.vStartPos.z, PxQuat(vQuar.x, vQuar.y, vQuar.z, vQuar.w)));
	
	m_bFirst = true;
}

void CRigid::Set_Sleep(_bool bIsSleep)
{
	m_pXBody->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, bIsSleep);
}

void CRigid::Add_Force(_float3 vForce)
{
	m_pXBody->addForce(PxVec3(vForce.x, vForce.y, vForce.z), PxForceMode::eFORCE);
}

void CRigid::Add_Torque(_float3 vTorque)
{
	m_pXBody->addTorque(PxVec3(vTorque.x, vTorque.y, vTorque.z), PxForceMode::eFORCE);
}


void CRigid::Update_Transform(CTransform * pTran)
{
#ifdef _DEBUG
	m_bGreenColor = false;
#endif // _DEBUG


	if (!m_bFirst && m_pXBody->isSleeping())
		return;


	// 내 Transform보다 vPos만큼 더한 값이 BodyX의 값.
	// BodyX 보다 vPos만큼 뺀 값이 Transform 값
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
	m_pOriginal_OBB->Transform(*m_pOBB, XMLoadFloat4x4(&mWorldMtx));

	// 그리기 위한 용도 -> Pivot 적용해서
	_vector vPivotPos = XMVector3TransformCoord(XMLoadFloat3(&m_Desc.vPos), XMLoadFloat4x4(&mWorldMtx));
	mWorldMtx._41 = XMVectorGetX(vPivotPos);
	mWorldMtx._42 = XMVectorGetY(vPivotPos);
	mWorldMtx._43 = XMVectorGetZ(vPivotPos);
	mWorldMtx._44 = 1.f;
	pTran->Set_WorldMatrix(mWorldMtx);

	m_bFirst = false;

}




CRigid * CRigid::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRigid*			pInstance = new CRigid(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CRigid"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CRigid::Clone(void * pArg)
{
	CRigid*			pInstance = new CRigid(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CRigid"));
		Safe_Release(pInstance);
	}
	return pInstance;
}


#ifdef _DEBUG

HRESULT CRigid::Render()
{


	m_pEffect->SetWorld(XMMatrixIdentity());

	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	m_pEffect->SetView(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

	RELEASE_INSTANCE(CPipeLine);

	m_pEffect->Apply(m_pContext);

	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pBatch->Begin();


	if(m_bGreenColor)
		DX::Draw(m_pBatch, *m_pOBB, XMLoadFloat4(&_float4(1.f, 0.f, 0.f, 1.f)));
	else
		DX::Draw(m_pBatch, *m_pOBB, XMLoadFloat4(&_float4(0.f, 1.f, 0.f, 1.f)));


	m_pBatch->End();

	return S_OK;
}
#endif // _DEBUG



void CRigid::Free()
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

	Safe_Delete(m_pOriginal_OBB);
	Safe_Delete(m_pOBB);

}


