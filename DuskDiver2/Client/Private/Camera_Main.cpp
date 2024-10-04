#include "stdafx.h"
#include "..\Public\Camera_Main.h"
#include "GameInstance.h"
#include "Player.h"
#include "Player_Manager.h"
#include "Camera_Manager.h"
#include "ToolManager.h"
#include "Monster_Manager.h"
#include "Creture.h"

CCamera_Main::CCamera_Main(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Main::CCamera_Main(const CCamera_Main & rhs, CTransform::TRANSFORMDESC * pArg)
	: CCamera(rhs, pArg)
{

}

HRESULT CCamera_Main::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Main::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;
	m_sTag = "Tag_Cam";

	Make_ImGui();

	m_vDistance = { 0.f,1.2f,-3.5f };
	m_vOriDistance = { 0.f,1.2f,-3.5f };
	CRM->Add_CameraMain(this);

	ZeroMemory(&m_vPrePos, sizeof(_float4));
	ZeroMemory(&m_vPreLook, sizeof(_float4));


	return S_OK;
}

void CCamera_Main::Tick(_float fTimeDelta)
{
	switch (m_eCamMode)
	{
	case Client::CCamera_Main::CAM_PLAYER:
	{
		if (m_bIsCutScene)
		{
			__super::Tick(fTimeDelta);
			Tick_Shake(fTimeDelta);
			Tick_Fov(fTimeDelta);
		}
		else
			Tick_Player(fTimeDelta);
	}
		break;
	case Client::CCamera_Main::CAM_DEBUG:
		Tick_Debug(fTimeDelta);
		break;
	}
}

void CCamera_Main::LateTick(_float fTimeDelta)
{
	
}

HRESULT CCamera_Main::Render()
{
	return S_OK;
}


void CCamera_Main::Set_CamMode()
{

	if (m_eCamMode == CAM_DEBUG)
	{
		m_eCamMode = CAM_PLAYER;
		m_pTransformCom->LookAt(XMLoadFloat4(&PM->Get_PlayerPos()) + _vector{ 0.f,4.f,0.f,0.f });
	}
	else if (m_eCamMode == CAM_PLAYER)
	{
		m_eCamMode = CAM_DEBUG;
	}
}

_float3 CCamera_Main::Get_CamPos()
{
	_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	return vPos;
}

_float3 CCamera_Main::Get_CamLook()
{
	_float3 vPos; XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	return vPos;
}

_float3 CCamera_Main::Get_CamBackPos(_float fRightDis, _float fUpDis, _float fLooDis)
{
	_float3 vPos;
	XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + 
		XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)) * fRightDis +
		XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP)) * fUpDis +
		XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)) * fLooDis);
	return vPos;
}

void CCamera_Main::Set_CamPos(_float vUp, _float vRight)
{

	//_matrix		RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(vUp));
	//RotationMatrix *= XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(vRight));

/*
	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(vUp));
	m_pTransformCom->Turn(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(vRight));
*/
	m_pTransformCom->RotationThree(_float3(1.f, 0.f, 0.f), vRight, _float3(0.f, 1.f, 0.f), vUp, _float3(0.f, 0.f, 1.f), 0.f);

	// World Pos 값 구하기
	_vector vLerpPos = XMLoadFloat4(&PM->Get_PlayerPos());
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vLerpPos);
	XMStoreFloat4(&m_vPrePos, vLerpPos);


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMVector3TransformCoord(XMLoadFloat3(&m_vDistance), m_pTransformCom->Get_WorldMatrix()), 1.f));
	XMStoreFloat4(&m_vPreLook, XMLoadFloat4(&PM->Get_PlayerPos()));

}

void CCamera_Main::Set_Pos(_float3 vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&vPos), 1.f));
}

_bool CCamera_Main::Move(_fvector vTargetPos, _float fSpeed, _float fTimeDelta, _float fLimitDistance, _bool bLimitPass)
{
	_bool b = m_pTransformCom->Move(vTargetPos, fSpeed, fTimeDelta, fLimitDistance, bLimitPass);
	XMStoreFloat4(&m_vPrePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	return b;
}

void CCamera_Main::LookAt(_fvector vTargetPos)
{
	m_pTransformCom->LookAt(vTargetPos);
	XMStoreFloat4(&m_vPreLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
}

void CCamera_Main::ResetPrePos()
{
	XMStoreFloat4(&m_vPrePos, XMLoadFloat4(&PM->Get_PlayerPos()));
	LookAt(XMLoadFloat4(&PM->Get_PlayerPos()));
}


void CCamera_Main::Tick_Debug(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	if (!TM->Get_CamaraMove())
		return;

	// Pos 이동
	if (K_PRESS(DIK_W))
		m_pTransformCom->Go_Straight(fTimeDelta);
	if (K_PRESS(DIK_S))
		m_pTransformCom->Go_Backward(fTimeDelta);
	if (K_PRESS(DIK_A))
		m_pTransformCom->Go_Left(fTimeDelta);
	if (K_PRESS(DIK_D))
		m_pTransformCom->Go_Right(fTimeDelta);

	// 회전
	_long	MouseMove = 0;
	if (MouseMove = GI->Get_DIMMoveState(DIMM_X))
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 
			MouseMove, fTimeDelta * 0.05f);
	if (MouseMove = GI->Get_DIMMoveState(DIMM_Y))
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT),
			MouseMove, fTimeDelta * 0.05f);
}



void CCamera_Main::LateTick_Player(_float fTimeDelta)
{
}

void CCamera_Main::LateTick_Debug(_float fTimeDelta)
{
}


CCamera_Main * CCamera_Main::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Main*		pInstance = new CCamera_Main(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCamera_Main"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Main::Clone(void* pArg)
{
	CCamera::CAMERADESC*		pCameraDesc = (CCamera::CAMERADESC*)pArg;

	CCamera_Main*		pInstance = new CCamera_Main(*this, &pCameraDesc->TransformDesc);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Created : CTerrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Main::Free()
{
	__super::Free();
}






// For. GameCam
void CCamera_Main::Shake(_float fPow, _float fTime, _bool bIsLerp, _float fTurnTime)
{
	if (m_fShakePow > fPow)
		return;

	m_bShake = true;
	m_bIsLerp = bIsLerp;
	m_fShakePow = fPow;
	m_fShakeTime = fTime;
	m_fShakeTimeAcc = 0.f;

	if (m_bIsLerp)
	{
		m_vLepDir.x = GI->Get_FloatRandom(-1.f, 1.f);
		m_vLepDir.y = GI->Get_FloatRandom(-1.f, 1.f);
		m_vLepDir.z = GI->Get_FloatRandom(-1.f, 1.f);

		m_fShakePowAcc = m_fShakePow;
		m_fShakeTurnTime = fTurnTime;
		m_fShakeTurnTimeAcc = 0.f;
	}
}

void CCamera_Main::Fov(_float fZoomPower, _float fZoomTime, _float fZoomStopTime, _bool bReset, _bool bSoothReset)
{
	m_bIsZoom = true;
	m_bIsZoomStop = false;
	m_fZoomPower = fZoomPower;
	m_fZoomTime = fZoomTime;
	m_fZoomStopTime = fZoomStopTime;

	m_bReset = bReset;
	m_bSmoothReset = bSoothReset;
	m_fZoomTimeAcc = 0.f;
}

void CCamera_Main::Target(CTransform * pTran)
{
	m_pTargetTran = pTran;
	if(m_pTargetTran)
		m_ePCamMode = PCAM_TARGET;
	else
		m_ePCamMode = PCAM_PLAYER;
}

void CCamera_Main::Show_Target(CUTDESC Desc)
{
	m_ePCamMode = PCAM_CUTSCENE;
	m_pTargetTran = Desc.pTran;
	m_vPosDis = Desc.vPosDis;
	m_vShowPos = Desc.vPos;
	m_vShowLook = Desc.vLook;
	m_bLerp = Desc.bLerp;
	m_bLookLerp = Desc.bLookLerp;
	m_bPixLocal = Desc.bPixLocal;

	m_fCutPosLerpSpeed = Desc.fLerpSpeed;
	m_fCutLookLerpSpeed = Desc.fLookLerpSpeed;

	m_vPivotLookPos = Desc.vPivotLookPos;
}
void CCamera_Main::End_Target()
{
	m_ePCamMode = PCAM_PLAYER;
	m_pTargetTran = nullptr;
}

void CCamera_Main::Hold_Pos(_float3 vPos, class CTransform* pTran_L, class CTransform* pTran_R, _float3 vPos_R , _float fLimitDis)
{
	m_ePCamMode = PCAM_HOLD;
	m_vHoldPos = vPos;
	m_pHoldTarget_L = pTran_L;
	m_pHoldTarget_R = pTran_R;
	m_vPos_R = vPos_R;
	m_fHoldLimitDis = fLimitDis;

	m_vPrePos.x = m_vHoldPos.x;
	m_vPrePos.y = m_vHoldPos.y;
	m_vPrePos.z = m_vHoldPos.z;
	m_vPrePos.w = 1.f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vHoldPos), 1.f));

}

void CCamera_Main::End_Hold()
{
	m_ePCamMode = PCAM_PLAYER;
	m_pHoldTarget_L = nullptr;
	m_pHoldTarget_R = nullptr;
}

void CCamera_Main::Edit_CamDis(_float3 vDis)
{
	m_vDistance = vDis;
}

void CCamera_Main::End_CamDis()
{
	m_vDistance = m_vOriDistance;
}

void CCamera_Main::Save_World()
{
	XMStoreFloat4x4(&m_SaveWorld, m_pTransformCom->Get_WorldMatrix());
}

void CCamera_Main::Load_World()
{
	XMStoreFloat4(&m_vPrePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	XMStoreFloat4(&m_vPlayerPrePos, XMLoadFloat4(&PM->Get_PlayerPos()));
	XMStoreFloat4(&m_vPreLook, XMLoadFloat4(&PM->Get_PlayerPos()));

	 m_pTransformCom->Set_WorldMatrix(m_SaveWorld);
}





void CCamera_Main::Tick_Player(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	switch (m_ePCamMode)
	{
	case PCAM_PLAYER:
		Input_Target(fTimeDelta);
		Tick_PlayerChase(fTimeDelta);
		break;
	case PCAM_TARGET:
		Input_Target(fTimeDelta);
		Tick_TargetChase(fTimeDelta);
		break;
	case PCAM_CUTSCENE:
		Tick_TargetShow(fTimeDelta);
		break;
	case PCAM_HOLD:
		Tick_HoldTick(fTimeDelta);
		break;
	default:
		break;
	}

	Tick_Shake(fTimeDelta); // 쉐이킹 함수
	Tick_Fov(fTimeDelta);	// 줌 인, 아웃 함수
}



void CCamera_Main::Tick_PlayerChase(_float fTimeDelta)
{
	// 마우스 변화량에 따른 회전행렬
	_long	MouseMove = 0;
	if (MouseMove = GI->Get_DIMMoveState(DIMM_X))
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * fTimeDelta * 0.05f);
	if (MouseMove = GI->Get_DIMMoveState(DIMM_Y))
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), MouseMove * fTimeDelta * 0.05f);

	// 보간된 Player 포지션 적용
	_vector vLerpPos = TM->Lerp_Pos(XMLoadFloat4(&m_vPlayerPrePos), 
		XMLoadFloat4(&PM->Get_PlayerPos()), fTimeDelta * m_fLerpSpeed);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vLerpPos);
	XMStoreFloat4(&m_vPlayerPrePos, vLerpPos);

	// m_vDistance(원점기준 떨어질 위치)과 위에서 구한 행렬을 곱해 최종 위치 적용
	_vector vDestPos = XMVector3TransformCoord(XMLoadFloat3(&m_vDistance), m_pTransformCom->Get_WorldMatrix());
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vDestPos, 1.f));
#pragma region 중략
	vLerpPos = TM->Lerp_Pos(XMLoadFloat4(&m_vPrePos), m_pTransformCom->Get_State(CTransform::STATE_POSITION), fTimeDelta * m_fLerpSpeed * 3.5f);  
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vLerpPos);
	XMStoreFloat4(&m_vPrePos, vLerpPos);

	XMStoreFloat4(&m_vPreLook, XMLoadFloat4(&PM->Get_PlayerPos()));
#pragma endregion

}

void CCamera_Main::Tick_TargetChase(_float fTimeDelta)
{
	// Player 코드와 동일
#pragma region 중략 - 마우스 변화량 회전행렬, Player 포지션 적용 코드

	if (!m_pTargetTran)
	{
		m_ePCamMode = PCAM_PLAYER;
		return;
	}

	// 마우스 변화량에 따른 회전행렬
	_long	MouseMove = 0;
	if (MouseMove = GI->Get_DIMMoveState(DIMM_X))
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), MouseMove * fTimeDelta * 0.05f);
	if (MouseMove = GI->Get_DIMMoveState(DIMM_Y))
		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), MouseMove * fTimeDelta * 0.05f);

	// 보간된 Player 포지션 적용
	_vector vLerpPos = TM->Lerp_Pos(XMLoadFloat4(&m_vPlayerPrePos),
		XMLoadFloat4(&PM->Get_PlayerPos()), fTimeDelta * m_fLerpSpeed);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vLerpPos);
	XMStoreFloat4(&m_vPlayerPrePos, vLerpPos);

	// m_vDistance(원점기준 떨어질 위치)과 위에서 구한 행렬을 곱해 최종 위치 적용
	_vector vDestPos = XMVector3TransformCoord(XMLoadFloat3(&m_vDistance), m_pTransformCom->Get_WorldMatrix());
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vDestPos, 1.f));

	vLerpPos = TM->Lerp_Pos(XMLoadFloat4(&m_vPrePos), m_pTransformCom->Get_State(CTransform::STATE_POSITION), fTimeDelta * m_fLerpSpeed * 3.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vLerpPos);
	XMStoreFloat4(&m_vPrePos, vLerpPos);

#pragma endregion

	_vector vPlayerPos = XMLoadFloat4(&PM->Get_PlayerPos());
	_vector vTargetPos = m_pTargetTran->Get_State(CTransform::STATE_POSITION);

	// 플레이어 위치와 타겟 위치를 90% 타겟 위치에 가깝게 보간한다.
	_vector vMiddlePos = TM->Lerp_PosByT(vPlayerPos, vTargetPos, 0.9f);
	_vector vDestLook = vMiddlePos;
	_vector vLerpLook = TM->Lerp_Pos(XMLoadFloat4(&m_vPreLook), vDestLook, 0.1f);
	XMStoreFloat4(&m_vPreLook,  vLerpLook);

	// 해당 좌표를 바라본다.
	m_pTransformCom->LookAt(vLerpLook);
}

void CCamera_Main::Tick_TargetShow(_float fTimeDelta)
{
	if (1.f / 60.f < fTimeDelta)
		fTimeDelta = 1.f / 60.f;
	// Pos
	_vector vCurPos;
	if (!m_pTargetTran)
		vCurPos = XMLoadFloat3(&m_vShowPos);
	else
	{
		if(m_bPixLocal)
			vCurPos = XMVectorSetW(XMVector3TransformCoord(XMLoadFloat3(&m_vPosDis), m_pTargetTran->Get_WorldMatrix()), 1.f);
		else
			vCurPos = m_pTargetTran->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&m_vPosDis); 
	}
	vCurPos = XMVectorSetW(vCurPos, 1.f);

	_vector vLerpPos;
	if (m_bLerp)
		vLerpPos = TM->Lerp_Pos(XMLoadFloat4(&m_vPrePos), vCurPos, fTimeDelta * m_fCutPosLerpSpeed);
	else
		vLerpPos = vCurPos;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vLerpPos);
	XMStoreFloat4(&m_vPrePos, vLerpPos);
	XMStoreFloat4(&m_vPlayerPrePos, XMLoadFloat4(&PM->Get_PlayerPos()));



	// Look
	_vector vLookPos;
	// 목표가 있으면 목표 포스 목적지, 아니면 설정한거
	if (!m_pTargetTran) // 설정 Look
		vLookPos = XMLoadFloat3(&m_vShowLook) + XMLoadFloat3(&m_vPivotLookPos);
	else // 목표 Look
	{
		if (m_bPixLocal)
		{
			vLookPos = m_pTargetTran->Get_State(CTransform::STATE_POSITION) +
				XMVector3Normalize(m_pTargetTran->Get_State(CTransform::STATE_RIGHT)) * m_vPivotLookPos.x +
				XMVector3Normalize(m_pTargetTran->Get_State(CTransform::STATE_UP)) * m_vPivotLookPos.y +
				XMVector3Normalize(m_pTargetTran->Get_State(CTransform::STATE_LOOK)) * m_vPivotLookPos.z;
		}

		else
			vLookPos = m_pTargetTran->Get_State(CTransform::STATE_POSITION) + XMLoadFloat3(&m_vPivotLookPos);
	}
	vLookPos = XMVectorSetW(vLookPos, 1.f);
		
	_vector vLerpLook;
	if (m_bLookLerp)
		vLerpLook = TM->Lerp_Pos(XMLoadFloat4(&m_vPreLook), vLookPos, fTimeDelta * m_fCutLookLerpSpeed);
	else
		vLerpLook = vLookPos;

	m_pTransformCom->LookAt(vLerpLook);



	XMStoreFloat4(&m_vPreLook, vLerpLook);

}

void CCamera_Main::Tick_HoldTick(_float fTimeDelta)
{
	if (!m_pHoldTarget_L)
	{
		m_ePCamMode = PCAM_PLAYER;
		return;
	}


	// 홀드 지점과 목표트렌의 거리를 잰다
	_vector vHoldPos = m_pHoldTarget_L->Get_State(CTransform::STATE_POSITION);

	_vector vDestPos;
	if (m_pHoldTarget_R)
		vDestPos = m_pHoldTarget_R->Get_State(CTransform::STATE_POSITION);
	else
		vDestPos = XMLoadFloat3(&m_vPos_R);

	// World Pos 값 구하기
  	_vector vDestLook = TM->Lerp_PosByT(vHoldPos, vDestPos, m_fHoldLimitDis);


	_vector vLerpLook = TM->Lerp_Pos(XMLoadFloat4(&m_vPreLook), vDestLook, fTimeDelta * 2.f);
	m_pTransformCom->LookAt(XMVectorSetW(vLerpLook, 1.f));

	XMStoreFloat4(&m_vPreLook, vLerpLook);

}

void CCamera_Main::Tick_Shake(_float fTimeDelta)
{
	if (!m_bShake)
		return;


	m_fShakeTimeAcc += fTimeDelta;
	if (m_fShakeTime < m_fShakeTimeAcc)
	{
		m_fShakePow = 0.f;
		m_fShakeTime = 0.f;
		m_fShakeTimeAcc = 0.f;
		m_bShake = false;
	}


	if (!m_bIsLerp)
	{
		// 랜덤 벡터를 꺼내온다.
		_float3 vDir;
		vDir.x = GI->Get_FloatRandom(-1.f, 1.f);
		vDir.y = GI->Get_FloatRandom(-1.f, 1.f);
		vDir.z = GI->Get_FloatRandom(-1.f, 1.f);

		_vector vD = XMVector3Normalize(XMLoadFloat3(&vDir));

		_vector vCamPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		vCamPos += vD * m_fShakePow;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPos);
	}
	else
	{
		_vector vD = XMVector3Normalize(XMLoadFloat3(&m_vLepDir));
		_vector vCamPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		m_fShakeTurnTimeAcc += fTimeDelta;
		if (m_fShakeTurnTime < m_fShakeTurnTimeAcc)
		{
			m_fShakeTurnTimeAcc = 0.f;
			
			m_vLepDir.x = GI->Get_FloatRandom(-1.f, 1.f);
			m_vLepDir.y = GI->Get_FloatRandom(-1.f, 1.f);
			m_vLepDir.z = GI->Get_FloatRandom(-1.f, 1.f);
		}

		vCamPos += vD * m_fShakePow;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCamPos);
	}

}

void CCamera_Main::Tick_Fov(_float fTimeDelta)
{
	if (!m_bIsZoom)
		return;

	m_fZoomTimeAcc += fTimeDelta;
	if (m_fZoomTime < m_fZoomTimeAcc && !m_bIsZoomStop)
	{
		m_bIsZoomStop = true;
		m_fZoomTimeAcc = 0.f;
	}
	else if(!m_bIsZoomStop)
	{
		m_CameraDesc.fFovy += m_fZoomPower * fTimeDelta;
	}
	else if(m_fZoomStopTime < m_fZoomTimeAcc && m_bIsZoomStop)
	{
		if (m_bSmoothReset)
		{  
			// - : 인  + : 아웃
			_float fDis = m_CameraDesc.fFovy - m_fOriFov;
			m_CameraDesc.fFovy += -fDis * fTimeDelta;

			if (0.01f > fDis && -0.01f < fDis)
				m_bSmoothReset = false;

		}
		else
		{
			m_bIsZoom = false;
			m_bIsZoomStop = false;
			m_bSmoothReset = false;

			m_fZoomTimeAcc = 0.f;

			if(m_bReset)
				m_CameraDesc.fFovy = m_fOriFov;
		}
	}

}

void CCamera_Main::Input_Target(_float fTimeDelta)
{
	if (CM_DOWN(DIMK_RBUTTON))
	{
		CCreture* pCre = MM->Find_NearstMonster();
		if (pCre)
			Target((CTransform*)pCre->Get_ComponentPtr(TEXT("Com_Transform")));
		else
			Target(nullptr);
	}
	else if (CM_UP(DIMK_RBUTTON))
		Target(nullptr);

}



void CCamera_Main::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);
	ImGui::InputFloat("LerpSpeed", &m_fLerpSpeed); 
	ImGui::InputFloat("ShakePow", &m_fShakePow);

	ImGui::InputFloat3("Distance", (_float*)&m_vDistance);

	ImGui::End();
}
