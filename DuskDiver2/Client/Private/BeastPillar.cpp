#include "stdafx.h"
#include "..\Public\BeastPillar.h"
#include "GameInstance.h"
#include "Player.h"
#include "Creture.h"
#include "UI_Manager.h"
#include "Time_Manager.h"
#include "ToolManager.h"

CBeastPillar::CBeastPillar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CBeastPillar::CBeastPillar(const CBeastPillar & rhs)
	: CCreture(rhs)
{
}

HRESULT CBeastPillar::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBeastPillar::Initialize(void * pArg)
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	m_CretureInfo = *(CCreture::CREATEINFO*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Boss";
	Make_ImGui();

	m_eHitType = TYPE_AWAY;

	m_fPushPower = 3.f;
	m_fAirPower = 4.f;
	m_fDamage = 40.f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_CretureInfo.vPosition));

	return S_OK;
}

void CBeastPillar::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_MONSTER, this);
	fTimeDelta *= m_fTMRatio;
	Emp_Col();

	m_fDeadAcc += 1.f * fTimeDelta;

	if (m_fDeadAcc > 0.8f)
	{
		m_vScale -= 1.6f * fTimeDelta;
		m_vColScale -= 2.f * fTimeDelta;
	}
	
	if (m_vScale <= 0.f)
		Set_Dead();

	m_pTransformCom->Set_Scale(XMVectorSet(m_vScale, 1.f, m_vScale, 0.f));
	
	Change_ColRange(_float3{ 1.2f + m_vColScale, 3.5f, 1.2f + m_vColScale }, _float3{ 0.f, 3.3f, 0.f });
}

void CBeastPillar::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Tick_Col(m_pTransformCom->Get_WorldMatrix_NoScale());

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	
	Choose_Col("Monster_Attack");

	Render_Col(m_pRendererCom);	

}

HRESULT CBeastPillar::Render()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &m_fBlurPower, sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	_uint iPassIndex = 0;
	_float fBlurPower = 0.f;
	_bool	bDiffuse = false;
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
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

void CBeastPillar::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		if (!strcmp("Monster_Attack", Desc.pMyDesc->sTag) && !strcmp("Player_Body", Desc.pOtherDesc->sTag))
		{
			//플레이어 한번만 충돌하게
			if (m_bPlayerHit)
			{
				m_bPlayerHit = false;
				((CCreture*)Desc.pOther)->On_Hit(this);
			}
		}
	}

}


void CBeastPillar::Change_ColRange(_float3 vSize, _float3 vCenter)
{
	Get_Colliders("Monster_Attack")->Set_Size(vSize);
	Get_Colliders("Monster_Attack")->Set_Center(vCenter);
}

HRESULT CBeastPillar::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Beast_Pillar"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(2.4f, 7.f, 2.4f);
	ColDesc.vCenter = _float3(0.f, 3.5f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Attack"); //플레이어랑 충돌처리 필요, 돌멩이랑도 충돌처리 필요 상대편의 태그 tag_Player, tag_boss의 stone_shield랑
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;

	return S_OK;
}


CBeastPillar * CBeastPillar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBeastPillar*		pInstance = new CBeastPillar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBeastPillar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCreture * CBeastPillar::Clone(void * pArg)
{
	CBeastPillar*		pInstance = new CBeastPillar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBeastPillar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBeastPillar::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);

}




void CBeastPillar::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);

	_float3 vSize = Get_Colliders("Monster_Attack")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Monster_Attack")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Get_Colliders("Monster_Attack")->Set_Size(vSize);
	Get_Colliders("Monster_Attack")->Set_Center(vCenter);



	ImGui::End();
}
