#include "stdafx.h"
#include "..\Public\Stage5Event1.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"
#include "Camera_Manager.h"
#include "Camera_Main.h"
#include "Monster_Manager.h"


CStage5Event1::CStage5Event1(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CStage5Event1::CStage5Event1(const CStage5Event1 & rhs)
	: CGameObject(rhs)
{
}

HRESULT CStage5Event1::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStage5Event1::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Potal";

	Make_ImGui();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, -63.59f, 1.f));

	return S_OK;
}

void CStage5Event1::Tick(_float fTimeDelta)
{
	
	if (!m_bOn)
		return;

	m_fTimeAcc += fTimeDelta;
	if (0.f < m_fTimeAcc && 10.f > m_fTimeAcc)
	{
		UM->ChaosName_On();
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(-1.351f, 4.613f, -34.128f);
		CamDesc.vLook = _float3(-1.309f, 4.709f, -32.047f);
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 1.f;
		CamDesc.fLookLerpSpeed = 1.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);

		UM->Set_FrontUI(false);

		m_fTimeAcc = 10.f;
	}
	else if (13.f < m_fTimeAcc && 20.f > m_fTimeAcc)
	{
		// 하울링을 한다.
		MM->Set_BeastCry();
		m_fTimeAcc = 20.f;
	}
	else if (22.f < m_fTimeAcc && 30.f > m_fTimeAcc)
	{
		UM->ChaosName_Off();
		// 카메라 뒤로 밀어낸다
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(-0.477f, 1.62f, -68.549f);
		CamDesc.vLook = _float3(-1.309f, 4.709f, -32.047f);
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 10.f;
		CamDesc.fLookLerpSpeed = 10.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);

		//// 강하게 쉐이킹 준다.
		CRM->Get_Cam()->Shake(0.5f, 1.f);



		m_fTimeAcc = 30.f;
	}

	else if (33.f < m_fTimeAcc && 40.f > m_fTimeAcc)
	{

		// 끝
		CRM->Get_Cam()->End_Target();
		UM->Set_FrontUI(true);
		m_fTimeAcc = 40.f;
		Set_Dead();
		MM->Beast_Spawn();
	}
}

void CStage5Event1::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	Choose_Col(nullptr);
	Render_Col(m_pRendererCom);
}

HRESULT CStage5Event1::Render()
{
	return S_OK;
}

void CStage5Event1::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);
	_float3 vSize = Get_Colliders("Potal")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Potal")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Get_Colliders("Potal")->Set_Size(vSize);
	Get_Colliders("Potal")->Set_Center(vCenter);
	ImGui::End();
}

void CStage5Event1::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{

		if ((!strcmp("Potal", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Body", Desc.pOtherDesc->sTag)))
		{
			m_bOn = true; // 맵 시작을 알리는 컷신
		}

	}
}



HRESULT CStage5Event1::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Player01Face"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(12.f, 1.f, 3.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Potal");
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;


	return S_OK;
}





CStage5Event1 * CStage5Event1::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStage5Event1*		pInstance = new CStage5Event1(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStage5Event1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStage5Event1::Clone(void * pArg)
{
	CStage5Event1*		pInstance = new CStage5Event1(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStage5Event1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CStage5Event1::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
