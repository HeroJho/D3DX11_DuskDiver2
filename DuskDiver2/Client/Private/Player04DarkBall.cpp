#include "stdafx.h"
#include "..\Public\Player04DarkBall.h"
#include "GameInstance.h"
#include "Time_Manager.h"
#include "Particle_Manager.h"
#include "Player01SmokeBall.h"
#include "ToolManager.h"
#include "Camera_Manager.h"
#include "Player01Wind2.h"
#include "Player01Wind1.h"

CPlayer04DarkBall::CPlayer04DarkBall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCreture(pDevice, pContext)
{
}

CPlayer04DarkBall::CPlayer04DarkBall(const CPlayer04DarkBall& rhs)
	: CCreture(rhs)
{
}

HRESULT CPlayer04DarkBall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer04DarkBall::Initialize(void * pArg)
{
	m_pInfo = (*(DARKBALLINFO**)pArg);

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
	m_vUVFixAcc = { 0.f,0.f };
	switch (m_pInfo->iChargeCount)
	{
	case 1:
	{
		CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
		Info->fBlurPower = 0.8f;
		Info->vColor = { 0.4f,0.f,1.f,1.f };
		Info->vColor2 = { 0.f,0.f,0.f,1.f };
		Info->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 1.f,3.f };
		Info->vRatioSpeed = { 3.f,4.f };
		Info->vScale = { 0.1f,0.1f,0.1f };
		Info->vScaleSpeed = { 100.f,100.f,100.f };
		Info->vUvFixSpeed = { 1.f,3.f };
		Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		Info->bFixPos = true;
		Info->fLookSpeed = m_pInfo->fSpeed;
		GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
		PTM->Create_InstanceParticle(L"Player04BallDistortion", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player04BallParticle", m_pTransformCom->Get_WorldFloat4x4());
		m_eHitType = TYPE_HIT;
		m_fPushPower = 1.f;
		GI->StopSound(SD_PLAYER4_SK); //차징하는 소리 끊겨도 계속 나오는거 막기
		GI->PlaySoundW(L"SE_BT_ZAKO16_SKILL022.wav", SD_PLAYER03_RE, 0.8f);
		break;
	}

	case 2:
	{
		CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
		Info->fBlurPower = 0.8f;
		Info->vColor = { 0.4f,0.f,1.f,1.f };
		Info->vColor2 = { 0.f,0.f,0.f,1.f };
		Info->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 1.f,3.f };
		Info->vRatioSpeed = { 3.f,4.f };
		Info->vScale = { 0.1f,0.1f,0.1f };
		Info->vScaleSpeed = { 150.f,150.f,150.f };
		Info->vUvFixSpeed = { 1.f,3.f };
		Info->bFixPos = true;
		Info->fLookSpeed = m_pInfo->fSpeed;
		Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
		PTM->Create_InstanceParticle(L"Player04BallDistortion", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player04BallParticle", m_pTransformCom->Get_WorldFloat4x4());
		m_eHitType = TYPE_HIT;
		m_fPushPower = 2.f;
		GI->StopSound(SD_PLAYER4_SK); //차징하는 소리 끊겨도 계속 나오는거 막기
		GI->PlaySoundW(L"SE_BT_ZAKO16_SKILL022.wav", SD_PLAYER03_RE, 0.8f);
		break;
	}

	case 3:
	{
		

		CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
		Info->fBlurPower = 0.8f;
		Info->vColor = { 0.4f,0.f,1.f,1.f };
		Info->vColor2 = { 0.f,0.f,0.f,1.f };
		Info->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 1.f,3.f };
		Info->vRatioSpeed = { 3.f,4.f };
		Info->vScale = { 0.1f,0.1f,0.1f };
		Info->vScaleSpeed = { 250.f,250.f,250.f };
		Info->vUvFixSpeed = { 1.f,3.f };
		Info->bFixPos = true;
		Info->fLookSpeed = m_pInfo->fSpeed;
		Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
		PTM->Create_InstanceParticle(L"Player04BallParticle_2", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player04Skill11Particle", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player04BallDistortion_2", m_pTransformCom->Get_WorldFloat4x4());
		
		m_eHitType = TYPE_SPIN;
		m_fPushPower = 1.f;
		m_fAirPower = 3.f;
		GI->StopSound(SD_PLAYER4_SK); //차징하는 소리 끊겨도 계속 나오는거 막기
		GI->PlaySoundW(L"SE_BT_ZAKO16_SKILL022.wav", SD_PLAYER03_RE, 0.8f);
		break;
	}

	}


	
	Make_ImGui();
	

	return S_OK;
}

void CPlayer04DarkBall::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_EFFECT, this);
	fTimeDelta *= m_fTMRatio;
	Emp_Col();
	m_fTimeLimitAcc += 1.f * fTimeDelta;

	m_pTransformCom->Go_Dir(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_pInfo->fSpeed, fTimeDelta);
	m_bAttackCol = false;

	if (m_bScaleUp)
	{
		m_vScaleTimeAcc.x += 1.f * fTimeDelta;
		m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pTransformCom->Get_Scale()) + _vector{ m_pInfo->vScaleSpeed.x, m_pInfo->vScaleSpeed.x, m_pInfo->vScaleSpeed.x } * fTimeDelta);
		if (m_vScaleTimeAcc.x >= m_pInfo->vScaleTime.x)
		{
			m_bScaleUp = false;
			m_vScaleTimeAcc.x = 0.f;
		}
	}
	else
	{
		m_vScaleTimeAcc.y += 1.f * fTimeDelta;
		m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pTransformCom->Get_Scale()) + (_vector{ m_pInfo->vScaleSpeed.y, m_pInfo->vScaleSpeed.y, m_pInfo->vScaleSpeed.y } * -1.f) * fTimeDelta);
		if (m_vScaleTimeAcc.y >= m_pInfo->vScaleTime.y)
		{
			m_bScaleUp = true;
			m_vScaleTimeAcc.y = 0.f;
			
			m_bAttackCol = true;
			switch (m_pInfo->iChargeCount)
			{
			case 1:
			{
				CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
				Info->fBlurPower = 0.8f;
				Info->vColor = { 0.4f,0.f,1.f,1.f };
				Info->vColor2 = { 0.f,0.f,0.f,1.f };
				Info->vLocalPos = { 0.f,0.f,0.f,1.f };
				Info->vRatio = { 1.f,3.f };
				Info->vRatioSpeed = { 3.f,4.f };
				Info->vScale = { 0.1f,0.1f,0.1f };
				Info->vScaleSpeed = { 100.f,100.f,100.f };
				Info->vUvFixSpeed = { 1.f,3.f };
				Info->bFixPos = true;
				Info->fLookSpeed = m_pInfo->fSpeed;
				Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				PTM->Create_InstanceParticle(L"Player04BallDistortion", m_pTransformCom->Get_WorldFloat4x4());
				PTM->Create_InstanceParticle(L"Player04BallParticle", m_pTransformCom->Get_WorldFloat4x4());
				GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
				GI->PlaySoundW(L"SE_BT_SS_SKILL02_2.wav", SD_PLAYER03_RE, 0.8f);
			}
				break;
			case 2:
			{
				CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
				Info->fBlurPower = 0.6f;
				Info->vColor = { 0.4f,0.f,1.f,1.f };
				Info->vColor2 = { 0.f,0.f,0.f,1.f };
				Info->vLocalPos = { 0.f,0.f,0.f,1.f };
				Info->vRatio = { 1.f,3.f };
				Info->vRatioSpeed = { 4.f,5.f };
				Info->vScale = { 0.1f,0.1f,0.1f };
				Info->vScaleSpeed = { 150.f,150.f,150.f };
				Info->vUvFixSpeed = { 1.f,3.f };
				Info->bFixPos = true;
				Info->fLookSpeed = m_pInfo->fSpeed;
				Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				PTM->Create_InstanceParticle(L"Player04BallDistortion", m_pTransformCom->Get_WorldFloat4x4());
				PTM->Create_InstanceParticle(L"Player04BallParticle", m_pTransformCom->Get_WorldFloat4x4());
				GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
				GI->PlaySoundW(L"SE_BT_SS_SKILL02_2.wav", SD_PLAYER03_RE, 0.8f);
			}
				break;
			case 3:
			{

				CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
				Info->fBlurPower = 0.6f;
				Info->vColor = { 0.f,0.f,0.f,1.f };
				Info->vColor2 = { 0.4f,0.f,1.f,1.f };
				Info->vLocalPos = { 0.f,0.f,0.f,1.f };
				Info->vRatio = { 3.f,2.f };
				Info->vRatioSpeed = { 6.f,4.f };
				Info->vScale = { 0.1f,0.1f,0.1f };
				Info->vScaleSpeed = { 200.f,200.f,200.f };
				Info->vUvFixSpeed = { 1.f,3.f };
				Info->bFixPos = true;
				Info->fLookSpeed = m_pInfo->fSpeed;
				Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
				PTM->Create_InstanceParticle(L"Player04BallParticle_2", m_pTransformCom->Get_WorldFloat4x4());
				PTM->Create_AlphaParticle(L"Player04Skill11Particle", m_pTransformCom->Get_WorldFloat4x4());
				PTM->Create_InstanceParticle(L"Player04BallDistortion_2", m_pTransformCom->Get_WorldFloat4x4());
				GI->PlaySoundW(L"SE_BT_SS_SKILL02_2.wav", SD_PLAYER03_RE, 0.8f);
				
			}
				break;
			
			}
			
		}
	}
		
		
	

	if (m_fTimeLimitAcc >= m_pInfo->fTimeLimit)
	{
			
		if (m_pInfo->vRatio.x > 0.f)
		{
			m_pInfo->vRatio.x -= m_pInfo->vRatioSpeed.x * fTimeDelta;
			m_pInfo->vRatio.y -= m_pInfo->vRatioSpeed.y * fTimeDelta;
		}

		if (m_pInfo->vRatio.x <= 0.f || m_pInfo->vRatio.y <= 0.f)
		{
			Set_Dead();
		}
	}
	
	
}

void CPlayer04DarkBall::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;
	Tick_Col(m_pTransformCom->Get_WorldMatrix_NoScale());
	
	if(m_bAttackCol)
		Choose_Col("Player_Attack");
	
	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NOBLUR, this);
	Render_Col(m_pRendererCom);
}

HRESULT CPlayer04DarkBall::Render()
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
		if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixX", &m_vUVFixAcc.x, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixY", &m_vUVFixAcc.y, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Render(m_pShaderCom,i, 2)))
			return E_FAIL;
	}
	return S_OK;
}

void CPlayer04DarkBall::Render_Debug()
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

void CPlayer04DarkBall::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
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
				
			
		}
	}
}

HRESULT CPlayer04DarkBall::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Player04MeshEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_BlastBall"), TEXT("Effect_BlastBall"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(_float(m_pInfo->iChargeCount) * 2.f, 0.f, 0.f);
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "Player_Attack");

	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	return S_OK;
}

CCreture * CPlayer04DarkBall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer04DarkBall*		pInstance = new CPlayer04DarkBall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer04DarkBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer04DarkBall::Clone(void * pArg)
{
	CPlayer04DarkBall*		pInstance = new CPlayer04DarkBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer04DarkBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer04DarkBall::Free()
{
	__super::Free();
	if(m_pInfo != nullptr)
	Safe_Delete(m_pInfo);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	

}
