#include "stdafx.h"
#include "..\Public\Player03.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Player_Manager.h"
#include "DataManager.h"
#include "ToolManager.h"
#include "Parts.h"
#include "Status_Manager.h"
#include "Particle_Manager.h"
#include "Time_Manager.h"
#include "Player03HairA.h"
#include "UI_Manager.h"
#include "Camera_Manager.h"
#include "Player01HitEffect_1.h"
#include "Player01HitEffect_2.h"
#include "Wall.h"
#include "Player03SniperEffect_1.h"
#include "Wind.h"
#include "EffectSpin.h"
#include "Player03MotionTrail.h"
#include "Player03HairB.h"
#include "SwordTrail.h"
#include "PointTrail.h"
#include "PointTrailObj.h"
#include "ViewAlpha.h"
#include "SwordTrailObj.h"
#include "Player03ShotGun.h"
#include "Player01Wind1.h"
#include "Player01Wind2.h"
#include "Player01Ball.h"
#include "Player01Ring.h"
#include "Player01Pillar.h"
#include "Player01Pillar2.h"
#include "Player01Pillar3.h"
#include "Player01Skill5Ball.h"
#include "CircleDistortion.h"
#include "Player01Rage1Ball.h"
#include "Player01SmokeBall.h"
#include "Player01Metamor.h"
#include "Player01Landing.h"
#include "Player01Upper.h"
#include "EffectSpin.h"
#include "EffectAtom.h"
#include "Ring.h"
#include "Tornado.h"
#include "ViewAlpha.h"
#include "Monster_Manager.h"
#include "Player03CannonBullet.h"
#include "Player03Bomb.h"
#include "Player04VoidParticle.h"

CPlayer03::CPlayer03(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CPlayer03::CPlayer03(const CPlayer03 & rhs)
	: CCreture(rhs)
{
}

HRESULT CPlayer03::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer03::Initialize(void * pArg)
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Player";
	// *[HeroJho]: ImGui로 디버깅을 하려면 꼭 호출해줘야 합니다.
	Make_ImGui();

	m_pModelCom->Set_AnimIndex(IDLE1);
	m_eCurState = IDLE1;
	m_eNextState = IDLE1;
	m_pModelCom->SetNextIndex(IDLE1);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 25.f, 1.f));

	// *[HeroJho]: 현재 내가 서있는 네비 인덱스를 자동으로 구해줍니다. 비용이 비싸기 때문에 Tick과 같은 반복 호출 금지입니다!
	_int iNaviIndex = m_pNavigationCom->Find_NaviIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pNavigationCom->Set_NaviIndex(iNaviIndex);

	Ready_Sockets();
	Ready_PlayerParts();

	Ready_Trails();

	m_fHurtTime = 0.2f;

	PM->Add_Player(this);
	STM->Set_FinalHp(CStatus_Manager::PLAYER03);
	STM->Set_FinalStr(CStatus_Manager::PLAYER03);
	m_fMaxHp = STM->Get_FinalHp(CStatus_Manager::PLAYER03);
	m_fNowHp = m_fMaxHp;
	m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER03);
	
	((CPlayer03HairA*)m_Parts[PARTS_HAIRA])->Set_Sleep();
	Get_Colliders("Player_HairCut")->Set_Sleep();

	((CPlayer03HairA*)m_Parts[PARTS_HAIRA])->Set_Sleep();
	Get_Colliders("Player_HairCut")->Set_Sleep();

	m_fPreHp = m_fNowHp;
	m_fMaxSp = 100.f;
	m_fNowSP = m_fMaxSp;
	m_fPreSp = m_fNowSP;
	m_fMaxEp = 100.f;
	m_fNowEP = 100.f;
	m_fPreEp = m_fNowEP;

	return S_OK;
}

void CPlayer03::Tick(_float fTimeDelta)
{
	if (m_bSelectPlayer && PM->Get_PlayerTick())
	{
		if (CK_DOWN(DIK_F7))
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vector{ 0.f,0.f,-10.f,1.f });
			Update_Parts();
			m_Parts[PARTS_HAIRA]->LateTick(fTimeDelta);
			((CPlayer03HairA*)m_Parts[PARTS_HAIRA])->SetInitPos();
		}


		TIM->Add_TimeObj(CTime_Manager::OBJ_PLAYER, this);

		Check_SP(fTimeDelta);

		Check_Hurt(fTimeDelta);

		Check_Rim(fTimeDelta);

		Check_Burst(fTimeDelta * m_fTMRatio, fTimeDelta);

		fTimeDelta *= m_fTMRatio;

		Emp_Col();

		Get_KeyInput(fTimeDelta);

		Update(fTimeDelta);

		Check_HP(fTimeDelta);

		Check_PlayerLight(fTimeDelta);

		Create_MotionTrail(fTimeDelta);
		Create_PointTrail(fTimeDelta);
				
	}

}

void CPlayer03::LateTick(_float fTimeDelta)
{
	if (m_bSelectPlayer && PM->Get_PlayerTick())
	{
		if (nullptr == m_pRendererCom)
			return;

		fTimeDelta *= m_fTMRatio;

		LateTick_Func(fTimeDelta);

	}
}

HRESULT CPlayer03::Render()
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

	// For.Blur
	/*if (m_ePlayerMode == PMODE_BURST)
		m_fBlurPower = 1.f;
	else
		m_fBlurPower = 0.f;*/

	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &m_fBlurPower, sizeof(_float))))
		return E_FAIL;

	_float fGama = 1.6f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;

	if (m_bRim)
	{
		if (FAILED(Set_RenderPass(SHADER_RIM)))
			return E_FAIL;
	}

	else if (m_bPlayerLight)
	{
		if (FAILED(Set_RenderPass(SHADER_LIGHT)))
			return E_FAIL;
	}

	else
	{
		if (FAILED(Set_RenderPass(SHADER_DEFAULT)))
			return E_FAIL;
	}

	return S_OK;
}

// *[HeroJho]: 디버깅 창입니다. ImGui로 작동됩니다.
void CPlayer03::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	//m_Parts[PARTS_LCANNON]->Render_Debug();

	//IG->Debug_Transform(m_pTransformCom);


	//IG->Debug_Col(Get_Colliders("Player_HairCut"));

	/*ImGui::DragFloat4("Power, Time, LerpTime", (float*)&ShakeTest, 0.01f, -10.f, 10.f, "%.2f");
	ImGui::Checkbox("Lerp", &ShakeBool);

	ImGui::DragFloat4("Power, Time, StopTime", (float*)&FowTest, 0.01f, -10.f, 10.f, "%.2f");*/

	ImGui::DragFloat("TrailSpeed", &m_fMotionTrailSpeed, 0.01f, 0.01f, 2.f, "%.2f");
	ImGui::DragFloat("TrailLimit", &m_fMotionTrailLimit, 0.01f, 0.01f, 2.f, "%.2f");
	ImGui::DragFloat("Test1", &fTest1, 0.01f, 0.01f, 50.f, "%.2f");
	ImGui::DragFloat("Test2", &fTest2, 0.01f, 0.01f, 50.f, "%.2f");
	m_Parts[PARTS_HAIRB]->Render_Debug();

	/*for (auto& iter : m_MotionTrails)
	{
		((CPlayer03MotionTrail*)iter)->Set_UpSpeed(fTest1);
		((CPlayer03MotionTrail*)iter)->Set_UpSpeed(fTest2);
	}*/
	m_MotionTrails[0]->Render_Debug();

	// 파츠 디버그
	// m_Parts[PARTS_HAIRA]->Render_Debug();

	ImGui::End();
}

// *[HeroJho]: 충돌이 발생할 때 호출되는 함수입니다.
void CPlayer03::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Monster" == Desc.pOther->Get_Tag())
	{
		// 내 OBB와 상대편의 OBB와 충돌 했다면

		if ((!strcmp("Player_Attack", Desc.pMyDesc->sTag) &&
			!strcmp("Monster_Body1", Desc.pOtherDesc->sTag)))
		{
		
			m_bRangeCol = false;
			((CCreture*)Desc.pOther)->On_Hit(this);
			Check_BP(TM->Get_TimeDelta() * m_fTMRatio);
			CRM->Get_Cam()->Shake(0.05f, 0.15f);
			
		}

	}
	else if ("Tag_Boss" == Desc.pOther->Get_Tag())
	{
		if (!strcmp("Player_Push_Range", Desc.pMyDesc->sTag) &&
			!strcmp("Monster_Push_Range", Desc.pOtherDesc->sTag))
		{
			//밀어내기
			m_pTransformCom->PushMeXZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION), Desc.pMyDesc->vSize.x, XMLoadFloat4(&((CCreture*)Desc.pOther)->Get_WorldPos()), Desc.pOtherDesc->vSize.x, TM->Get_TimeDelta(), m_pNavigationCom);
			Update_Parts();
		}

		if ((!strcmp("Player_Attack", Desc.pMyDesc->sTag) &&
			!strcmp("Monster_Body1", Desc.pOtherDesc->sTag)))
		{
			
			((CCreture*)Desc.pOther)->On_Hit(this);
			Check_BP(TM->Get_TimeDelta() * m_fTMRatio);
		}

		if ((!strcmp("Player_Attack", Desc.pMyDesc->sTag) &&
			!strcmp("Monster_Body2", Desc.pOtherDesc->sTag)))
		{

			((CCreture*)Desc.pOther)->On_Hit(this);
			Check_BP(TM->Get_TimeDelta() * m_fTMRatio);
		}

		if ((!strcmp("Player_Attack", Desc.pMyDesc->sTag) &&
			!strcmp("Monster_Body3", Desc.pOtherDesc->sTag)))
		{

			((CCreture*)Desc.pOther)->On_Hit(this);
			Check_BP(TM->Get_TimeDelta() * m_fTMRatio);
		}
	}
}

void CPlayer03::On_Hit(CCreture* pOther)
{
	if (m_bHurt && !m_bReHp && !Get_PreUp())
	{
		GI->PlaySoundW(L"SwordHit.ogg", SD_PLAYER3_SK, 0.5f);

		_uint iRandomVoice = GI->Get_Random(0, 6);
		switch (iRandomVoice)
		{
		case 0:
			GI->PlaySoundW(L"Hero04_ba_09.wav", SD_PLAYER3_VO, 0.6f);
			break;
		case 1:
			GI->PlaySoundW(L"Hero04_ba_10.wav", SD_PLAYER3_VO, 0.6f);
			break;
		case 2:
			GI->PlaySoundW(L"Hero04_ba_11.wav", SD_PLAYER3_VO, 0.6f);
			break;
		case 3:
			GI->PlaySoundW(L"Hero04_ba_12.wav", SD_PLAYER3_VO, 0.6f);
			break;
		case 4:
			GI->PlaySoundW(L"Hero04_ba_13.wav", SD_PLAYER3_VO, 0.6f);
			break;
		case 5:
			GI->PlaySoundW(L"Hero04_ba_14.wav", SD_PLAYER3_VO, 0.6f);
			break;
		case 6:
			GI->PlaySoundW(L"Hero04_ba_15.wav", SD_PLAYER3_VO, 0.6f);
			break;
		}
		
		Hit_Effect(pOther);

		Hit_State(pOther);

		m_bHurt = false;
	}
}

void CPlayer03::Create_HitEffect(_float4 vWorldPos)
{
	switch (m_eHE)
	{
	case Client::CPlayer03::HE_1:
		Create_HE_1(vWorldPos);
		break;

	case Client::CPlayer03::HE_2:
		Create_HE_2(vWorldPos);
		break;

	case Client::CPlayer03::HE_3:
		Create_HE_3(vWorldPos);
		break;

	}
}

_float4 CPlayer03::Get_PlayerPos()
{
	_float4 PlayerPos;
	XMStoreFloat4(&PlayerPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	return PlayerPos;
}

void CPlayer03::Change_PlayerPos(_float4 vChanges)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMLoadFloat4(&vChanges));
}

void CPlayer03::Set_ChangeLook(DIR eDir, _float3 vDir)
{
	m_eDir = eDir;
	m_vTargetLook = vDir;
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&vDir));
}

_int CPlayer03::Get_NaviIndex()
{
	return m_pNavigationCom->Get_CurCellIndex();
}

void CPlayer03::Set_NaviIndex(_int iNaviIndex)
{
	m_pNavigationCom->Set_NaviIndex(iNaviIndex);
}

void CPlayer03::Reset_Animation()
{
	m_bCannonRender = false;
	if (m_ePlayerMode == PMODE_NORMAL)
	{
		m_bLGunRender = false;
		m_bRGunRender = false;
		m_pModelCom->Set_AnimIndex(IDLE1);
		m_eCurState = IDLE1;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
	}
	else
	{
		m_bLGunRender = true;
		m_bRGunRender = true;
		m_pModelCom->Set_AnimIndex(IDLE2);
		m_eCurState = IDLE2;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
	}
	m_bVoid = false;
}

HRESULT CPlayer03::Change_Navigation(_uint iNaviNum)
{
	if (m_pNavigationCom == nullptr)
		return E_FAIL;

	

	Safe_Release(m_pNavigationCom);

	CNavigation::NAVIGATIONDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = 0;

	switch (iNaviNum)
	{
	case 1:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage1Navigation"), TEXT("Stage1"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, -10.f, 1.f));
		m_vTargetLook = _float3{ 0.f,0.f,1.f };
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&m_vTargetLook));
		break;
	case 2:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage2Navigation"), TEXT("Stage2"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-29.3f, 0.f, 14.f, 1.f));
		m_vTargetLook = _float3{ 0.f,0.f,-1.f };
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&m_vTargetLook));
		break;
	case 3:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage3Navigation"), TEXT("Stage3"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(8.05f, 1.19f, -1.62f, 1.f));
		m_vTargetLook = _float3{ 1.f,0.f,0.f };
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&m_vTargetLook));
		break;
	case 4:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage4Navigation"), TEXT("Stage4"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(1.f, 0.f, 1.f, 1.f));
		m_vTargetLook = _float3{ 1.f,0.f,0.f };
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&m_vTargetLook));
		break;
	case 5:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage5Navigation"), TEXT("Stage5"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, -30.f, 1.f));
		m_vTargetLook = _float3{ 1.f,0.f,0.f };
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&m_vTargetLook));
		break;
	}

	_int iNaviIndex = m_pNavigationCom->Find_NaviIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pNavigationCom->Set_NaviIndex(iNaviIndex);

	Update_Parts();
	if (m_ePlayerMode == PMODE_BURST)
	{
		((CPlayer03HairA*)m_Parts[PARTS_HAIRA])->SetInitPos();
	}

	return S_OK;
}

void CPlayer03::Change_Tag()
{
	Set_State(STEPBACK);
}

void CPlayer03::Create_TagEffect()
{
	CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
	CWall::WALLINFO* Info = new CWall::WALLINFO;
	Info->vSize = _float3{ 0.15f,6.f,0.15f };
	XMStoreFloat4(&Info->vWorldPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	Info->vWorldPos.y += 10.f;
	Info->vColor = _float4{ 0.f,1.f,0.7f,1.f };
	Info->vColor2 = _float4{ 0.f,1.f,0.7f,1.f };
	Info->vRatio = _float2{ 1.f,1.f };
	Info->vUVFixSpeed = _float2{ 0.f,-0.5f };
	Info->vFixPosSpeed = _float3{ 0.f,-40.f,0.f };
	GI->Add_GameObjectToLayer(L"Wall", GI->Get_Cur_LevelIndex(), L"Layer_PlayerEffect", &Info);
}

void CPlayer03::Change_HairCut(_bool bOnOff)
{
	if (bOnOff)
	{		
		Update_Parts();
		if (m_ePlayerMode == PMODE_BURST)
		{
			Get_Colliders("Player_HairCut")->Set_Awake();
			((CPlayer03HairA*)m_Parts[PARTS_HAIRA])->Set_Wake();
			((CPlayer03HairA*)m_Parts[PARTS_HAIRA])->SetInitPos();
		}
	}
}

_bool CPlayer03::Get_BurstMode()
{
	if (m_ePlayerMode == PMODE_BURST)
		return true;
	else
		return false;
}

HRESULT CPlayer03::Create_Particle(PARTICLE ePT)
{
	switch (ePT)
	{

	
	case Client::CPlayer03::PT_SNIPERSHOT:
	{
		_matrix WorldMatrix = XMMatrixIdentity();
		_float4 vPos = m_Parts[PARTS_SNIPER]->Get_ParentPos();
		WorldMatrix.r[3] = XMLoadFloat4(&vPos);
		_float4x4 matrix;
		XMStoreFloat4x4(&matrix, WorldMatrix);
		PTM->Create_AlphaParticle(L"Player03SniperShot", matrix);
		break;
	}

	case Client::CPlayer03::PT_SKILL13:
	{
		if (FAILED(PTM->Create_AlphaParticle(L"Player03Skill13", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;
		break;
	}

	case Client::CPlayer03::PT_SKILL1_1:
	{
		if (FAILED(PTM->Create_AlphaParticle(L"Player03Skill1_1", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;
		break;
	}

	case Client::CPlayer03::PT_SKILL1_2:
	{
		if (FAILED(PTM->Create_AlphaParticle(L"Player03Skill1_2", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;
		break;
	}

	case Client::CPlayer03::PT_SKILL9:
	{
		if (FAILED(PTM->Create_AlphaParticle(L"Player03Skill9", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;
		break;
	}

	case Client::CPlayer03::PT_SKILL2_1:
	{	
		if (FAILED(PTM->Create_AlphaParticle(L"Player03Skill2_3", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;
		if (FAILED(PTM->Create_AlphaParticle(L"Player03Skill2_1", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;
		break;
	}

	case Client::CPlayer03::PT_SKILL2_2:
	{

		if (FAILED(PTM->Create_AlphaParticle(L"Player03Skill2_4", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;
		if (FAILED(PTM->Create_AlphaParticle(L"Player03Skill2_2", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;
		break;
	}

	case Client::CPlayer03::PT_ATTACK1:
	{
		if (FAILED(PTM->Create_AlphaParticle(L"Player03Attack1_2", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;
		if (FAILED(PTM->Create_AlphaParticle(L"Player03Attack1", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;
		
		break;
	}

	case Client::CPlayer03::PT_ATTACK2:
	{
		if (FAILED(PTM->Create_AlphaParticle(L"Player03Attack2_2", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;
		if (FAILED(PTM->Create_AlphaParticle(L"Player03Attack2", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;

		break;
	}

	case Client::CPlayer03::PT_ATTACK3:
	{
		if (FAILED(PTM->Create_AlphaParticle(L"Player03Attack3_2", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;
		if (FAILED(PTM->Create_AlphaParticle(L"Player03Attack3_3", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;
		if (FAILED(PTM->Create_AlphaParticle(L"Player03Attack3", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;
		if (FAILED(PTM->Create_AlphaParticle(L"Player03Attack3_1", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;

		break;
	}

	

	}
	return S_OK;
}

HRESULT CPlayer03::Create_ParticleInstance(PARTICLEINSTANCE ePTI)
{
	switch (ePTI)
	{
	case Client::CPlayer03::PTI_SKILL13:
		if (FAILED(PTM->Create_InstanceParticle(L"Player03Skill13", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;
		break;

	case Client::CPlayer03::PTI_SKILL1_1:
		if (FAILED(PTM->Create_InstanceParticle(L"Player03Skill1_1", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;
		break;

	case Client::CPlayer03::PTI_SKILL1_2:
		if (FAILED(PTM->Create_InstanceParticle(L"Player03Skill1_2", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;
		break;

	case Client::CPlayer03::PTI_SKILL9:
		if (FAILED(PTM->Create_InstanceParticle(L"Player03Skill9", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;
		break;

	case Client::CPlayer03::PTI_SKILL2_1BULLET:
	{

		if (FAILED(PTM->Create_InstanceParticle(L"Player03Skill2_1Bullet", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;
	}
		break;

	case Client::CPlayer03::PTI_SKILL2_2BULLET:
	{
	
		if (FAILED(PTM->Create_InstanceParticle(L"Player03Skill2_2Bullet", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;
	}
		break;

	case Client::CPlayer03::PTI_SKILL7:
	{

		if (FAILED(PTM->Create_InstanceParticle(L"Player03SniperBullet", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;
	}
	break;

	case Client::CPlayer03::PTI_SKILL7_1:
	{
		if (FAILED(PTM->Create_InstanceParticle(L"Player03SniperSmoke2", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;

		if (FAILED(PTM->Create_InstanceParticle(L"Player03SniperSmoke", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;

		if (FAILED(PTM->Create_InstanceParticle(L"Player03SniperSmoke3", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;
		break;
	}
	

	case Client::CPlayer03::PTI_ATTACK1:
	{

		if (FAILED(PTM->Create_InstanceParticle(L"Player03Attack1Bullet", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;
		break;
	}
	

	case Client::CPlayer03::PTI_ATTACK2:
	{

		if (FAILED(PTM->Create_InstanceParticle(L"Player03Attack2Bullet", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;
	}
	break;

	case Client::CPlayer03::PTI_ATTACK3:
	{

		if (FAILED(PTM->Create_InstanceParticle(L"Player03Attack3Bullet", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;
		if (FAILED(PTM->Create_InstanceParticle(L"Player03Attack3Bullet_2", m_pTransformCom->Get_WorldFloat4x4())))
			return E_FAIL;
	}
	break;

	case Client::CPlayer03::PTI_BURST:
	{
		PTM->Create_InstanceParticle(L"Player03Burst", m_pTransformCom->Get_WorldFloat4x4());
		break;
	}

	case Client::CPlayer03::PTI_RGUNPARTICLE:
	{
		_matrix	RHandMatrix = m_Sockets[SOCKET_WEAPON_R]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
		RHandMatrix.r[2] *= -1.f;
		_float4x4 Matrix;
		XMStoreFloat4x4(&Matrix, RHandMatrix);
		PTM->Create_InstanceParticle(L"Player03GunParticle", Matrix);
		PTM->Create_InstanceParticle(L"Player03GunParticle_2", Matrix);
		break;
	}

	case Client::CPlayer03::PTI_LGUNPARTICLE:
	{
		_matrix	LHandMatrix = m_Sockets[SOCKET_WEAPON_L]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
		LHandMatrix.r[2] *= -1.f;
		_float4x4 Matrix;
		XMStoreFloat4x4(&Matrix, LHandMatrix);
		PTM->Create_InstanceParticle(L"Player03GunParticle", Matrix);
		PTM->Create_InstanceParticle(L"Player03GunParticle_2", Matrix);
		break;
	}

	case Client::CPlayer03::PTI_RSHOTGUN:
	{
		_matrix	RHandMatrix = m_Sockets[SOCKET_WEAPON_R]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
		RHandMatrix.r[2] *= -1.f;
		_float4x4 Matrix;
		XMStoreFloat4x4(&Matrix, RHandMatrix);
		PTM->Create_InstanceParticle(L"Player03ShotGunParticle", Matrix);
		PTM->Create_InstanceParticle(L"Player03ShotGunParticle_2", Matrix);
		break;
	}

	case Client::CPlayer03::PTI_LSHOTGUN:
	{
		_matrix	LHandMatrix = m_Sockets[SOCKET_WEAPON_L]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
		LHandMatrix.r[2] *= -1.f;
		_float4x4 Matrix;
		XMStoreFloat4x4(&Matrix, LHandMatrix);
		PTM->Create_InstanceParticle(L"Player03ShotGunParticle", Matrix);
		PTM->Create_InstanceParticle(L"Player03ShotGunParticle_2", Matrix);
		break;
	}

	case Client::CPlayer03::PTI_RAGEGUN:
	{
		_matrix	LHandMatrix = m_Sockets[SOCKET_WEAPON_L]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
		LHandMatrix.r[2] *= -1.f;
		_float4x4 Matrix;
		XMStoreFloat4x4(&Matrix, LHandMatrix);
		PTM->Create_InstanceParticle(L"Player03GunParticle", Matrix);
		PTM->Create_InstanceParticle(L"Player03GunParticle_2", Matrix);
		PTM->Create_InstanceParticle(L"Player03RageGunBullet", Matrix);
		PTM->Create_AlphaParticle(L"Player03RageGunParticle", Matrix);
		PTM->Create_AlphaParticle(L"Player03RageGunParticle_2", Matrix);

		_matrix	RHandMatrix = m_Sockets[SOCKET_WEAPON_R]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
		RHandMatrix.r[2] *= -1.f;
		XMStoreFloat4x4(&Matrix, RHandMatrix);
		PTM->Create_InstanceParticle(L"Player03GunParticle", Matrix);
		PTM->Create_InstanceParticle(L"Player03GunParticle_2", Matrix);
		PTM->Create_InstanceParticle(L"Player03RageGunBullet", Matrix);
		PTM->Create_AlphaParticle(L"Player03RageGunParticle", Matrix);
		PTM->Create_AlphaParticle(L"Player03RageGunParticle_2", Matrix);

		CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
		Info->fBlurPower = 0.f;
		Info->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
		Info->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 0.7f,0.7f };
		Info->vRatioSpeed = { 1.f,1.f };
		Info->vScale = { 0.1f,0.1f,0.1f };
		Info->vScaleSpeed = { 30.f,30.f,30.f };
		Info->vUvFixSpeed = { 1.f,3.f };
		Info->vWorldMatrix = Matrix;
		GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
		break;
	}

	case Client::CPlayer03::PTI_RAGELSHOTGUN:
	{
		_matrix	LHandMatrix = m_Sockets[SOCKET_WEAPON_L]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
		LHandMatrix.r[2] *= -1.f;
		_float4x4 Matrix;
		XMStoreFloat4x4(&Matrix, LHandMatrix);
		PTM->Create_InstanceParticle(L"Player03ShotGunParticle", Matrix);
		PTM->Create_InstanceParticle(L"Player03ShotGunParticle_2", Matrix);
		PTM->Create_AlphaParticle(L"Player03RageShotGunParticle", Matrix);
		PTM->Create_InstanceParticle(L"Player03RageShotGunBullet", Matrix);

		CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
		Info->fBlurPower = 0.f;
		Info->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
		Info->vLocalPos = { 0.f,0.2f,0.4f,1.f };
		Info->vRatio = { 1.f,1.f };
		Info->vRatioSpeed = { 1.f,1.f };
		Info->vScale = { 0.1f,0.1f,0.1f };
		Info->vScaleSpeed = { 20.f,20.f,20.f };
		Info->vUvFixSpeed = { 0.5f,1.5f };
		Info->vWorldMatrix = Matrix;
		GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
		break;
	}

	case Client::CPlayer03::PTI_RAGERSHOTGUN:
	{

		_matrix	RHandMatrix = m_Sockets[SOCKET_WEAPON_R]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
		RHandMatrix.r[2] *= -1.f;
		_float4x4 Matrix;
		XMStoreFloat4x4(&Matrix, RHandMatrix);
		PTM->Create_InstanceParticle(L"Player03ShotGunParticle", Matrix);
		PTM->Create_InstanceParticle(L"Player03ShotGunParticle_2", Matrix);
		PTM->Create_AlphaParticle(L"Player03RageShotGunParticle", Matrix);
		PTM->Create_InstanceParticle(L"Player03RageShotGunBullet", Matrix);

		CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
		Info->fBlurPower = 0.f;
		Info->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
		Info->vLocalPos = { 0.f,0.2f,0.4f,1.f };
		Info->vRatio = { 1.f,1.f };
		Info->vRatioSpeed = { 1.f,1.f };
		Info->vScale = { 0.1f,0.1f,0.1f };
		Info->vScaleSpeed = { 20.f,20.f,20.f };
		Info->vUvFixSpeed = { 0.5f,1.5f };
		Info->vWorldMatrix = Matrix;
		GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
		break;
	}


	}

	return S_OK;
	
}

HRESULT CPlayer03::Create_Effect(EFFECT eET)
{
	switch (eET)
	{
	case Client::CPlayer03::ET_DASH:
	{
		CWind::WINDINFO* Info = new CWind::WINDINFO;
		Info->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
		Info->vFixPosSpeed = { 0.f,0.f,-3.f };
		Info->vFixScaleSpeed = { 60.f,60.f,60.f };
		Info->vRatio = { 1.f,1.f };
		Info->vRatioSpeed = { 2.f,2.f };
		Info->vUVFixSpeed = { 1.f,-3.f };
		_matrix Effectmatrix;
		Effectmatrix.r[0] = { 0.1f, 0.f, 0.f, 0.f };
		Effectmatrix.r[1] = { 0.f, 0.f, 1.f, 0.f };
		Effectmatrix.r[2] = { 0.f, -1.f, 0.f, 0.f };
		Effectmatrix.r[3] = { 0.f, 1.f, 1.f, 1.f };
		XMStoreFloat4x4(&Info->WorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		if (FAILED(GI->Add_GameObjectToLayer(L"Wind", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
			return E_FAIL;

		CWind::WINDINFO* Info2 = new CWind::WINDINFO;
		Info2->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info2->vColor2 = { 0.7f,0.7f,0.7f,1.f };
		Info2->vFixPosSpeed = { 0.f,0.f,-5.f };
		Info2->vFixScaleSpeed = { 80.f,80.f,80.f };
		Info2->vRatio = { 1.f,1.f };
		Info2->vRatioSpeed = { 2.f,2.f };
		Info2->vUVFixSpeed = { 1.f,-3.f };
		Effectmatrix.r[0] = { 0.1f, 0.f, 0.f, 0.f };
		Effectmatrix.r[1] = { 0.f, 0.f, 1.f, 0.f };
		Effectmatrix.r[2] = { 0.f, -1.f, 0.f, 0.f };
		Effectmatrix.r[3] = { 0.f, 1.f, 1.f, 1.f };
		XMStoreFloat4x4(&Info2->WorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		if (FAILED(GI->Add_GameObjectToLayer(L"Wind", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info2)))
			return E_FAIL;

		break;
	}

	case Client::CPlayer03::ET_SKILLCHARGE:
	{
		CEffectSpin::EFFECTSPINDESC EffectSpinDesc;
		EffectSpinDesc.iSpinType = 0;
		EffectSpinDesc.iPlayerType = CEffectSpin::PLAYERTYPE::PLAYERTYPE_LI;
		if (FAILED(GI->Add_GameObjectToLayer(TEXT("EffectSpin"), GI->Get_Cur_LevelIndex(), TEXT("Layer_KSEFFECT"), &EffectSpinDesc)))
			return E_FAIL;
		EffectSpinDesc.iSpinType = 1;
		EffectSpinDesc.iPlayerType = CEffectSpin::PLAYERTYPE::PLAYERTYPE_LI;
		if (FAILED(GI->Add_GameObjectToLayer(TEXT("EffectSpin"), GI->Get_Cur_LevelIndex(), TEXT("Layer_KSEFFECT"), &EffectSpinDesc)))
			return E_FAIL;
		break;
	}

	case Client::CPlayer03::ET_RSHOTGUN:
	{
		_matrix	RHandMatrix = m_Sockets[SOCKET_WEAPON_R]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
		RHandMatrix.r[2] *= -1.f;
		_float4x4 Matrix;
		XMStoreFloat4x4(&Matrix, RHandMatrix);

		CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
		Info->fBlurPower = 0.f;
		Info->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
		Info->vLocalPos = { 0.f,0.2f,0.4f,1.f };
		Info->vRatio = { 1.f,1.f };
		Info->vRatioSpeed = { 1.f,1.f };
		Info->vScale = { 0.1f,0.1f,0.1f };
		Info->vScaleSpeed = { 20.f,20.f,20.f };
		Info->vUvFixSpeed = { 0.5f,1.5f };
		Info->vWorldMatrix = Matrix;
		GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
		break;
	}

	case Client::CPlayer03::ET_LSHOTGUN:
	{
		_matrix	LHandMatrix = m_Sockets[SOCKET_WEAPON_L]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
		LHandMatrix.r[2] *= -1.f;
		_float4x4 Matrix;
		XMStoreFloat4x4(&Matrix, LHandMatrix);

		CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
		Info->fBlurPower = 0.f;
		Info->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
		Info->vLocalPos = { 0.f,0.2f,0.4f,1.f };
		Info->vRatio = { 1.f,1.f };
		Info->vRatioSpeed = { 1.f,1.f };
		Info->vScale = { 0.1f,0.1f,0.1f };
		Info->vScaleSpeed = { 20.f,20.f,20.f };
		Info->vUvFixSpeed = { 0.5f,1.5f };
		Info->vWorldMatrix = Matrix;
		GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
		break;
	}

	case Client::CPlayer03::ET_SKILL11:
	{
		CWind::WINDINFO* Info = new CWind::WINDINFO;
		Info->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
		Info->vFixPosSpeed = { 0.f,0.f,-3.f };
		Info->vFixScaleSpeed = { 60.f,60.f,60.f };
		Info->vRatio = { 1.f,1.f };
		Info->vRatioSpeed = { 2.f,2.f };
		Info->vUVFixSpeed = { 1.f,-3.f };
		_matrix Effectmatrix;
		Effectmatrix.r[0] = { 0.1f, 0.f, 0.f, 0.f };
		Effectmatrix.r[1] = { 0.f, 0.f, 1.f, 0.f };
		Effectmatrix.r[2] = { 0.f, -1.f, 0.f, 0.f };
		Effectmatrix.r[3] = { 0.f, 0.5f, 1.f, 1.f };
		XMStoreFloat4x4(&Info->WorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		if (FAILED(GI->Add_GameObjectToLayer(L"Wind", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
			return E_FAIL;

		CWind::WINDINFO* Info2 = new CWind::WINDINFO;
		Info2->vColor = { 0.7f,0.7f,0.7f,1.f };
		Info2->vColor2 = { 0.7f,0.7f,0.7f,1.f };
		Info2->vFixPosSpeed = { 0.f,0.f,-5.f };
		Info2->vFixScaleSpeed = { 80.f,80.f,80.f };
		Info2->vRatio = { 1.f,1.f };
		Info2->vRatioSpeed = { 2.f,2.f };
		Info2->vUVFixSpeed = { 1.f,-3.f };
		Effectmatrix.r[0] = { 0.1f, 0.f, 0.f, 0.f };
		Effectmatrix.r[1] = { 0.f, 0.f, 1.f, 0.f };
		Effectmatrix.r[2] = { 0.f, -1.f, 0.f, 0.f };
		Effectmatrix.r[3] = { 0.f, 0.5f, 1.f, 1.f };
		XMStoreFloat4x4(&Info2->WorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
		if (FAILED(GI->Add_GameObjectToLayer(L"Wind", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info2)))
			return E_FAIL;
		break;
	}

	case Client::CPlayer03::ET_CANNONBULLET:
	{
		CPlayer03CannonBullet::BULLETINFO* Info = new CPlayer03CannonBullet::BULLETINFO;
		Info->vScale = { 4.f,4.f,4.f };
		Info->Worldmatrix = m_pTransformCom->Get_WorldFloat4x4();
		Info->Worldmatrix._42 += 1.3f;
		Info->bRage = false;	
		GI->Add_GameObjectToLayer(L"Player03CannonBullet", TM->Get_CurLevel(), L"Layer_Player03Effect", &Info);
		PTM->Create_InstanceParticle(L"Player03SniperSmoke2", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03SniperSmoke", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03SniperSmoke3", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03CannonDistortion_3", m_pTransformCom->Get_WorldFloat4x4());
		break;
	}

	case Client::CPlayer03::ET_RAGECANNON:
	{
		CPlayer03CannonBullet::BULLETINFO* Info = new CPlayer03CannonBullet::BULLETINFO;
		Info->vScale = { 4.f,4.f,4.f };
		Info->Worldmatrix = m_pTransformCom->Get_WorldFloat4x4();
		Info->Worldmatrix._42 += 1.6f;
		Info->bRage = true;
		GI->Add_GameObjectToLayer(L"Player03CannonBullet", TM->Get_CurLevel(), L"Layer_Player03Effect", &Info);
		PTM->Create_InstanceParticle(L"Player03RageSmoke_2", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03RageSmoke", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03RageSmoke_3", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03CannonDistortion_3", m_pTransformCom->Get_WorldFloat4x4());
		break;
	}

	case Client::CPlayer03::ET_BOMB:
	{
		_matrix	RHandMatrix = m_Sockets[SOCKET_WEAPON_R]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
		RHandMatrix.r[2] *= -1.f;
		_float4x4 RMatrix;
		XMStoreFloat4x4(&RMatrix, RHandMatrix);

		_matrix	LHandMatrix = m_Sockets[SOCKET_WEAPON_L]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
		LHandMatrix.r[2] *= -1.f;
		_float4x4 LMatrix;
		XMStoreFloat4x4(&LMatrix, LHandMatrix);


		CPlayer03Bomb::BOMBINFO* Info = new CPlayer03Bomb::BOMBINFO;
		Info->vScale = { 1.f,1.f,1.f };
		Info->Worldmatrix = m_pTransformCom->Get_WorldFloat4x4();
		Info->Worldmatrix._42 += 2.f;
		Info->fAirCount = GI->Get_FloatRandom(3.f, 6.f);
		Info->iNaviIndex = m_pNavigationCom->Get_CurCellIndex();
		GI->Add_GameObjectToLayer(L"Player03Bomb", TM->Get_CurLevel(), L"Layer_Player03Effect", &Info);

		CPlayer03Bomb::BOMBINFO* Info2 = new CPlayer03Bomb::BOMBINFO;
		Info2->vScale = { 1.f,1.f,1.f };
		Info2->Worldmatrix = LMatrix;
		Info2->fAirCount = GI->Get_FloatRandom(3.f, 6.f);
		Info2->iNaviIndex = m_pNavigationCom->Get_CurCellIndex();
		GI->Add_GameObjectToLayer(L"Player03Bomb", TM->Get_CurLevel(), L"Layer_Player03Effect", &Info2);

		CPlayer03Bomb::BOMBINFO* Info3 = new CPlayer03Bomb::BOMBINFO;
		Info3->vScale = { 1.f,1.f,1.f };
		Info3->Worldmatrix = RMatrix;
		Info3->fAirCount = GI->Get_FloatRandom(3.f, 6.f);
		Info3->iNaviIndex = m_pNavigationCom->Get_CurCellIndex();
		GI->Add_GameObjectToLayer(L"Player03Bomb", TM->Get_CurLevel(), L"Layer_Player03Effect", &Info3);


		break;
	}

	}

	
	

	return S_OK;
}

HRESULT CPlayer03::Ready_Trails()
{
	/*CSwordTrail::TRAILINFO Info;
	Info.HighAndLow.vHigh = { 0.3f,0.f,0.f };
	Info.HighAndLow.vLow = { -0.3f,0.f,0.f };
	Info.TexName = L"SwordTrail01";
	Info.vColor1 = { 0.f,1.f,0.9f,1.f };
	Info.vColor2 = { 0.f,1.f,0.9f,1.f };
	Info.vRatio = { 1.f,1.f };
	Info.bFixColor = true;
	Info.iShaderPass = 1;
	Info.fBlurPower = 2.f;
	CGameObject*		SwordTrail1 = GI->Clone_GameObject(TEXT("SwordTrailObj"), &Info);
	if (nullptr == SwordTrail1)
		return E_FAIL;
	m_SwordTrails.push_back((CSwordTrailObj*)SwordTrail1);*/

	CPointTrail::TRAILCREATEINFO Info2;
	Info2.vColor = { 1.f, 1.f, 1.f,1.f };
	Info2.vColor2 = { 0.f, 1.f, 0.9f,1.f };
	Info2.bFixColor = true;
	Info2.fBlurPower = 0.5f;
	Info2.iShaderPass = 1;
	Info2.vRatio = { 1.f,1.f };
	Info2.fSize = 0.07f;
	Info2.TexName = L"PointTrail02";
	Info2.iMaxIndex = 10;

	CGameObject*		LKick = GI->Clone_GameObject(TEXT("PointTrailObj"), &Info2);
	if (nullptr == LKick)
		return E_FAIL;
	m_PointTrails.push_back((CPointTrailObj*)LKick);

	CGameObject*		RKick = GI->Clone_GameObject(TEXT("PointTrailObj"), &Info2);
	if (nullptr == RKick)
		return E_FAIL;
	m_PointTrails.push_back((CPointTrailObj*)RKick);

	CGameObject*		LHand = GI->Clone_GameObject(TEXT("PointTrailObj"), &Info2);
	if (nullptr == LHand)
		return E_FAIL;
	m_PointTrails.push_back((CPointTrailObj*)LHand);

	CGameObject*		RHand = GI->Clone_GameObject(TEXT("PointTrailObj"), &Info2);
	if (nullptr == RHand)
		return E_FAIL;
	m_PointTrails.push_back((CPointTrailObj*)RHand);

	CGameObject*		pMotionTrail1 = GI->Clone_GameObject(TEXT("Player03MotionTrail"));
	if (nullptr == pMotionTrail1)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail1);

	CGameObject*		pMotionTrail2 = GI->Clone_GameObject(TEXT("Player03MotionTrail"));
	if (nullptr == pMotionTrail2)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail2);

	CGameObject*		pMotionTrail3 = GI->Clone_GameObject(TEXT("Player03MotionTrail"));
	if (nullptr == pMotionTrail3)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail3);

	CGameObject*		pMotionTrail4 = GI->Clone_GameObject(TEXT("Player03MotionTrail"));
	if (nullptr == pMotionTrail4)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail4);

	CGameObject*		pMotionTrail5 = GI->Clone_GameObject(TEXT("Player03MotionTrail"));
	if (nullptr == pMotionTrail5)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail5);

	CGameObject*		pMotionTrail6 = GI->Clone_GameObject(TEXT("Player03MotionTrail"));
	if (nullptr == pMotionTrail6)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail6);

	CGameObject*		pMotionTrail7 = GI->Clone_GameObject(TEXT("Player03MotionTrail"));
	if (nullptr == pMotionTrail7)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail7);

	CGameObject*		pMotionTrail8 = GI->Clone_GameObject(TEXT("Player03MotionTrail"));
	if (nullptr == pMotionTrail8)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail8);

	CGameObject*		pMotionTrail9 = GI->Clone_GameObject(TEXT("Player03MotionTrail"));
	if (nullptr == pMotionTrail9)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail9);

	CGameObject*		pMotionTrail10 = GI->Clone_GameObject(TEXT("Player03MotionTrail"));
	if (nullptr == pMotionTrail10)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail10);

	return S_OK;
}

void CPlayer03::Create_MotionTrail(_float fTimeDelta)
{
	if (m_bMotionTrail)
	{
		if (m_MotionTrails.size() > 0)
			Set_MotionTrail(fTimeDelta);
	}

	for (auto& iter : m_MotionTrails)
	{
		iter->Tick(fTimeDelta);
	}

}

void CPlayer03::Create_PointTrail(_float fTimeDelta)
{

	Control_KickTrail(fTimeDelta);
	Control_HandTrail(fTimeDelta);
	
}

void CPlayer03::Create_SwordTrail(_float fTimeDelta)
{
}

void CPlayer03::Set_MotionTrail(_float fTimeDelta)
{
	m_fMotionTrailAcc += m_fMotionTrailSpeed * fTimeDelta;
	if (m_fMotionTrailAcc >= m_fMotionTrailLimit)
	{
		if (m_ePlayerMode == PMODE_BURST)
		{
			CPlayer03MotionTrail::PLAYER03MOTIONTRAILINFO Info;
			Info.WorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			Info.eHeadType = CPlayer03MotionTrail::HEADA;
			Info.Bodys = m_pModelCom->Get_HierarchyPointer();
			Info.HeadAMatrix = ((CPlayer03HairA*)m_Parts[PARTS_HAIRA])->Get_ConbineMatrix();
			Info.Heads = ((CPlayer03HairA*)m_Parts[PARTS_HAIRA])->Get_ModelCom()->Get_HierarchyPointer();
			((CPlayer03MotionTrail*)m_MotionTrails[m_iMotionTrailIndex])->Set_OnTrail(Info);
		}
		else
		{
			CPlayer03MotionTrail::PLAYER03MOTIONTRAILINFO Info;
			Info.WorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			Info.eHeadType = CPlayer03MotionTrail::HEADB;
			Info.Bodys = m_pModelCom->Get_HierarchyPointer();
			Info.HeadBMatrix = ((CPlayer03HairB*)m_Parts[PARTS_HAIRB])->Get_ConbineMatrix();			
			((CPlayer03MotionTrail*)m_MotionTrails[m_iMotionTrailIndex])->Set_OnTrail(Info);
		}
		
		m_fMotionTrailAcc = 0.f;
		++m_iMotionTrailIndex;
		if (m_iMotionTrailIndex >= m_MotionTrails.size())
		{
			m_bMotionTrail = false;
			m_iMotionTrailIndex = 0;
		}
	}
}

void CPlayer03::Control_KickTrail(_float fTimeDelta)
{
	if (m_bPointTrail)
	{
		m_PointTrails[PTR_LKICK]->TrailOn();
		m_PointTrails[PTR_RKICK]->TrailOn();
		if (m_eCurState != IDLE2 && m_eCurState != IDLE1)
		{

			_matrix	LKickMatrix = m_Sockets[SOCKET_LKICK]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
			_float4 LKickPos;
			XMStoreFloat4(&LKickPos, LKickMatrix.r[3]);
			m_PointTrails[PTR_LKICK]->Add_Point(LKickPos);


			_matrix RKickMatrix = m_Sockets[SOCKET_RKICK]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
			_float4 RKickPos;
			XMStoreFloat4(&RKickPos, RKickMatrix.r[3]);
			m_PointTrails[PTR_RKICK]->Add_Point(RKickPos);
		}
		m_PointTrails[PTR_LKICK]->Tick(fTimeDelta);
		m_PointTrails[PTR_RKICK]->Tick(fTimeDelta);
	}
	else
	{
		m_PointTrails[PTR_LKICK]->TrailOff();
		m_PointTrails[PTR_RKICK]->TrailOff();
	}
}

void CPlayer03::Control_HandTrail(_float fTimeDelta)
{
	if (m_bLHandTrail)
	{
		m_PointTrails[PTR_LHAND]->TrailOn();

		if (m_eCurState != IDLE2 && m_eCurState != IDLE1)
		{

			_matrix	LKickMatrix = m_Sockets[SOCKET_WEAPON_L]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
			_float4 LKickPos;
			XMStoreFloat4(&LKickPos, LKickMatrix.r[3]);
			m_PointTrails[PTR_LHAND]->Add_Point(LKickPos);			
		}
		m_PointTrails[PTR_LHAND]->Tick(fTimeDelta);

	}
	else
	{
		m_PointTrails[PTR_LHAND]->TrailOff();
	}

	if (m_bRHandTrail)
	{
		m_PointTrails[PTR_RHAND]->TrailOn();

		if (m_eCurState != IDLE2 && m_eCurState != IDLE1)
		{

			_matrix	LKickMatrix = m_Sockets[SOCKET_WEAPON_R]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
			_float4 LKickPos;
			XMStoreFloat4(&LKickPos, LKickMatrix.r[3]);
			m_PointTrails[PTR_RHAND]->Add_Point(LKickPos);
		}
		m_PointTrails[PTR_RHAND]->Tick(fTimeDelta);

	}
	else
	{
		m_PointTrails[PTR_RHAND]->TrailOff();
	}
}

void CPlayer03::Player_Die()
{
	m_bDie = true;
	m_bSelectPlayer = false;
	PM->Change_Player(CPlayer::PLAYER_04);
}

void CPlayer03::Reset_SlowMotionTrail()
{
	for (auto& iter : m_MotionTrails)
	{
		((CPlayer03MotionTrail*)iter)->Set_AlphaLimit(0.1f);
		((CPlayer03MotionTrail*)iter)->Set_ShaderPass(8);
		((CPlayer03MotionTrail*)iter)->Set_ShaderPass2(21);
		((CPlayer03MotionTrail*)iter)->Set_BlurPower(0.3f);
		((CPlayer03MotionTrail*)iter)->Set_DownSpeed(1.f);
		((CPlayer03MotionTrail*)iter)->Set_UpSpeed(1.f);
	}
}

void CPlayer03::Set_SlowMotionTrail()
{
	for (auto& iter : m_MotionTrails)
	{
		((CPlayer03MotionTrail*)iter)->Set_AlphaLimit(1.f);
		((CPlayer03MotionTrail*)iter)->Set_ShaderPass(10);
		((CPlayer03MotionTrail*)iter)->Set_ShaderPass2(22);
		((CPlayer03MotionTrail*)iter)->Set_DownSpeed(20.f);	
		((CPlayer03MotionTrail*)iter)->Set_UpSpeed(5.f);
	}
}

void CPlayer03::Check_PlayerLight(_float fTimeDelta)
{
	if (m_bPlayerLight)
	{
		m_eShaderPass = SHADER_LIGHT;
		((CPlayer03HairA*)m_Parts[PARTS_HAIRA])->Set_PlayerLight(true);
		((CPlayer03HairB*)m_Parts[PARTS_HAIRB])->Set_PlayerLight(true);
		m_fPlayerLightAcc += 1.f * fTimeDelta;
		if (m_fPlayerLightAcc >= m_fPlayerLightLimit)
		{
			m_bPlayerLight = false;
			m_fPlayerLightAcc = 0.f;
			m_eShaderPass = SHADER_DEFAULT;
			((CPlayer03HairA*)m_Parts[PARTS_HAIRA])->Set_PlayerLight(false);
			((CPlayer03HairB*)m_Parts[PARTS_HAIRB])->Set_PlayerLight(false);
			Create_LightEffect();
		}
	}
}

void CPlayer03::Change_Sound()
{
	
		_uint iRandomVoice = GI->Get_Random(1, 3);
	

		GI->PlaySoundW(L"UI_GodChange.wav", SD_UI, 0.6f);

		switch (iRandomVoice)
		{
		case 1:
			GI->PlaySoundW(L"Hero04_ba_56.wav", SD_PLAYER3_VO, 1.f);
			break;
		case 2:
			GI->PlaySoundW(L"Hero04_ba_57.wav", SD_PLAYER3_VO, 1.f);
			break;
		case 3:
			GI->PlaySoundW(L"Hero04_ba_58.wav", SD_PLAYER3_VO, 1.f);
			break;
		default:
			break;
		}
	
}

_bool CPlayer03::Get_IsJump()
{
	return m_pTransformCom->Get_Air();
}

void CPlayer03::Set_ScenePos(_float4 vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));
	m_pNavigationCom->Set_NaviIndex(m_pNavigationCom->Find_NaviIndex(XMLoadFloat4(&vPos)));
}

void CPlayer03::Set_SceneLook(_float4 vPos)
{
Update_Parts();
   m_Parts[PARTS_HAIRA]->LateTick(1.f / 60.f);
   ((CPlayer03HairA*)m_Parts[PARTS_HAIRA])->SetInitPos();
   Tick_Col(m_pTransformCom->Get_WorldMatrix());
   m_pTransformCom->LookAt_ForLandObject(XMLoadFloat4(&vPos));
   XMStoreFloat3(&m_vTargetLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));

}

void CPlayer03::Set_SceneIdle()
{
	Reset_Collider();
	Set_State(IDLE1);
}

void CPlayer03::Set_SceneHit(HITTYPE eHitType, _float fPushPower, _float fAirPower, _float4 vPos, _float3 vDir)
{
	Reset_Collider();
	m_fPushPower = fPushPower;
	m_fAirPower = fAirPower;

	switch (eHitType)
	{
	case Client::CCreture::TYPE_HIT:
		if (m_pTransformCom->Check_TargetFront(XMLoadFloat4(&vPos)))
			Set_State(HURTFRONT);
		else
			Set_State(HURTBACK);
		break;
	case Client::CCreture::TYPE_SPIN:
		XMStoreFloat3(&m_vTargetLook, XMVector3Normalize(XMLoadFloat4(&vPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
		XMStoreFloat3(&m_vPushLook, XMVector3Normalize(XMLoadFloat3(&vDir)));
		Set_State(HITSPIN);
		m_pTransformCom->ResetAndJump(m_fAirPower);
		break;
	case Client::CCreture::TYPE_AWAY:
		XMStoreFloat3(&m_vTargetLook, XMVector3Normalize(XMLoadFloat4(&vPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
		XMStoreFloat3(&m_vPushLook, XMVector3Normalize(XMLoadFloat3(&vDir)));
		Set_State(HITAWAY);
		m_pTransformCom->ResetAndJump(m_fAirPower);
		break;
	case Client::CCreture::TYPE_UP:
		break;
	case Client::CCreture::TYPE_FASTDOWN:
		XMStoreFloat3(&m_vTargetLook, XMVector3Normalize(XMLoadFloat4(&vPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
		XMStoreFloat3(&m_vPushLook, XMVector3Normalize(XMLoadFloat3(&vDir)));
		Set_State(FALLDOWN);
		break;
	}
}

void CPlayer03::Set_SceneRealHit(HITTYPE eHitType, _float fPushPower, _float fAirPower, _float4 vPos, _float3 vDir, _float fDamage)
{
	Reset_Collider();
	m_fPushPower = fPushPower;
	m_fAirPower = fAirPower;
	m_bRim = true;
	m_fNowHp -= fDamage;
	switch (eHitType)
	{
	case Client::CCreture::TYPE_HIT:
		if (m_pTransformCom->Check_TargetFront(XMLoadFloat4(&vPos)))
			Set_State(HURTFRONT);
		else
			Set_State(HURTBACK);
		break;
	case Client::CCreture::TYPE_SPIN:
		XMStoreFloat3(&m_vTargetLook, XMVector3Normalize(XMLoadFloat4(&vPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
		XMStoreFloat3(&m_vPushLook, XMVector3Normalize(XMLoadFloat3(&vDir)));
		Set_State(HITSPIN);
		m_pTransformCom->ResetAndJump(m_fAirPower);
		break;
	case Client::CCreture::TYPE_AWAY:
		XMStoreFloat3(&m_vTargetLook, XMVector3Normalize(XMLoadFloat4(&vPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
		XMStoreFloat3(&m_vPushLook, XMVector3Normalize(XMLoadFloat3(&vDir)));
		Set_State(HITAWAY);
		m_pTransformCom->ResetAndJump(m_fAirPower);
		break;
	case Client::CCreture::TYPE_UP:
		break;
	case Client::CCreture::TYPE_FASTDOWN:
		XMStoreFloat3(&m_vTargetLook, XMVector3Normalize(XMLoadFloat4(&vPos) - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
		XMStoreFloat3(&m_vPushLook, XMVector3Normalize(XMLoadFloat3(&vDir)));
		Set_State(FALLDOWN);
		break;
	}
}

void CPlayer03::Die_Sound()
{
	_uint iRandomVoice = GI->Get_Random(0, 2);

	switch (iRandomVoice)
	{
	case 0:
		GI->PlaySoundW(L"Hero04_ba_74.wav", SD_PLAYER3_VO, 1.f);
		break;
	case 1:
		GI->PlaySoundW(L"Hero04_ba_75.wav", SD_PLAYER3_VO, 1.f);
		break;
	case 2:
		GI->PlaySoundW(L"Hero04_ba_76.wav", SD_PLAYER3_VO, 1.f);
		break;
	default:
		break;
	}
}

_bool CPlayer03::Get_IsIdle()
{
	if (m_eCurState == IDLE1 || m_eCurState == IDLE2)
		return true;

	return false;
}

_bool CPlayer03::Get_PreUp()
{
	if (m_fPreHp < m_fNowHp)
		return true;

	return false;
}

_bool CPlayer03::Get_PreDown()
{
	if (m_fPreHp > m_fNowHp)
		return true;

	return false;
}

void CPlayer03::Set_State(STATE eState)
{
	if (m_eNextState == eState)
		return;

	m_eNextState = eState;

	switch (m_eNextState)
	{
	case Client::CPlayer03::NORMALRUN:
		break;
	case Client::CPlayer03::NORMALWALK:
		break;
	case Client::CPlayer03::AIRATTACK1:
		break;
	case Client::CPlayer03::AIRATTACK2:
		
		break;
	case Client::CPlayer03::AIRATTACK2_END:
		break;
	case Client::CPlayer03::AIRATTACK2_LOOF:
		break;
	case Client::CPlayer03::AIRATTACK2_LOOFSTART:
		break;
	case Client::CPlayer03::AIRATTACK2_START:
		m_bLGunRender = true;
		m_bRGunRender = true;
		m_bAirAttack2LoofCol_1 = false;
		m_bAirAttack2LoofCol_2 = false;
		m_eHitType = TYPE_HIT;
		m_fPushPower = 0.2f;
		m_fAirPower = 0.f;
		m_eHE = HE_1;
		Change_ColRange(_float3{ 2.f,3.f,5.f }, _float3{ 0.f,-1.f,5.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER03) * 0.1f;
		break;
	case Client::CPlayer03::ATTACK1:
		m_bLGunRender = true;
		m_bRGunRender = true;
		m_bAttack1Col = false;
		m_eHitType = TYPE_HIT;
		m_eHE = HE_1;
		m_fPushPower = 1.f;
		m_fAirPower = 0.f;
		Change_ColRange(_float3{ 2.f,1.5f,6.f }, _float3{ 0.f,1.5f,6.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER03) * 0.6f;
		break;
	case Client::CPlayer03::ATTACK2:
		m_bAttack2Col = false;
		m_eHitType = TYPE_HIT;
		m_fPushPower = 1.f;
		m_fAirPower = 0.f;
		Change_ColRange(_float3{ 2.f,1.5f,6.f }, _float3{ 0.f,1.5f,6.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER03) * 0.6f;
		break;
	case Client::CPlayer03::ATTACK3:
		m_bAttack3Col = false;
		m_eHitType = TYPE_HIT;
		m_fPushPower = 2.f;
		m_fAirPower = 0.f;
		Change_ColRange(_float3{ 2.f,1.5f,6.f }, _float3{ 0.f,1.5f,6.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER03) * 0.4f;
		break;
	case Client::CPlayer03::ATTACK4:
		m_bAttack4Col_1 = false; m_bAttack4Col_2 = false; m_bAttack4Col_3 = false;
		m_bAttack4Col_4 = false; m_bAttack4Col_5 = false; m_bAttack4Col_6 = false;
		m_eHitType = TYPE_HIT;
		m_fPushPower = 0.5f;
		m_fAirPower = 0.f;
		Change_ColRange(_float3{ 3.f,1.5f,3.f }, _float3{ 0.f,1.5f,0.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER03) * 0.2f;
		break;
	case Client::CPlayer03::ATTACK5:
		m_bAttack5Col = false;
		m_eHitType = TYPE_SPIN;
		m_fPushPower = 3.f;
		m_fAirPower = 5.f;
		Change_ColRange(_float3{ 2.f,1.5f,6.f }, _float3{ 0.f,1.5f,6.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER03) * 1.5f;
		break;
	case Client::CPlayer03::BOUNCE:
		m_pTransformCom->ResetAndJump(1.f);
		break;
	case Client::CPlayer03::BREAK:
		break;
	case Client::CPlayer03::BROKEN:
		break;
	case Client::CPlayer03::BURST:
		GI->PlaySoundW(L"UI_BP_full.wav", SD_UI, 0.3f);
		GI->PlaySoundW(L"Hero04_ba_50.wav", SD_PLAYER3_VO, 1.f);
		((CPlayer03HairA*)m_Parts[PARTS_HAIRA])->Set_PlayerLightColor(_float3{ 0.f,1.f,0.9f });
		((CPlayer03HairB*)m_Parts[PARTS_HAIRB])->Set_PlayerLightColor(_float3{ 0.f,1.f,0.9f });
		m_bVoid = true;
		m_bRGunRender = false;
		break;
	case Client::CPlayer03::GROGGYEND:
		break;
	case Client::CPlayer03::GROGGYLOOF:
		break;
	case Client::CPlayer03::GROGGYSTART:
		Die_Sound();
		m_bVoid = true;
		break;
	case Client::CPlayer03::DOWN:
		break;
	case Client::CPlayer03::VOIDFRONT:
		break;
	case Client::CPlayer03::VOIDAIR:
		break;
	case Client::CPlayer03::VOIDATTACK:
		break;
	case Client::CPlayer03::RAGESKILL1:
		m_bVoid = true;
		break;
	case Client::CPlayer03::FINISHNORMAL:
		break;
	case Client::CPlayer03::FINISHWALL:
		break;
	case Client::CPlayer03::GETUP:
		m_bVoid = true;
		break;
	case Client::CPlayer03::SKILL1:
		m_bLGunRender = true;
		m_bRGunRender = true;
		m_bSkill1Col_1 = false;
		m_bSkill1Col_2 = false;
		m_bSkill1Col_3 = false;
		m_eHitType = TYPE_HIT;
		m_fPushPower = 2.f;
		m_fAirPower = 0.f;
		m_eHE = HE_3;
		Change_ColRange(_float3{ 4.f,1.5f,4.f }, _float3{ 4.f,1.5f,0.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER03);
		break;
	case Client::CPlayer03::HURTBACK:
		break;
	case Client::CPlayer03::HURTFLY:
		break;
	case Client::CPlayer03::HURTFRONT:
		break;
	case Client::CPlayer03::IDLE1:
		m_bLGunRender = false;
		m_bRGunRender = false;
		m_eHE = HE_1;
		break;
	case Client::CPlayer03::IDLE2:
		m_bLGunRender = true;
		m_bRGunRender = true;
		m_eHE = HE_1;
		break;
	case Client::CPlayer03::IDLE3:
		break;
	case Client::CPlayer03::JUMPLOOF:
		m_bLGunRender = true;
		m_bRGunRender = true;
		break;
	case Client::CPlayer03::JUMPSTART:
		GI->PlaySoundW(L"se_heels_landing_asphalt_01.wav", SD_PLAYER3_SK, 0.8f);
		GI->PlaySoundW(L"Hero04_ba_16.wav", SD_PLAYER3_VO, 1.f);
		m_bLGunRender = true;
		m_bRGunRender = true;
		break;
	case Client::CPlayer03::JUMPEND:
		break;
	case Client::CPlayer03::QICKSTAND:
		break;
	case Client::CPlayer03::RUN:
		m_fRunSpeed = 5.f;
		m_bLGunRender = true;
		m_bRGunRender = true;
		break;
	case Client::CPlayer03::SKILL2:
		GI->PlaySoundW(L"se_ba_skill.wav", SD_PLAYER3_SK, 0.6f);
		m_bLGunRender = true;
		m_bRGunRender = true;
		m_bSkill2Col_1 = false; m_bSkill2Col_2 = false; m_bSkill2Col_3 = false; m_bSkill2Col_4 = false; m_bSkill2Col_5 = false;
		m_bSkill2Col_6 = false; m_bSkill2Col_7 = false; m_bSkill2Col_8 = false; m_bSkill2Col_9 = false; 
		m_eHitType = TYPE_HIT;
		m_eHE = HE_1;
		m_fPushPower = 0.1f;
		m_fAirPower = 0.f;
		Change_ColRange(_float3{ 2.f,1.5f,10.f }, _float3{ 0.f,1.5f,10.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER03) * 0.2f;
		Create_Effect(ET_SKILLCHARGE);
		break;
	case Client::CPlayer03::SKILL3:
		GI->PlaySoundW(L"se_ba_skill.wav", SD_PLAYER3_SK, 0.6f);
		m_bLGunRender = false;
		m_bRGunRender = false;
		m_eHE = HE_1;
		m_bSkill3On = false;
		Create_Effect(ET_SKILLCHARGE);
		break;
	case Client::CPlayer03::SKILL3AIR:
		break;
	case Client::CPlayer03::SKILL4:
		break;
	case Client::CPlayer03::SKILL5END:
		break;
	case Client::CPlayer03::SKILL5LOOF:
		break;
	case Client::CPlayer03::SKILL5START:
		GI->PlaySoundW(L"se_ba_skill.wav", SD_PLAYER3_SK, 0.6f);
		GI->PlaySoundW(L"Hero04_ba_34.wav", SD_PLAYER1_VO, 1.f);
		m_bSkill5StartCol = false;
		m_bSkill5LoofCol_1 = false;	m_bSkill5LoofCol_2 = false;	m_bSkill5LoofCol_3 = false;
		m_bSkill5EndCol_1 = false;	m_bSkill5EndCol_2 = false;
		m_eHitType = TYPE_HIT;
		m_fAirPower = 0.f;
		m_fPushPower = 0.5f;
		m_fSkill5Speed = 20.f;
		Change_ColRange(_float3{ 5.f,3.f,5.f }, _float3{ 5.f,3.f,5.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER03) * 0.5f;
		m_eHE = HE_3;
		break;
	case Client::CPlayer03::SKILL6:
		GI->PlaySoundW(L"se_ba_skill.wav", SD_PLAYER3_SK, 0.6f);
		m_bLGunRender = false;
		m_bRGunRender = false;
		m_bCannonRender = true;
		m_bSkill6Col_1 = false;
		m_bSkill6Col_2 = false;
		m_bSkill6Col_3 = false;
		m_eHitType = TYPE_HIT;
		m_fPushPower = 2.f;
		m_fAirPower = 0.f;
		Change_ColRange(_float3{ 2.f,1.5f,2.f }, _float3{ 0.f,1.5f,2.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER03) * 0.2f;
		Create_Effect(ET_SKILLCHARGE);
		m_eHE = HE_3;
		break;
	case Client::CPlayer03::SKILL7END:
		break;
	case Client::CPlayer03::SKILL7SHOT:
		break;
	case Client::CPlayer03::SKILL7LOOF:
		break;
	case Client::CPlayer03::SKILL7START:
		m_iSkill7Count = 0;
		m_bLGunRender = false;
		m_bRGunRender = false;
		m_bSniperEnd = false;
		m_eHitType = TYPE_SPIN;
		m_fPushPower = 4.f;
		m_fAirPower = 4.f;
		Change_ColRange(_float3{ 3.f,5.f,40.f }, _float3{ 0.f,5.f,40.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER03) * 3.f;
		m_eHE = HE_2;
		break;
	case Client::CPlayer03::SKILL8:
		break;
	case Client::CPlayer03::SLAPDOWN:
		break;
	case Client::CPlayer03::SKILL9:
		m_bLGunRender = true;
		m_bRGunRender = false;
		m_bShotGunRender = true;
		m_bSkill9Col = false;
		m_eHitType = TYPE_AWAY;
		m_fPushPower = 30.f;
		m_fAirPower = 5.f;
		m_eHE = HE_3;
		Change_ColRange(_float3{ 3.f,1.5f,6.f }, _float3{ 0.f,1.5f,6.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER03) * 1.2f;
		m_fMotionTrailAcc = 0.f;
		m_bMotionTrail = true;
	
		break;
	case Client::CPlayer03::SKILL10END:
		break;
	case Client::CPlayer03::SKILL10LOOF:
		break;
	case Client::CPlayer03::SKILL10START:
		break;
	case Client::CPlayer03::SKILL11:
		m_bLGunRender = true;
		m_bRGunRender = true;
		m_bSkill11Col_1 = false;
		m_bSkill11Col_2 = false;
		m_eHitType = TYPE_AWAY;
		m_fPushPower = 3.f;
		m_fAirPower = 0.f;
		m_eHE = HE_3;
		Change_ColRange(_float3{ 2.f,2.5f,2.f }, _float3{ 0.f,2.5f,2.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER03) * 0.4f;
		break;
	case Client::CPlayer03::SKILL12:
		break;
	case Client::CPlayer03::SKILL13:
		m_bLGunRender = true;
		m_bRGunRender = true;
		m_bSkill13Col = false;
		m_eHitType = TYPE_AWAY;
		m_fPushPower = 20.f;
		m_fAirPower = 3.f;
		m_eHE = HE_3;
		Change_ColRange(_float3{ 3.f,1.5f,4.f }, _float3{ 0.f,1.5f,4.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER03);
		break;
	case Client::CPlayer03::RUNFASTEND:
		break;
	case Client::CPlayer03::RUNFASTLOOF:
		break;
	case Client::CPlayer03::RUNFASTSTART:
		GI->PlaySoundW(L"UI_Dash.wav", SD_PLAYER3_SK, 0.4f);
		m_fRunSpeed = 8.f;
		Create_Effect(ET_DASH);
		m_bLGunRender = false;
		m_bRGunRender = false;
		break;
	case Client::CPlayer03::STEPBACK:
		m_bVoid = true;
		m_bShotGunRender = false;
		m_bLShotGunRender = false;
		m_bLCannonRender = false;
		m_bCannonRender = false;
		m_bSniperRender = false;
		m_bLGunRender = true;
		m_bRGunRender = true;
	/*	m_vTargetLook = GI->Get_CamDir(CPipeLine::DIR_LOOK);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&m_vTargetLook));*/
		Get_Colliders("Player_HairCut")->Set_Sleep();
		break;
	case Client::CPlayer03::WALKLEFT:
		break;
	case Client::CPlayer03::WALKRIGHT:
		break;
	case Client::CPlayer03::STUN:
		break;
	case Client::CPlayer03::STUNEND:
		break;
	case Client::CPlayer03::RAGESKILL2:
		UM->Set_FrontUI(false);
		GI->PlaySoundW(L"UI_Burst_st.wav", SD_UI, 0.3f);
		GI->PlaySoundW(L"Hero04_ba_52.wav", SD_PLAYER1_VO, 1.f);
		GI->PlaySoundW(L"se_Event_ShotPic.wav", SD_PLAYER03_RE, 0.6f); //update에서 넣어주면 이거 빼기
		m_bBurstSkillOn = true;
		m_bVoid = true;
		m_bRageSkill2Col_1 = false;	m_bRageSkill2Col_2 = false;	m_bRageSkill2Col_3 = false;	m_bRageSkill2Col_4 = false;
		m_bRageSkill2Col_5 = false;	m_bRageSkill2Col_6 = false;	m_bRageSkill2Col_7 = false;	m_bRageSkill2Col_8 = false;
		m_bRageSkill2Col_9 = false;	m_bRageSkill2Col_10 = false; m_bRageSkill2Col_11 = false; m_bRageSkill2Col_12 = false;
		m_bRageSkill2Col_13 = false; m_bRageSkill2Col_14 = false; m_bRageSkill2Trail = false;
		m_bLGunRender = true;
		m_bRGunRender = true;
		m_bPatternSkill = true;
		m_eHitType = TYPE_HIT;
		m_eHE = HE_1;
		m_fPushPower = 0.1f;
		m_fAirPower = 0.f;
		Change_ColRange(_float3{ 5.f,2.5f,5.f }, _float3{ 0.f,2.5f,0.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER03) * 0.6f;

		m_bHeroTest_1 = false;
		m_bHeroTest_2 = false;
		m_bHeroTest_3 = false;

		break;
	case Client::CPlayer03::FALLDOWN:
		break;
	case Client::CPlayer03::HITAWAY:
		break;
	case Client::CPlayer03::HITSPIN:
		break;
	case Client::CPlayer03::HITDOWN:
		break;
	}

	m_pModelCom->SetNextIndex(m_eNextState);
	m_pModelCom->SetChangeBool(true);
	m_eCurState = m_eNextState;

}

void CPlayer03::Set_Dir(DIR eDir)
{
	m_eDir = eDir;

	_matrix View = XMLoadFloat4x4(&GI->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW));
	_vector vCamLook = View.r[2];

	_vector vCamRight = XMVector3Normalize(XMVector3Cross(_vector{ 0.f,1.f,0.f }, vCamLook));

	vCamLook = XMVector3Normalize(XMVector3Cross(vCamRight, _vector{ 0.f,1.f,0.f }));

	switch (m_eDir)
	{
	case Client::CPlayer03::DIR_UP:
		XMStoreFloat3(&m_vTargetLook, (vCamLook));
		break;
	case Client::CPlayer03::DIR_DOWN:
		XMStoreFloat3(&m_vTargetLook, (vCamLook * -1.f));
		break;
	case Client::CPlayer03::DIR_RIGHT:
		XMStoreFloat3(&m_vTargetLook, (vCamRight));
		break;
	case Client::CPlayer03::DIR_LEFT:
		XMStoreFloat3(&m_vTargetLook, (vCamRight * -1.f));
		break;
	case Client::CPlayer03::DIR_LU:
		XMStoreFloat3(&m_vTargetLook, XMVector3Normalize(vCamLook - vCamRight));
		break;
	case Client::CPlayer03::DIR_RU:
		XMStoreFloat3(&m_vTargetLook, XMVector3Normalize(vCamLook + vCamRight));
		break;
	case Client::CPlayer03::DIR_LD:
		XMStoreFloat3(&m_vTargetLook, XMVector3Normalize(vCamLook * -1.f + vCamRight * -1.f));
		break;
	case Client::CPlayer03::DIR_RD:
		XMStoreFloat3(&m_vTargetLook, XMVector3Normalize(vCamLook * -1.f + vCamRight));
		break;
	}
}

void CPlayer03::End_Animation()
{
	if (m_pModelCom->GetAniEnd())
	{
		switch (m_eCurState)
		{
		case Client::CPlayer03::NORMALRUN:
			break;
		case Client::CPlayer03::NORMALWALK:
			break;
		case Client::CPlayer03::AIRATTACK1:
			m_pModelCom->Set_AnimIndex(JUMPLOOF);
			m_eCurState = JUMPLOOF;
			break;
		case Client::CPlayer03::AIRATTACK2:
			Set_State(JUMPLOOF);
			break;
		case Client::CPlayer03::AIRATTACK2_END:
			m_bAirOn = false;
			m_pModelCom->Set_AnimIndex(JUMPLOOF);
			m_eCurState = JUMPLOOF;
			m_bAirAttack2LoofCol_1 = false;
			m_bAirAttack2LoofCol_2 = false;
			break;
		case Client::CPlayer03::AIRATTACK2_LOOF:
			m_bAirAttack2LoofCol_1 = false;
			m_bAirAttack2LoofCol_2 = false;
			break;
		case Client::CPlayer03::AIRATTACK2_LOOFSTART:
			m_pModelCom->Set_AnimIndex(AIRATTACK2_LOOF);
			m_eCurState = AIRATTACK2_LOOF;
			m_bAirAttack2LoofCol_1 = false;
			m_bAirAttack2LoofCol_2 = false;
			m_eHitType = TYPE_HIT;
			m_fPushPower = 0.2f;
			m_fAirPower = 0.f;
			break;
		case Client::CPlayer03::AIRATTACK2_START:
			m_pModelCom->Set_AnimIndex(AIRATTACK2_LOOFSTART);
			m_eCurState = AIRATTACK2_LOOFSTART;
			break;
		case Client::CPlayer03::ATTACK1:
			Set_State(IDLE2);
			break;
		case Client::CPlayer03::ATTACK2:
			Set_State(IDLE2);
			break;
		case Client::CPlayer03::ATTACK3:
			Set_State(IDLE2);
			break;
		case Client::CPlayer03::ATTACK4:
			Set_State(IDLE2);
			break;
		case Client::CPlayer03::ATTACK5:
			Set_State(IDLE2);
			break;
		case Client::CPlayer03::BOUNCE:
			Set_State(GETUP);
			break;
		case Client::CPlayer03::BREAK:
			break;
		case Client::CPlayer03::BROKEN:
			break;
		case Client::CPlayer03::BURST:
			m_bVoid = false;
			Set_State(IDLE2);
			m_bRGunRender = true;
			m_bHairChange = false;
			m_vPlayerLightColor = { 1.f,1.f,1.f };
			((CPlayer03HairA*)m_Parts[PARTS_HAIRA])->Set_PlayerLightColor(_float3{ 1.f,1.f,1.f });
			((CPlayer03HairB*)m_Parts[PARTS_HAIRB])->Set_PlayerLightColor(_float3{ 1.f,1.f,1.f });
			break;
		case Client::CPlayer03::GROGGYEND:
			break;
		case Client::CPlayer03::GROGGYLOOF:
			break;
		case Client::CPlayer03::GROGGYSTART:
			Player_Die();
			break;
		case Client::CPlayer03::DOWN:
			break;
		case Client::CPlayer03::VOIDFRONT:
			GI->PlaySoundW(L"se_heels_landing_asphalt_01.wav", SD_PLAYER3_SK, 0.8f);
			m_bVoidOn = false;
			m_bVoid = false;
			Set_State(IDLE2);
			break;
		case Client::CPlayer03::VOIDAIR:
			break;
		case Client::CPlayer03::VOIDATTACK:
			Set_State(IDLE2);
			break;
		case Client::CPlayer03::RAGESKILL1:
			m_bVoid = false;
			break;
		case Client::CPlayer03::FINISHNORMAL:
			break;
		case Client::CPlayer03::FINISHWALL:
			break;
		case Client::CPlayer03::GETUP:
			m_bVoid = false;
			Set_State(IDLE2);
			break;
		case Client::CPlayer03::SKILL1:
			Set_State(IDLE2);
			break;
		case Client::CPlayer03::HURTBACK:
			Set_State(IDLE2);
			break;
		case Client::CPlayer03::HURTFLY:
			Set_State(IDLE2);
			break;
		case Client::CPlayer03::HURTFRONT:
			Set_State(IDLE2);
			break;
		case Client::CPlayer03::IDLE1:
			break;
		case Client::CPlayer03::IDLE2:
			break;
		case Client::CPlayer03::IDLE3:
			break;
		case Client::CPlayer03::JUMPLOOF:
			break;
		case Client::CPlayer03::JUMPSTART:
			m_pModelCom->Set_AnimIndex(JUMPLOOF);
			m_eCurState = JUMPLOOF;
			break;
		case Client::CPlayer03::JUMPEND:
			GI->PlaySoundW(L"se_heels_landing_asphalt_02.wav", SD_PLAYER3_SK, 0.8f);
			Set_State(IDLE1);
			break;
		case Client::CPlayer03::QICKSTAND:
			break;
		case Client::CPlayer03::RUN:
			break;
		case Client::CPlayer03::SKILL2:
			Set_State(IDLE2);
			break;
		case Client::CPlayer03::SKILL3:
			m_bSkill3On = false;
			Set_State(IDLE2);
			break;
		case Client::CPlayer03::SKILL3AIR:
			Set_State(JUMPLOOF);
			break;
		case Client::CPlayer03::SKILL4:
			break;
		case Client::CPlayer03::SKILL5END:
			Set_State(IDLE2);
			break;
		case Client::CPlayer03::SKILL5LOOF:		
			m_pModelCom->Set_AnimIndex(SKILL5END);
			m_eCurState = SKILL5END;
			m_eNextState = SKILL5END;
			m_pModelCom->SetNextIndex(SKILL5END);
			m_bSkill5EndCol_1 = false;
			m_bSkill5EndCol_2 = false;				
			m_bAttackCol = true;
			break;
		case Client::CPlayer03::SKILL5START:
			m_pModelCom->Set_AnimIndex(SKILL5LOOF);
			m_eCurState = SKILL5LOOF;
			m_eNextState = SKILL5LOOF;
			m_pModelCom->SetNextIndex(SKILL5LOOF);
			m_bSkill5LoofCol_1 = false;
			m_bSkill5LoofCol_2 = false;
			m_bSkill5LoofCol_3 = false;
			break;
		case Client::CPlayer03::SKILL6:
			
			Set_State(IDLE2);
			break;
		case Client::CPlayer03::SKILL7END:
			Set_State(IDLE2);
			break;
		case Client::CPlayer03::SKILL7SHOT:		
			if (!m_bSniperEnd)
			{
				if (m_iSkill7Count == 3)
				{
					Set_State(SKILL7END);		
					m_bSniperEnd = true;
					m_bSniperOn = false;
					
				}
				else
				{
					Set_State(SKILL7LOOF);
				}
			}
			break;
		case Client::CPlayer03::SKILL7LOOF:
			
			break;
		case Client::CPlayer03::SKILL7START:
			m_pModelCom->Set_AnimIndex(SKILL7LOOF);
			m_eCurState = SKILL7LOOF;
			break;
		case Client::CPlayer03::SKILL8:
			break;
		case Client::CPlayer03::SLAPDOWN:
			break;
		case Client::CPlayer03::SKILL9:
			m_bSkill9On = false;
			Set_State(IDLE2);
			m_bRGunRender = true;
			m_bShotGunRender = false;
			Update_Parts();
			break;
		case Client::CPlayer03::SKILL10END:
			break;
		case Client::CPlayer03::SKILL10LOOF:
			break;
		case Client::CPlayer03::SKILL10START:
			break;
		case Client::CPlayer03::SKILL11:
			break;
		case Client::CPlayer03::SKILL12:
			Set_State(IDLE2);
			break;
		case Client::CPlayer03::SKILL13:
			Set_State(IDLE2);
			break;
		case Client::CPlayer03::RUNFASTEND:
			if (m_ePlayerMode == PMODE_NORMAL)
				Set_State(IDLE1);
			else
				Set_State(IDLE2);
			break;
		case Client::CPlayer03::RUNFASTLOOF:
			break;
		case Client::CPlayer03::RUNFASTSTART:
			m_pModelCom->Set_AnimIndex(RUNFASTLOOF);
			m_eCurState = RUNFASTLOOF;
			break;
		case Client::CPlayer03::STEPBACK:
			m_bSelectPlayer = false;
			((CPlayer03HairA*)m_Parts[PARTS_HAIRA])->Set_Sleep();
			break;
		case Client::CPlayer03::WALKLEFT:
			break;
		case Client::CPlayer03::WALKRIGHT:
			break;
		case Client::CPlayer03::STUN:
			break;
		case Client::CPlayer03::STUNEND:
			break;
		case Client::CPlayer03::RAGESKILL2:
			UM->Set_FrontUI(true);
			m_bBurstSkillOn = false;
			m_bPatternSkill = false;
			m_bVoid = false;
			Set_State(IDLE2);
			break;
		case Client::CPlayer03::FALLDOWN:
			break;
		case Client::CPlayer03::HITAWAY:
			Set_State(FALLDOWN);
			break;
		case Client::CPlayer03::HITSPIN:
			Set_State(FALLDOWN);
			break;
		case Client::CPlayer03::HITDOWN:
			m_pModelCom->Set_AnimIndex(BOUNCE);
			m_eCurState = BOUNCE;
			m_eNextState = m_eCurState;
			m_pModelCom->SetNextIndex(m_eNextState);
			break;
		}
	}
}

void CPlayer03::Get_KeyInput(_float fTimeDelta)
{
	m_bDirKeyInput = false;

	if (!m_pModelCom->GetChangeBool())
	{
		switch (m_eCurState)
		{
		case Client::CPlayer03::NORMALRUN:
			break;
		case Client::CPlayer03::NORMALWALK:
			break;
		case Client::CPlayer03::AIRATTACK1:
			break;
		case Client::CPlayer03::AIRATTACK2:
			break;
		case Client::CPlayer03::AIRATTACK2_END:
			break;
		case Client::CPlayer03::AIRATTACK2_LOOF:
			AirAttack2Loof_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer03::AIRATTACK2_LOOFSTART:
			break;
		case Client::CPlayer03::AIRATTACK2_START:
			break;
		case Client::CPlayer03::ATTACK1:
			Attack1_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer03::ATTACK2:
			Attack2_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer03::ATTACK3:
			Attack3_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer03::ATTACK4:
			Attack4_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer03::ATTACK5:
			Attack5_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer03::BOUNCE:
			break;
		case Client::CPlayer03::BREAK:
			break;
		case Client::CPlayer03::BROKEN:
			break;
		case Client::CPlayer03::BURST:
			break;
		case Client::CPlayer03::GROGGYEND:
			break;
		case Client::CPlayer03::GROGGYLOOF:
			break;
		case Client::CPlayer03::GROGGYSTART:
			break;
		case Client::CPlayer03::DOWN:
			break;
		case Client::CPlayer03::VOIDFRONT:
			break;
		case Client::CPlayer03::VOIDAIR:
			break;
		case Client::CPlayer03::VOIDATTACK:
			break;
		case Client::CPlayer03::RAGESKILL1:
			break;
		case Client::CPlayer03::FINISHNORMAL:
			break;
		case Client::CPlayer03::FINISHWALL:
			break;
		case Client::CPlayer03::GETUP:
			break;
		case Client::CPlayer03::SKILL1:
			break;
		case Client::CPlayer03::HURTBACK:
			break;
		case Client::CPlayer03::HURTFLY:
			break;
		case Client::CPlayer03::HURTFRONT:
			break;
		case Client::CPlayer03::IDLE1:
			Idle_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer03::IDLE2:
			Idle_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer03::IDLE3:
			break;
		case Client::CPlayer03::JUMPLOOF:
			JumpLoof_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer03::JUMPSTART:
			break;
		case Client::CPlayer03::JUMPEND:
			break;
		case Client::CPlayer03::QICKSTAND:
			break;
		case Client::CPlayer03::RUN:
			Run_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer03::SKILL2:
			break;
		case Client::CPlayer03::SKILL3:
			break;
		case Client::CPlayer03::SKILL3AIR:
			break;
		case Client::CPlayer03::SKILL4:
			break;
		case Client::CPlayer03::SKILL5END:
			break;
		case Client::CPlayer03::SKILL5LOOF:
			break;
		case Client::CPlayer03::SKILL5START:
			break;
		case Client::CPlayer03::SKILL6:
			break;
		case Client::CPlayer03::SKILL7END:
			break;
		case Client::CPlayer03::SKILL7SHOT:
			break;
		case Client::CPlayer03::SKILL7LOOF:
			Skill7Loof_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer03::SKILL7START:
			break;
		case Client::CPlayer03::SKILL8:
			break;
		case Client::CPlayer03::SLAPDOWN:
			break;
		case Client::CPlayer03::SKILL9:
			break;
		case Client::CPlayer03::SKILL10END:
			break;
		case Client::CPlayer03::SKILL10LOOF:
			break;
		case Client::CPlayer03::SKILL10START:
			break;
		case Client::CPlayer03::SKILL11:
			break;
		case Client::CPlayer03::SKILL12:
			break;
		case Client::CPlayer03::SKILL13:
			break;
		case Client::CPlayer03::RUNFASTEND:
			break;
		case Client::CPlayer03::RUNFASTLOOF:
			Run_FastInput(fTimeDelta);
			break;
		case Client::CPlayer03::RUNFASTSTART:
			Run_FastStartInput(fTimeDelta);
			break;
		case Client::CPlayer03::STEPBACK:
			break;
		case Client::CPlayer03::WALKLEFT:
			break;
		case Client::CPlayer03::WALKRIGHT:
			break;
		case Client::CPlayer03::STUN:
			break;
		case Client::CPlayer03::STUNEND:
			break;
		case Client::CPlayer03::RAGESKILL2:
			break;
		case Client::CPlayer03::FALLDOWN:
			break;
		case Client::CPlayer03::HITAWAY:
			break;
		case Client::CPlayer03::HITSPIN:
			break;
		case Client::CPlayer03::HITDOWN:
			break;
		}
	}

}

void CPlayer03::Update(_float fTimeDelta)
{
	m_bRangeCol = true;

	if (!m_pModelCom->GetChangeBool())
	{
		switch (m_eCurState)
		{
		case Client::CPlayer03::NORMALRUN:
			break;
		case Client::CPlayer03::NORMALWALK:
			break;
		case Client::CPlayer03::AIRATTACK1:
			AirAttack1_Update(fTimeDelta);
			break;
		case Client::CPlayer03::AIRATTACK2:
			AirAttack2_Update(fTimeDelta);
			break;
		case Client::CPlayer03::AIRATTACK2_END:
			break;
		case Client::CPlayer03::AIRATTACK2_LOOF:
			AirAttack2Loof_Update(fTimeDelta);
			break;
		case Client::CPlayer03::AIRATTACK2_LOOFSTART:
			AirAttack2LoofStart_Update(fTimeDelta);
			break;
		case Client::CPlayer03::AIRATTACK2_START:
			AirAttack2Start_Update(fTimeDelta);
			break;
		case Client::CPlayer03::ATTACK1:
			Attack1_Update(fTimeDelta);
			break;
		case Client::CPlayer03::ATTACK2:
			Attack2_Update(fTimeDelta);
			break;
		case Client::CPlayer03::ATTACK3:
			Attack3_Update(fTimeDelta);
			break;
		case Client::CPlayer03::ATTACK4:
			Attack4_Update(fTimeDelta);
			break;
		case Client::CPlayer03::ATTACK5:
			Attack5_Update(fTimeDelta);
			break;
		case Client::CPlayer03::BOUNCE:
			break;
		case Client::CPlayer03::BREAK:
			break;
		case Client::CPlayer03::BROKEN:
			break;
		case Client::CPlayer03::BURST:
			Burst_Update(fTimeDelta);
			break;
		case Client::CPlayer03::GROGGYEND:
			break;
		case Client::CPlayer03::GROGGYLOOF:
			break;
		case Client::CPlayer03::GROGGYSTART:
			break;
		case Client::CPlayer03::DOWN:
			break;
		case Client::CPlayer03::VOIDFRONT:
			VoidFront_Update(fTimeDelta);
			break;
		case Client::CPlayer03::VOIDAIR:
			break;
		case Client::CPlayer03::VOIDATTACK:
			VoidAttack_Update(fTimeDelta);
			break;
		case Client::CPlayer03::RAGESKILL1:
			break;
		case Client::CPlayer03::FINISHNORMAL:
			break;
		case Client::CPlayer03::FINISHWALL:
			break;
		case Client::CPlayer03::GETUP:
			break;
		case Client::CPlayer03::SKILL1:
			Skill1_Update(fTimeDelta);
			break;
		case Client::CPlayer03::HURTBACK:
			HurtBack_Update(fTimeDelta);
			break;
		case Client::CPlayer03::HURTFLY:
			break;
		case Client::CPlayer03::HURTFRONT:
			HurtFront_Update(fTimeDelta);
			break;
		case Client::CPlayer03::IDLE1:
			break;
		case Client::CPlayer03::IDLE2:
			break;
		case Client::CPlayer03::IDLE3:
			break;
		case Client::CPlayer03::JUMPLOOF:
			JumpLoof_Update(fTimeDelta);
			break;
		case Client::CPlayer03::JUMPSTART:
			JumpStart_Update(fTimeDelta);
			break;
		case Client::CPlayer03::JUMPEND:
			break;
		case Client::CPlayer03::QICKSTAND:
			break;
		case Client::CPlayer03::RUN:
			Run_Update(fTimeDelta);
			break;
		case Client::CPlayer03::SKILL2:
			Skill2_Update(fTimeDelta);
			break;
		case Client::CPlayer03::SKILL3:
			Skill3_Update(fTimeDelta);
			break;
		case Client::CPlayer03::SKILL3AIR:
			Skill3Air_Update(fTimeDelta);
			break;
		case Client::CPlayer03::SKILL4:
			Skill4_Update(fTimeDelta);
			break;
		case Client::CPlayer03::SKILL5END:
			Skill5End_Update(fTimeDelta);
			break;
		case Client::CPlayer03::SKILL5LOOF:
			Skill5Loof_Update(fTimeDelta);
			break;
		case Client::CPlayer03::SKILL5START:
			Skill5Start_Update(fTimeDelta);
			break;
		case Client::CPlayer03::SKILL6:
			Skill6_Update(fTimeDelta);
			break;
		case Client::CPlayer03::SKILL7END:
			Skill7End_Update(fTimeDelta);
			break;
		case Client::CPlayer03::SKILL7SHOT:
			Skill7Shot_Update(fTimeDelta);
			break;
		case Client::CPlayer03::SKILL7LOOF:
			Skill7Loof_Update(fTimeDelta);
			break;
		case Client::CPlayer03::SKILL7START:
			Skill7Start_Update(fTimeDelta);
			break;
		case Client::CPlayer03::SKILL8:
			Skill8_Update(fTimeDelta);
			break;
		case Client::CPlayer03::SLAPDOWN:
			SlapDown_Update(fTimeDelta);
			break;
		case Client::CPlayer03::SKILL9:
			Skill9_Update(fTimeDelta);
			break;
		case Client::CPlayer03::SKILL10END:
			Skill10End_Update(fTimeDelta);
			break;
		case Client::CPlayer03::SKILL10LOOF:
			Skill10Loof_Update(fTimeDelta);
			break;
		case Client::CPlayer03::SKILL10START:
			Skill10Start_Update(fTimeDelta);
			break;
		case Client::CPlayer03::SKILL11:
			Skill11_Update(fTimeDelta);
			break;
		case Client::CPlayer03::SKILL12:
			Skill12_Update(fTimeDelta);
			break;
		case Client::CPlayer03::SKILL13:
			Skill13_Update(fTimeDelta);
			break;
		case Client::CPlayer03::RUNFASTEND:
			RunFastEnd_Update(fTimeDelta);
			break;
		case Client::CPlayer03::RUNFASTLOOF:
			RunFastLoof_Update(fTimeDelta);
			break;
		case Client::CPlayer03::RUNFASTSTART:
			RunFastStart_Update(fTimeDelta);
			break;
		case Client::CPlayer03::STEPBACK:
			StepBack_Update(fTimeDelta);
			break;
		case Client::CPlayer03::WALKLEFT:
			break;
		case Client::CPlayer03::WALKRIGHT:
			break;
		case Client::CPlayer03::STUN:
			Stun_Update(fTimeDelta);
			break;
		case Client::CPlayer03::STUNEND:
			StunEnd_Update(fTimeDelta);
			break;
		case Client::CPlayer03::RAGESKILL2:
			RageSkill2_Update(fTimeDelta);
			break;
		case Client::CPlayer03::FALLDOWN:
			FallDown_Update(fTimeDelta);
			break;
		case Client::CPlayer03::HITAWAY:
			HitAway_Update(fTimeDelta);
			break;
		case Client::CPlayer03::HITSPIN:
			HitSpin_Update(fTimeDelta);
			break;
		case Client::CPlayer03::HITDOWN:
			HitDown_Update(fTimeDelta);
			break;
		}
	}
}

void CPlayer03::LateTick_Func(_float fTimeDelta)
{
	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&m_vTargetLook), 0.2f);

	m_pModelCom->Play_Animation(fTimeDelta, m_pTransformCom, m_pNavigationCom);

	m_pTransformCom->Tick_Gravity(fTimeDelta, m_pNavigationCom, m_fGravity, true);

	Update_Parts();

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	End_Animation();

	Push_Colliders();

	Render_Parts(fTimeDelta);
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);

	for (auto& iter : m_MotionTrails)
		iter->LateTick(fTimeDelta);

	for (auto& iter : m_PointTrails)
		iter->LateTick(fTimeDelta);

	Render_Col(m_pRendererCom);
}

void CPlayer03::Push_Colliders()
{
	if (m_bAttackCol)
		Choose_Col("Player_Attack");

	if (m_bHurt && !m_bVoid)
		Choose_Col("Player_Body");
	
	if(m_bRangeCol)
		Choose_Col("Player_Push_Range");
}

void CPlayer03::Change_Mode()
{
	if (CK_DOWN(DIK_J))
	{
		m_ePlayerMode = PMODE_BATTLE;
		Update_Parts();
		((CPlayer03HairA*)m_Parts[PARTS_HAIRA])->Set_Sleep();
		Get_Colliders("Player_HairCut")->Set_Sleep();
		UM->Set_BurstOn(false);
		m_bPointTrail = false;
	}

	if (CK_DOWN(DIK_K))
	{
		Set_State(BURST);
	}
}

void CPlayer03::Change_ColRange(_float3 vSize, _float3 vCenter)
{
	Get_Colliders("Player_Attack")->Set_Size(vSize);
	Get_Colliders("Player_Attack")->Set_Center(vCenter);
}

void CPlayer03::Hit_State(CCreture * pOther)
{
	m_fPushPower = pOther->Get_PushPower();
	m_fAirPower = pOther->Get_AirPower();
	m_fAirPower = 8.f;

	Reset_Collider();
	Reset_Weapon();

	m_fNowHp -= (pOther->Get_Damage() - STM->Get_FinalStamina(CStatus_Manager::PLAYER03));
	if (m_fNowHp <= 0.f)
	{
		m_fNowHp = 0.f;
		Set_State(GROGGYSTART);
		return;
	}

	m_bRim = true;

	UM->Set_ShakeOn2(true);

	switch (pOther->Get_HitType())
	{
	case TYPE_HIT:
		if (m_pTransformCom->Get_Air())
		{
			CRM->Get_Cam()->Shake((m_fPushPower * 0.01f + m_fAirPower * 0.01f), 0.2f);
			
			return;
		}

		if (m_eCurState != HURTFLY)
		{
			if (m_pTransformCom->Check_TargetFront(XMLoadFloat4(&pOther->Get_WorldPos())))
				Set_State(HURTFRONT);
			else
				Set_State(HURTBACK);
			CRM->Get_Cam()->Shake(0.06f, 0.15f);
		}
		
		break;

	case TYPE_AWAY:
	{
		switch (pOther->Get_PushType())
		{
		case PUSH:
		{
			_vector vMonDir = XMVector3Normalize(XMLoadFloat3(&(pOther)->Get_Dir(_float3(0.f, 0.f, 1.f))));
			XMStoreFloat3(&m_vTargetLook, vMonDir * -1.f);
			XMStoreFloat3(&m_vPushLook, vMonDir);
		}
		break;
		case PULL:
		{
			_vector vMonDir = XMVector3Normalize(XMLoadFloat3(&(pOther)->Get_Dir(_float3(0.f, 0.f, 1.f))));
			XMStoreFloat3(&m_vTargetLook, vMonDir * -1.f);
			XMStoreFloat3(&m_vPushLook, vMonDir * -1.f);
		}
		break;
		case TARGET:
		{
			_vector vMonDir = XMVector3Normalize(XMLoadFloat3(&(pOther)->Get_Dir(_float3(0.f, 0.f, 1.f))));
			XMStoreFloat3(&m_vTargetLook, vMonDir * -1.f);
			XMStoreFloat3(&m_vPushLook, XMVector3Normalize(XMLoadFloat3(&pOther->Get_PushLook())));
		}
		break;
		}

		CRM->Get_Cam()->Shake((m_fPushPower * 0.01f + m_fAirPower * 0.01f), 0.2f);
		Set_State(HITAWAY);
		m_pTransformCom->ResetAndJump(m_fAirPower);

	}
	break;

	case TYPE_UP:
		Set_State(HITDOWN);
		break;

	case TYPE_SPIN:
	{
		switch (pOther->Get_PushType())
		{
		case PUSH:
		{
			_vector vMonDir = XMVector3Normalize(XMLoadFloat3(&(pOther)->Get_Dir(_float3(0.f, 0.f, 1.f))));
			XMStoreFloat3(&m_vTargetLook, vMonDir * -1.f);
			XMStoreFloat3(&m_vPushLook, vMonDir);
		}
		break;
		case PULL:
		{
			_vector vMonDir = XMVector3Normalize(XMLoadFloat3(&(pOther)->Get_Dir(_float3(0.f, 0.f, 1.f))));
			XMStoreFloat3(&m_vTargetLook, vMonDir * -1.f);
			XMStoreFloat3(&m_vPushLook, vMonDir * -1.f);
		}
		break;
		case TARGET:
		{
			_vector vMonDir = XMVector3Normalize(XMLoadFloat3(&(pOther)->Get_Dir(_float3(0.f, 0.f, 1.f))));
			XMStoreFloat3(&m_vTargetLook, vMonDir * -1.f);
			XMStoreFloat3(&m_vPushLook, XMVector3Normalize(XMLoadFloat3(&pOther->Get_PushLook())));
		}
		break;
		}

		CRM->Get_Cam()->Shake((m_fPushPower * 0.01f + m_fAirPower * 0.01f), 0.2f);
		Set_State(HITSPIN);
		m_pTransformCom->ResetAndJump(m_fAirPower);

	}
	break;
	}
}

void CPlayer03::Hit_Effect(CCreture * pOther)
{
	_float4 vPos;
	XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	vPos.y += 1.f;
	pOther->Create_HitEffect(vPos);
}

void CPlayer03::Reset_Collider()
{
	m_bAttackCol = false;
	m_bAirAttack1Col = false;
	m_bAirAttack2Col_1 = false;
	m_bAirAttack2Col_2 = false;
	m_bAirAttack2LoofCol_1 = false;
	m_bAirAttack2LoofCol_2 = false;
	m_bAttack1Col = false;
	m_bAttack2Col = false;
	m_bAttack3Col = false;
	m_bAttack4Col_1 = false;
	m_bAttack4Col_2 = false;
	m_bAttack4Col_3 = false;
	m_bAttack4Col_4 = false;
	m_bAttack4Col_5 = false;
	m_bAttack4Col_6 = false;
	m_bAttack5Col = false;
	m_bSkill1Col_1 = false;
	m_bSkill1Col_2 = false;
	m_bSkill1Col_3 = false;
	m_bSkill2Col_1 = false;
	m_bSkill2Col_2 = false;
	m_bSkill2Col_3 = false;
	m_bSkill2Col_4 = false;
	m_bSkill2Col_5 = false;
	m_bSkill2Col_6 = false;
	m_bSkill2Col_7 = false;
	m_bSkill2Col_8 = false;
	m_bSkill2Col_9 = false;
	m_bSkill5StartCol = false;
	m_bSkill5LoofCol_1 = false;
	m_bSkill5LoofCol_2 = false;
	m_bSkill5LoofCol_3 = false;
	m_bSkill5EndCol_1 = false;
	m_bSkill5EndCol_2 = false;
	m_bSkill6Col_1 = false;
	m_bSkill6Col_2 = false;
	m_bSkill6Col_3 = false;
	m_bSkill9Col = false;
	m_bSkill11Col_1 = false;
	m_bSkill11Col_2 = false;
	m_bSkill12Col_1 = false;
	m_bSkill12Col_2 = false;
	m_bSkill12Col_3 = false;
	m_bSkill12Col_4 = false;
	m_bSkill12Col_5 = false;
	m_bSkill13Col = false;
	m_bRageSkill2Col_1 = false;
	m_bRageSkill2Col_2 = false;
	m_bRageSkill2Col_3 = false;
	m_bRageSkill2Col_4 = false;
	m_bRageSkill2Col_5 = false;
	m_bRageSkill2Col_6 = false;
	m_bRageSkill2Col_7 = false;
	m_bRageSkill2Col_8 = false;
	m_bRageSkill2Col_9 = false;
	m_bRageSkill2Col_10 = false;
	m_bRageSkill2Col_11 = false;
	m_bRageSkill2Col_12 = false;
	m_bRageSkill2Col_13 = false;
	m_bRageSkill2Col_14 = false;
	m_bVoidAttackCol_1 = false;
	m_bVoidAttackCol_2 = false;
	m_bVoidAttackCol_3 = false;
	m_bVoidAttackCol_4 = false;
}

void CPlayer03::Check_Burst(_float fTimeDelta, _float OriginTimeDelta)
{
	if (m_fPreEp < m_fNowEP)
	{
		m_fPreEp += 10.f * fTimeDelta;

		if (m_fPreEp >= m_fNowEP)
		{
			m_fPreEp = m_fNowEP;
		}
	}

	if (m_fPreEp == m_fNowEP)
	{
		if (m_ePlayerMode == PMODE_BURST)
		{
			m_fBurstParticleAcc += 1.f * OriginTimeDelta;
			if (m_fBurstParticleAcc >= m_fBurstParticleLimit)
			{
				Create_ParticleInstance(PTI_BURST);
				m_fBurstParticleAcc = 0.f;
			}
			m_fNowEP -= 0.5f * fTimeDelta;
			m_fPreEp -= 0.5f * fTimeDelta;
			m_bMotionTrail = true;
			if (m_fNowEP < 0.f)
			{
				m_fNowEP = 0.f;
				m_fPreEp = 0.f;
				m_fNowBP = 0.f;
				m_ePlayerMode = PMODE_BATTLE;
				Update_Parts();
				((CPlayer03HairA*)m_Parts[PARTS_HAIRA])->Set_Sleep();
				Get_Colliders("Player_HairCut")->Set_Sleep();
				m_bPointTrail = false;
			}
		}
		else if (m_fNowEP < m_fMaxEp)
		{
			m_fNowEP += 5.f * fTimeDelta;
			m_fPreEp += 5.f * fTimeDelta;

			if (m_fNowEP > m_fMaxEp)
			{
				m_fNowEP = m_fMaxEp;
				m_fPreEp = m_fMaxEp;
			}
		}
	}
}

void CPlayer03::Check_Rim(_float fTimeDelta)
{
	if (m_bRim)
	{
		m_fRimAcc += 1.f * fTimeDelta;
		if (m_fRimAcc >= 0.2f)
		{
			m_fRimAcc = 0.f;
			m_bRim = false;
		}
	}
}

HRESULT CPlayer03::Set_RenderPass(SHADER eShader)
{
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	switch (eShader)
	{
	case Client::CPlayer03::SHADER_DEFAULT:
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
				return E_FAIL;
		}
		break;
	case Client::CPlayer03::SHADER_RIM:
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pShaderCom->Set_RawValue("g_CamPosition", &GI->Get_CamPosition(), sizeof(_float4))))
				return E_FAIL;
			_float g_RimPower = 1.5f;
			if (FAILED(m_pShaderCom->Set_RawValue("g_RimPower", &g_RimPower, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 1)))
				return E_FAIL;
		}
		break;
	case Client::CPlayer03::SHADER_LIGHT:
	{
		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			_float Alpha = 1.f;
			if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &Alpha, sizeof(_float))))
				return E_FAIL;

			_float Blur = 1.f;
			if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &Blur, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Set_RawValue("g_vPlayerLightColor", &m_vPlayerLightColor, sizeof(_float3))))
				return E_FAIL;

			if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 11)))
				return E_FAIL;
		}
	}
	}
	

	return S_OK;
}

void CPlayer03::Reset_Weapon()
{
	m_bLCannonRender = false;
	m_bCannonRender = false;
	m_bSniperRender = false;
	m_bShotGunRender = false;
	m_bLShotGunRender = false;
	m_bLGunRender = true;
	m_bRGunRender = true;
}

void CPlayer03::Check_HP(_float fTimeDelta)
{
	if (m_bReHp)
	{
		m_fNowHp += 300.f * fTimeDelta;
		m_fPreHp += 300.f * fTimeDelta;

		if (m_fPreHp >= STM->Get_FinalHp(CStatus_Manager::PLAYER03))
		{
			m_fPreHp = STM->Get_FinalHp(CStatus_Manager::PLAYER03);
			m_fNowHp = m_fPreHp;
			m_bReHp = false;
		}
	}

	if (m_fPreHp < m_fNowHp)
	{
		m_fPreHp += 400.f * fTimeDelta;

		if (m_fPreHp >= m_fNowHp)
		{
			m_fPreHp = m_fNowHp;
		}
	}

	else if (m_fPreHp > m_fNowHp)
	{
		m_fPreHp -= 200.f * fTimeDelta;

		if (m_fPreHp <= m_fNowHp)
		{
			m_fPreHp = m_fNowHp;
		}
	}
}

void CPlayer03::Check_SP(_float fTimeDelta)
{
	if (m_fPreSp < m_fNowSP)
	{
		m_fPreSp += 15.f * fTimeDelta;

		if (m_fPreSp >= m_fNowSP)
		{
			m_fPreSp = m_fNowSP;
		}
	}

	if (!m_bSPUse)
	{
		if (CK_DOWN(DIK_X))
		{
			m_bSPUse = true;
			Set_SlowMotionTrail();
			return;
		}

		if (m_fPreSp == m_fNowSP)
		{
			m_fNowSP += 10.f * (fTimeDelta);
			m_fPreSp += 10.f * (fTimeDelta);
			if (m_fNowSP > m_fMaxSp)
			{
				m_fNowSP = m_fMaxSp;
				m_fPreSp = m_fMaxSp;
			}
		}
	}
	else
	{
		TIM->Fix_All(0.2f, 100.f);
		if (CK_DOWN(DIK_X))
		{
			m_bSPUse = false;
			TIM->Reset_All();
			Reset_SlowMotionTrail();
			return;
		}

		if (m_fPreSp == m_fNowSP)
		{
			m_fNowSP -= 10.f * fTimeDelta;
			m_fPreSp -= 10.f * fTimeDelta;
			m_bMotionTrail = true;
			if (m_fNowSP < 0.f)
			{
				m_bSPUse = false;
				m_fNowSP = 0.f;
				m_fPreSp = 0.f;
				TIM->Reset_All();
				Reset_SlowMotionTrail();
			}
		}
	}
}

void CPlayer03::Check_BP(_float fTimeDelta)
{
	if (m_ePlayerMode == PMODE_BURST && !m_bBurstSkillOn)
	{
		m_fNowBP += 5.f;

		if (m_fNowBP > 100.f)
			m_fNowBP = 100.f;

	}
}

bool CPlayer03::Input_Direction()
{
	if (CK_PRESS(DIK_W))
	{
		Set_Dir(DIR_UP);
		m_bDirKeyInput = true;
		if (CK_PRESS(DIK_D))
		{
			Set_Dir(DIR_RU);
			m_bDirKeyInput = true;
		}
		if (CK_PRESS(DIK_A))
		{
			Set_Dir(DIR_LU);
			m_bDirKeyInput = true;
		}
	}
	else if (CK_PRESS(DIK_S))
	{
		Set_Dir(DIR_DOWN);
		m_bDirKeyInput = true;
		if (CK_PRESS(DIK_D))
		{
			Set_Dir(DIR_RD);
			m_bDirKeyInput = true;
		}
		if (CK_PRESS(DIK_A))
		{
			Set_Dir(DIR_LD);
			m_bDirKeyInput = true;
		}

	}
	else if (CK_PRESS(DIK_D))
	{
		Set_Dir(DIR_RIGHT);
		m_bDirKeyInput = true;
	}
	else if (CK_PRESS(DIK_A))
	{
		Set_Dir(DIR_LEFT);
		m_bDirKeyInput = true;
	}

	return m_bDirKeyInput;
}

void CPlayer03::Default_KeyInput(_float fTimeDelta)
{
	Change_Mode();

	if (CM_DOWN(DIMK_LBUTTON))
	{
		Set_State(ATTACK1);
		return;
	}

	if (CK_DOWN(DIK_Q))
	{
		Set_State(SKILL1);
		return;
	}

	if (CK_DOWN(DIK_E))
	{
		Set_State(SKILL2);
		return;
	}

	if (CK_DOWN(DIK_F))
	{
		Set_State(SKILL3);
		return;
	}

	if (CK_DOWN(DIK_R))
	{
		Set_State(SKILL5START);
		return;
	}

	if (CK_DOWN(DIK_T))
	{
		Set_State(SKILL7START);
		return;
	}

	if (CK_DOWN(DIK_G))
	{
		Set_State(SKILL11);
		return;
	}

	if (CK_DOWN(DIK_1))
	{
		Set_State(SKILL9);
		return;
	}

	if (CK_DOWN(DIK_2))
	{
		Set_State(SKILL13);
		return;
	}

	if (CK_DOWN(DIK_TAB))
	{
		Set_State(SKILL6);
		return;
	}

	if (CK_DOWN(DIK_B))
	{
		Set_State(RAGESKILL2);
		return;
	}

	if (CK_DOWN(DIK_CAPSLOCK))
	{
		m_bVoid = true;
		m_bRGunRender = true;
		m_bLGunRender = true;
		m_fVoidSpeed = 10.f;
		m_pModelCom->Set_AnimIndex(VOIDFRONT);
		m_eCurState = VOIDFRONT;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
		return;
	}
}

void CPlayer03::AirAttack1_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() < m_pModelCom->Get_TimeLimit(0))
		m_pTransformCom->ResetGravity();

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bAirAttack1Col)
	{
		GI->PlaySoundW(L"Hero04_ba_06.wav", SD_PLAYER3_VO, 1.f);
		GI->PlaySoundW(L"se_HE04_Special01_2.wav", SD_PLAYER3_SK, 0.8f);
		CRM->Get_Cam()->Shake(0.2f, 0.2f, false, 0.2f);
		CRM->Get_Cam()->Fov(0.5f, 0.2f, 0.f);
		m_pTransformCom->ResetAndJump(4.f);
		m_bAttackCol = true;
		m_bAirAttack1Col = true;
		PTM->Create_AlphaParticle(L"Player03AirAttack1A", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03AirAttack1Bullet", m_pTransformCom->Get_WorldFloat4x4());
		_matrix	RHandMatrix = m_Sockets[SOCKET_WEAPON_R]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
		RHandMatrix.r[2] *= -1.f;
		_float4x4 Matrix;
		XMStoreFloat4x4(&Matrix, RHandMatrix);
		PTM->Create_InstanceParticle(L"Player03AirAttack", Matrix);
		PTM->Create_InstanceParticle(L"Player03ShotGunParticle", Matrix);
	}

	if (m_pModelCom->Get_KeyFrame() > m_pModelCom->Get_TimeLimit(0))
	{
		m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook) * -1.f, m_fAirAttack1Speed, fTimeDelta, m_pNavigationCom);
	}
}

void CPlayer03::AirAttack2_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() < m_pModelCom->Get_TimeLimit(0))
		m_pTransformCom->ResetGravity();

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bAirAttack2Col_1)
	{
		GI->PlaySoundW(L"se_HE04_Nattack_1_04.wav", SD_PLAYER3_SK, 0.8f);
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		m_bAttackCol = true;
		m_bAirAttack2Col_1 = true;
		m_pTransformCom->ResetAndJump(1.5f);
		PTM->Create_AlphaParticle(L"Player03AirAttack2LoofA", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03AirAttack2LoofB", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03AirAttack2LoofBullet", m_pTransformCom->Get_WorldFloat4x4());
		Create_ParticleInstance(PTI_LGUNPARTICLE);
		
	}
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bAirAttack2Col_2)
	{
		m_bAttackCol = true;
		m_bAirAttack2Col_2 = true;
		m_pTransformCom->ResetAndJump(1.5f);
		PTM->Create_AlphaParticle(L"Player03AirAttack2LoofA_2", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03AirAttack2LoofB_2", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03AirAttack2LoofBullet_2", m_pTransformCom->Get_WorldFloat4x4());
		Create_ParticleInstance(PTI_RGUNPARTICLE);
	}
	if (m_pModelCom->Get_KeyFrame() > m_pModelCom->Get_TimeLimit(0))
	{
		m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook) * -1.f, m_fAirAttack2Speed, fTimeDelta, m_pNavigationCom);
	}
}

void CPlayer03::AirAttack2End_Update(_float fTimeDelta)
{
}

void CPlayer03::AirAttack2Loof_Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	Input_Direction();
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bAirAttack2LoofCol_1)
	{
		if (!m_bAirOn)
		{
			GI->PlaySoundW(L"Hero04_ba_06.wav", SD_PLAYER3_VO, 1.f);
			m_bAirOn = true;
		}
		GI->PlaySoundW(L"se_HE04_Hattack_2.wav", SD_PLAYER3_SK, 0.6f);
		CRM->Get_Cam()->Shake(0.05f, 0.02f, false, 0.05f);
		m_bAttackCol = true;
		m_bAirAttack2LoofCol_1 = true;
		m_pTransformCom->ResetAndJump(0.f);
		PTM->Create_AlphaParticle(L"Player03AirAttack2LoofA", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03AirAttack2LoofB", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03AirAttack2LoofBullet", m_pTransformCom->Get_WorldFloat4x4());
		Create_ParticleInstance(PTI_RGUNPARTICLE);
	
	}
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bAirAttack2LoofCol_2)
	{
		GI->PlaySoundW(L"se_HE04_Hattack_2.wav", SD_PLAYER3_SK, 0.6f);
		CRM->Get_Cam()->Shake(0.05f, 0.02f, false, 0.05f);
		m_bAttackCol = true;
		m_bAirAttack2LoofCol_2 = true;
		m_pTransformCom->ResetAndJump(0.f);
		PTM->Create_AlphaParticle(L"Player03AirAttack2LoofA_2", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03AirAttack2LoofB_2", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03AirAttack2LoofBullet_2", m_pTransformCom->Get_WorldFloat4x4());
		Create_ParticleInstance(PTI_LGUNPARTICLE);
	}
}

void CPlayer03::AirAttack2LoofStart_Update(_float fTimeDelta)
{
	m_pTransformCom->ResetGravity();
}

void CPlayer03::AirAttack2Start_Update(_float fTimeDelta)
{
	m_pTransformCom->ResetGravity();
}

void CPlayer03::Attack1_Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bAttack1Col)
	{
		GI->PlaySoundW(L"Hero04_ba_02.wav", SD_PLAYER1_VO, 1.f);
		GI->PlaySoundW(L"se_HE04_Nattack_1_04.wav", SD_PLAYER3_SK, 0.4f);
		CRM->Get_Cam()->Shake(0.15f, 0.1f, false, 0.1f);
		m_bAttackCol = true;
		m_bAttack1Col = true;
		Create_Particle(PT_ATTACK1);
		Create_ParticleInstance(PTI_ATTACK1);
		Create_ParticleInstance(PTI_RGUNPARTICLE);
		
	}
}

void CPlayer03::Attack2_Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bAttack2Col)
	{
		GI->PlaySoundW(L"se_HE04_Nattack_1_03.wav", SD_PLAYER3_SK, 0.6f);
		CRM->Get_Cam()->Shake(0.15f, 0.1f, false, 0.1f);
		m_bAttackCol = true;
		m_bAttack2Col = true;
		Create_Particle(PT_ATTACK2);
		Create_ParticleInstance(PTI_ATTACK2);
		Create_ParticleInstance(PTI_LGUNPARTICLE);
	
	}
}

void CPlayer03::Attack3_Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bAttack3Col)
	{
		GI->PlaySoundW(L"Hero04_ba_01.wav", SD_PLAYER1_VO, 1.f);
		GI->PlaySoundW(L"se_HE04_Nattack_1_02.wav", SD_PLAYER3_SK, 0.6f);
		CRM->Get_Cam()->Shake(0.15f, 0.1f, false, 0.1f);
		m_bAttackCol = true;
		m_bAttack3Col = true;
		Create_Particle(PT_ATTACK3);
		Create_ParticleInstance(PTI_ATTACK3);
		Create_ParticleInstance(PTI_RGUNPARTICLE);
		Create_ParticleInstance(PTI_LGUNPARTICLE);

	
	}
}

void CPlayer03::Attack4_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bAttack4Col_1)
	{
		GI->PlaySoundW(L"Hero04_ba_05.wav", SD_PLAYER1_VO, 1.f);
		GI->PlaySoundW(L"se_HE04_Nattack_3.wav", SD_PLAYER3_SK, 0.6f);
		m_bAttackCol = true;
		m_bAttack4Col_1 = true;
		PTM->Create_AlphaParticle(L"Player03Attack4B_2", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4B", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4A", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4A_2", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet_2", m_pTransformCom->Get_WorldFloat4x4());
		Create_ParticleInstance(PTI_LGUNPARTICLE);
		Create_ParticleInstance(PTI_RGUNPARTICLE);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bAttack4Col_2)
	{
		m_bAttackCol = true;
		m_bAttack4Col_2 = true;
		PTM->Create_AlphaParticle(L"Player03Attack4B_3", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4B_4", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4A_3", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4A_4", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet_3", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet_4", m_pTransformCom->Get_WorldFloat4x4());
		Create_ParticleInstance(PTI_LGUNPARTICLE);
		Create_ParticleInstance(PTI_RGUNPARTICLE);
	
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(3) && !m_bAttack4Col_3)
	{
		m_bAttackCol = true;
		m_bAttack4Col_3 = true;
		PTM->Create_AlphaParticle(L"Player03Attack4B_5", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4B_6", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4A_5", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4A_6", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet_5", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet_6", m_pTransformCom->Get_WorldFloat4x4());
		Create_ParticleInstance(PTI_LGUNPARTICLE);
		Create_ParticleInstance(PTI_RGUNPARTICLE);
	
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(4) && !m_bAttack4Col_4)
	{
		m_bAttackCol = true;
		m_bAttack4Col_4 = true;
		PTM->Create_AlphaParticle(L"Player03Attack4B", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4B_7", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4A", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4A_7", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet_7", m_pTransformCom->Get_WorldFloat4x4());
		Create_ParticleInstance(PTI_LGUNPARTICLE);
		Create_ParticleInstance(PTI_RGUNPARTICLE);
	
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(5) && !m_bAttack4Col_5)
	{
		
		m_bAttackCol = true;
		m_bAttack4Col_5 = true;
		PTM->Create_AlphaParticle(L"Player03Attack4B_4", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4B_8", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4A_4", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4A_8", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet_4", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet_8", m_pTransformCom->Get_WorldFloat4x4());
		Create_ParticleInstance(PTI_LGUNPARTICLE);
		Create_ParticleInstance(PTI_RGUNPARTICLE);
	
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(6) && !m_bAttack4Col_6)
	{
		m_bAttackCol = true;
		m_bAttack4Col_6 = true;
		PTM->Create_AlphaParticle(L"Player03Attack4B_5", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4B_6", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4A_5", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4A_6", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet_5", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet_6", m_pTransformCom->Get_WorldFloat4x4());
		Create_ParticleInstance(PTI_LGUNPARTICLE);
		Create_ParticleInstance(PTI_RGUNPARTICLE);
	
	}
}

void CPlayer03::Attack5_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bAttack5Col)
	{
		GI->PlaySoundW(L"Hero04_ba_03.wav", SD_PLAYER1_VO, 1.f);
		GI->PlaySoundW(L"se_HE04_Nattack_2.wav", SD_PLAYER3_SK, 0.6f);
		CRM->Get_Cam()->Shake(0.2f, 0.2f, false, 0.1f);
		CRM->Get_Cam()->Fov(-1.f, 0.1f, 0.f);
		m_bAttackCol = true;
		m_bAttack5Col = true;
		PTM->Create_AlphaParticle(L"Player03Attack5B", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack5B_2", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack5A", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack5A_2", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Attack5Bullet", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Attack5Bullet_2", m_pTransformCom->Get_WorldFloat4x4());
		Create_ParticleInstance(PTI_LGUNPARTICLE);
		Create_ParticleInstance(PTI_RGUNPARTICLE);

	}
}

void CPlayer03::Bounce_Update(_float fTimeDelta)
{
}

void CPlayer03::Broken_Update(_float fTimeDelta)
{
}

void CPlayer03::Burst_Update(_float fTimeDelta)
{
	m_bHairChange = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bHairChange)
	{

		m_bHairChange = true;
		m_ePlayerMode = PMODE_BURST;
		m_bPointTrail = true;
		Update_Parts();
		((CPlayer03HairA*)m_Parts[PARTS_HAIRA])->Set_Wake();
		((CPlayer03HairA*)m_Parts[PARTS_HAIRA])->SetInitPos();
		Get_Colliders("Player_HairCut")->Set_Awake();
		((CPlayer03HairA*)m_Parts[PARTS_HAIRA])->Set_PlayerLightColor(_float3{ 0.f,1.f,0.9f });
		((CPlayer03HairB*)m_Parts[PARTS_HAIRB])->Set_PlayerLightColor(_float3{ 0.f,1.f,0.9f });
		m_eShaderPass = SHADER_LIGHT;
		m_bPlayerLight = true;
		m_fPlayerLightLimit = 0.1f;
		m_vPlayerLightColor = { 0.f,1.f,0.9f };
		PTM->Create_InstanceParticle(L"Player01BurstParticle", m_pTransformCom->Get_WorldFloat4x4());
		GI->PlaySoundW(L"HSChangeSound.ogg", SD_PLAYER1_SK, 1.f);
	}
}

void CPlayer03::GroggyEnd_Update(_float fTimeDelta)
{
}

void CPlayer03::GroggyLoof_Update(_float fTimeDelta)
{
}

void CPlayer03::GroggyStart_Update(_float fTimeDelta)
{
}

void CPlayer03::Down_Update(_float fTimeDelta)
{
}

void CPlayer03::VoidFront_Update(_float fTimeDelta)
{
	if (!m_bVoidOn)
	{
		GI->PlaySoundW(L"Track 02.wav", SD_PLAYER1_SK, 0.5f);
		m_bVoidOn = true;
		Create_Effect(ET_DASH);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0))
	{
		if (CM_PRESS(DIMK_LBUTTON))
		{
			m_bVoid = false;
			Input_Direction();
			m_pModelCom->Set_AnimIndex(VOIDATTACK);
			Change_ColRange(_float3{ 3.f,1.5f,3.f }, _float3{ 0.f,1.5f,0.f });
			m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER03) * 0.2f;
			m_eCurState = VOIDATTACK;
			m_eNextState = m_eCurState;
			m_pModelCom->SetNextIndex(m_eNextState);
			m_bVoidAttackCol_1 = false; m_bVoidAttackCol_2 = false; m_bVoidAttackCol_3 = false; m_bVoidAttackCol_4 = false;
			m_eHitType = TYPE_HIT;
			m_fPushPower = 0.5f;
			m_fAirPower = 0.f;
			m_bLGunRender = true;
			m_bRGunRender = true;
			m_fVoidAttackSpeed = 10.f;
			if (m_bVoidOn)
			{
				GI->PlaySoundW(L"Hero04_ba_31.wav", SD_PLAYER1_VO, 1.f);
				m_bVoidOn = false;
			}
		}
	}

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(1)
		&& m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(0))
	{
		if (m_fVoidSpeed > 0.f)
		{
			m_fVoidSpeed -= 20.f * fTimeDelta;
			if (m_fVoidSpeed < 0.f)
				m_fVoidSpeed = 0.f;
		}

		m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), m_fVoidSpeed, fTimeDelta, m_pNavigationCom);
	}
}

void CPlayer03::VoidAir_Update(_float fTimeDelta)
{
}

void CPlayer03::VoidAttack_Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	m_bVoidOn = false;

	if (m_fVoidAttackSpeed >= 0.f)
		m_fVoidAttackSpeed -= 20.f * fTimeDelta; 
	 
	if (m_fVoidAttackSpeed <= 0.f)
		m_fVoidAttackSpeed = 0.f;

	if (m_pModelCom->Get_KeyFrame() < m_pModelCom->Get_TimeLimit(4))
	{
		m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), m_fVoidAttackSpeed, fTimeDelta, m_pNavigationCom);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bVoidAttackCol_1)
	{
		GI->PlaySoundW(L"se_HE04_Nattack_3.wav", SD_PLAYER3_SK, 0.6f);
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		m_bVoidAttackCol_1 = true;
		m_bAttackCol = true;
		PTM->Create_AlphaParticle(L"Player03Attack4B_2", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4B", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4A", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4A_2", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet_2", m_pTransformCom->Get_WorldFloat4x4());
		Create_ParticleInstance(PTI_LGUNPARTICLE);
		Create_ParticleInstance(PTI_RGUNPARTICLE);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bVoidAttackCol_2)
	{
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		m_bVoidAttackCol_2 = true;
		m_bAttackCol = true;
		PTM->Create_AlphaParticle(L"Player03Attack4B_3", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4B_4", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4A_3", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4A_4", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet_3", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet_4", m_pTransformCom->Get_WorldFloat4x4());
		Create_ParticleInstance(PTI_LGUNPARTICLE);
		Create_ParticleInstance(PTI_RGUNPARTICLE);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bVoidAttackCol_3)
	{
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		m_bVoidAttackCol_3 = true;
		m_bAttackCol = true;
		PTM->Create_AlphaParticle(L"Player03Attack4B_5", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4B_6", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4A_5", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4A_6", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet_5", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet_6", m_pTransformCom->Get_WorldFloat4x4());
		Create_ParticleInstance(PTI_LGUNPARTICLE);
		Create_ParticleInstance(PTI_RGUNPARTICLE);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(3) && !m_bVoidAttackCol_4)
	{
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		m_bVoidAttackCol_4 = true;
		m_bAttackCol = true;
		PTM->Create_AlphaParticle(L"Player03Attack4B", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4B_7", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4A", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4A_7", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet_7", m_pTransformCom->Get_WorldFloat4x4());
		Create_ParticleInstance(PTI_LGUNPARTICLE);
		Create_ParticleInstance(PTI_RGUNPARTICLE);
	}

}

void CPlayer03::RageSkill1_Update(_float fTimeDelta)
{
}

void CPlayer03::FinishNormal_Update(_float fTimeDelta)
{
}

void CPlayer03::FinishWall_Update(_float fTimeDelta)
{
}

void CPlayer03::GetUp_Update(_float fTimeDelta)
{
}

void CPlayer03::Skill1_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0))
	{
		m_bShotGunRender = true;
		m_bRGunRender = false;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1))
	{
		m_bShotGunRender = false;
		m_bRGunRender = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bSkill1Col_1)
	{
		GI->PlaySoundW(L"Hero04_ba_08.wav", SD_PLAYER3_VO, 1.f);
		GI->PlaySoundW(L"se_HE04_Hattack_1_01.wav", SD_PLAYER3_SK, 0.8f);
		CRM->Get_Cam()->Shake(0.1f, 0.2f, false, 0.1f);
		m_bSkill1Col_1 = true;
		m_bAttackCol = true;
		Create_Particle(PT_SKILL1_1);
		Create_ParticleInstance(PTI_SKILL1_1);
		Create_ParticleInstance(PTI_RSHOTGUN);
		Create_Effect(ET_RSHOTGUN);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(3) && !m_bSkill1Col_2)
	{
		GI->PlaySoundW(L"se_HE04_Hattack_1_02.wav", SD_PLAYER3_SK, 0.8f);
		CRM->Get_Cam()->Shake(0.1f, 0.2f, false, 0.1f);
		Change_ColRange(_float3{ 3.f,1.5f,3.f }, _float3{ -3.f,1.5f,0.f });
		m_bSkill1Col_2 = true;
		m_bAttackCol = true;
		Create_Particle(PT_SKILL1_2);
		Create_ParticleInstance(PTI_SKILL1_2);
		Create_ParticleInstance(PTI_RSHOTGUN);
		Create_Effect(ET_RSHOTGUN);
	}

	// 사이에 reload 넣어주기
	
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(4) && !m_bSkill1Col_3)
	{
		GI->PlaySoundW(L"se_HE04_Reload.wav", SD_PLAYER03_RE, 0.3f);
		GI->PlaySoundW(L"se_HE04_Hattack_1_02.wav", SD_PLAYER3_SK, 0.8f);
		CRM->Get_Cam()->Shake(0.1f, 0.2f, false, 0.1f);
		Change_ColRange(_float3{ 2.f,1.5f,3.f }, _float3{ 0.f,1.5f,3.f });
		m_eHitType = TYPE_SPIN;
		m_fAirPower = 7.f;
		m_fPushPower = 5.f;
		m_bSkill1Col_3 = true;
		m_bAttackCol = true;	
		Create_Particle(PT_SKILL13);
		Create_ParticleInstance(PTI_SKILL13);
		Create_ParticleInstance(PTI_RSHOTGUN);
		Create_Effect(ET_RSHOTGUN);
	}


}

void CPlayer03::HurtBack_Update(_float fTimeDelta)
{
	if (CK_DOWN(DIK_CAPSLOCK))
	{
		m_bVoid = true;
		m_bRGunRender = true;
		m_bLGunRender = true;
		m_fVoidSpeed = 10.f;
		m_pModelCom->Set_AnimIndex(VOIDFRONT);
		m_eCurState = VOIDFRONT;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
		return;
	}
}

void CPlayer03::HurtFly_Update(_float fTimeDelta)
{
}

void CPlayer03::HurtFront_Update(_float fTimeDelta)
{
	if (CK_DOWN(DIK_CAPSLOCK))
	{
		m_bVoid = true;
		m_bRGunRender = true;
		m_bLGunRender = true;
		m_fVoidSpeed = 10.f;
		m_pModelCom->Set_AnimIndex(VOIDFRONT);
		m_eCurState = VOIDFRONT;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
		return;
	}
}

void CPlayer03::Idle1_Update(_float fTimeDelta)
{
}

void CPlayer03::Idle2_Update(_float fTimeDelta)
{
}

void CPlayer03::Idle3_Update(_float fTimeDelta)
{
}

void CPlayer03::JumpLoof_Update(_float fTimeDelta)
{
	Input_Direction();
	if (!m_pTransformCom->Get_Air())
	{
		m_pModelCom->Set_AnimIndex(JUMPEND);
		m_eCurState = JUMPEND;
	}
	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), m_fJumpSpeed, fTimeDelta, m_pNavigationCom);
}

void CPlayer03::JumpStart_Update(_float fTimeDelta)
{
	Input_Direction();
	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), m_fJumpSpeed, fTimeDelta, m_pNavigationCom);
}

void CPlayer03::JumpEnd_Update(_float fTimeDelta)
{
}

void CPlayer03::QickStand_Update(_float fTimeDelta)
{
}

void CPlayer03::Run_Update(_float fTimeDelta)
{
	m_pTransformCom->Go_Dir(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_fRunSpeed, fTimeDelta, m_pNavigationCom);

	m_fRunSoundAcc += 1.f * fTimeDelta;

	if (!m_bOtherFoot)
	{
		if (m_fRunSoundAcc >= 0.3f)
		{
			GI->PlaySoundW(L"se_flat_run_s_asphalt_03.wav", SD_PLAYER3_SK, 0.7f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = true;
		}
	}

	else
	{
		if (m_fRunSoundAcc >= 0.3f)
		{
			GI->PlaySoundW(L"se_flat_run_s_asphalt_04.wav", SD_PLAYER3_SK, 0.7f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = false;
		}
	}
}

void CPlayer03::Skill2_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bSkill2Col_1)
	{
		GI->PlaySoundW(L"Hero04_ba_27.wav", SD_PLAYER1_VO, 1.f);
		GI->PlaySoundW(L"se_HE04_Nattack_1_01.wav", SD_PLAYER3_SK, 0.5f);
		CRM->Get_Cam()->Shake(0.06f, 0.8f, false, 0.1f);
		CRM->Get_Cam()->Fov(-0.2f, 0.8f, 0.f);
		m_bAttackCol = true;
		m_bSkill2Col_1 = true;
		Create_Particle(PT_SKILL2_1);
		Create_ParticleInstance(PTI_SKILL2_1BULLET);
		Create_ParticleInstance(PTI_RGUNPARTICLE);
		
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bSkill2Col_2)
	{
		GI->PlaySoundW(L"se_HE04_Nattack_1_02.wav", SD_PLAYER3_SK, 0.5f);
		m_bAttackCol = true;
		m_bSkill2Col_2 = true;
		Create_Particle(PT_SKILL2_2);
		Create_ParticleInstance(PTI_SKILL2_2BULLET);
		Create_ParticleInstance(PTI_LGUNPARTICLE);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bSkill2Col_3)
	{
		GI->PlaySoundW(L"se_HE04_Nattack_1_01.wav", SD_PLAYER3_SK, 0.5f);
		m_bAttackCol = true;
		m_bSkill2Col_3 = true;
		Create_Particle(PT_SKILL2_1);
		Create_ParticleInstance(PTI_SKILL2_1BULLET);
		Create_ParticleInstance(PTI_RGUNPARTICLE);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(3) && !m_bSkill2Col_4)
	{
		GI->PlaySoundW(L"se_HE04_Nattack_1_03.wav", SD_PLAYER3_SK, 0.5f);
		m_bAttackCol = true;
		m_bSkill2Col_4 = true;
		Create_Particle(PT_SKILL2_2);
		Create_ParticleInstance(PTI_SKILL2_2BULLET);
		Create_ParticleInstance(PTI_LGUNPARTICLE);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(4) && !m_bSkill2Col_5)
	{
		GI->PlaySoundW(L"se_HE04_Nattack_1_04.wav", SD_PLAYER3_SK, 0.5f);
		m_bAttackCol = true;
		m_bSkill2Col_5 = true;
		Create_Particle(PT_SKILL2_1);
		Create_ParticleInstance(PTI_SKILL2_1BULLET);
		Create_ParticleInstance(PTI_RGUNPARTICLE);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(5) && !m_bSkill2Col_6)
	{
		GI->PlaySoundW(L"se_HE04_Nattack_1_03.wav", SD_PLAYER3_SK, 0.5f);
		m_bAttackCol = true;
		m_bSkill2Col_6 = true;
		Create_Particle(PT_SKILL2_2);
		Create_ParticleInstance(PTI_SKILL2_2BULLET);
		Create_ParticleInstance(PTI_LGUNPARTICLE);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(6) && !m_bSkill2Col_7)
	{
		GI->PlaySoundW(L"se_HE04_Nattack_1_04.wav", SD_PLAYER3_SK, 0.5f);
		m_bAttackCol = true;
		m_bSkill2Col_7 = true;
		Create_Particle(PT_SKILL2_1);
		Create_ParticleInstance(PTI_SKILL2_1BULLET);
		Create_ParticleInstance(PTI_RGUNPARTICLE);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(7) && !m_bSkill2Col_8)
	{
		m_bAttackCol = true;
		m_bSkill2Col_8 = true;
		Create_Particle(PT_SKILL2_2);
		Create_ParticleInstance(PTI_SKILL2_2BULLET);
		Create_ParticleInstance(PTI_LGUNPARTICLE);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(8) && !m_bSkill2Col_9)
	{
		GI->PlaySoundW(L"se_HE04_Nattack_2.wav", SD_PLAYER3_SK, 0.7f);
		CRM->Get_Cam()->Shake(0.3f, 0.2f, false, 0.1f);
		CRM->Get_Cam()->Fov(1.f, 0.2f, 0.f);
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER03) * 0.8f;
		m_eHitType = TYPE_SPIN;
		m_fAirPower = 7.f;
		m_fPushPower = 5.f;
		m_bAttackCol = true;
		m_bSkill2Col_9 = true;
		Create_Particle(PT_SKILL2_2);
		Create_ParticleInstance(PTI_SKILL2_2BULLET);
		Create_Particle(PT_SKILL2_1);
		Create_ParticleInstance(PTI_SKILL2_1BULLET);
		Create_ParticleInstance(PTI_LGUNPARTICLE);
		Create_ParticleInstance(PTI_RGUNPARTICLE);
	}
}

void CPlayer03::Skill3_Update(_float fTimeDelta)
{
	//폭탄 투사체를 발사할 때
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bSkill3On)
	{
		m_bSkill3On = true;
		GI->PlaySoundW(L"Hero04_ba_30.wav", SD_PLAYER1_VO, 1.f);
		GI->PlaySoundW(L"se_HE04_Skill02_1.wav", SD_PLAYER3_SK, 0.7f);

		Create_Effect(ET_BOMB);

	


	}

	//폭탄 투사체가 터질 때
	if (m_pModelCom->Get_KeyFrame() == 60 && !m_bRGunRender && !m_bLGunRender)
	{
	
		m_bRGunRender = true;
		m_bLGunRender = true;
	}
}

void CPlayer03::Skill3Air_Update(_float fTimeDelta)
{
}

void CPlayer03::Skill4_Update(_float fTimeDelta)
{
}

void CPlayer03::Skill5End_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_fSkill5Speed >= 0.f)
		m_fSkill5Speed -= 15.f * fTimeDelta;
	if (m_fSkill5Speed <= 0.f)
		m_fSkill5Speed = 0.f;

	if (m_pModelCom->Get_KeyFrame() < m_pModelCom->Get_TimeLimit(1))
	{
		m_pTransformCom->Go_Dir(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_fSkill5Speed, fTimeDelta, m_pNavigationCom);
	}
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0))
	{
		m_bLGunRender = true;
		m_bRGunRender = true;
		m_bShotGunRender = false;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bSkill5EndCol_1)
	{
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		m_bSkill5EndCol_1 = true;
		m_bAttackCol = true;
		PTM->Create_AlphaParticle(L"Player03Skill5End", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Skill5EndBullet", m_pTransformCom->Get_WorldFloat4x4());
		Create_ParticleInstance(PTI_RSHOTGUN);
		Create_Effect(ET_RSHOTGUN);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(3) && !m_bSkill5EndCol_2)
	{
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		m_bSkill5EndCol_2 = true;
		m_bAttackCol = true;
		PTM->Create_AlphaParticle(L"Player03Skill5End_2", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Skill5EndBullet_2", m_pTransformCom->Get_WorldFloat4x4());
		Create_ParticleInstance(PTI_RSHOTGUN);
		Create_Effect(ET_RSHOTGUN);
	}
}

void CPlayer03::Skill5Loof_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_fSkill5Speed >= 0.f)
		m_fSkill5Speed -= 15.f * fTimeDelta;
	if (m_fSkill5Speed <= 0.f)
		m_fSkill5Speed = 0.f;

	m_pTransformCom->Go_Dir(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_fSkill5Speed, fTimeDelta, m_pNavigationCom);

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bSkill5LoofCol_1)
	{
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		m_bSkill5LoofCol_1 = true;
		m_bAttackCol = true;
		PTM->Create_AlphaParticle(L"Player03Skill5Loof", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Skill5LoofBullet", m_pTransformCom->Get_WorldFloat4x4());
		Create_ParticleInstance(PTI_RSHOTGUN);
		Create_Effect(ET_RSHOTGUN);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bSkill5LoofCol_2)
	{
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		m_bSkill5LoofCol_2 = true;
		m_bAttackCol = true;
		PTM->Create_AlphaParticle(L"Player03Skill5Loof_2", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Skill5LoofBullet_2", m_pTransformCom->Get_WorldFloat4x4());
		Create_ParticleInstance(PTI_RSHOTGUN);
		Create_Effect(ET_RSHOTGUN);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bSkill5LoofCol_3)
	{
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		m_bSkill5LoofCol_3 = true;
		m_bAttackCol = true;
		PTM->Create_AlphaParticle(L"Player03Skill5Loof_3", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Skill5LoofBullet_3", m_pTransformCom->Get_WorldFloat4x4());
		Create_ParticleInstance(PTI_RSHOTGUN);
		Create_Effect(ET_RSHOTGUN);
	}

}

void CPlayer03::Skill5Start_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_fSkill5Speed >= 0.f)
		m_fSkill5Speed -= 15.f * fTimeDelta;

	if (m_pModelCom->Get_KeyFrame() > m_pModelCom->Get_TimeLimit(1))
	{
		m_pTransformCom->Go_Dir(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_fSkill5Speed, fTimeDelta, m_pNavigationCom);
	}
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0))
	{
		m_bLGunRender = false;
		m_bRGunRender = false;
		m_bShotGunRender = true;
	}
	//사운드 4개인데 타이밍 못잡아서 그냥 합쳐서 넣어둠
	//se_HE04_Skill04_1-01 부터 se_HE04_Skill04_2-02 쓰면 됨
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bSkill5StartCol)
	{
		GI->PlaySoundW(L"se_HE04_Skill04_1+2.wav", SD_PLAYER3_SK, 0.8f);
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		m_bSkill5StartCol = true;
		m_bAttackCol = true;
		PTM->Create_AlphaParticle(L"Player03Skill5Start", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Skill5StartBullet", m_pTransformCom->Get_WorldFloat4x4());
		Create_ParticleInstance(PTI_RSHOTGUN);
		Create_Effect(ET_RSHOTGUN);
	}
}

void CPlayer03::Skill6_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() > m_pModelCom->Get_TimeLimit(0))
	{
		m_bLGunRender = true;
		m_bRGunRender = true;
		m_bCannonRender = false;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bSkill6Col_1)
	{
		GI->PlaySoundW(L"Hero04_ba_25.wav", SD_PLAYER3_VO, 1.f);
		GI->PlaySoundW(L"se_HE04_Skill05_1.wav", SD_PLAYER3_SK, 0.5f);
		m_bSkill6Col_1 = true;
		m_bAttackCol = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bSkill6Col_2)
	{
		GI->PlaySoundW(L"se_HE04_Skill05_3.wav", SD_PLAYER3_SK, 0.5f);
		CRM->Get_Cam()->Shake(0.15f, 0.1f, false, 0.15f);
		m_eHitType = TYPE_UP;
		m_fPushPower = 4.f;
		m_fAirPower = 6.f;
		m_bSkill6Col_2 = true;
		m_bAttackCol = true;
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER03) * 0.4f;
		
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(3) && !m_bSkill6Col_3)
	{
		GI->PlaySoundW(L"Hero04_ba_35.wav", SD_PLAYER3_VO, 1.f);
		GI->PlaySoundW(L"se_HE04_Skill05_2.wav", SD_PLAYER3_SK, 0.5f);
		CRM->Get_Cam()->Shake(0.3f, 0.2f, false, 0.1f);
		
		m_eHitType = TYPE_SPIN;
		m_fPushPower = 7.f;
		m_fAirPower = 8.f;
		m_bSkill6Col_3 = true;
		Change_ColRange(_float3{ 3.f,3.f,4.f }, _float3{ 0.f,3.f,4.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER03) * 2.f;
		Create_Effect(ET_CANNONBULLET);
	}
}

void CPlayer03::Skill7End_Update(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0))
	{
		m_bSniperRender = false;
		GI->PlaySoundW(L"se_HE04_Skill07_3.wav", SD_PLAYER3_SK, 0.4f);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1))
	{		
		m_bLGunRender = true;
		m_bRGunRender = true;
		Update_Parts();
	}


}

void CPlayer03::Skill7Shot_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(0))
	{
		if (m_fSkill7Speed >= 0.f)
			m_fSkill7Speed -= 25.f * fTimeDelta;
		if (m_fSkill7Speed <= 0.f)
			m_fSkill7Speed = 0.f;
		m_pTransformCom->Go_Dir(m_pTransformCom->Get_State(CTransform::STATE_LOOK) * -1.f, m_fSkill7Speed, fTimeDelta, m_pNavigationCom);
	}


}

void CPlayer03::Skill7Loof_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (CM_DOWN(DIMK_LBUTTON))
	{
		m_eShaderPass = SHADER_LIGHT;
		m_bPlayerLight = true;
		m_fPlayerLightLimit = 0.1f;
		GI->PlaySoundW(L"se_HE04_Skill07_2.wav", SD_PLAYER3_SK, 0.8f);
		m_pModelCom->Set_AnimIndex(SKILL7SHOT);
		m_eCurState = SKILL7SHOT;
		m_eNextState = SKILL7SHOT;
		m_fSkill7Speed = 10.f;
		++m_iSkill7Count;
		m_bAttackCol = true;
		CRM->Get_Cam()->Fov(2.f, 0.2f, 0.f);
		CRM->Get_Cam()->Shake(0.3f, 0.1f, false, 0.1f);
		CPlayer03SniperEffect_1::SNIPERINFO* Info = new CPlayer03SniperEffect_1::SNIPERINFO;
		Info->vPos = m_Parts[PARTS_SNIPER]->Get_ParentPos();
		Info->vScale = { 0.1f,0.1f,0.f };
		GI->Add_GameObjectToLayer(L"Player03SniperEffect_1", GI->Get_Cur_LevelIndex(), L"Layer_Player03Effect", &Info);
		Create_ParticleInstance(PTI_SKILL7);
		Create_ParticleInstance(PTI_SKILL7_1);
		CViewAlpha::VIEWALPHAINFO* Info2 = new CViewAlpha::VIEWALPHAINFO;
		Info2->fBlurPower = 1.f;
		Info2->vColor1 = { 1.f,1.f,0.f,1.f };
		Info2->vColor2 = { 1.f,0.2f,0.f,1.f };
		Info2->vRatio = { 0.7f,0.7f };
		Info2->vRatioSpeed = { 2.5f,1.5f };
		GI->Add_GameObjectToLayer(L"ViewAlpha", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2);
			
		
		
		return;
	}
}

void CPlayer03::Skill7Start_Update(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bSniperOn)
	{
		GI->PlaySoundW(L"se_HE04_Skill07_1.wav", SD_PLAYER3_SK, 0.4f);
		GI->PlaySoundW(L"Hero04_ba_40.wav", SD_PLAYER1_VO, 1.f);
		m_bSniperRender = true;
		m_bSniperOn = true;
	}
}

void CPlayer03::Skill8_Update(_float fTimeDelta)
{
}

void CPlayer03::SlapDown_Update(_float fTimeDelta)
{
}

void CPlayer03::Skill9_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bSkill9Col)
	{
		GI->PlaySoundW(L"se_HE04_Hattack_2.wav", SD_PLAYER3_SK, 0.4f);
		GI->PlaySoundW(L"Hero04_ba_03.wav", SD_PLAYER1_VO, 1.f); // 휘릭하고 넣으면 빼기
		CRM->Get_Cam()->Shake(0.2f, 0.2f, false, 0.2f);
		CRM->Get_Cam()->Fov(-3.f, 0.15f, 0.f);
		m_bAttackCol = true;
		m_bSkill9Col = true;
		m_bSkill9Re = true;
		Create_ParticleInstance(PTI_SKILL9);
		Create_Particle(PT_SKILL9);
		Create_ParticleInstance(PTI_RSHOTGUN);
		Create_Effect(ET_RSHOTGUN);
		m_eShaderPass = SHADER_LIGHT;
		m_bPlayerLight = true;
		m_fPlayerLightLimit = 0.1f;
	}

	//총 쏘고 reload
	if (m_bSkill9Re)
	{
		GI->PlaySoundW(L"se_HE04_Reload.wav", SD_PLAYER03_RE, 0.3f);
		m_bSkill9Re = false;
	}

	//몸 휘릭
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(01))
	{
		if (!m_bSkill9On)
		{
			GI->PlaySoundW(L"se_HE04_Special02_1.wav", SD_PLAYER3_SK, 0.4f);
			m_bSkill9On = true;
		}
		TIM->Fix_All(0.2f, 0.5f);
		
	}

	//휘릭 하고
	//GI->PlaySoundW(L"Hero04_ba_08.wav", SD_PLAYER1_VO, 1.f);
}

void CPlayer03::Skill10End_Update(_float fTimeDelta)
{
}

void CPlayer03::Skill10Loof_Update(_float fTimeDelta)
{
}

void CPlayer03::Skill10Start_Update(_float fTimeDelta)
{
}

void CPlayer03::Skill11_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0))
	{
		m_bSkill12Col_1 = false;
		m_bSkill12Col_2 = false;
		m_bSkill12Col_3 = false;
		m_bSkill12Col_4 = false;
		m_bSkill12Col_5 = false;
		m_eHitType = TYPE_SPIN;
		m_fPushPower = 0.f;
		m_fAirPower = 3.f;
		m_pModelCom->Set_AnimIndex(SKILL12);
		m_eCurState = SKILL12;
		m_eHE = HE_1;
		Change_ColRange(_float3{ 3.f,1.5f,3.f }, _float3{ 0.f,1.5f,0.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER03) * 0.3f;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bSkill11Col_1)
	{
		GI->PlaySoundW(L"se_HE01_Skill07_1.wav", SD_PLAYER1_SK, 0.7f);
		//GI->PlaySoundW(L"se_ba_skill.wav", SD_PLAYER3_SK, 0.6f);
		m_bAttackCol = true;
		m_bSkill11Col_1 = true;   
		Create_Effect(ET_SKILL11);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bSkill11Col_2)
	{
		GI->PlaySoundW(L"Hero04_ba_33.wav", SD_PLAYER1_VO, 1.f);
		m_bAttackCol = true;
		m_bSkill11Col_2 = true;
		Create_Effect(ET_SKILL11);
	}
}

void CPlayer03::Skill12_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bSkill12Col_1)
	{
		GI->PlaySoundW(L"se_HE04_Nattack_3.wav", SD_PLAYER3_SK, 0.5f);
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		m_bAttackCol = true;
		m_bSkill12Col_1 = true;
		
		PTM->Create_AlphaParticle(L"Player03Attack4B_2", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4B", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4A", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4A_2", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet_2", m_pTransformCom->Get_WorldFloat4x4());
		Create_ParticleInstance(PTI_LGUNPARTICLE);
		Create_ParticleInstance(PTI_RGUNPARTICLE);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bSkill12Col_2)
	{
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		m_bAttackCol = true;
		m_bSkill12Col_2 = true;
		_float4x4 a = m_pTransformCom->Get_WorldFloat4x4();
		a._42 += 0.5f;
		PTM->Create_AlphaParticle(L"Player03Attack4B_3", a);
		PTM->Create_AlphaParticle(L"Player03Attack4B_4", a);
		PTM->Create_AlphaParticle(L"Player03Attack4A_3", a);
		PTM->Create_AlphaParticle(L"Player03Attack4A_4", a);
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet_3", a);
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet_4", a);
		Create_ParticleInstance(PTI_LGUNPARTICLE);
		Create_ParticleInstance(PTI_RGUNPARTICLE);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bSkill12Col_3)
	{
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		m_bAttackCol = true;
		m_bSkill12Col_3 = true;
		_float4x4 a = m_pTransformCom->Get_WorldFloat4x4();
		a._42 += 1.f;
		PTM->Create_AlphaParticle(L"Player03Attack4B_5", a);
		PTM->Create_AlphaParticle(L"Player03Attack4B_6", a);
		PTM->Create_AlphaParticle(L"Player03Attack4A_5", a);
		PTM->Create_AlphaParticle(L"Player03Attack4A_6", a);
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet_5", a);
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet_6", a);
		Create_ParticleInstance(PTI_LGUNPARTICLE);
		Create_ParticleInstance(PTI_RGUNPARTICLE);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(3) && !m_bSkill12Col_4)
	{
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		m_bAttackCol = true;
		m_bSkill12Col_4 = true;
		_float4x4 a = m_pTransformCom->Get_WorldFloat4x4();
		a._42 += 0.5f;
		PTM->Create_AlphaParticle(L"Player03Attack4B", a);
		PTM->Create_AlphaParticle(L"Player03Attack4B_7", a);
		PTM->Create_AlphaParticle(L"Player03Attack4A", a);
		PTM->Create_AlphaParticle(L"Player03Attack4A_7", a);
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet", a);
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet_7", a);
		Create_ParticleInstance(PTI_LGUNPARTICLE);
		Create_ParticleInstance(PTI_RGUNPARTICLE);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(4) && !m_bSkill12Col_5)
	{
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		m_bAttackCol = true;
		m_bSkill12Col_5 = true;
		PTM->Create_AlphaParticle(L"Player03Attack4B_4", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4B_8", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4A_4", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_AlphaParticle(L"Player03Attack4A_8", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet_4", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03Attack4Bullet_8", m_pTransformCom->Get_WorldFloat4x4());
		Create_ParticleInstance(PTI_LGUNPARTICLE);
		Create_ParticleInstance(PTI_RGUNPARTICLE);
	}
}

void CPlayer03::Skill13_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0))
	{
		m_bRGunRender = false;
		m_bShotGunRender = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1))
	{
		m_bRGunRender = true;
		m_bShotGunRender = false;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bSkill13Col)
	{
		GI->PlaySoundW(L"Hero04_ba_44.wav", SD_PLAYER1_VO, 1.f);
		GI->PlaySoundW(L"se_HE04_Hattack_1_01.wav", SD_PLAYER3_SK, 0.6f);
		CRM->Get_Cam()->Shake(0.2f, 0.2f, false, 0.2f);
		m_bAttackCol = true;
		m_bSkill13Col = true;
		Create_ParticleInstance(PTI_SKILL13);
		Create_Particle(PT_SKILL13);
		Create_ParticleInstance(PTI_RSHOTGUN);
		Create_Effect(ET_RSHOTGUN);
	
		
	}
	
	//쏘고 End_Animation 전에
	//GI->PlaySoundW(L"se_HE04_Reload.wav", SD_PLAYER03_RE, 0.3f);
}

void CPlayer03::RunFastEnd_Update(_float fTimeDelta)
{
}

void CPlayer03::RunFastLoof_Update(_float fTimeDelta)
{
	m_pTransformCom->Go_Dir(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_fRunSpeed, fTimeDelta, m_pNavigationCom);

	m_fRunSoundAcc += 1.f * fTimeDelta;

	if (!m_bOtherFoot)
	{
		if (m_fRunSoundAcc >= 0.23f)
		{
			GI->PlaySoundW(L"se_flat_run_s_asphalt_03.wav", SD_PLAYER3_SK, 0.7f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = true;
		}
	}

	else
	{
		if (m_fRunSoundAcc >= 0.23f)
		{
			GI->PlaySoundW(L"se_flat_run_s_asphalt_04.wav", SD_PLAYER3_SK, 0.7f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = false;
		}
	}
}

void CPlayer03::RunFastStart_Update(_float fTimeDelta)
{
}

void CPlayer03::StepBack_Update(_float fTimeDelta)
{
}

void CPlayer03::Stun_Update(_float fTimeDelta)
{
}

void CPlayer03::StunEnd_Update(_float fTimeDelta)
{
}

void CPlayer03::RageSkill2_Update(_float fTimeDelta)
{
	m_bAttackCol = false;

	//대사 끝나고 몸 숙일때쯤
	//GI->PlaySoundW(L"se_Event_ShotPic.wav", SD_PLAYER03_RE, 0.6f);

	if (m_pModelCom->Get_KeyFrame() == 0 && !m_bHeroTest_1)
	{
		m_bHeroTest_1 = true;

		CRM->Get_Cam()->Save_World();
		CRM->Play_CutScene(12, true, false, m_pTransformCom);
	}
	if (m_pModelCom->Get_KeyFrame() == 41 && !m_bHeroTest_2)
	{
		m_bHeroTest_2 = true;

		CRM->Get_Cam()->Load_World();
		CRM->Get_Cam()->End_Target();
	}
	if (m_pModelCom->Get_KeyFrame() == 193 && !m_bHeroTest_3)
	{
		m_bHeroTest_3 = true;

		CRM->Get_Cam()->Save_World();
		CRM->Get_Cam()->Load_World();
		CRM->Get_Cam()->End_Target();
	}




	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(18) && !m_bRageSkill2Trail)
	{
		m_bRageSkill2Trail = true;
		m_bLHandTrail = true;
		m_bRHandTrail = true;
		GI->PlaySoundW(L"se_Event_ShotPic.wav", SD_PLAYER03_RE, 0.6f);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(0) && !m_bRageSkill2Col_1)
	{
		GI->PlaySoundW(L"se_HE04_Nattack_3.wav", SD_PLAYER3_SK, 0.6f);
		CRM->Get_Cam()->Shake(0.07f, 0.8f, false, 0.07f);
		m_bPointTrail = false;
		m_bAttackCol = true;
		m_bRageSkill2Col_1 = true;
		Create_ParticleInstance(PTI_RAGEGUN);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(1) && !m_bRageSkill2Col_2)
	{
		m_bAttackCol = true;
		m_bRageSkill2Col_2 = true;
		Create_ParticleInstance(PTI_RAGEGUN);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(2) && !m_bRageSkill2Col_3)
	{
		GI->PlaySoundW(L"Hero04_ba_53.wav", SD_PLAYER1_VO, 1.f);
		m_bAttackCol = true;
		m_bRageSkill2Col_3 = true;
		Create_ParticleInstance(PTI_RAGEGUN);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(3) && !m_bRageSkill2Col_4)
	{
		m_bAttackCol = true;
		m_bRageSkill2Col_4 = true;
		Create_ParticleInstance(PTI_RAGEGUN);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(4) && !m_bRageSkill2Col_5)
	{
		GI->PlaySoundW(L"se_HE04_Nattack_3.wav", SD_PLAYER3_SK, 0.6f);
		m_bAttackCol = true;
		m_bRageSkill2Col_5 = true;
		Create_ParticleInstance(PTI_RAGEGUN);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(5) && !m_bRageSkill2Col_6)
	{
		m_bAttackCol = true;
		m_bRageSkill2Col_6 = true;
		Create_ParticleInstance(PTI_RAGEGUN);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(6) && !m_bRageSkill2Col_7)
	{
		m_bAttackCol = true;
		m_bRageSkill2Col_7 = true;
		Create_ParticleInstance(PTI_RAGEGUN);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(7) && !m_bRageSkill2Col_8)
	{
		m_bAttackCol = true;
		m_bRageSkill2Col_8 = true;
		Create_ParticleInstance(PTI_RAGEGUN);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(8) && !m_bRageSkill2Col_9)
	{	
		m_bAttackCol = true;
		m_bRageSkill2Col_9 = true;
		Create_ParticleInstance(PTI_RAGEGUN);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(9) && !m_bRageSkill2Col_10)
	{
		GI->PlaySoundW(L"se_HE04_Hattack_1_01.wav", SD_PLAYER3_SK, 0.6f);
		CRM->Get_Cam()->Shake(0.25f, 0.1f, false, 0.1f);
		CRM->Get_Cam()->Fov(-0.07f, 4.f, 0.f,false);
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER03) * 2.f;
		m_fPushPower = 2.f;
		m_bAttackCol = true;
		m_bRageSkill2Col_10 = true;
		Change_ColRange(_float3{ 4.f,2.5f,8.f }, _float3{ 4.f,2.5f,8.f });
		m_eHE = HE_3;
		Create_ParticleInstance(PTI_RAGERSHOTGUN);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(10) && !m_bRageSkill2Col_11)
	{
		GI->PlaySoundW(L"se_HE04_Hattack_1_02.wav", SD_PLAYER3_SK, 0.6f);
		CRM->Get_Cam()->Shake(0.25f, 0.1f, false, 0.1f);
		m_bAttackCol = true;
		m_bRageSkill2Col_11 = true;
		Create_ParticleInstance(PTI_RAGELSHOTGUN);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(11) && !m_bRageSkill2Col_12)
	{
		GI->PlaySoundW(L"se_HE04_Hattack_2.wav", SD_PLAYER3_SK, 0.6f);
		CRM->Get_Cam()->Shake(0.25f, 0.1f, false, 0.1f);
		m_fPushPower =3.f;
		m_bAttackCol = true;
		m_bRageSkill2Col_12 = true;
		Create_ParticleInstance(PTI_RAGERSHOTGUN);
		m_bRageChargeSound = false;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(12) && !m_bRageSkill2Col_13)
	{
		m_bAttackCol = true;
		m_bRageSkill2Col_13 = true;
		Create_ParticleInstance(PTI_RAGELSHOTGUN);
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(13) && !m_bRageSkill2Col_14)
	{
		GI->PlaySoundW(L"se_HE04_Skill05_2.wav", SD_PLAYER3_SK, 0.6f);
		CRM->Get_Cam()->Shake(0.4f, 0.2f, false, 0.2f);
		CRM->Get_Cam()->Fov(3.f, 0.2f, 0.f);
		m_bRageSkill2Col_14 = true;
		Change_ColRange(_float3{ 5.f,3.f,7.f }, _float3{ 0.f,3.f,7.f });
		Create_Effect(ET_RAGECANNON);
		m_bLHandTrail = false;
		m_bRHandTrail = false;
		m_bPointTrail = true;
		m_pModelCom->Set_TickPerSecond(30.f);
		m_fRageParticleAcc = 0.f;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(14))
	{
		m_bLGunRender = false;
		m_bRGunRender = false;
		m_bShotGunRender = true;
		m_bLShotGunRender = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(15))
	{
		m_bShotGunRender = false;
		m_bLShotGunRender = false;
		m_bLCannonRender = true;
	}

	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(16))
	{
		m_bLCannonRender = false;
		m_bLGunRender = true;
		m_bRGunRender = true;
	}
	
	if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(17) && !m_bRageChargeSound)
	{
		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(0.f, 0.f, 0.f);
		CamDesc.vLook = _float3(0.f, 0.f, 0.f);
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 10.f;
		CamDesc.fLookLerpSpeed = 10.f;
		CamDesc.vPivotLookPos = _float3(0.f, 1.f, 2.f);
		CamDesc.pTran = m_pTransformCom;
		CamDesc.vPosDis = _float3(2.f, 1.5f, -2.f);
		CamDesc.bPixLocal = true;
		CRM->Get_Cam()->Show_Target(CamDesc);


		m_bRageChargeSound = true;
		m_pModelCom->Set_TickPerSecond(8.f);
		GI->PlaySoundW(L"CannonCharge.wav", SD_PLAYER3_SK, 1.f);
		m_fNowBP = 0.f;
	}

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(17) && m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(19))
	{
		PTM->Create_InstanceParticle(L"Player03RageCharge", m_pTransformCom->Get_WorldFloat4x4());
		PTM->Create_InstanceParticle(L"Player03RageCharge_2", m_pTransformCom->Get_WorldFloat4x4());

		
		
	}

	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(17) && m_pModelCom->Get_KeyFrame() <= m_pModelCom->Get_TimeLimit(20))
	{
		m_fRageParticleAcc += 1.f * fTimeDelta;
		if (m_fRageParticleAcc >= 0.2f)
		{
			m_bRageChargeEnd = false;
			PTM->Create_InstanceParticle(L"Player03RageCharge_3", m_pTransformCom->Get_WorldFloat4x4());
			m_fRageParticleAcc = 0.f;
		
		}
	}

	/*if (m_pModelCom->Get_KeyFrame() == m_pModelCom->Get_TimeLimit(21) && !m_bRageChargeEnd)
	{
		m_bRageChargeEnd = true;
		PTM->Create_AlphaParticle(L"Player03RageChargeEnd", m_pTransformCom->Get_WorldFloat4x4());
	}*/

}

void CPlayer03::FallDown_Update(_float fTimeDelta)
{
	if (!m_pTransformCom->Get_Air())
	{
		m_pModelCom->Set_AnimIndex(BOUNCE);
		m_eCurState = BOUNCE;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
		m_pTransformCom->ResetAndJump(1.f);
	}

	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vPushLook), m_fPushPower, fTimeDelta, m_pNavigationCom);
}

void CPlayer03::HitAway_Update(_float fTimeDelta)
{
	if (!m_pTransformCom->Get_Air())
	{
		Set_State(BOUNCE);
	}

	if (m_fPushPower > 5.f)
		m_fPushPower -= 5.f * fTimeDelta;

	if (m_fPushPower < 5.f)
		m_fPushPower = 5.f;

	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vPushLook), m_fPushPower, fTimeDelta, m_pNavigationCom);
}

void CPlayer03::HitSpin_Update(_float fTimeDelta)
{
	if (!m_pTransformCom->Get_Air())
	{
		Set_State(BOUNCE);
	}

	if (m_fPushPower > 0.f)
		m_fPushPower -= 3.f * fTimeDelta;

	if (m_fPushPower < 0.f)
		m_fPushPower = 0.f;

	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vPushLook), m_fPushPower, fTimeDelta, m_pNavigationCom);
}

void CPlayer03::HitDown_Update(_float fTimeDelta)
{
}

void CPlayer03::Create_HE_1(_float4 vWorldPos)
{
	/*_float4 vPos;
	XMStoreFloat4(&vPos, XMLoadFloat4(&vWorldPos) +
		XMVector3Normalize((XMLoadFloat4(&GI->Get_CamPosition()) - XMLoadFloat4(&vWorldPos) * 1.f)));
	XMStoreFloat4(&vPos, XMLoadFloat4(&vPos) + _vector{ GI->Get_FloatRandom(-0.5f,0.5f),
		GI->Get_FloatRandom(-0.5f,0.5f) ,  GI->Get_FloatRandom(-0.5f,0.5f) });

	CPlayer01HitEffect_1::HITEFFECT1INFO* HitEffect = new CPlayer01HitEffect_1::HITEFFECT1INFO;
	HitEffect->vScale = { 3.f,3.f,3.f };
	HitEffect->vPosition = vPos;
	HitEffect->vRatio = { 3.f,2.f,15.f };
	HitEffect->vColor = { 1.f,0.8f,0.5f,1.f };
	HitEffect->vColor2 = { 1.f,0.5f,0.1f,1.f };
	GI->Add_GameObjectToLayer(L"Player01HitEffect_1", GI->Get_Cur_LevelIndex(), L"Layer_PlayerEffect", &HitEffect);*/

	_matrix Matrix = XMMatrixIdentity();
	Matrix.r[3] = XMLoadFloat4(&vWorldPos);
	_float4x4 a;
	XMStoreFloat4x4(&a, Matrix);
	PTM->Create_AlphaParticle(L"GunHit", a);

	/*CPlayer01HitEffect_2::HITEFFECT2INFO* HitEffect2 = new CPlayer01HitEffect_2::HITEFFECT2INFO;
	HitEffect2->vScale = { 3.f,3.f,3.f };
	HitEffect2->vPosition = vPos;
	HitEffect2->vRatio = { 3.f,2.f,20.f };
	HitEffect2->vColor = { 0.f,0.f,0.f,1.f };
	HitEffect2->vColor2 = { 0.f,0.f,0.f,1.f };
	HitEffect2->fY = 1.f;
	GI->Add_GameObjectToLayer(L"Player01HitEffect_2", GI->Get_Cur_LevelIndex(), L"Layer_PlayerEffect", &HitEffect2);*/
}

void CPlayer03::Create_HE_2(_float4 vWorldPos)
{
	_matrix Matrix = XMMatrixIdentity();
	Matrix.r[3] = XMLoadFloat4(&vWorldPos);
	_float4x4 a;
	XMStoreFloat4x4(&a, Matrix);

	PTM->Create_InstanceParticle(L"Player03Hit5", a);

	CPlayer01HitEffect_1::HITEFFECT1INFO* HitEffect = new CPlayer01HitEffect_1::HITEFFECT1INFO;
	HitEffect->vScale = { 4.f,4.f,4.f };
	HitEffect->vPosition = vWorldPos;
	HitEffect->vRatio = { 3.f,2.f,15.f };
	HitEffect->vColor = { 1.f,1.f,0.9f,1.f };
	HitEffect->vColor2 = { 1.f,0.5f,0.f,1.f };
	GI->Add_GameObjectToLayer(L"Player01HitEffect_1", GI->Get_Cur_LevelIndex(), L"Layer_PlayerEffect", &HitEffect);

	CPlayer01HitEffect_2::HITEFFECT2INFO* HitEffect2 = new CPlayer01HitEffect_2::HITEFFECT2INFO;
	HitEffect2->vScale = { 5.f,5.f,5.f };
	HitEffect2->vPosition = vWorldPos;
	HitEffect2->vRatio = { 3.f,2.f,20.f };
	HitEffect2->vColor = { 0.f,0.f,0.f,1.f };
	HitEffect2->vColor2 = { 0.f,0.f,0.f,1.f };
	GI->Add_GameObjectToLayer(L"Player01HitEffect_2", GI->Get_Cur_LevelIndex(), L"Layer_PlayerEffect", &HitEffect2);

	



	
}

void CPlayer03::Create_HE_3(_float4 vWorldPos)
{
	_float4 vPos;
	XMStoreFloat4(&vPos, XMLoadFloat4(&vWorldPos) +
		XMVector3Normalize((XMLoadFloat4(&GI->Get_CamPosition()) - XMLoadFloat4(&vWorldPos) * 1.f)));
	XMStoreFloat4(&vPos, XMLoadFloat4(&vPos) + _vector{ GI->Get_FloatRandom(-0.5f,0.5f),
		GI->Get_FloatRandom(-0.5f,0.5f) ,  GI->Get_FloatRandom(-0.5f,0.5f) });

	CPlayer01HitEffect_2::HITEFFECT2INFO* HitEffect2 = new CPlayer01HitEffect_2::HITEFFECT2INFO;
	HitEffect2->vScale = { 3.f,3.f,3.f };
	HitEffect2->vPosition = vPos;
	HitEffect2->vRatio = { 3.f,2.f,20.f };
	HitEffect2->vColor = { 0.f,0.f,0.f,1.f };
	HitEffect2->vColor2 = { 0.f,0.f,0.f,1.f };
	GI->Add_GameObjectToLayer(L"Player01HitEffect_2", GI->Get_Cur_LevelIndex(), L"Layer_PlayerEffect", &HitEffect2);

	CPlayer01HitEffect_1::HITEFFECT1INFO* HitEffect = new CPlayer01HitEffect_1::HITEFFECT1INFO;
	HitEffect->vScale = { 2.f,2.f,2.f };
	HitEffect->vPosition = vPos;
	HitEffect->vRatio = { 3.f,2.f,15.f };
	HitEffect->vColor = { 1.f,1.f,0.9f,1.f };
	HitEffect->vColor2 = { 1.f,0.5f,0.f,1.f };
	GI->Add_GameObjectToLayer(L"Player01HitEffect_1", GI->Get_Cur_LevelIndex(), L"Layer_PlayerEffect", &HitEffect);

	

	_matrix Matrix = XMMatrixIdentity();
	Matrix.r[3] = XMLoadFloat4(&vPos);
	_float4x4 Matrix2;
	XMStoreFloat4x4(&Matrix2, Matrix);
	PTM->Create_InstanceParticle(L"Player01Hit1", Matrix2);
	PTM->Create_InstanceParticle(L"Player01Hit1_2", Matrix2);
}

void CPlayer03::Create_LightEffect()
{
	CPlayer04VoidParticle::COLORINFO* Info = new CPlayer04VoidParticle::COLORINFO;
	Info->vSize = { 1.f,1.f };
	Info->vColor = { 1.f,1.f,1.f,1.f };
	Info->vColor2 = { 0.f,1.f,0.9f,1.f };
	GI->Add_GameObjectToLayer(L"Player04VoidParticle", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
}

void CPlayer03::Idle_KeyInput(_float fTimeDelta)
{
	if (Input_Direction())
	{
		Set_State(RUN);
		return;
	}

	if (CK_DOWN(DIK_SPACE))
	{
		m_fJumpSpeed = 2.f;
		m_pTransformCom->Jump(7.f);
		Set_State(JUMPSTART);
		return;
	}

	if (CK_DOWN(DIK_LSHIFT))
	{
		Set_State(RUNFASTSTART);
		return;
	}

	Default_KeyInput(fTimeDelta);

}

void CPlayer03::Run_KeyInput(_float fTimeDelta)
{
	if (!Input_Direction())
	{
		if (m_ePlayerMode == PMODE_NORMAL)
			Set_State(IDLE1);
		else
			Set_State(IDLE2);
	}

	if (CK_DOWN(DIK_SPACE))
	{
		m_fJumpSpeed = 5.f;
		m_pTransformCom->Jump(9.f);
		Set_State(JUMPSTART);
		return;
	}

	if (CK_DOWN(DIK_LSHIFT))
	{
		Set_State(RUNFASTSTART);
		return;
	}

	Default_KeyInput(fTimeDelta);
}

void CPlayer03::Run_FastStartInput(_float fTimeDelta)
{
	if (CK_DOWN(DIK_SPACE))
	{
		m_fJumpSpeed = 8.f;
		m_pTransformCom->Jump(10.f);
		Set_State(JUMPSTART);
		return;
	}
}

void CPlayer03::Run_FastInput(_float fTimeDelta)
{
	if (CK_DOWN(DIK_LSHIFT))
	{
		Set_State(RUNFASTSTART);
		return;
	}

	if (CK_DOWN(DIK_SPACE))
	{
		m_fJumpSpeed = 8.f;
		m_pTransformCom->Jump(10.f);
		Set_State(JUMPSTART);
		return;
	}

	if (!Input_Direction())
	{
		m_pModelCom->Set_AnimIndex(RUNFASTEND);
		m_eCurState = RUNFASTEND;
		GI->PlaySoundW(L"stop-land.wav", SD_PLAYER3_SK, 0.2f);
		return;
	}

	Default_KeyInput(fTimeDelta);
}

void CPlayer03::JumpLoof_KeyInput(_float fTimeDelta)
{
	if (CM_PRESS(DIMK_LBUTTON))
	{
		Set_State(AIRATTACK2_START);
		return;
	}

	if (CK_DOWN(DIK_E))
	{
		m_pModelCom->Set_AnimIndex(AIRATTACK2);
		m_eCurState = AIRATTACK2;
		m_eNextState = AIRATTACK2;
		m_pModelCom->SetNextIndex(AIRATTACK2);
		m_eHE = HE_1;
		m_bAirAttack2Col_1 = false;
		m_bAirAttack2Col_2 = false;
		m_eHitType = TYPE_HIT;
		m_fPushPower = 2.f;
		m_fAirPower = 0.f;
		Change_ColRange(_float3{ 2.f,3.f,7.f }, _float3{ 0.f,-1.f,7.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER03) * 0.8f;
		
		return;
	}

	if (CK_DOWN(DIK_R))
	{
		m_pModelCom->Set_AnimIndex(AIRATTACK1);
		m_eCurState = AIRATTACK1;
		m_eNextState = AIRATTACK1;
		m_pModelCom->SetNextIndex(AIRATTACK1);
		m_bAirAttack1Col = false;
		m_eHE = HE_2;
		m_eHitType = TYPE_UP;
		m_fAirPower = 5.f;
		m_fPushPower = 0.f;
		Change_ColRange(_float3{ 2.f,3.f,5.f }, _float3{ 0.f,-1.f,5.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER03) * 1.4f;
		GI->PlaySoundW(L"se_HE04_Special01_1.wav", SD_PLAYER3_SK, 0.8f);
		return;
	}

}

void CPlayer03::Attack1_KeyInput(_float fTimeDelta)
{
	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(0))
	{
		if (CM_PRESS(DIMK_LBUTTON))
		{
			Input_Direction();
			Set_State(ATTACK2);
			return;
		}
	}

	if (CK_DOWN(DIK_CAPSLOCK))
	{
		m_bVoid = true;
		m_bRGunRender = true;
		m_bLGunRender = true;
		m_fVoidSpeed = 10.f;
		m_pModelCom->Set_AnimIndex(VOIDFRONT);
		m_eCurState = VOIDFRONT;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
		return;
	}
}

void CPlayer03::Attack2_KeyInput(_float fTimeDelta)
{
	
	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(0))
	{
		if (CM_PRESS(DIMK_LBUTTON))
		{
			Input_Direction();
			Set_State(ATTACK3);
			return;
		}
	}

	if (CK_DOWN(DIK_CAPSLOCK))
	{
		m_bVoid = true;
		m_bRGunRender = true;
		m_bLGunRender = true;
		m_fVoidSpeed = 10.f;
		m_pModelCom->Set_AnimIndex(VOIDFRONT);
		m_eCurState = VOIDFRONT;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
		return;
	}
}

void CPlayer03::Attack3_KeyInput(_float fTimeDelta)
{
	
	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(0))
	{
		if (CM_PRESS(DIMK_LBUTTON))
		{
			Input_Direction();
			Set_State(ATTACK4);
			return;
		}
	}

	if (CK_DOWN(DIK_CAPSLOCK))
	{
		m_bVoid = true;
		m_bRGunRender = true;
		m_bLGunRender = true;
		m_fVoidSpeed = 10.f;
		m_pModelCom->Set_AnimIndex(VOIDFRONT);
		m_eCurState = VOIDFRONT;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
		return;
	}
}

void CPlayer03::Attack4_KeyInput(_float fTimeDelta)
{
	
	if (m_pModelCom->Get_KeyFrame() >= m_pModelCom->Get_TimeLimit(0))
	{
		if (CM_PRESS(DIMK_LBUTTON))
		{
			Input_Direction();
			Set_State(ATTACK5);
			return;
		}
	}

	if (CK_DOWN(DIK_CAPSLOCK))
	{
		m_bVoid = true;
		m_bRGunRender = true;
		m_bLGunRender = true;
		m_fVoidSpeed = 10.f;
		m_pModelCom->Set_AnimIndex(VOIDFRONT);
		m_eCurState = VOIDFRONT;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
		return;
	}
}

void CPlayer03::Attack5_KeyInput(_float fTimeDelta)
{
	if (CK_DOWN(DIK_CAPSLOCK))
	{
		m_bVoid = true;
		m_bRGunRender = true;
		m_bLGunRender = true;
		m_fVoidSpeed = 10.f;
		m_pModelCom->Set_AnimIndex(VOIDFRONT);
		m_eCurState = VOIDFRONT;
		m_eNextState = m_eCurState;
		m_pModelCom->SetNextIndex(m_eNextState);
		return;
	}
}

void CPlayer03::Skill7Loof_KeyInput(_float fTimeDelta)
{
	if (!m_pModelCom->GetChangeBool())
		Input_Direction();
}

void CPlayer03::AirAttack2Loof_KeyInput(_float fTimeDelta)
{
	if (CM_PRESS_F(DIMK_LBUTTON))
	{
		m_pModelCom->Set_AnimIndex(AIRATTACK2_END);
		m_eCurState = AIRATTACK2_END;
		m_bAttackCol = false;
		m_bAirAttack2LoofCol_1 = false;
		m_bAirAttack2LoofCol_2 = false;
	}
}

HRESULT CPlayer03::Ready_Components()
{

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Player03Body"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	m_pModelCom->Set_RootHierarchy(1);


	/* For.Com_Navigation */
	CNavigation::NAVIGATIONDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = 0;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage1Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vCenter = _float3(0.f, 1.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(30.f, 30.f, 30.f);
	strcpy(ColDesc.sTag, "Player_Attack");
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;

	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.8f, 0.f);
	ColDesc.vSize = _float3(0.7f, 1.6f, 0.7f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Player_Body");
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;

	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.3f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.5f, 0.5f, 0.5f);
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "Player_Push_Range");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;



	CHierarchyNode*		pPlayer_Body = m_pModelCom->Get_HierarchyNode("Neck_M_offset");
	if (nullptr == pPlayer_Body)
		return E_FAIL;
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(2.f, 2.f, 2.f);
	ColDesc.vCenter = _float3(0.f, 1.f, 0.f);
	ColDesc.pSocket = pPlayer_Body;
	XMStoreFloat4x4(&ColDesc.PivotMatrix, m_pModelCom->Get_PivotMatrix());
	ColDesc.pOwner = this;
	ColDesc.iFlag = 10;
	strcpy(ColDesc.sTag, "Player_HairCut");
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayer03::Ready_Sockets()
{
	CHierarchyNode*		pHead = m_pModelCom->Get_HierarchyNode("Head_M");
	if (nullptr == pHead)
		return E_FAIL;
	m_Sockets.push_back(pHead);

	CHierarchyNode*		pWeaponL = m_pModelCom->Get_HierarchyNode("Weapon_L");
	if (nullptr == pWeaponL)
		return E_FAIL;
	m_Sockets.push_back(pWeaponL);

	CHierarchyNode*		pWeaponR = m_pModelCom->Get_HierarchyNode("Weapon_R");
	if (nullptr == pWeaponR)
		return E_FAIL;
	m_Sockets.push_back(pWeaponR);

	CHierarchyNode*		pKickL = m_pModelCom->Get_HierarchyNode("Toes_L");
	if (nullptr == pKickL)
		return E_FAIL;
	m_Sockets.push_back(pKickL);

	CHierarchyNode*		pKickR = m_pModelCom->Get_HierarchyNode("Toes_R");
	if (nullptr == pKickR)
		return E_FAIL;
	m_Sockets.push_back(pKickR);


	return S_OK;
}

HRESULT CPlayer03::Ready_PlayerParts()
{

	CGameObject*		pPlayer03HairA = GI->Clone_GameObject(TEXT("Player03HairA"));
	if (nullptr == pPlayer03HairA)
		return E_FAIL;
	m_Parts.push_back((CParts*)pPlayer03HairA);

	CGameObject*		pPlayer03HairB = GI->Clone_GameObject(TEXT("Player03HairB"));
	if (nullptr == pPlayer03HairB)
		return E_FAIL;
	m_Parts.push_back((CParts*)pPlayer03HairB);

	CGameObject*		pPlayer03LGun = GI->Clone_GameObject(TEXT("Player03LGun"));
	if (nullptr == pPlayer03LGun)
		return E_FAIL;
	m_Parts.push_back((CParts*)pPlayer03LGun);

	CGameObject*		pPlayer03RGun = GI->Clone_GameObject(TEXT("Player03RGun"));
	if (nullptr == pPlayer03RGun)
		return E_FAIL;
	m_Parts.push_back((CParts*)pPlayer03RGun);

	CGameObject*		pPlayer03ShotGun = GI->Clone_GameObject(TEXT("Player03ShotGun"));
	if (nullptr == pPlayer03ShotGun)
		return E_FAIL;
	m_Parts.push_back((CParts*)pPlayer03ShotGun);

	CGameObject*		pPlayer03LShotGun = GI->Clone_GameObject(TEXT("Player03LShotGun"));
	if (nullptr == pPlayer03LShotGun)
		return E_FAIL;
	m_Parts.push_back((CParts*)pPlayer03LShotGun);

	CGameObject*		pPlayer03Cannon = GI->Clone_GameObject(TEXT("Player03Cannon"));
	if (nullptr == pPlayer03Cannon)
		return E_FAIL;
	m_Parts.push_back((CParts*)pPlayer03Cannon);

	CGameObject*		pPlayer03LCannon = GI->Clone_GameObject(TEXT("Player03LCannon"));
	if (nullptr == pPlayer03LCannon)
		return E_FAIL;
	m_Parts.push_back((CParts*)pPlayer03LCannon);

	CGameObject*		pPlayer03Sniper = GI->Clone_GameObject(TEXT("Player03Sniper"));
	if (nullptr == pPlayer03Sniper)
		return E_FAIL;
	m_Parts.push_back((CParts*)pPlayer03Sniper);

	return S_OK;
}

HRESULT CPlayer03::Update_Parts()
{

	XMStoreFloat4x4(&m_HeadMatrix, m_Sockets[SOCKET_HEAD]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix());
	if(m_ePlayerMode == PMODE_BURST)
		m_Parts[PARTS_HAIRA]->SetUp_State(XMLoadFloat4x4(&m_HeadMatrix));
	else
		m_Parts[PARTS_HAIRB]->SetUp_State(XMLoadFloat4x4(&m_HeadMatrix));

	if (m_bLGunRender)
	{
		XMStoreFloat4x4(&m_WeaponLMatrix, m_Sockets[SOCKET_WEAPON_L]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix());
		m_Parts[PARTS_LGUN]->SetUp_State(XMLoadFloat4x4(&m_WeaponLMatrix));
	}

	if (m_bRGunRender)
	{
		XMStoreFloat4x4(&m_WeaponRMatrix, m_Sockets[SOCKET_WEAPON_R]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix());
		m_Parts[PARTS_RGUN]->SetUp_State(XMLoadFloat4x4(&m_WeaponRMatrix));
	}

	if (m_bShotGunRender)
	{
		XMStoreFloat4x4(&m_WeaponRMatrix, m_Sockets[SOCKET_WEAPON_R]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix());
		m_Parts[PARTS_SHOTGUN]->SetUp_State(XMLoadFloat4x4(&m_WeaponRMatrix));
	}

	if (m_bLShotGunRender)
	{
		XMStoreFloat4x4(&m_WeaponLMatrix, m_Sockets[SOCKET_WEAPON_L]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix());
		m_Parts[PARTS_LSHOTGUN]->SetUp_State(XMLoadFloat4x4(&m_WeaponLMatrix));
	}

	if (m_bCannonRender)
	{
		XMStoreFloat4x4(&m_WeaponRMatrix, m_Sockets[SOCKET_WEAPON_R]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix());
		m_Parts[PARTS_CANNON]->SetUp_State(XMLoadFloat4x4(&m_WeaponRMatrix));
	}

	if (m_bLCannonRender)
	{
		XMStoreFloat4x4(&m_WeaponLMatrix, m_Sockets[SOCKET_WEAPON_L]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix());
		m_Parts[PARTS_LCANNON]->SetUp_State(XMLoadFloat4x4(&m_WeaponLMatrix));
	}

	if (m_bSniperRender)
	{
		XMStoreFloat4x4(&m_WeaponRMatrix, m_Sockets[SOCKET_WEAPON_R]->Get_CombinedTransformation()* m_pModelCom->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix());
		m_Parts[PARTS_SNIPER]->SetUp_State(XMLoadFloat4x4(&m_WeaponRMatrix));
	}
	

	
	return S_OK;
}

void CPlayer03::Render_Parts(_float fTimeDelta)
{
	if (m_ePlayerMode == PMODE_BURST)
	{
		m_Parts[PARTS_HAIRA]->LateTick(fTimeDelta);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_Parts[PARTS_HAIRA]);
	}
	else
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_Parts[PARTS_HAIRB]);
	
	if(m_bLGunRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_Parts[PARTS_LGUN]);

	if (m_bRGunRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_Parts[PARTS_RGUN]);

	if (m_bShotGunRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_Parts[PARTS_SHOTGUN]);

	if(m_bLShotGunRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_Parts[PARTS_LSHOTGUN]);

	if (m_bCannonRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_Parts[PARTS_CANNON]);

	if (m_bLCannonRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_Parts[PARTS_LCANNON]);

	if (m_bSniperRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_Parts[PARTS_SNIPER]);
}

CPlayer03 * CPlayer03::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer03*		pInstance = new CPlayer03(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer03"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer03::Clone(void * pArg)
{
	CPlayer03*		pInstance = new CPlayer03(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer03"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer03::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	for (auto& iter : m_SwordTrails)
		Safe_Release(iter);
	
	for (auto& iter : m_PointTrails)
		Safe_Release(iter);

	for (auto& pPart : m_Parts)
		Safe_Release(pPart);
	
	m_Parts.clear();

	for (auto& iter : m_MotionTrails)
		Safe_Release(iter);

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pShaderCom);
}
