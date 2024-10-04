#include "stdafx.h"
#include "..\Public\DGCuma.h"
#include "GameInstance.h"


#include "ToolManager.h"
#include "Time_Manager.h"
#include "Camera_Manager.h"

#include "DGCuma_1.h"
#include "DGCuma_2.h"
#include "DGCuma_3.h"

#include "UI_Manager.h"

CDGCuma::CDGCuma(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CDGCuma::CDGCuma(const CDGCuma & rhs)
	: CCreture(rhs)
{
}

HRESULT CDGCuma::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CDGCuma::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_DG";
	Make_ImGui();



	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));


	CCamera_Main::CUTDESC CamDesc;
	CamDesc.vPos = _float3(-181.236f, 41.503f, 119.062f);
	CamDesc.vLook = *((_float3*)&m_pCuma_1->Get_WorldPos());
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




	return S_OK;
}

void CDGCuma::Tick(_float fTimeDelta)
{
	m_fTimeAcc += fTimeDelta;

	if (0.1f < m_fTimeAcc && 10.f > m_fTimeAcc)
	{
		if (!m_bMoveBlind)
		{
			m_bMoveBlind = true;
			UM->MoveBlind(false);
		}	

		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(-184.791f, 41.085f, 121.196f);
		CamDesc.vLook = *((_float3*)&m_pCuma_1->Get_WorldPos());
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
		GI->PlaySoundW(L"se_Env12_Mole_Opening_01.wav", SD_MAP, 1.f);


		m_fTimeAcc = 10.f;
	}
	else 	if (15.f < m_fTimeAcc && 20.f > m_fTimeAcc)
	{

		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(-183.162f, 41.632f, 125.564f);
		CamDesc.vLook = _float3(-188.445f, 40.481f, 125.915f);
		// CamDesc.vLook.y += 1.5f;
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 10.f;
		CamDesc.fLookLerpSpeed = 10.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);
		GI->PlaySoundW(L"CMN_MINIGAME_BNK_SE_0000001B.wav", SD_NPC, 1.f);


		m_fTimeAcc = 20.f;
	}
	else if (23.f < m_fTimeAcc && 30.f > m_fTimeAcc)
	{
		m_pCuma_1->Set_Down();
		m_fTimeAcc = 30.f;
		GI->PlaySoundW(L"se_Env12_Mole_Opening_02.wav", SD_MAP, 1.f);
	}
	else if (30.4f < m_fTimeAcc && 40.f > m_fTimeAcc)
	{
		m_pCuma_3->Set_Down();
		m_fTimeAcc = 40.f;
		GI->PlaySoundW(L"se_Env12_Mole_Opening_03.wav", SD_MAP, 1.f);
	}
	else if (42.f < m_fTimeAcc && 50.f > m_fTimeAcc)
	{
		m_pCuma_2->Set_Down();
		m_fTimeAcc = 50.f;
	}
	else if (54.f < m_fTimeAcc && 60.f > m_fTimeAcc)
	{
		CRM->Get_Cam()->End_Target();
		Set_Dead();
		m_fTimeAcc = 60.f;

		UM->Set_BattleTalkIndex(25);
		UM->Set_FrontUI(true);

		GI->PlaySoundW(L"minigame_common_start.wav", SD_MAP, 1.f);
		if (FAILED(GI->Add_GameObjectToLayer(TEXT("Bear"), LEVEL_STAGE4, TEXT("Layer_Bear"))))
			return;

		
	}


}

void CDGCuma::LateTick(_float fTimeDelta)
{

	if (nullptr == m_pRendererCom)
		return;





	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

}

HRESULT CDGCuma::Render()
{


	return S_OK;
}

// *[HeroJho]: 디버깅 창입니다. ImGui로 작동됩니다.
void CDGCuma::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);



	ImGui::End();
}

// *[HeroJho]: 충돌이 발생할 때 호출되는 함수입니다.
void CDGCuma::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{

}














HRESULT CDGCuma::Ready_Components()
{

	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;


	CGameObject* pObj = nullptr;
	if (FAILED(GI->Add_Get_GameObjectToLayer(TEXT("DGCuma_1"), LEVEL_STAGE4, TEXT("Layer_DG"), nullptr, &pObj)))
		return E_FAIL;
	m_pCuma_1 = (CDGCuma_1*)pObj;
	m_pCuma_1->Set_Face(CDGCuma_1::FACE_SMILE);

	if (FAILED(GI->Add_Get_GameObjectToLayer(TEXT("DGCuma_2"), LEVEL_STAGE4, TEXT("Layer_DG"), nullptr, &pObj)))
		return E_FAIL;
	m_pCuma_2 = (CDGCuma_2*)pObj;
	m_pCuma_2->Set_Face(CDGCuma_2::FACE_IDLE);

	if (FAILED(GI->Add_Get_GameObjectToLayer(TEXT("DGCuma_3"), LEVEL_STAGE4, TEXT("Layer_DG"), nullptr, &pObj)))
		return E_FAIL;
	m_pCuma_3 = (CDGCuma_3*)pObj;
	m_pCuma_3->Set_Face(CDGCuma_3::FACE_HURT);



	return S_OK;
}



CDGCuma * CDGCuma::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDGCuma*		pInstance = new CDGCuma(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CDGCuma"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDGCuma::Clone(void * pArg)
{
	CDGCuma*		pInstance = new CDGCuma(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CDGCuma"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDGCuma::Free()
{
	__super::Free();



}
