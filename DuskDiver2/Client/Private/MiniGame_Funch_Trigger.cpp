#include "stdafx.h"
#include "..\Public\MiniGame_Funch_Trigger.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Monster_Manager.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "MiniGame_Block.h"

#include "Camera_Manager.h"
#include "Camera_Main.h"

CMiniGame_Funch_Triger::CMiniGame_Funch_Triger(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMiniGame_Funch_Triger::CMiniGame_Funch_Triger(const CMiniGame_Funch_Triger & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMiniGame_Funch_Triger::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMiniGame_Funch_Triger::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Potal";

	Make_ImGui();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(133.698f, -15.64f, 100.102f, 1.f));

	return S_OK;
}

void CMiniGame_Funch_Triger::Tick(_float fTimeDelta)
{
	Emp_Col();

	if (m_bStart)
	{
		TM->Start_MiniGame_Funch();
		Set_Dead();
	}
}

void CMiniGame_Funch_Triger::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	Choose_Col(nullptr);
	Render_Col(m_pRendererCom);
}

HRESULT CMiniGame_Funch_Triger::Render()
{


	return S_OK;
}

void CMiniGame_Funch_Triger::Render_Debug()
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

void CMiniGame_Funch_Triger::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{

		if ((!strcmp("Potal", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Body", Desc.pOtherDesc->sTag)))
		{
			m_bStart = true;

		}

	}
}



HRESULT CMiniGame_Funch_Triger::Ready_Components()
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





CMiniGame_Funch_Triger * CMiniGame_Funch_Triger::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMiniGame_Funch_Triger*		pInstance = new CMiniGame_Funch_Triger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMiniGame_Funch_Triger"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMiniGame_Funch_Triger::Clone(void * pArg)
{
	CMiniGame_Funch_Triger*		pInstance = new CMiniGame_Funch_Triger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMiniGame_Funch_Triger"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CMiniGame_Funch_Triger::Free()
{
	__super::Free();


	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
