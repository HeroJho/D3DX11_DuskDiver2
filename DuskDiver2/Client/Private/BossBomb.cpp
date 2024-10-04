#include "stdafx.h"
#include "..\Public\BossBomb.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "MonsterBullet.h"
#include "Player_Manager.h"
#include "Time_Manager.h"
#include "PointTrail.h"
#include "PointTrailObj.h"
#include "SwordTrail.h"
#include "SwordTrailObj.h"
#include "Particle_Manager.h"
#include "Player01SmokeBall.h"


CBossBomb::CBossBomb(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CBossBomb::CBossBomb(const CBossBomb & rhs)
	: CCreture(rhs)
{
}

HRESULT CBossBomb::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBossBomb::Initialize(void * pArg)
{

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Trails()))
		return E_FAIL;

	if (nullptr != pArg)
	{
		m_BombInfo = *(CBossBomb::BOMBINFO*)pArg;
		_vector vPosition = XMLoadFloat4(&m_BombInfo.vPosition);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition); //처음 생성위치
	}

	if (FAILED(Create_Navigation(m_BombInfo.iNaviNum)))
		return E_FAIL;

	m_sTag = "Tag_Monster";

	//pArg로 정보 받아서 직선 불릿또는 포물선 불릿으로 구분해야됨

	//불릿 타입 총 세가지?
	//attack type으로 나누기 
	//1일때는 한자리에서 나오는데 내가 바라보는 룩으로 go_dir 하고 점프하기 

	//go_dir하고 점프하는거는 다 똑같음  가야되는 방향만 다를뿐
	//hit type도 똑같이 해야할지 고민  


	if (FAILED(Ready_Components()))
		return E_FAIL;


	if (m_BombInfo.iAttackType == 2)
	{
		m_eHitType = TYPE_HIT;
		m_fDamage = 80.f;
		m_fAirPower = 0.f;
		m_fPushPower = 7.f;
	}
	else if (m_BombInfo.iAttackType == 3)
	{
		m_eHitType = TYPE_AWAY;
		m_fDamage = 80.f;
		m_fAirPower = 1.f;
		m_fPushPower = 3.f;
	}
	else if (m_BombInfo.iAttackType == 4)
	{
		m_eHitType = TYPE_HIT;
		m_fDamage = 80.f;
		m_fAirPower = 0.f;
		m_fPushPower = 5.f;
	}

	m_pTransformCom->Set_WorldMatrix(m_BombInfo.vWorldMatrix);

	m_pTransformCom->ResetAndJump(m_BombInfo.fJumpPower);

	_matrix	BulletMatrix = m_pTransformCom->Get_WorldMatrix();

	m_SwordTrails->TrailOn(BulletMatrix);

	GI->PlaySoundW(L"se_EN0103_flare_1.wav", SD_TRAININGBOT2, 0.3f);

	return S_OK;
}

void CBossBomb::Tick(_float fTimeDelta)
{

	TIM->Add_TimeObj(CTime_Manager::OBJ_MONSTER, this);

	fTimeDelta *= m_fTMRatio;
	
	Emp_Col();


	Create_PointTrail(fTimeDelta);
	//Create_SwordTrail(fTimeDelta);

	//DeadTime(fTimeDelta);
}

void CBossBomb::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	fTimeDelta *= m_fTMRatio;

	m_pTransformCom->Tick_Gravity(fTimeDelta, nullptr, m_BombInfo.fGravity);

	if(m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] > 15.531f)
		m_pTransformCom->Go_Dir(XMLoadFloat3((_float3*)&m_BombInfo.vWorldMatrix.m[2]), m_BombInfo.fSpeed, fTimeDelta);
	else
	{
		Set_Dead();
		Create_ExplosionFloorEffect();
		GI->PlaySoundW(L"se_EN0103_flare_2.wav", SD_TRAININGBOT2, 0.3f);
	}


	if (m_BombInfo.iAttackType == 2)
	{
		_float4 vPos = { 0.f, 0.f, 0.f, 1.f };
			
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			
		if (vPos.y <= 17.231f)
		{
			if (m_bOne)
			{
				m_bOne = false;
				m_bOneCheck = true;
			}
		}
			if (m_bOneCheck)
			{
				m_bOneCheck = false;
			
				Create_OtherBomb();
				Create_ExplosionAirEffect();
				Set_Dead();

				GI->PlaySoundW(L"se_EN0103_flare_2.wav", SD_TRAININGBOT2, 0.3f);
			}
		
	}

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	m_PointTrails->LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	Choose_Col(nullptr);
	Render_Col(m_pRendererCom);
}

HRESULT CBossBomb::Render()
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


	m_pShaderCom->Set_RawValue("g_bFixColor", &m_bFix, sizeof(_bool));


	RELEASE_INSTANCE(CGameInstance);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_BombInfo.vColor, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor2", &m_BombInfo.vColor2, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio1", &m_BombInfo.vRatio.x, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio2", &m_BombInfo.vRatio.y, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_fBlurPower", &m_BombInfo.fBlurPower, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 1)))
			return E_FAIL;
	}

	return S_OK;
}

void CBossBomb::Render_Debug()
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

void CBossBomb::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		// 내 손 Sphere 둘중 하나와 상대편의 OBB와 충돌 했다면
		if (!strcmp("Monster_Attack", Desc.pMyDesc->sTag) && !strcmp("Player_Body", Desc.pOtherDesc->sTag))
		{
			if (m_bOneHit)
			{
				((CCreture*)Desc.pOther)->On_Hit(this);
				Set_Dead();

				//피격이펙트?
				m_bOneHit = false;
				//m_SwordTrails->TrailOff();
				GI->PlaySoundW(L"se_EN0103_flare_2.wav", SD_TRAININGBOT2, 0.3f);
			}
		}
	}
}

void CBossBomb::Create_HitEffect(_float4 vWorldPos)
{
	Create_ExplosionAirEffect();
}

void CBossBomb::DeadTime(_float fTimeDelta)
{

}

HRESULT CBossBomb::Ready_Trails()
{
	CPointTrail::TRAILCREATEINFO Info2;

	Info2.vColor = { 0.909f,0.839f, 0.776f,1.f };
	Info2.vColor2 = { 0.933f,0.063f,0.090f, 1.f };
	Info2.bFixColor = true;
	Info2.fBlurPower = 0.7f;
	Info2.iShaderPass = 1;
	Info2.vRatio = { 1.f, 1.f };
	Info2.fSize = 0.7f;
	Info2.TexName = L"MissileTrail2";
	Info2.iMaxIndex = 5;

	CGameObject*      MissilePoint = GI->Clone_GameObject(TEXT("PointTrailObj"), &Info2);
	if (nullptr == MissilePoint)
		return E_FAIL;

	m_PointTrails = (CPointTrailObj*)MissilePoint;


	CSwordTrail::TRAILINFO Info;
	Info.HighAndLow.vLow = { 0.f,0.f,0.f };
	Info.HighAndLow.vHigh = { 0.f,0.4f,0.4f };
	Info.TexName = L"MissileTrail2";
	Info.vColor1 = { 1.f, 0.980f, 0.819f,1.f };
	Info.vColor2 = { 0.933f,0.647f,0.337f,1.f };
	Info.vRatio = { 1.f,1.f };
	Info.bFixColor = true;
	Info.iShaderPass = 1;
	Info.fBlurPower = 1.5f;
	CGameObject*		SwordTrail1 = GI->Clone_GameObject(TEXT("SwordTrailObj"), &Info);
	if (nullptr == SwordTrail1)
		return E_FAIL;

	m_SwordTrails = (CSwordTrailObj*)SwordTrail1;
	return S_OK;
}

void CBossBomb::Create_PointTrail(_float fTimeDelta)
{

	m_PointTrails->TrailOn();

	_matrix	BulletMatrix = m_pTransformCom->Get_WorldMatrix();

	_float4 BulletPos;

	XMStoreFloat4(&BulletPos, BulletMatrix.r[3]);

	m_PointTrails->Add_Point(BulletPos);

	m_PointTrails->Tick(fTimeDelta);

}

void CBossBomb::Create_SwordTrail(_float fTimeDelta)
{

	_matrix	BulletMatrix = m_pTransformCom->Get_WorldMatrix();

	if (m_SwordTrails->Get_TrailOn())
		m_SwordTrails->Tick(fTimeDelta, BulletMatrix);


}

void CBossBomb::Create_ExplosionFloorEffect()
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

void CBossBomb::Create_ExplosionAirEffect()
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

	PTM->Create_InstanceParticle(L"Player03Bomb_2", Matrix);
	PTM->Create_InstanceParticle(L"Player03Bomb_3", Matrix);
	PTM->Create_InstanceParticle(L"Player03Bomb_5", Matrix);
	PTM->Create_InstanceParticle(L"Player03Bomb_6", Matrix);
	PTM->Create_InstanceParticle(L"Player03Bomb_7", Matrix);
	PTM->Create_InstanceParticle(L"Player03Bomb_8", Matrix);
}

void CBossBomb::Create_OtherBomb()
{
	CBossBomb::BOMBINFO BombInfo;
	BombInfo.fJumpPower = 4.f;
	BombInfo.iAttackType = 4;
	BombInfo.fSpeed = 1.5f;
	BombInfo.fGravity = 60.f;
	BombInfo.vColor = { 0.933f,0.063f,0.090f, 1.f };
	BombInfo.vColor2 = { 0.f, 0.f, 0.f, 1.f };
	BombInfo.vRatio = { 1.f, 1.f };
	BombInfo.fBlurPower = 1.5f;
	BombInfo.iNaviNum = m_pNavigationCom->Get_CurCellIndex();
	_matrix Bombmatrix;
	Bombmatrix.r[0] = { 1.3f, 0.f, 0.f, 0.f };
	Bombmatrix.r[1] = { 0.f, 1.3f, 0.f, 0.f };
	Bombmatrix.r[2] = { 0.f, 0.f, 1.3f, 0.f };
	Bombmatrix.r[3] = { 0.f, 0.f, 0.f, 1.f };
	XMStoreFloat4x4(&BombInfo.vWorldMatrix, Bombmatrix * m_pTransformCom->Get_WorldMatrix());
	(GI->Add_GameObjectToLayer(TEXT("BossBomb"), GI->Get_Cur_LevelIndex(), TEXT("Layer_Monster"), &BombInfo));

	BombInfo.fJumpPower = 5.f;
	Bombmatrix.r[0] = { 1.3f, 0.f, 0.f, 0.f };
	Bombmatrix.r[1] = { 0.f,  0.f, 1.3f,0.f };
	Bombmatrix.r[2] = { 0.f, -1.3f, 0.f, 0.f };
	Bombmatrix.r[3] = { 0.f, 0.f, 0.f, 1.f };
	XMStoreFloat4x4(&BombInfo.vWorldMatrix, Bombmatrix * m_pTransformCom->Get_WorldMatrix());
	(GI->Add_GameObjectToLayer(TEXT("BossBomb"), GI->Get_Cur_LevelIndex(), TEXT("Layer_Monster"), &BombInfo));
	BombInfo.fJumpPower = 6.5f;
	Bombmatrix.r[0] = { 1.3f, 0.f, 0.f, 0.f };
	Bombmatrix.r[1] = { 0.f, -1.3f, 0.f, 0.f };
	Bombmatrix.r[2] = { 0.f, 0.f, -1.3f, 0.f };
	Bombmatrix.r[3] = { 0.f, 0.f, 0.f, 1.f };
	XMStoreFloat4x4(&BombInfo.vWorldMatrix, Bombmatrix * m_pTransformCom->Get_WorldMatrix());
	(GI->Add_GameObjectToLayer(TEXT("BossBomb"), GI->Get_Cur_LevelIndex(), TEXT("Layer_Monster"), &BombInfo));

	Bombmatrix.r[0] = { 1.3f, 0.f, 0.f, 0.f };
	Bombmatrix.r[1] = { 0.f,  0.f, -1.3f,0.f };
	Bombmatrix.r[2] = { 0.f, 1.3f, 0.f, 0.f };
	Bombmatrix.r[3] = { 0.f, 0.f, 0.f, 1.f };
	XMStoreFloat4x4(&BombInfo.vWorldMatrix, Bombmatrix * m_pTransformCom->Get_WorldMatrix());
	(GI->Add_GameObjectToLayer(TEXT("BossBomb"), GI->Get_Cur_LevelIndex(), TEXT("Layer_Monster"), &BombInfo));
}

HRESULT CBossBomb::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MonsterMeshEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("BossBomb"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.7f, 0.7f, 0.7f);
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "Monster_Attack");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CBossBomb::Create_Navigation(_uint iNaviNum)
{
	CNavigation::NAVIGATIONDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = 0;


	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage4Navigation"), TEXT("Stage4"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4((_float4*)m_BombInfo.vWorldMatrix.m[3]));
	_uint iNaviIndex = m_pNavigationCom->Find_NaviIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pNavigationCom->Set_NaviIndex(iNaviIndex);

	return S_OK;
}





CBossBomb * CBossBomb::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBossBomb*		pInstance = new CBossBomb(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBossBomb"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBossBomb::Clone(void * pArg)
{
	CBossBomb*		pInstance = new CBossBomb(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBossBomb"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CBossBomb::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pNavigationCom);
	Safe_Release(m_PointTrails);
	Safe_Release(m_SwordTrails);

}
