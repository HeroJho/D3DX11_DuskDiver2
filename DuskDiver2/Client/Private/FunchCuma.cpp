#include "stdafx.h"
#include "..\Public\FunchCuma.h"
#include "GameInstance.h"


#include "Player_Manager.h"
#include "UI_Manager.h"
#include "Parts.h"
#include "ToolManager.h"
#include "Time_Manager.h"
#include "Camera_Manager.h"
#include "Stage4Machine0.h"
#include "Light.h"
#include "Bear_HP_Bar.h"

#include "Player01Wind1.h"
#include "Player01SmokeBall.h"
#include "Particle_Manager.h"

CFunchCuma::CFunchCuma(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CFunchCuma::CFunchCuma(const CFunchCuma & rhs)
	: CCreture(rhs)
{
}

HRESULT CFunchCuma::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CFunchCuma::Initialize(void * pArg)
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



	m_pTransformCom->Set_Scale(XMVectorSet(10.f, 10.f, 10.f, 1.f));


	Set_State(STATE_CUT);


	XMStoreFloat3(&m_vInitPos, XMVectorSet(141.7f, -15.7f, 85.6f, 1.f));



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

void CFunchCuma::Tick(_float fTimeDelta)
{
	m_pLight->Get_LightDesc()->vPosition = Get_WorldPos();

	Emp_Col();

	switch (m_eState)
	{
	case STATE_CUT:
		Tick_Cut(fTimeDelta);
		break;
	case STATE_IDLE:
		Tick_Idle(fTimeDelta);
		break;
	case STATE_ENDCUT:
		Tick_EndCut(fTimeDelta);
		break;
	default:
		break;
	}


	if (m_bShake)
		Tick_Shake(fTimeDelta);

}

void CFunchCuma::LateTick(_float fTimeDelta)
{

	if (nullptr == m_pRendererCom)
		return;



	Choose_Col(nullptr);
	Tick_Col(m_pTransformCom->Get_WorldMatrix());
	Render_Col(m_pRendererCom);



	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

}

HRESULT CFunchCuma::Render()
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
void CFunchCuma::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);



	ImGui::End();
}

// *[HeroJho]: 충돌이 발생할 때 호출되는 함수입니다.
void CFunchCuma::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	// *[HeroJho]:
	// Desc.pOther		- 충돌한 상대편 객체의 주소입니다.
	// Desc.pMyDesc		- 상대편과 충돌한 나의 콜라이더 정보입니다.
	// Desc.pOtherDesc	- 나와 충돌한 상대편의 콜라이더 정보입니다.
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{

		UM->Set_TalkButtonType(4);
		UM->Set_TalkButton(true);

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

void CFunchCuma::On_Hit(CCreture* pOther)
{
	if (!m_bStart)
		return;

	//hit이 한번만 처리되게끔
	if (m_bHurt)
	{
		m_bHurt = true;
		Shake();


		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		vPos.y += 1.f;
		pOther->Create_HitEffect(vPos);

		// 점수 중첩
		m_fDamageScore += 500.f;
		TM->Get_CumaHpBar()->Deal_PunchHp(500.f);
	}

}

void CFunchCuma::Set_State(STATE eState)
{

	switch (m_eState)
	{
	case STATE_CUT:
		break;
	case STATE_IDLE:
		break;
	case STATE_ENDCUT:
		GI->PlaySoundW(L"GGST_254.ogg", SD_NPC, 1.f);
		break;
	default:
		break;
	}




	switch (eState)
	{
	case STATE_CUT:
	{
		UM->Set_FrontUI(false);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(126.f, -18.1f, 84.4f, 1.f));
		m_pTransformCom->Rotation(XMVectorSet(0.f, 195.f, 0.f, 0.f), XMConvertToRadians(195.f));


		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(131.863f, -12.96f, 103.161f);
		CamDesc.vLook = _float3(135.488f, -13.238f, 100.988f);
		CamDesc.bLerp = false;
		CamDesc.bLookLerp = false;
		CamDesc.fLerpSpeed = 0.f;
		CamDesc.fLookLerpSpeed = 0.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);
	}
	break;
	case STATE_IDLE:
		break;
	case STATE_ENDCUT:
	{
		//GI->PlaySound_Linear(L"Arcade.wav", SD_BGM, 0.2f, true, 0.4f);
		//GI->StopSound(SD_BGM);
		//GI->PlayBGM(L"Arcade.wav", 0.2f);
		m_eFace = FACE_HURT;
		m_fTImeAcc = 0.f;
		m_bJump = false;
		m_bDonw = false;

		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(141.603f, -13.977f, 94.226f);
		XMStoreFloat3(&CamDesc.vLook, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		CamDesc.vLook.y += 2.f;
		CamDesc.bLerp = false;
		CamDesc.bLookLerp = false;
		CamDesc.fLerpSpeed = 10.f;
		CamDesc.fLookLerpSpeed = 10.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);
	}
		break;
	default:
		break;
	}

	m_eState = eState;

}

void CFunchCuma::Shake()
{
	m_bShake = true;
	m_bShakeTurn = false;
	m_fShakeTimeAcc = 0.f;
	m_fShakeEndTimeAcc = 0.f;
	m_eFace = FACE_HURT;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vInitPos), 1.f));
}

void CFunchCuma::Start()
{
	m_bStart = true;
	m_fDamageScore = 0.f;
	m_fGameTimeAcc = 0.f;
	m_fSussDamageScore = 10000.f;
	m_fMScore = 2.f;
	CRM->Get_Cam()->Edit_CamDis(_float3(0.f, 1.2f * 3.f, -3.5f* 3.f));
}

void CFunchCuma::Check_Score()
{
	_float fv = m_fSussDamageScore / 3.f;

	if (fv * 2 < m_fDamageScore)
	{
		m_fMScore = 400.f;
	}
	else if (fv < m_fDamageScore)
	{
		m_fMScore = 200.f;
	}
	else
	{
		m_fMScore = 100.f;
	}

}

void CFunchCuma::Tick_Cut(_float fTimeDelta)
{
	m_fTImeAcc += fTimeDelta;

	if (0.5f < m_fTImeAcc && 10.f > m_fTImeAcc)
	{
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(131.863f, -12.96f, 103.161f);
		CamDesc.vLook = _float3(133.388f, -13.211f, 98.95f);
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 1.f;
		CamDesc.fLookLerpSpeed = 1.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);

		m_fTImeAcc = 10.f;
	}
	else if (13.f < m_fTImeAcc && 20.f > m_fTImeAcc)
	{
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(131.863f, -12.96f, 103.161f);
		CamDesc.vLook = _float3(137.798f, -13.227f, 100.783f);
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 1.f;
		CamDesc.fLookLerpSpeed = 1.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);
		GI->PlaySoundW(L"CMN_MINIGAME_BNK_SE_0000000F.wav", SD_MAP, 1.f);

		m_fTImeAcc = 20.f;
	}
	else if (22.f < m_fTImeAcc && 30.f > m_fTImeAcc)
	{
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(131.863f, -12.96f, 103.161f);
		XMStoreFloat3(&CamDesc.vLook, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		CamDesc.vLook.y += 1.5f;
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 1.f;
		CamDesc.fLookLerpSpeed = 3.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);
		GI->PlaySoundW(L"CMN_MINIGAME_BNK_SE_0000001E.wav", SD_MAP, 1.f);

		m_fTImeAcc = 30.f;
	}
	else if (30.5f < m_fTImeAcc && 40.f > m_fTImeAcc)
	{
		// 느낌표
		// 떨림

		m_fTImeAcc = 40.f;
	}
	else if (41.f < m_fTImeAcc && 50.f > m_fTImeAcc)
	{
		// 느낌표 사라짐

		// 점프
		m_bJump = true;
		CRM->Get_Cam()->Shake(0.4f, 0.2f, false, 0.1f);
		GI->PlaySoundW(L"ADV_012.ogg", SD_MAP, 1.f);

		m_bUpLookat = true;

		// 카메라포스 오락기 정면
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(131.863f, -12.96f, 103.161f);
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


		m_fTImeAcc = 50.f;
	}
	else if (53.f < m_fTImeAcc && 60.f > m_fTImeAcc)
	{
		// 삥 이펙트
		GI->PlaySoundW(L"GGST_142.wav", SD_MAP, 0.9f);
		m_bJump = false;
		m_bDonw = true;
		
		// x, z 맞춤
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_vector vIdlePos = XMVectorSet(141.7f, XMVectorGetY(vPos), 85.6f, 1.f);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vIdlePos);


		// 카메라포스 오락기 정면
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(141.603f, -13.977f, 94.226f);
		XMStoreFloat3(&CamDesc.vLook, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		CamDesc.vLook.y += 5.f;
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 5.f;
		CamDesc.fLookLerpSpeed = 5.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);


		m_fTImeAcc = 100.f;
	}




	if (m_bJump)
	{
		m_pTransformCom->Go_Dir(XMVectorSet(0.f, 1.f, 0.f, 0.f), 150.f, fTimeDelta);
	}
	else if (m_bDonw)
	{		
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		if (-15.7f < XMVectorGetY(vPos))
		{
			m_pTransformCom->Go_Dir(XMVectorSet(0.f, -1.f, 0.f, 0.f), 150.f, fTimeDelta);

			// 카메라포스 오락기 정면
			CCamera_Main::CUTDESC CamDesc;
			CamDesc.vPos = _float3(141.603f, -13.977f, 94.226f);
			XMStoreFloat3(&CamDesc.vLook, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			// CamDesc.vLook.y += 5.f;
			CamDesc.bLerp = true;
			CamDesc.bLookLerp = true;
			CamDesc.fLerpSpeed = 10.f;
			CamDesc.fLookLerpSpeed = 10.f;
			CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
			CamDesc.pTran = nullptr;
			CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
			CamDesc.bPixLocal = false;
			CRM->Get_Cam()->Show_Target(CamDesc);


			m_pTransformCom->LookAt_ForLandObject(XMVectorSetW(XMLoadFloat3(&CRM->Get_Cam()->Get_CamPos()), 1.f), true);
		
		}
		else
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(141.7f, -15.7f, 85.6f, 1.f)); // Idle
			
			m_bCutIdle = true;
			m_bDonw = false;
			GI->PlaySoundW(L"ADV_008.ogg", SD_MAP, 1.f);

			CCamera_Main::CUTDESC CamDesc;
			CamDesc.vPos = _float3(141.603f, -13.977f, 94.226f);
			XMStoreFloat3(&CamDesc.vLook, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			CamDesc.vLook.y += 1.f;
			CamDesc.bLerp = true;
			CamDesc.bLookLerp = true;
			CamDesc.fLerpSpeed = 10.f;
			CamDesc.fLookLerpSpeed = 10.f;
			CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
			CamDesc.pTran = nullptr;
			CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
			CamDesc.bPixLocal = false;
			CRM->Get_Cam()->Show_Target(CamDesc);



			CRM->Get_Cam()->Shake(0.6f, 0.2f, false, 0.1f);


			CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
			Info3->fBlurPower = 1.f;
			Info3->vLocalPos = { 0.f,0.f,0.f,1.f };
			Info3->vColor = { 0.7f,0.7f,0.7f,1.f };
			Info3->vColor2 = { 0.7f,0.7f,0.7f,1.f };
			Info3->vRatio = { 1.f,1.f };
			Info3->vRatioSpeed = { 1.4f,1.4f };
			Info3->vScaleSpeed = { 120.f,120.f,120.f };
			Info3->vScale = { 0.1f,0.1f,0.1f };
			Info3->bRotation = false;
			Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3)))
				return;




		}

	}


	if (m_bCutIdle)
	{
		m_fTimeCutIdleAcc += fTimeDelta;

		if (0.4f < m_fTimeCutIdleAcc && 10.f > m_fTimeCutIdleAcc)
		{
			// 오락기 불 켜진다
			TM->Get_FunchMachine()->Set_Start(true);
			UM->Set_PunchLight(true);

			// 카메라각 오락기 정면
			CCamera_Main::CUTDESC CamDesc;
			CamDesc.vPos = _float3(141.897f, -8.988f, 97.606f);
			XMStoreFloat3(&CamDesc.vLook, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			CamDesc.vLook.y += 5.5f;
			CamDesc.bLerp = true;
			CamDesc.bLookLerp = true;
			CamDesc.fLerpSpeed = 0.1f;
			CamDesc.fLookLerpSpeed = 0.1f;
			CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
			CamDesc.pTran = nullptr;
			CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
			CamDesc.bPixLocal = false;
			CRM->Get_Cam()->Show_Target(CamDesc);


			

			m_fTimeCutIdleAcc = 10.f;
		}
		else if (17.f < m_fTimeCutIdleAcc && 20.f > m_fTimeCutIdleAcc)
		{
			CRM->Get_Cam()->End_Target();
			Set_State(STATE_IDLE);
			TM->Get_FunchMachine()->Set_Start(false);
			m_fTimeCutIdleAcc = 20.f;


			UM->Set_TalkIndex(75);
		}

	}

	
		

}

void CFunchCuma::Tick_Idle(_float fTimeDelta)
{



	if (CK_DOWN(DIK_C))
	{
		Start();
		GI->PlaySoundW(L"minigame_common_start.wav", SD_MAP, 1.f);
	}

	if (!m_bStart)
	{
		UM->Set_QuestPoint(_float3(141.47f, -15.63f, 86.2f), 1);
		return;
	}
	else
		UM->Set_QuestPointOff();
		

	m_fGameTimeAcc += fTimeDelta;
	UM->Set_PunchTime(m_fGameTimeAcc);
	if (15.f < m_fGameTimeAcc)
	{
		// 실패
		CRM->Get_Cam()->End_CamDis();
		TM->Get_FunchMachine()->Set_Gage(0.f);
		m_bStart = false;
		GI->PlaySoundW(L"minigame3_fail.wav", SD_MAP, 1.f);
		
		// 데미지
		_float3 vDir;  XMStoreFloat3(&vDir, -1.f * XMLoadFloat3((_float3*)&PM->Get_PlayerWorldMatrix().m[2]));
		vDir.y = 0.f;
		PM->Set_SceneRealHit(PM->Get_SelectPlayer(), CCreture::TYPE_SPIN, 20.f, 8.f, PM->Get_PlayerPos(), vDir, 100.f);

		// m_eFace = FACE_SMILE;

		TM->Get_CumaHpBar()->Recorve_HpAcc();

		return;
	}
	

	if (m_fSussDamageScore < m_fDamageScore)
	{
		// 성공
		CRM->Get_Cam()->End_CamDis();
		TM->Get_FunchMachine()->Set_Gage(0.f);
		m_bStart = false;
		Set_State(STATE_ENDCUT);
		TM->Get_FunchMachine()->Set_Start(true);
		GI->PlaySoundW(L"END1.wav", SD_MAP, 1.f);

		UM->Set_FrontUI(false);

		return;
	}


	// m_fUV.y     +0.05f마다 한칸씩 올라간다.(초록->노랑->빨강)
	Check_Score();

	m_fDamageScore -= fTimeDelta * m_fMScore;
	if (0.f > m_fDamageScore)
		m_fDamageScore = 0.f;
	_float fRatio = m_fDamageScore / m_fSussDamageScore;
	TM->Get_FunchMachine()->Set_Gage(fRatio);

}

void CFunchCuma::Tick_EndCut(_float fTimeDelta)
{
	

	m_fTImeAcc += fTimeDelta;

	if (0.f < m_fTImeAcc && 10.f > m_fTImeAcc)
	{
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(141.603f, -13.977f, 94.226f);
		XMStoreFloat3(&CamDesc.vLook, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
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

		m_fTImeAcc = 10.f;

		m_bCutShake = true;

		m_bBoom = true;

	}
	else if (14.f < m_fTImeAcc && 20.f > m_fTImeAcc)
	{
		m_bJump = true;
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
		m_fTImeAcc = 20.f;
	}
	else if (21.f < m_fTImeAcc && 30.f > m_fTImeAcc)
	{

		// 카메라포스 오락기 정면
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = CRM->Get_Cam()->Get_CamPos();
		CamDesc.vLook = _float3(173.f, 3.902f, 143.591f);
		CamDesc.vLook.y += 5.f;
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 1.f;
		CamDesc.fLookLerpSpeed = 1.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);


		m_fTImeAcc = 30.f;
	}
	else if (31.f < m_fTImeAcc && 40.f > m_fTImeAcc)
	{
		m_bJump = false;
		m_bDonw = true;

		_float fY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(173.388f, fY, 143.591f, 1.f));

		m_fTImeAcc = 40.f;
	}



	if (m_bJump)
	{
		m_pTransformCom->Go_Dir(XMVectorSet(0.f, 1.f, 0.f, 0.f), 150.f, fTimeDelta);
	}
	else if (m_bDonw)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		if (3.902f < XMVectorGetY(vPos))
		{
			if (!m_bGoBalling)
			{
				GI->PlaySoundW(L"GGST_142.wav", SD_MAP, 0.9f);
				m_bGoBalling = true;
			}
			m_pTransformCom->Go_Dir(XMVectorSet(0.f, -1.f, 0.f, 0.f), 60.f, fTimeDelta);
		}
		else
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(173.388f, 2.902f, 143.591f, 1.f));
			m_bCutEnd = true;
			if (!m_bEndSound)
			{
				GI->PlaySoundW(L"ADV_008.ogg", SD_MAP, 1.f);
				m_bEndSound = true;
			}
		}

	}


	if (m_bCutEnd)
	{
		m_fTimeCutCut += fTimeDelta;
		if (2.f < m_fTimeCutCut)
		{
			Set_Dead();
			CRM->Get_Cam()->End_Target();
			TM->Open_Stage4Trigger(1);
			UM->Set_QuestPointOff();


			if (FAILED(GI->Add_GameObjectToLayer(TEXT("BallingCuma"), LEVEL_STATIC, TEXT("Layer_Balling"))))
				return;
			return;
		}

	}
	

	if (m_bCutShake)
	{
		m_fCutShakeTimeAcc += fTimeDelta;

		if (0.03f < m_fCutShakeTimeAcc)
		{
			Shake();
			m_fCutShakeTimeAcc = 0.f;
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

void CFunchCuma::Tick_Shake(_float fTimeDelta)
{
	/*_vector vInitPos = XMLoadFloat3(&m_vInitPos);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	*/
	m_fShakeEndTimeAcc += fTimeDelta;
	if (0.2f < m_fShakeEndTimeAcc)
	{
		m_bShake = false;
		m_eFace = FACE_IDLE;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vInitPos), 1.f));
		return;
	}


	m_fShakeTimeAcc += fTimeDelta;
	if (0.03f < m_fShakeTimeAcc)
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


HRESULT CFunchCuma::Ready_Components()
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


	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));


	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(2.f, 2.f, 2.f);
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.iFlag = 2;
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "Monster_Push_Range"); // 밀어내기 위함

	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(2.f, 2.f, 2.f);
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 0;
	strcpy(ColDesc.sTag, "Monster_Body1"); //몬스터 공격용 충돌체
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}



CFunchCuma * CFunchCuma::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFunchCuma*		pInstance = new CFunchCuma(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CFunchCuma"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFunchCuma::Clone(void * pArg)
{
	CFunchCuma*		pInstance = new CFunchCuma(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CFunchCuma"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFunchCuma::Free()
{
	__super::Free();

	if (m_pLight)
		m_pLight->Set_Dead();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pModelCom1);
	Safe_Release(m_pModelCom2);
	Safe_Release(m_pShaderCom);


}
