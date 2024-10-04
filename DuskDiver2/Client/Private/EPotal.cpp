#include "stdafx.h"
#include "..\Public\EPotal.h"
#include "GameInstance.h"


#include "Player_Manager.h"
#include "ToolManager.h"
#include "Time_Manager.h"
#include "Camera_Manager.h"
#include "UI_Manager.h"

#include "Particle_Manager.h"


CEPotal::CEPotal(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CEPotal::CEPotal(const CEPotal & rhs)
	: CCreture(rhs)
{
}

HRESULT CEPotal::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CEPotal::Initialize(void * pArg)
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



	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-174.877f, 39.197f, 132.313f, 1.f));



	return S_OK;
}

void CEPotal::Tick(_float fTimeDelta)
{
	Emp_Col();


	if (m_bCutScene)
	{
		if (!m_bCutSceneSound)
		{
			GI->PlaySoundW(L"Holo_On.wav", SD_NPC, 0.7f);
			m_bCutSceneSound = true;
		}

		m_fAlpha += fTimeDelta * 0.3f;
		if (1.f < m_fAlpha)
			m_fAlpha = 1.f;


		m_fTimeAcc += fTimeDelta;
		if (0.f < m_fTimeAcc  && 10.f > m_fTimeAcc)
		{
			CCamera_Main::CUTDESC CamDesc;
			CamDesc.vPos = _float3(-179.517f, 42.862f, 128.512f);
			CamDesc.vLook = _float3(-174.877f, 39.097f, 132.313f);
			CamDesc.bLerp = true;
			CamDesc.bLookLerp = true;
			CamDesc.fLerpSpeed = 3.f;
			CamDesc.fLookLerpSpeed = 3.f;
			CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
			CamDesc.pTran = nullptr;
			CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
			CamDesc.bPixLocal = false;
			CRM->Get_Cam()->Show_Target(CamDesc);

			//UM->Set_FrontUI(false);

			m_fTimeAcc = 10.f;
		}
		else if (15.f < m_fTimeAcc  && 20.f > m_fTimeAcc)
		{
			m_bCutScene = false;
			CRM->Get_Cam()->End_Target();
			m_fTimeAcc = 20.f;

			UM->Set_BattleTalkIndex(27);
			UM->Set_FrontUI(true);

			m_bCanUse = true;
		}


	}
	else if(m_bCanUse)
	{
		// For. 기능
		if (m_bTrigger && CK_DOWN(DIK_C))
		{
			UM->Set_FrontUI(false);
			m_bPotal = true;
			m_fTimeAcc = 0.f;
			GI->PlaySoundW(L"PotalOn.wav", SD_MAP, 0.7f);
		}


		m_fParticleAcc += fTimeDelta;

		if (0.7f < m_fParticleAcc)
		{
			_matrix vvM = XMMatrixIdentity();
			vvM.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 1.f, 0.f, 1.f);
			_float4x4 vM; XMStoreFloat4x4(&vM, vvM);
			PTM->Create_InstanceParticle(L"Portal", vM);

			m_fParticleAcc = 0.f;
		}


		if (m_bPotal)
		{
			m_fTimeAcc += fTimeDelta;
			if (0.f < m_fTimeAcc  && 10.f > m_fTimeAcc)
			{
				CRM->Get_Cam()->Shake(0.6f, 0.2f, false, 0.1f);

				CCamera_Main::CUTDESC CamDesc;
				CamDesc.vPos = _float3(-97.753f, 123.335f, 52.714f);
				CamDesc.vLook = _float3(-97.281f, 105.105f, 63.64f);
				CamDesc.bLerp = true;
				CamDesc.bLookLerp = true;
				CamDesc.fLerpSpeed = 3.f;
				CamDesc.fLookLerpSpeed = 2.f;
				CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
				CamDesc.pTran = nullptr;
				CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
				CamDesc.bPixLocal = false;
				CRM->Get_Cam()->Show_Target(CamDesc);

				//UM->Set_FrontUI(false);

				_matrix vvM = XMMatrixIdentity();
				vvM.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				_float4x4 vM; XMStoreFloat4x4(&vM, vvM);
				//PTM->Create_InstanceParticle(L"PortalUp", vM);
				PM->PlayerPortal();



				m_fTimeAcc = 10.f;
			}
			else if (11.5f < m_fTimeAcc  && 20.f > m_fTimeAcc)
			{
				CCamera_Main::CUTDESC CamDesc;
				CamDesc.vPos = _float3(12.602f, 18.41f, 74.312f);
				CamDesc.vLook = _float3(1.407f, 7.820f, 78.016f);
				CamDesc.bLerp = true;
				CamDesc.bLookLerp = true;
				CamDesc.fLerpSpeed = 3.f;
				CamDesc.fLookLerpSpeed = 3.5f;
				CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
				CamDesc.pTran = nullptr;
				CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
				CamDesc.bPixLocal = false;
				CRM->Get_Cam()->Show_Target(CamDesc);

				m_fTimeAcc = 20.f;
			}
			else if (23.f < m_fTimeAcc  && 30.f > m_fTimeAcc)
			{
				_matrix vvM = XMMatrixIdentity();
				vvM.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				_float4x4 vM; XMStoreFloat4x4(&vM, vvM);
				//PTM->Create_InstanceParticle(L"PortalDown", vM);
				PM->PlayerPortal();
				PM->Set_ScenePos(PM->Get_SelectPlayer(), _float4(1.407f, 7.820f, 78.016f, 1.f));


				m_fTimeAcc = 30.f;

				if (!m_bOffSound) // 소리 한번만
				{
					GI->PlaySoundW(L"PotalOff.wav", SD_MAP, 0.7f);
					m_bOffSound = true;
				}
			}
			else if (31.f < m_fTimeAcc && 40.f > m_fTimeAcc)
			{

				UM->Set_FrontUI(true);
				CRM->Get_Cam()->End_Target();
				m_fTimeAcc = 40.f;
			}

		}


	}


}


void CEPotal::LateTick(_float fTimeDelta)
{

	if (nullptr == m_pRendererCom)
		return;



	Choose_Col(nullptr);
	Tick_Col(m_pTransformCom->Get_WorldMatrix());
	Render_Col(m_pRendererCom);



	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	m_bTrigger = false;
}

HRESULT CEPotal::Render()
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
	_float fBlurPower = 1.f;
	_float fAlpha = 0.f;

	if (FAILED(m_pShaderCom->Set_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;




	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
		return E_FAIL;

	_float fGama = 2.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;



	iNumMeshes = m_pModelCom->Get_NumMeshes();
	iPassIndex = 0;
	fBlurPower = 0.f;

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		// 노말 텍스쳐가 있다면
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_EMISSIVE, "g_EmissiveTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 25)))
			return E_FAIL;
	}



	return S_OK;
}

// *[HeroJho]: 디버깅 창입니다. ImGui로 작동됩니다.
void CEPotal::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);



	ImGui::End();
}

// *[HeroJho]: 충돌이 발생할 때 호출되는 함수입니다.
void CEPotal::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{

	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		if (m_bCanUse)
		{
			UM->Set_TalkButtonType(4);
			UM->Set_TalkButton(true);

			m_bTrigger = true;
		}
	}

}

void CEPotal::On_Hit(CCreture* pOther)
{
	//hit이 한번만 처리되게끔
	if (m_bHurt)
	{
		m_bHurt = true;


	}
}





HRESULT CEPotal::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_Portal"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;



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



CEPotal * CEPotal::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEPotal*		pInstance = new CEPotal(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEPotal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEPotal::Clone(void * pArg)
{
	CEPotal*		pInstance = new CEPotal(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEPotal"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEPotal::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);


}
