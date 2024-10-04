#include "stdafx.h"
#include "..\Public\AzitEvent_1.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"
#include "CluePic.h"

#include "Camera_Manager.h"
#include "Camera_Main.h"

CAzitEvent_1::CAzitEvent_1(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CAzitEvent_1::CAzitEvent_1(const CAzitEvent_1 & rhs)
	: CGameObject(rhs)
{
}

HRESULT CAzitEvent_1::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CAzitEvent_1::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Stage3_Prop";

	Make_ImGui();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(16.33f, 0.5f, 164.95f, 1.f)); // 1인 1메뉴

	return S_OK;
}

void CAzitEvent_1::Tick(_float fTimeDelta)
{
	Emp_Col();

	if (CK_DOWN(DIK_F7))
		UM->Set_FrontUI(true);


	if (40 == UM->Get_TalkIndex())
		m_bActivate = true;


	if (!m_bOn)
		return;

	m_fTimeAcc += fTimeDelta;
	// 컷신 시작
	if (0.1f < m_fTimeAcc && 10.f > m_fTimeAcc)
	{
		UM->Set_FrontUI(false);

		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(17.032f, 1.895f, 164.625f);
		CamDesc.vLook = _float3(16.417f, 1.86f, 164.782f);
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 2.5f;
		CamDesc.fLookLerpSpeed = 2.5f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);

		m_fTimeAcc = 10.f;
	}
	// 2초 기다린다.  대사 or 사진 찾기
	else if (13.f < m_fTimeAcc && 20.f > m_fTimeAcc)
	{
		UM->Set_BattleTalkIndex(31);

		m_fTimeAcc = 20.f;
	}
	// 나간다.
	else if (25.f < m_fTimeAcc && 30.f > m_fTimeAcc)
	{
		UM->Set_FrontUI(true);
		CRM->Get_Cam()->End_Target();
		m_bOn = false;
		m_fTimeAcc = 0.f;
	}






}

void CAzitEvent_1::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;


	Tick_Col(m_pTransformCom->Get_WorldMatrix());
	Choose_Col(nullptr);
	Render_Col(m_pRendererCom);


	//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CAzitEvent_1::Render()
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

	_float fGama = 2.2f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();


	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
			return E_FAIL;
	}

	return S_OK;
}

void CAzitEvent_1::Render_Debug()
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

void CAzitEvent_1::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		if ((!strcmp("Potal", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Body", Desc.pOtherDesc->sTag)))
		{

			UM->Set_TalkButtonType(7);
			UM->Set_TalkButton(true);


			if (CK_DOWN(DIK_C))
			{
				m_bOn = true;
			}
		}

	}

}



HRESULT CAzitEvent_1::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_DXBook"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Potal");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}




CAzitEvent_1 * CAzitEvent_1::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CAzitEvent_1*		pInstance = new CAzitEvent_1(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CAzitEvent_1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CAzitEvent_1::Clone(void * pArg)
{
	CAzitEvent_1*		pInstance = new CAzitEvent_1(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CAzitEvent_1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CAzitEvent_1::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
