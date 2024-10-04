#include "stdafx.h"
#include "..\Public\YabaweBall.h"
#include "GameInstance.h"
#include "HockeyReserve.h"
#include "Player.h"
#include "Creture.h"
#include "UI_Manager.h"
#include "Time_Manager.h"
#include "ToolManager.h"
#include "MiniGame_Yabawe.h"
#include "RealYabawe.h"
#include "Camera_Manager.h"

CYabaweBall::CYabaweBall(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CYabaweBall::CYabaweBall(const CYabaweBall & rhs)
	: CCreture(rhs)
{
}



HRESULT CYabaweBall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CYabaweBall::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	m_Desc = *(YABAWEDESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Yabawe";
	Make_ImGui();

	m_fHurtTime = 0.02f;
	m_eHitType = TYPE_HIT;
	m_fAirPower = 2.f;
	m_fPushPower = 4.f;
	m_fDamage = 0.f;

	_vector vPos = XMVector3TransformCoord(XMLoadFloat3(&m_Desc.vPos), XMLoadFloat4x4(&m_Desc.mMatrix));

	XMStoreFloat3(&m_vInitPos, vPos);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransformCom->Set_Look(-1.f * XMVectorSetW(XMLoadFloat4x4(&m_Desc.mMatrix).r[2], 0.f));
	m_pTransformCom->Set_Scale(XMVectorSet(0.5f, 0.5f, 0.5f, 1.f));

	XMStoreFloat3(&m_vGoLook, XMLoadFloat4x4(&m_Desc.mMatrix).r[2]);

	m_bRender = true;

	return S_OK;
}








void CYabaweBall::Tick(_float fTimeDelta)
{
	Emp_Col();


	if (m_bStart && !m_bBlock)
	{
		m_fTimeCutStartAcc += fTimeDelta;
		if (0.5f < m_fTimeCutStartAcc && 1.f > m_fTimeCutStartAcc)
		{
			CCamera_Main::CUTDESC CamDesc;
			CamDesc.vPos = _float3();
			CamDesc.vLook = _float3();
			CamDesc.bLerp = false;
			CamDesc.bLookLerp = false;
			CamDesc.fLerpSpeed = 1.f;
			CamDesc.fLookLerpSpeed = 1.f;
			CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
			CamDesc.pTran = m_pTransformCom;
			CamDesc.vPosDis = _float3(1.f, 1.f, -1.f);
			CamDesc.bPixLocal = false;
			CRM->Get_Cam()->Show_Target(CamDesc);
			GI->PlaySoundW(L"se_mini05#10 (se_mini05_iron_ball_fly2).wav", SD_MAP, 1.f);
			m_fTimeCutStartAcc = 2.f;
		}
		
		m_fTimeDeltaAcc += fTimeDelta * 1.5f;
		
		m_pTransformCom->Go_Dir(XMVectorSetW(XMLoadFloat3(&m_vGoLook), 0.f), 3.5f - m_fTimeDeltaAcc * 0.5f, fTimeDelta);
		m_pTransformCom->Turn(-1.f * m_pTransformCom->Get_State(CTransform::STATE_RIGHT), 3.f - m_fTimeDeltaAcc * 0.5f, fTimeDelta);
	}

	if (m_bBlock)
	{
		m_fTimeDeltaAcc += fTimeDelta;
		_float fSpeed = 1.f - m_fTimeDeltaAcc;
		if (0.f > fSpeed)
			fSpeed = 0.f;
		else
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), 0.3f, fTimeDelta);
		
		m_pTransformCom->Go_Dir(-1.f * XMVectorSetW(XMLoadFloat3(&m_vGoLook), 0.f), fSpeed, fTimeDelta);
	}

}

void CYabaweBall::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;



	Tick_Col(m_pTransformCom->Get_WorldMatrix_NoScale());
	Choose_Col(nullptr);
	Render_Col(m_pRendererCom);

	if(m_bRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CYabaweBall::Render()
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


	_float fBlurPower = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
		return E_FAIL;


	_float fGama = 1.3f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	_bool	bDiffuse = false;
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		_uint iPassIndex = 0;
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		// 노말 텍스쳐가 있다면
		if (!FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			iPassIndex = 2;



		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iPassIndex)))
			return E_FAIL;
	}

	return S_OK;
}

void CYabaweBall::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{

	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		if (!strcmp("BallBody", Desc.pMyDesc->sTag) && !strcmp("Player_Body", Desc.pOtherDesc->sTag))
		{
			if (m_bStart)
				return;
			m_bStart = true;
			TM->Set_YavaweBallStart(true);
			UM->Set_FrontUI(false);
			//GI->PlaySound_Linear(L"Yabawe.wav", SD_BGM, 0.3f, true, 0.4f);
			//GI->StopSound(SD_BGM);
			//GI->PlayBGM(L"Yabawe.wav", 0.3f);
		}
	}
	if ("Tag_Boss" == Desc.pOther->Get_Tag())
	{
		if (!strcmp("BallBody", Desc.pMyDesc->sTag) && !strcmp("Monster_Body1", Desc.pOtherDesc->sTag))
		{
			if (!m_bOne)
			{
				m_fTimeDeltaAcc = 0.f;
				m_bBlock = true;
				TM->Get_MiniGameYabawe()->Get_RealYabawe()->Start_CutScene();

				m_bOne = true;

				CCamera_Main::CUTDESC CamDesc;
				CamDesc.vPos = CRM->Get_Cam()->Get_CamPos();
				CamDesc.vLook = CRM->Get_Cam()->Get_PreLook();
				CamDesc.bLerp = false;
				CamDesc.bLookLerp = false;
				CRM->Get_Cam()->Show_Target(CamDesc);

				GI->PlaySoundW(L"se_Env12_Mole_Hit_Body.wav", SD_MAP, 1.f);
				// 야바위 시작
			}

		}
	}
}

void CYabaweBall::On_Hit(CCreture * pOther)
{
	if (m_bHurt)
	{
		m_bHurt = false;



	}
}

HRESULT CYabaweBall::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Ball"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.7f, 1.f, 1.f);
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "BallBody");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	return S_OK;
}

CYabaweBall * CYabaweBall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CYabaweBall*		pInstance = new CYabaweBall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CYabaweBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCreture * CYabaweBall::Clone(void * pArg)
{
	CYabaweBall*		pInstance = new CYabaweBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CYabaweBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CYabaweBall::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	// Safe_Release(m_pTextureCom);
}




void CYabaweBall::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	IG->Debug_Transform(m_pTransformCom);

	IG->Debug_Col(*m_Colliders.begin());


	ImGui::End();
}

void CYabaweBall::Reset()
{
	m_bStart = false;
	m_bBlock = false;
	m_fTimeDeltaAcc = 0.f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vInitPos), 1.f));
}
