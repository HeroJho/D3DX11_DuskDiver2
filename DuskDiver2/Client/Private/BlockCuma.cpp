#include "stdafx.h"
#include "..\Public\BlockCuma.h"
#include "GameInstance.h"

#include "Camera_Manager.h"
#include "Camera_Main.h"
#include "Player_Manager.h"
#include "ToolManager.h"
#include "MiniGame_Block.h"
#include "UI_Manager.h"
#include "Player01SmokeBall.h"
#include "Particle_Manager.h"
#include "Light.h"

CBlockCuma::CBlockCuma(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CBlockCuma::CBlockCuma(const CBlockCuma & rhs)
	: CCreture(rhs)
{
}

HRESULT CBlockCuma::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBlockCuma::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Boss";
	Make_ImGui();

	m_fHurtTime = 0.02f;
	m_eHitType = TYPE_HIT;
	m_fAirPower = 2.f;
	m_fPushPower = 4.f;
	m_fDamage = 0.f;



	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-133.087f, 32.276f, 102.13f, 1.f));
	m_pTransformCom->RotationThree(_float3(1.f, 0.f, 0.f), 0.f, 
		_float3(0.f, 1.f, 0.f), 0.f,
		_float3(0.f, 0.f, 1.f), 0.f);
	m_pTransformCom->Set_Scale(XMVectorSet(10.f, 10.f, 10.f, 1.f));


	m_vHomePos = _float3(-133.087f, 33.276f, 102.13f);
	_vector vLook = XMVector3Normalize(XMVectorSet(-133.087f, 32.276f, 102.13f, 1.f) - XMVectorSet(-130.17f, 32.934f, 103.556f, 1.f));
	m_pTransformCom->Set_Look(XMVectorSetW(vLook, 0.f));

	Set_State(STATE_CUT);
	UM->Set_FrontUI(false);

	GI->Add_GameObjectToLayer(TEXT("CutBlock"), LEVEL_STAGE4, L"Mini_Block", nullptr);


	// 보조광원1
	LIGHTDESC LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eType = LIGHTDESC::TYPE_POINT;
	LightDesc.fRange = 15.f;
	XMStoreFloat4(&LightDesc.vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.7f, 0.7f, 0.7f, 1.f);
	LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 1.f);
	if (FAILED(GI->Add_Light(LightDesc, &m_pLight)))
		return E_FAIL;




	return S_OK;
}


void CBlockCuma::Set_State(STATE eState)
{
	if (eState == m_eState)
		return;

	switch (m_eState)
	{

	case STATE_CUT:
	{
		m_eState = STATE_IDLE;
		m_vHomePos = _float3(-138.7f, 31.276f, 96.696f);
		m_vHomePos.x += -4.f;
		m_vHomePos.z += -8.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vHomePos), 1.f));

		CRM->Get_Cam()->Hold_Pos(_float3(-128.4f, 47.375f, 80.1f), PM->Get_Transform(), nullptr, m_vHomePos, 0.7f);
	}
		break;
	case STATE_PUSE:
	{
		m_vHomePos = _float3(-138.7f, 31.276f, 96.696f);
		m_vHomePos.x += -4.f;
		m_vHomePos.z += -8.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vHomePos), 1.f));

		CRM->Get_Cam()->Hold_Pos(_float3(-128.4f, 47.375f, 80.1f), PM->Get_Transform(), nullptr, m_vHomePos, 0.7f);
	}
		break;
	case STATE_IDLE:
		break;
	case STATE_HIDE:
		break;
	default:
		break;
	}





	switch (eState)
	{
	case STATE_CUT:
		break;
	case STATE_IDLE:
		break;
	case STATE_HIDE: //방해 시작
	{
		GI->PlaySoundW(L"CMN_MINIGAME_BNK_SE_00000016.wav", SD_MAP, 1.f);
		m_fGoHomeTimeAcc = 0.f;
		m_bOutHide = false;
		m_bUpHide = false;
		m_bDownHide = false;
		m_bStayHide = false;
		m_fHideTime = 1.f;
		m_bLeftEndHide = false;
		m_bTemm = false;
	}
		break;
	case STATE_ENDCUT:
	{
		
	}
		break;
	default:
		break;
	}

	m_eState = eState;
}

void CBlockCuma::Shake()
{
	m_bShake = true;
	m_bShakeTurn = false;
	m_fShakeTimeAcc = 0.f;
	m_fShakeEndTimeAcc = 0.f;
	m_eFace = FACE_HURT;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vHomePos), 1.f));
}





void CBlockCuma::Tick(_float fTimeDelta)
{
	m_pLight->Get_LightDesc()->vPosition = Get_WorldPos();

	switch (m_eState)
	{
	case STATE_CUT:
		Tick_Cut(fTimeDelta);
		break;
	case STATE_PUSE:
		Tick_Puse(fTimeDelta);
		break;
	case STATE_IDLE:
		Tick_Idle(fTimeDelta);
		break;
	case STATE_HIDE:
		Tick_Hide(fTimeDelta);
		break;
	case STATE_ENDCUT:
		Tick_EndCut(fTimeDelta);
		break;
	default:
		break;
	}


	if (STATE_CUT != m_eState)
	{
		if (TM->Get_MiniGameBlock()->Get_Bome())
			m_eFace = FACE_SMILE;
		else
			m_eFace = FACE_IDLE;
	}




	if (m_bShake)
		Tick_Shake(fTimeDelta);
}



void CBlockCuma::Tick_Cut(_float fTimeDelta)
{
	m_fGoHomeTimeAcc += fTimeDelta;

	if (5.f < m_fGoHomeTimeAcc && 10.f > m_fGoHomeTimeAcc)
	{
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(-127.43f, 32.538f, 103.971f);
		XMStoreFloat3(&CamDesc.vLook, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		CamDesc.vLook.y += 1.f;
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 1.f;
		CamDesc.fLookLerpSpeed = 1.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);
		GI->PlaySoundW(L"CMN_MINIGAME_BNK_SE_0000001E.wav", SD_MAP, 1.f);
		m_fGoHomeTimeAcc = 11.f;
	}
	if (20.f < m_fGoHomeTimeAcc)
	{
		Set_State(STATE_IDLE);
		TM->Start_MiniGame_Block();
		UM->Set_BattleTalkIndex(20);
		UM->Set_FrontUI(true);
		return;
	}



	_float4 vPos = Get_WorldPos();
	if (vPos.y < m_vHomePos.y - 0.5f)
		m_bHoverTurn = true;
	else if (vPos.y > m_vHomePos.y + 0.5f)
		m_bHoverTurn = false;

	if (m_bHoverTurn)
		m_pTransformCom->Go_Dir(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.5f, fTimeDelta);

	else
		m_pTransformCom->Go_Dir(XMVectorSet(0.f, -1.f, 0.f, 0.f), 0.5f, fTimeDelta);

	_vector vCamPos = XMLoadFloat3(&CRM->Get_Cam()->Get_CamPos());

}

void CBlockCuma::Tick_Puse(_float fTimeDelta)
{
	_vector vPlayerPos = XMLoadFloat4(&PM->Get_PlayerPos());
	_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	
	_float fDis = XMVectorGetX(XMVector3Length(vPlayerPos - vMyPos));

	if (5.f > fDis)
	{
		if (CK_DOWN(DIK_C))
		{
			TM->Start_MiniGame_Block();
			Set_State(STATE_IDLE);
			return;
		}
	}

// ======================================================================================

	// 원래 자리로
	m_vHomePos = _float3(-138.7f, 31.276f, 96.696f);
	m_vHomePos.x += -4.f;
	m_vHomePos.z += -8.f;

	_float4 vPos = Get_WorldPos();
	if (vPos.y < m_vHomePos.y - 0.5f)
		m_bHoverTurn = true;
	else if (vPos.y > m_vHomePos.y + 0.5f)
		m_bHoverTurn = false;

	if (m_bHoverTurn)
		m_pTransformCom->Go_Dir(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.5f, fTimeDelta);

	else
		m_pTransformCom->Go_Dir(XMVectorSet(0.f, -1.f, 0.f, 0.f), 0.5f, fTimeDelta);

	_vector vCamPos = XMLoadFloat3(&CRM->Get_Cam()->Get_CamPos());
	m_pTransformCom->LookAt_ForLandObject(XMVectorSetW(vCamPos, 1.f), true);

}

void CBlockCuma::Tick_Idle(_float fTimeDelta)
{

	m_vHomePos = TM->Get_MiniGameBlock()->Get_InitPos();
	m_vHomePos.x += -4.f;
	m_vHomePos.z += -8.f;


	_float4 vPos = Get_WorldPos();
	if (vPos.y < m_vHomePos.y - 0.5f)
		m_bHoverTurn = true;
	else if (vPos.y > m_vHomePos.y + 0.5f)
		m_bHoverTurn = false;

	if (m_bHoverTurn)
		m_pTransformCom->Go_Dir(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.5f, fTimeDelta);

	else
		m_pTransformCom->Go_Dir(XMVectorSet(0.f, -1.f, 0.f, 0.f), 0.5f, fTimeDelta);

	_vector vCamPos = XMLoadFloat3(&CRM->Get_Cam()->Get_CamPos());
	m_pTransformCom->LookAt_ForLandObject(XMVectorSetW(vCamPos, 1.f), true);

}

void CBlockCuma::Tick_Hide(_float fTimeDelta)
{
	if (!m_bOutHide)
	{
		m_fGoHideTimeAcc += fTimeDelta;
		if (1.f < m_fGoHideTimeAcc)
		{
			m_bOutHide = true;
			m_fGoHideTimeAcc = 0.f;
			XMStoreFloat3(&m_vLeftPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			return;
		}
		m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 30.f, fTimeDelta);
		m_pTransformCom->Go_Dir(XMVector4Normalize(XMVectorSet(0.f, 0.f, -1.f, 0.f)), 20.f, fTimeDelta);
	}
	else if(!m_bTemm)
	{
		m_fGoHideTimeAcc += fTimeDelta;
		if (0.1f < m_fGoHideTimeAcc)
		{
			m_bTemm = true;
			m_bUpHide = true;
			m_fGoHideTimeAcc = 0.f;
			_vector vMyPos = XMLoadFloat3(&CRM->Get_Cam()->Get_CamBackPos(0.f, -10.f, 3.5f));
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vMyPos, 1.f));

			_vector vCamPos = XMLoadFloat3(&CRM->Get_Cam()->Get_CamPos());
			m_pTransformCom->LookAt_ForLandObject(XMVectorSetW(vCamPos, 1.f));
			m_pTransformCom->TurnAngle(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(40.f));
		}
	}

	if (m_bUpHide)
	{
	
		// 카메라 앞 포지션 (목적지)
		_vector vCamLookPos = XMLoadFloat3(&CRM->Get_Cam()->Get_CamBackPos(0.f, -2.f, 3.5f));
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vDisVec = vCamLookPos - vMyPos;
		_float fDis =  XMVectorGetX(XMVector3Length(vDisVec));

		if (0.5f > fDis)
		{
			Peek_A_Boo();
			m_bUpHide = false;
			m_bStayHide = true;
			m_fGoHideTimeAcc = 0.f;
			XMStoreFloat3(&m_vHomePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			return;
		}

		m_pTransformCom->Go_Dir(XMVectorSetW(XMVector3Normalize(vDisVec), 0.f), 10.f, fTimeDelta);

	}


	if (m_bStayHide)
	{
		m_fGoHideTimeAcc += fTimeDelta;
		if (m_fHideTime < m_fGoHideTimeAcc)
		{
			m_fGoHideTimeAcc = 0.f;
			m_bStayHide = false;
			m_bDownHide = true;
			return;
		}

		

		//_float4 vPos = Get_WorldPos();
		//if (vPos.y < m_vHomePos.y - 0.5f)
		//	m_bHoverTurn = true;
		//else if (vPos.y > m_vHomePos.y + 0.5f)
		//	m_bHoverTurn = false;

		//if (m_bHoverTurn)
		//	m_pTransformCom->Go_Dir(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.5f, fTimeDelta);
		//else
		//	m_pTransformCom->Go_Dir(XMVectorSet(0.f, -1.f, 0.f, 0.f), 0.5f, fTimeDelta);

		//_vector vCamPos = XMLoadFloat3(&CRM->Get_Cam()->Get_CamPos());
		//m_pTransformCom->LookAt(XMVectorSetW(vCamPos, 1.f), true);
	}


	if (m_bDownHide)
	{
		// 카메라 앞 포지션 (목적지)
		_vector vCamLookPos = XMLoadFloat3(&CRM->Get_Cam()->Get_CamBackPos(0.f, -10.f, 3.5f));
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vDisVec = vCamLookPos - vMyPos;
		_float fDis = XMVectorGetX(XMVector3Length(vDisVec));

		if (0.3f > fDis)
		{
			m_bDownHide = false;
			m_bLeftEndHide = true;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vLeftPos), 1.f));
			m_pTransformCom->TurnAngle(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(-40.f));
			return;
		}

		m_pTransformCom->Go_Dir(XMVectorSetW(XMVector3Normalize(vDisVec), 0.f), 10.f, fTimeDelta);
	}


	if (m_bLeftEndHide)
	{
		m_vHomePos = TM->Get_MiniGameBlock()->Get_InitPos();
		m_vHomePos.x += -4.f;
		m_vHomePos.z += -8.f;

		// 카메라 앞 포지션 (목적지)
		_vector vCamLookPos = XMLoadFloat3(&m_vHomePos);
		_vector vMyPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vDisVec = vCamLookPos - vMyPos;
		_float fDis = XMVectorGetX(XMVector3Length(vDisVec));

		if (0.5f > fDis)
		{
			m_bLeftEndHide = false;
			XMStoreFloat3(&m_vHomePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			Set_State(STATE_IDLE);
			return;
		}

		_vector vCamPos = XMLoadFloat3(&CRM->Get_Cam()->Get_CamPos());
		m_pTransformCom->LookAt_ForLandObject(XMVectorSetW(vCamPos, 1.f), true);

		m_pTransformCom->Go_Dir(XMVectorSetW(XMVector3Normalize(vDisVec), 0.f), 20.f, fTimeDelta);
	}


}

void CBlockCuma::Tick_EndCut(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (0.f < m_fTimeAcc && 10.f > m_fTimeAcc)
	{
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(0.f, 0.f, 0.f);
		CamDesc.vLook = _float3(0.f, 0.f, 0.f);
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 0.5f;
		CamDesc.fLookLerpSpeed = 0.5f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = m_pTransformCom;
		CamDesc.vPosDis = _float3(0.f, 0.5f, -1.f);
		CamDesc.bPixLocal = true;
		CRM->Get_Cam()->Show_Target(CamDesc);

		m_fTimeAcc = 10.f;
		GI->PlaySoundW(L"GGST_254.ogg", SD_MAP, 1.f);
		m_bCutShake = true;
		m_bBoom = true;

		UM->Set_FrontUI(false);
	}
	else if (13.f < m_fTimeAcc && 20.f > m_fTimeAcc)
	{
		m_bJump = true;
		m_bBoom = false;
		CRM->Get_Cam()->Shake(0.6f, 0.2f, false, 0.1f);
		GI->PlaySoundW(L"ADV_012.ogg", SD_MAP, 1.f);

		// 카메라포스 오락기 정면
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = CRM->Get_Cam()->Get_CamPos();
		XMStoreFloat3(&CamDesc.vLook, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		CamDesc.vLook.y += 15.f;
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 10.f;
		CamDesc.fLookLerpSpeed = 10.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);


		m_bCutShake = false;




		// 스모크
		CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
		Info->fBlurPower = 1.f;
		Info->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
		Info->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 1.f,1.f };
		Info->vRatioSpeed = { 2.f,2.f };
		Info->vScale = { 0.1f,0.1f,0.1f };
		Info->vScaleSpeed = { 250.f,250.f,250.f };
		Info->vUvFixSpeed = { 1.f,3.f };
		Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);

		CPlayer01SmokeBall::SMOKEBALLINFO* Info2 = new CPlayer01SmokeBall::SMOKEBALLINFO;
		Info2->fBlurPower = 1.f;
		Info2->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info2->vColor2 = { 0.7f,0.7f,0.7f,1.f };
		Info2->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info2->vRatio = { 1.f,1.f };
		Info2->vRatioSpeed = { 2.f,2.f };
		Info2->vScale = { 0.1f,0.1f,0.1f };
		Info2->vScaleSpeed = { 300.f,300.f,300.f };
		Info2->vUvFixSpeed = { -1.f,-3.f };
		Info2->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2);




		m_fTimeAcc = 20.f;
	}
	else if (21.f < m_fTimeAcc && 30.f > m_fTimeAcc)
	{

		// 카메라포스 오락기 정면
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = CRM->Get_Cam()->Get_CamPos();
		CamDesc.vLook = _float3(-192.825f, 39.097f, 130.33f);
		CamDesc.vLook.y += 5.f;
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 0.5f;
		CamDesc.fLookLerpSpeed = 0.5f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);


		m_fTimeAcc = 30.f;
	}
	else if (31.f < m_fTimeAcc && 40.f > m_fTimeAcc)
	{
		m_bJump = false;
		m_bDonw = true;

		_float fY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-192.825f, fY, 130.33f, 1.f));

		m_fTimeAcc = 40.f;
	}




	if (m_bJump)
	{
		m_pTransformCom->Go_Dir(XMVectorSet(0.f, 1.f, 0.f, 0.f), 80.f, fTimeDelta);
	}
	else if (m_bDonw)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		if (39.097f < XMVectorGetY(vPos))
		{
			m_pTransformCom->Go_Dir(XMVectorSet(0.f, -1.f, 0.f, 0.f), 60.f, fTimeDelta);
		}
		else
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-192.825f, 39.097f, 130.33f, 1.f));

			GI->PlaySoundW(L"ADV_008.ogg", SD_MAP, 1.f);
			m_bDonw = false;
			m_bCutEnd = true;
			CRM->Get_Cam()->Shake(0.6f, 0.2f, false, 0.1f);
		}

	}





	if (m_bCutShake)
	{
		m_fCutShakeTimeAcc += fTimeDelta;

		if (0.04f < m_fCutShakeTimeAcc)
		{
			Shake();
			m_fCutShakeTimeAcc = 0.f;
		}

	}



	if (m_bCutEnd)
	{
		m_fTimeCutCut += fTimeDelta;
		if (2.f < m_fTimeCutCut)
		{
			Set_Dead();
			CRM->Get_Cam()->End_Target();
			UM->Set_BattleTalkIndex(23);
			UM->Set_FrontUI(true);
			// TM->Open_Stage4Trigger(1);
			return;
		}
	}





	if (m_bBoom)
	{
		m_fBoomTimeAcc += fTimeDelta;
		if (0.25f < m_fBoomTimeAcc)
		{

			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector vRanPos = XMVectorSet(GI->Get_FloatRandom(-2.5f, 2.5f), -2.f + GI->Get_FloatRandom(0.5f, 2.5f), GI->Get_FloatRandom(-2.5f, 2.5f), 1.f);
			_matrix vvM = XMMatrixIdentity();
			vvM.r[3] = vPos + vRanPos;
			_float4x4 vM; XMStoreFloat4x4(&vM, vvM);

			_int iIndex = GI->Get_Random(0, 3);

			if (0 == iIndex)
			{
				PTM->Create_InstanceParticle(L"FireCracker", vM); // 빨강
				PTM->Create_InstanceParticle(L"FireCracker_2", vM); // 블루
			}
			else if (1 == iIndex)
			{
				PTM->Create_InstanceParticle(L"FireCracker_2", vM); // 블루
				PTM->Create_InstanceParticle(L"FireCracker_3", vM); // 노랑
			}
			else if (3 == iIndex)
			{
				PTM->Create_InstanceParticle(L"FireCracker", vM); // 빨강
				PTM->Create_InstanceParticle(L"FireCracker_3", vM); // 노랑
			}
			else if (4 == iIndex)
			{
				PTM->Create_InstanceParticle(L"FireCracker", vM); // 빨강
				PTM->Create_InstanceParticle(L"FireCracker_2", vM); // 블루
				PTM->Create_InstanceParticle(L"FireCracker_3", vM); // 노랑
			}


			m_fBoomTimeAcc = 0.f;
		}

	}



}

void CBlockCuma::Tick_Shake(_float fTimeDelta)
{
	m_fShakeEndTimeAcc += fTimeDelta;
	if (0.2f < m_fShakeEndTimeAcc)
	{
		m_bShake = false;
		// m_eFace = FACE_IDLE;
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_vHomePos.y = XMVectorGetY(vPos);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vHomePos), 1.f));
		return;
	}


	m_fShakeTimeAcc += fTimeDelta;
	if (0.06f < m_fShakeTimeAcc)
	{
		m_bShakeTurn = !m_bShakeTurn;
		m_fShakeTimeAcc = 0.f;
	}

	if (m_bShakeTurn)
	{
		m_pTransformCom->Go_Dir(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), 5.f, fTimeDelta);
	}
	else
	{
		m_pTransformCom->Go_Dir(-1.f * m_pTransformCom->Get_State(CTransform::STATE_RIGHT), 5.f, fTimeDelta);
	}

}

void CBlockCuma::Peek_A_Boo()
{
	_uint iRandomVoice = GI->Get_Random(0, 1);
	switch (iRandomVoice)
	{
	case 0:
		GI->PlaySoundW(L"ufo-beep.wav", SD_MAP, 1.f);
		break;
	case 1:
		GI->PlaySoundW(L"ufo-laser.wav", SD_MAP, 1.f);
		break;
	}
}






void CBlockCuma::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;



	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}



HRESULT CBlockCuma::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);


	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	_uint		iNumMeshes = 0;
	_uint iPassIndex = 0;
	_float fBlurPower = 0.f;
	_float fAlpha = 0.f;


	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
		return E_FAIL;

	_float fGama = 1.8f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;

	if (FACE_SMILE == m_eFace)
	{
		iNumMeshes = m_pModelCom1->Get_NumMeshes();
		iPassIndex = 0;
		fBlurPower = 0.f;


		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom1->SetUp_OnShader(m_pShaderCom, m_pModelCom1->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;
			// 노말 텍스쳐가 있다면
			if (!FAILED(m_pModelCom1->SetUp_OnShader(m_pShaderCom, m_pModelCom1->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
				iPassIndex = 2;

			if (FAILED(m_pModelCom1->Render(m_pShaderCom, i, iPassIndex)))
				return E_FAIL;
		}
	}
	else if (FACE_IDLE == m_eFace)
	{
		iNumMeshes = m_pModelCom->Get_NumMeshes();
		iPassIndex = 0;
		fBlurPower = 0.f;

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;
			// 노말 텍스쳐가 있다면
			if (!FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
				iPassIndex = 2;

			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iPassIndex)))
				return E_FAIL;
		}
	}
	else if (FACE_HURT == m_eFace)
	{
		iNumMeshes = m_pModelCom2->Get_NumMeshes();
		iPassIndex = 0;
		fBlurPower = 0.f;

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom2->SetUp_OnShader(m_pShaderCom, m_pModelCom2->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;
			// 노말 텍스쳐가 있다면
			if (!FAILED(m_pModelCom2->SetUp_OnShader(m_pShaderCom, m_pModelCom2->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
				iPassIndex = 2;

			if (FAILED(m_pModelCom2->Render(m_pShaderCom, i, iPassIndex)))
				return E_FAIL;
		}
	}



	return S_OK;
}


HRESULT CBlockCuma::Ready_Components()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_PR_BearA"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_PR_BearAa"), TEXT("Com_Model1"), (CComponent**)&m_pModelCom1)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_PR_BearAc"), TEXT("Com_Model2"), (CComponent**)&m_pModelCom2)))
		return E_FAIL;


	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(2.f, 2.f, 2.f);
	ColDesc.vCenter = _float3(0.f, 0.2f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Body1");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(2.f, 2.f, 2.f);
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Push_Range"); // 밀어내기 위함

	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}





CBlockCuma * CBlockCuma::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBlockCuma*		pInstance = new CBlockCuma(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBlockCuma"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCreture * CBlockCuma::Clone(void * pArg)
{
	CBlockCuma*		pInstance = new CBlockCuma(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBlockCuma"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBlockCuma::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pModelCom1);
	Safe_Release(m_pModelCom2);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}




void CBlockCuma::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);


	ImGui::End();
}





