#include "stdafx.h"
#include "..\Public\Tornado.h"
#include "GameInstance.h"
#include "Status_Manager.h"
#include "Camera_Manager.h"
#include "Time_Manager.h"
#include "Particle_Manager.h"
#include "ToolManager.h"
#include "ViewAlpha.h"
#include "Player_Manager.h"

CTornado::CTornado(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCreture(pDevice, pContext)
{
}

CTornado::CTornado(const CTornado& rhs)
	: CCreture(rhs)
{
}

HRESULT CTornado::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTornado::Initialize(void * pArg)
{
	m_pInfo = (*(TORNADOINFO**)pArg);

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_bDead = false;
	
	_matrix Matrix = XMMatrixIdentity();
	Matrix.r[3] = XMLoadFloat4(&m_pInfo->vLocalPos);
	XMStoreFloat4x4(&m_pInfo->vWorldMatrix, Matrix * XMLoadFloat4x4(&m_pInfo->vWorldMatrix));
	m_pTransformCom->Set_WorldMatrix(m_pInfo->vWorldMatrix);
	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pInfo->vScale));

	m_fMaxUVIndexX = 0.f;
	m_fMaxUVIndexY = 0.f;
	m_vUVFixAcc = { 0.f,0.f };
	m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER04) * 1.5f;
	m_eHitType = TYPE_SPIN;
	m_ePushType = PULL;
	m_fPushPower = 15.f;
	m_fAirPower = 3.f;

	CRM->Get_Cam()->Shake(0.11f, 10.f, false, 0.11f);
	GI->PlaySoundW(L"EFF_CHR_Buff_Rage_01_A.ogg", SD_PLAYER1_SK, 0.8f);
	GI->PlaySoundW(L"SE_BT_BOSSFINISH_EXPLOSION.wav", SD_PLAYER4_SK, 0.8f);
	return S_OK;
}

void CTornado::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_EFFECT, this);
	fTimeDelta *= m_fTMRatio;
	Emp_Col();
	m_vUVFixAcc.x += m_pInfo->vUVFixSpeed.x * fTimeDelta;
	m_vUVFixAcc.y += m_pInfo->vUVFixSpeed.y * fTimeDelta;
	m_pTransformCom->Turn(_vector{ 0.f,1.f,0.f }, m_pInfo->fTurnSpeed, fTimeDelta);

	
	if (!m_bEnd)
	{
		if (!m_bAttackCol)
		{
			m_fColAcc += 1.f * fTimeDelta;
			if (m_fColAcc >= 0.3f)
			{
				m_bAttackCol = true;
				m_fColAcc = 0.f;
			}
		}

		m_fLifeTimeAcc += 1.f * fTimeDelta;

		if (m_pInfo->bOrigin)
		{
			m_fParticleAcc += 1.f * fTimeDelta;
			if (m_fParticleAcc >= 0.1f)
			{
				_matrix WorldMatrix = XMMatrixIdentity();
				WorldMatrix.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				_float4x4 Mat;
				XMStoreFloat4x4(&Mat, WorldMatrix);
				PTM->Create_InstanceParticle(L"Player04Skill8", Mat);
				m_fParticleAcc = 0.f;
			}

			if (m_fLifeTimeAcc >= m_pInfo->fLifeTime - 1.f && m_bDistortion)
			{
				_matrix WorldMatrix = XMMatrixIdentity();
				WorldMatrix.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				_float4x4 Mat;
				XMStoreFloat4x4(&Mat, WorldMatrix);
				PTM->Create_InstanceParticle(L"Player04RageEndParticle_2", Mat);
			}

			if (m_fLifeTimeAcc >= 1.f && !m_bHitFix)
			{
				m_bHitFix = true;
				m_ePushType = PUSH;
				m_fPushPower = 6.f;
				m_fAirPower = 2.f;
			}
		}


	}

	if (m_fLifeTimeAcc >= m_pInfo->fLifeTime)
	{
		if (!m_bEnd)
		{
			if (m_pInfo->bOrigin)
			{
				//GI->PlaySoundW(L"CHR_LongStaff_Ice_01_B.ogg", SD_PLAYER03_RE, 0.8f); //너무 마법공주같은데
				m_ePushType = PUSH;
				m_fPushPower = 10.f;
				m_fAirPower = 5.f;
				m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER04) * 10.f;
				//m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER04) * 2.f;
				m_bAttackCol = true;
				Get_Colliders("Player_Attack")->Set_Size(_float3{ 15.f,0.f,0.f });
				CRM->Get_Cam()->Shake(0.7f, 1.f, false, 0.3f);
				_matrix WorldMatrix = XMMatrixIdentity();
				WorldMatrix.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				_float4x4 Mat;
				XMStoreFloat4x4(&Mat, WorldMatrix);
				PTM->Create_InstanceParticle(L"Player04Skill8_2", Mat);
				PTM->Create_InstanceParticle(L"Player04RageEndParticle", Mat);
			
				PM->Set_Player04Rage1End();
			}
			GI->PlaySoundW(L"EFF_CHR_Spell_Arrow_07_A.ogg", SD_PLAYER4_SK, 0.8f);
			m_bEnd = true;
		}
		m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pTransformCom->Get_Scale()) + _vector{ 150.f*fTimeDelta, 50.f*fTimeDelta,150.f*fTimeDelta });
		m_pInfo->vRatio.x -= m_pInfo->vRatioSpeed.x * fTimeDelta;
		m_pInfo->vRatio.y -= m_pInfo->vRatioSpeed.y * fTimeDelta;

		if (m_pInfo->vRatio.x <= 0.f)
		{
			Set_Dead();
			
		}
	}
	
}

void CTornado::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	if (m_pInfo->bOrigin)
	{
		Tick_Col(m_pTransformCom->Get_WorldMatrix_NoScale());
		if (m_bAttackCol)
		{
			Choose_Col("Player_Attack");
		}
		Render_Col(m_pRendererCom);
	}
	
	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NOBLUR, this);	


}

HRESULT CTornado::Render()
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
		if (FAILED(m_pShaderCom->Set_RawValue("g_fMaxUVIndexX", &m_fMaxUVIndexX, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixX", &m_fMaxUVIndexX, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixY", &m_vUVFixAcc.y, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Render(m_pShaderCom,i, 2)))
			return E_FAIL;
	}

	return S_OK;
}

void CTornado::Render_Debug()
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

void CTornado::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
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

void CTornado::Create_HitEffect(_float4 vWorldPos)
{
	_matrix Matrix = XMMatrixIdentity();
	Matrix.r[3] = XMLoadFloat4(&vWorldPos);
	_float4x4 a;
	XMStoreFloat4x4(&a, Matrix);
	PTM->Create_AlphaParticle(L"TornadoHit", a);
	PTM->Create_AlphaParticle(L"TornadoHit_2", a);
	PTM->Create_InstanceParticle(L"Player04Tornado", a);
}

HRESULT CTornado::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Player04MeshEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_Storm_Center"), TEXT("Effect_Storm_Center"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (m_pInfo->bOrigin)
	{
		CCollider::COLLIDERDESC ColDesc;
		ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

		ColDesc.vCenter = _float3(0.f, 1.f, 0.f);
		ColDesc.vSize = _float3(12.f, 0.f, 0.f);
		strcpy(ColDesc.sTag, "Player_Attack");
		ColDesc.pOwner = this;
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;

		Get_Colliders("Player_Attack")->Set_Size(_float3{ 13.f,0.f,0.f });
		Get_Colliders("Player_Attack")->Set_Center(_float3{ 0.f,1.f,0.f });
	}

	return S_OK;
}

CCreture * CTornado::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTornado*		pInstance = new CTornado(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTornado"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTornado::Clone(void * pArg)
{
	CTornado*		pInstance = new CTornado(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTornado"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTornado::Free()
{
	__super::Free();
	Safe_Delete(m_pInfo);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	

}
