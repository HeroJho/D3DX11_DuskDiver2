#include "stdafx.h"
#include "..\Public\BeastMarvelBall.h"
#include "GameInstance.h"
#include "Player.h"
#include "Creture.h"
#include "UI_Manager.h"
#include "Time_Manager.h"
#include "ToolManager.h"
#include "Effect_Manager.h"
#include "Camera_Manager.h"
#include "Particle_Manager.h"
#include "Player01SmokeBall.h"
#include "Monster_Manager.h"

CBeastMarvelBall::CBeastMarvelBall(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CBeastMarvelBall::CBeastMarvelBall(const CBeastMarvelBall & rhs)
	: CCreture(rhs)
{
}

HRESULT CBeastMarvelBall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBeastMarvelBall::Initialize(void * pArg)
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	m_pInfo = (*(BALLINFO**)pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_MarvelBall";
	Make_ImGui();

	m_eHitType = TYPE_SPIN;
	m_fPushPower = 0.f;
	m_fAirPower = 0.f;
	m_fDamage = 20000.f;
	m_fMaxHp = 0.f;
	m_fNowHp = m_fMaxHp;

	_matrix Identity = XMMatrixIdentity();
	_float4x4 Matrix;
	XMStoreFloat4x4(&Matrix, Identity);
	m_pTransformCom->Set_WorldMatrix(Matrix);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_pInfo->vPos));
	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pInfo->vScale));

	return S_OK;
}

void CBeastMarvelBall::Tick(_float fTimeDelta)
{

	TIM->Add_TimeObj(CTime_Manager::OBJ_MONSTER, this);
	fTimeDelta *= m_fTMRatio;
	Emp_Col();

	if (m_bRender)
	{

		if (m_bStartEffect)
		{
			m_bStartEffect = false;
			Create_AppearEffect();
		}

		if (m_bChaseTarget)
		{
			if (true == Move_MarvelBall(XMLoadFloat4(&m_vTargetPos[m_iTargetIndex]), 15.f, fTimeDelta, 0.5f))
			{

				GI->PlaySoundW(L"toyPiano.wav", SD_BEAST6, 1.f);

				++m_iTargetIndex;
			}
		}

		m_fEffectAcc += 1.f * fTimeDelta;
		if (m_fEffectAcc >= 0.1f)
		{
			Create_Effect();
			m_fEffectAcc = 0.f;
		}

		m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_UP), 5.f, fTimeDelta);

		if (m_iTargetIndex == 5) //마지막 인덱스까지 간 경우-> 성공
		{
			m_iTargetIndex = 0;
			m_bChaseTarget = false;
			m_bSuccess = true;
			EM->Set_MarbleEnd(true);
			GI->PlaySoundW(L"EFF_CHR_Spell_Ground_17_D.ogg", SD_BEAST6, 0.7f);
			m_bExplosion = false;
			m_bRender = false;
			Create_EndEffect();
		}
	}
}

void CBeastMarvelBall::LateTick(_float fTimeDelta)
{	
	if (nullptr == m_pRendererCom)
		return;

	Tick_Col(m_pTransformCom->Get_WorldMatrix_NoScale());
	
	if (m_bRender)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
	}
	
	if(m_bAttackCol)
		Choose_Col("Monster_Attack");

	Render_Col(m_pRendererCom);
}

HRESULT CBeastMarvelBall::Render()
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
		_float Zero = 0.f;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fMaxUVIndexX", &Zero, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixX", &Zero, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixY", &Zero, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 4)))
			return E_FAIL;
	}

	return S_OK;
}

void CBeastMarvelBall::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	//방해물이랑 부딪혔을때
	if ("Tag_Boss" == Desc.pOther->Get_Tag())
	{
		if (!strcmp("Monster_Attack", Desc.pMyDesc->sTag) && !strcmp("Obstacle_Body", Desc.pOtherDesc->sTag))
		{
			m_bAttackCol = false;
			m_bExplosion = true;
			m_bSuccess = false;

			if (m_bOneHit)
			{
				m_bOneHit = false;
				EM->Set_MarbleCol(true);
				GI->PlaySoundW(L"EFF_CHR_Spell_Disappearance_01_A.ogg", SD_BEAST5, 0.7f);
				Create_ColEffect();
				m_bRender = false;
				MM->Plus_BeastHp(2400.f);
				((CCreture*)Desc.pOther)->On_Hit(this);
				
			}
		}
	}

}

void CBeastMarvelBall::Set_TargetPos(_float4 vTargetPos, _uint iIndex)
{
	m_vTargetPos[iIndex] = vTargetPos;

	//4번째 인자는 무조건 비스트 앞이어야한다. 고정
	m_vTargetPos[4] = { 0.051f, 0.665f, -24.f, 1.f };
}

void CBeastMarvelBall::Reset_State()
{
	Reset_Position();
	m_iTargetIndex = 0;
	m_bChaseTarget = false;
	m_bRender = false;
	m_bExplosion = false;
	m_bOneHit = true;
	m_bSuccess = false;
	
}

void CBeastMarvelBall::Reset_Position()
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, { -0.451f, 1.5f, -66.831f, 1.f });

}

HRESULT CBeastMarvelBall::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Player04MeshEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Effect_Bomb"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.5f, 1.25f, 1.25f);
	ColDesc.vCenter = _float3(0.f, 0.5f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Attack"); //막는 오브젝트랑, 비스트랑 충돌해야함
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	return S_OK;
}

void CBeastMarvelBall::Create_ColEffect()
{
	_matrix Identity = XMMatrixIdentity();
	Identity.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float4x4 Matrix;
	XMStoreFloat4x4(&Matrix, Identity);
	CRM->Get_Cam()->Shake(0.3f, 0.2f, false, 0.1f);
	PTM->Create_InstanceParticle(L"BeastBallCol", Matrix);

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
	Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
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
	Info2->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
	GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2);
}

void CBeastMarvelBall::Create_EndEffect()
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
	Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
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
	Info2->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
	GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2);

	_matrix Identity = XMMatrixIdentity();
	Identity.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float4x4 Matrix;
	XMStoreFloat4x4(&Matrix, Identity);
	CRM->Get_Cam()->Shake(0.5f, 0.3f, false, 0.1f);
	PTM->Create_InstanceParticle(L"BeastBallEnd", Matrix);
}

void CBeastMarvelBall::Create_AppearEffect()
{
	_matrix Identity = XMMatrixIdentity();
	Identity.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float4x4 Matrix;
	XMStoreFloat4x4(&Matrix, Identity);
	PTM->Create_InstanceParticle(L"BeastBallAppear", Matrix);

}

void CBeastMarvelBall::Create_Effect()
{
	_matrix Identity = XMMatrixIdentity();
	Identity.r[3] = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float4x4 Matrix;
	XMStoreFloat4x4(&Matrix, Identity);
	PTM->Create_InstanceParticle(L"BeastBallEffect", Matrix);
}

_bool CBeastMarvelBall::Move_MarvelBall(_vector vTargetPos, _float fSpeed, _float fTimeDelta, _float fLimitDistance)
{
	_vector		vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	vTargetPos.m128_f32[1] += 1.5f;
	_vector		vDirection = vTargetPos - vPosition;

	_float		fDistance = XMVectorGetX(XMVector3Length(vDirection));

	if (fDistance > fLimitDistance)
		vPosition += XMVector3Normalize(vDirection) * fSpeed * fTimeDelta;
	else
		return true;


	m_pTransformCom->Set_Look(XMVector3Normalize(vDirection));
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);
	return false;
}

CBeastMarvelBall * CBeastMarvelBall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBeastMarvelBall*		pInstance = new CBeastMarvelBall(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBeastMarvelBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCreture * CBeastMarvelBall::Clone(void * pArg)
{
	CBeastMarvelBall*		pInstance = new CBeastMarvelBall(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBeastMarvelBall"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBeastMarvelBall::Free()
{
	__super::Free();
	Safe_Delete(m_pInfo);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);

}




void CBeastMarvelBall::Render_Debug()
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
