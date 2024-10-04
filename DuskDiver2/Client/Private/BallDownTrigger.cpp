#include "stdafx.h"
#include "..\Public\BallDownTrigger.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"
#include "Camera_Manager.h"
#include "Camera_Main.h"
#include "Monster_Manager.h"
#include "Player_Manager.h"
#include "Player01SmokeBall.h"

CBallDownTrigger::CBallDownTrigger(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CBallDownTrigger::CBallDownTrigger(const CBallDownTrigger & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBallDownTrigger::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBallDownTrigger::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Potal";

	Make_ImGui();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(1.207f, 15.679f, 130.935f, 1.f));

	PM->Set_ScenePos(PM->Get_SelectPlayer(), _float4(1.273f, 15.681f, 131.145f, 1.f));
	PM->Set_SceneLook(PM->Get_SelectPlayer(), _float4(1.226f, 16.995f, 132.905f, 1.f));


	return S_OK;
}

void CBallDownTrigger::Tick(_float fTimeDelta)
{

	Emp_Col();

	if (!m_bOn)
		return;

	m_fTimeAcc += fTimeDelta;

	if (1.f < m_fTimeAcc && 10.f > m_fTimeAcc)
	{
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(-1.41f, 18.103f, 122.759f);
		CamDesc.vLook = _float3(-0.663f, 17.512f, 125.954f);
		CamDesc.bLerp = false;
		CamDesc.bLookLerp = false;
		CamDesc.fLerpSpeed = 0.f;
		CamDesc.fLookLerpSpeed = 0.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);


		m_fTimeAcc = 10.f;
		// 특정 타이밍,+ bool값조건하에 한번만 블라인드를 움직여 화면을 보여준다
		if (10.f == m_fTimeAcc)
		{
			UM->MoveBlind(false);
		}
	}
	else if (11.f < m_fTimeAcc && 20.f > m_fTimeAcc)
	{ // 오락 공 떨어진다.
		MM->Arcade_Event3();


		m_fTimeAcc = 20.f;
	}
	else if (22.f < m_fTimeAcc && 30.f > m_fTimeAcc)
	{ 
		CRM->Get_Cam()->Shake(0.6f, 0.2f, false, 0.1f);

		_float3 vDir;  XMStoreFloat3(&vDir, -1.f * XMLoadFloat3((_float3*)&PM->Get_PlayerWorldMatrix().m[2]));
		vDir.y = 0.f;
		PM->Set_SceneHit(PM->Get_SelectPlayer(), CCreture::TYPE_AWAY, 9.f, 7.f, PM->Get_PlayerPos(), vDir);

		GI->PlaySoundW(L"ADV_075.ogg", SD_MAP, 1.f); 
		GI->PlaySoundW(L"SwordHit.ogg", SD_PLAYER1_SK, 0.5f);
		GI->PlaySoundW(L"Hero01_ba_16.wav", SD_PLAYER1_VO, 0.6f);

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
		Info2->vScaleSpeed = { 300.f,300.f,300.f};
		Info2->vUvFixSpeed = { -1.f,-3.f };
		Info2->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2);




		m_fTimeAcc = 30.f;
	}
	else if (34.f < m_fTimeAcc && 40.f > m_fTimeAcc)
	{

		if (FAILED(GI->Add_GameObjectToLayer(TEXT("BallDownCuma"), LEVEL_STAGE4, TEXT("Layer_Cuma"))))
			return;

		Set_Dead();

		m_fTimeAcc = 40.f;
	}
	//else if (30.f < m_fTimeAcc && 40.f > m_fTimeAcc)
	//{

	//	m_fTimeAcc = 40.f;
	//}
	//else if (30.f < m_fTimeAcc && 40.f > m_fTimeAcc)
	//{

	//	m_fTimeAcc = 40.f;
	//}


	// 유모 괜찮아 대화 나오면 트리거를 삭제
	if (69 == UM->Get_TalkIndex())
		Set_Dead();



}

void CBallDownTrigger::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	Choose_Col(nullptr);
	Render_Col(m_pRendererCom);
}

HRESULT CBallDownTrigger::Render()
{
	return S_OK;
}

void CBallDownTrigger::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);


	ImGui::End();
}

void CBallDownTrigger::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{

		if ((!strcmp("Potal", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Body", Desc.pOtherDesc->sTag)))
		{
			m_bOn = true;
			UM->Set_FrontUI(false);
			if (false == m_bBlindMove)
			{
				m_bBlindMove = true;
				UM->MoveBlind(true);
			}
		
		}

	}
}



HRESULT CBallDownTrigger::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;




	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(2.f, 2.f, 2.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Potal");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}





CBallDownTrigger * CBallDownTrigger::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBallDownTrigger*		pInstance = new CBallDownTrigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBallDownTrigger"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBallDownTrigger::Clone(void * pArg)
{
	CBallDownTrigger*		pInstance = new CBallDownTrigger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBallDownTrigger"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CBallDownTrigger::Free()
{
	__super::Free();



	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
