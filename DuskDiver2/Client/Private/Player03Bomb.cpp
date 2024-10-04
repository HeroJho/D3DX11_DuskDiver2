#include "stdafx.h"
#include "..\Public\Player03Bomb.h"
#include "GameInstance.h"
#include "Time_Manager.h"
#include "Camera_Manager.h"
#include "Particle_Manager.h"
#include "Player01Rage1Ball.h"
#include "Player01SmokeBall.h"
#include "ToolManager.h"
#include "ViewAlpha.h"
#include "Player01Wind1.h"
#include "Player01Wind2.h"
#include "Status_Manager.h"
#include "PointTrailObj.h"
#include "PointTrail.h"

CPlayer03Bomb::CPlayer03Bomb(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCreture(pDevice, pContext)
{
}

CPlayer03Bomb::CPlayer03Bomb(const CPlayer03Bomb& rhs)
	: CCreture(rhs)
{
}

HRESULT CPlayer03Bomb::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer03Bomb::Initialize(void * pArg)
{
	m_pInfo = (*(BOMBINFO**)pArg);

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_bDead = false;

	m_pTransformCom->Set_WorldMatrix(m_pInfo->Worldmatrix);
	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pInfo->vScale));
	m_pTransformCom2->Set_WorldMatrix(m_pInfo->Worldmatrix);
	m_pTransformCom2->Set_Scale(XMLoadFloat3(&m_pInfo->vScale));

	m_fMaxUVIndexX = 0.f;
	m_fMaxUVIndexY = 0.f;
	m_vUVFixAcc = { 0.f,0.f };
	m_vUVFixSpeed = { 0.f,0.f };
	XMStoreFloat3(&m_vTargetLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	m_eHitType = TYPE_SPIN;
	m_fPushPower = 0.f;
	m_fAirPower = 8.f;
	m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER03);
	m_fLimitAcc = 1.f;
	m_vColor1 = { 1.f, 0.3f, 0.f, 1.f };
	m_vColor2 = { 0.f, 0.f, 0.f, 1.f };
	m_vRatio = { 1.f, 2.f };
	m_fBlurPower = 5.f;
	m_pTransformCom2->ResetAndJump(3.f);
	m_bAttackCol = false;
	return S_OK;
}

void CPlayer03Bomb::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_EFFECT, this);
	fTimeDelta *= m_fTMRatio;	
	Emp_Col();
	_matrix a = m_pTransformCom2->Get_WorldMatrix();
	a.r[0] = XMVector3Normalize(a.r[0]);
	a.r[1] = XMVector3Normalize(a.r[1]);
	a.r[2] = XMVector3Normalize(a.r[2]);
	_float4x4 Matrix;
	XMStoreFloat4x4(&Matrix, a);

	m_pTransformCom2->Go_Dir(XMLoadFloat3(&m_vTargetLook), m_pInfo->fAirCount * 0.5f, fTimeDelta);
	
	if (!m_pTransformCom2->Get_Air() && m_pInfo->fAirCount > 0.f)
	{
		m_pTransformCom2->ResetAndJump(m_pInfo->fAirCount);
		m_pInfo->fAirCount -= GI->Get_FloatRandom(0.8f,1.5f);
		GI->PlaySoundW(L"se_HE04_Skill02_2.wav", SD_PLAYER3_SK, 0.7f);

		if (m_pInfo->fAirCount <= 0.f)
		{
			CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
			Info->fBlurPower = 0.f;
			Info->vColor = { 0.7f,0.7f,0.7f,1.f };
			Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
			Info->vLocalPos = { 0.f,0.f,0.f,1.f };
			Info->vRatio = { 1.f,1.f };
			Info->vRatioSpeed = { 1.5f,1.5f };
			Info->vScale = { 0.1f,0.1f,0.1f };
			Info->vScaleSpeed = { 100.f,100.f,100.f };
			Info->vUvFixSpeed = { 1.f,3.f };
			Info->vWorldMatrix = Matrix;
			GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);

			CPlayer01SmokeBall::SMOKEBALLINFO* Info2 = new CPlayer01SmokeBall::SMOKEBALLINFO;
			Info2->fBlurPower = 1.f;
			Info2->vColor = { 0.7f,0.7f,0.7f,1.f };
			Info2->vColor2 = { 0.7f,0.7f,0.7f,1.f };
			Info2->vLocalPos = { 0.f,0.f,0.f,1.f };
			Info2->vRatio = { 1.f,1.f };
			Info2->vRatioSpeed = { 1.5f,1.5f };
			Info2->vScale = { 0.1f,0.1f,0.1f };
			Info2->vScaleSpeed = { 100.f,100.f,100.f };
			Info2->vUvFixSpeed = { -1.f,-3.f };
			Info2->vWorldMatrix = Matrix;
			GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2);

			PTM->Create_InstanceParticle(L"Player03Bomb_3", Matrix);
			PTM->Create_InstanceParticle(L"Player03Bomb_4", Matrix);
			PTM->Create_InstanceParticle(L"Player03Bomb_5", Matrix);
			PTM->Create_InstanceParticle(L"Player03Bomb_6", Matrix);
			PTM->Create_InstanceParticle(L"Player03Bomb_7", Matrix);
			PTM->Create_InstanceParticle(L"Player03Bomb_8", Matrix);
			CRM->Get_Cam()->Shake(0.2f, 0.15f, false, 0.2f);

			m_bAttackCol = true;
			GI->PlaySoundW(L"se_HE04_Skill02_3.wav", SD_PLAYER3_SK, 0.5f);
			Set_Dead();
		}
		else
			PTM->Create_InstanceParticle(L"Player03Bomb_2", m_pTransformCom2->Get_WorldFloat4x4());
	}
	
	m_fLimitAcc += 1.f * fTimeDelta;
	if(m_fLimitAcc >= 0.15f)
	{
		m_fLimitAcc = 0.f;
		PTM->Create_InstanceParticle(L"Player03Bomb", m_pTransformCom2->Get_WorldFloat4x4());
		
	}

	
	if (m_pTransformCom2->Get_Air())
	{
		m_vColor1 = { 1.f, 0.3f, 0.f, 1.f };
	}

	else
	{
		m_vColor1 = { 1.f, 1.f, 1.f, 1.f };
	}
	
	_float4 Pos;
	XMStoreFloat4(&Pos, m_pTransformCom2->Get_State(CTransform::STATE_POSITION));
	m_pPointTrail->TrailOn();
	m_pPointTrail->Add_Point(Pos);
	m_pPointTrail->Tick(fTimeDelta);


	_matrix WindMatrix = XMMatrixIdentity();
	WindMatrix.r[3] = a.r[3];
	_float4x4 Wind;
	XMStoreFloat4x4(&Wind, WindMatrix);


		
	
}

void CPlayer03Bomb::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	fTimeDelta *= m_fTMRatio;

	Tick_Col(m_pTransformCom2->Get_WorldMatrix());
	if (m_bAttackCol)
	{
		Choose_Col("Player_Attack");
	}
	Compute_CamZ(m_pTransformCom2->Get_State(CTransform::STATE_POSITION));
	if (m_bRender)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}
	Render_Col(m_pRendererCom);

	m_pTransformCom2->Tick_Gravity(fTimeDelta, m_pNavigationCom, 25.f);
	m_pPointTrail->LateTick(fTimeDelta);
	m_fDamage = 0.01f;

}

HRESULT CPlayer03Bomb::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom2->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
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

		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_vColor1, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor2", &m_vColor2, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio1", &m_vRatio.x, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio2", &m_vRatio.y, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fBlurPower", &m_fBlurPower, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fMaxUVIndexX", &m_fMaxUVIndexX, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Render(m_pShaderCom,i, 3)))
			return E_FAIL;
	}

	return S_OK;
}

void CPlayer03Bomb::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());
	_float3 vSize = Get_Colliders("Player_Attack")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Player_Attack")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);

	Get_Colliders("Player_Attack")->Set_Size(vSize);
	Get_Colliders("Player_Attack")->Set_Center(vCenter);

	ImGui::End();
}

void CPlayer03Bomb::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Monster" == Desc.pOther->Get_Tag())
	{
		// 내 OBB와 상대편의 OBB와 충돌 했다면

		if ((!strcmp("Player_Attack", Desc.pMyDesc->sTag) &&
			!strcmp("Monster_Body1", Desc.pOtherDesc->sTag)))
		{			
			((CCreture*)Desc.pOther)->On_Hit(this);
			CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
			m_bAttackCol = false;
			m_fColAcc = 0.f;
			
		}
	}
}

HRESULT CPlayer03Bomb::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Player03MeshEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_Bomb"), TEXT("Effect_Bomb"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform2"), (CComponent**)&m_pTransformCom2)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 1.5f, 0.f);
	ColDesc.vSize = _float3(5.f, 0.f, 0.f);
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "Player_Attack");

	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	switch (TM->Get_CurLevel())
	{
	case LEVEL_TEST:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage1Navigation"), TEXT("Stage1"), (CComponent**)&m_pNavigationCom)))
			return E_FAIL;
		break;

	case LEVEL_STAGE2:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage2Navigation"), TEXT("Stage2"), (CComponent**)&m_pNavigationCom)))
			return E_FAIL;
		break;

	case LEVEL_STAGE3:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage3Navigation"), TEXT("Stage3"), (CComponent**)&m_pNavigationCom)))
			return E_FAIL;
		break;

	case LEVEL_STAGE4:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage4Navigation"), TEXT("Stage4"), (CComponent**)&m_pNavigationCom)))
			return E_FAIL;
		break;

	case LEVEL_STAGE5:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage5Navigation"), TEXT("Stage5"), (CComponent**)&m_pNavigationCom)))
			return E_FAIL;
		break;
	}

	m_pNavigationCom->Set_NaviIndex(m_pInfo->iNaviIndex);


	CPointTrail::TRAILCREATEINFO Info2;
	Info2.vColor = { 1.f, 1.f, 0.5f,1.f };
	Info2.vColor2 = { 1.f, 0.f, 0.f,1.f };
	Info2.bFixColor = true;
	Info2.fBlurPower = 1.f;
	Info2.iShaderPass = 1;
	Info2.vRatio = { 1.f,1.f };
	Info2.fSize = 0.2f;
	Info2.TexName = L"MissileTrail2";
	Info2.iMaxIndex = 10;

	m_pPointTrail = ((CPointTrailObj*)GI->Clone_GameObject(TEXT("PointTrailObj"), &Info2));

	m_pPointTrail->Set_TimeLimit(0.02f);


	return S_OK;
}

CCreture * CPlayer03Bomb::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer03Bomb*		pInstance = new CPlayer03Bomb(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer03Bomb"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer03Bomb::Clone(void * pArg)
{
	CPlayer03Bomb*		pInstance = new CPlayer03Bomb(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer03Bomb"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer03Bomb::Free()
{
	__super::Free();
	if(m_pInfo != nullptr)
		Safe_Delete(m_pInfo);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTransformCom2);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pPointTrail);
	

}
