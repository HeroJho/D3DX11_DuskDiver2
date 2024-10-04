#include "stdafx.h"
#include "..\Public\BallingCuma.h"
#include "GameInstance.h"


#include "ToolManager.h"
#include "Time_Manager.h"
#include "Camera_Manager.h"
#include "UI_Manager.h"
#include "Monster_Manager.h"
#include "Player_Manager.h"
#include "Particle_Manager.h"
#include "Player01SmokeBall.h"


CBallingCuma::CBallingCuma(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CBallingCuma::CBallingCuma(const CBallingCuma & rhs)
	: CCreture(rhs)
{
}

HRESULT CBallingCuma::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CBallingCuma::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Boss";
	Make_ImGui();



	m_pTransformCom->Set_Scale(XMVectorSet(10.f, 10.f, 10.f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(172.4f, 3.902f, 150.566f, 1.f));

	m_vInitPos = { 172.4f, 3.902f, 150.566f };

	TM->Set_BallingCuma(this);

	return S_OK;
}

void CBallingCuma::Tick(_float fTimeDelta)
{
	if (CK_DOWN(DIK_F8))
	{
		Set_Dead();
	}



	switch (m_eState)
	{
	case STATE_IDLE:
		Tick_Idle(fTimeDelta);
		break;
	case STATE_RUN:
		Tick_Run(fTimeDelta);
		break;
	case STATE_HAKI:
		Tick_Haki(fTimeDelta);
		break;
	case STATE_HAKI_RUN:
		Tick_Haki_Run(fTimeDelta);
		break;
	default:
		break;
	}







	if (m_bRealShake)
	{
		m_fTimeShakeAcc += fTimeDelta;
		if (0.3f < m_fTimeShakeAcc)
			m_bShake = true;

		if (m_bShake)
			Tick_Shake(fTimeDelta);

	}

}

void CBallingCuma::LateTick(_float fTimeDelta)
{

	if (nullptr == m_pRendererCom)
		return;



	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

}

HRESULT CBallingCuma::Render()
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
void CBallingCuma::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);



	ImGui::End();
}

// *[HeroJho]: 충돌이 발생할 때 호출되는 함수입니다.
void CBallingCuma::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{

}

void CBallingCuma::Tick_Idle(_float fTimeDelta)
{
	_vector vPPos = XMLoadFloat4(&PM->Get_PlayerPos());
	_vector vMPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float fDis = XMVectorGetX(XMVector3Length(vPPos - vMPos));

	// 플레이어가 가까이오면 몬스터 소환 후 도망
	if (4.f > fDis)
	{
		// 몬스터 소환
		if (!m_bOneSpawn)
		{
			m_bOneSpawn = true;
			MM->Spawn_Stage4_5();
			GI->PlaySoundW(L"ARC_BTL_SYS_GaugeCount.ogg", SD_NPC, 0.7f);
		}


		UM->Set_FrontUI(false);

		m_eState = STATE_RUN;
		m_eFace = FACE_SMILE;
		m_fTimeAcc = 0.f;


		_float4x4 vM; XMStoreFloat4x4(&vM, m_pTransformCom->Get_WorldMatrix_NoScale());
		PTM->Create_InstanceParticle(L"BearCutLight_2", vM);
		GI->PlaySoundW(L"ARC_BTL_SYS_GaugeCount.ogg", SD_NPC, 0.7f);



	}

	m_fTimeAcc += fTimeDelta;
	if (0.8f < m_fTimeAcc)
	{
		Shake();
		m_fTimeAcc = 0.f;
	}


	m_pTransformCom->LookAt_ForLandObject(vPPos, true);
}

void CBallingCuma::Tick_Run(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (1.f < m_fTimeAcc && 10.f > m_fTimeAcc)
	{
		m_bJump = true;
		CRM->Get_Cam()->Shake(0.6f, 0.2f, false, 0.1f);
		m_bRealShake = false;

		m_fTimeAcc = 10.f;

		m_bHoldCam = true;
		UM->Set_QuestPointOff();
		GI->PlaySoundW(L"ADV_012.ogg", SD_MAP, 1.f);

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





	}
	else if (11.f < m_fTimeAcc && 20.f > m_fTimeAcc)
	{
		m_bJump = false;
		m_bDown = true;
		GI->PlaySoundW(L"GGST_142.wav", SD_MAP, 0.9f);

		_float fY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(106.103f, fY, 222.074f, 1.f));

		m_fTimeAcc = 20.f;
	}



	if (m_bJump)
	{
		m_pTransformCom->Go_Dir(XMVectorSet(0.f, 1.f, 0.f, 0.f), 80.f, fTimeDelta);
	}
	else if (m_bDown)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		if (-31.725f < XMVectorGetY(vPos))
		{
			m_pTransformCom->Go_Dir(XMVectorSet(0.f, -1.f, 0.f, 0.f), 60.f, fTimeDelta);
		}
		else if(!m_bCutEnd)
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(106.103f, -31.725f, 222.074f, 1.f));
			CRM->Get_Cam()->Shake(0.6f, 0.2f, false, 0.1f);
			m_bCutEnd = true;
			GI->PlaySoundW(L"ADV_008.ogg", SD_MAP, 1.f);

			UM->Set_QuestPoint(_float3(106.103f, -31.725f, 222.074f), 1);
		}
	}

	if (m_bCutEnd)
	{
		m_fEndTimeAcc += fTimeDelta;

		if (1.f < m_fEndTimeAcc)
		{
		

			/*if (m_bOneControl)
			{
				MM->Control_Beta();
				MM->Control_Fusion();
				MM->Control_BombRobot();
				MM->Control_DefenceRobot();
				m_bOneControl = false;
			}
*/
		
			m_bHoldCam = false;
			CRM->Get_Cam()->End_Target();
			m_fTimeAcc = 0.f;
			// Set_Dead();


			UM->Set_BattleTalkIndex(11);
			UM->Set_FrontUI(true);

			if(4.f < m_fEndTimeAcc)
				m_eState = STATE_HAKI;
		}
	}

	if(m_bHoldCam)
	{
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(175.952f, 32.801f, 114.751f);
		XMStoreFloat3(&CamDesc.vLook, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		CamDesc.vLook.y += 1.5f;
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 5.f;
		CamDesc.fLookLerpSpeed = 0.5f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);
	}
}

void CBallingCuma::Tick_Haki(_float fTimeDelta)
{
	m_fTurnAngle += fTimeDelta * 0.5f;
	_matrix RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fTurnAngle);
	_vector vDir = XMVector3TransformCoord(XMVectorSet(1.f, 0.f, 0.f, 0.f), RotationMatrix);

	_vector vPos = XMVectorSet(99.141f, -29.725f, 218.888f, 1.f) + vDir * 15.f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vPos, 1.f));

	_vector vPPos = XMLoadFloat4(&PM->Get_PlayerPos());
	m_pTransformCom->LookAt_ForLandObject(vPPos, true);


	if (m_bShake)
		Tick_Shake(fTimeDelta);



	if (!m_bHakiEnd)
		return;

	m_fHakiEndTimeAcc += fTimeDelta;
	if (4.f < m_fHakiEndTimeAcc && 10.f > m_fHakiEndTimeAcc)
	{
		End_Game();
		UM->Set_FrontUI(false);
		m_fHakiEndTimeAcc = 100.f;
	}

}

void CBallingCuma::Tick_Haki_Run(_float fTimeDelta)
{
	if (!m_bEndSound)
	{
		//GI->StopSound(SD_BGM);
		//GI->PlayBGM(L"Arcade.wav", 0.2f);
		GI->PlaySoundW(L"minigame1_collision_meteor.wav", SD_MAP, 1.f);
		m_bEndSound = true;
	}

	m_fTimeAcc += fTimeDelta;
	

	if (10.f > m_fHakiTimeAcc)
	{
		m_fHakiTimeAcc += fTimeDelta;
		if (2.f < m_fHakiTimeAcc)
		{
			CCamera_Main::CUTDESC CamDesc;
			CamDesc.vPos = CRM->Get_Cam()->Get_CamPos();
			CamDesc.vLook = CRM->Get_Cam()->Get_PreLook();
			CamDesc.bLerp = true;
			CamDesc.bLookLerp = true;
			CamDesc.fLerpSpeed = 10.f;
			CamDesc.fLookLerpSpeed = 10.f;
			CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
			CamDesc.pTran = nullptr;
			CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
			CamDesc.bPixLocal = false;
			CRM->Get_Cam()->Show_Target(CamDesc);


			m_fHakiTimeAcc = 10.f;
			m_bBoom = true;
		}

	}



	if (2.f < m_fTimeAcc && 10.f > m_fTimeAcc)
	{
		m_bJump = true;
		CRM->Get_Cam()->Shake(0.6f, 0.2f, false, 0.1f);
		m_bRealShake = false;
		GI->PlaySoundW(L"ADV_012.ogg", SD_MAP, 1.f);

		m_fTimeAcc = 10.f;

		m_bHoldCam = true;

		m_bBoom = true;

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



	}
	else if (11.f < m_fTimeAcc && 20.f > m_fTimeAcc)
	{
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(-1.08f, 173.524f, 50.586f);
		CamDesc.vLook = _float3(-98.455f, -2.263f, 131.533f);
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 5.f;
		CamDesc.fLookLerpSpeed = 1.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);

		m_fTimeAcc = 20.f;
	}
	else if (22.f < m_fTimeAcc && 30.f > m_fTimeAcc)
	{
		m_bJump = false;
		m_bDown = true;


		_float fY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-83.917f, fY, 130.985f, 1.f));

		m_fTimeAcc = 30.f;
	}



	if (m_bJump)
	{
		m_pTransformCom->Go_Dir(XMVectorSet(0.f, 1.f, 0.f, 0.f), 80.f, fTimeDelta);
	}
	else if (m_bDown)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		if (23.916f < XMVectorGetY(vPos))
		{
			m_pTransformCom->Go_Dir(XMVectorSet(0.f, -1.f, 0.f, 0.f), 80.f, fTimeDelta);
		}
		else if (!m_bCutEnd)
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-83.917f, 23.916f, 130.985f, 1.f));
			CRM->Get_Cam()->Shake(0.6f, 0.2f, false, 0.1f);
			m_bCutEnd = true;
			GI->PlaySoundW(L"ADV_008.ogg", SD_MAP, 1.f);
		}
	}

	if (m_bCutEnd)
	{
		m_fEndTimeAcc += fTimeDelta;

		if (1.f < m_fEndTimeAcc)
		{
			// CBallingCuma::Tick_Haki_Run() 하키 끝

			TM->Open_Stage4Trigger(4);
			m_fTimeAcc = 0.f;
			Set_Dead();
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




HRESULT CBallingCuma::Ready_Components()
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


	return S_OK;
}




void CBallingCuma::Shake()
{
	m_bShake = true;
	m_bShakeTurn = false;
	m_fShakeTimeAcc = 0.f;
	m_fShakeEndTimeAcc = 0.f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vInitPos), 1.f));
}

void CBallingCuma::Tick_Shake(_float fTimeDelta)
{
	/*_vector vInitPos = XMLoadFloat3(&m_vInitPos);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	*/
	m_fShakeEndTimeAcc += fTimeDelta;
	if (0.2f < m_fShakeEndTimeAcc)
	{
		m_bShake = false;
		if(STATE_HAKI == m_eState)
			m_eFace = FACE_IDLE;
		return;
	}


	m_fShakeTimeAcc += fTimeDelta;
	if (0.04f < m_fShakeTimeAcc)
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

void CBallingCuma::Hit()
{
	m_eFace = FACE_HURT;
	Shake();
}

void CBallingCuma::End_Game()
{

	m_fTimeAcc = 0.f;
	m_bJump = false;
	m_bDown = false;
	m_bRealShake = true;
	m_bCutEnd = false;

	m_fEndTimeAcc = 0.f;

	m_fTimeShakeAcc = 0.f;

	m_eState = STATE_HAKI_RUN;

	UM->Set_FrontUI(false);


	CCamera_Main::CUTDESC CamDesc;
	CamDesc.vPos = _float3(0.f, 0.f, 0.f);
	CamDesc.vLook = _float3(0.f, 0.f, 0.f);
	CamDesc.bLerp = true;
	CamDesc.bLookLerp = true;
	CamDesc.fLerpSpeed = 5.f;
	CamDesc.fLookLerpSpeed = 5.f;
	CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
	CamDesc.pTran = m_pTransformCom;
	CamDesc.vPosDis = _float3(0.f, 0.5f, -1.f);
	CamDesc.bPixLocal = true;
	CRM->Get_Cam()->Show_Target(CamDesc);
}



CBallingCuma * CBallingCuma::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBallingCuma*		pInstance = new CBallingCuma(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBallingCuma"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBallingCuma::Clone(void * pArg)
{
	CBallingCuma*		pInstance = new CBallingCuma(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBallingCuma"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBallingCuma::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pModelCom1);
	Safe_Release(m_pModelCom2);
	Safe_Release(m_pShaderCom);


}
