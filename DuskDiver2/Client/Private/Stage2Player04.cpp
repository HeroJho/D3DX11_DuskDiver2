#include "stdafx.h"
#include "Stage2Player04.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "DataManager.h"
#include "ToolManager.h"
#include "Parts.h"
#include "Particle_Manager.h"
#include "Player_Manager.h"
#include "Monster_Manager.h"
#include "UI_Manager.h"
#include "Crack_Manager.h"

CStage2Player04::CStage2Player04(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CStage2Player04::CStage2Player04(const CStage2Player04 & rhs)
	: CCreture(rhs)
{
}

HRESULT CStage2Player04::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStage2Player04::Initialize(void * pArg)
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_NPC";

	Make_ImGui();

	m_pModelCom->Set_AnimIndex(GROGGYLOOF);
	m_eCurState = GROGGYLOOF;
	m_eNextState = GROGGYLOOF;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-10.f, 0.f, -16.f, 1.f));

	_int iNaviIndex = m_pNavigationCom->Find_NaviIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pNavigationCom->Set_NaviIndex(iNaviIndex);

	Ready_Sockets();
	Ready_PlayerParts();

	return S_OK;
}

void CStage2Player04::Tick(_float fTimeDelta)
{
	if (!CRKM->Get_CrackExist())
	{
		fTimeDelta *= m_fTMRatio;
		Emp_Col();

		Update(fTimeDelta);

		if (!m_bTalkEnd)
		{
			if (UM->Get_TalkIndex() == 27)
			{
				m_bTalkEnd = true;
				Set_State(GROGGYEND);
			}
		}

		if ((MM->Get_Stage2Count() == 3) && (false == m_bQuestPointOn))
		{
			UM->Set_FrontUI(false);
			UM->Make_BattleEnd();
			UM->Set_FrontUI(false);
			m_bQuestPointOn = true;
		}
	}
	
}

void CStage2Player04::LateTick(_float fTimeDelta)
{
	if (!CRKM->Get_CrackExist())
	{
		fTimeDelta *= m_fTMRatio;

		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&m_vTargetLook), 0.2f);

		m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom, m_pNavigationCom);

		Update_Parts();

		Tick_Col(m_pTransformCom->Get_WorldMatrix());

		End_Animation();

		Choose_Col("NPC_Stage2Player04");
		Render_Parts();
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

		Render_Col(m_pRendererCom);
	}
	
}

HRESULT CStage2Player04::Render()
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

	// For.Blur
	_float fBlurPower = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
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

// *[HeroJho]: 디버깅 창입니다. ImGui로 작동됩니다.
void CStage2Player04::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());
	IG->Debug_Transform(m_pTransformCom);
	//_float3 vSize = Get_Colliders("Player_Attack")->Get_Desc()->vSize;
	//_float3 vCenter = Get_Colliders("Player_Attack")->Get_Desc()->vCenter;

	//ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	//ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);


	ImGui::End();
}

// *[HeroJho]: 충돌이 발생할 때 호출되는 함수입니다.
void CStage2Player04::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		// 내 OBB와 상대편의 OBB와 충돌 했다면

		if ((!strcmp("NPC_Stage2Player04", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Body", Desc.pOtherDesc->sTag)))
		{
			if (MM->Get_Stage2Count() == 3)
			{
				if (!m_bTalk)
				{
					m_bTalk = true;
					UM->Set_TalkIndex(19);
					UM->Set_QuestPointOff();
					_float3 vSize ={ 0.5f,0.f,0.f };
					Get_Colliders("NPC_Stage2Player04")->Set_Size(vSize);
					UM->Set_FrontUI(false);
				}			
			}

			if (m_bTalkEnd)
			{
				Set_Dead();
				UM->DD_On();
				UM->PartyOn(true);
			}
		}
	}
}

void CStage2Player04::Set_State(STATE eState)
{
	if (m_eNextState == eState)
		return;

	m_eNextState = eState;

	switch (m_eNextState)
	{
	case Client::CStage2Player04::WALK:
		break;
	case Client::CStage2Player04::GROGGYEND:
		break;
	case Client::CStage2Player04::GROGGYLOOF:
		break;
	}

	m_pModelCom->SetNextIndex(m_eNextState);
	m_pModelCom->SetChangeBool(true);
	m_eCurState = m_eNextState;

}

void CStage2Player04::End_Animation()
{
	if (m_pModelCom->GetAniEnd())
	{
		switch (m_eCurState)
		{
		case Client::CStage2Player04::WALK:
			
			break;	
		case Client::CStage2Player04::GROGGYEND:
			Set_State(WALK);
			break;
		case Client::CStage2Player04::GROGGYLOOF:
			break;
		
		}
	}
}



void CStage2Player04::Update(_float fTimeDelta)
{
	if (!m_pModelCom->GetChangeBool())
	{
		switch (m_eCurState)
		{
		case Client::CStage2Player04::WALK:
			XMStoreFloat3(&m_vTargetLook, XMVector3Normalize(XMLoadFloat4(&PM->Get_PlayerPos()) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
			m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook),1.5f, fTimeDelta, m_pNavigationCom);
			break;

		case Client::CStage2Player04::GROGGYEND:
			
			break;
		case Client::CStage2Player04::GROGGYLOOF:
			
			break;
		}
	}
}


HRESULT CStage2Player04::Ready_Components()
{

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Player04Body"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	m_pModelCom->Set_RootHierarchy(1);

	/* For.Com_Navigation */
	CNavigation::NAVIGATIONDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = 0;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage1Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.3f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(2.f, 0.5f, 0.5f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "NPC_Stage2Player04"); // 충돌체tag
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CStage2Player04::Ready_Sockets()
{
	CHierarchyNode*		pHead = m_pModelCom->Get_HierarchyNode("Head_M");
	if (nullptr == pHead)
		return E_FAIL;
	m_Sockets.push_back(pHead);

	return S_OK;
}

HRESULT CStage2Player04::Ready_PlayerParts()
{

	CGameObject*		pPlayer04Hair = GI->Clone_GameObject(TEXT("Player04Hair"));
	if (nullptr == pPlayer04Hair)
		return E_FAIL;
	m_Parts.push_back((CParts*)pPlayer04Hair);

	return S_OK;
}

HRESULT CStage2Player04::Update_Parts()
{

	m_HeadMatrix = m_Sockets[SOCKET_HEAD]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
		m_Parts[PARTS_HAIR]->SetUp_State(m_HeadMatrix);

	return S_OK;
}

void CStage2Player04::Render_Parts()
{
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_Parts[PARTS_HAIR]);
}

CStage2Player04 * CStage2Player04::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStage2Player04*		pInstance = new CStage2Player04(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStage2Player04"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStage2Player04::Clone(void * pArg)
{
	CStage2Player04*		pInstance = new CStage2Player04(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStage2Player04"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStage2Player04::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);

	for (auto& pPart : m_Parts)
		Safe_Release(pPart);

	m_Parts.clear();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pShaderCom);
}
