#include "stdafx.h"
#include "..\Public\Boss_Cuma.h"
#include "GameInstance.h"


#include "Player_Manager.h"
#include "Parts.h"
#include "ToolManager.h"
#include "Time_Manager.h"
#include "Camera_Manager.h"
#include "Stage4Machine0.h"
#include "MechanicalOctopus.h"
#include "UI_Manager.h"
#include "Monster_Manager.h" 
#include "Particle_Manager.h"
#include "OctopusParticle.h"
#include "Player01SmokeBall.h"
#include "Player_Manager.h"

CBoss_Cuma::CBoss_Cuma(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CBoss_Cuma::CBoss_Cuma(const CBoss_Cuma & rhs)
	: CCreture(rhs)
{
}

HRESULT CBoss_Cuma::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CBoss_Cuma::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Boss";
	Make_ImGui();

	m_fHurtTime = 0.02f;
	m_fMaxHp = 1000.f;
	m_fNowHp = m_fMaxHp;
	m_fStamina = 1.f;
	//m_fDamage = 20.f;

	m_fPreHp = m_fNowHp;



	m_pTransformCom->Set_Scale(XMVectorSet(4.f, 4.f, 4.f, 1.f));

	XMStoreFloat3(&m_vInitPos, XMVectorSet(2.2f, 15.531f, 155.164f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(2.2f, 15.531f, 155.164f, 1.f));




	m_vLocalPos = { 0.f, 1.f, 0.f, 1.f };




	CCamera_Main::CUTDESC CamDesc;
	CamDesc.vPos = _float3(2.2f, 15.531f, 155.164f);
	CamDesc.vPos.z -= 5.f;
	CamDesc.vPos.y += 2.f;
	CamDesc.vLook = _float3(2.2f, 15.531f, 155.164f);
	CamDesc.vLook.y += 1.f;
	CamDesc.bLerp = false;
	CamDesc.bLookLerp = false;
	CamDesc.fLerpSpeed = 1.f;
	CamDesc.fLookLerpSpeed = 1.f;
	CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
	CamDesc.pTran = nullptr;
	CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
	CamDesc.bPixLocal = false;
	CRM->Get_Cam()->Show_Target(CamDesc);


	Set_State(STATE_IDLE);



	return S_OK;
}

void CBoss_Cuma::Tick(_float fTimeDelta)
{

	// Emp_Col();

	switch (m_eState)
	{
	case STATE_IDLE:
		Tick_Idle(fTimeDelta);
		break;
	case STATE_ONING:
		Tick_Oning(fTimeDelta);
		break;
	case STATE_ONIDLE:
		Tick_OnIdle(fTimeDelta);
		break;
	case STATE_DIE:
		Tick_Die(fTimeDelta);
		break;
	default:
		break;
	}

	//바닥에 떨어질때 FALL.wav나 FALL_OFFICE.wav 넣어주세여

	if (m_bShake)
		Tick_Shake(fTimeDelta);

}

void CBoss_Cuma::LateTick(_float fTimeDelta)
{

	if (nullptr == m_pRendererCom)
		return;



	//Choose_Col(nullptr);
	//Tick_Col(m_pTransformCom->Get_WorldMatrix());
	//Render_Col(m_pRendererCom);


	if(m_bRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

}

HRESULT CBoss_Cuma::Render()
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

		//// For.Rim
		//if (FAILED(m_pShaderCom->Set_RawValue("g_CamPosition", &GI->Get_CamPosition(), sizeof(_float4))))
		//	return E_FAIL;
		//_float m_fRim = 10.f;
		//if (FAILED(m_pShaderCom->Set_RawValue("g_RimPower", &m_fRim, sizeof(_float))))
		//	return E_FAIL;



		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom2->SetUp_OnShader(m_pShaderCom, m_pModelCom2->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;
			//// 노말 텍스쳐가 있다면
			//if (!FAILED(m_pModelCom2->SetUp_OnShader(m_pShaderCom, m_pModelCom2->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			//	iPassIndex = 2;

			if (FAILED(m_pModelCom2->Render(m_pShaderCom, i, iPassIndex)))
				return E_FAIL;
		}
	}



	return S_OK;
}

// *[HeroJho]: 디버깅 창입니다. ImGui로 작동됩니다.
void CBoss_Cuma::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);



	ImGui::End();
}

// *[HeroJho]: 충돌이 발생할 때 호출되는 함수입니다.
void CBoss_Cuma::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	// *[HeroJho]:
	// Desc.pOther		- 충돌한 상대편 객체의 주소입니다.
	// Desc.pMyDesc		- 상대편과 충돌한 나의 콜라이더 정보입니다.
	// Desc.pOtherDesc	- 나와 충돌한 상대편의 콜라이더 정보입니다.
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		// 내 손 Sphere 둘중 하나와 상대편의 OBB와 충돌 했다면
		if (!strcmp("Monster_Push_Range", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Push_Range", Desc.pOtherDesc->sTag))
		{
			m_pTransformCom->PushMeXZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION), Desc.pMyDesc->vSize.x, XMLoadFloat4(&((CCreture*)Desc.pOther)->Get_WorldPos()), Desc.pOtherDesc->vSize.x, TM->Get_TimeDelta(), nullptr);
		}
		if (strcmp("Monster_Body1", Desc.pMyDesc->sTag) &&
			strcmp("Monster_Push_Range", Desc.pMyDesc->sTag) && !strcmp("Player_Body", Desc.pOtherDesc->sTag))
		{
			((CCreture*)Desc.pOther)->On_Hit(this);
		}
	}

}

void CBoss_Cuma::On_Hit(CCreture* pOther)
{
	//hit이 한번만 처리되게끔
	if (m_bHurt)
	{
		m_bHurt = true;
		Shake();

	}
}









void CBoss_Cuma::Set_State(STATE eState)
{

	switch (m_eState)
	{
	case STATE_IDLE:
		break;
	case STATE_ONING:
		break;
	case STATE_ONIDLE:
		break;
	default:
		break;
	}


	switch (eState)
	{
	case STATE_IDLE:
	{
		m_eFace = FACE_HURT;
	}

	break;
	case STATE_ONING:
	{
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = CRM->Get_Cam()->Get_CamPos();
		CamDesc.vLook = CRM->Get_Cam()->Get_PreLook();
		CamDesc.vLook.y += 1.5f;
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 5.f;
		CamDesc.fLookLerpSpeed = 5.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);

		CRM->Get_Cam()->Shake(0.4f, 0.2f, false, 0.1f);
		m_bJump = true;
	}
	break;
	case STATE_ONIDLE:
		m_eFace = FACE_SMILE;
		CRM->Get_Cam()->Shake(0.4f, 0.2f, false, 0.1f);
		break;
	default:
		break;
	}

	m_eState = eState;

}

void CBoss_Cuma::Start_On()
{
	m_eState = STATE_ONING;
}

void CBoss_Cuma::Shake()
{
	m_bShake = true;
	m_bShakeTurn = false;
	m_fShakeTimeAcc = 0.f;
	m_fShakeEndTimeAcc = 0.f;
	// m_eFace = FACE_HURT;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vInitPos), 1.f));
}

void CBoss_Cuma::Tick_Idle(_float fTimeDelta)
{

	// 주기적으로 쉐이크를 준다.
	if (m_bRealShake)
	{
		m_fRealShakeTimeAcc += fTimeDelta;
		if (0.2f < m_fRealShakeTimeAcc)
		{
			Shake();
			m_fRealShakeTimeAcc = 0.f;

			_float4 fPlayerPos = PM->Get_PlayerPos();
			_bool bNear = m_pTransformCom->Check_LimitDistance(XMLoadFloat4(&fPlayerPos), 10.f);
			//if(bNear)
			//	GI->PlaySoundW(L"GGST_254.ogg", SD_NPC, 1.f);
		}
	}

	if (!TM->Get_StartBossCutScene())
		return;


	m_fTimeAcc += fTimeDelta;
	if (0.1f < m_fTimeAcc && 10.f > m_fTimeAcc)
	{
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(2.2f, 15.531f, 155.164f);
		CamDesc.vPos.z -= 1.f;
		CamDesc.vPos.y += 1.f;
		CamDesc.vLook = _float3(2.2f, 15.531f, 155.164f);
		CamDesc.vLook.y += 1.f;
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 0.5f;
		CamDesc.fLookLerpSpeed = 0.5f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);

		m_fTimeAcc = 10.f;
	}
	else if (13.f < m_fTimeAcc && 20.f > m_fTimeAcc)
	{
		m_pBoss->Set_OctopusCutSceneState(2.2f, 163.164f, 50.f);
		m_fTimeAcc = 30.f;
	}
	// 문어 떨어지는 타이밍
	else if (31.5f < m_fTimeAcc && 40.f > m_fTimeAcc)
	{
		// 쉐이킹
		CRM->Get_Cam()->Shake(0.4f, 0.2f, false, 0.1f);

		// 약간 뒤로
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = CRM->Get_Cam()->Get_CamPos();
		CamDesc.vPos.z -= 1.f;
		CamDesc.vPos.y += 0.5f;
		CamDesc.vLook = CRM->Get_Cam()->Get_PreLook();
		CamDesc.vLook.y += 1.f;
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 2.f;
		CamDesc.fLookLerpSpeed = 2.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);


		m_fTimeAcc = 40.f;
	}
	else if (43.f < m_fTimeAcc && 50.f > m_fTimeAcc)
	{
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = CRM->Get_Cam()->Get_CamPos();
		CamDesc.vLook = CRM->Get_Cam()->Get_PreLook();
		CamDesc.vLook.y -= 1.f;
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 5.f;
		CamDesc.fLookLerpSpeed = 5.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);

		m_eFace = FACE_SMILE;
		m_bRealShake = false;

		_float4x4 vM; XMStoreFloat4x4(&vM, m_pTransformCom->Get_WorldMatrix_NoScale());
		PTM->Create_InstanceParticle(L"BearCutLight", vM);
		GI->StopSound(SD_NPC);
		GI->PlaySoundW(L"ARC_BTL_SYS_GaugeCount.ogg", SD_NPC, 0.7f);

		m_fTimeAcc = 50.f;
	}
	// 점프 시작
	else if (52.f < m_fTimeAcc)
	{
		Set_State(STATE_ONING);
		m_fTimeAcc = 0.f;
		GI->PlaySoundW(L"GGST_142.wav", SD_NPC, 1.f);
		return;
	}
}

void CBoss_Cuma::Tick_Oning(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (1.f < m_fTimeAcc && 10.f > m_fTimeAcc)
	{
		// 위 도착, 포지션 맞춰주고 떨어질 준빈
		m_bJump = false;
		_float fY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		_float4 vPos = m_pBoss->Get_WorldPos();
		vPos.y = fY;
		vPos.w = 1.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));

		m_fTimeAcc = 10.f;
	}


	// 점프
	if (m_bJump)
	{
		m_pTransformCom->Go_Dir(XMVectorSet(0.f, 1.f, 0.f, 0.f), 80.f, fTimeDelta);
	}
	else
	{
		m_pTransformCom->Go_Dir(XMVectorSet(0.f, -1.f, 0.f, 0.f), 60.f, fTimeDelta);

		_matrix mBoss = XMLoadFloat4x4(&m_pBoss->Get_BoneMatrix());
		_vector vLocalPos = XMLoadFloat4(&m_vLocalPos);
		_vector vWorldPos = XMVector3TransformCoord(vLocalPos, mBoss);
		_float vDestY = XMVectorGetY(vWorldPos);
		_float vMyY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		// 안착
		if (vMyY < vDestY)
		{
			CCamera_Main::CUTDESC CamDesc;
			CamDesc.vPos = CRM->Get_Cam()->Get_CamPos();
			CamDesc.vLook = CRM->Get_Cam()->Get_PreLook();
			CamDesc.vLook.y -= 0.6f;
			CamDesc.bLerp = true;
			CamDesc.bLookLerp = true;
			CamDesc.fLerpSpeed = 5.f;
			CamDesc.fLookLerpSpeed = 5.f;
			CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
			CamDesc.pTran = nullptr;
			CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
			CamDesc.bPixLocal = false;
			CRM->Get_Cam()->Show_Target(CamDesc);

			Set_State(STATE_ONIDLE);

			m_fTimeAcc = 0.f;

			GI->PlaySoundW(L"song050_start_2.wav", SD_NPC, 0.7f);



			//  스모크
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
			Info2->vScaleSpeed = { 300.f,300.f,300.f};
			Info2->vUvFixSpeed = { -1.f,-3.f };
			Info2->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2);






			return;
		}

	}

}

void CBoss_Cuma::Tick_OnIdle(_float fTimeDelta)
{
	if (!m_bStart)
	{
		m_fTimeAcc += fTimeDelta;
		if (5.f < m_fTimeAcc)
		{
			m_bStart = true;
			m_fTimeAcc = 0.f;

			// 보스전 시작
			m_pBoss->Set_CutScene(false);
			CRM->Get_Cam()->End_Target();
			m_pBoss->Control_MechanicalOctopus();

			UM->Set_BattleTalkIndex(28);
			UM->Set_FrontUI(true);
		}
	}

	if (TM->Get_BossGro())
	{
		m_eFace = FACE_HURT;
		UM->Set_FrontUI(false);

		if (TM->Get_BossDie())
			m_eState = STATE_DIE;
	}


	
	

	// 내가 붙어야할 위치
	_matrix mBoss = XMLoadFloat4x4(&m_pBoss->Get_BoneMatrix());
	_vector vLocalPos = XMLoadFloat4(&m_vLocalPos);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(mBoss.r[3] + vLocalPos, 1.f));
	m_pTransformCom->Set_Look(-1.f * XMVector3Normalize(mBoss.r[1]));

}

void CBoss_Cuma::Tick_Die(_float fTimeDelta)
{
	// 내가 붙어야할 위치
	if (m_pBoss)
	{
		_matrix mBoss = XMLoadFloat4x4(&m_pBoss->Get_BoneMatrix());
		_vector vLocalPos = XMLoadFloat4(&m_vLocalPos);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(mBoss.r[3] + vLocalPos, 1.f));
		m_pTransformCom->Set_Look(-1.f * XMVector3Normalize(mBoss.r[1]));
	}



	m_fCutSceneAcc += fTimeDelta;
	if (0.f < m_fCutSceneAcc && 10.f > m_fCutSceneAcc)
	{
		

		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = *((_float3*)&m_pBoss->Get_WorldPos());
		CamDesc.vPos.z -= 5.f;
		CamDesc.vPos.y += 5.f;
		CamDesc.vLook = *((_float3*)&m_pBoss->Get_WorldPos());
		CamDesc.vLook.y += 3.f;
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 0.2f;
		CamDesc.fLookLerpSpeed = 0.2f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);


// 		CRM->Get_Cam()->Shake(0.1f, 6.f);
		CRM->Get_Cam()->Shake(0.1f, 6.f);
		m_bBoome = true;


		m_fCutSceneAcc = 10.f;
	}
	else if (16.f < m_fCutSceneAcc && 20.f > m_fCutSceneAcc)
	{
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(0.f, 0.f, 0.f);
		CamDesc.vLook = _float3(0.f, 0.f, 0.f);
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 3.f;
		CamDesc.fLookLerpSpeed = 3.f;
		CamDesc.vPivotLookPos = _float3(0.f, 2.f, 0.f);
		CamDesc.pTran = m_pTransformCom;
		CamDesc.vPosDis = _float3(0.f, 1.5f, -3.5f);
		CamDesc.bPixLocal = true;
		CRM->Get_Cam()->Show_Target(CamDesc);


		m_fCutSceneAcc = 20.f;
	}
	else if (20.2f < m_fCutSceneAcc && 30.f > m_fCutSceneAcc)
	{
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(0.f, 0.f, 0.f);
		CamDesc.vLook = _float3(0.f, 0.f, 0.f);
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 0.5f;
		CamDesc.fLookLerpSpeed = 0.5f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.2f, 0.f);
		CamDesc.pTran = m_pTransformCom;
		CamDesc.vPosDis = _float3(0.f, 0.4f, -0.5f);
		CamDesc.bPixLocal = true;
		CRM->Get_Cam()->Show_Target(CamDesc);

		m_bBoome = false;
		m_bCry = true;
		m_fCryTimeAcc = 2.1f;

		m_fCutSceneAcc = 30.f;
	}
	else if (35.f < m_fCutSceneAcc && 40.f > m_fCutSceneAcc)
	{
		// 대 폭발
		// 피직스 터트리기

		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = *((_float3*)&m_pBoss->Get_WorldPos());
		CamDesc.vPos.x -= 10.f;
		CamDesc.vPos.z -= 10.f;
		CamDesc.vPos.y += 10.f;
		CamDesc.vLook = *((_float3*)&m_pBoss->Get_WorldPos());
		CamDesc.vLook.y += 3.f;
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 3.f;
		CamDesc.fLookLerpSpeed = 3.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);


		 m_pBoss->Die();
		 m_pBoss = nullptr;
		 m_bRender = false;
		 m_bCry = false;

		CRM->Get_Cam()->Shake(1.5f, 0.4f, false, 0.1f);







		//테스트
		COctopusParticle::PARTDESC OctopusPartDesc;
		ZeroMemory(&OctopusPartDesc, sizeof(COctopusParticle::PARTDESC));

		_float4 vPos = Get_WorldPos();
		vPos.y -= 2.f;

		//나중에 문어 포지션 넣어줘야함
		OctopusPartDesc.vLocalPosition = vPos;

		//곰돌이
		OctopusPartDesc.iModelType = 1;
		OctopusPartDesc.fAngle = 0.f;
		OctopusPartDesc.vStartPos = _float3(0.f, 1.8f, 0.f); // 모델 위치랑 상관있음, 로컬
		OctopusPartDesc.vPos = _float3(0.f, -0.8f, 0.f); //콜라이더와의 위치
		OctopusPartDesc.vRotation = _float3(0.f, 0.f, 0.f); //회전은 그냥
		OctopusPartDesc.vExtense = _float3(1.1f, 1.2f, 1.f);
		//OctopusPartDesc.vFlyPower = _float3(0.f, 0.f, 0.f);
		(GI->Add_GameObjectToLayer(TEXT("OctopusParticle"), LEVEL_STAGE4, TEXT("Layer_BeastObj"), &OctopusPartDesc));


		//문어다리
		OctopusPartDesc.iModelType = 2;

		OctopusPartDesc.vStartPos = _float3(0.7f, 0.2f, 1.6f); // 모델 위치랑 상관있음
		OctopusPartDesc.vPos = _float3(-0.7f, -0.5f, 0.f);
		OctopusPartDesc.vExtense = _float3(3.4f, 1.2f, 0.9f);
		//OctopusPartDesc.vFlyPower = _float3(0.f, 0.f, 0.f);
		(GI->Add_GameObjectToLayer(TEXT("OctopusParticle"), LEVEL_STAGE4, TEXT("Layer_BeastObj"), &OctopusPartDesc));

		OctopusPartDesc.iModelType = 3;
		OctopusPartDesc.vStartPos = _float3(-1.f, 0.7f, -0.3f); // 모델 위치랑 상관있음
		OctopusPartDesc.vPos = _float3(-0.7f, -0.6f, 0.f);
		OctopusPartDesc.vExtense = _float3(3.4f, 1.2f, 0.9f);
		//OctopusPartDesc.vFlyPower = _float3(0.f, 0.f, 0.f);
		(GI->Add_GameObjectToLayer(TEXT("OctopusParticle"), LEVEL_STAGE4, TEXT("Layer_BeastObj"), &OctopusPartDesc));

		OctopusPartDesc.iModelType = 4;
		OctopusPartDesc.vStartPos = _float3(-1.2f, 0.2f, 1.f); // 모델 위치랑 상관있음
		OctopusPartDesc.vPos = _float3(-0.6f, -0.6f, 0.f);
		OctopusPartDesc.vExtense = _float3(3.4f, 1.2f, 0.9f);
		//OctopusPartDesc.vFlyPower = _float3(0.f, 0.f, 0.f);
		(GI->Add_GameObjectToLayer(TEXT("OctopusParticle"), LEVEL_STAGE4, TEXT("Layer_BeastObj"), &OctopusPartDesc));

		OctopusPartDesc.iModelType = 5;
		OctopusPartDesc.vStartPos = _float3(0.7f, 0.5f, 0.f); // 모델 위치랑 상관있음
		OctopusPartDesc.vPos = _float3(-0.7f, -0.5f, 0.f);
		OctopusPartDesc.vExtense = _float3(3.4f, 1.2f, 0.9f);
		//OctopusPartDesc.vFlyPower = _float3(0.f, 0.f, 0.f);
		(GI->Add_GameObjectToLayer(TEXT("OctopusParticle"), LEVEL_STAGE4, TEXT("Layer_BeastObj"), &OctopusPartDesc));




		_vector vVPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_matrix vvM = XMMatrixIdentity();
		vvM.r[3] = vVPos;
		_float4x4 vM; XMStoreFloat4x4(&vM, vvM);


		PTM->Create_InstanceParticle(L"Player03CannonParticle_2", vM);
		PTM->Create_InstanceParticle(L"Player03CannonParticle_4", vM);
		PTM->Create_InstanceParticle(L"Player03CannonParticle_5", vM);
		PTM->Create_InstanceParticle(L"Player03CannonDistortion", vM);
		PTM->Create_InstanceParticle(L"Player03CannonParticle", vM);

		CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
		Info->fBlurPower = 0.f;
		Info->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
		Info->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 1.f,1.f };
		Info->vRatioSpeed = { 1.5f,1.5f };
		Info->vScale = { 0.1f,0.1f,0.1f };
		Info->vScaleSpeed = { 200.f,200.f,200.f };
		Info->vUvFixSpeed = { 1.f,3.f };
		Info->vWorldMatrix = vM;
		GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);

		CPlayer01SmokeBall::SMOKEBALLINFO* Info2 = new CPlayer01SmokeBall::SMOKEBALLINFO;
		Info2->fBlurPower = 2.f;
		Info2->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info2->vColor2 = { 0.7f,0.7f,0.7f,1.f };
		Info2->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info2->vRatio = { 1.f,1.f };
		Info2->vRatioSpeed = { 1.5f,1.5f };
		Info2->vScale = { 0.1f,0.1f,0.1f };
		Info2->vScaleSpeed = { 200.f,200.f,200.f };
		Info2->vUvFixSpeed = { -1.f,-3.f };
		Info2->vWorldMatrix = vM;
		GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2);



		GI->PlaySoundW(L"se_EN0005_attack2_3.wav", SD_TRAININGBOT1, 0.6f);
		GI->StopSound(SD_BGM);

		m_fCutSceneAcc = 40.f;
	}
	else if (45.f < m_fCutSceneAcc && 50.f > m_fCutSceneAcc)
	{


		m_fCutSceneAcc = 50.f; 
	}
	else if (50.f < m_fCutSceneAcc && 60.f > m_fCutSceneAcc)
	{
		// 끝
		CRM->Get_Cam()->End_Target();
		Set_Dead();
		UM->Set_TalkIndex(87);

		m_fCutSceneAcc = 60.f;
	}








	if (m_bBoome)
	{
		m_fBoomeTimeAcc += fTimeDelta;
		if (0.5f < m_fBoomeTimeAcc)
		{

			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_vector vRanPos = XMVectorSet(GI->Get_FloatRandom(-2.5f, 2.5f), -2.f + GI->Get_FloatRandom(-2.5f, 2.5f), GI->Get_FloatRandom(-2.5f, 2.5f), 1.f);

			vPos += vRanPos;

			_matrix vvM = XMMatrixIdentity();
			vvM.r[3] = vPos;
			_float4x4 vM; XMStoreFloat4x4(&vM, vvM);

			PTM->Create_InstanceParticle(L"OctoPusDie", vM);
			PTM->Create_InstanceParticle(L"OctoPusDie_2", vM);
			PTM->Create_InstanceParticle(L"OctoPusDie_3", vM);
			PTM->Create_InstanceParticle(L"OctoPusDie_4", vM);

			GI->PlaySoundW(L"se_HE04_Skill05_4.wav", SD_PLAYER3_SK, 0.5f);

			m_fBoomeTimeAcc = 0.f;
		}

	}

	if (m_bCry)
	{
		m_fCryTimeAcc += fTimeDelta;
		if (2.f < m_fCryTimeAcc)
		{
			GI->PlaySoundW(L"Yawn.wav", SD_NPC, 0.7f);
			m_fCryTimeAcc = 0.f;
		}


		m_fCry2TimeAcc += fTimeDelta;
		if (0.2f < m_fCry2TimeAcc)
		{
			_matrix vvM = m_pTransformCom->Get_WorldMatrix_NoScale();
			_float4x4 vM; XMStoreFloat4x4(&vM, vvM);
			PTM->Create_InstanceParticle(L"BearCryLeft", vM);
			PTM->Create_InstanceParticle(L"BearCryRight", vM);
			
			GI->PlaySoundW(L"CMN_MINIGAME_BNK_SE_00000019.wav", SD_NPC, 0.3f);
			m_fCry2TimeAcc = 0.f;
		}
	}
		


}














void CBoss_Cuma::Tick_Shake(_float fTimeDelta)
{
	/*_vector vInitPos = XMLoadFloat3(&m_vInitPos);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	*/
	m_fShakeEndTimeAcc += fTimeDelta;
	if (0.2f < m_fShakeEndTimeAcc)
	{
		m_bShake = false;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vInitPos), 1.f));
		return;
	}


	m_fShakeTimeAcc += fTimeDelta;
	if (0.05f < m_fShakeTimeAcc)
	{
		m_bShakeTurn = !m_bShakeTurn;
		m_fShakeTimeAcc = 0.f;
	}

	if (m_bShakeTurn)
	{
		m_pTransformCom->Go_Dir(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), 1.f, fTimeDelta);
	}
	else
	{
		m_pTransformCom->Go_Dir(-1.f * m_pTransformCom->Get_State(CTransform::STATE_RIGHT), 1.f, fTimeDelta);
	}


}


HRESULT CBoss_Cuma::Ready_Components()
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


	///* For.Com_Collider */
	//CCollider::COLLIDERDESC ColDesc;
	//ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));


	//ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	//ColDesc.vSize = _float3(2.f, 2.f, 2.f);
	//ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	//ColDesc.iFlag = 2;
	//ColDesc.pOwner = this;
	//strcpy(ColDesc.sTag, "Monster_Push_Range"); // 밀어내기 위함

	//if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
	//	return E_FAIL;


	//ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	//ColDesc.vSize = _float3(2.f, 2.f, 2.f);
	//ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	//ColDesc.pOwner = this;
	//ColDesc.iFlag = 0;
	//strcpy(ColDesc.sTag, "Monster_Body1"); //몬스터 공격용 충돌체
	//if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
	//	return E_FAIL;


	return S_OK;
}



CBoss_Cuma * CBoss_Cuma::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBoss_Cuma*		pInstance = new CBoss_Cuma(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBoss_Cuma"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBoss_Cuma::Clone(void * pArg)
{
	CBoss_Cuma*		pInstance = new CBoss_Cuma(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBoss_Cuma"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss_Cuma::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pModelCom1);
	Safe_Release(m_pModelCom2);
	Safe_Release(m_pShaderCom);


}
