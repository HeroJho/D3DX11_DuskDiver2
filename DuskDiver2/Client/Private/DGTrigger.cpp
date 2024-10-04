#include "stdafx.h"
#include "..\Public\DGTrigger.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Monster_Manager.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "MiniGame_Block.h"

#include "Camera_Manager.h"
#include "Camera_Main.h"

CDGTrigger::CDGTrigger(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CDGTrigger::CDGTrigger(const CDGTrigger & rhs)
	: CGameObject(rhs)
{
}

HRESULT CDGTrigger::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDGTrigger::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Potal";

	Make_ImGui();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-178.443f, 39.097f, 122.181f, 1.f));

	return S_OK;
}

void CDGTrigger::Tick(_float fTimeDelta)
{
	Emp_Col();


	if (m_bTrigger)
	{
		m_fTimeAcc += fTimeDelta;

		if (2.f < m_fTimeAcc)
		{
			TM->Start_CutScene_DG();
			//GI->PlaySound_Linear(L"Kuma.wav", SD_BGM, 0.3f, true, 0.4f);
			//GI->StopSound(SD_BGM);
			//GI->PlayBGM(L"Kuma.wav", 0.3f);
			Set_Dead();
		}
		

	}

}

void CDGTrigger::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	Choose_Col(nullptr);
	Render_Col(m_pRendererCom);
}

HRESULT CDGTrigger::Render()
{


	return S_OK;
}

void CDGTrigger::Render_Debug()
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

void CDGTrigger::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{

		if ((!strcmp("Potal", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Body", Desc.pOtherDesc->sTag)))
		{

			m_bTrigger = true;

			if (false == m_bBlind)
			{
				m_bBlind = true;
				UM->MoveBlind(true);
			}
		}

	}
}



HRESULT CDGTrigger::Ready_Components()
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





CDGTrigger * CDGTrigger::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDGTrigger*		pInstance = new CDGTrigger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CDGTrigger"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDGTrigger::Clone(void * pArg)
{
	CDGTrigger*		pInstance = new CDGTrigger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CDGTrigger"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CDGTrigger::Free()
{
	__super::Free();


	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
