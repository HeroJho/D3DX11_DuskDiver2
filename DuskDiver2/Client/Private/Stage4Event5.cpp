#include "stdafx.h"
#include "..\Public\Stage4Event5.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"
#include "Camera_Manager.h"
#include "Camera_Main.h"
#include "Monster_Manager.h"


CStage4Event5::CStage4Event5(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CStage4Event5::CStage4Event5(const CStage4Event5 & rhs)
	: CGameObject(rhs)
{
}

HRESULT CStage4Event5::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStage4Event5::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Potal";

	Make_ImGui();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(1.69f, 16.99f, 144.42f, 1.f));

	

	return S_OK;
}

void CStage4Event5::Tick(_float fTimeDelta)
{
	//if (CK_DOWN(DIK_F8))
	//{
	//	MM->Spawn_MechanicalOctopus();
	//	TM->Set_BossGro(false);
	//	TM->Set_BossDie(false);
	//}

	if (true == m_bOn)
	{
		// 보스 컷신 시작

		TM->Set_StartBossCutScene(true);
		//GI->PlaySound_Linear(L"Octopus.wav", SD_BGM, 0.3f, true, 0.3f);
		//GI->StopSound(SD_BGM);
		//GI->PlayBGM(L"Octopus.wav", 0.3f);
		UM->Set_FrontUI(false);

		Set_Dead();
	}

}

void CStage4Event5::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	Choose_Col(nullptr);
	Render_Col(m_pRendererCom);
}

HRESULT CStage4Event5::Render()
{
	return S_OK;
}

void CStage4Event5::Render_Debug()
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

void CStage4Event5::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{

		if ((!strcmp("Potal", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Body", Desc.pOtherDesc->sTag)))
		{
			m_bOn = true; // 최종 보스 컷신 시작 트리거
			
		}

	}
}



HRESULT CStage4Event5::Ready_Components()
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





CStage4Event5 * CStage4Event5::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStage4Event5*		pInstance = new CStage4Event5(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStage4Event5"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStage4Event5::Clone(void * pArg)
{
	CStage4Event5*		pInstance = new CStage4Event5(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStage4Event5"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CStage4Event5::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
