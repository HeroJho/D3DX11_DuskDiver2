#include "stdafx.h"
#include "..\Public\UpPotal.h"
#include "GameInstance.h"


#include "Player_Manager.h"
#include "ToolManager.h"
#include "Time_Manager.h"
#include "Camera_Manager.h"
#include "UI_Manager.h"

#include "Particle_Manager.h"


CUpPotal::CUpPotal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCreture(pDevice, pContext)
{
}

CUpPotal::CUpPotal(const CUpPotal& rhs)
	: CCreture(rhs)
{
}

HRESULT CUpPotal::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CUpPotal::Initialize(void* pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Potal";
	Make_ImGui();

	m_fHurtTime = 0.02f;
	m_fMaxHp = 1000.f;
	m_fNowHp = m_fMaxHp;
	m_fStamina = 1.f;
	//m_fDamage = 20.f;

	m_fPreHp = m_fNowHp;


	// 59.489f, 33.729f, 132.072f
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(52.914f, 0.f, 139.27f, 1.f));



	return S_OK;
}

void CUpPotal::Tick(_float fTimeDelta)
{
	Emp_Col();


	m_fParticleAcc += fTimeDelta;
	if (0.7f < m_fParticleAcc)
	{
		_matrix vvM = XMMatrixIdentity();
		vvM.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 1.f, 0.f, 1.f);
		_float4x4 vM; XMStoreFloat4x4(&vM, vvM);
		PTM->Create_InstanceParticle(L"Portal", vM);

		m_fParticleAcc = 0.f;
	}

	// For. 기능
	if (m_bTrigger && CK_DOWN(DIK_C))
	{
		m_bPotal = true;
		// UM->Set_FrontUI(false);
		// m_fTimeAcc = 0.f;
		// GI->PlaySoundW(L"PotalOn.wav", SD_MAP, 0.7f);

		_matrix vvM = XMMatrixIdentity();
		vvM.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float4x4 vM; XMStoreFloat4x4(&vM, vvM);
		//PTM->Create_InstanceParticle(L"PortalDown", vM);
		PM->PlayerPortal();
		PM->Set_ScenePos(PM->Get_SelectPlayer(), _float4(59.489f, 33.729f, 132.072f, 1.f));
		


		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(50.94f, 45.833f, 139.705f);
		CamDesc.vLook = _float3(54.663f, 42.492f, 135.904f);
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 2.f;
		CamDesc.fLookLerpSpeed = 2.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);

		GI->PlaySoundW(L"se_Env12_Platform_Reach.wav", SD_MAP, 1.f);
	}


	if (m_bPotal)
	{
		m_fTimeAcc += fTimeDelta;
		if (2.f < m_fTimeAcc)
		{
			CRM->Get_Cam()->Set_CamPos(0.f, 20.f);
			CRM->Get_Cam()->End_Target();

			m_bPotal = false;
			m_fTimeAcc = 0.f;
		}
	}
	



}


void CUpPotal::LateTick(_float fTimeDelta)
{

	if (nullptr == m_pRendererCom)
		return;



	Choose_Col(nullptr);
	Tick_Col(m_pTransformCom->Get_WorldMatrix());
	Render_Col(m_pRendererCom);



	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	m_bTrigger = false;
}

HRESULT CUpPotal::Render()
{
	


	return S_OK;
}

// *[HeroJho]: 디버깅 창입니다. ImGui로 작동됩니다.
void CUpPotal::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);



	ImGui::End();
}

// *[HeroJho]: 충돌이 발생할 때 호출되는 함수입니다.
void CUpPotal::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{

	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		// if (m_bCanUse)
		{
			UM->Set_TalkButtonType(6);
			UM->Set_TalkButton(true);

			m_bTrigger = true;
		}
	}

}

void CUpPotal::On_Hit(CCreture* pOther)
{
	//hit이 한번만 처리되게끔
	if (m_bHurt)
	{
		m_bHurt = true;


	}
}





HRESULT CUpPotal::Ready_Components()
{

	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	///* For.Com_Shader */
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
	//	return E_FAIL;

	///* For.Com_Model */
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_Portal"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
	//	return E_FAIL;



	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));


	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(2.f, 2.f, 2.f);
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.iFlag = 2;
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "Potal_Range"); // 밀어내기 위함

	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;



	return S_OK;
}



CUpPotal* CUpPotal::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUpPotal* pInstance = new CUpPotal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CUpPotal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CUpPotal::Clone(void* pArg)
{
	CUpPotal* pInstance = new CUpPotal(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CUpPotal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUpPotal::Free()
{
	__super::Free();


	// Safe_Release(m_pModelCom);
	// Safe_Release(m_pShaderCom);


}
