#include "stdafx.h"
#include "..\Public\Stage4Event4.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"
#include "Camera_Manager.h"
#include "Camera_Main.h"
#include "Monster_Manager.h"
#include "Crack_Manager.h"


CStage4Event4::CStage4Event4(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CStage4Event4::CStage4Event4(const CStage4Event4 & rhs)
	: CGameObject(rhs)
{
}

HRESULT CStage4Event4::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStage4Event4::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Potal";

	Make_ImGui();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(110.73f, -30.912f, 221.16f, 1.f));

	return S_OK;
}

void CStage4Event4::Tick(_float fTimeDelta)
{
	if (true == m_bOn)
	{
		if (m_bDead)
			return;

		CRKM->Activate_Hockey(); // 게임이 시작되면서 하키공이 움직인다

		UM->Set_BattleTalkIndex(13);
		UM->Set_QuestPointOff();
		MM->Reset_Monster();
		MM->Spawn_Stage4_2();
		//GI->PlaySound_Linear(L"Punch.wav", SD_BGM, 0.3f, true, 0.4f);
		//GI->StopSound(SD_BGM);
		//GI->PlayBGM(L"Punch.wav", 0.3f);
		GI->PlaySoundW(L"minigame_common_start.wav", SD_MAP, 1.f);
		Set_Dead();
	}

}

void CStage4Event4::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	Choose_Col(nullptr);
	Render_Col(m_pRendererCom);
}

HRESULT CStage4Event4::Render()
{
	return S_OK;
}

void CStage4Event4::Render_Debug()
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

void CStage4Event4::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{

		if ((!strcmp("Potal", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Body", Desc.pOtherDesc->sTag)))
		{
			m_bOn = true; // 하키 무한 생명력 몬스터 스폰
			
		}

	}
}



HRESULT CStage4Event4::Ready_Components()
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





CStage4Event4 * CStage4Event4::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStage4Event4*		pInstance = new CStage4Event4(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStage4Event4"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStage4Event4::Clone(void * pArg)
{
	CStage4Event4*		pInstance = new CStage4Event4(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStage4Event4"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CStage4Event4::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
