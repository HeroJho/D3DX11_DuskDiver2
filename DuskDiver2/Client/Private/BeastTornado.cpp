#include "stdafx.h"
#include "..\Public\BeastTornado.h"
#include "GameInstance.h"
#include "Status_Manager.h"
#include "Camera_Manager.h"
#include "Time_Manager.h"
#include "Particle_Manager.h"
#include "ToolManager.h"
#include "ViewAlpha.h"
#include "Player_Manager.h"
#include "Effect_Manager.h"
#include "Player01SmokeBall.h"

CBeastTornado::CBeastTornado(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCreture(pDevice, pContext)
{
}

CBeastTornado::CBeastTornado(const CBeastTornado& rhs)
	: CCreture(rhs)
{
}

HRESULT CBeastTornado::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBeastTornado::Initialize(void * pArg)
{

	m_pInfo = (*(TORNADOINFO**)pArg);

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_bDead = false;


	m_eHitType = TYPE_SPIN;
	m_ePushType = PUSH;
	m_fPushPower = 30.f;
	m_fAirPower = 6.f;
	m_fDamage = 150.f;

	Make_ImGui();

	m_vTornadoScale.x = 1.f;
	m_vTornadoScale.y = 1.f;
	m_vTornadoScale.z = 1.f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_pInfo->vLocalPos));

	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pInfo->vScale));

	if (m_pInfo->bOrigin)
		EM->Add_BeastTornado(this);

	return S_OK;
}

void CBeastTornado::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_EFFECT, this);
	fTimeDelta *= m_fTMRatio;

	Emp_Col();
	m_fLifeTimeAcc += 1.f * fTimeDelta;
	
	if (EM->Get_BeastTornadoEnd())
	{
		m_bDeadStart = true;
		m_fDeadAcc = 3.f;
	}

	if (!m_bScaleEnd)
	{
		m_fParticleAcc += 1.f * fTimeDelta;
		if (m_fParticleAcc >= 0.3f)
		{
			m_fParticleAcc = 0.f;
			_matrix Identity = XMMatrixIdentity();
			Identity.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_float4x4 Matrix;
			XMStoreFloat4x4(&Matrix, Identity);
			PTM->Create_InstanceParticle(L"BeastTornado", Matrix);
			PTM->Create_InstanceParticle(L"BeastTornado_2", Matrix);

			if (m_fLifeTimeAcc >= 1.f)
				PTM->Create_InstanceParticle(L"BeastTornado_7", Matrix);
		}
	}
	
	else
	{
		m_fParticleAcc += 1.f * fTimeDelta;
		if (m_fParticleAcc >= 0.3f)
		{
			m_fParticleAcc = 0.f;
			_matrix Identity = XMMatrixIdentity();
			Identity.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_float4x4 Matrix;
			XMStoreFloat4x4(&Matrix, Identity);
			PTM->Create_InstanceParticle(L"BeastTornado_9", Matrix);
			PTM->Create_InstanceParticle(L"BeastTornado_2", Matrix);		
			PTM->Create_InstanceParticle(L"BeastTornado_8", Matrix);
		}
	}

	Check_Death(fTimeDelta);

	if (m_pInfo->bOrigin)
	{
		Check_Hurt(fTimeDelta);
		if (!m_bScaleEnd)
		{
			m_fSoundAcc += 1.f * fTimeDelta;
			if (m_fSoundAcc >= 0.6f)
			{
				GI->PlaySoundW(L"EFF_CHR_Spell_Wind_03_A.ogg", SD_DEFENCEBOT, 0.4f);
				m_fSoundAcc = 0.f;
			}
		}
	}
	if(!m_bDeadStart)
		Change_TornadoSize(fTimeDelta); //토네이도 사이즈 점점 커지게
	Chase_Player(); // 플레이어 추격
	Instant_ChangeSize(fTimeDelta); //정해진 사이즈 됐을때 순간적으로 커졌다가 마지막 본 곳으로 날라감
	
	m_vUVFixAcc.x += m_pInfo->vUVFixSpeed.x * fTimeDelta;
	m_vUVFixAcc.y += m_pInfo->vUVFixSpeed.y * fTimeDelta;

	//토네이도 회전, 이동
	m_pTransformCom->Turn(_vector{ 0.f,1.f,0.f }, m_fTurnSpeed, fTimeDelta);
	if(!m_bChange)
		m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), m_fSpeed, fTimeDelta);
}

void CBeastTornado::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	if (m_pInfo->bOrigin)
	{
		
			Choose_Col("Tornado_Attack");
			Tick_Col(m_pTransformCom->Get_WorldMatrix_NoScale());
			Render_Col(m_pRendererCom);
			
	}

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);	
	

}

HRESULT CBeastTornado::Render()
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
		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 2)))
			return E_FAIL;
	}

	return S_OK;
}

void CBeastTornado::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());
	_float3 vSize = Get_Colliders("Tornado_Attack")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Tornado_Attack")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);

	Get_Colliders("Tornado_Attack")->Set_Size(vSize);
	Get_Colliders("Tornado_Attack")->Set_Center(vCenter);

	ImGui::End();
}

void CBeastTornado::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		if ((!strcmp("Tornado_Attack", Desc.pMyDesc->sTag) &&
			!strcmp("Player_Body", Desc.pOtherDesc->sTag)))
		{
			m_bColDelay = true;

			if (m_bPlayerHit && m_bPlayerChase)
			{
				((CCreture*)Desc.pOther)->On_Hit(this);
				CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
				m_fDamage = 150.f;
			}
			
		}
	}
	if ("Tag_Boss" == Desc.pOther->Get_Tag())
	{
		if ((!strcmp("Tornado_Attack", Desc.pMyDesc->sTag) &&
			!strcmp("Monster_Push_Range", Desc.pOtherDesc->sTag)))
		{
			if (m_bOneCol)
			{
				m_bOneCol = false;
				m_bDeadStart = true;
				m_fDeadAcc = 3.f;
				m_fDamage = 1000.f;
				((CCreture*)Desc.pOther)->On_Hit(this);
				CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
			}
		}
	}
}

void CBeastTornado::Create_HitEffect(_float4 vWorldPos)
{
	_matrix Matrix = XMMatrixIdentity();
	Matrix.r[3] = XMLoadFloat4(&vWorldPos);
	_float4x4 a;
	XMStoreFloat4x4(&a, Matrix);
	PTM->Create_AlphaParticle(L"GunHit", a);
}

HRESULT CBeastTornado::Ready_Components()
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
		ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
		ColDesc.vSize = _float3(0.8f, 1.f, 0.8f);
		ColDesc.vCenter = _float3(0.f, 0.5f, 0.f);
		ColDesc.pOwner = this;
		ColDesc.iFlag = 2;
		strcpy(ColDesc.sTag, "Tornado_Attack");

		if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
			return E_FAIL;
	}
	return S_OK;
}

void CBeastTornado::Change_ColRange(_float3 vSize, _float3 vCenter)
{
	Get_Colliders("Tornado_Attack")->Set_Size(vSize);
	Get_Colliders("Tornado_Attack")->Set_Center(vCenter);

	/*ColDesc.vSize = _float3(0.4f, 0.5f, 0.4f);
	ColDesc.vCenter = _float3(0.f, 0.5f, 0.f);*/
}

void CBeastTornado::Check_Death(_float fTimeDelta)
{
	if (m_bDeadStart)
	{
	
		m_fDeadAcc += 1.f * fTimeDelta;
		if (m_fDeadAcc >= 3.f)
		{
			if (!m_bTorandoEnd)
			{
				CViewAlpha::VIEWALPHAINFO* Info2 = new CViewAlpha::VIEWALPHAINFO;
				Info2->fBlurPower = 1.f;
				Info2->vColor1 = { 1.f,1.f,1.f,1.f };
				Info2->vColor2 = { 1.f,0.f,0.f,1.f };
				Info2->vRatio = { 0.7f,0.7f };
				Info2->vRatioSpeed = { 1.f,2.f };
				GI->Add_GameObjectToLayer(L"ViewAlpha", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2);
				_matrix Identity = XMMatrixIdentity();
				Identity.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				_float4x4 Matrix;
				XMStoreFloat4x4(&Matrix, Identity);
				PTM->Create_InstanceParticle(L"BeastTornado_10", Matrix);
				GI->PlaySoundW(L"EFF_CHR_Spell_Ground_17_D.ogg", SD_FUSION, 0.6f);
					
			}
			m_bTorandoEnd = true;
			m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pTransformCom->Get_Scale()) + _vector{ 200.f*fTimeDelta, 100.f*fTimeDelta,200.f*fTimeDelta });
			m_pInfo->vRatio.x -= m_pInfo->vRatioSpeed.x * fTimeDelta;
			m_pInfo->vRatio.y -= m_pInfo->vRatioSpeed.y * fTimeDelta;

			

			if (m_pInfo->vRatio.x <= 0.f)
			{
				EM->ReleaseTornado();
				Set_Dead();

			}
		}
	}
}

void CBeastTornado::Check_Hurt(_float fTimeDelta)
{
	if (m_bColDelay) //충돌 텀 주기
	{
		m_bPlayerHit = false;
		m_fColDelayTime += 0.1f * fTimeDelta;

		if (m_fColDelayTime > 0.2f)
		{
			m_fColDelayTime = 0.f;
			m_bPlayerHit = true;
			m_bColDelay = false;
		}
	}
}

void CBeastTornado::Change_TornadoSize(_float fTimeDelta)
{
	if (m_fLifeTimeAcc >= 10.f && !m_bScaleEnd)
	{	
		if (m_pInfo->bOrigin)
		{
			m_pInfo->vColor = { 0.f,0.f,0.f,1.f };
			m_pInfo->vColor2 = { 0.f,0.f,0.f,1.f };
			m_pInfo->fBlurPower = 1.f;
		}

		else
		{
			m_pInfo->vColor = { 1.f,1.f,1.f,1.f };
			m_pInfo->vColor2 = { 1.f,0.f,0.f,1.f };
			m_pInfo->fBlurPower = 1.f;
		}
		m_bScaleEnd = true;
		m_bChange = true;
		m_bPlayerChase = false;
		GI->PlaySoundW(L"EFF_CHR_Buff_Rage_01_A.ogg", SD_DISASTERBOSS, 0.5f);

		_matrix Identity = XMMatrixIdentity();
		Identity.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float4x4 Matrix;
		XMStoreFloat4x4(&Matrix, Identity);
		PTM->Create_InstanceParticle(L"BeastTornado_3", Matrix);
		PTM->Create_InstanceParticle(L"BeastTornado_4", Matrix);
		CRM->Get_Cam()->Shake(0.3f, 0.3f, false, 0.1f);

		

		
	}
	else if (m_vTornadoScale.y <= 15.f)
	{
		m_vTornadoScale.x += 7.f * fTimeDelta;
		m_vTornadoScale.y += 9.f * fTimeDelta;
		m_vTornadoScale.z += 7.f * fTimeDelta;
	
	}

	//mesh, collider 사이즈 변경
	m_pTransformCom->Set_Scale(XMVectorSet(m_vTornadoScale.x + m_fPlusScale, m_vTornadoScale.y, m_vTornadoScale.z + m_fPlusScale, 0.f));
	
	if(m_pInfo->bOrigin)
		Change_ColRange(_float3{3.f, 10.f, 3.f }, _float3{ 0.f, 10.f, 0.f });
	
	if(m_fTurnSpeed <= 10.f)
		m_fTurnSpeed += 0.8f * fTimeDelta;

	if (m_fSpeed <= 5.f)
		m_fSpeed += 0.8f * fTimeDelta;

}

void CBeastTornado::Instant_ChangeSize(_float fTimeDelta)
{

	if (m_bChange)
	{
		m_fScaleTimeAcc += 1.f * fTimeDelta;
		if (m_bInstantSize)
			m_fPlusScale += 5.f * fTimeDelta;
		else
		{
			m_fPlusScale -= 5.f * fTimeDelta;

			if (m_fPlusScale <= 0.f && !m_bDeadStart)
			{
				m_fPlusScale = 0.f;
				m_bChange = false;
				m_bDeadStart = true;
				m_bPlayerChase = false;
				m_fSpeed = 22.f;
				m_fDamage = 100.f;
				GI->PlaySoundW(L"TornadoGo.ogg", SD_BULLET, 0.6f);
				m_vPlayerPos = PM->Get_PlayerPos();
				m_vPlayerPos.y = 0.02f; 
				_vector vLook = XMVector3Normalize(XMLoadFloat4(&m_vPlayerPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));
				XMStoreFloat3(&m_vTargetLook, vLook);
				_matrix Identity = XMMatrixIdentity();
				Identity.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				_float4x4 Matrix;
				XMStoreFloat4x4(&Matrix, Identity);
				PTM->Create_InstanceParticle(L"BeastTornado_5", Matrix);
				PTM->Create_InstanceParticle(L"BeastTornado_6", Matrix);
				CRM->Get_Cam()->Shake(0.3f, 0.3f, false, 0.1f);
				
			}
		}

		if (m_fScaleTimeAcc >= 1.5f)
		{
			m_bInstantSize = false;
			
		
		}

	}
}

void CBeastTornado::Chase_Player()
{
	if (m_bPlayerChase)
	{
		m_vPlayerPos = PM->Get_PlayerPos();

		m_vPlayerPos.y = 0.02f; //네비 안태우는 대신에 y는 고정

		_vector vLook = XMVector3Normalize(XMLoadFloat4(&m_vPlayerPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));

		XMStoreFloat3(&m_vTargetLook, vLook);
	}
}

CCreture * CBeastTornado::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBeastTornado*		pInstance = new CBeastTornado(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBeastTornado"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBeastTornado::Clone(void * pArg)
{
	CBeastTornado*		pInstance = new CBeastTornado(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBeastTornado"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBeastTornado::Free()
{
	__super::Free();

	Safe_Delete(m_pInfo);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
}
