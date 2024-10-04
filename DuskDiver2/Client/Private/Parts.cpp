#include "stdafx.h"
#include "Parts.h"
#include "GameInstance.h"

CParts::CParts(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CParts::CParts(const CParts & rhs)
	: CGameObject(rhs)
{
}

HRESULT CParts::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CParts::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}


HRESULT CParts::SetUp_State(_fmatrix StateMatrix)
{
	m_pParentTransformCom->Set_State(CTransform::STATE_RIGHT, StateMatrix.r[0]);
	m_pParentTransformCom->Set_State(CTransform::STATE_UP, StateMatrix.r[1]);
	m_pParentTransformCom->Set_State(CTransform::STATE_LOOK, StateMatrix.r[2]);
	m_pParentTransformCom->Set_State(CTransform::STATE_POSITION, StateMatrix.r[3]);

	m_pParentTransformCom->Set_Scale(XMVectorSet(1.f, 1.f, 1.f, 1.f));

	return S_OK;
}

void CParts::Rotation(_float3 vAxis, _float fRadian, _float3 vAxis2, _float fRadian2, _float3 vAxis3, _float fRadian3)
{
	m_pTransformCom->RotationThree(vAxis, fRadian, vAxis2, fRadian2, vAxis3, fRadian3);
}

_float4 CParts::Get_Pos()
{
	_float4 Pos;
	XMStoreFloat4(&Pos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	return Pos;
}

void CParts::Set_Pos(_float4 Pos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&Pos));
}

_float4 CParts::Get_ParentPos()
{
	_float4 Pos;
	XMStoreFloat4(&Pos, m_pParentTransformCom->Get_State(CTransform::STATE_POSITION));
	return Pos;
}

void CParts::Set_ParentPos(_float4 Pos)
{
	m_pParentTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&Pos));
}

_float3 CParts::Get_ParentDir()
{
	_float3 vDir;
	XMStoreFloat3(&vDir, m_pParentTransformCom->Get_State(CTransform::STATE_LOOK));
	return vDir;
}

_float4x4 CParts::Get_ParentMatrix()
{
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix());
	return WorldMatrix;
}

HRESULT CParts::Ready_Components()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.fSpeedPerSec = 10.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_ParentTransform"), (CComponent**)&m_pParentTransformCom)))
		return E_FAIL;

	return S_OK;
}

CParts * CParts::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CParts*		pInstance = new CParts(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CParts"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CParts::Clone(void * pArg)
{
	CParts*		pInstance = new CParts(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CParts"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CParts::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pParentTransformCom);
}
