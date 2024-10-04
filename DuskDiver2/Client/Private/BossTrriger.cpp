#include "stdafx.h"
#include "..\Public\BossTrriger.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"
#include "Camera_Manager.h"
#include "Camera_Main.h"
#include "Monster_Manager.h"


CBossTrriger::CBossTrriger(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CBossTrriger::CBossTrriger(const CBossTrriger & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBossTrriger::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBossTrriger::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Potal";

	Make_ImGui();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(1.69f, 16.99f, 144.42f, 1.f));



	return S_OK;
}

void CBossTrriger::Tick(_float fTimeDelta)
{
	//if (CK_DOWN(DIK_F8))
	//{
	//	MM->Spawn_MechanicalOctopus();
	//	TM->Set_BossGro(false);
	//	TM->Set_BossDie(false);
	//}

	if (true == m_bOn)
	{
		// ���� �ƽ� ����

		TM->Set_StartBossCutScene(true);

		UM->Set_FrontUI(false);

		Set_Dead();
	}

}

void CBossTrriger::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	Choose_Col(nullptr);
	Render_Col(m_pRendererCom);
}

HRESULT CBossTrriger::Render()
{
	return S_OK;
}

void CBossTrriger::Render_Debug()
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

void CBossTrriger::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{

		if ((!strcmp("Potal", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Body", Desc.pOtherDesc->sTag)))
		{
			m_bOn = true; // ���� ���� �ƽ� ���� Ʈ����

		}

	}
}



HRESULT CBossTrriger::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;



	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(1.5f, 1.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(4.f, 4.f, 4.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Potal");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}





CBossTrriger * CBossTrriger::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBossTrriger*		pInstance = new CBossTrriger(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBossTrriger"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBossTrriger::Clone(void * pArg)
{
	CBossTrriger*		pInstance = new CBossTrriger(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBossTrriger"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CBossTrriger::Free()
{
	__super::Free();


	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
