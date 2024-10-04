#include "stdafx.h"
#include "..\Public\MiniGame_Block_Triger.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Monster_Manager.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "MiniGame_Block.h"

#include "Camera_Manager.h"
#include "Camera_Main.h"

CMiniGame_Block_Triger::CMiniGame_Block_Triger(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMiniGame_Block_Triger::CMiniGame_Block_Triger(const CMiniGame_Block_Triger & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMiniGame_Block_Triger::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMiniGame_Block_Triger::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Potal";

	Make_ImGui();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-124.f, 32.5f, 106.f, 1.f));

	return S_OK;
}

void CMiniGame_Block_Triger::Tick(_float fTimeDelta)
{
	Emp_Col();


	if (m_bTrigger)
	{
		TM->Start_MiniGame_Block_CutScene();
		PM->Set_SceneIdle(PM->Get_SelectPlayer());
		PM->Set_SceneLook(PM->Get_SelectPlayer(), _float4(-125.717f, 31.276f, 105.62f, 1.f));
		PM->Set_ScenePos(PM->Get_SelectPlayer(), _float4(-126.07f, 31.276f, 105.405f, 1.f));
	
		//GI->PlaySound_Linear(L"PaperStack.wav", SD_BGM, 0.5f, true, 0.4f);
		//GI->StopSound(SD_BGM);
		//GI->PlayBGM(L"PaperStack.wav", 0.5f);

		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(-127.43f, 32.538f, 103.971f);
		CamDesc.vLook = _float3(-127.04f, 31.8f, 104.9f);
		CamDesc.bLerp = false;
		CamDesc.bLookLerp = false;
		CamDesc.fLerpSpeed = 0.f;
		CamDesc.fLookLerpSpeed = 0.f;
		CamDesc.vPivotLookPos = _float3(0.f, 0.f, 0.f);
		CamDesc.pTran = nullptr;
		CamDesc.vPosDis = _float3(0.f, 0.f, 0.f);
		CamDesc.bPixLocal = false;
		CRM->Get_Cam()->Show_Target(CamDesc);

		Set_Dead();
	}

}

void CMiniGame_Block_Triger::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	Choose_Col(nullptr);
	Render_Col(m_pRendererCom);
}

HRESULT CMiniGame_Block_Triger::Render()
{


	return S_OK;
}

void CMiniGame_Block_Triger::Render_Debug()
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

void CMiniGame_Block_Triger::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{

		if ((!strcmp("Potal", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Body", Desc.pOtherDesc->sTag)))
		{

			m_bTrigger = true;


		}

	}
}



HRESULT CMiniGame_Block_Triger::Ready_Components()
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

	

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(3.f, 0.f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Potal");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}





CMiniGame_Block_Triger * CMiniGame_Block_Triger::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMiniGame_Block_Triger*		pInstance = new CMiniGame_Block_Triger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMiniGame_Block_Triger"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMiniGame_Block_Triger::Clone(void * pArg)
{
	CMiniGame_Block_Triger*		pInstance = new CMiniGame_Block_Triger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMiniGame_Block_Triger"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CMiniGame_Block_Triger::Free()
{
	__super::Free();


	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
