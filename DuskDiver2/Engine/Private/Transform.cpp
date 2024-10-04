#include "..\Public\Transform.h"
#include "Navigation.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
	ZeroMemory(&m_vRotation, sizeof(_float3));
	ZeroMemory(&m_vScale, sizeof(_float3));
}

void CTransform::Set_State(STATE eState, _fvector vState)
{
	_matrix	WorldMatrix = XMLoadFloat4x4(&m_WorldMatrix);

	WorldMatrix.r[eState] = vState;

	XMStoreFloat4x4(&m_WorldMatrix, WorldMatrix);
}

void CTransform::Set_Look(_fvector vLook)
{
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	_vector		vUp = XMVector3Cross(vLook, vRight);

	_float3		vScale = Get_Scale();

	Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}


_matrix CTransform::Get_WorldMatrix_NoScale()
{
	_matrix vMatrix;
	vMatrix.r[0] = XMVector3Normalize(Get_State(CTransform::STATE_RIGHT));
	vMatrix.r[1] = XMVector3Normalize(Get_State(CTransform::STATE_UP));
	vMatrix.r[2] = XMVector3Normalize(Get_State(CTransform::STATE_LOOK));
	vMatrix.r[3] = Get_State(CTransform::STATE_POSITION);

	return vMatrix;
}

HRESULT CTransform::Initialize_Prototype()
{
	/* vector -> float : XMStore*/
	/* float -> vector : XMLoad */

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());	

	return S_OK;
}

HRESULT CTransform::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORMDESC));
	
	return S_OK;
}

void CTransform::Go_Straight(_float fTimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Backward(_float fTimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Left(_float fTimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Right(_float fTimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * fTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::RotationThree(_float3 vAxis, _float fRadian, _float3 vAxis2, _float fRadian2, _float3 vAxis3, _float fRadian3)
{
	_float3		vScale = Get_Scale();
	_vector vRight = { 1.f,0.f,0.f };
	vRight = vRight * vScale.x;

	_vector vUp = { 0.f,1.f,0.f };
	vUp = vUp * vScale.y;

	_vector vLook = { 0.f,0.f,1.f };
	vLook = vLook * vScale.z;

	_vector Axis = XMLoadFloat3(&vAxis);
	_matrix		RotationMatrix = XMMatrixRotationAxis(Axis, XMConvertToRadians(fRadian));

	Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(vRight, RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector3TransformNormal(vUp, RotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(vLook, RotationMatrix));


	_vector		vRight2 = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp2 = Get_State(CTransform::STATE_UP);
	_vector		vLook2 = Get_State(CTransform::STATE_LOOK);

	_vector Axis2 = XMLoadFloat3(&vAxis2);
	_matrix		RotationMatrix2 = XMMatrixRotationAxis(Axis2, XMConvertToRadians(fRadian2));

	Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(vRight2, RotationMatrix2));
	Set_State(CTransform::STATE_UP, XMVector3TransformNormal(vUp2, RotationMatrix2));
	Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(vLook2, RotationMatrix2));

	_vector		vRight3 = Get_State(CTransform::STATE_RIGHT);
	_vector		vUp3 = Get_State(CTransform::STATE_UP);
	_vector		vLook3 = Get_State(CTransform::STATE_LOOK);

	_vector Axis3 = XMLoadFloat3(&vAxis3);
	_matrix		RotationMatrix3 = XMMatrixRotationAxis(Axis3, XMConvertToRadians(fRadian3));

	Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(vRight3, RotationMatrix3));
	Set_State(CTransform::STATE_UP, XMVector3TransformNormal(vUp3, RotationMatrix3));
	Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(vLook3, RotationMatrix3));

}

void CTransform::Go_Dir(_fvector vDir, _float fSpeed, _float fTimeDelta)
{
	_vector vPosition = Get_State(CTransform::STATE_POSITION);
	_vector vLook = XMVector3Normalize(vDir);

	vPosition += vLook * fSpeed * fTimeDelta;

	Set_State(CTransform::STATE_POSITION, vPosition);
}

void CTransform::Go_Dir(_fvector vDir, _float fSpeed, _float fTimeDelta, class CNavigation* pNavigation)
{
	_vector vPosition = Get_State(CTransform::STATE_POSITION);
	_vector vDirN = XMVector3Normalize(vDir);

	if (0.05f > XMVectorGetX(XMVector3Length(vDirN)))
		return;

	vPosition += vDir * fSpeed * fTimeDelta;

	_vector vNormal = { 0.f,0.f,0.f,0.f };
	
	if (pNavigation->isMove(vPosition, &vNormal) == true)	
		Set_State(CTransform::STATE_POSITION, vPosition);
	else
	{
		if (XMVectorGetX(XMVector3Length(vNormal)) == 0)
			return;
		_vector      vPosition = Get_State(CTransform::STATE_POSITION);
		_vector vSlide = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDirN) * (-1.f), vNormal)) * vNormal;
		vSlide += XMVector3Normalize(vDirN);
		vPosition += vSlide * m_TransformDesc.fSpeedPerSec * fTimeDelta;
		Set_State(CTransform::STATE_POSITION, vPosition);
	}
	

	
}

void CTransform::Set_Scale(_fvector vScaleInfo)
{
	/*if (0.001f > XMVectorGetX(vScaleInfo) || 0.001f > XMVectorGetY(vScaleInfo) || 0.001f > XMVectorGetZ(vScaleInfo))
		return;*/

	XMStoreFloat3(&m_vScale, vScaleInfo);

	Set_State(CTransform::STATE_RIGHT, 
		XMVector3Normalize(Get_State(CTransform::STATE_RIGHT)) * XMVectorGetX(vScaleInfo));
	Set_State(CTransform::STATE_UP, 
		XMVector3Normalize(Get_State(CTransform::STATE_UP)) * XMVectorGetY(vScaleInfo));
	Set_State(CTransform::STATE_LOOK, 
		XMVector3Normalize(Get_State(CTransform::STATE_LOOK)) * XMVectorGetZ(vScaleInfo));	
}

void CTransform::Set_ScaleX(_fvector vScaleInfo)
{
	Set_State(CTransform::STATE_RIGHT,
		XMVector3Normalize(Get_State(CTransform::STATE_RIGHT)) * XMVectorGetX(vScaleInfo));
}

void CTransform::Set_ScaleY(_fvector vScaleInfo)
{
	Set_State(CTransform::STATE_UP,
		XMVector3Normalize(Get_State(CTransform::STATE_UP)) * XMVectorGetY(vScaleInfo));
}

void CTransform::Set_ScaleZ(_fvector vScaleInfo)
{
	Set_State(CTransform::STATE_LOOK,
		XMVector3Normalize(Get_State(CTransform::STATE_LOOK)) * XMVectorGetZ(vScaleInfo));
}

_float3 CTransform::Get_Scale()
{
	return _float3(
		XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_RIGHT))),
		XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_UP))),
		XMVectorGetX(XMVector3Length(Get_State(CTransform::STATE_LOOK))));
}

void CTransform::TurnAngle(_fvector vAxis, _float _fAngle)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, _fAngle);

	Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(Get_State(CTransform::STATE_RIGHT), RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector3TransformNormal(Get_State(CTransform::STATE_UP), RotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(Get_State(CTransform::STATE_LOOK), RotationMatrix));
}

void CTransform::Turn(_fvector vAxis, _float fTimeDelta)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * fTimeDelta);
	
	Set_State(CTransform::STATE_RIGHT, 
		XMVector3TransformNormal(Get_State(CTransform::STATE_RIGHT), RotationMatrix));
	Set_State(CTransform::STATE_UP, 
		XMVector3TransformNormal(Get_State(CTransform::STATE_UP), RotationMatrix));
	Set_State(CTransform::STATE_LOOK, 
		XMVector3TransformNormal(Get_State(CTransform::STATE_LOOK), RotationMatrix));
}

void CTransform::Turn(_fvector vAxis, _float fSpeed, _float fTimeDelta)
{
	// vAxis를 축으로 fSpeed * fTimeDelta 만큼 회전하는 회전 행렬을 만든다.
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fSpeed * fTimeDelta);

	// 월드 행렬에 회전 행렬 적용.
	Set_State(CTransform::STATE_RIGHT, 
		XMVector3TransformNormal(Get_State(CTransform::STATE_RIGHT), RotationMatrix));
	Set_State(CTransform::STATE_UP, 
		XMVector3TransformNormal(Get_State(CTransform::STATE_UP), RotationMatrix));
	Set_State(CTransform::STATE_LOOK, 
		XMVector3TransformNormal(Get_State(CTransform::STATE_LOOK), RotationMatrix));
}

void CTransform::Turn(_fvector StartLook, _fvector EndLook, _float _fRatio)
{
	_vector vLook, vRight;
	vLook = (XMVector3Normalize(StartLook) * (1.f - _fRatio)) + (XMVector3Normalize(EndLook)*_fRatio);

	vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook));
	vLook = XMVector3Normalize(XMVector3Cross(vRight, XMVectorSet(0.f, 1.f, 0.f, 0.f)));

	_float3      vScale = Get_Scale();

	Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	// Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(60.0f));

	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);
	_float3		Scale = Get_Scale();

	Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(XMVectorSet(1.f, 0.f, 0.f, 0.f) * Scale.x, RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector3TransformNormal(XMVectorSet(0.f, 1.f, 0.f, 0.f) * Scale.y, RotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(XMVectorSet(0.f, 0.f, 1.f, 0.f) * Scale.z, RotationMatrix));
}

void CTransform::LookAt(_fvector vAt, _bool bInv)
{
	_vector		vLook;
	if (!bInv)
		vLook = vAt - Get_State(CTransform::STATE_POSITION);
	else
		vLook = Get_State(CTransform::STATE_POSITION) - vAt;
	

	 vLook = XMVectorSetW(vLook, 0.f);
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	_vector		vUp = XMVector3Cross(vLook, vRight);

	_float3		vScale = Get_Scale();

	Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CTransform::LookAt_ForLandObject(_fvector vAt, _bool bInv)
{
	
	_vector		vLook; 
	if(!bInv)
		vLook = vAt - Get_State(CTransform::STATE_POSITION);
	else
		vLook = Get_State(CTransform::STATE_POSITION) - vAt;

	_float3		vScale = Get_Scale();

	_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;	

	vLook = XMVector3Normalize(XMVector3Cross(vRight, Get_State(CTransform::STATE_UP))) * vScale.z;

	Set_State(CTransform::STATE_RIGHT, vRight);
	Set_State(CTransform::STATE_LOOK, vLook);

}

_bool  CTransform::Move(_fvector vTargetPos, _float fSpeed, _float fTimeDelta, _float fLimitDistance, _bool bLimitPass)
{
	_vector		vPosition = Get_State(CTransform::STATE_POSITION);
	_vector		vDirection = vTargetPos - vPosition;

	_float		fDistance = XMVectorGetX(XMVector3Length(vDirection));

	if (fDistance > fLimitDistance)
		vPosition += XMVector3Normalize(vDirection) * fSpeed * fTimeDelta;
	else
	{
		if (bLimitPass)
		{
			vPosition += XMVector3Normalize(vDirection) * fSpeed * fTimeDelta;
			Set_State(CTransform::STATE_POSITION, vPosition);
			return true;
		}
		else
			return true;
	}


	Set_State(CTransform::STATE_POSITION, vPosition);
	return false;
}

_float CTransform::TargetDistance(_fvector vTargetPos)
{
	_vector vPos = Get_State(CTransform::STATE_POSITION);

	_vector vLook = vPos - vTargetPos;

	_vector vDistance = XMVector3Length(vLook);


	return XMVectorGetX(vDistance);
}

_bool CTransform::Check_LimitDistance(_fvector vTargetPos, _float fDistance)
{
	_vector vPos = Get_State(CTransform::STATE_POSITION);

	_vector vLook = vTargetPos - vPos;

	_vector	vDistance = XMVector3Length(vLook);

	if (XMVectorGetX(vDistance) <= fDistance)
		return true;
	else
		return false;
}

_bool CTransform::Check_TargetFront(_fvector vTargetPos)
{
	//내 원래 룩벡터와 타겟포스에서 내포스 뺀 방향벡터를 내적해야됨

	_vector vOriLook = Get_State(CTransform::STATE_LOOK);
	_vector vPos = Get_State(CTransform::STATE_POSITION);

	_vector vTargetLook = XMVector3Normalize(vTargetPos - vPos);

	_float fAngle = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vOriLook), vTargetLook));

	if (fAngle > 0) //각도가 0보다 크면 앞에있고 
		return true;
	else 
		return false;

}

void CTransform::Push(_fvector vMyPos, _float fMyRad, _fvector vOtherPos, _float fOtherRad, _float fTimeDelta, CNavigation * pNavigation)
{
	_vector vDir = vMyPos - vOtherPos;

	_float fDis = XMVectorGetX(XMVector3Length(vDir));
	_float fRadDis = fMyRad + fOtherRad;

	_float fDipDis = fRadDis - fDis;
	if (0.f > fDipDis)
		return;

	fDipDis *= 0.5f;
	vDir = XMVector3Normalize(vDir);

	_vector vPosition = Get_State(CTransform::STATE_POSITION);
	vPosition += vDir * fDipDis;

	_vector vNormal = { 0.f,0.f,0.f,0.f };

	if (pNavigation->isMove(vPosition, &vNormal) == true)
		Set_State(CTransform::STATE_POSITION, vPosition);
	else
	{
		if (XMVectorGetX(XMVector3Length(vNormal)) == 0)
			return;
		_vector      vPosition = Get_State(CTransform::STATE_POSITION);
		_vector vSlide = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDir) * (-1.f), vNormal)) * vNormal;
		vSlide += XMVector3Normalize(vDir);
		vPosition += vSlide * m_TransformDesc.fSpeedPerSec * fTimeDelta;
		Set_State(CTransform::STATE_POSITION, vPosition);
	}


}

void CTransform::PushXZ(_fvector vMyPos, _float fMyRad, _fvector vOtherPos, _float fOtherRad, _float fTimeDelta, CNavigation * pNavigation)
{
	_vector vDir = vMyPos - vOtherPos;

	_float fDis = XMVectorGetX(XMVector3Length(vDir));
	_float fRadDis = fMyRad + fOtherRad;

	_float fDipDis = fRadDis - fDis;
	if (0.f > fDipDis)
		return;

	fDipDis *= 0.5f;

	vDir = XMVector3Normalize(XMVectorSetY(vDir, 0.f));

	_vector vPosition = Get_State(CTransform::STATE_POSITION);
	vPosition += vDir * fDipDis;

	_vector vNormal = { 0.f,0.f,0.f,0.f };

	if (pNavigation->isMove(vPosition, &vNormal) == true)
		Set_State(CTransform::STATE_POSITION, vPosition);
	else
	{
		if (XMVectorGetX(XMVector3Length(vNormal)) == 0)
			return;
		_vector      vPosition = Get_State(CTransform::STATE_POSITION);
		_vector vSlide = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDir) * (-1.f), vNormal)) * vNormal;
		vSlide += XMVector3Normalize(vDir);
		vPosition += vSlide * m_TransformDesc.fSpeedPerSec * fTimeDelta;
		Set_State(CTransform::STATE_POSITION, vPosition);
	}

}

void CTransform::PushMe(_fvector vMyPos, _float fMyRad, _fvector vOtherPos, _float fOtherRad, _float fTimeDelta, CNavigation * pNavigation)
{
	_vector vDir = vMyPos - vOtherPos;

	_float fDis = XMVectorGetX(XMVector3Length(vDir));
	_float fRadDis = fMyRad + fOtherRad;

	_float fDipDis = fRadDis - fDis;
	if (0.f > fDipDis)
		return;

	vDir = XMVector3Normalize(vDir);

	_vector vPosition = Get_State(CTransform::STATE_POSITION);
	vPosition += vDir * fDipDis;


	_vector vNormal = { 0.f,0.f,0.f,0.f };

	if (pNavigation->isMove(vPosition, &vNormal) == true)
		Set_State(CTransform::STATE_POSITION, vPosition);
	else
	{
		if (XMVectorGetX(XMVector3Length(vNormal)) == 0)
			return;
		_vector      vPosition = Get_State(CTransform::STATE_POSITION);
		_vector vSlide = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDir) * (-1.f), vNormal)) * vNormal;
		vSlide += XMVector3Normalize(vDir);
		vPosition += vSlide * m_TransformDesc.fSpeedPerSec * fTimeDelta;
		Set_State(CTransform::STATE_POSITION, vPosition);
	}

}

void CTransform::PushOther(_fvector vMyPos, _float fMyRad, _fvector vOtherPos, _float fOtherRad, _float fTimeDelta, CNavigation * pNavigation)
{
	_vector vDir = vMyPos - vOtherPos;

	_float fDis = XMVectorGetX(XMVector3Length(vDir));
	_float fRadDis = fMyRad + fOtherRad;

	_float fDipDis = fRadDis - fDis;
	if (0.f > fDipDis)
		return;

	vDir = XMVector3Normalize(vDir);

	_vector vPosition = Get_State(CTransform::STATE_POSITION);
	vPosition += vDir * fDipDis * 0.5f;

	_vector vNormal = { 0.f,0.f,0.f,0.f };

	if (pNavigation->isMove(vPosition, &vNormal) == true)
		Set_State(CTransform::STATE_POSITION, vPosition);
	else
	{
		if (XMVectorGetX(XMVector3Length(vNormal)) == 0)
			return;
		_vector      vPosition = Get_State(CTransform::STATE_POSITION);
		_vector vSlide = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDir) * (-1.f), vNormal)) * vNormal;
		vSlide += XMVector3Normalize(vDir);
		vPosition += vSlide * m_TransformDesc.fSpeedPerSec * fTimeDelta;
		Set_State(CTransform::STATE_POSITION, vPosition);
	}

}

void CTransform::PushMeXZ(_fvector vMyPos, _float fMyRad, _fvector vOtherPos, _float fOtherRad, _float fTimeDelta, CNavigation * pNavigation)
{
	_vector vDir = vMyPos - vOtherPos;
	_float fDis = XMVectorGetX(XMVector3Length(vDir));

	_float fRadDis = fMyRad + fOtherRad;

	_float fDipDis = fRadDis - fDis;
	if (0.f > fDipDis)
		return;

	vDir = XMVector3Normalize(XMVectorSetY(vDir, 0.f));

	_vector vPosition = Get_State(CTransform::STATE_POSITION);
	vPosition += vDir * fDipDis;

	_vector vNormal = { 0.f,0.f,0.f,0.f };

	if (pNavigation)
	{
		if (pNavigation->isMove(vPosition, &vNormal) == true)
			Set_State(CTransform::STATE_POSITION, vPosition);
		else
		{
			if (XMVectorGetX(XMVector3Length(vNormal)) == 0)
				return;
			_vector      vPosition = Get_State(CTransform::STATE_POSITION);
			_vector vSlide = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDir) * (-1.f), vNormal)) * vNormal;
			vSlide += XMVector3Normalize(vDir);
			vPosition += vSlide * m_TransformDesc.fSpeedPerSec * fTimeDelta;
			Set_State(CTransform::STATE_POSITION, vPosition);
		}
	}
	else
	{
		Set_State(CTransform::STATE_POSITION, vPosition);
	}



}

void CTransform::PushOtherXZ(_fvector vMyPos, _float fMyRad, _fvector vOtherPos, _float fOtherRad, _float fTimeDelta, CNavigation * pNavigation)
{
	_vector vDir = vMyPos - vOtherPos;

	_float fDis = XMVectorGetX(XMVector3Length(vDir));
	_float fRadDis = fMyRad + fOtherRad;

	_float fDipDis = fRadDis - fDis;
	if (0.f > fDipDis)
		return;

	vDir = XMVector3Normalize(XMVectorSetY(vDir, 0.f));

	_vector vPosition = Get_State(CTransform::STATE_POSITION);
	vPosition += vDir * fDipDis * 0.5f;

	_vector vNormal = { 0.f,0.f,0.f,0.f };

	if (pNavigation->isMove(vPosition, &vNormal) == true)
		Set_State(CTransform::STATE_POSITION, vPosition);
	else
	{
		if (XMVectorGetX(XMVector3Length(vNormal)) == 0)
			return;
		_vector      vPosition = Get_State(CTransform::STATE_POSITION);
		_vector vSlide = XMVectorGetX(XMVector3Dot(XMVector3Normalize(vDir) * (-1.f), vNormal)) * vNormal;
		vSlide += XMVector3Normalize(vDir);
		vPosition += vSlide * m_TransformDesc.fSpeedPerSec * fTimeDelta;
		Set_State(CTransform::STATE_POSITION, vPosition);
	}

}


// For Gravity
void CTransform::Tick_Gravity(_float fTimeDelta, CNavigation* pNavigation, _float fGravity, _bool bCheckNano)
{
	_float3 vPos;
	XMStoreFloat3(&vPos, Get_State(CTransform::STATE_POSITION));

	_float fCellY = 0.f;

	if (nullptr != pNavigation)
	{
		// fGravity *= 1.04f;

		_bool bIsGround = pNavigation->isGround(Get_State(CTransform::STATE_POSITION), &fCellY);
		if(bCheckNano && (0.05f > abs(fCellY - XMVectorGetY(Get_State(CTransform::STATE_POSITION)))))
			vPos.y = fCellY;
		
		if (bIsGround && (0.01f > m_fVelocity))
		{
			m_fGravityAcc = 0.f;
			m_fVelocity = 0.f;
			// 여기서 태운다.
			m_bAir = false;
			vPos.y = fCellY;
		}
		else
		{
			m_fGravityAcc += fGravity * fTimeDelta;
			/*if (0.5f < m_fGravityAcc)
				m_fGravityAcc = 15.f;*/
			m_bAir = true;
			m_fVelocity -= m_fGravityAcc * fTimeDelta;
		}
	}
	else
	{
		m_fGravityAcc += fGravity * fTimeDelta;
		/*if (0.5f < m_fGravityAcc)
			m_fGravityAcc = 15.f;*/
		m_fVelocity -= m_fGravityAcc * fTimeDelta;
	}

	vPos.y += m_fVelocity * fTimeDelta;

	Set_State(STATE_POSITION, XMVectorSetW(XMLoadFloat3(&vPos), 1.f));
	
}
void CTransform::Jump(_float fPower)
{
	m_fVelocity += fPower;

}
void CTransform::ResetGravity()
{
	m_fGravityAcc = 0.f;
	m_fVelocity = 0.f;
}

void CTransform::ResetAndJump(_float fPower)
{
	m_fGravityAcc = 0.f;
	m_fVelocity = fPower;
}



CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTransform*			pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTransform"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform*			pInstance = new CTransform(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTransform"));
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CTransform::Free()
{
	__super::Free();
}
