#include "stdafx.h"
#include "..\Public\DGEndCuma.h"
#include "GameInstance.h"


#include "ToolManager.h"
#include "Time_Manager.h"
#include "Camera_Manager.h"
#include "UI_Manager.h"
#include "Monster_Manager.h"

#include "EPotal.h"
#include "Player01SmokeBall.h"

CDGEndCuma::CDGEndCuma(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CDGEndCuma::CDGEndCuma(const CDGEndCuma & rhs)
	: CCreture(rhs)
{
}

HRESULT CDGEndCuma::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CDGEndCuma::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Boss";
	Make_ImGui();

	m_pTransformCom->Set_Scale(XMVectorSet(10.f, 10.f, 10.f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-188.540f, 38.7f, 123.563f, 1.f));

	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(-60.f));

	UM->MoveBlind(true);

	return S_OK;
}

void CDGEndCuma::Tick(_float fTimeDelta)
{
	if (!m_bEndSound)
	{
		//GI->PlaySound_Linear(L"Arcade.wav", SD_BGM, 0.2f, true, 0.4f);
		//GI->StopSound(SD_BGM);
		//GI->PlayBGM(L"Arcade.wav", 0.2f);
		m_bEndSound = true;
	}

	m_fTimeAcc += fTimeDelta;



	if (1.f < m_fTimeAcc && 10.f > m_fTimeAcc)
	{
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(-181.236f, 41.503f, 119.062f);
		CamDesc.vLook = _float3(-188.540f, 38.7f, 123.563f);
		CamDesc.vLook.y += 1.5f;
		CamDesc.bLerp = false;
		CamDesc.bLookLerp = false;
		CamDesc.fLerpSpeed = 5.f;
		CamDesc.fLookLerpSpeed = 5.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);
		// GI->PlaySoundW(L"GGST_254.ogg", SD_MAP, 1.f);
		m_fTimeAcc = 10.f;
	}
	else if (11.f < m_fTimeAcc && 20.f > m_fTimeAcc)
	{
		UM->MoveBlind(false);

		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(-184.791f, 41.085f, 121.196f);
		CamDesc.vLook = _float3(-188.540f, 38.7f, 123.563f);
		CamDesc.vLook.y += 1.5f;
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 0.1f;
		CamDesc.fLookLerpSpeed = 0.1f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);

		m_fTimeAcc = 20.f;
	}
	else if (23.f < m_fTimeAcc && 30.f > m_fTimeAcc)
	{

		m_bJump = true;
		CRM->Get_Cam()->Shake(0.6f, 0.2f, false, 0.1f);
		m_bRealShake = false;
		GI->PlaySoundW(L"ADV_012.ogg", SD_MAP, 1.f);


		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = CRM->Get_Cam()->Get_CamPos();
		CamDesc.vLook = CRM->Get_Cam()->Get_PreLook();
		CamDesc.vLook.y += 3.f;
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 10.f;
		CamDesc.fLookLerpSpeed = 10.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);



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




		m_fTimeAcc = 30.f;
	}
	else if (34.f < m_fTimeAcc && 40.f > m_fTimeAcc)
	{

		m_bJump = false;
		m_bDown = true;
		GI->PlaySoundW(L"GGST_142.wav", SD_MAP, 0.9f);



		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = CRM->Get_Cam()->Get_CamPos();
		CamDesc.vPos.y += 4.f;
		CamDesc.vLook = _float3(2.2f, 15.531f, 163.164f);
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 5.f;
		CamDesc.fLookLerpSpeed = 5.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);

		_float fY = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(2.2f, fY, 163.164f, 1.f));



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



		m_fTimeAcc = 40.f;
	}




	if (m_bJump)
	{
		m_pTransformCom->Go_Dir(XMVectorSet(0.f, 1.f, 0.f, 0.f), 120.f, fTimeDelta);
	}
	else if (m_bDown)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		if (15.531f < XMVectorGetY(vPos))
		{
			m_pTransformCom->Go_Dir(XMVectorSet(0.f, -1.f, 0.f, 0.f), 120.f, fTimeDelta);
		}
		else
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(2.2f, 15.531f, 163.164f, 1.f));

			// m_bCutEnd = true;
			m_bCrane = true;
			m_bRealShake = true;
			if (!m_bJumpEndSound)
			{
				GI->PlaySoundW(L"ADV_008.ogg", SD_MAP, 1.f);
				m_bJumpEndSound = true;
			}
		}
	}



	if (m_bCrane)
	{
		m_fCraneTimeAcc += fTimeDelta;
		if (1.f < m_fCraneTimeAcc && 10.f > m_fCraneTimeAcc)
		{
			CCamera_Main::CUTDESC CamDesc;
			CamDesc.vPos = _float3(-31.138f, 53.233f, 114.369f);
			CamDesc.vLook = _float3(-22.065f, 47.832f, 123.249f);
			CamDesc.bLerp = true;
			CamDesc.bLookLerp = true;
			CamDesc.fLerpSpeed = 10.f;
			CamDesc.fLookLerpSpeed = 5.f;
			CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
			CamDesc.pTran = nullptr;
			CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
			CamDesc.bPixLocal = false;
			CRM->Get_Cam()->Show_Target(CamDesc);


			m_fCraneTimeAcc = 10.f;
		}
		else if (11.f < m_fCraneTimeAcc && 20.f > m_fCraneTimeAcc)
		{
			// 들어 올린다.
			MM->Arcade_Event1();
			CRM->Get_Cam()->Shake(0.6f, 0.2f, false, 0.1f);

			m_fCraneTimeAcc = 20.f;
		}
		else if (32.f < m_fCraneTimeAcc && 40.f > m_fCraneTimeAcc)
		{
			// 들어 올린다.
			m_bCutEnd = true;


			m_fCraneTimeAcc = 40.f;
		}

	}


	// DGEndCuma::Tick() 두더지 도망 컷 신 종료
	if (m_bCutEnd)
	{
		m_fEndTimeAcc += fTimeDelta;

		CRM->Get_Cam()->End_Target();
		Set_Dead();

		// TM->Make_EPotal();
		TM->Get_EPotal()->Set_CutScene();

		MM->Spawn_MechanicalOctopus();
	}




	if (m_bRealShake)
	{
		m_fTimeShakeAcc += fTimeDelta;
		if (0.3f < m_fTimeShakeAcc)
		{
			m_bShake = true;
			m_fTimeShakeAcc = 0.f;
		}

		if (m_bShake)
			Tick_Shake(fTimeDelta);

	}
	
}

void CDGEndCuma::LateTick(_float fTimeDelta)
{

	if (nullptr == m_pRendererCom)
		return;



	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

}

HRESULT CDGEndCuma::Render()
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
void CDGEndCuma::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);



	ImGui::End();
}

// *[HeroJho]: 충돌이 발생할 때 호출되는 함수입니다.
void CDGEndCuma::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{

}




HRESULT CDGEndCuma::Ready_Components()
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




void CDGEndCuma::Shake()
{
	m_bShake = true;
	m_bShakeTurn = false;
	m_fShakeTimeAcc = 0.f;
	m_fShakeEndTimeAcc = 0.f;
	m_eFace = FACE_HURT;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vInitPos), 1.f));
}

void CDGEndCuma::Tick_Shake(_float fTimeDelta)
{
	/*_vector vInitPos = XMLoadFloat3(&m_vInitPos);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	*/
	m_fShakeEndTimeAcc += fTimeDelta;
	if (0.2f < m_fShakeEndTimeAcc)
	{
		m_bShake = false;
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



CDGEndCuma * CDGEndCuma::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDGEndCuma*		pInstance = new CDGEndCuma(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CDGEndCuma"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDGEndCuma::Clone(void * pArg)
{
	CDGEndCuma*		pInstance = new CDGEndCuma(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CDGEndCuma"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDGEndCuma::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pModelCom1);
	Safe_Release(m_pModelCom2);
	Safe_Release(m_pShaderCom);


}
