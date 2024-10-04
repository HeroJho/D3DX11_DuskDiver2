#include "stdafx.h"
#include "..\Public\BeastLaser.h"
#include "GameInstance.h"
#include "Player.h"
#include "Creture.h"
#include "UI_Manager.h"
#include "Time_Manager.h"
#include "ToolManager.h"
#include "Particle_Manager.h"
#include "Camera_Manager.h"

CBeastLaser::CBeastLaser(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CBeastLaser::CBeastLaser(const CBeastLaser & rhs)
	: CCreture(rhs)
{
}

HRESULT CBeastLaser::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBeastLaser::Initialize(void * pArg)
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;
		

		m_pInfo = (*(CBeastLaser::LASERINFO**)pArg);
		m_pTransformCom->Set_Look(XMLoadFloat4(&m_pInfo->vTargetLook));
		_float4 fTemp = m_pInfo->vPosition;
		fTemp.y += 1.f;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&fTemp));
		m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pInfo->vScale));
		m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(90.f));

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Boss";
	Make_ImGui();

	m_eHitType = TYPE_HIT;

	m_fPushPower = 0.f;
	m_fAirPower = 0.f;
	m_fDamage = 200.f;
	_matrix FixMatrix = m_pTransformCom->Get_WorldMatrix();
	FixMatrix.r[0] = XMVector3Normalize(FixMatrix.r[0]);
	FixMatrix.r[1] = XMVector3Normalize(FixMatrix.r[1]);
	FixMatrix.r[2] = XMVector3Normalize(FixMatrix.r[2]);
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, FixMatrix);

	GI->PlaySoundW(L"BladeAttack.ogg", SD_BEAST5, GI->Get_FloatRandom(0.5f, 0.8f));
	PTM->Create_InstanceParticle(L"BeastBar", WorldMatrix);
	CRM->Get_Cam()->Shake(0.3f, 0.2f, false, 0.1f);

	return S_OK;
}

void CBeastLaser::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_MONSTER, this);
	fTimeDelta *= m_fTMRatio;
	Emp_Col();

	m_pInfo->vUVFixAcc.x += m_pInfo->vUVFixSpeed.x * fTimeDelta;
	m_pInfo->vUVFixAcc.y += m_pInfo->vUVFixSpeed.y * fTimeDelta;

	m_pInfo->vUVFixAcc_2.x += m_pInfo->vUVFixSpeed_2.x * fTimeDelta;
	m_pInfo->vUVFixAcc_2.y += m_pInfo->vUVFixSpeed_2.y * fTimeDelta;

	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pTransformCom->Get_Scale()) + (XMLoadFloat3(&m_pInfo->vScaleSpeed) * fTimeDelta));

	if (m_pTransformCom->Get_Scale().x <= 0.1f ||
		m_pTransformCom->Get_Scale().y <= 0.1f ||
		m_pTransformCom->Get_Scale().z <= 0.1f)
	{
		Set_Dead();
	}
	


	/*m_pTransformCom->Set_Scale(XMVectorSet(m_vScale, m_vScale, 1.f, 0.f));
	
	Change_ColRange(_float3{ 1.f + m_vColScale , 1.f + m_vColScale, 20.f }, _float3{ 0.f, 0.f, 20.f });*/
}

void CBeastLaser::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Tick_Col(m_pTransformCom->Get_WorldMatrix_NoScale());
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	Choose_Col("Monster_Attack");
	Render_Col(m_pRendererCom);	

}

HRESULT CBeastLaser::Render()
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

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_pInfo->vColor1, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor2", &m_pInfo->vColor1_2, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio1", &m_pInfo->vRatio.x, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio2", &m_pInfo->vRatio.y, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fBlurPower", &m_pInfo->fBlurPower, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixX", &m_pInfo->vUVFixAcc.x, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixY", &m_pInfo->vUVFixAcc.y, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 7)))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_fBlurPower", &m_pInfo->fBlurPower_2, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_pInfo->vColor2, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor2", &m_pInfo->vColor2_2, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio1", &m_pInfo->vRatio_2.x, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio2", &m_pInfo->vRatio_2.y, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixX", &m_pInfo->vUVFixAcc_2.x, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixY", &m_pInfo->vUVFixAcc_2.y, sizeof(_float))))
		return E_FAIL;

	_uint		iNumMeshes2 = m_pBreathCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes2; ++i)
	{
		if (FAILED(m_pBreathCom->SetUp_OnShader(m_pShaderCom, m_pBreathCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pBreathCom->Render(m_pShaderCom, i, 7)))
			return E_FAIL;
	}

	return S_OK;
}

void CBeastLaser::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
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


void CBeastLaser::Change_ColRange(_float3 vSize, _float3 vCenter)
{
	Get_Colliders("Monster_Attack")->Set_Size(vSize);
	Get_Colliders("Monster_Attack")->Set_Center(vCenter);
}

HRESULT CBeastLaser::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Player01MeshEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Breath"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Breath"), TEXT("Com_Model2"), (CComponent**)&m_pBreathCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.2f, 100.f, 1.2f);
	ColDesc.vCenter = _float3(0.f, 50.f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Attack"); //플레이어랑 충돌처리 필요, 돌멩이랑도 충돌처리 필요 상대편의 태그 tag_Player, tag_boss의 stone_shield랑
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;

	return S_OK;
}


CBeastLaser * CBeastLaser::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBeastLaser*		pInstance = new CBeastLaser(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBeastLaser"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCreture * CBeastLaser::Clone(void * pArg)
{
	CBeastLaser*		pInstance = new CBeastLaser(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBeastLaser"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBeastLaser::Free()
{
	__super::Free();
	Safe_Delete(m_pInfo);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pBreathCom);

}




void CBeastLaser::Render_Debug()
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
