#include "stdafx.h"
#include "..\Public\BallDownCuma.h"
#include "GameInstance.h"


#include "ToolManager.h"
#include "Time_Manager.h"
#include "Camera_Manager.h"
#include "UI_Manager.h"

#include "Player_Manager.h"
#include "Monster_Manager.h"
#include "Particle_Manager.h"



CBallDownCuma::CBallDownCuma(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CBallDownCuma::CBallDownCuma(const CBallDownCuma & rhs)
	: CCreture(rhs)
{
}

HRESULT CBallDownCuma::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CBallDownCuma::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Boss";
	Make_ImGui();



	m_pTransformCom->Set_Scale(XMVectorSet(10.f, 10.f, 10.f, 1.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(1.558f, 18.839f, 139.164f, 1.f));

	m_bLittleUp = true;

	m_fUpHight = Get_WorldPos().y;


	TM->Make_CumaHp();

	return S_OK;
}

void CBallDownCuma::Tick(_float fTimeDelta)
{
	//if (CK_DOWN(DIK_F8))
	//{
	//	Set_Dead();
	//}

	m_pTransformCom->LookAt_ForLandObject(XMLoadFloat4(&PM->Get_PlayerPos()), true);

	if (m_bLittleUp)
	{
		m_pTransformCom->Go_Dir(XMVectorSet(0.f, 1.f, 0.f, 0.f), 1.f, fTimeDelta);


		//쿠마를 본다.
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = CRM->Get_Cam()->Get_CamPos();
		CamDesc.vPos.y += 1.f;
		XMStoreFloat3(&CamDesc.vLook, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		CamDesc.vLook.y += 2.f;
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 0.5f;
		CamDesc.fLookLerpSpeed = 0.5f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.5f, -1.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);


		if (m_fUpHight + 2.5f < Get_WorldPos().y)
		{
			m_bLittleUp = false;

		}
	}


	if(!m_bLittleUp)
		m_fTimeAcc += fTimeDelta;

	if (1.f < m_fTimeAcc && 10.f > m_fTimeAcc)
	{
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = CRM->Get_Cam()->Get_CamBackPos(0, 0, 10.f);
		XMStoreFloat3(&CamDesc.vLook, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		CamDesc.vLook.y += 2.f;
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 5.f;
		CamDesc.fLookLerpSpeed = 5.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.5f, -1.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);



		m_eFace = FACE_SMILE;
		Shake();


		_float4x4 vM; XMStoreFloat4x4(&vM, m_pTransformCom->Get_WorldMatrix_NoScale());

		PTM->Create_InstanceParticle(L"BearCutLight_2", vM);
		GI->PlaySoundW(L"ARC_BTL_SYS_GaugeCount.ogg", SD_NPC, 0.7f);


		m_fTimeAcc = 10.f;
	}
	else if(12.f < m_fTimeAcc && 20.f > m_fTimeAcc)
	{
		m_bJump = true;
		CRM->Get_Cam()->Shake(0.6f, 0.2f, false, 0.1f);
		GI->PlaySoundW(L"GGST_142.wav", SD_MAP, 1.f);


		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = CRM->Get_Cam()->Get_CamPos();
		XMStoreFloat3(&CamDesc.vLook, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		CamDesc.vLook.y += 4.f;
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 1.f;
		CamDesc.fLookLerpSpeed = 5.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.5f, -1.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);



		m_fTimeAcc = 20.f;
	}
	else if (22.f < m_fTimeAcc && 30.f > m_fTimeAcc)
	{

		Set_Dead();
		MM->Stage4Scene_On();
		CRM->Get_Cam()->End_Target();
		CRM->Get_Cam()->Set_CamPos(0.f, 20.f);
		m_fTimeAcc = 30.f;
	

		UM->Set_TalkIndex(69);
		UM->Set_FrontUI(false);
		UM->Set_QuestPointOff();

		return;
	}








	if (m_bJump)
	{
		m_pTransformCom->Go_Dir(XMVectorSet(0.f, 1.f, 0.f, 0.f), 80.f, fTimeDelta);
	}




	if (m_bShake)
		Tick_Shake(fTimeDelta);


}

void CBallDownCuma::LateTick(_float fTimeDelta)
{

	if (nullptr == m_pRendererCom)
		return;



	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

}

HRESULT CBallDownCuma::Render()
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
void CBallDownCuma::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);



	ImGui::End();
}

// *[HeroJho]: 충돌이 발생할 때 호출되는 함수입니다.
void CBallDownCuma::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{

}




HRESULT CBallDownCuma::Ready_Components()
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




void CBallDownCuma::Shake()
{
	m_bShake = true;
	m_bShakeTurn = false;
	m_fShakeTimeAcc = 0.f;
	m_fShakeEndTimeAcc = 0.f;
}

void CBallDownCuma::Tick_Shake(_float fTimeDelta)
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

void CBallDownCuma::Hit()
{
	m_eFace = FACE_HURT;




}



CBallDownCuma * CBallDownCuma::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBallDownCuma*		pInstance = new CBallDownCuma(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBallDownCuma"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBallDownCuma::Clone(void * pArg)
{
	CBallDownCuma*		pInstance = new CBallDownCuma(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBallDownCuma"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBallDownCuma::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pModelCom1);
	Safe_Release(m_pModelCom2);
	Safe_Release(m_pShaderCom);


}
