#include "stdafx.h"
#include "..\Public\MonsterMissile.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "MonsterBullet.h"
#include "Player_Manager.h"
#include "Time_Manager.h"
#include "ToolManager.h"
#include "PointTrail.h"
#include "PointTrailObj.h"
#include "Player01SmokeBall.h"
#include "Particle_Manager.h"

CMonsterMissile::CMonsterMissile(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CMonsterMissile::CMonsterMissile(const CMonsterMissile & rhs)
	: CCreture(rhs)
{
}

HRESULT CMonsterMissile::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMonsterMissile::Initialize(void * pArg)
{

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	if (FAILED(Ready_Trails()))
		return E_FAIL;

	m_sTag = "Tag_Monster";

	//pArg로 정보 받아서 직선 불릿또는 포물선 불릿으로 구분해야됨

	if (nullptr != pArg)
	{
		CMonsterMissile::MISSILEINFO MissileInfo = *(CMonsterMissile::MISSILEINFO*)pArg;
		m_eHitType = (HITTYPE)MissileInfo.iHitType;
		m_iBulletType = MissileInfo.iBulletType; //0이면 직선 1이면 포물선
		m_iMissileIndex = MissileInfo.iMissileIndex;
		m_vPosistion = MissileInfo.vPosition;
		m_vLook = MissileInfo.vDir;
		m_fSide = MissileInfo.m_fSide;
		vColor = { 0.929f, 0.905f, 0.870f, 1.f };
		vColor2 = { 0.929f, 0.f, 0.078f, 1.f };
		vRatio = { 3.f, 7.f };
		fBlurPower = 0.1f;
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vPosistion)); //처음 생성위치
	}

	switch (m_eHitType)
	{
	case Client::CCreture::TYPE_HIT:
		m_fPushPower = 5.f;
		m_fAirPower = 0.f;
		break;
	case Client::CCreture::TYPE_SPIN:
		m_fPushPower = 8.f;
		m_fAirPower = 7.f;
		break;
	case Client::CCreture::TYPE_AWAY:
		m_fPushPower = 8.f;
		m_fAirPower = 7.f;
		break;
	case Client::CCreture::TYPE_UP:
		m_fPushPower = 3.f;
		m_fAirPower = 6.f;
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
	case 0: //0이면 직선
		ReadyToDirect();
		break;
	case 1: //1이면 베지어
		ReadyToBezier1();
		break;
	default:
		break;
	}

	return S_OK;
}

void CMonsterMissile::Tick(_float fTimeDelta)
{
	fTimeDelta *= m_fTMRatio;

	TIM->Add_TimeObj(CTime_Manager::OBJ_MONSTER, this);

	Emp_Col();

	Create_PointTrail(fTimeDelta);

	switch (m_iBulletType)
	{
	case 0:
		Direct_Shoot(fTimeDelta);
		break;
	case 1:
		Bezier_Shoot(fTimeDelta);
		break;
	default:
		break;
	}

	DeadTime(fTimeDelta);
}

void CMonsterMissile::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	fTimeDelta *= m_fTMRatio;

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	Choose_Col(nullptr);
	m_PointTrails->LateTick(fTimeDelta);

	Render_Col(m_pRendererCom);
}

HRESULT CMonsterMissile::Render()
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

	m_pShaderCom->Set_RawValue("g_bFixColor", &m_bFixColor, sizeof(_bool));


	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor2", &vColor2, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio1", &vRatio.x, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio2", &vRatio.y, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fBlurPower", &fBlurPower, sizeof(_float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 1)))
			return E_FAIL;
	}

	return S_OK;
}

void CMonsterMissile::Render_Debug()
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

void CMonsterMissile::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		// 내 손 Sphere 둘중 하나와 상대편의 OBB와 충돌 했다면
		if (!strcmp("Monster_Attack", Desc.pMyDesc->sTag) && !strcmp("Player_Body", Desc.pOtherDesc->sTag))
		{
			if (m_bOnecheck)
			{
				((CCreture*)Desc.pOther)->On_Hit(this);
				Set_Dead(); //히트 이펙트
				m_bOnecheck = false;

				GI->PlaySoundW(L"se_EN0005_attack2_3.wav", SD_TRAININGBOT1, 0.6f);
			}

		}
	}
}

void CMonsterMissile::Create_HitEffect(_float4 vWorldPos)
{
	Create_ExplosionEffect();
}

HRESULT CMonsterMissile::Ready_Trails()
{
	CPointTrail::TRAILCREATEINFO Info2;

	Info2.vColor = { 1.f, 0.980f, 0.819f,1.f };
	Info2.vColor2 = { 0.933f,0.647f,0.337f,1.f };
	Info2.bFixColor = true;
	Info2.fBlurPower = 0.7f;
	Info2.iShaderPass = 1;
	Info2.vRatio = { 0.9f, 1.3f };
	Info2.fSize = 0.2f;
	Info2.TexName = L"MissileTrail";
	Info2.iMaxIndex = 29;

	CGameObject*		MissilePoint = GI->Clone_GameObject(TEXT("PointTrailObj"), &Info2);
	if (nullptr == MissilePoint)
		return E_FAIL;

	m_PointTrails = (CPointTrailObj*)MissilePoint;

	return S_OK;
}

void CMonsterMissile::DeadTime(_float fTimeDelta)
{
	m_fDeadAcc += 1.f * fTimeDelta;

	if (m_fDeadAcc > 7.f)
	{
		
		Set_Dead();
		//GI->PlaySoundW(L"se_EN0005_attack2_3.wav", SD_TRAININGBOT1, 0.6f);
	}
}

void CMonsterMissile::ReadyToDirect()
{
	m_fDamage = 40.f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vPosistion));
	m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(180.f));
	m_pTransformCom->Set_Look(XMLoadFloat4(&m_vLook));
	m_eHitType = TYPE_HIT;
	m_fPushPower = 5.f;
	m_fAirPower = 0.f;

}

void CMonsterMissile::ReadyToBezier1()
{

	m_eHitType = TYPE_AWAY;
	m_fPushPower = 2.f;
	m_fAirPower = 4.f;

	m_fDamage = 30.f;

	m_pTransformCom->Set_Look(XMLoadFloat4(&m_vLook));

	_float fX = 0.f;
	_float fY = 0.f;
	switch (m_iMissileIndex)
	{
	case 0:
		ComputeBezier1();
		fY = 0.3f;
		break;
	case 1:
		fY = 0.5f;
		fX = 0.2f;
		break;
	case 2:
		fY = 0.5f;
		fX = -0.2f;
		break;
	default:
		break;
	}
	_float4 vTempPos = { fX, fY, 0.f, 0.f };
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vPosistion) + XMLoadFloat4(&vTempPos));

	//첫번째 인자 m_vPosition
	XMStoreFloat3(&m_vMovingPos[0], m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//두번째 인자 플레이어 pos에서 X값 랜덤으로 더한거
	_float fMoving1X = GI->Get_FloatRandom(1.f, 5.f);
	_float fMoving1Y = GI->Get_FloatRandom(1.f, 6.f);
	//랜덤으로 있어야되는값 x, y, z
	_float4 fTemp2 = { fMoving1X * m_fSide, fMoving1Y, 0.f, 0.f };
	XMStoreFloat3(&m_vMovingPos[1], XMLoadFloat4(&fTemp2) + m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//세번째 인자 m_vPosition에다가 y 값 5보다 크고 10보다 작은숫자 랜덤으로 더한거
	_float fMoving2Z = GI->Get_FloatRandom(-6.f, -2.5f);
	_float4 fTemp3 = { (fMoving1X + 2.f)* m_fSide, fMoving1Y, fMoving2Z, 0.f };
	XMStoreFloat3(&m_vMovingPos[2], XMLoadFloat4(&fTemp3) + m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//네번째 인자 플레이어 pos
	_float fMoving3X = GI->Get_FloatRandom(-0.2f, 0.2f);
	_float fMoving3Y = GI->Get_FloatRandom(0.1f, 1.f);
	_float3 fPlayerTemp = { m_vPlayerPos.x + fMoving3X, m_vPlayerPos.y + fMoving3Y,m_vPlayerPos.z };
	m_vMovingPos[3] = fPlayerTemp;

	m_fSpeed = GI->Get_FloatRandom(14.f, 20.f);
	m_vTargetPos = TM->GetBesierPos(m_vMovingPos[0], m_vMovingPos[1], m_vMovingPos[2], m_vMovingPos[3], m_fBezierTime);


}



void CMonsterMissile::ComputeBezier1()
{
	//카이사 Q
	_float4 vTempPos = { 0.f, 0.3f, 0.f, 0.f };
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vPosistion) + XMLoadFloat4(&vTempPos));

	//첫번째 인자 m_vPosition
	XMStoreFloat3(&m_vMovingPos[0], m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//두번째 인자 플레이어 pos에서 X값 랜덤으로 더한거
	_float4 fTemp2 = { 3.f * m_fSide, 5.f, 0.f, 0.f };
	XMStoreFloat3(&m_vMovingPos[1], XMLoadFloat4(&fTemp2) + XMLoadFloat4(&m_vPosistion));
	//세번째 인자 m_vPosition에다가 y 값 5보다 크고 10보다 작은숫자 랜덤으로 더한거
	_float fTempY = GI->Get_FloatRandom(9.f, 11.f);
	_float4 fTemp3 = { 7.f * m_fSide, 0.f, -4.f, 0.f };
	XMStoreFloat3(&m_vMovingPos[2], XMLoadFloat4(&fTemp3) + XMLoadFloat4(&m_vPosistion));
	//네번째 인자 플레이어 pos
	_float3 fPlayerTemp = { m_vPlayerPos.x, m_vPlayerPos.y ,m_vPlayerPos.z };
	m_vMovingPos[3] = fPlayerTemp;



}

void CMonsterMissile::ComputeBezier2()
{
	//롤러코스터
	_float4 vTempPos = { 0.f, 0.3f, 0.f, 0.f };
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vPosistion) + XMLoadFloat4(&vTempPos));

	//첫번째 인자 m_vPosition에다가 y 0.3f 더한거
	XMStoreFloat3(&m_vMovingPos[0], m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//두번째 인자 플레이어 pos에서 X값 랜덤으로 더한거
	_float4 fTemp2 = { 0.f, 6.f, -15.f, 0.f };
	XMStoreFloat3(&m_vMovingPos[1], XMLoadFloat4(&fTemp2) + XMLoadFloat4(&m_vPlayerPos));
	//세번째 인자 m_vPosition에다가 y 값 5보다 크고 10보다 작은숫자 랜덤으로 더한거
	_float fTempY = GI->Get_FloatRandom(9.f, 11.f);
	_float4 fTemp3 = { 0.f, fTempY, 20.f, 0.f };
	XMStoreFloat3(&m_vMovingPos[2], XMLoadFloat4(&fTemp3) + XMLoadFloat4(&m_vPosistion));
	//네번째 인자 플레이어 pos
	_float3 fPlayerTemp = { m_vPlayerPos.x, m_vPlayerPos.y ,m_vPlayerPos.z };
	m_vMovingPos[3] = fPlayerTemp;

	m_vTargetPos = TM->GetBesierPos(m_vMovingPos[0], m_vMovingPos[1], m_vMovingPos[2], m_vMovingPos[3], m_fBezierTime);

}

void CMonsterMissile::Direct_Shoot(_float fTimeDelta)
{
	m_pTransformCom->Go_Dir(XMLoadFloat4(&m_vLook), 15.f, fTimeDelta);
}

void CMonsterMissile::Bezier_Shoot(_float fTimeDelta)
{
	_vector vTargetPos = XMLoadFloat3(&m_vTargetPos);
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector vTargetLook = vTargetPos - vPosition;
	_float fDist = XMVectorGetX(XMVector3Length(vTargetLook));


	m_pTransformCom->Go_Dir(XMVectorSetW(vTargetLook, 0.f), m_fSpeed, fTimeDelta);

	if (0.6f >= fDist)
	{
		m_fBezierTime += 0.025f;
		m_vTargetPos = CToolManager::Get_Instance()->GetBesierPos(m_vMovingPos[0], m_vMovingPos[1], m_vMovingPos[2], m_vMovingPos[3], m_fBezierTime);
	}

	LookAt(XMLoadFloat3(&m_vTargetPos));

	if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] < PM->Get_PlayerPos().y - 0.2f)
	{
		Create_ExplosionEffect();
		Set_Dead();
	}

}

void CMonsterMissile::LookAt(_fvector vAt)
{
	_vector		vLook = vAt - m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	vLook = XMVectorSetW(vLook, 0.f);
	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	_vector		vUp = XMVector3Cross(vLook, vRight);

	_float3		vScale = m_pTransformCom->Get_Scale();

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CMonsterMissile::Create_PointTrail(_float fTimeDelta)
{
	m_PointTrails->TrailOn();

	_matrix	MissileMatrix = m_pTransformCom->Get_WorldMatrix();
	_float4 MissilePos;

	_vector vTemp = MissileMatrix.r[3];

	_vector vLook = MissileMatrix.r[2];

	vLook = XMVector3Normalize(vLook) * 0.25f;
	vTemp += vLook;

	XMStoreFloat4(&MissilePos, vTemp);
	m_PointTrails->Add_Point(MissilePos);

	m_PointTrails->Tick(fTimeDelta);
}

void CMonsterMissile::Create_ExplosionEffect()
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

	PTM->Create_InstanceParticle(L"Player03Bomb_3", Matrix);
	PTM->Create_InstanceParticle(L"ExplosionEmbers", Matrix);
	PTM->Create_InstanceParticle(L"Player03Bomb_5", Matrix);
	PTM->Create_InstanceParticle(L"Player03Bomb_6", Matrix);
	PTM->Create_InstanceParticle(L"Player03Bomb_7", Matrix);
	PTM->Create_InstanceParticle(L"Player03Bomb_8", Matrix);

}

HRESULT CMonsterMissile::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MonsterMeshEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("MonsterMissile"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.7f, 0.7f, 0.7f);
	ColDesc.iFlag = 0;
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "Monster_Attack");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}





CMonsterMissile * CMonsterMissile::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonsterMissile*		pInstance = new CMonsterMissile(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMonsterMissile"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMonsterMissile::Clone(void * pArg)
{
	CMonsterMissile*		pInstance = new CMonsterMissile(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMonsterMissile"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CMonsterMissile::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_PointTrails);

}
