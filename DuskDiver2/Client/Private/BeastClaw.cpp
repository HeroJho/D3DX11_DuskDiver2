#include "stdafx.h"
#include "..\Public\BeastClaw.h"
#include "GameInstance.h"
#include "Player.h"
#include "Creture.h"
#include "UI_Manager.h"
#include "Time_Manager.h"
#include "ToolManager.h"
#include "Particle_Manager.h"

CBeastClaw::CBeastClaw(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CBeastClaw::CBeastClaw(const CBeastClaw & rhs)
	: CCreture(rhs)
{
}

HRESULT CBeastClaw::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBeastClaw::Initialize(void * pArg)
{
	m_pInfo = (*(CLAWINFO**)pArg);

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Boss";
	Make_ImGui();

	m_eHitType = TYPE_AWAY;

	m_fPushPower = 30.f;
	m_fAirPower = 6.f;
	m_fDamage = 500.f;
	
	_matrix Matrix = XMMatrixIdentity();
	Matrix.r[3] = XMLoadFloat4(&m_pInfo->vLocalPos);
	XMStoreFloat4x4(&m_pInfo->vWorldMatrix, Matrix * XMLoadFloat4x4(&m_pInfo->vWorldMatrix));
	m_pTransformCom->Set_WorldMatrix(m_pInfo->vWorldMatrix);

	

	return S_OK;
}

void CBeastClaw::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_MONSTER, this);
	fTimeDelta *= m_fTMRatio;
	Emp_Col();

	Change_ClawSize(fTimeDelta);

	m_pTransformCom->Go_Dir(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 27.f, fTimeDelta);

	m_pTransformCom->Set_Scale(XMVectorSet(m_vScale.x, m_vScale.y, m_vScale.z, 0.f));
	Change_ColRange(_float3{ 1.48f + m_vColScale.x, 0.07f + m_vColScale.y, 0.6f + m_vColScale.z }, _float3{ 0.f, 0.f, 0.2f + m_fColZSize });
	
	_matrix Matrix = XMMatrixIdentity();
	Matrix.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	XMStoreFloat4x4(&m_pInfo->vWorldMatrix, Matrix);
	PTM->Create_InstanceParticle(L"BeastClaw", m_pInfo->vWorldMatrix);

	if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[2] <= -80.f)
		Set_Dead();
}

void CBeastClaw::LateTick(_float fTimeDelta)
{

	//렌더 키고 끄기
	
	if (nullptr == m_pRendererCom)
		return;

	Tick_Col(m_pTransformCom->Get_WorldMatrix_NoScale());

	
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	
	Choose_Col("Monster_Attack");

	Render_Col(m_pRendererCom);	

}

HRESULT CBeastClaw::Render()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_pInfo->vColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor2", &m_pInfo->vColor2, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio1", &m_pInfo->vRatio.x, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio2", &m_pInfo->vRatio.y, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fBlurPower", &m_pInfo->fBlurPower, sizeof(_float))))
		return E_FAIL;
	_float UVFix = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixX", &UVFix, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixY", &UVFix, sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		//// 노말 텍스쳐가 있다면
		//if (!FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
		//	iPassIndex = 2;


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 3)))
			return E_FAIL;
	}

	return S_OK;
}

void CBeastClaw::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	//종류 상관없이 플레이어와 충돌은 필요함
	//빨간색 브레스일때만 돌기둥 충돌

	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		int i = 0;
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


void CBeastClaw::Change_ColRange(_float3 vSize, _float3 vCenter)
{
	((COBB*)Get_Colliders("Monster_Attack"))->Set_Size(vSize);
	Get_Colliders("Monster_Attack")->Set_Center(vCenter);
}


HRESULT CBeastClaw::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Player01MeshEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Beast_Claw2"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.48f, 0.07f, 0.6f);
	ColDesc.vCenter = _float3(0.f, 0.f, 0.2f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Attack"); //플레이어랑 충돌처리 필요, 돌멩이랑도 충돌처리 필요 상대편의 태그 tag_Player, tag_boss의 stone_shield랑
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;

	return S_OK;
}

void CBeastClaw::Change_ClawSize(_float fTimeDelta)
{
	if (m_vScale.x <= 18.5f)
	{
		m_vScale.x += 12.f * fTimeDelta;
		m_vColScale.x += 8.17f * fTimeDelta;
	}

	if (m_vScale.y <= 12.f)
	{
		m_vScale.y += 7.11f * fTimeDelta;
		m_vColScale.y += 0.26f * fTimeDelta;
	}

	if (m_vScale.z <= 9.f)
	{
		m_vScale.z += 5.5f * fTimeDelta;
		m_vColScale.z += 0.63f * fTimeDelta;
		m_fColZSize += 1.3f * fTimeDelta;
	}

}


CBeastClaw * CBeastClaw::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBeastClaw*		pInstance = new CBeastClaw(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBeastClaw"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCreture * CBeastClaw::Clone(void * pArg)
{
	CBeastClaw*		pInstance = new CBeastClaw(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBeastClaw"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBeastClaw::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Delete(m_pInfo);

}




void CBeastClaw::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);

	ImGui::Text("Breathdistance : %f", m_fBreathDistance);

	_float3 vSize = Get_Colliders("Monster_Attack")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Monster_Attack")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Get_Colliders("Monster_Attack")->Set_Size(vSize);
	Get_Colliders("Monster_Attack")->Set_Center(vCenter);



	ImGui::End();
}
