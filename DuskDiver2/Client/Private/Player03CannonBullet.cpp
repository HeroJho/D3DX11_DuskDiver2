#include "stdafx.h"
#include "..\Public\Player03CannonBullet.h"
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

CPlayer03CannonBullet::CPlayer03CannonBullet(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCreture(pDevice, pContext)
{
}

CPlayer03CannonBullet::CPlayer03CannonBullet(const CPlayer03CannonBullet& rhs)
	: CCreture(rhs)
{
}

HRESULT CPlayer03CannonBullet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer03CannonBullet::Initialize(void * pArg)
{
	m_pInfo = (*(BULLETINFO**)pArg);

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
	m_fPushPower = 6.f;
	m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER03) * 0.6f;
	m_fAirPower = 1.f;
	m_fLimitAcc = 0.4f;
	m_vColor1 = { 1.f, 1.f, 1.f, 1.f };
	m_vColor2 = { 0.f, 1.f, 0.7f, 1.f };
	m_vRatio = { 10.f, 30.f };
	m_fBlurPower = 0.1f;
	return S_OK;
}

void CPlayer03CannonBullet::Tick(_float fTimeDelta)
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

	if (!m_bAttackCol)
	{
		m_fColAcc += 1.f * fTimeDelta;
		if (m_fColAcc >= 0.2f)
		{
			if (!m_bEffect1)
			{
				m_bAttackCol = true;
				m_fColAcc = 0.f;
			}
		}
	}

	if (m_fLifeTimeAcc <= 0.9f)
	{
		m_pTransformCom2->Go_Dir(XMVector3Normalize(XMLoadFloat3(&m_vTargetLook)), 15.f, fTimeDelta);
		m_pTransformCom2->Turn(m_pTransformCom2->Get_State(CTransform::STATE_LOOK), 5.f, fTimeDelta);
		if(!m_pInfo->bRage)
			PTM->Create_AlphaParticle(L"Player03CannonSmoke", Matrix);
		else
			PTM->Create_AlphaParticle(L"Player03RageCannonSmoke", Matrix);
	}

	m_fLifeTimeAcc += 1.f * fTimeDelta;
	if (m_fLifeTimeAcc >= 1.f && !m_bEffect1)
	{
		
		if(!m_pInfo->bRage)
			PTM->Create_AlphaParticle(L"Player03CannonLight", Matrix);
		else
			PTM->Create_AlphaParticle(L"Player03RageCannonLight", Matrix);

		GI->PlaySoundW(L"se_enemy_warning.wav", SD_PLAYER3_SK, 0.5f);
		
		m_bRender = false;
		m_bEffect1 = true;
	}

	else if (m_fLifeTimeAcc >= 1.7f && !m_bEffect2)
	{
		
		CRM->Get_Cam()->Shake(0.4f, 0.4f, false, 0.3f);
		if (!m_pInfo->bRage)
		{
			CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
			Info->fBlurPower = 0.f;
			Info->vColor = { 0.7f,0.7f,0.7f,1.f };
			Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
			Info->vLocalPos = { 0.f,0.f,0.f,1.f };
			Info->vRatio = { 1.f,1.f };
			Info->vRatioSpeed = { 1.5f,1.5f };
			Info->vScale = { 0.1f,0.1f,0.1f };
			Info->vScaleSpeed = { 200.f,200.f,200.f };
			Info->vUvFixSpeed = { 1.f,3.f };
			Info->vWorldMatrix = Matrix;
			GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);

			CPlayer01SmokeBall::SMOKEBALLINFO* Info2 = new CPlayer01SmokeBall::SMOKEBALLINFO;
			Info2->fBlurPower = 2.f;
			Info2->vColor = { 0.7f,0.7f,0.7f,1.f };
			Info2->vColor2 = { 0.7f,0.7f,0.7f,1.f };
			Info2->vLocalPos = { 0.f,0.f,0.f,1.f };
			Info2->vRatio = { 1.f,1.f };
			Info2->vRatioSpeed = { 1.5f,1.5f };
			Info2->vScale = { 0.1f,0.1f,0.1f };
			Info2->vScaleSpeed = { 200.f,200.f,200.f };
			Info2->vUvFixSpeed = { -1.f,-3.f };
			Info2->vWorldMatrix = Matrix;
			GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2);
		}
		else
		{
			CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
			Info->fBlurPower = 2.f;
			Info->vColor = { 1.f,0.5f,0.9f,1.f };
			Info->vColor2 = { 1.f,0.5f,0.7f,1.f };
			Info->vLocalPos = { 0.f,0.f,0.f,1.f };
			Info->vRatio = { 1.f,1.f };
			Info->vRatioSpeed = { 1.5f,1.5f };
			Info->vScale = { 0.1f,0.1f,0.1f };
			Info->vScaleSpeed = { 200.f,200.f,200.f };
			Info->vUvFixSpeed = { 1.f,3.f };
			Info->vWorldMatrix = Matrix;
			GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);

			CPlayer01SmokeBall::SMOKEBALLINFO* Info2 = new CPlayer01SmokeBall::SMOKEBALLINFO;
			Info2->fBlurPower = 2.f;
			Info2->vColor = { 1.f,0.5f,0.9f,1.f };
			Info2->vColor2 = { 1.f,0.5f,0.7f,1.f };
			Info2->vLocalPos = { 0.f,0.f,0.f,1.f };
			Info2->vRatio = { 1.f,1.f };
			Info2->vRatioSpeed = { 1.5f,1.5f };
			Info2->vScale = { 0.1f,0.1f,0.1f };
			Info2->vScaleSpeed = { 200.f,200.f,200.f };
			Info2->vUvFixSpeed = { -1.f,-3.f };
			Info2->vWorldMatrix = Matrix;
			GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2);
		}

		PTM->Create_InstanceParticle(L"Player03CannonParticle_2", Matrix);
		PTM->Create_InstanceParticle(L"Player03CannonParticle_4", Matrix);
		PTM->Create_InstanceParticle(L"Player03CannonParticle_5", Matrix);
		PTM->Create_InstanceParticle(L"Player03CannonDistortion", Matrix);		
		PTM->Create_InstanceParticle(L"Player03CannonParticle", Matrix);

		GI->PlaySoundW(L"se_HE04_Skill05_4.wav", SD_PLAYER3_SK, 0.5f);
	
		m_eHitType = TYPE_SPIN;
		m_fPushPower = 1.f;
		m_fAirPower = 12.f;
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER03) * 6.f;
		Get_Colliders("Player_Attack")->Set_Size({ 15.f,0.f,0.f });
		Get_Colliders("Player_Attack")->Set_Center({ 0.f,0.f,0.f });
		m_bAttackCol = true;
		
		m_bEffect2 = true;
	}

	else if (m_fLifeTimeAcc >= 1.8f && !m_bEffect3)
	{
		if (!m_pInfo->bRage)
		{
			CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
			Info->fBlurPower = 0.f;
			Info->vColor = { 0.7f,0.7f,0.7f,1.f };
			Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
			Info->vLocalPos = { 0.f,0.f,0.f,1.f };
			Info->vRatio = { 1.f,1.f };
			Info->vRatioSpeed = { 1.5f,1.5f };
			Info->vScale = { 0.1f,0.1f,0.1f };
			Info->vScaleSpeed = { 400.f,400.f,400.f };
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
			Info2->vScaleSpeed = { 500.f,500.f,500.f };
			Info2->vUvFixSpeed = { -1.f,-3.f };
			Info2->vWorldMatrix = Matrix;
			GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2);

			_matrix WindMatrix = XMMatrixIdentity();
			WindMatrix.r[3] = a.r[3];
			_float4x4 Wind;
			XMStoreFloat4x4(&Wind, WindMatrix);

			CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
			Info3->fBlurPower = 1.f;
			Info3->vLocalPos = { 0.f,-1.3f,0.f,1.f };
			Info3->vColor = { 0.5f,0.5f,0.5f,1.f };
			Info3->vColor2 = { 0.4f,0.4f,0.4f,1.f };
			Info3->vRatio = { 1.3f,1.3f };
			Info3->vScale = { 20.f,10.f,20.f };
			Info3->vRatioSpeed = { 1.2f,1.2f };
			Info3->vScaleSpeed = { 100.f,20.f,100.f };
			Info3->fTurnSpeed = 0.f;
			Info3->vWorldMatrix = Wind;
			GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3);


			CPlayer01Wind2::WIND2INFO* Info4 = new CPlayer01Wind2::WIND2INFO;
			Info4->fBlurPower = 1.f;
			Info4->fTurnSpeed = 0.f;
			Info4->vScale = { 20.f,10.f,20.f };
			Info4->vLocalPos = { 0.f,-1.3f,0.f,1.f };
			Info4->vColor = { 0.5f,0.5f,0.5f,1.f };
			Info4->vColor2 = { 0.4f,0.4f,0.4f,1.f };
			Info4->vRatio = { 1.3f,1.3f };
			Info4->vRatioSpeed = { 1.2f,1.2f };
			Info4->vScaleSpeed = { 100.f,20.f,100.f };
			Info4->vWorldMatrix = Wind;
			GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4);
			_matrix ParticleMatrix = m_pTransformCom->Get_WorldMatrix();
			ParticleMatrix.r[3] = a.r[3];
			_float4x4 Particle;
			XMStoreFloat4x4(&Particle, WindMatrix);
			PTM->Create_InstanceParticle(L"Player03CannonParticle_3", Particle);
			m_bEffect3 = true;
			CRM->Get_Cam()->Fov(7.f, 0.05f, 0.f);
			Set_Dead();
			PTM->Create_InstanceParticle(L"Player03CannonDistortion_2", Matrix);

		}

		else
		{
			CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
			Info->fBlurPower = 2.f;
			Info->vColor = { 1.f,0.5f,0.9f,1.f };
			Info->vColor2 = { 1.f,0.5f,0.7f,1.f };
			Info->vLocalPos = { 0.f,0.f,0.f,1.f };
			Info->vRatio = { 1.f,1.f };
			Info->vRatioSpeed = { 1.5f,1.5f };
			Info->vScale = { 0.1f,0.1f,0.1f };
			Info->vScaleSpeed = { 400.f,400.f,400.f };
			Info->vUvFixSpeed = { 1.f,3.f };
			Info->vWorldMatrix = Matrix;
			GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);

			CPlayer01SmokeBall::SMOKEBALLINFO* Info2 = new CPlayer01SmokeBall::SMOKEBALLINFO;
			Info2->fBlurPower = 2.f;
			Info2->vColor = { 1.f,0.5f,0.9f,1.f };
			Info2->vColor2 = { 1.f,0.5f,0.7f,1.f };
			Info2->vLocalPos = { 0.f,0.f,0.f,1.f };
			Info2->vRatio = { 1.f,1.f };
			Info2->vRatioSpeed = { 1.5f,1.5f };
			Info2->vScale = { 0.1f,0.1f,0.1f };
			Info2->vScaleSpeed = { 500.f,500.f,500.f };
			Info2->vUvFixSpeed = { -1.f,-3.f };
			Info2->vWorldMatrix = Matrix;
			GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2);

			_matrix WindMatrix = XMMatrixIdentity();
			WindMatrix.r[3] = a.r[3];
			_float4x4 Wind;
			XMStoreFloat4x4(&Wind, WindMatrix);
			
			CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
			Info3->fBlurPower = 1.f;
			Info3->vLocalPos = { 0.f,-1.3f,0.f,1.f };
			Info3->vColor = { 1.f,0.5f,0.9f,1.f };
			Info3->vColor2 = { 1.f,0.5f,0.7f,1.f };
			Info3->vRatio = { 1.3f,1.3f };
			Info3->vScale = { 20.f,10.f,20.f };
			Info3->vRatioSpeed = { 1.2f,1.2f };
			Info3->vScaleSpeed = { 100.f,20.f,100.f };
			Info3->fTurnSpeed = 0.f;
			Info3->vWorldMatrix = Wind;
			GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3);


			CPlayer01Wind2::WIND2INFO* Info4 = new CPlayer01Wind2::WIND2INFO;
			Info4->fBlurPower = 1.f;
			Info4->fTurnSpeed = 0.f;
			Info4->vScale = { 20.f,10.f,20.f };
			Info4->vLocalPos = { 0.f,-1.3f,0.f,1.f };
			Info4->vColor = { 1.f,0.5f,0.9f,1.f };
			Info4->vColor2 = { 1.f,0.5f,0.7f,1.f };
			Info4->vRatio = { 1.3f,1.3f };
			Info4->vRatioSpeed = { 1.2f,1.2f };
			Info4->vScaleSpeed = { 100.f,20.f,100.f };
			Info4->vWorldMatrix = Wind;
			GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4);
			_matrix ParticleMatrix = m_pTransformCom->Get_WorldMatrix();
			ParticleMatrix.r[3] = a.r[3];
			_float4x4 Particle;
			XMStoreFloat4x4(&Particle, WindMatrix);
			PTM->Create_InstanceParticle(L"Player03CannonParticle_3", Particle);
			m_bEffect3 = true;
			CRM->Get_Cam()->Fov(7.f, 0.05f, 0.f);
			Set_Dead();
			PTM->Create_InstanceParticle(L"Player03CannonDistortion_2", Matrix);
			

		}
		

		

		
			
		
		
		
	}
}

void CPlayer03CannonBullet::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;
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

}

HRESULT CPlayer03CannonBullet::Render()
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

void CPlayer03CannonBullet::Render_Debug()
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

void CPlayer03CannonBullet::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
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

HRESULT CPlayer03CannonBullet::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Player03MeshEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("MonsterMissile"), TEXT("MonsterMissile"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform2"), (CComponent**)&m_pTransformCom2)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.5f, 0.f);
	ColDesc.vSize = _float3(10.f, 10.f, 10.f);
	ColDesc.vRotation = _float3{ 0.f,0.f,0.f };
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Player_Attack");

	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;

	return S_OK;
}

CCreture * CPlayer03CannonBullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer03CannonBullet*		pInstance = new CPlayer03CannonBullet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer03CannonBullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer03CannonBullet::Clone(void * pArg)
{
	CPlayer03CannonBullet*		pInstance = new CPlayer03CannonBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer03CannonBullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer03CannonBullet::Free()
{
	__super::Free();
	if(m_pInfo != nullptr)
		Safe_Delete(m_pInfo);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTransformCom2);
	Safe_Release(m_pShaderCom);
	

}
