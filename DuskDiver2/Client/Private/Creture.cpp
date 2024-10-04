#include "stdafx.h"
#include "..\Public\Creture.h"
#include "GameInstance.h"
CCreture::CCreture(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CCreture::CCreture(const CCreture & rhs)
	: CGameObject(rhs)
{
}

HRESULT CCreture::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCreture::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	return S_OK;
}


_float4 CCreture::Get_WorldPos()
{
	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	return vPos;
}

_float4x4 CCreture::Get_WorldMatrix()
{
	return m_pTransformCom->Get_WorldFloat4x4();
}

void CCreture::Set_WorldPos(_float4 vWorldPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vWorldPos));
}

_float3 CCreture::Get_Dir(_float3 vAxis)
{
	if (vAxis.x == 1.f)
	{
		_float3 vRight;
		XMStoreFloat3(&vRight, m_pTransformCom->Get_State(CTransform::STATE_RIGHT));
		return vRight;
	}
	else if (vAxis.y == 1.f)
	{
		_float3 vUp;
		XMStoreFloat3(&vUp, m_pTransformCom->Get_State(CTransform::STATE_UP));
		return vUp;
	}
	else if (vAxis.z == 1.f)
	{
		_float3 vLook;
		XMStoreFloat3(&vLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
		return vLook;
	}
	return _float3();
}

void CCreture::Set_Dir(_float3 vAxis, _float3 vDir)
{
	if (vAxis.x = 1.f)		
		m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMLoadFloat3(&vDir));	
	else if (vAxis.y = 1.f)
		m_pTransformCom->Set_State(CTransform::STATE_UP, XMLoadFloat3(&vDir));
	else if (vAxis.z = 1.f)
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&vDir));
}



HRESULT CCreture::Ready_Components()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	return S_OK;
}

void CCreture::Check_Hurt(_float fTimeDelta)
{
	if (!m_bHurt)
	{
		m_fHurtAcc += 1.f * fTimeDelta;
		if (m_fHurtAcc >= m_fHurtTime)
		{
			m_fHurtAcc = 0.f;
			m_bHurt = true;
		}
	}
}

CCreture * CCreture::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCreture*		pInstance = new CCreture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCreture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCreture::Clone(void * pArg)
{
	CCreture*		pInstance = new CCreture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCreture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCreture::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
