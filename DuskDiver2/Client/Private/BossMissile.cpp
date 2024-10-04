#include "stdafx.h"
#include "..\Public\BossMissile.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "MonsterBullet.h"
#include "Player_Manager.h"
#include "Time_Manager.h"
#include "ToolManager.h"
#include "PointTrailObj.h"
#include "PointTrail.h"
#include "EffectMissileRange.h"
#include "Player01SmokeBall.h"
#include "Particle_Manager.h"

CBossMissile::CBossMissile(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CBossMissile::CBossMissile(const CBossMissile & rhs)
	: CCreture(rhs)
{
}

HRESULT CBossMissile::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBossMissile::Initialize(void * pArg)
{

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Trails()))
		return E_FAIL;

	m_sTag = "Tag_Monster";

	//pArg로 정보 받아서 직선 불릿또는 포물선 불릿으로 구분해야됨

	if (nullptr != pArg)
	{
		m_MissileInfo = *(CBossMissile::BOSSMISSILEINFO*)pArg;
		m_eHitType = (HITTYPE)m_MissileInfo.iHitType;

		if(m_MissileInfo.iBulletType == 2)
			m_MissileInfo.vColor = { 0.f, 0.f, 0.f, 1.f };
		else
			m_MissileInfo.vColor = { 0.929f, 0.905f, 0.870f, 1.f };
		m_MissileInfo.vColor2 = { 0.929f, 0.f, 0.078f, 1.f };
		m_MissileInfo.vRatio = { 3.f, 7.f };
		m_MissileInfo.fBlurPower = 0.1f;
	
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_vPlayerPos = PM->Get_PlayerPos();

	switch (m_MissileInfo.iBulletType)
	{
	case 0: //0이면 일반 배지어
		ReadyToBezier1();
		break;
	case 1: //1이면 베지어 새로운거
		ReadyToBezier2();
		break;
	case 2: //3이면 베지어 유도탄
		ReadyToGuided();
		break;
	case 3: //패턴게이지용 포물선
		ReadyToParabolic();
		break;
	default:
		break;
	}

	m_fHurtTime = 0.02f;

	return S_OK;
}

void CBossMissile::Tick(_float fTimeDelta)
{
	fTimeDelta *= m_fTMRatio;

	TIM->Add_TimeObj(CTime_Manager::OBJ_MONSTER, this);

	Emp_Col();
	Create_PointTrail(fTimeDelta);

	m_vPlayerPos = PM->Get_PlayerPos();

	m_fDistance = m_pTransformCom->TargetDistance(XMLoadFloat4(&m_vPlayerPos)); //타겟과 나의 거리 계속 계산

	switch (m_MissileInfo.iBulletType)
	{
	case 0:
		Bezier_Shoot(fTimeDelta);
		break;
	case 1:
		Bezier_Shoot(fTimeDelta);
		break;
	case 2:
		Guided_Shoot(fTimeDelta);
		break;
	case 3:
		Parabolic_Shoot(fTimeDelta);
		break;
	default:
		break;
	}


	if(m_MissileInfo.iBulletType != 2)
		DeadTime(fTimeDelta);
}

void CBossMissile::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	fTimeDelta *= m_fTMRatio;

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	if (m_bHurt)
		Choose_Col("Monster_Body1");

	Choose_Col("Monster_Attack");

	m_PointTrails->LateTick(fTimeDelta);


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	Render_Col(m_pRendererCom);
}

HRESULT CBossMissile::Render()
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

	m_pShaderCom->Set_RawValue("g_bFixColor", &m_bFixColor, sizeof(_bool));

	RELEASE_INSTANCE(CGameInstance);


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_MissileInfo.vColor, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor2", &m_MissileInfo.vColor2, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio1", &m_MissileInfo.vRatio.x, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio2", &m_MissileInfo.vRatio.y, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_fBlurPower", &m_MissileInfo.fBlurPower, sizeof(_float))))
			return E_FAIL;


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 1)))
			return E_FAIL;
	}

	return S_OK;
}

void CBossMissile::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	//_float3 vSize = Get_Colliders("Monster_Attack")->Get_Desc()->vSize;
	//_float3 vCenter = Get_Colliders("Monster_Attack")->Get_Desc()->vCenter;

	//ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	//ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	//Get_Colliders("Monster_Attack")->Set_Size(vSize);
	//Get_Colliders("Monster_Attack")->Set_Center(vCenter);
	ImGui::End();
}

void CBossMissile::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		// 내 손 Sphere 둘중 하나와 상대편의 OBB와 충돌 했다면
		if (!strcmp("Monster_Attack", Desc.pMyDesc->sTag) && !strcmp("Player_Body", Desc.pOtherDesc->sTag))
		{
			
			((CCreture*)Desc.pOther)->On_Hit(this);

			if(m_EffectRange != nullptr && m_MissileInfo.iBulletType == 3)
				m_EffectRange->Set_Dead();

			Set_Dead();
			Dead_Sound();
		}
	}
}

void CBossMissile::On_Hit(CCreture * pOther)
{
	if (m_bHurt)
	{
		GI->PlaySoundW(L"SwordHit.ogg", SD_HIT, 0.6f);
		

		if (m_EffectRange != nullptr && m_MissileInfo.iBulletType == 3)
			m_EffectRange->Set_Dead();

		Set_Dead();
		Dead_Sound();
		Create_ExplosionEffect();
	}
}

void CBossMissile::Create_HitEffect(_float4 vWorldPos)
{
	Create_ExplosionEffect();
}

HRESULT CBossMissile::Ready_Trails()
{
	CPointTrail::TRAILCREATEINFO Info2;
	if(m_MissileInfo.iBulletType == 2)
		Info2.vColor = { 0.890f, 0.231f, 0.082f,1.f };
	else
		Info2.vColor = { 0.909f,0.839f, 0.776f,1.f };
	
	Info2.vColor2 = { 0.890f, 0.231f, 0.082f,1.f };
	Info2.bFixColor = true;
	Info2.fBlurPower = 1.2f;
	Info2.iShaderPass = 1;
	Info2.vRatio = { 1.f,1.f };
	Info2.fSize = 0.6f;
	Info2.TexName = L"MissileTrail";
	Info2.iMaxIndex = 32;

	CGameObject*		MissilePoint = GI->Clone_GameObject(TEXT("PointTrailObj"), &Info2);
	if (nullptr == MissilePoint)
		return E_FAIL;

	m_PointTrails = (CPointTrailObj*)MissilePoint;

	return S_OK;
}

void CBossMissile::DeadTime(_float fTimeDelta)
{
	m_fDeadAcc += 1.f * fTimeDelta;

	if (m_fDeadAcc > 8.f)
	{
		if (m_EffectRange != nullptr && m_MissileInfo.iBulletType == 3)
			m_EffectRange->Set_Dead();
		
		Set_Dead();
		Create_ExplosionEffect();
		//Dead_Sound();			
	}
}


void CBossMissile::ReadyToBezier1()
{
	m_eHitType = TYPE_HIT;
	m_fPushPower = 4.f;
	m_fAirPower = 0.f;

	m_fDamage = 80.f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_MissileInfo.vPosition));

	m_pTransformCom->Set_Look(XMLoadFloat4(&m_MissileInfo.vDir));

	//첫번째 인자 m_vPosition
	XMStoreFloat3(&m_vMovingPos[0], m_pTransformCom->Get_State(CTransform::STATE_POSITION));


	//두번째 인자 플레이어 pos에서 X값 랜덤으로 더한거
	_float fMoving1Y = GI->Get_FloatRandom(1.f, 3.f);
	_float fMoving1Z = GI->Get_FloatRandom(-2.f, 2.f);
	//랜덤으로 있어야되는값 x, y, z
	_float4 fTemp2 = { (m_vMovingPos[0].x) *m_MissileInfo.m_fSide, m_vMovingPos[0].y + fMoving1Y,  m_vMovingPos[0].z + fMoving1Z, 0.f };
	XMStoreFloat3(&m_vMovingPos[1], XMLoadFloat4(&fTemp2));

	//세번째 인자 m_vPosition에다가 y 값 5보다 크고 10보다 작은숫자 랜덤으로 더한거
	_float fMoving2X = GI->Get_FloatRandom(2.f, 5.f);
	_float fMoving2Y = GI->Get_FloatRandom(7.f,	12.f);
	_float4 fTemp3 = { (m_vMovingPos[1].x + fMoving2X)* m_MissileInfo.m_fSide,  m_vMovingPos[1].y +fMoving2Y, m_vMovingPos[1].z, 0.f };
	XMStoreFloat3(&m_vMovingPos[2], XMLoadFloat4(&fTemp3));
	//네번째 인자 플레이어 pos
	_float fMoving3X = GI->Get_FloatRandom(-1.f, 1.f);
	_float fMoving3Y = GI->Get_FloatRandom(0.9f, 2.f);
	_float fMoving3Z = GI->Get_FloatRandom(-0.3f, 0.3f);

	_float3 fPlayerTemp = { m_vPlayerPos.x + fMoving3X, m_vPlayerPos.y + fMoving3Y,m_vPlayerPos.z + fMoving3Z};
	m_vMovingPos[3] = fPlayerTemp;

	m_fMagicNum2 = fMoving3X;

	m_fSpeed = GI->Get_FloatRandom(14.f, 17.f);
	m_vTargetPos = TM->GetBesierPos(m_vMovingPos[0], m_vMovingPos[1], m_vMovingPos[2], m_vMovingPos[3], m_fBezierTime);
}

void CBossMissile::ReadyToBezier2()
{
	m_eHitType = TYPE_HIT;
	m_fPushPower = 5.f;
	m_fAirPower = 0.f;

	m_fDamage = 80.f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_MissileInfo.vPosition));

	m_pTransformCom->Set_Look(XMLoadFloat4(&m_MissileInfo.vDir));

	XMStoreFloat3(&m_vMovingPos[0], m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	_float fMoving1X = GI->Get_FloatRandom(1.f, 5.f);
	_float fMoving1Z = GI->Get_FloatRandom(-2.f, 2.f);
	_float fMoving1Y = GI->Get_FloatRandom(2.f, 5.f);

	_float4 fTemp2 = { m_vMovingPos[0].x +fMoving1X, m_vMovingPos[0].y + fMoving1Y,  m_vMovingPos[0].z + fMoving1Z, 0.f };
	XMStoreFloat3(&m_vMovingPos[1], XMLoadFloat4(&fTemp2));

	_float fMoving2X = GI->Get_FloatRandom(3.f, 7.f);
	_float fMoving2Y = GI->Get_FloatRandom(7.f, 10.5f);
	_float fMoving2Z = GI->Get_FloatRandom(-4.f, 4.f);

	_float4 fTemp3 = { (m_vMovingPos[1].x + fMoving2X) * m_MissileInfo.m_fSide , m_vMovingPos[1].y + fMoving2Y,   m_vMovingPos[1].z + fMoving2Z, 0.f };
	XMStoreFloat3(&m_vMovingPos[2], XMLoadFloat4(&fTemp3));


	_float fMoving3X = GI->Get_FloatRandom(-0.4f, 0.4f);
	_float fMoving3Y = GI->Get_FloatRandom(0.9f, 1.3f);
	m_fMagicNum = fMoving3Y;
	_float3 fPlayerTemp = { m_vPlayerPos.x + fMoving3X, m_vPlayerPos.y + fMoving3Y,m_vPlayerPos.z };
	m_vMovingPos[3] = fPlayerTemp;

	m_fMagicNum2 = fMoving3X;

	m_fSpeed = GI->Get_FloatRandom(12.f, 15.f);
	m_vTargetPos = TM->GetBesierPos(m_vMovingPos[0], m_vMovingPos[1], m_vMovingPos[2], m_vMovingPos[3], m_fBezierTime);

	
}

void CBossMissile::ReadyToGuided()
{

	m_eHitType = TYPE_SPIN;
	m_fPushPower = 2.f;
	m_fAirPower = 1.f;

	m_fDamage = 80.f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_MissileInfo.vPosition));

	m_pTransformCom->Set_Look(XMLoadFloat4(&m_MissileInfo.vDir));

	//첫번째 인자 m_vPosition
	XMStoreFloat3(&m_vMovingPos[0], m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	_float fMoving1X = GI->Get_FloatRandom(-2.f, 2.f);
	_float fMoving1Z = GI->Get_FloatRandom(-2.f, 2.f);
	_float fMoving1Y = GI->Get_FloatRandom(1.f, 3.f);

	_float4 fTemp2 = { m_vMovingPos[0].x + fMoving1X, m_vMovingPos[0].y + fMoving1Y,  m_vMovingPos[0].z + fMoving1Z, 0.f };
	XMStoreFloat3(&m_vMovingPos[1], XMLoadFloat4(&fTemp2));

	_float fMoving2X = GI->Get_FloatRandom(5.f, 10.f);
	_float fMoving2Y = GI->Get_FloatRandom(8.f, 12.5f);
	_float fMoving2Z = GI->Get_FloatRandom(-7.f, 6.f);

	_float4 fTemp3 = { (m_vMovingPos[1].x + fMoving2X) * m_MissileInfo.m_fSide , m_vMovingPos[1].y + fMoving2Y,   m_vMovingPos[1].z + fMoving2Z, 0.f };
	XMStoreFloat3(&m_vMovingPos[2], XMLoadFloat4(&fTemp3));


	_float fMoving3X = GI->Get_FloatRandom(-0.4f, 0.4f);
	_float fMoving3Y = GI->Get_FloatRandom(0.9f, 1.3f);
	_float3 fPlayerTemp = { m_vPlayerPos.x + fMoving3X, m_vPlayerPos.y + fMoving3Y,m_vPlayerPos.z };
	m_vMovingPos[3] = fPlayerTemp;
	m_fMagicNum = fMoving3Y;

	m_fSpeed = 9.f;
	m_vTargetPos = TM->GetBesierPos(m_vMovingPos[0], m_vMovingPos[1], m_vMovingPos[2], m_vMovingPos[3], m_fBezierTime);

}

void CBossMissile::ReadyToParabolic()
{
	//이펙트 생성
	Create_Effect();

	m_eHitType = TYPE_SPIN;

	m_fPushPower = 2.f;
	m_fAirPower = 3.f;

	m_fDamage = 90.f;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_MissileInfo.vPosition));

	m_pTransformCom->Set_Look(XMLoadFloat4(&m_MissileInfo.vDir));

	//포물선 준비

	_vector vPosition = XMLoadFloat4(&m_MissileInfo.vPosition);

	XMStoreFloat3(&m_vStartPos, vPosition); //시작 지점
	XMStoreFloat3(&m_vPos, vPosition); //현재지점
	
	//도착지점 플레이어 근처 랜덤으로 
	_float fMoving3X = GI->Get_FloatRandom(-15.f, 15.f);
	_float fMoving3Z = GI->Get_FloatRandom(-15.f, 15.f);
	_float3 EndTemp = { m_vPlayerPos.x + fMoving3X, 15.531f ,m_vPlayerPos.z + fMoving3Z };
	m_vEndPos = EndTemp;
	_float4 CreatePos = { m_vEndPos.x, m_vEndPos.y, m_vEndPos.z, 1.f };
	static_cast<CEffectMissileRange*>(m_EffectRange)->SetUp_Position(CreatePos);

	_float fRandom = GI->Get_FloatRandom(2.f, 10.f);
	m_fMaxHeight = m_vPos.y + fRandom;


}

void CBossMissile::Create_Effect()
{

	CEffectMissileRange::EFFECTRANGEINFO* Info = new CEffectMissileRange::EFFECTRANGEINFO;
	Info->vColor = { 1.f,0.843f,0.643f,1.f };
	Info->vColor2 = { 1.f,0.843f,0.643f,1.f };
	Info->vRatio = { 2.f, 0.5f };
	Info->vRatioSpeed = { 0.f,0.f };

	_matrix Effectmatrix;

	XMStoreFloat4x4(&Info->WorldMatrix, m_pTransformCom->Get_WorldMatrix());
	Info->fBlurPower = 1.f;

	(GI->Add_Get_GameObjectToLayer(TEXT("EffectMissileRange"), LEVEL_TEST, TEXT("Layer_Monster"), &Info, &m_EffectRange));

	Safe_AddRef(m_EffectRange);

}



void CBossMissile::ComputeBezier1()
{
	//카이사 Q
	_float4 vTempPos = { 0.f, 0.3f, 0.f, 0.f };
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_MissileInfo.vPosition) + XMLoadFloat4(&vTempPos));

	//첫번째 인자 m_vPosition
	XMStoreFloat3(&m_vMovingPos[0], m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//두번째 인자 플레이어 pos에서 X값 랜덤으로 더한거
	_float4 fTemp2 = { 3.f * m_MissileInfo.m_fSide, 5.f, 0.f, 0.f };
	XMStoreFloat3(&m_vMovingPos[1], XMLoadFloat4(&fTemp2) + XMLoadFloat4(&m_MissileInfo.vPosition));
	//세번째 인자 m_vPosition에다가 y 값 5보다 크고 10보다 작은숫자 랜덤으로 더한거
	_float fTempY = GI->Get_FloatRandom(9.f, 11.f);
	_float4 fTemp3 = { 7.f * m_MissileInfo.m_fSide, 0.f, -4.f, 0.f };
	XMStoreFloat3(&m_vMovingPos[2], XMLoadFloat4(&fTemp3) + XMLoadFloat4(&m_MissileInfo.vPosition));
	//네번째 인자 플레이어 pos
	_float3 fPlayerTemp = { m_vPlayerPos.x, m_vPlayerPos.y ,m_vPlayerPos.z };
	m_vMovingPos[3] = fPlayerTemp;



}

void CBossMissile::ComputeBezier2()
{
	//롤러코스터
	_float4 vTempPos = { 0.f, 0.3f, 0.f, 0.f };
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_MissileInfo.vPosition) + XMLoadFloat4(&vTempPos));

	//첫번째 인자 m_vPosition에다가 y 0.3f 더한거
	XMStoreFloat3(&m_vMovingPos[0], m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//두번째 인자 플레이어 pos에서 X값 랜덤으로 더한거
	_float4 fTemp2 = { 0.f, 6.f, -15.f, 0.f };
	XMStoreFloat3(&m_vMovingPos[1], XMLoadFloat4(&fTemp2) + XMLoadFloat4(&m_vPlayerPos));
	//세번째 인자 m_vPosition에다가 y 값 5보다 크고 10보다 작은숫자 랜덤으로 더한거
	_float fTempY = GI->Get_FloatRandom(9.f, 11.f);
	_float4 fTemp3 = { 0.f, fTempY, 20.f, 0.f };
	XMStoreFloat3(&m_vMovingPos[2], XMLoadFloat4(&fTemp3) + XMLoadFloat4(&m_MissileInfo.vPosition));
	//네번째 인자 플레이어 pos
	_float3 fPlayerTemp = { m_vPlayerPos.x, m_vPlayerPos.y ,m_vPlayerPos.z };
	m_vMovingPos[3] = fPlayerTemp;

	m_vTargetPos = TM->GetBesierPos(m_vMovingPos[0], m_vMovingPos[1], m_vMovingPos[2], m_vMovingPos[3], m_fBezierTime);

}

void CBossMissile::Dead_Sound()
{
	_uint iRandomMissile = GI->Get_Random(0, 3);
	switch (iRandomMissile)
	{
	case 0:
		GI->PlaySoundW(L"se_EN0103_missle_3-01.wav", SD_BULLET, 0.2f);
		break;
	case 1:
		GI->PlaySoundW(L"se_EN0103_missle_3-02.wav", SD_BULLET, 0.2f);
		break;
	case 2:
		GI->PlaySoundW(L"se_EN0103_missle_3-03.wav", SD_BULLET, 0.2f);
		break;
	case 3:
		GI->PlaySoundW(L"se_EN0103_missle_3-04.wav", SD_BULLET, 0.2f);
		break;
	}
}


void CBossMissile::Parabolic_Shoot(_float fTimeDelta)
{

	m_fEndHeight = m_vEndPos.y - m_vStartPos.y;

	m_fHeight = m_fMaxHeight - m_vStartPos.y;

	m_fg = 2.f * m_fHeight / (m_fMaxTime * m_fMaxTime);

	m_fSpeedY = sqrtf(2 * m_fg * m_fHeight);

	_float a = m_fg;
	_float b = -2.f * m_fSpeedY;
	_float c = 2.f * m_fEndHeight;

	m_fEndTime = (-b + sqrtf(b * b - 4 * a * c)) / (2 * a);

	m_fSpeedX = -(m_vStartPos.x - m_vEndPos.x) / m_fEndTime;
	m_fSpeedZ = -(m_vStartPos.z - m_vEndPos.z) / m_fEndTime;

	m_fTime += 1.f * fTimeDelta;

	m_vPos.x = m_vStartPos.x + m_fSpeedX * m_fTime;
	m_vPos.y = m_vStartPos.y + (m_fSpeedY * m_fTime) - (0.5f * m_fg * m_fTime * m_fTime);
	m_vPos.z = m_vStartPos.z + m_fSpeedZ * m_fTime;
	m_pTransformCom->LookAt(XMLoadFloat3(&m_vPos));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_vPos.x, m_vPos.y, m_vPos.z, 1.f));

	if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] < 15.53f)
	{
		Set_Dead();
		Dead_Sound();
		Create_ExplosionEffect();

		if (m_EffectRange != nullptr)
			m_EffectRange->Set_Dead();
	}

}

void CBossMissile::Bezier_Shoot(_float fTimeDelta)
{

	_vector vTargetPos = XMLoadFloat3(&m_vTargetPos);
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector vTargetLook = vTargetPos - vPosition;
	_float fDist = XMVectorGetX(XMVector3Length(vTargetLook));


	m_pTransformCom->Go_Dir(XMVectorSetW(vTargetLook, 0.f), m_fSpeed, fTimeDelta);

	if (0.6f >= fDist)
	{
		m_fBezierTime += 0.025f;

		if(m_MissileInfo.iBulletType == 1)
			m_vMovingPos[3] = { m_vPlayerPos.x + m_fMagicNum2, m_vPlayerPos.y + m_fMagicNum,m_vPlayerPos.z };

		m_vTargetPos = CToolManager::Get_Instance()->GetBesierPos(m_vMovingPos[0], m_vMovingPos[1], m_vMovingPos[2], m_vMovingPos[3], m_fBezierTime);
	}

	LookAtSmooth(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&m_vTargetPos), 0.1f);

	if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] < 15.53f || m_fDistance < 1.5f)
	{
		Set_Dead();
		Dead_Sound();
		Create_ExplosionEffect();
	}

}


void CBossMissile::Guided_Shoot(_float fTimeDelta)
{
	_vector vTargetPos = XMLoadFloat3(&m_vTargetPos);
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_vector vTargetLook = vTargetPos - vPosition;
	_float fDist = XMVectorGetX(XMVector3Length(vTargetLook));

	if (m_fBezierTime >= 1.f)
	{
		Change_Way(fTimeDelta);
		m_vMovingPos[3] = { m_vPlayerPos.x + m_fMagicNumX, 16.031f + m_fMagicNumY,m_vPlayerPos.z };
		_vector vLook = XMVector3Normalize(XMLoadFloat3(&m_vMovingPos[3]) - m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		XMStoreFloat3(&m_vTargetLook, vLook);
		m_vTargetPos = m_vMovingPos[3];

		m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), m_fSpeed, fTimeDelta);

	}
	else
	{
		m_pTransformCom->Go_Dir(XMVectorSetW(vTargetLook, 0.f), m_fSpeed, fTimeDelta);	

		if (0.6f >= fDist)
		{
			m_fBezierTime += 0.025f;
			m_vMovingPos[3] = { m_vPlayerPos.x + m_fMagicNum2, m_vPlayerPos.y + m_fMagicNum,m_vPlayerPos.z };

			m_vTargetPos = CToolManager::Get_Instance()->GetBesierPos(m_vMovingPos[0], m_vMovingPos[1], m_vMovingPos[2], m_vMovingPos[3], m_fBezierTime);
		}

	}
	if (m_fDistance <= 1.5f)
	{
		Set_Dead();
		Dead_Sound();
		Create_ExplosionEffect();
	}

	LookAtSmooth(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&m_vTargetPos), 0.1f);
}

void CBossMissile::LookAt(_fvector vAt)
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

void CBossMissile::LookAtSmooth(_fvector StartLook, _fvector TargetPos, _float _fRatio)
{

	_vector		vTargetLook = XMVector3Normalize(TargetPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION)); //타겟 룩

	_vector vLook;

	vLook = (XMVector3Normalize(StartLook) * (1.f - _fRatio)) + (XMVector3Normalize(vTargetLook)*_fRatio);

	vLook = XMVectorSetW(vLook, 0.f);

	_vector		vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);

	_vector		vUp = XMVector3Cross(vLook, vRight);

	_float3		vScale = m_pTransformCom->Get_Scale();

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

void CBossMissile::Change_Way(_float fTimeDelta)
{
	if (m_bChange)
	{
		m_fMagicNumY += 1.4f * fTimeDelta;
		m_fMagicNumX += 1.2f * fTimeDelta;

	}
	else
	{
		m_fMagicNumY -= 1.4f * fTimeDelta;
		m_fMagicNumX -= 1.2f * fTimeDelta;
	}

	if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] >= 16.631f)
	{
		m_bChange = false;
	}
	if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] <= 16.031f)
	{
		m_bChange = true;
	}
}

void CBossMissile::Change_AttackColRange(_float3 vSize, _float3 vCenter)
{
	Get_Colliders("Monster_Attack")->Set_Size(vSize);
	Get_Colliders("Monster_Attack")->Set_Center(vCenter);
}

void CBossMissile::Create_PointTrail(_float fTimeDelta)
{
	
	m_PointTrails->TrailOn();

	_matrix	MissileMatrix =  m_pTransformCom->Get_WorldMatrix();
	_float4 MissilePos;

	_vector vTemp = MissileMatrix.r[3];

	_vector vLook = MissileMatrix.r[2];

	vLook = XMVector3Normalize(vLook) * 0.25f;
	vTemp += vLook; 

	XMStoreFloat4(&MissilePos, vTemp);
	m_PointTrails->Add_Point(MissilePos);

	m_PointTrails->Tick(fTimeDelta);

}

void CBossMissile::Create_ExplosionEffect()
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


HRESULT CBossMissile::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MonsterMeshEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("BossMissile"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.7f, 0.1f, 0.1f);
	ColDesc.iFlag = 2;
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "Monster_Attack");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	if (m_MissileInfo.iBulletType == 2)
	{
		ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
		ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
		ColDesc.vSize = _float3(1.f, 0.1f, 0.1f);
		ColDesc.iFlag = 2;
		ColDesc.pOwner = this;
		strcpy(ColDesc.sTag, "Monster_Body1");
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;

	}


	return S_OK;
}





CBossMissile * CBossMissile::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBossMissile*		pInstance = new CBossMissile(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBossMissile"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBossMissile::Clone(void * pArg)
{
	CBossMissile*		pInstance = new CBossMissile(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBossMissile"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CBossMissile::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_PointTrails);
	Safe_Release(m_EffectRange);
}
