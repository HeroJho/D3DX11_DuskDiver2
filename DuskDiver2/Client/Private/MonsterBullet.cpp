#include "stdafx.h"
#include "..\Public\MonsterBullet.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "MonsterBullet.h"
#include "Player_Manager.h"
#include "Time_Manager.h"
#include "PointTrailObj.h"
#include "PointTrail.h"
#include "Particle_Manager.h"
#include "Player01SmokeBall.h"

CMonsterBullet::CMonsterBullet(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CMonsterBullet::CMonsterBullet(const CMonsterBullet & rhs)
	: CCreture(rhs)
{
}

HRESULT CMonsterBullet::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonsterBullet::Initialize(void * pArg)
{

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Trails()))
		return E_FAIL;

	m_sTag = "Tag_Monster";

	//pArg로 정보 받아서 직선 불릿또는 포물선 불릿으로 구분해야됨

	if (nullptr != pArg)
	{
		m_BulletInfo = *(CMonsterBullet::BULLETINFO*)pArg;
		m_eHitType = (HITTYPE)m_BulletInfo.iHitType;
		m_iBulletType = m_BulletInfo.iBulletType; //0이면 직선 1이면 포물선
		_vector vPosition = XMLoadFloat4(&m_BulletInfo.vPosition);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition); //처음 생성위치
		m_pTransformCom->Set_Look(XMLoadFloat4(&m_BulletInfo.vDir));

		XMStoreFloat3(&m_vStartPos, vPosition); //시작 지점
		XMStoreFloat3(&m_vPos, vPosition); //현재지점
	}

	m_pTransformCom->Set_Scale(XMVectorSet(3.5f, 3.5f, 3.5f, 1.f));

	if (FAILED(Ready_Components()))
		return E_FAIL;

	switch (m_eHitType)
	{
	case Client::CCreture::TYPE_HIT:
		m_fPushPower = 8.f;
		m_fAirPower = 0.f;
		m_fDamage = 110.f;
		break;
	case Client::CCreture::TYPE_SPIN:
		m_fPushPower = 9.f;
		m_fAirPower = 6.f;
		m_fDamage = 130.f;
		break;
	case Client::CCreture::TYPE_AWAY:
		m_fPushPower = 8.f;
		m_fAirPower = 5.f;
		m_fDamage = 150.f;
		m_pTransformCom->Set_Scale(XMVectorSet(4.f, 4.f, 4.f, 1.f));
		break;
	case Client::CCreture::TYPE_UP:
		m_fPushPower = 5.f;
		m_fAirPower = 4.f;
		m_fDamage = 100.f;
		break;
	case Client::CCreture::TYPE_FASTDOWN:
		break;
	case Client::CCreture::TYPE_END:
		break;
	default:
		break;
	}

	m_vPlayerPos = PM->Get_PlayerPos();

	switch (m_iBulletType)
	{
	case 0:
		ReadyToDirect();
		m_fSpeed = 16.f;
		m_fDamage = 110.f;
		break;
	case 1:
		ReadyToParabolic();
		m_fDamage = 150.f;
		break;
	case 2:
		ReadyToDirect();
		m_fSpeed = 18.f;
		m_fDamage = 100.f;
		break;
	case 3:
		ReadyToDiagonal();
		m_fSpeed = 18.f;
		m_fDamage = 130.f;
		break;
	default:
		break;
	}
	return S_OK;
}

void CMonsterBullet::Tick(_float fTimeDelta)
{

	TIM->Add_TimeObj(CTime_Manager::OBJ_MONSTER, this);

	fTimeDelta *= m_fTMRatio;
	
	Emp_Col();

	Create_PointTrail(fTimeDelta);


	switch (m_iBulletType)
	{
	case 0:
		Direct_Shoot(fTimeDelta);
		break;
	case 1:
		Parabolic_Shoot(fTimeDelta);
		break;
	case 2:
		Direct_Shoot(fTimeDelta);
		break;
	case 3:
		Direct_Shoot(fTimeDelta);
		break;
	default:
		break;
	}

	DeadTime(fTimeDelta);
}

void CMonsterBullet::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	fTimeDelta *= m_fTMRatio;

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	Choose_Col(nullptr);

	m_PointTrails->LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	Render_Col(m_pRendererCom);
}

HRESULT CMonsterBullet::Render()
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

	m_pShaderCom->Set_RawValue("g_bFixColor", &m_BulletInfo.bFixColor, sizeof(_bool));

	RELEASE_INSTANCE(CGameInstance);


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_BulletInfo.vColor, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor2", &m_BulletInfo.vColor2, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio1", &m_BulletInfo.vRatio.x, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio2", &m_BulletInfo.vRatio.y, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_fBlurPower", &m_BulletInfo.fBlurPower, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 1)))
			return E_FAIL;
	}

	return S_OK;
}

void CMonsterBullet::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	_float3 vSize = Get_Colliders("Monster_Attack")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Monster_Attack")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Get_Colliders("Monster_Attack")->Set_Size(vSize);
	Get_Colliders("Monster_Attack")->Set_Center(vCenter);
	ImGui::End();
}

void CMonsterBullet::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		// 내 손 Sphere 둘중 하나와 상대편의 OBB와 충돌 했다면
		if (!strcmp("Monster_Attack", Desc.pMyDesc->sTag) && !strcmp("Player_Body", Desc.pOtherDesc->sTag))
		{
			((CCreture*)Desc.pOther)->On_Hit(this);
			Set_Dead();
			GI->PlaySoundW(L"se_EN0007_attack1_4.wav", SD_BULLET, 0.6f);
			//이펙트 플레이어랑 맞았을때
		}
	}
}

void CMonsterBullet::Create_HitEffect(_float4 vWorldPos)
{
	_matrix a = m_pTransformCom->Get_WorldMatrix();
	a.r[0] = XMVector3Normalize(a.r[0]);
	a.r[1] = XMVector3Normalize(a.r[1]);
	a.r[2] = XMVector3Normalize(a.r[2]);
	_float4x4 Matrix;
	XMStoreFloat4x4(&Matrix, a);
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
	(PTM->Create_InstanceParticle(L"ExplosionBlackSmoke", m_pTransformCom->Get_WorldFloat4x4()));
	(PTM->Create_InstanceParticle(L"ExplosionParticle", m_pTransformCom->Get_WorldFloat4x4()));
	(PTM->Create_InstanceParticle(L"ExplosionRed", m_pTransformCom->Get_WorldFloat4x4()));
	(PTM->Create_InstanceParticle(L"ExplosionWreck2", m_pTransformCom->Get_WorldFloat4x4()));
	(PTM->Create_InstanceParticle(L"ExplosionBlackSmokeSmall", m_pTransformCom->Get_WorldFloat4x4()));
	(PTM->Create_InstanceParticle(L"ExplosionParticle", m_pTransformCom->Get_WorldFloat4x4()));
	(PTM->Create_InstanceParticle(L"ExplosionPlayer", m_pTransformCom->Get_WorldFloat4x4()));
}

HRESULT CMonsterBullet::Ready_Trails()
{
	CPointTrail::TRAILCREATEINFO Info2;
	Info2.vColor = { 1.f, 0.980f, 0.819f,1.f };
	Info2.vColor2 = { 0.933f,0.647f,0.337f,1.f };
	Info2.bFixColor = true;
	Info2.fBlurPower = 0.7f;
	Info2.iShaderPass = 1;
	Info2.vRatio = { 0.9f, 1.3f };
	Info2.fSize = 0.2f;
	Info2.TexName = L"MissileTrail2";
	Info2.iMaxIndex = 7;

	CGameObject*		MissilePoint = GI->Clone_GameObject(TEXT("PointTrailObj"), &Info2);
	if (nullptr == MissilePoint)
		return E_FAIL;

	m_PointTrails = (CPointTrailObj*)MissilePoint;

	return S_OK;
}

void CMonsterBullet::Create_Particle(_float fTimeDelta)
{
	m_fParticleAcc += 1.f * fTimeDelta;

	if (m_fParticleAcc > 0.24f)
		m_bParticle = true;

	if (m_bParticle)
	{
		m_bParticle = false;
		m_fParticleAcc = 0.f;

		if(m_iBulletType == 1)
			(PTM->Create_AlphaParticle(L"BulletFlare", m_pTransformCom->Get_WorldFloat4x4()));
		else
			(PTM->Create_AlphaParticle(L"BulletFlareRandom", m_pTransformCom->Get_WorldFloat4x4()));
	}
}

void CMonsterBullet::DeadTime(_float fTimeDelta)
{
	m_fDeadAcc += 1.f * fTimeDelta;


	if (m_fDeadAcc > 5.f)
	{
		Set_Dead();
		//이펙트
	}

	if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] < PM->Get_PlayerPos().y - 0.2f)
	{
		Set_Dead();

		_matrix a = m_pTransformCom->Get_WorldMatrix();
		a.r[0] = XMVector3Normalize(a.r[0]);
		a.r[1] = XMVector3Normalize(a.r[1]);
		a.r[2] = XMVector3Normalize(a.r[2]);
		_float4x4 Matrix;
		XMStoreFloat4x4(&Matrix, a);

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
		PTM->Create_InstanceParticle(L"ExplosionEmbers", Matrix);
		PTM->Create_InstanceParticle(L"Player03Bomb_5", Matrix);
		PTM->Create_InstanceParticle(L"Player03Bomb_6", Matrix);
		PTM->Create_InstanceParticle(L"Player03Bomb_7", Matrix);
		PTM->Create_InstanceParticle(L"Player03Bomb_8", Matrix);
	}
}

void CMonsterBullet::ReadyToDirect()
{
	_vector vLook = XMVector3Normalize(XMLoadFloat4(&m_vPlayerPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	_float3		vScale = m_pTransformCom->Get_Scale();

	_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;

	vLook = XMVector3Normalize(XMVector3Cross(vRight, m_pTransformCom->Get_State(CTransform::STATE_UP))) * vScale.z;

	XMStoreFloat3(&m_vTargetLook, vLook);

}

void CMonsterBullet::ReadyToDiagonal()
{
	_vector vLook = XMVector3Normalize(XMLoadFloat4(&m_vPlayerPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	XMStoreFloat3(&m_vTargetLook, vLook);

}

void CMonsterBullet::ReadyToParabolic()
{
	XMStoreFloat3(&m_vEndPos, XMLoadFloat4(&m_vPlayerPos));

	//m_fMaxHeight = GI->Get_FloatRandom(5.f, 6.f) ;
	_float fDistance = m_pTransformCom->TargetDistance(XMLoadFloat4(&m_vPlayerPos)); //타겟과 나의 거리 계속 계산

	if (fDistance <= 7.f)
		m_fMaxHeight = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] + 2.f;
	else
		m_fMaxHeight = m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] + 5.f;

}

void CMonsterBullet::Direct_Shoot(_float fTimeDelta)
{
	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), m_fSpeed, fTimeDelta);

	if(m_iBulletType == 3)
		m_pTransformCom->LookAt(XMLoadFloat4(&m_vPlayerPos));

}

void CMonsterBullet::Parabolic_Shoot(_float fTimeDelta)
{
	m_fEndHeight = m_vEndPos.y - m_vStartPos.y;

	m_fHeight = m_fMaxHeight - m_vStartPos.y;

	m_fg = 2.f * m_fHeight / (m_fMaxTime * m_fMaxTime);

	m_fSpeedY = sqrtf(2 * m_fg * m_fHeight);

	//if(m_vPos.y >= m_fMaxHeight)
	//	m_pTransformCom->Rotation(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(226.f));

	_float a = m_fg;
	_float b = -2.f * m_fSpeedY;
	_float c = 2.f * m_fEndHeight;

	m_fEndTime = (-b + sqrtf(b * b - 4 * a * c)) / (2 * a);

	m_fSpeedX = -(m_vStartPos.x - m_vEndPos.x) / m_fEndTime;
	m_fSpeedZ = -(m_vStartPos.z - m_vEndPos.z) / m_fEndTime;

	m_fTime += 1.5f * fTimeDelta;

	m_vPos.x = m_vStartPos.x + m_fSpeedX * m_fTime;
	m_vPos.y = m_vStartPos.y + (m_fSpeedY * m_fTime) - (0.5f * m_fg * m_fTime * m_fTime);
	m_vPos.z = m_vStartPos.z + m_fSpeedZ * m_fTime;
	m_pTransformCom->LookAt(XMLoadFloat3(&m_vPos));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_vPos.x, m_vPos.y, m_vPos.z, 1.f));

}

void CMonsterBullet::Create_PointTrail(_float fTimeDelta)
{

	m_PointTrails->TrailOn();

	_matrix	BulletMatrix = m_pTransformCom->Get_WorldMatrix();

	_float4 BulletPos;

	_vector vTemp = BulletMatrix.r[3];

	_vector vLook = BulletMatrix.r[2];

	vLook = XMVector3Normalize(vLook) * 0.25f;
	vTemp += vLook; //좌우

	XMStoreFloat4(&BulletPos, vTemp);
	m_PointTrails->Add_Point(BulletPos);

	m_PointTrails->Tick(fTimeDelta);

	Create_Particle(fTimeDelta);

}


HRESULT CMonsterBullet::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MonsterMeshEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("MonsterBullet"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);

	if(m_iBulletType == 1)
		ColDesc.vSize = _float3(0.7f, 0.7f, 0.7f);
	else
		ColDesc.vSize = _float3(0.4f, 0.4f, 0.4f);
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "Monster_Attack");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}





CMonsterBullet * CMonsterBullet::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonsterBullet*		pInstance = new CMonsterBullet(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMonsterBullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMonsterBullet::Clone(void * pArg)
{
	CMonsterBullet*		pInstance = new CMonsterBullet(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMonsterBullet"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CMonsterBullet::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_PointTrails);

}
