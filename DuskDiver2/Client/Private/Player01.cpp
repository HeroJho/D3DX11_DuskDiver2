#include "stdafx.h"
#include "..\Public\Player01.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Player_Manager.h"
#include "DataManager.h"
#include "Parts.h"
#include "Particle_Manager.h"
#include "ToolManager.h"
#include "Status_Manager.h"
#include "Player01HairB.h"
#include "Player01HairC.h"
#include "Time_Manager.h"
#include "UI_Manager.h"
#include "Player01HairA.h"
#include "Player01HitEffect_1.h"
#include "Player01HitEffect_2.h"
#include "Camera_Manager.h"
#include "Player01Sunder_1.h"
#include "Player01Sunder_2.h"
#include "Wall.h"
#include "Player01Trail_1.h"
#include "Wind.h"
#include "Monster_Manager.h"
#include "EffectSpin.h"
#include "EffectAtom.h"
#include "Ring.h"
#include "Tornado.h"
#include "ViewAlpha.h"
#include "PointTrailObj.h"
#include "SwordTrailObj.h"
#include "PointTrail.h"
#include "SwordTrail.h"
#include "Player01MotionTrail.h"
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
#include "Player04VoidParticle.h"
#include "MonsterSpawn.h"

CPlayer01::CPlayer01(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CPlayer01::CPlayer01(const CPlayer01 & rhs)
	: CCreture(rhs)
{
}

HRESULT CPlayer01::Initialize_Prototype()
{
	for (int i = MODEL_NORMAL; i < MODEL_END; ++i)
		m_pModelCom[i] = nullptr;
	return S_OK;
}

HRESULT CPlayer01::Initialize(void * pArg)
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Player";
	// *[HeroJho]: ImGui로 디버깅을 하려면 꼭 호출해줘야 합니다.
	Make_ImGui();
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 25.f, 1.f));
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(2.5f, 2.5f, -45.f, 1.f));
	m_vTargetLook = _float3{ 0.f,0.f,1.f };
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&m_vTargetLook));
	// *[HeroJho]: 현재 내가 서있는 네비 인덱스를 자동으로 구해줍니다. 비용이 비싸기 때문에 Tick과 같은 반복 호출 금지입니다!
	_int iNaviIndex = m_pNavigationCom->Find_NaviIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	//_int iNaviIndex = m_pNavigationCom->Find_NaviIndex({ 2.5f, 2.5f, -45.f, 1.f });

	m_pNavigationCom->Set_NaviIndex(iNaviIndex);

	Ready_PlayerParts();

	Set_AniInfo();

	m_pModelCom[MODEL_NORMAL]->Set_AnimIndex(IDLE1);
	m_pModelCom[MODEL_BURST]->Set_AnimIndex(IDLE1);

	PM->Add_Player(this);

	m_fHurtTime = 0.1f;
	STM->Set_FinalHp(CStatus_Manager::PLAYER01);
	m_fMaxHp = STM->Get_FinalHp(CStatus_Manager::PLAYER01);
	m_fNowHp = m_fMaxHp;
	STM->Set_FinalStr(CStatus_Manager::PLAYER01);
	m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER01);
	//((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->Set_Sleep();
	//((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->Set_Sleep();

	Get_Colliders("Player_HairCut")->Set_Sleep();
	Get_Colliders("Player_HairCut_Burst")->Set_Sleep();
	
	m_fPreHp = m_fNowHp;
	m_fMaxSp = 100.f;
	m_fNowSP = m_fMaxSp;
	m_fPreSp = m_fNowSP;
	m_fMaxEp = 100.f;
	m_fNowEP = 100.f;
	m_fPreEp = m_fNowEP;
	
	Ready_Trails();


	((CPlayer01HairA*)m_Parts[PARTS_HAIRA])->Set_Wake();
	((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->Set_Wake();
	((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->Set_Wake();


	m_pModelCom[MODEL_NORMAL]->Play_Animation(0.1f, m_pTransformCom, m_pNavigationCom);
	m_pModelCom[MODEL_BURST]->Play_Animation(0.1f, m_pTransformCom, m_pNavigationCom);

	_matrix HeadMatrix = m_Sockets[SOCKET_HEAD]->Get_CombinedTransformation()* m_pModelCom[m_eAnimModel]->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
	m_Parts[PARTS_HAIRA]->SetUp_State(HeadMatrix);
	m_Parts[PARTS_HAIRB]->SetUp_State(HeadMatrix);
	m_Parts[PARTS_HAIRC]->SetUp_State(HeadMatrix);
	((CPlayer01HairA*)m_Parts[PARTS_HAIRA])->SetInitPos();
	((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->SetInitPos();
	((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->SetInitPos();

	((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->Set_Sleep();
	((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->Set_Sleep();

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	return S_OK;
}

void CPlayer01::Tick(_float fTimeDelta)
{

	if (m_bSelectPlayer && PM->Get_PlayerTick())
	{
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
		Create_SwordTrail(fTimeDelta);
		Create_PointTrail(fTimeDelta);



	}

}

void CPlayer01::LateTick(_float fTimeDelta)
{
	if (m_bSelectPlayer && PM->Get_PlayerTick())
	{
		if (nullptr == m_pRendererCom)
			return;

		fTimeDelta *= m_fTMRatio;

		LateTick_Func(fTimeDelta);

	}
}

HRESULT CPlayer01::Change_Navigation(_uint iNaviNum)
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
		//플레이어 원래 위치
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(1.239f, -7.803f, -20.961f, 1.f));
		//두더지
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-180.86f, 38.7f, 136.778f, 1.f));
		//문어
		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-1.538f, 15.531f, 147.338f, 1.f));

		m_vTargetLook = _float3{ 1.f,0.f,0.f };
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&m_vTargetLook));
		break;
	case 5:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage5Navigation"), TEXT("Stage5"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
			return E_FAIL;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, -30.f, 1.f));

		//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.619f, -32.962f, -158.067f, 1.f));
		m_vTargetLook = _float3{ 1.f,0.f,0.f };
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&m_vTargetLook));
		break;
	}

	_int iNaviIndex = m_pNavigationCom->Find_NaviIndex(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pNavigationCom->Set_NaviIndex(iNaviIndex);


	Update_Parts();
	if (m_ePlayerMode == PMODE_BATTLE)
	{
		((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->SetInitPos();
	}
	else if (m_ePlayerMode == PMODE_BURST)
	{
		((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->SetInitPos();
	}
	else
	{
		((CPlayer01HairA*)m_Parts[PARTS_HAIRA])->SetInitPos();
	}

	return S_OK;
}

void CPlayer01::Change_Tag()
{
	Set_State(STEPBACK);
}

void CPlayer01::Create_TagEffect()
{
	CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
	CWall::WALLINFO* Info = new CWall::WALLINFO;
	Info->vSize = _float3{ 0.15f,6.f,0.15f };
	XMStoreFloat4(&Info->vWorldPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	Info->vWorldPos.y += 10.f;
	Info->vColor = _float4{ 1.f,0.9f,0.f,1.f };
	Info->vColor2 = _float4{ 1.f,0.7f,0.f,1.f };
	Info->vRatio = _float2{ 1.f,1.f };
	Info->vUVFixSpeed = _float2{ 0.f,-0.5f };
	Info->vFixPosSpeed = _float3{ 0.f,-40.f,0.f };
	GI->Add_GameObjectToLayer(L"Wall", GI->Get_Cur_LevelIndex(), L"Layer_PlayerEffect", &Info);
}

void CPlayer01::Change_HairCut(_bool bOnOff)
{
	if (bOnOff)
	{
		Update_Parts();
		if (m_ePlayerMode == PMODE_BATTLE)
		{
  			Get_Colliders("Player_HairCut")->Set_Awake();
			Get_Colliders("Player_HairCut_Burst")->Set_Sleep();
			
			((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->Set_Wake();			
			((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->SetInitPos();
		}
		else if (m_ePlayerMode == PMODE_BURST)
		{
			Get_Colliders("Player_HairCut_Burst")->Set_Awake();
			Get_Colliders("Player_HairCut")->Set_Sleep();
			((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->Set_Wake();
			((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->SetInitPos();
		}
		else
		{
			((CPlayer01HairA*)m_Parts[PARTS_HAIRA])->Set_Wake();
			((CPlayer01HairA*)m_Parts[PARTS_HAIRA])->SetInitPos();
		}
	}


	
}

_bool CPlayer01::Get_BurstMode()
{
	if (m_ePlayerMode == PMODE_BURST)
		return true;
	else
		return false;
}

_bool CPlayer01::Get_IsIdle()
{
	if (m_eCurState == IDLE1 || m_eCurState == IDLE2)
		return true;

	return false;
}

_bool CPlayer01::Get_PreUp()
{
	if (m_fPreHp < m_fNowHp)
		return true;

	return false;
}

_bool CPlayer01::Get_PreDown()
{
	if (m_fPreHp > m_fNowHp)
		return true;

	return false;
}

void CPlayer01::LateTick_Func(_float fTimeDelta)
{
	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&m_vTargetLook), 0.2f);

	m_pModelCom[m_eAnimModel]->Play_Animation(fTimeDelta, m_pTransformCom, m_pNavigationCom);

	m_pTransformCom->Tick_Gravity(fTimeDelta, m_pNavigationCom, m_fGravity, true);

	Update_Parts();

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	End_Animation();

	Push_Collider();

	Render_Parts(fTimeDelta);

	for (auto& iter : m_MotionTrails)
		iter->LateTick(fTimeDelta);

	for (auto& iter : m_SwordTrails)
		iter->LateTick(fTimeDelta);
	
	for (auto& iter : m_PointTrails)
		iter->LateTick(fTimeDelta);
	
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	// if (m_bShadow)
	//	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_MOVESHADOWDEPTH, this);

	// Render_Col(m_pRendererCom);
	_int iCount = 0;
	for (auto& pCollider : m_Colliders)
	{
		if (6 != iCount && 3 != iCount && 0 != iCount && 1 != iCount)
			m_pRendererCom->Add_DebugGroup(pCollider);
		iCount++;
	}

	

	// m_pRendererCom->Add_DebugGroup(m_pNavigationCom);
	m_pNavigationCom->Set_CurCell(m_pNavigationCom->Get_CurCellIndex());

}

HRESULT CPlayer01::Render()
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
	_float fBlurPower = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
		return E_FAIL;

	_float fGama = 1.8f;
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

HRESULT CPlayer01::Render_ShadowDepth()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pTransformCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_ShadowLightViewMatrix(GI->Get_Cur_LevelIndex()), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);


	_uint		iNumMeshes = m_pModelCom[m_eAnimModel]->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom[m_eAnimModel]->SetUp_OnShader(m_pShaderCom, m_pModelCom[m_eAnimModel]->Get_MaterialIndex(0), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom[m_eAnimModel]->Render(m_pShaderCom, i, 2)))
			return E_FAIL;
	}


	// m_Parts[PARTS_HAIRA]->Render_ShadowDepth();

	return S_OK;
}

// *[HeroJho]: 디버깅 창입니다. ImGui로 작동됩니다.
void CPlayer01::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);
	
	ImGui::NewLine();
	IG->Debug_Col(Get_Colliders("Player_Body"));
	
	ImGui::NewLine();
	ImGui::DragFloat("Gravity", &m_fGravity, 
		0.01f, 0.f, 100.f, "%.2f");
	ImGui::Checkbox("Shadow", &m_bShadow);
	
	ImGui::NewLine();
	ImGui::InputFloat("Pre", &m_fPreHp);
	ImGui::InputFloat("Cur", &m_fNowHp);

	ImGui::End();
}

// *[HeroJho]: 충돌이 발생할 때 호출되는 함수입니다.
void CPlayer01::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
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
		int i = 0;
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

void CPlayer01::On_Hit(CCreture* pOther)
{
	if (m_bHurt && !m_bReHp && !Get_PreUp())
	{
		GI->PlaySoundW(L"SwordHit.ogg", SD_PLAYER1_SK, 0.5f);

		_uint iRandomVoice = GI->Get_Random(0, 6);
		switch (iRandomVoice)
		{
		case 0:
			GI->PlaySoundW(L"Hero01_ba_14.wav", SD_PLAYER1_VO, 0.6f);
			break;
		case 1:
			GI->PlaySoundW(L"Hero01_ba_15.wav", SD_PLAYER1_VO, 0.6f);
			break;
		case 2:
			GI->PlaySoundW(L"Hero01_ba_16.wav", SD_PLAYER1_VO, 0.6f);
			break;
		case 3:
			GI->PlaySoundW(L"Hero01_ba_17.wav", SD_PLAYER1_VO, 0.6f);
			break;
		case 4:
			GI->PlaySoundW(L"Hero01_ba_18.wav", SD_PLAYER1_VO, 0.6f);
			break;
		case 5:
			GI->PlaySoundW(L"Hero01_ba_19.wav", SD_PLAYER1_VO, 0.6f);
			break;
		case 6:
			GI->PlaySoundW(L"Hero01_ba_20.wav", SD_PLAYER1_VO, 0.6f);
			break;
		}

		Hit_Effect(pOther);
		Hit_State(pOther);

		m_bHurt = false;
	}
}

void CPlayer01::Create_HitEffect(_float4 vWorldPos)
{
	switch (m_eHE)
	{
	case Client::CPlayer01::HE_1:
		Create_HE_1(vWorldPos);
		break;

	case Client::CPlayer01::HE_2:
		Create_HE_2(vWorldPos);
		break;
	}

}

_float4 CPlayer01::Get_PlayerPos()
{
	_float4 PlayerPos;
	XMStoreFloat4(&PlayerPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	return PlayerPos;
}

void CPlayer01::Change_PlayerPos(_float4 vChanges)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMLoadFloat4(&vChanges));
}

void CPlayer01::Set_ChangeLook(DIR eDir, _float3 vDir)
{
	m_eDir = eDir;
	m_vTargetLook = vDir;
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&vDir));
}

_int CPlayer01::Get_NaviIndex()
{
	return m_pNavigationCom->Get_CurCellIndex();
}

void CPlayer01::Set_NaviIndex(_int iNaviIndex)
{
	m_pNavigationCom->Set_NaviIndex(iNaviIndex);
}

void CPlayer01::Reset_Animation()
{
	if (m_ePlayerMode == PMODE_NORMAL)
	{
		m_pModelCom[m_eAnimModel]->Set_AnimIndex(IDLE1);
		m_eCurState = IDLE1;
		m_eNextState = m_eCurState;
		m_pModelCom[m_eAnimModel]->SetNextIndex(m_eNextState);
	}
	else
	{
		m_pModelCom[m_eAnimModel]->Set_AnimIndex(IDLE2);
		m_eCurState = IDLE2;
		m_eNextState = m_eCurState;
		m_pModelCom[m_eAnimModel]->SetNextIndex(m_eNextState);
	}
	m_bVoid = false;
}

void CPlayer01::Set_State(STATE eState)
{
	if (m_eNextState == eState)
		return;

	m_eNextState = eState;

	switch (m_eNextState)
	{
	case Client::CPlayer01::AIRATTACK1:
		Input_Direction();
		m_bAirAttack1Col = false;
		m_eHitType = TYPE_HIT;
		m_fAirPower = 1.f;
		m_fPushPower = 0.5f;
		Change_ColRange(_float3{ 1.5f,2.f,1.5f }, _float3{ 0.f,1.f,1.5f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER01) * 0.6f;
		break;
	case Client::CPlayer01::AIRATTACK2:
		Input_Direction();
		m_bAirAttack2Col = false;
		m_eHitType = TYPE_HIT;
		m_fAirPower = 1.f;
		break;
	case Client::CPlayer01::AIRATTACK3:
		Input_Direction();
		m_bAirAttack3Col = false;
		m_eHitType = TYPE_HIT;
		m_fAirPower = 1.f;
		break;
	case Client::CPlayer01::AIRATTACK4:
		Input_Direction();
		break;
	case Client::CPlayer01::AIRATTACK5END:
		break;
	case Client::CPlayer01::AIRATTACK5LOOF:
		break;
	case Client::CPlayer01::AIRATTACK5START:
		Input_Direction();
		m_eHitType = TYPE_FASTDOWN;
		m_fAirPower = -30.f;
		m_fPushPower = 0.f;
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER01) * 1.4f;
		break;
	case Client::CPlayer01::ATTACK1:	
	{
		Input_Direction();
		m_bAttack1Col = false;
		m_eHitType = TYPE_HIT;
		m_fPushPower = 3.f;
		m_fAirPower = 0.f;
		Change_ColRange(_float3{ 1.5f,1.5f,1.5f }, _float3{ 0.f,1.5f,1.5f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER01) * 0.6f;
	
	}
		break;
	case Client::CPlayer01::ATTACK2:
	{
		Input_Direction();
		m_bAttack2Col = false;
		m_eHitType = TYPE_HIT;
		m_fPushPower = 3.f;
		m_fAirPower = 0.f;

		/*_matrix LHandMatrix;
		if (m_ePlayerMode != PMODE_BURST)
		{
			LHandMatrix = m_Sockets[SOCKET_WEAPON_L]->Get_CombinedTransformation()* m_pModelCom[m_eAnimModel]->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();

		}
		else
		{
			LHandMatrix = m_Sockets[SOCKET_WEAPON_L2]->Get_CombinedTransformation()* m_pModelCom[m_eAnimModel]->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
		}
		m_SwordTrails[STR_LHAND]->TrailOn(LHandMatrix);*/

		break;

	}
	case Client::CPlayer01::ATTACK3:
		Input_Direction();
		m_bAttack3Col = false;
		m_eHitType = TYPE_SPIN;
		m_fPushPower = 2.f;
		m_fAirPower = 5.f;
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER01) * 0.8f;

		break;
	case Client::CPlayer01::ATTACK4:
		Input_Direction();
		m_bAttack4Col = false;
		m_eHitType = TYPE_UP;
		m_fPushPower = 3.f;
		m_fAirPower = 6.f;
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER01);

		break;
	case Client::CPlayer01::ATTACK5:
		Input_Direction();
		m_bAttack5Col = false;
		m_eHitType = TYPE_AWAY;
		m_fPushPower = 30.f;
		m_fAirPower = 1.f;
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER01) * 1.5f;
		break;
	case Client::CPlayer01::BOUNCE:
		m_bVoid = true;
		m_pTransformCom->ResetAndJump(1.f);
		break;
	case Client::CPlayer01::CHANGEBATTLE:
		GI->PlaySoundW(L"Hero01_ba_59.wav", SD_PLAYER1_VO, 1.f);
		m_bVoid = true;
		m_bBattleChange = false;
		break;
	case Client::CPlayer01::DASH:
		break;
	case Client::CPlayer01::GROGGYEND:
		break;
	case Client::CPlayer01::GROGGYLOOF:
		break;
	case Client::CPlayer01::GROGGYSTART:
		Die_Sound();
		m_bVoid = true;
		break;
	case Client::CPlayer01::DOWN:
		break;
	case Client::CPlayer01::VOIDFRONT:
		Input_Direction();
		break;
	case Client::CPlayer01::VOIDAIR:
		break;
	case Client::CPlayer01::VOIDATTACK:
	
		break;
	case Client::CPlayer01::CHANGEBURST:
		GI->PlaySoundW(L"UI_BP_full.wav", SD_UI, 0.3f);
		GI->PlaySoundW(L"Hero01_ba_59.wav", SD_PLAYER1_VO, 1.f);
		m_fChangeEffectAcc = 0.f;
		m_bVoid = true;
		m_bBurstChange = false;
		m_bHeroTest7 = false;
		m_bHeroTest8 = false;
		m_bHeroTest9 = false;
		break;
	case Client::CPlayer01::FINISHNORMAL:
		break;
	case Client::CPlayer01::FINISHKICK:
		break;
	case Client::CPlayer01::GETUP:
		break;
	case Client::CPlayer01::RUSHKICK:
		GI->PlaySoundW(L"se_ba_skill.wav", SD_PLAYER1_SK, 0.6f);
		GI->PlaySoundW(L"se_HE01_Hattack_1.wav", SD_PLAYER1_SK, 0.5f);
		
		Input_Direction();
		m_bRushKickCol = false;
		m_eHitType = TYPE_SPIN;
		m_fPushPower = 2.f;
		m_fAirPower = 6.f;
		Change_ColRange(_float3{ 2.f,3.f,3.f }, _float3{ 0.f,1.f,3.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER01) * 1.5f;
		Create_Effect(ET_SKILLCHARGE);
		break;
	case Client::CPlayer01::HURTBACK:
		Reset_Collider();
		break;
	case Client::CPlayer01::HURTFLY:
		break;
	case Client::CPlayer01::HURTFRONT:
		Reset_Collider();
		break;
	case Client::CPlayer01::IDLE1:
		break;
	case Client::CPlayer01::IDLE2:
		break;
	case Client::CPlayer01::JUMPLOOF:
		break;
	case Client::CPlayer01::JUMPSTART:
		GI->PlaySoundW(L"se_flat_Landing_s_asphalt_01.wav", SD_PLAYER1_SK, 0.8f);
		GI->PlaySoundW(L"Hero01_ba_21.wav", SD_PLAYER1_VO, 0.9f);
		break;
	case Client::CPlayer01::JUMPEND:
		break;
	case Client::CPlayer01::FALLUP:
		break;
	case Client::CPlayer01::RUN:
		m_fRunSpeed = 5.f;
		break;
	case Client::CPlayer01::SKILL1:
		GI->PlaySoundW(L"se_ba_skill.wav", SD_PLAYER1_SK, 0.6f);
		Input_Direction();
		m_bSkill1Col = false;
		m_eHitType = TYPE_AWAY;
		m_fPushPower = 40.f;
		m_fAirPower = 1.f;
		Change_ColRange(_float3{ 1.5f,1.5f,2.f }, _float3{ 0.f,1.5f,2.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER01) * 1.5f;
		Create_Effect(ET_DASH);
		m_bRHandTrail = true;
		if (m_ePlayerMode != PMODE_BURST)
		{
			((CPointTrailObj*)m_PointTrails[4])->Set_Color(_float4{ 1.f,1.f,1.f,1.f });
			((CPointTrailObj*)m_PointTrails[4])->Set_Color2(_float4{ 0.f,1.f,0.9f,1.f });
			((CPointTrailObj*)m_PointTrails[4])->Set_BlurPower(1.f);
			((CPointTrailObj*)m_PointTrails[4])->Set_Ratio(_float2{ 1.f,1.f });
			((CPointTrailObj*)m_PointTrails[4])->Set_Size(0.1f);
			((CPointTrailObj*)m_PointTrails[4])->Set_MaxIndex(15);
		}
		else
		{
			((CPointTrailObj*)m_PointTrails[4])->Set_Color(_float4{ 1.f,1.f,0.5f,1.f });
			((CPointTrailObj*)m_PointTrails[4])->Set_Color2(_float4{ 1.f,0.5f,0.f,1.f });
			((CPointTrailObj*)m_PointTrails[4])->Set_BlurPower(1.f);
			((CPointTrailObj*)m_PointTrails[4])->Set_Ratio(_float2{ 1.f,1.f });
			((CPointTrailObj*)m_PointTrails[4])->Set_Size(0.1f);
			((CPointTrailObj*)m_PointTrails[4])->Set_MaxIndex(15);
		}
		
		break;
	case Client::CPlayer01::SKILL2:
		GI->PlaySoundW(L"se_ba_skill.wav", SD_PLAYER1_SK, 0.6f);
		Input_Direction();
		m_bSkill2Col_1 = false;
		m_bSkill2Col_2 = false;
		m_bSkill2Col_3 = false;
		m_bSkill2Col_4 = false;
		m_bSkill2Sound = false;
		m_eHitType = TYPE_HIT;
		m_fPushPower = 1.5f;
		m_fAirPower = 0.f;
		Change_ColRange(_float3{ 1.5f,1.5f,2.f }, _float3{ 0.f,1.5f,2.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER01) * 1.f;
		break;
	case Client::CPlayer01::SKILL3:
		GI->PlaySoundW(L"se_ba_skill.wav", SD_PLAYER1_SK, 0.6f);
		Input_Direction();
		m_bSkill3Col_1 = false;
		m_bSkill3Col_2 = false;
		m_bSkill3Effect = false;
		m_bSkill3Effect2 = false;
		m_eHitType = TYPE_SPIN;
		m_fAirPower = 6.f;
		m_fPushPower = 1.f;
		Change_ColRange(_float3{ 1.5f,1.5f,2.f }, _float3{ 0.f,1.5f,2.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER01) * 1.2f;
		break;
	case Client::CPlayer01::SKILL4_START:
		Input_Direction();
		m_eHitType = TYPE_HIT;
		m_fPushPower = 0.5f;
		m_fAirPower = 0.f;
		Change_ColRange(_float3{ 1.5f,1.5f,2.f }, _float3{ 0.f,1.5f,2.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER01) * 0.6f;
		break;
	case Client::CPlayer01::SKILL4_LOOF:
		break;
	case Client::CPlayer01::SKILL4_END:
		break;
	case Client::CPlayer01::SKILL5:
		GI->PlaySoundW(L"se_ba_skill.wav", SD_PLAYER1_SK, 0.6f);
		Input_Direction();
		m_bSkill5Col = false;
		m_eHitType = TYPE_SPIN;
		m_fPushPower = 6.f;
		m_fAirPower = 6.f;
		Change_ColRange(_float3{ 3.f,4.f,5.f }, _float3{ 0.f,4.f,5.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER01) * 1.4f;
		Create_Effect(ET_SKILL5);
		Create_Effect(ET_SKILLCHARGE);
		m_bSkill5Effect = false;
		m_bSkill5Effect2 = false;
		m_bLHandTrail = true;
		if (m_ePlayerMode != PMODE_BURST)
		{
			((CPointTrailObj*)m_PointTrails[3])->Set_Color(_float4{ 1.f,1.f,1.f,1.f });
			((CPointTrailObj*)m_PointTrails[3])->Set_Color2(_float4{ 0.f,1.f,0.9f,1.f });
			((CPointTrailObj*)m_PointTrails[3])->Set_BlurPower(1.f);
			((CPointTrailObj*)m_PointTrails[3])->Set_Ratio(_float2{ 1.f,1.f });
			((CPointTrailObj*)m_PointTrails[3])->Set_Size(2.f);
			((CPointTrailObj*)m_PointTrails[3])->Set_MaxIndex(3);
		}
		else
		{
			((CPointTrailObj*)m_PointTrails[3])->Set_Color(_float4{ 1.f,1.f,0.5f,1.f });
			((CPointTrailObj*)m_PointTrails[3])->Set_Color2(_float4{ 1.f,0.5f,0.f,1.f });
			((CPointTrailObj*)m_PointTrails[3])->Set_BlurPower(1.f);
			((CPointTrailObj*)m_PointTrails[3])->Set_Ratio(_float2{ 1.f,1.f });
			((CPointTrailObj*)m_PointTrails[3])->Set_Size(2.f);
			((CPointTrailObj*)m_PointTrails[3])->Set_MaxIndex(3);
		}
		
		break;
	case Client::CPlayer01::SKILL6:
		GI->PlaySoundW(L"se_ba_skill.wav", SD_PLAYER1_SK, 0.6f);
		Input_Direction();
		m_bSkill6Col = false;
		m_eHitType = TYPE_UP;
		m_fAirPower = 10.f;
		m_fPushPower = 1.f;
		Change_ColRange(_float3{ 1.5f,2.f,2.f }, _float3{ 0.f,2.f,2.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER01) * 1.5f;
		Create_Effect(ET_SKILLCHARGE);
		m_bLHandTrail = true;
		if (m_ePlayerMode != PMODE_BURST)
		{
			((CPointTrailObj*)m_PointTrails[3])->Set_Color(_float4{ 1.f,1.f,1.f,1.f });
			((CPointTrailObj*)m_PointTrails[3])->Set_Color2(_float4{ 0.f,1.f,0.9f,1.f });
			((CPointTrailObj*)m_PointTrails[3])->Set_BlurPower(1.f);
			((CPointTrailObj*)m_PointTrails[3])->Set_Ratio(_float2{ 1.f,1.f });
			((CPointTrailObj*)m_PointTrails[3])->Set_Size(0.1f);
			((CPointTrailObj*)m_PointTrails[3])->Set_MaxIndex(15);
		}
		else
		{
			((CPointTrailObj*)m_PointTrails[3])->Set_Color(_float4{ 1.f,1.f,0.5f,1.f });
			((CPointTrailObj*)m_PointTrails[3])->Set_Color2(_float4{ 1.f,0.5f,0.f,1.f });
			((CPointTrailObj*)m_PointTrails[3])->Set_BlurPower(1.f);
			((CPointTrailObj*)m_PointTrails[3])->Set_Ratio(_float2{ 1.f,1.f });
			((CPointTrailObj*)m_PointTrails[3])->Set_Size(0.1f);
			((CPointTrailObj*)m_PointTrails[3])->Set_MaxIndex(15);
		}

		break;
	case Client::CPlayer01::SKILL6_END:
		break;
	case Client::CPlayer01::SKILL6_LOOF:
		break;
	case Client::CPlayer01::SKILL6_START:
		GI->PlaySoundW(L"Hero01_ba_11.wav", SD_PLAYER1_VO, 1.3f);
		GI->PlaySoundW(L"se_HE01_Nattack_1.wav", SD_PLAYER1_SK, 0.8f);
		Input_Direction();
		m_bSkill6StartCol = false;
		m_eHitType = TYPE_FASTDOWN;
		m_fAirPower = -40.f;
		m_fPushPower = 0.f;
		m_bSkill6StartSlowEnd = false;
		m_bSkill6StartSlowStart = false;
		m_bSkill6StartEffect = false;
		m_bSkill6JumpEnd = false;
		Change_ColRange(_float3{ 1.5f,2.f,2.f }, _float3{ 0.f,1.f,2.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER01) * 2.f;
		break;
	case Client::CPlayer01::SKILL7:
		Input_Direction();
		m_bSkill7Col_1 = false;
		m_bSkill7Col_2 = false;
		m_bSkill7Col_3 = false;
		m_bSkill7Col_4 = false;
		m_bSkill7Col_5 = false;
		m_bSkill7Col_6 = false;
		m_eHitType = TYPE_HIT;
		m_fPushPower = 1.f;
		m_fAirPower = 0.f;
		Change_ColRange(_float3{ 1.5f,1.5f,2.f }, _float3{ 0.f,1.5f,2.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER01) * 0.6f;
		break;
	case Client::CPlayer01::SKILL8:
		GI->PlaySoundW(L"se_HE01_Skill05_1.wav", SD_PLAYER1_SK, 0.8f);
		Input_Direction();
		m_bSkill8Col = false;
		m_eHitType = TYPE_UP;
		m_fAirPower = 10.f;
		m_fPushPower = 0.f;
		m_fParticleY = 0.5f;
		Change_ColRange(_float3{ 3.f,3.f,4.f }, _float3{ 0.f,3.f,4.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER01) * 0.8f;
		break;
	case Client::CPlayer01::FALL:
		break;
	case Client::CPlayer01::SKILL9:
		GI->PlaySoundW(L"se_ba_skill.wav", SD_PLAYER1_SK, 0.6f);
		Input_Direction();
		m_bSkill9Col = false;
		m_eHitType = TYPE_AWAY;
		m_fPushPower = 20.f;
		m_fAirPower = 6.f;
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER01) * 1.2f;
		break;
	case Client::CPlayer01::SKILL10:
		m_bSkill10Col = false;
		m_eHitType = TYPE_HIT;
		m_fPushPower = 3.f;
		m_fAirPower = 0.f;
		Change_ColRange(_float3{ 1.5f,1.5f,2.f }, _float3{ 0.f,1.5f,2.f });
		break;
	case Client::CPlayer01::SKILL11:
		break;
	case Client::CPlayer01::RUNFASTEND:
		break;
	case Client::CPlayer01::RUNFAST:
		break;
	case Client::CPlayer01::RUNFASTSTART:
		GI->PlaySoundW(L"UI_Dash.wav", SD_PLAYER1_SK, 0.4f);
		m_fRunSpeed = 8.f; // 8.f;
		Create_Effect(ET_DASH);
		break;
	case Client::CPlayer01::STEPBACK:
		m_bVoid = true;
		Get_Colliders("Player_HairCut")->Set_Sleep();
		Get_Colliders("Player_HairCut_Burst")->Set_Sleep();
		/*m_vTargetLook = GI->Get_CamDir(CPipeLine::DIR_LOOK);
		m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMLoadFloat3(&m_vTargetLook));*/
		break;
	case Client::CPlayer01::LEFTSTEP:
		break;
	case Client::CPlayer01::RIGHTSTEP:
		break;
	case Client::CPlayer01::STUN:
		break;
	case Client::CPlayer01::STUNEND:
		break;
	case Client::CPlayer01::RAGESKILL1: 
		GI->PlaySoundW(L"Hero01_ba_61.wav", SD_PLAYER1_VO, 1.f);
		UM->Set_FrontUI(false);
		m_bVoid = true;
		m_bRageSkill1Col_1 = false;	m_bRageSkill1Col_2 = false;	m_bRageSkill1Col_3 = false;	m_bRageSkill1Col_4 = false;
		m_bRageSkill1Col_5 = false;	m_bRageSkill1Col_6 = false;	m_bRageSkill1Col_7 = false; m_fPushPower = 0.f; m_fAirPower = 0.f;
		m_bRageSkill1Effect = false;
		m_eHitType = TYPE_SPIN;
		m_fPushPower = 0.5f;
		m_fAirPower = 2.f;
		Change_ColRange(_float3{ 3.f,4.f,4.f }, _float3{ 0.f,4.f,4.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER01) * 1.4f;
		m_bPatternSkill = true;
		m_bHeroTest = false;
		m_bHeroTest1 = false;
		m_bHeroTest11 = false;
		break;
	case Client::CPlayer01::RAGESKILL2:
		UM->Set_FrontUI(false);
		GI->PlaySoundW(L"se_HE01_Super02_4.wav", SD_PLAYER1_SK, 0.8f);
		GI->PlaySoundW(L"Hero01_ba_63.wav", SD_PLAYER1_VO, 1.f);
		m_bBurstSkillOn = true;
		m_bPatternSkill = true;
		m_bVoid = true;
		m_bRageSkill2Col_1 = false;	m_bRageSkill2Col_2 = false;	m_bRageSkill2Col_3 = false;	m_bRageSkill2Col_4 = false;
		m_bRageSkill2Col_5 = false;	m_bRageSkill2Col_6 = false;		m_bRageSkill2Col_7 = false;		m_bRageSkill2Col_8 = false;
		m_bRageSkill2Col_9 = false;	m_bRageSkill2Col_10 = false;	m_bRageSkill2Col_11 = false;	m_bRageSkill2Col_12 = false;	
		m_bRageSkill2Col_13 = false;	m_bRageSkill2Col_14 = false; m_eHitType = TYPE_SPIN; m_fPushPower = 0.f; m_fAirPower = 1.f;
		m_fRageSkill1ParticleAcc = 0.2f; m_bRageSkill2Effect = false; m_bLKickTrail = false; m_bRKickTrail = false;
		Change_ColRange(_float3{ 5.f,5.f,5.f }, _float3{ 0.f,5.f,5.f });
		((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->SkillMass();
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER01) * 1.f;

		m_bHeroTest2 = false;
		m_bHeroTest3 = false;
		m_bHeroTest4 = false;
		m_bHeroTest5 = false;
		m_bHeroTest6 = false;
		m_bHeroTest10 = false;
		break;
	case Client::CPlayer01::FALLDOWN:
		break;
	case Client::CPlayer01::HITAWAY:
		Reset_Collider();
		break;
	case Client::CPlayer01::HITSPIN:
		Reset_Collider();
		break;
	case Client::CPlayer01::HITDOWN:
		Reset_Collider();
		break;
	}

	m_pModelCom[m_eAnimModel]->SetNextIndex(m_eNextState);
	m_pModelCom[m_eAnimModel]->SetChangeBool(true);
	m_eCurState = m_eNextState;
	

}

void CPlayer01::Set_Dir(DIR eDir)
{
	m_eDir = eDir;

	_matrix View = XMLoadFloat4x4(&GI->Get_TransformFloat4x4_Inverse(CPipeLine::D3DTS_VIEW));
	_vector vCamLook = View.r[2];

	_vector vCamRight = XMVector3Normalize(XMVector3Cross(_vector{ 0.f,1.f,0.f }, vCamLook));

	vCamLook = XMVector3Normalize(XMVector3Cross(vCamRight, _vector{ 0.f,1.f,0.f }));

	switch (m_eDir)
	{
	case Client::CPlayer01::DIR_UP:
		XMStoreFloat3(&m_vTargetLook, (vCamLook));
		break;
	case Client::CPlayer01::DIR_DOWN:
		XMStoreFloat3(&m_vTargetLook, (vCamLook * -1.f));
		break;
	case Client::CPlayer01::DIR_RIGHT:
		XMStoreFloat3(&m_vTargetLook, (vCamRight));
		break;
	case Client::CPlayer01::DIR_LEFT:
		XMStoreFloat3(&m_vTargetLook, (vCamRight * -1.f));
		break;
	case Client::CPlayer01::DIR_LU:
		XMStoreFloat3(&m_vTargetLook, XMVector3Normalize(vCamLook - vCamRight));
		break;
	case Client::CPlayer01::DIR_RU:
		XMStoreFloat3(&m_vTargetLook, XMVector3Normalize(vCamLook + vCamRight));
		break;
	case Client::CPlayer01::DIR_LD:
		XMStoreFloat3(&m_vTargetLook, XMVector3Normalize(vCamLook * -1.f + vCamRight * -1.f));
		break;
	case Client::CPlayer01::DIR_RD:
		XMStoreFloat3(&m_vTargetLook, XMVector3Normalize(vCamLook * -1.f + vCamRight));
		break;
	}
}

void CPlayer01::End_Animation()
{
	if (m_pModelCom[m_eAnimModel]->GetAniEnd())
	{
		switch (m_eCurState)
		{
		case Client::CPlayer01::AIRATTACK1:
			if (!m_pTransformCom->Get_Air())
				Set_State(JUMPEND);
			else
				Set_State(JUMPLOOF);
			break;
		case Client::CPlayer01::AIRATTACK2:
			if (!m_pTransformCom->Get_Air())
				Set_State(JUMPEND);
			else
				Set_State(JUMPLOOF);
			break;
		case Client::CPlayer01::AIRATTACK3:
			if (!m_pTransformCom->Get_Air())
				Set_State(JUMPEND);
			else
				Set_State(JUMPLOOF);
			break;
		case Client::CPlayer01::AIRATTACK4:
			if (!m_pTransformCom->Get_Air())
				Set_State(JUMPEND);
			else
				Set_State(JUMPLOOF);
			break;
		case Client::CPlayer01::AIRATTACK5END:		
			Set_State(IDLE2);
			break;
		case Client::CPlayer01::AIRATTACK5LOOF:
			break;
		case Client::CPlayer01::AIRATTACK5START:
			m_pModelCom[m_eAnimModel]->Set_AnimIndex(AIRATTACK5LOOF);
			m_eCurState = AIRATTACK5LOOF;
			m_pTransformCom->ResetAndJump(-30.f);
			m_eShaderPass = SHADER_LIGHT;
			m_bPlayerLight = true;
			m_fPlayerLightLimit = 0.1f;
			m_bAirAttack5Col = false;
			break;
		case Client::CPlayer01::ATTACK1:	
			Set_State(IDLE2);
			break;
		case Client::CPlayer01::ATTACK2:		
			Set_State(IDLE2);
			break;
		case Client::CPlayer01::ATTACK3:			
			Set_State(IDLE2);
			break;
		case Client::CPlayer01::ATTACK4:		
			Set_State(IDLE2);
			break;
		case Client::CPlayer01::ATTACK5:		
			Set_State(IDLE2);
			break;
		case Client::CPlayer01::BOUNCE:
			Set_State(GETUP);
			break;
		case Client::CPlayer01::CHANGEBATTLE:	
			m_bVoid = false;		
			m_vPlayerLightColor = { 1.f,1.f,1.f };
			((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->Set_PlayerLightColor(_float3{ 1.f,1.f,1.f });
			((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->Set_PlayerLightColor(_float3{ 1.f,1.f,1.f });
			Set_State(IDLE2);
			break;
		case Client::CPlayer01::DASH:
			break;
		case Client::CPlayer01::GROGGYEND:
			m_bVoid = false;
			Set_State(IDLE2);
			break;
		case Client::CPlayer01::GROGGYLOOF:
			Set_State(GROGGYEND);
			break;
		case Client::CPlayer01::GROGGYSTART:
			Set_State(GROGGYLOOF);		
			Player_Die();
			break;
		case Client::CPlayer01::DOWN:
			break;
		case Client::CPlayer01::VOIDFRONT:
			GI->PlaySoundW(L"se_HE01_Hattack_2.wav", SD_PLAYER1_SK, 0.8f);
			m_bVoidOn = false;
			m_bVoid = false;
			m_pModelCom[m_eAnimModel]->Set_AnimIndex(IDLE2);
			m_eCurState = IDLE2;
			m_eNextState = m_eCurState;
			m_pModelCom[m_eAnimModel]->SetNextIndex(m_eNextState);
			break;
		case Client::CPlayer01::VOIDAIR:
			break;
		case Client::CPlayer01::VOIDATTACK:
			m_pModelCom[m_eAnimModel]->Set_AnimIndex(IDLE2);
			m_eCurState = IDLE2;
			m_eNextState = m_eCurState;
			m_pModelCom[m_eAnimModel]->SetNextIndex(m_eNextState);			
			break;
		case Client::CPlayer01::CHANGEBURST:
			m_bVoid = false;
			m_bChangeBurstSound = false;
			Set_State(IDLE2);
			break;
		case Client::CPlayer01::FINISHNORMAL:
			break;
		case Client::CPlayer01::FINISHKICK:
			break;
		case Client::CPlayer01::GETUP:
			m_bVoid = false;
			Set_State(IDLE2);
			break;
		case Client::CPlayer01::RUSHKICK:		
			Set_State(IDLE2);
			break;
		case Client::CPlayer01::HURTBACK:			
			Set_State(IDLE2);
			break;
		case Client::CPlayer01::HURTFLY:
			Set_State(IDLE2);
			break;
		case Client::CPlayer01::HURTFRONT:			
			Set_State(IDLE2);
			break;
		case Client::CPlayer01::IDLE1:
			break;
		case Client::CPlayer01::IDLE2:
			break;
		case Client::CPlayer01::JUMPLOOF:
			break;
		case Client::CPlayer01::JUMPSTART:
			m_pModelCom[m_eAnimModel]->Set_AnimIndex(JUMPLOOF);
			m_eCurState = JUMPLOOF;
			break;
		case Client::CPlayer01::JUMPEND:
			GI->PlaySoundW(L"se_flat_Landing_s_asphalt_02.wav", SD_PLAYER1_SK, 0.8f);
			Set_State(IDLE1);			
			break;
		case Client::CPlayer01::FALLUP:
			break;
		case Client::CPlayer01::RUN:
			break;
		case Client::CPlayer01::SKILL1:		
			GI->PlaySoundW(L"se_HE01_Skill01_7.wav", SD_PLAYER1_SK, 0.6f);
			Set_State(IDLE2);
			break;
		case Client::CPlayer01::SKILL2:			
			Set_State(IDLE2);
			break;
		case Client::CPlayer01::SKILL3:			
			Set_State(IDLE2);
			break;
		case Client::CPlayer01::SKILL4_START:
			m_pModelCom[m_eAnimModel]->Set_AnimIndex(SKILL4_LOOF);
			m_eCurState = SKILL4_LOOF;
			m_eNextState = m_eCurState;
			m_pModelCom[m_eAnimModel]->SetNextIndex(SKILL4_LOOF);
			m_bSkill4LoofCol_1 = false;
			m_bSkill4LoofCol_2 = false;
			m_eHitType = TYPE_HIT;
			break;
		case Client::CPlayer01::SKILL4_LOOF:
			if (m_bSkill4Loof)
			{
				Input_Direction();
				m_bSkill4Loof = false;
				m_bSkill4LoofCol_1 = false;
				m_bSkill4LoofCol_2 = false;
				m_eHitType = TYPE_HIT;
				m_fPushPower = 0.5f;
				m_fAirPower = 0.f;
			}
			else
			{
				m_pModelCom[m_eAnimModel]->Set_AnimIndex(SKILL4_END);
				m_eCurState = SKILL4_END;
				m_eNextState = m_eCurState;
				m_pModelCom[m_eAnimModel]->SetNextIndex(SKILL4_END);
				m_bSkill4EndCol = false;
				m_bSkill4EndSlowEnd = false;
				m_bSkill4EndSlowStart = false;
				m_eHitType = TYPE_AWAY;
				m_fPushPower = 20.f;
				m_fAirPower = 6.f;
				Input_Direction();
				m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER01) * 1.5f;
			}
			break;
		case Client::CPlayer01::SKILL4_END:
			Set_State(IDLE2);
			
			break;
		case Client::CPlayer01::SKILL5:
			GI->PlaySoundW(L"se_HE01_Skill01_7.wav", SD_PLAYER1_SK, 0.6f);
			Set_State(IDLE2);
			break;
		case Client::CPlayer01::SKILL6:
			m_pModelCom[m_eAnimModel]->Set_AnimIndex(JUMPLOOF);
			m_eCurState = JUMPLOOF;
			m_eNextState = m_eCurState;
			m_pModelCom[m_eAnimModel]->SetNextIndex(m_eNextState);
			m_fJumpSpeed = 0.f;
			break;
		case Client::CPlayer01::SKILL6_END:
			Set_State(IDLE2);
			m_fGravity = 40.f;
			m_bLHandTrail = false;
			m_bRHandTrail = false;
			break;
		case Client::CPlayer01::SKILL6_LOOF:
			
			break;
		case Client::CPlayer01::SKILL6_START:
			Set_State(SKILL6_LOOF);
			break;
		case Client::CPlayer01::SKILL7:
			Set_State(IDLE2);
			break;
		case Client::CPlayer01::SKILL8:
			Set_State(IDLE2);
			m_fParticleY = 1.f;
			break;
		case Client::CPlayer01::FALL:
			break;
		case Client::CPlayer01::SKILL9:
			Set_State(IDLE2);
			break;
		case Client::CPlayer01::SKILL10:
			break;
		case Client::CPlayer01::SKILL11:
			Set_State(IDLE2);
			break;
		case Client::CPlayer01::RUNFASTEND:		
			Set_State(IDLE1);		
			break;
		case Client::CPlayer01::RUNFAST:
			break;
		case Client::CPlayer01::RUNFASTSTART:
			m_pModelCom[m_eAnimModel]->Set_AnimIndex(RUNFAST);
			m_eCurState = RUNFAST;
			m_eNextState = m_eCurState;
			m_pModelCom[m_eAnimModel]->SetNextIndex(m_eNextState);
			break;
		case Client::CPlayer01::STEPBACK:
			m_bSelectPlayer = false;
			((CPlayer01HairA*)m_Parts[PARTS_HAIRA])->Set_Sleep();
			((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->Set_Sleep();
			((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->Set_Sleep();
			Get_Colliders("Player_HairCut")->Set_Sleep();
			Get_Colliders("Player_HairCut_Burst")->Set_Sleep();
			break;
		case Client::CPlayer01::LEFTSTEP:
			break;
		case Client::CPlayer01::RIGHTSTEP:
			break;
		case Client::CPlayer01::STUN:
			break;
		case Client::CPlayer01::STUNEND:
			break;
		case Client::CPlayer01::RAGESKILL1:		
			UM->Set_FrontUI(true);
			m_bVoid = false;
			Set_State(IDLE2);			
			m_bPatternSkill = false;
			break;
		case Client::CPlayer01::RAGESKILL2:	
			UM->Set_FrontUI(true);
			m_bBurstSkillOn = false;
			m_bVoid = false;
			Set_State(IDLE2);
			m_bLKickTrail = true;
			m_bRKickTrail = true;
			m_bPatternSkill = false;
			((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->StagMass();
			break;
		case Client::CPlayer01::FALLDOWN:
			break;
		case Client::CPlayer01::HITAWAY:
			Set_State(FALLDOWN);
			break;
		case Client::CPlayer01::HITSPIN:
			Set_State(FALLDOWN);
			break;
		case Client::CPlayer01::HITDOWN:
			m_pModelCom[m_eAnimModel]->Set_AnimIndex(BOUNCE);
			m_eCurState = BOUNCE;
			m_eNextState = m_eCurState;
			m_pModelCom[m_eAnimModel]->SetNextIndex(m_eNextState);
			break;
		}
	}
}

void CPlayer01::Get_KeyInput(_float fTimeDelta)
{
	m_bDirKeyInput = false;

	if (!m_pModelCom[m_eAnimModel]->GetChangeBool())
	{
		switch (m_eCurState)
		{
		case Client::CPlayer01::AIRATTACK1:
			AirAttack1_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer01::AIRATTACK2:
			AirAttack2_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer01::AIRATTACK3:
			AirAttack3_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer01::AIRATTACK4:
			break;
		case Client::CPlayer01::AIRATTACK5END:
			break;
		case Client::CPlayer01::AIRATTACK5LOOF:
			break;
		case Client::CPlayer01::AIRATTACK5START:
			break;
		case Client::CPlayer01::ATTACK1:
			Attack1_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer01::ATTACK2:
			Attack2_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer01::ATTACK3:
			Attack3_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer01::ATTACK4:
			Attack4_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer01::ATTACK5:
			Attack5_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer01::BOUNCE:
			break;
		case Client::CPlayer01::CHANGEBATTLE:
			break;
		case Client::CPlayer01::DASH:
			break;
		case Client::CPlayer01::GROGGYEND:
			break;
		case Client::CPlayer01::GROGGYLOOF:
			break;
		case Client::CPlayer01::GROGGYSTART:
			break;
		case Client::CPlayer01::DOWN:
			break;
		case Client::CPlayer01::VOIDFRONT:
			break;
		case Client::CPlayer01::VOIDAIR:
			break;
		case Client::CPlayer01::VOIDATTACK:
			break;
		case Client::CPlayer01::CHANGEBURST:
			break;
		case Client::CPlayer01::FINISHNORMAL:
			break;
		case Client::CPlayer01::FINISHKICK:
			break;
		case Client::CPlayer01::GETUP:
			break;
		case Client::CPlayer01::RUSHKICK:
			RushKick_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer01::HURTBACK:
			break;
		case Client::CPlayer01::HURTFLY:
			break;
		case Client::CPlayer01::HURTFRONT:
			break;
		case Client::CPlayer01::IDLE1:
			Idle_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer01::IDLE2:
			Idle_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer01::JUMPLOOF:
			JumpLoof_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer01::JUMPSTART:
			JumpStart_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer01::JUMPEND:
			break;
		case Client::CPlayer01::FALLUP:
			break;
		case Client::CPlayer01::RUN:
			Run_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer01::SKILL1:
			Skill1_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer01::SKILL2:
			Skill2_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer01::SKILL3:
			Skill3_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer01::SKILL4_START:
			break;
		case Client::CPlayer01::SKILL4_LOOF:
			Skill4_Loof_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer01::SKILL4_END:
			Skill4End_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer01::SKILL5:
			Skill5_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer01::SKILL6:
			Skill6_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer01::SKILL6_END:
			break;
		case Client::CPlayer01::SKILL6_LOOF:
			break;
		case Client::CPlayer01::SKILL6_START:
			break;
		case Client::CPlayer01::SKILL7:
			Skill7_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer01::SKILL8:
			Skill8_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer01::FALL:
			break;
		case Client::CPlayer01::SKILL9:
			Skill9_KeyInput(fTimeDelta);
			break;
		case Client::CPlayer01::SKILL10:
			break;
		case Client::CPlayer01::SKILL11:
			break;
		case Client::CPlayer01::RUNFASTEND:
			break;
		case Client::CPlayer01::RUNFAST:
			Run_FastInput(fTimeDelta);
			break;
		case Client::CPlayer01::RUNFASTSTART:
			Run_FastStartInput(fTimeDelta);
			break;
		case Client::CPlayer01::STEPBACK:
			break;
		case Client::CPlayer01::LEFTSTEP:
			break;
		case Client::CPlayer01::RIGHTSTEP:
			break;
		case Client::CPlayer01::STUN:
			break;
		case Client::CPlayer01::STUNEND:
			break;
		case Client::CPlayer01::RAGESKILL1:
			break;
		case Client::CPlayer01::RAGESKILL2:
			break;
		case Client::CPlayer01::FALLDOWN:
			break;
		case Client::CPlayer01::HITAWAY:
			break;
		case Client::CPlayer01::HITSPIN:
			break;
		case Client::CPlayer01::HITDOWN:
			break;
		}
	}
	
}

void CPlayer01::Update(_float fTimeDelta)
{
	m_bRangeCol = true;

	if (!m_pModelCom[m_eAnimModel]->GetChangeBool())
	{
		switch (m_eCurState)
		{
		case Client::CPlayer01::AIRATTACK1:
			AirAttack1_Update(fTimeDelta);			
			break;
		case Client::CPlayer01::AIRATTACK2:
			AirAttack2_Update(fTimeDelta);
			break;
		case Client::CPlayer01::AIRATTACK3:
			AirAttack3_Update(fTimeDelta);
			break;
		case Client::CPlayer01::AIRATTACK4:
			AirAttack4_Update(fTimeDelta);
			break;
		case Client::CPlayer01::AIRATTACK5END:
			AirAttack5End_Update(fTimeDelta);
			break;
		case Client::CPlayer01::AIRATTACK5LOOF:
			AirAttack5Loof_Update(fTimeDelta);
			break;
		case Client::CPlayer01::AIRATTACK5START:
			break;
		case Client::CPlayer01::ATTACK1:
			Attack1_Update(fTimeDelta);
			break;
		case Client::CPlayer01::ATTACK2:
			Attack2_Update(fTimeDelta);
			break;
		case Client::CPlayer01::ATTACK3:
			Attack3_Update(fTimeDelta);
			break;
		case Client::CPlayer01::ATTACK4:
			Attack4_Update(fTimeDelta);
			break;
		case Client::CPlayer01::ATTACK5:
			Attack5_Update(fTimeDelta);
			break;
		case Client::CPlayer01::BOUNCE:
			break;
		case Client::CPlayer01::CHANGEBATTLE:
			Change_Update(CHANGEBATTLE, fTimeDelta);
			break;
		case Client::CPlayer01::DASH:
			break;
		case Client::CPlayer01::GROGGYEND:
			GroggyEnd_Update(fTimeDelta);
			break;
		case Client::CPlayer01::GROGGYLOOF:
			GroggyLoof_Update(fTimeDelta);
			break;
		case Client::CPlayer01::GROGGYSTART:
			GroggyStart_Update(fTimeDelta);
			break;
		case Client::CPlayer01::DOWN:
			break;
		case Client::CPlayer01::VOIDFRONT:
			VoidFront_Update(fTimeDelta);
			break;
		case Client::CPlayer01::VOIDAIR:
			break;
		case Client::CPlayer01::VOIDATTACK:
			VoidAttack_Update(fTimeDelta);
			break;
		case Client::CPlayer01::CHANGEBURST:
			Change_Update(CHANGEBURST, fTimeDelta);
			break;
		case Client::CPlayer01::FINISHNORMAL:
			break;
		case Client::CPlayer01::FINISHKICK:
			break;
		case Client::CPlayer01::GETUP:
			break;
		case Client::CPlayer01::RUSHKICK:
			RushKick_Update(fTimeDelta);
			break;
		case Client::CPlayer01::HURTBACK:
			HurtBack_Update(fTimeDelta);
			break;
		case Client::CPlayer01::HURTFLY:
			break;
		case Client::CPlayer01::HURTFRONT:	
			HurtFront_Update(fTimeDelta);
			break;
		case Client::CPlayer01::IDLE1:
			break;
		case Client::CPlayer01::IDLE2:
			break;
		case Client::CPlayer01::JUMPLOOF:
			JumpLoof_Update(fTimeDelta);			
			break;
		case Client::CPlayer01::JUMPSTART:
			JumpStart_Update(fTimeDelta);
			break;
		case Client::CPlayer01::JUMPEND:
			break;
		case Client::CPlayer01::FALLUP:
			break;
		case Client::CPlayer01::RUN:
			Run_Update(fTimeDelta);
			break;
		case Client::CPlayer01::SKILL1:
			Skill1_Update(fTimeDelta);
			break;
		case Client::CPlayer01::SKILL2:
			Skill2_Update(fTimeDelta);
			break;
		case Client::CPlayer01::SKILL3:
			Skill3_Update(fTimeDelta);
			break;
		case Client::CPlayer01::SKILL4_START:
			Skill4Start_Update(fTimeDelta);
			break;
		case Client::CPlayer01::SKILL4_LOOF:
			Skill4Loof_Update(fTimeDelta);
			break;
		case Client::CPlayer01::SKILL4_END:
			Skill4End_Update(fTimeDelta);
			break;
		case Client::CPlayer01::SKILL5:
			Skill5_Update(fTimeDelta);
			break;
		case Client::CPlayer01::SKILL6:
			Skill6_Update(fTimeDelta);
			break;
		case Client::CPlayer01::SKILL6_END:		
			break;
		case Client::CPlayer01::SKILL6_LOOF:
			Skill6Loof_Update(fTimeDelta);
			break;
		case Client::CPlayer01::SKILL6_START:
			Skill6Start_Update(fTimeDelta);		
			break;
		case Client::CPlayer01::SKILL7:
			Skill7_Update(fTimeDelta);
			break;
		case Client::CPlayer01::SKILL8:
			Skill8_Update(fTimeDelta);
			break;
		case Client::CPlayer01::FALL:
			break;
		case Client::CPlayer01::SKILL9:
			Skill9_Update(fTimeDelta);
			break;
		case Client::CPlayer01::SKILL10:
			Skill10_Update(fTimeDelta);
			break;
		case Client::CPlayer01::SKILL11:
			Skill11_Update(fTimeDelta);
			break;
		case Client::CPlayer01::RUNFASTEND:
			break;
		case Client::CPlayer01::RUNFAST:
			RunFast_Update(fTimeDelta);
			break;
		case Client::CPlayer01::RUNFASTSTART:
			break;
		case Client::CPlayer01::STEPBACK:
			break;
		case Client::CPlayer01::LEFTSTEP:
			break;
		case Client::CPlayer01::RIGHTSTEP:
			break;
		case Client::CPlayer01::STUN:
			break;
		case Client::CPlayer01::STUNEND:
			break;
		case Client::CPlayer01::RAGESKILL1:
			RageSkill1_Update(fTimeDelta);
			break;
		case Client::CPlayer01::RAGESKILL2:
			RageSkill2_Update(fTimeDelta);
			break;
		case Client::CPlayer01::FALLDOWN:
			FallDown_Update(fTimeDelta);
			break;
		case Client::CPlayer01::HITAWAY:
			HitAway_Update(fTimeDelta);
			break;
		case Client::CPlayer01::HITSPIN:
			HitSpin_Update(fTimeDelta);
			break;
		case Client::CPlayer01::HITDOWN:
			HitDown_Update(fTimeDelta);
			break;
		}
	}
}

void CPlayer01::Set_AniInfo()
{
	int AniNums = m_pModelCom[MODEL_NORMAL]->Get_NumAnimations();
	int AniIndex = 0;
	for (int i = 0; i < AniNums; ++i)
	{
		m_pModelCom[MODEL_NORMAL]->Set_AnimIndex(AniIndex);
		m_pModelCom[MODEL_BURST]->Set_AnimIndex(AniIndex);
	
		m_pModelCom[MODEL_BURST]->Set_Duration(m_pModelCom[MODEL_NORMAL]->Get_Duration());
		m_pModelCom[MODEL_BURST]->Set_TickPerSecond(m_pModelCom[MODEL_NORMAL]->Get_TickPerSecond());
		m_pModelCom[MODEL_BURST]->SetTimeLimit(m_pModelCom[MODEL_NORMAL]->GetTimeLimit());
		++AniIndex;
	}
}

void CPlayer01::Default_KeyInput(_float fTimeDelta)
{
	if (CK_PRESS(DIK_LSHIFT))
	{
		Set_State(RUNFASTSTART);
		return;
	}

	if (CM_PRESS(DIMK_LBUTTON))
	{
		Set_State(ATTACK1);
		return;
	}

	if (CK_PRESS(DIK_Q))
	{
		Set_State(SKILL1);
		return;
	}

	if (CK_PRESS(DIK_E))
	{
		Set_State(SKILL2);
		return;
	}

	if (CK_PRESS(DIK_F))
	{
		Set_State(SKILL3);
		return;
	}

	if (CK_PRESS(DIK_R))
	{
		Set_State(SKILL4_START);
		return;
	}

	if (CK_PRESS(DIK_V))
	{
		Set_State(RAGESKILL1);
		return;
	}

	if (CK_PRESS(DIK_B))
	{
		Set_State(RAGESKILL2);
		return;
	}

	if (CK_PRESS(DIK_T))
	{
		Set_State(RUSHKICK);
		return;
	}

	if (CK_PRESS(DIK_G))
	{
		Set_State(SKILL7);
		return;
	}

	if (CK_PRESS(DIK_1))
	{
		Set_State(SKILL9);
		return;
	}

	if (CK_PRESS(DIK_2))
	{
		Set_State(SKILL5);
		return;
	}

	if (CK_PRESS(DIK_3))
	{
		Set_State(SKILL6);
		return;
	}

	if (CK_PRESS(DIK_TAB))
	{
		Set_State(SKILL8);
		return;
	}

	if (CK_PRESS(DIK_CAPSLOCK))
	{
		m_bVoid = true;
		m_fVoidSpeed = 10.f;
		m_pModelCom[m_eAnimModel]->Set_AnimIndex(VOIDFRONT);
		m_eCurState = VOIDFRONT;
		m_eNextState = m_eCurState;
		m_pModelCom[m_eAnimModel]->SetNextIndex(m_eNextState);
		Create_Effect(ET_DASH);
		return;
	}

	Change_Mode();
}

void CPlayer01::Change_Mode()
{
	if (CK_DOWN(DIK_H))
	{
		m_ePlayerMode = PMODE_NORMAL;
		m_eAnimModel = MODEL_NORMAL;
		m_eNextState = IDLE1;
		m_pModelCom[m_eAnimModel]->Set_AnimIndex(IDLE1);
		m_eCurState = IDLE1;
		m_bBattleChange = false;
		m_bLKickTrail = false;
		m_bRKickTrail = false;
		((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->Set_Sleep();
		((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->Set_Sleep();
		((CPlayer01HairA*)m_Parts[PARTS_HAIRA])->Set_Wake();

		Get_Colliders("Player_HairCut")->Set_Sleep();
		Get_Colliders("Player_HairCut_Burst")->Set_Sleep();
		Tick_Col(m_pTransformCom->Get_WorldMatrix());

		Update_Parts();
		m_Parts[PARTS_HAIRA]->LateTick(1.f/60.f);
		((CPlayer01HairA*)m_Parts[PARTS_HAIRA])->SetInitPos();
		
	}
	if (CK_DOWN(DIK_J))
	{
		Set_State(CHANGEBATTLE);
	
	}
	if (CK_DOWN(DIK_K))
	{
		Set_State(CHANGEBURST);
	}

}

void CPlayer01::Push_Collider()
{
	if (m_bHurt && !m_bVoid)
		Choose_Col("Player_Body");
	
	if (m_bAttackCol)
		Choose_Col("Player_Attack");

	if(m_bRangeCol)
		Choose_Col("Player_Push_Range");
}

void CPlayer01::Change_Update(STATE eState, _float fTimeDelta)
{
	m_bHurt = false;

	if (eState == CHANGEBATTLE)
	{
		if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(0) + 1)
		{
			if (m_ePlayerMode == PMODE_NORMAL)
			{
				m_ePlayerMode = PMODE_BATTLE;
				((CPlayer01HairA*)m_Parts[PARTS_HAIRA])->Set_Sleep();
				((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->Set_Sleep();
				((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->Set_Wake();

				Get_Colliders("Player_HairCut")->Set_Awake();
				Get_Colliders("Player_HairCut_Burst")->Set_Sleep();
				m_bLKickTrail = false;
				m_bRKickTrail = false;
				Tick_Col(m_pTransformCom->Get_WorldMatrix());

				Update_Parts();
				m_Parts[PARTS_HAIRB]->LateTick(fTimeDelta);
				((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->SetInitPos();

				m_bPlayerLight = true;
				m_fPlayerLightLimit = 0.1f;
				m_eShaderPass = SHADER_LIGHT;
				m_vPlayerLightColor = { 0.f,1.f,0.9f };
				((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->Set_PlayerLight(true);
				((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->Set_PlayerLight(true);
				((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->Set_PlayerLightColor(_float3{ 0.f,1.f,0.9f });
				((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->Set_PlayerLightColor(_float3{ 0.f,1.f,0.9f });
				PTM->Create_InstanceParticle(L"Player01BurstParticle", m_pTransformCom->Get_WorldFloat4x4());
				GI->PlaySoundW(L"HSChangeSound.ogg", SD_PLAYER1_SK, 1.f);
				
			}

			else if (m_ePlayerMode == PMODE_BURST)
			{
				if (!m_bBattleChange)
				{
					m_pModelCom[MODEL_NORMAL]->Set_AnimIndex(CHANGEBATTLE);
					m_pModelCom[MODEL_NORMAL]->SetPlayTime(m_pModelCom[m_eAnimModel]->Get_PlayTime());
					m_ePlayerMode = PMODE_BATTLE;
					m_eAnimModel = MODEL_NORMAL;
					m_bBattleChange = true;
					((CPlayer01HairA*)m_Parts[PARTS_HAIRA])->Set_Sleep();
					((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->Set_Sleep();
					((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->Set_Wake();

					Get_Colliders("Player_HairCut")->Set_Awake();
					Get_Colliders("Player_HairCut_Burst")->Set_Sleep();
					m_bLKickTrail = false;
					m_bRKickTrail = false;
					Tick_Col(m_pTransformCom->Get_WorldMatrix());
			
					Update_Parts();
					m_Parts[PARTS_HAIRB]->LateTick(fTimeDelta);
					((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->SetInitPos();

					m_bPlayerLight = true;
					m_fPlayerLightLimit = 0.1f;
					m_eShaderPass = SHADER_LIGHT;
					m_vPlayerLightColor = { 0.f,1.f,0.9f };
					((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->Set_PlayerLight(true);
					((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->Set_PlayerLight(true);
					((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->Set_PlayerLightColor(_float3{ 0.f,1.f,0.9f });
					((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->Set_PlayerLightColor(_float3{ 0.f,1.f,0.9f });
					PTM->Create_InstanceParticle(L"Player01BurstParticle", m_pTransformCom->Get_WorldFloat4x4());
					GI->PlaySoundW(L"HSChangeSound.ogg", SD_PLAYER1_SK, 1.f);
				}
			}
		}
	}
	else if (eState == CHANGEBURST)
	{

		if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() >= 14.f)
		{
			if (!m_bChangeBurstSound)
			{
				GI->PlaySoundW(L"01A_201_UltCast_01.ogg", SD_PLAYER1_SK, 1.f);
				m_bChangeBurstSound = true;
			}
		}
		if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == 1 && !m_bHeroTest7)
		{
			m_bHeroTest7 = true;

			CRM->Get_Cam()->Save_World();
			CRM->Play_CutScene(14, false, false, m_pTransformCom);
			
		}
		if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == 20 && !m_bHeroTest9)
		{
			m_bHeroTest9 = true;
			CRM->Get_Cam()->Shake(0.02f, 1.f);
			CRM->Get_Cam()->Fov(-0.5f, 1.f, 0.1f);
		}
		if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == 96 && !m_bHeroTest8)
		{
			m_bHeroTest8 = true;
			CRM->End_CutScene();
			CRM->Get_Cam()->Load_World();
		}
		





		if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() >= 14.f && m_pModelCom[m_eAnimModel]->Get_KeyFrame() <= m_pModelCom[m_eAnimModel]->Get_TimeLimit(0) - 13)
		{
			

			m_fChangeEffectAcc += 1.f * fTimeDelta;
			if (m_fChangeEffectAcc >= 0.1f)
			{
				PTM->Create_InstanceParticle(L"Player01ChangeBurstB", m_pTransformCom->Get_WorldFloat4x4());
				PTM->Create_InstanceParticle(L"Player01ChangeBurstB_2", m_pTransformCom->Get_WorldFloat4x4());
				m_fChangeEffectAcc = 0.f;
			}
		}
		if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(0))
		{
			if (!m_bBurstChange)
			{
				GI->PlaySoundW(L"14A_103_FS_03.ogg", SD_PLAYER1_SK, 1.f);
				m_pModelCom[MODEL_BURST]->Set_AnimIndex(CHANGEBURST);
				m_pModelCom[MODEL_BURST]->SetPlayTime(m_pModelCom[m_eAnimModel]->Get_PlayTime());
				m_ePlayerMode = PMODE_BURST;
				m_eAnimModel = MODEL_BURST;
				m_bBurstChange = true;		
				((CPlayer01HairA*)m_Parts[PARTS_HAIRA])->Set_Sleep();
				((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->Set_Sleep();
				((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->Set_Wake();

				Get_Colliders("Player_HairCut")->Set_Sleep();
				Get_Colliders("Player_HairCut_Burst")->Set_Awake();
				m_bLKickTrail = true;
				m_bRKickTrail = true;
				Tick_Col(m_pTransformCom->Get_WorldMatrix());

				//Update_Parts();
				//m_Parts[PARTS_HAIRA]->LateTick(fTimeDelta);
				//((CPlayer01HairA*)m_Parts[PARTS_HAIRA])->SetInitPos();

				//Update_Parts();
				//m_Parts[PARTS_HAIRB]->LateTick(fTimeDelta);
				//((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->SetInitPos();

				Update_Parts();
				m_Parts[PARTS_HAIRC]->LateTick(fTimeDelta);
				((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->SetInitPos();

				m_bPlayerLight = true;
				m_fPlayerLightLimit = 0.1f;
				m_eShaderPass = SHADER_LIGHT;
				m_vPlayerLightColor = { 1.f,0.5f,0.f };
				((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->Set_PlayerLight(true);
				((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->Set_PlayerLight(true);
				((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->Set_PlayerLightColor(_float3{ 0.f,1.f,0.9f });
				((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->Set_PlayerLightColor(_float3{ 0.f,1.f,0.9f });
				PTM->Create_InstanceParticle(L"Player01BurstParticleB", m_pTransformCom->Get_WorldFloat4x4());
				PTM->Create_InstanceParticle(L"Player01ChangeBurstB_3", m_pTransformCom->Get_WorldFloat4x4());

				CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
				Info->fBlurPower = 2.f;
				Info->vColor = { 0.7f,0.7f,0.7f,1.f };
				Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
				Info->vLocalPos = { 0.f,0.f,1.f,1.f };
				Info->vRatio = { 1.f,1.f };
				Info->vRatioSpeed = { 2.f,2.f };
				Info->vScale = { 0.1f,0.1f,0.1f };
				Info->vScaleSpeed = { 250.f,250.f,250.f };
				Info->vUvFixSpeed = { 1.f,3.f };
				Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);

			}
		}
	}
}

void CPlayer01::Change_ColRange(_float3 vSize, _float3 vCenter)
{
	Get_Colliders("Player_Attack")->Set_Size(vSize);
	Get_Colliders("Player_Attack")->Set_Center(vCenter);
}

void CPlayer01::Hit_State(CCreture* pOther)
{
	Reset_Collider();

		m_fPushPower = pOther->Get_PushPower();
		m_fAirPower = pOther->Get_AirPower();
		m_fNowHp -= (pOther->Get_Damage() - STM->Get_FinalStamina(CStatus_Manager::PLAYER01));
		if (m_fNowHp <= 0.f)
		{
			m_fNowHp = 0.f;
			Set_State(GROGGYSTART);
			m_bVoid = true;
			return;
		}

		m_bRim = true;

		UM->Set_ShakeOn1(true);

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

void CPlayer01::Reset_Collider()
{
	m_bAttackCol = false;
	m_bAirAttack1Col = false;	m_bAirAttack2Col = false;	m_bAirAttack3Col = false;	m_bAirAttack5Col = false;
	m_bAttack1Col = false;	m_bAttack2Col = false;	m_bAttack3Col = false;	m_bAttack4Col = false;
	m_bAttack5Col = false;	m_bVoidAttackCol = false;	m_bRushKickCol = false;	m_bSkill1Col = false;	
	m_bSkill2Col_1 = false;	m_bSkill2Col_2 = false;	m_bSkill2Col_3 = false;
	m_bSkill2Col_4 = false;	m_bSkill3Col_1 = false;	m_bSkill3Col_2 = false;	m_bSkill4LoofCol_1 = false;
	m_bSkill4LoofCol_2 = false;	m_bSkill4EndCol = false;	m_bSkill5Col = false;	m_bSkill6Col = false;
	m_bSkill6StartCol = false;	m_bSkill7Col_1 = false;	m_bSkill7Col_2 = false;	m_bSkill7Col_3 = false;
	m_bSkill7Col_4 = false;	m_bSkill7Col_5 = false;	m_bSkill7Col_6 = false;	m_bSkill8Col = false;
	m_bSkill9Col = false;	m_bSkill10Col = false;	m_bSkill11Col = false;	m_bRageSkill1Col_1 = false;
	m_bRageSkill1Col_2 = false;	m_bRageSkill1Col_3 = false;	m_bRageSkill1Col_4 = false;	m_bRageSkill1Col_5 = false;
	m_bRageSkill1Col_6 = false;	m_bRageSkill1Col_7 = false;	m_bRageSkill2Col_1 = false;	m_bRageSkill2Col_2 = false;
	m_bRageSkill2Col_3 = false; m_bRageSkill2Col_4 = false;	m_bRageSkill2Col_5 = false;	m_bRageSkill2Col_6 = false;
	m_bRageSkill2Col_7 = false;	m_bRageSkill2Col_8 = false;	m_bRageSkill2Col_9 = false;	m_bRageSkill2Col_10 = false;
	m_bRageSkill2Col_11 = false;	m_bRageSkill2Col_12 = false;	m_bRageSkill2Col_13 = false;	m_bRageSkill2Col_14 = false;
	m_bLHandTrail = false;
	m_bRHandTrail = false;
}

void CPlayer01::Check_Burst(_float fTimeDelta, _float OriginTimeDelta)
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
			if (m_bBurstEffect)
			{
				m_fBurstParticleAcc += 1.f * OriginTimeDelta;
				if (m_fBurstParticleAcc >= m_fBurstParticleLimit)
				{
					Create_ParticleInstance(PTI_BURST);
					m_fBurstParticleAcc = 0.f;
				}
			}
			m_bMotionTrail = true;
			m_fNowEP -= 0.5f * fTimeDelta;
			m_fPreEp -= 0.5f * fTimeDelta;

			if (m_fNowEP <= 0.f)
			{
				m_fNowEP = 0.f;
				m_fPreEp = 0.f;
				m_fNowBP = 0.f;
				m_pModelCom[MODEL_NORMAL]->Set_AnimIndex(m_eCurState);
				m_pModelCom[MODEL_NORMAL]->SetPlayTime(m_pModelCom[m_eAnimModel]->Get_PlayTime());
				m_ePlayerMode = PMODE_BATTLE;
				m_eAnimModel = MODEL_NORMAL;
				Update_Parts();
				((CPlayer01HairA*)m_Parts[PARTS_HAIRA])->Set_Sleep();
				((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->Set_Sleep();
				((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->Set_Wake();
				((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->SetInitPos();
				Get_Colliders("Player_HairCut")->Set_Awake();
				Get_Colliders("Player_HairCut_Burst")->Set_Sleep();
				m_bLKickTrail = false;
				m_bRKickTrail = false;
			}
		}
		else if (m_fNowEP < m_fMaxEp)
		{
			m_fNowEP += 5.f * fTimeDelta;
			m_fPreEp += 5.f * fTimeDelta;

			if (m_fNowEP >= m_fMaxEp)
			{
				m_fNowEP = m_fMaxEp;
				m_fPreEp = m_fMaxEp;
			}
		}
	}
}

void CPlayer01::Check_Rim(_float fTimeDelta)
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

HRESULT CPlayer01::Set_RenderPass(SHADER eShader)
{
	_uint		iNumMeshes = m_pModelCom[m_eAnimModel]->Get_NumMeshes();



	switch (eShader)
	{
	case Client::CPlayer01::SHADER_DEFAULT:
		for (_uint i = 0; i < iNumMeshes; ++i)
		{		
			if (FAILED(m_pModelCom[m_eAnimModel]->SetUp_OnShader(m_pShaderCom, m_pModelCom[m_eAnimModel]->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;

			if (FAILED(m_pModelCom[m_eAnimModel]->Render(m_pShaderCom, i, 0)))
					return E_FAIL;	
		}
		break;
	case Client::CPlayer01::SHADER_RIM:
		for (_uint i = 0; i < iNumMeshes; ++i)
		{		
			if (FAILED(m_pShaderCom->Set_RawValue("g_CamPosition", &GI->Get_CamPosition(), sizeof(_float4))))
				return E_FAIL;
			_float g_RimPower = 1.5f;
			if (FAILED(m_pShaderCom->Set_RawValue("g_RimPower", &g_RimPower, sizeof(_float))))
				return E_FAIL;

			if (FAILED(m_pModelCom[m_eAnimModel]->SetUp_OnShader(m_pShaderCom, m_pModelCom[m_eAnimModel]->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;

			if (FAILED(m_pModelCom[m_eAnimModel]->Render(m_pShaderCom, i, 1)))
					return E_FAIL;		
		}
		break;
	case Client::CPlayer01::SHADER_LIGHT:
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

			if (FAILED(m_pModelCom[m_eAnimModel]->SetUp_OnShader(m_pShaderCom, m_pModelCom[m_eAnimModel]->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;

			if (FAILED(m_pModelCom[m_eAnimModel]->Render(m_pShaderCom, i, 11)))
				return E_FAIL;
		}
	}
	}

	return S_OK;
}

void CPlayer01::Check_HP(_float fTimeDelta)
{
	if (m_bReHp)
	{
		m_fNowHp += 300.f * fTimeDelta;
		m_fPreHp += 300.f * fTimeDelta;

		if (m_fPreHp >= STM->Get_FinalHp(CStatus_Manager::PLAYER01))
		{
			m_fPreHp = STM->Get_FinalHp(CStatus_Manager::PLAYER01);
			m_fNowHp = m_fPreHp;
			m_bReHp = false;
		}
	}
	
	if (m_fPreHp < m_fNowHp) //hp 찰때
	{
		m_fPreHp += 400.f * fTimeDelta;

		if (m_fPreHp >= m_fNowHp)
		{
			m_fPreHp = m_fNowHp;
		}
	}

	else if (m_fPreHp > m_fNowHp) // hp 달때
	{
		m_fPreHp -= 200.f * fTimeDelta;

		if (m_fPreHp <= m_fNowHp)
		{
			m_fPreHp = m_fNowHp;
		}
	}
}

void CPlayer01::Check_SP(_float fTimeDelta)
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
			m_bMotionTrail = true;
			m_fNowSP -= 10.f * fTimeDelta;
			m_fPreSp -= 10.f * fTimeDelta;
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

void CPlayer01::Check_BP(_float fTimeDelta)
{
	if (m_ePlayerMode == PMODE_BURST && !m_bBurstSkillOn)
	{		
		m_fNowBP += 5.f;

		if (m_fNowBP > 100.f)
			m_fNowBP = 100.f;
	}
}

HRESULT CPlayer01::Create_Particle(PARTICLE ePT)
{
	switch (ePT)
	{
	case Client::CPlayer01::PT_SUNDER:		
		if (m_ePlayerMode != PMODE_BURST)
		{
			if (FAILED(PTM->Create_AlphaParticle(L"Player01Sunder", m_pTransformCom->Get_WorldFloat4x4())))
				return E_FAIL;
			if (FAILED(PTM->Create_AlphaParticle(L"Player01Sunder2", m_pTransformCom->Get_WorldFloat4x4())))
				return E_FAIL;
			if (FAILED(PTM->Create_AlphaParticle(L"Player01SunderA", m_pTransformCom->Get_WorldFloat4x4())))
				return E_FAIL;
			if (FAILED(PTM->Create_AlphaParticle(L"Player01SunderB", m_pTransformCom->Get_WorldFloat4x4())))
				return E_FAIL;
		}
		else
		{
			if (FAILED(PTM->Create_AlphaParticle(L"BPlayer01Sunder", m_pTransformCom->Get_WorldFloat4x4())))
				return E_FAIL;
			if (FAILED(PTM->Create_AlphaParticle(L"BPlayer01Sunder2", m_pTransformCom->Get_WorldFloat4x4())))
				return E_FAIL;
			if (FAILED(PTM->Create_AlphaParticle(L"BPlayer01SunderA", m_pTransformCom->Get_WorldFloat4x4())))
				return E_FAIL;
			if (FAILED(PTM->Create_AlphaParticle(L"Player01SunderB", m_pTransformCom->Get_WorldFloat4x4())))
				return E_FAIL;
		}

		break;

	}
	return S_OK;

}

HRESULT CPlayer01::Create_ParticleInstance(PARTICLEINSTANCE ePTI)
{
	switch (ePTI)
	{
	case Client::CPlayer01::PTI_BURST:
		PTM->Create_InstanceParticle(L"Player01Burst", m_pTransformCom->Get_WorldFloat4x4());
		break;
	
	}
	return S_OK;
}

HRESULT CPlayer01::Create_Effect(EFFECT eET)
{
	
	switch (eET)
	{
		case Client::CPlayer01::ET_SUNDER:	
		{
			Create_Sunder();			
			break;
		}
		
		case Client::CPlayer01::ET_ATTACK1:
		{
			if (m_ePlayerMode != PMODE_BURST)
			{
				CPlayer01Trail_1::TRAILINFO* Info = new CPlayer01Trail_1::TRAILINFO;
				Info->vColor = { 0.2f,1.f,0.9f,1.f };
				Info->vColor2 = { 0.f,0.f,0.f,1.f };
				Info->vRatio = { 5.f,1.f };
				Info->vRatioSpeed = { 30.f,0.f };
				_matrix Effectmatrix;
				Effectmatrix.r[0] = { -3.83f, -3.21f, 0.f, 0.f };
				Effectmatrix.r[1] = { -3.21f, 3.83f, 0.f, 0.f };
				Effectmatrix.r[2] = { 0.f, 0.f, -5.f, 0.f };
				Effectmatrix.r[3] = { -0.11f, 0.79f, 0.7f, 1.f };
				XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
				Info->fBlurPower = 0.2f;
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Trail_1", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
					return E_FAIL;
			}
			else
			{
				CPlayer01Trail_1::TRAILINFO* Info = new CPlayer01Trail_1::TRAILINFO;
				Info->vColor = { 1.f,0.6f,0.f,1.f };
				Info->vColor2 = { 1.f,1.f,1.f,1.f };
				Info->vRatio = { 1.5f,1.f };
				Info->vRatioSpeed = { 10.f,0.f };
				_matrix Effectmatrix;
				Effectmatrix.r[0] = { -3.83f, -3.21f, 0.f, 0.f };
				Effectmatrix.r[1] = { -3.21f, 3.83f, 0.f, 0.f };
				Effectmatrix.r[2] = { 0.f, 0.f, -5.f, 0.f };
				Effectmatrix.r[3] = { -0.11f, 0.79f, 0.7f, 1.f };
				XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
				Info->fBlurPower = 0.1f;
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Trail_1", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
					return E_FAIL;
			}

	

			break;
		}
		case Client::CPlayer01::ET_ATTACK2:
		{
			if (m_ePlayerMode != PMODE_BURST)
			{
				CPlayer01Trail_1::TRAILINFO* Info = new CPlayer01Trail_1::TRAILINFO;
				Info->vColor = { 0.2f,1.f,0.9f,1.f };
				Info->vColor2 = { 0.f,0.f,0.f,1.f };
				Info->vRatio = { 5.f,1.f };
				Info->vRatioSpeed = { 30.f,0.f };
				_matrix Effectmatrix;
				Effectmatrix.r[0] = { 3.21f, 3.83f, 0.f, 0.f };
				Effectmatrix.r[1] = { 3.83f, -3.21f, 0.f, 0.f };
				Effectmatrix.r[2] = { 0.f, 0.f, -5.f, 0.f };
				Effectmatrix.r[3] = { 0.02f, 1.f, 0.7f, 1.f };
				XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
				Info->fBlurPower = 0.2f;
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Trail_1", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
					return E_FAIL;
			}
			else
			{
				CPlayer01Trail_1::TRAILINFO* Info = new CPlayer01Trail_1::TRAILINFO;
				Info->vColor = { 1.f,0.6f,0.f,1.f };
				Info->vColor2 = { 1.f,1.f,1.f,1.f };
				Info->vRatio = { 1.5f,1.f };
				Info->vRatioSpeed = { 10.f,0.f };
				_matrix Effectmatrix;
				Effectmatrix.r[0] = { 3.21f, 3.83f, 0.f, 0.f };
				Effectmatrix.r[1] = { 3.83f, -3.21f, 0.f, 0.f };
				Effectmatrix.r[2] = { 0.f, 0.f, -5.f, 0.f };
				Effectmatrix.r[3] = { 0.02f, 1.f, 0.7f, 1.f };
				XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
				Info->fBlurPower = 0.1f;
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Trail_1", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
					return E_FAIL;
			}

			break;
		}
		case Client::CPlayer01::ET_ATTACK3:
		{
			if (m_ePlayerMode != PMODE_BURST)
			{
				CPlayer01Trail_1::TRAILINFO* Info = new CPlayer01Trail_1::TRAILINFO;
				Info->vColor = { 0.2f,1.f,0.9f,1.f };
				Info->vColor2 = { 0.f,0.f,0.f,1.f };
				Info->vRatio = { 5.f,1.f };
				Info->vRatioSpeed = { 30.f,0.f };
				_matrix Effectmatrix;
				Effectmatrix.r[0] = { 8.f, -0.f, 0.f, 0.f };
				Effectmatrix.r[1] = { 0.f, -7.51f, 2.73f, 0.f };
				Effectmatrix.r[2] = { 0.f, -2.73f, -7.51f, 0.f };
				Effectmatrix.r[3] = { 0.f, 1.3f, 1.2f, 1.f };
				XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
				Info->fBlurPower = 0.2f;
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Trail_1", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
					return E_FAIL;
			}
			else
			{
				CPlayer01Trail_1::TRAILINFO* Info = new CPlayer01Trail_1::TRAILINFO;
				Info->vColor = { 1.f,0.6f,0.f,1.f };
				Info->vColor2 = { 1.f,1.f,1.f,1.f };
				Info->vRatio = { 1.5f,1.f };
				Info->vRatioSpeed = { 10.f,0.f };
				_matrix Effectmatrix;
				Effectmatrix.r[0] = { 8.f, -0.f, 0.f, 0.f };
				Effectmatrix.r[1] = { 0.f, -7.51f, 2.73f, 0.f };
				Effectmatrix.r[2] = { 0.f, -2.73f, -7.51f, 0.f };
				Effectmatrix.r[3] = { 0.f, 1.3f, 1.2f, 1.f };
				XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
				Info->fBlurPower = 0.1f;
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Trail_1", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
					return E_FAIL;
			}

			break;
		}
		case Client::CPlayer01::ET_ATTACK4:
		{
			if (m_ePlayerMode != PMODE_BURST)
			{
				CPlayer01Trail_1::TRAILINFO* Info = new CPlayer01Trail_1::TRAILINFO;
				Info->vColor = { 0.2f,1.f,0.9f,1.f };
				Info->vColor2 = { 0.f,0.f,0.f,1.f };
				Info->vRatio = { 5.f,1.f };
				Info->vRatioSpeed = { 30.f,0.f };
				_matrix Effectmatrix;
				Effectmatrix.r[0] = { 0.f, 8.f, 0.f, 0.f };
				Effectmatrix.r[1] = { 8.f, 0.f, 0.f, 0.f };
				Effectmatrix.r[2] = { 0.f, 0.f, -8.f, 0.f };
				Effectmatrix.r[3] = { 0.f, 0.97f, 0.7f, 1.f };
				XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
				Info->fBlurPower = 0.2f;
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Trail_1", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
					return E_FAIL;
			}
			else
			{
				CPlayer01Trail_1::TRAILINFO* Info = new CPlayer01Trail_1::TRAILINFO;
				Info->vColor = { 1.f,0.6f,0.f,1.f };
				Info->vColor2 = { 1.f,1.f,1.f,1.f };
				Info->vRatio = { 1.5f,1.f };
				Info->vRatioSpeed = { 10.f,0.f };
				_matrix Effectmatrix;
				Effectmatrix.r[0] = { 0.f, 8.f, 0.f, 0.f };
				Effectmatrix.r[1] = { 8.f, 0.f, 0.f, 0.f };
				Effectmatrix.r[2] = { 0.f, 0.f, -8.f, 0.f };
				Effectmatrix.r[3] = { 0.f, 0.97f, 0.7f, 1.f };
				XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
				Info->fBlurPower = 0.1f;
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Trail_1", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
					return E_FAIL;
			}

			break;
		}

		case Client::CPlayer01::ET_ATTACK5:
		{
			CWind::WINDINFO* Info = new CWind::WINDINFO;
			Info->vColor = { 0.7f,0.7f,0.7f,1.f };
			Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
			Info->vFixPosSpeed = { 0.f,0.f,3.f };
			Info->vFixScaleSpeed = { 30.f,30.f,30.f };
			Info->vRatio = { 1.f,1.f };
			Info->vRatioSpeed = { 2.f,2.f };
			Info->vUVFixSpeed = { 1.f,-3.f };
			_matrix Effectmatrix;
			Effectmatrix.r[0] = { 0.1f, 0.f, 0.f, 0.f };
			Effectmatrix.r[1] = { 0.f, 0.f, 1.f, 0.f };
			Effectmatrix.r[2] = { 0.f, -1.f, 0.f, 0.f };
			Effectmatrix.r[3] = { 0.f, 1.f, 0.5f, 1.f };
			XMStoreFloat4x4(&Info->WorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
			if (FAILED(GI->Add_GameObjectToLayer(L"Wind", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
				return E_FAIL;

			CWind::WINDINFO* Info2 = new CWind::WINDINFO;
			Info2->vColor = { 0.7f,0.7f,0.7f,1.f };
			Info2->vColor2 = { 0.7f,0.7f,0.7f,1.f };
			Info2->vFixPosSpeed = { 0.f,0.f,5.f };
			Info2->vFixScaleSpeed = { 40.f,40.f,40.f };
			Info2->vRatio = { 1.f,1.f };
			Info2->vRatioSpeed = { 2.f,2.f };
			Info2->vUVFixSpeed = { 1.f,-3.f };
			Effectmatrix.r[0] = { 0.1f, 0.f, 0.f, 0.f };
			Effectmatrix.r[1] = { 0.f, 0.f, 1.f, 0.f };
			Effectmatrix.r[2] = { 0.f, -1.f, 0.f, 0.f };
			Effectmatrix.r[3] = { 0.f, 1.f, 0.5f, 1.f };
			XMStoreFloat4x4(&Info2->WorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
			if (FAILED(GI->Add_GameObjectToLayer(L"Wind", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info2)))
				return E_FAIL;

			PTM->Create_InstanceParticle(L"Player01Distortion", m_pTransformCom->Get_WorldFloat4x4());

			if (m_ePlayerMode != PMODE_BURST)
			{
				PTM->Create_InstanceParticle(L"Player01Attack5", m_pTransformCom->Get_WorldFloat4x4());
				PTM->Create_InstanceParticle(L"Player01Skill1End", m_pTransformCom->Get_WorldFloat4x4());
			}
			else
			{
				PTM->Create_InstanceParticle(L"Player01Attack5B", m_pTransformCom->Get_WorldFloat4x4());
				PTM->Create_InstanceParticle(L"Player01Skill1End_2", m_pTransformCom->Get_WorldFloat4x4());
			}				

			break;
		}

		case Client::CPlayer01::ET_DASH:
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


		case Client::CPlayer01::ET_AIRATTACK1:
		{
			if (m_ePlayerMode != PMODE_BURST)
			{
				CPlayer01Trail_1::TRAILINFO* Info = new CPlayer01Trail_1::TRAILINFO;
				Info->vColor = { 0.2f,1.f,0.9f,1.f };
				Info->vColor2 = { 0.f,0.f,0.f,1.f };
				Info->vRatio = { 5.f,1.f };
				Info->vRatioSpeed = { 30.f,0.f };
				_matrix Effectmatrix;
				Effectmatrix.r[0] = { -0.86f, 4.92f, 0.f, 0.f };
				Effectmatrix.r[1] = { 4.92f, 0.86f, 0.f, 0.f };
				Effectmatrix.r[2] = { 0.f, 0.f, -5.f, 0.f };
				Effectmatrix.r[3] = { -0.1f, 1.51f, 1.f, 1.f };
				XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
				Info->fBlurPower = 0.2f;
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Trail_1", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
					return E_FAIL;
			}
			else
			{
				CPlayer01Trail_1::TRAILINFO* Info = new CPlayer01Trail_1::TRAILINFO;
				Info->vColor = { 1.f,0.6f,0.f,1.f };
				Info->vColor2 = { 1.f,1.f,1.f,1.f };
				Info->vRatio = { 1.5f,1.f };
				Info->vRatioSpeed = { 10.f,0.f };
				_matrix Effectmatrix;
				Effectmatrix.r[0] = { -0.86f, 4.92f, 0.f, 0.f };
				Effectmatrix.r[1] = { 4.92f, 0.86f, 0.f, 0.f };
				Effectmatrix.r[2] = { 0.f, 0.f, -5.f, 0.f };
				Effectmatrix.r[3] = { -0.1f, 1.51f, 1.f, 1.f };
				XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
				Info->fBlurPower = 0.1f;
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Trail_1", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
					return E_FAIL;
			}

			break;
		}

		case Client::CPlayer01::ET_AIRATTACK2:
		{
			if (m_ePlayerMode != PMODE_BURST)
			{
				CPlayer01Trail_1::TRAILINFO* Info = new CPlayer01Trail_1::TRAILINFO;
				Info->vColor = { 0.2f,1.f,0.9f,1.f };
				Info->vColor2 = { 0.f,0.f,0.f,1.f };
				Info->vRatio = { 5.f,1.f };
				Info->vRatioSpeed = { 30.f,0.f };
				_matrix Effectmatrix;
				Effectmatrix.r[0] = { 4.7f, -1.71f, 0.f, 0.f };
				Effectmatrix.r[1] = { -1.71f, -4.7f, 0.f, 0.f };
				Effectmatrix.r[2] = { 0.f, 0.f, -5.f, 0.f };
				Effectmatrix.r[3] = { 0.1f, 1.2f, 1.f, 1.f };
				XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
				Info->fBlurPower = 0.2f;
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Trail_1", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
					return E_FAIL;
			}
			else
			{
				CPlayer01Trail_1::TRAILINFO* Info = new CPlayer01Trail_1::TRAILINFO;
				Info->vColor = { 1.f,0.6f,0.f,1.f };
				Info->vColor2 = { 1.f,1.f,1.f,1.f };
				Info->vRatio = { 1.5f,1.f };
				Info->vRatioSpeed = { 10.f,0.f };
				_matrix Effectmatrix;
				Effectmatrix.r[0] = { 4.7f, -1.71f, 0.f, 0.f };
				Effectmatrix.r[1] = { -1.71f, -4.7f, 0.f, 0.f };
				Effectmatrix.r[2] = { 0.f, 0.f, -5.f, 0.f };
				Effectmatrix.r[3] = { 0.1f, 1.2f, 1.f, 1.f };
				XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
				Info->fBlurPower = 0.1f;
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Trail_1", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
					return E_FAIL;
			}

			break;
		}

		case Client::CPlayer01::ET_AIRATTACK3:
		{
			if (m_ePlayerMode != PMODE_BURST)
			{
				CPlayer01Trail_1::TRAILINFO* Info = new CPlayer01Trail_1::TRAILINFO;
				Info->vColor = { 0.2f,1.f,0.9f,1.f };
				Info->vColor2 = { 0.f,0.f,0.f,1.f };
				Info->vRatio = { 5.f,1.f };
				Info->vRatioSpeed = { 30.f,0.f };
				_matrix Effectmatrix;
				Effectmatrix.r[0] = { 1.29f, -4.82f, 0.f, 0.f };
				Effectmatrix.r[1] = { -4.82f, -1.29f, 0.f, 0.f };
				Effectmatrix.r[2] = { 0.f, 0.f, -5.f, 0.f };
				Effectmatrix.r[3] = { 0.f, 0.9f, 1.f, 1.f };
				XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
				Info->fBlurPower = 0.2f;
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Trail_1", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
					return E_FAIL;
			}
			else
			{
				CPlayer01Trail_1::TRAILINFO* Info = new CPlayer01Trail_1::TRAILINFO;
				Info->vColor = { 1.f,0.6f,0.f,1.f };
				Info->vColor2 = { 1.f,1.f,1.f,1.f };
				Info->vRatio = { 1.5f,1.f };
				Info->vRatioSpeed = { 10.f,0.f };
				_matrix Effectmatrix;
				Effectmatrix.r[0] = { 1.29f, -4.82f, 0.f, 0.f };
				Effectmatrix.r[1] = { -4.82f, -1.29f, 0.f, 0.f };
				Effectmatrix.r[2] = { 0.f, 0.f, -5.f, 0.f };
				Effectmatrix.r[3] = { 0.f, 0.9f, 1.f, 1.f };
				XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
				Info->fBlurPower = 0.1f;
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Trail_1", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
					return E_FAIL;
			}

			break;
		}

		case Client::CPlayer01::ET_SKILL4LEFT:
		{
			

			if (m_ePlayerMode != PMODE_BURST)
			{
				CPlayer01Trail_1::TRAILINFO* Info = new CPlayer01Trail_1::TRAILINFO;
				Info->vColor = { 0.2f,1.f,0.9f,1.f };
				Info->vColor2 = { 0.f,0.f,0.f,1.f };
				Info->vRatio = { 5.f,1.f };
				Info->vRatioSpeed = { 40.f,0.f };
				_matrix Effectmatrix;
				Effectmatrix.r[0] = { 7.51f, -2.73f, 0.f, 0.f };
				Effectmatrix.r[1] = { -2.73f, -7.51f, 0.f, 0.f };
				Effectmatrix.r[2] = { 0.f, 0.f, -8.f, 0.f };
				Effectmatrix.r[3] = { 0.6f, 0.8f, 1.f, 1.f };
				XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
				Info->fBlurPower = 0.2f;
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Trail_1", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
					return E_FAIL;
			}
			else
			{
				CPlayer01Trail_1::TRAILINFO* Info = new CPlayer01Trail_1::TRAILINFO;
				Info->vColor = { 1.f,0.6f,0.f,1.f };
				Info->vColor2 = { 1.f,1.f,1.f,1.f };
				Info->vRatio = { 1.5f,1.f };
				Info->vRatioSpeed = { 10.f,0.f };
				_matrix Effectmatrix;
				Effectmatrix.r[0] = { 7.51f, -2.73f, 0.f, 0.f };
				Effectmatrix.r[1] = { -2.73f, -7.51f, 0.f, 0.f };
				Effectmatrix.r[2] = { 0.f, 0.f, -8.f, 0.f };
				Effectmatrix.r[3] = { 0.6f, 0.8f, 1.f, 1.f };
				XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
				Info->fBlurPower = 0.1f;
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Trail_1", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
					return E_FAIL;
			}

			break;
		}

		case Client::CPlayer01::ET_SKILL4RIGHT:
		{

			if (m_ePlayerMode != PMODE_BURST)
			{
				CPlayer01Trail_1::TRAILINFO* Info = new CPlayer01Trail_1::TRAILINFO;
				Info->vColor = { 0.2f,1.f,0.9f,1.f };
				Info->vColor2 = { 0.f,0.f,0.f,1.f };
				Info->vRatio = { 5.f,1.f };
				Info->vRatioSpeed = { 40.f,0.f };
				_matrix Effectmatrix;
				Effectmatrix.r[0] = { -7.51f, -2.73f, 0.f, 0.f };
				Effectmatrix.r[1] = { -2.73f, 7.51f, 0.f, 0.f };
				Effectmatrix.r[2] = { 0.f, 0.f, -8.f, 0.f };
				Effectmatrix.r[3] = { -0.6f, 0.8f, 1.f, 1.f };
				XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
				Info->fBlurPower = 0.2f;
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Trail_1", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
					return E_FAIL;
			}
			else
			{
				CPlayer01Trail_1::TRAILINFO* Info = new CPlayer01Trail_1::TRAILINFO;
				Info->vColor = { 1.f,0.6f,0.f,1.f };
				Info->vColor2 = { 1.f,1.f,1.f,1.f };
				Info->vRatio = { 1.5f,1.f };
				Info->vRatioSpeed = { 10.f,0.f };
				_matrix Effectmatrix;
				Effectmatrix.r[0] = { -7.51f, -2.73f, 0.f, 0.f };
				Effectmatrix.r[1] = { -2.73f, 7.51f, 0.f, 0.f };
				Effectmatrix.r[2] = { 0.f, 0.f, -8.f, 0.f };
				Effectmatrix.r[3] = { -0.6f, 0.8f, 1.f, 1.f };
				XMStoreFloat4x4(&Info->vWorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
				Info->fBlurPower = 0.1f;
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Trail_1", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
					return E_FAIL;
			}

			break;
		}

		case Client::CPlayer01::ET_SKILL4END:
		{
			CWind::WINDINFO* Info = new CWind::WINDINFO;
			Info->vColor = { 0.7f,0.7f,0.7f,1.f };
			Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
			Info->vFixPosSpeed = { 0.f,0.f,3.f };
			Info->vFixScaleSpeed = { 30.f,30.f,30.f };
			Info->vRatio = { 1.f,1.f };
			Info->vRatioSpeed = { 2.f,2.f };
			Info->vUVFixSpeed = { 1.f,-3.f };
			_matrix Effectmatrix;
			Effectmatrix.r[0] = { 0.1f, 0.f, 0.f, 0.f };
			Effectmatrix.r[1] = { 0.f, 0.f, 1.f, 0.f };
			Effectmatrix.r[2] = { 0.f, -1.f, 0.f, 0.f };
			Effectmatrix.r[3] = { 0.f, 1.f, 0.5f, 1.f };
			XMStoreFloat4x4(&Info->WorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
			if (FAILED(GI->Add_GameObjectToLayer(L"Wind", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
				return E_FAIL;

			CWind::WINDINFO* Info2 = new CWind::WINDINFO;
			Info2->vColor = { 0.7f,0.7f,0.7f,1.f };
			Info2->vColor2 = { 0.7f,0.7f,0.7f,1.f };
			Info2->vFixPosSpeed = { 0.f,0.f,5.f };
			Info2->vFixScaleSpeed = { 40.f,40.f,40.f };
			Info2->vRatio = { 1.f,1.f };
			Info2->vRatioSpeed = { 2.f,2.f };
			Info2->vUVFixSpeed = { 1.f,-3.f };
			Effectmatrix.r[0] = { 0.1f, 0.f, 0.f, 0.f };
			Effectmatrix.r[1] = { 0.f, 0.f, 1.f, 0.f };
			Effectmatrix.r[2] = { 0.f, -1.f, 0.f, 0.f };
			Effectmatrix.r[3] = { 0.f, 1.f, 0.5f, 1.f };
			XMStoreFloat4x4(&Info2->WorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
			if (FAILED(GI->Add_GameObjectToLayer(L"Wind", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info2)))
				return E_FAIL;

			PTM->Create_InstanceParticle(L"Player01Distortion", m_pTransformCom->Get_WorldFloat4x4());

			if (m_ePlayerMode != PMODE_BURST)
			{
				PTM->Create_InstanceParticle(L"Player01Skill4End", m_pTransformCom->Get_WorldFloat4x4());
				PTM->Create_InstanceParticle(L"Player01Skill1End", m_pTransformCom->Get_WorldFloat4x4());
			}
			else
			{
				PTM->Create_InstanceParticle(L"Player01Skill4EndB", m_pTransformCom->Get_WorldFloat4x4());
				PTM->Create_InstanceParticle(L"Player01Skill1End_2", m_pTransformCom->Get_WorldFloat4x4());
			}

			break;
		}
		case Client::CPlayer01::ET_SKILLCHARGE:
		{
			CEffectSpin::EFFECTSPINDESC EffectSpinDesc;
			EffectSpinDesc.iSpinType = 0;
			EffectSpinDesc.iPlayerType = CEffectSpin::PLAYERTYPE::PLAYERTYPE_YUMO;
			if (FAILED(GI->Add_GameObjectToLayer(TEXT("EffectSpin"), GI->Get_Cur_LevelIndex(), TEXT("Layer_KSEFFECT"), &EffectSpinDesc)))
				return E_FAIL;
			EffectSpinDesc.iSpinType = 1;
			EffectSpinDesc.iPlayerType = CEffectSpin::PLAYERTYPE::PLAYERTYPE_YUMO;
			if (FAILED(GI->Add_GameObjectToLayer(TEXT("EffectSpin"), GI->Get_Cur_LevelIndex(), TEXT("Layer_KSEFFECT"), &EffectSpinDesc)))
				return E_FAIL;
			break;
		}

		case Client::CPlayer01::ET_SKILL5:
		{
			if (m_ePlayerMode != PMODE_BURST)
			{
				CPlayer01Skill5Ball::SKILL5BALLINFO* Info = new CPlayer01Skill5Ball::SKILL5BALLINFO;
				Info->fBlurPower = 1.f;
				Info->vColor = { 0.5f,1.f,1.f,1.f };
				Info->vColor2 = { 0.f,1.f,1.f,1.f };
				Info->vLocalPos = { 0.f,1.f,1.f,1.f };
				Info->vScaleSpeed = { 5.f,5.f,5.f };
				Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				Info->vRatio = { 1.f,1.f };
				Info->fLimitTime = 1.3f;
				GI->Add_GameObjectToLayer(L"Player01Skill5Ball", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);

				PTM->Create_InstanceParticle(L"Player01Skill5", m_pTransformCom->Get_WorldFloat4x4());

				CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
				Info3->fBlurPower = 5.f;
				Info3->vLocalPos = { 0.f,1.f,1.f,1.f };
				Info3->vColor = { 1.f,1.f,1.f,1.f };
				Info3->vColor2 = { 0.f,1.f,1.f,1.f };
				Info3->vRatio = { 1.f,1.f };
				Info3->vRatioSpeed = { 1.5f,1.5f };
				Info3->vScaleSpeed = { -70.f,-70.f,-70.f };
				Info3->vScale = { 40.f,40.f,40.f };
				Info3->vRotation = { 90.f,0.f,0.f };
				Info3->bRotation = true;
				Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3)))
					return E_FAIL;

				CPlayer01Wind2::WIND2INFO* Info4 = new CPlayer01Wind2::WIND2INFO;
				Info4->fBlurPower = 5.f;
				Info4->vLocalPos = { 0.f,1.f,1.f,1.f };
				Info4->vColor = { 1.f,1.f,0.9f,1.f };
				Info4->vColor2 = { 0.f,1.f,0.9f,1.f };
				Info4->vRatio = { 1.f,1.f };
				Info4->vRatioSpeed = { 1.5f,1.5f };
				Info4->vScaleSpeed = { -70.f,-70.f,-70.f };
				Info4->vScale = { 40.f,40.f,40.f };
				Info4->vRotation = { 90.f,0.f,0.f };
				Info4->bRotation = true;
				Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4)))
					return E_FAIL;
			}
			else
			{
				CPlayer01Skill5Ball::SKILL5BALLINFO* Info = new CPlayer01Skill5Ball::SKILL5BALLINFO;
				Info->fBlurPower = 1.f;
				Info->vColor = { 1.f,1.f,0.5f,1.f };
				Info->vColor2 = { 1.f,0.2f,0.f,1.f };
				Info->vLocalPos = { 0.f,1.f,1.f,1.f };
				Info->vScaleSpeed = { 5.f,5.f,5.f };
				Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				Info->vRatio = { 1.f,1.f };
				Info->fLimitTime = 1.3f;
				Info->bBurst = true;
				GI->Add_GameObjectToLayer(L"Player01Skill5Ball", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);

				PTM->Create_InstanceParticle(L"Player01Skill5B", m_pTransformCom->Get_WorldFloat4x4());

				CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
				Info3->fBlurPower = 5.f;
				Info3->vLocalPos = { 0.f,1.f,1.f,1.f };
				Info3->vColor = { 1.f,1.f,0.5f,1.f };
				Info3->vColor2 = { 1.f,0.2f,0.f,1.f };
				Info3->vRatio = { 1.f,1.f };
				Info3->vRatioSpeed = { 1.5f,1.5f };
				Info3->vScaleSpeed = { -70.f,-70.f,-70.f };
				Info3->vScale = { 40.f,40.f,40.f };
				Info3->vRotation = { 90.f,0.f,0.f };
				Info3->bRotation = true;
				Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3)))
					return E_FAIL;

				CPlayer01Wind2::WIND2INFO* Info4 = new CPlayer01Wind2::WIND2INFO;
				Info4->fBlurPower = 5.f;
				Info4->vLocalPos = { 0.f,1.f,1.f,1.f };
				Info4->vColor = { 1.f,1.f,0.5f,1.f };
				Info4->vColor2 = { 1.f,0.2f,0.f,1.f };
				Info4->vRatio = { 1.f,1.f };
				Info4->vRatioSpeed = { 1.5f,1.5f };
				Info4->vScaleSpeed = { -70.f,-70.f,-70.f };
				Info4->vScale = { 40.f,40.f,40.f };
				Info4->vRotation = { 90.f,0.f,0.f };
				Info4->bRotation = true;
				Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4)))
					return E_FAIL;
			}
			
			break;
		}

		case Client::CPlayer01::ET_SKILL9:
		{
			CWind::WINDINFO* Info = new CWind::WINDINFO;
			Info->vColor = { 0.7f,0.7f,0.7f,1.f };
			Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
			Info->vFixPosSpeed = { 0.f,0.f,3.f };
			Info->vFixScaleSpeed = { 30.f,30.f,30.f };
			Info->vRatio = { 1.f,1.f };
			Info->vRatioSpeed = { 2.f,2.f };
			Info->vUVFixSpeed = { 1.f,-3.f };
			_matrix Effectmatrix;
			Effectmatrix.r[0] = { 0.1f, 0.f, 0.f, 0.f };
			Effectmatrix.r[1] = { 0.f, 0.034f, 0.09f, 0.f };
			Effectmatrix.r[2] = { 0.f, -0.09f, 0.034f, 0.f };
			Effectmatrix.r[3] = { 0.f, 1.3f, 0.53f, 1.f };
			XMStoreFloat4x4(&Info->WorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
			if (FAILED(GI->Add_GameObjectToLayer(L"Wind", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
				return E_FAIL;

			CWind::WINDINFO* Info2 = new CWind::WINDINFO;
			Info2->vColor = { 0.7f,0.7f,0.7f,1.f };
			Info2->vColor2 = { 0.7f,0.7f,0.7f,1.f };
			Info2->vFixPosSpeed = { 0.f,0.f,5.f };
			Info2->vFixScaleSpeed = { 40.f,40.f,40.f };
			Info2->vRatio = { 1.f,1.f };
			Info2->vRatioSpeed = { 2.f,2.f };
			Info2->vUVFixSpeed = { 1.f,-3.f };
			Effectmatrix.r[0] = { 0.1f, 0.f, 0.f, 0.f };
			Effectmatrix.r[1] = { 0.f, 0.034f, 0.09f, 0.f };
			Effectmatrix.r[2] = { 0.f, -0.09f, 0.034f, 0.f };
			Effectmatrix.r[3] = { 0.f, 1.3f, 0.53f, 1.f };
			XMStoreFloat4x4(&Info2->WorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
			if (FAILED(GI->Add_GameObjectToLayer(L"Wind", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info2)))
				return E_FAIL;

			PTM->Create_InstanceParticle(L"Player01Distortion", m_pTransformCom->Get_WorldFloat4x4());

			if (m_ePlayerMode != PMODE_BURST)
			{
				PTM->Create_InstanceParticle(L"Player01Skill9", m_pTransformCom->Get_WorldFloat4x4());
				PTM->Create_InstanceParticle(L"Player01Skill9_2", m_pTransformCom->Get_WorldFloat4x4());
			}
			else
			{
				PTM->Create_InstanceParticle(L"Player01Skill9B", m_pTransformCom->Get_WorldFloat4x4());
				PTM->Create_InstanceParticle(L"Player01Skill9_2B", m_pTransformCom->Get_WorldFloat4x4());
			}



			break;
		}

		case Client::CPlayer01::ET_SKILL1:
		{			
			_matrix Effectmatrix;
			if (m_ePlayerMode != PMODE_BURST)
			{
				PTM->Create_InstanceParticle(L"Player01Distortion", m_pTransformCom->Get_WorldFloat4x4());

				CPlayer01Wind1::WIND1INFO* Info5 = new CPlayer01Wind1::WIND1INFO;
				Info5->fBlurPower = 5.f;
				Info5->vLocalPos = { 0.f,0.f,0.f,1.f };
				Info5->vColor = { 1.f,1.f,0.9f,1.f };
				Info5->vColor2 = { 0.f,1.f,0.9f,1.f };
				Info5->vRatio = { 1.f,1.f };
				Info5->vRatioSpeed = { 1.5f,1.5f };
				Info5->vScaleSpeed = { 80.f,100.f,80.f };
				Info5->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info5)))
					return E_FAIL;

				CPlayer01Wind2::WIND2INFO* Info6 = new CPlayer01Wind2::WIND2INFO;
				Info6->fBlurPower = 5.f;
				Info6->vLocalPos = { 0.f,0.f,0.f,1.f };
				Info6->vColor = { 1.f,1.f,0.9f,1.f };
				Info6->vColor2 = { 0.f,1.f,0.9f,1.f };
				Info6->vRatio = { 1.f,1.f };
				Info6->vRatioSpeed = { 1.5f,1.5f };
				Info6->vScaleSpeed = { 80.f,100.f,80.f };
				Info6->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info6)))
					return E_FAIL;

				CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
				Info->fBlurPower = 2.f;
				Info->vColor = { 0.7f,0.7f,0.7f,1.f };
				Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
				Info->vLocalPos = { 0.f,0.f,1.f,1.f };
				Info->vRatio = { 1.f,1.f };
				Info->vRatioSpeed = { 2.f,2.f };
				Info->vScale = { 0.1f,0.1f,0.1f };
				Info->vScaleSpeed = { 250.f,250.f,250.f };
				Info->vUvFixSpeed = { 1.f,3.f };
				Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
				PTM->Create_InstanceParticle(L"Player01Skill4End", m_pTransformCom->Get_WorldFloat4x4());
			}
			else
			{
				PTM->Create_InstanceParticle(L"Player01Distortion", m_pTransformCom->Get_WorldFloat4x4());

				CPlayer01Wind1::WIND1INFO* Info5 = new CPlayer01Wind1::WIND1INFO;
				Info5->fBlurPower = 5.f;
				Info5->vLocalPos = { 0.f,0.f,0.f,1.f };
				Info5->vColor = { 1.f,1.f,0.5f,1.f };
				Info5->vColor2 = { 1.f,0.3f,0.f,1.f };
				Info5->vRatio = { 1.f,1.f };
				Info5->vRatioSpeed = { 1.5f,1.5f };
				Info5->vScaleSpeed = { 80.f,100.f,80.f };
				Info5->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info5)))
					return E_FAIL;

				CPlayer01Wind2::WIND2INFO* Info6 = new CPlayer01Wind2::WIND2INFO;
				Info6->fBlurPower = 5.f;
				Info6->vLocalPos = { 0.f,0.f,0.f,1.f };
				Info6->vColor = { 1.f,1.f,0.5f,1.f };
				Info6->vColor2 = { 1.f,0.3f,0.f,1.f };
				Info6->vRatio = { 1.f,1.f };
				Info6->vRatioSpeed = { 1.5f,1.5f };
				Info6->vScaleSpeed = { 80.f,100.f,80.f };
				Info6->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info6)))
					return E_FAIL;

				CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
				Info->fBlurPower = 2.f;
				Info->vColor = { 0.7f,0.7f,0.7f,1.f };
				Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
				Info->vLocalPos = { 0.f,0.f,1.f,1.f };
				Info->vRatio = { 1.f,1.f };
				Info->vRatioSpeed = { 2.f,2.f };
				Info->vScale = { 0.1f,0.1f,0.1f };
				Info->vScaleSpeed = { 250.f,250.f,250.f };
				Info->vUvFixSpeed = { 1.f,3.f };
				Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);


				PTM->Create_InstanceParticle(L"Player01Skill4EndB", m_pTransformCom->Get_WorldFloat4x4());
			}
		


			break;
		}
		
		case Client::CPlayer01::ET_SKILL3_1:
		{
			
			if (m_ePlayerMode != PMODE_BURST)
			{
				CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
				Info3->fBlurPower = 2.f;
				Info3->vLocalPos = { 0.f,1.f,1.f,1.f };
				Info3->vColor = { 1.f,1.f,0.9f,1.f };
				Info3->vColor2 = { 0.f,1.f,0.9f,1.f };
				Info3->vRatio = { 2.f,2.f };
				Info3->vRatioSpeed = { 2.5f,2.5f };
				Info3->vScaleSpeed = { 50.f,50.f,50.f };
				Info3->vRotation = { 0.f,0.f,45.f };
				Info3->bRotation = true;
				Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3)))
					return E_FAIL;

				CPlayer01Wind2::WIND2INFO* Info4 = new CPlayer01Wind2::WIND2INFO;
				Info4->fBlurPower = 2.f;
				Info4->vLocalPos = { 0.f,1.f,1.f,1.f };
				Info4->vColor = { 1.f,1.f,0.9f,1.f };
				Info4->vColor2 = { 0.f,1.f,0.9f,1.f };
				Info4->vRatio = { 2.f,2.f };
				Info4->vRatioSpeed = { 2.5f,2.5f };
				Info4->vScaleSpeed = { 50.f,50.f,50.f };
				Info4->vRotation = { 0.f,0.f,45.f };
				Info4->bRotation = true;
				Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4)))
					return E_FAIL;
			}
			else
			{
				CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
				Info3->fBlurPower = 2.f;
				Info3->vLocalPos = { 0.f,1.f,1.f,1.f };
				Info3->vColor = { 1.f,1.f,0.5f,1.f };
				Info3->vColor2 = { 1.f,0.f,0.f,1.f };
				Info3->vRatio = { 2.f,2.f };
				Info3->vRatioSpeed = { 2.5f,2.5f };
				Info3->vScaleSpeed = { 50.f,50.f,50.f };
				Info3->vRotation = { 0.f,0.f,45.f };
				Info3->bRotation = true;
				Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3)))
					return E_FAIL;

				CPlayer01Wind2::WIND2INFO* Info4 = new CPlayer01Wind2::WIND2INFO;
				Info4->fBlurPower = 2.f;
				Info4->vLocalPos = { 0.f,1.f,1.f,1.f };
				Info4->vColor = { 1.f,1.f,0.5f,1.f };
				Info4->vColor2 = { 1.f,0.f,0.f,1.f };
				Info4->vRatio = { 2.f,2.f };
				Info4->vRatioSpeed = { 2.5f,2.5f };
				Info4->vScaleSpeed = { 50.f,50.f,50.f };
				Info4->vRotation = { 0.f,0.f,45.f };
				Info4->bRotation = true;
				Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4)))
					return E_FAIL;
			}

			break;
		}

		case Client::CPlayer01::ET_SKILL3_2:
		{
			if (m_ePlayerMode != PMODE_BURST)
			{
				CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
				Info3->fBlurPower = 2.f;
				Info3->vLocalPos = { 0.f,1.f,1.f,1.f };
				Info3->vColor = { 1.f,1.f,0.9f,1.f };
				Info3->vColor2 = { 0.f,1.f,0.9f,1.f };
				Info3->vRatio = { 2.f,2.f };
				Info3->vRatioSpeed = { 2.5f,2.5f };
				Info3->vScaleSpeed = { 50.f,50.f,50.f };
				Info3->vRotation = { 0.f,0.f,-60.f };
				Info3->bRotation = true;
				Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3)))
					return E_FAIL;

				CPlayer01Wind2::WIND2INFO* Info4 = new CPlayer01Wind2::WIND2INFO;
				Info4->fBlurPower = 2.f;
				Info4->vLocalPos = { 0.f,1.f,1.f,1.f };
				Info4->vColor = { 1.f,1.f,0.9f,1.f };
				Info4->vColor2 = { 0.f,1.f,0.9f,1.f };
				Info4->vRatio = { 2.f,2.f };
				Info4->vRatioSpeed = { 2.5f,2.5f };
				Info4->vScaleSpeed = { 50.f,50.f,50.f };
				Info4->vRotation = { 0.f,0.f,-60.f };
				Info4->bRotation = true;
				Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4)))
					return E_FAIL;
			}

			else
			{
				CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
				Info3->fBlurPower = 2.f;
				Info3->vLocalPos = { 0.f,1.f,1.f,1.f };
				Info3->vColor = { 1.f,1.f,0.5f,1.f };
				Info3->vColor2 = { 1.f,0.f,0.f,1.f };
				Info3->vRatio = { 2.f,2.f };
				Info3->vRatioSpeed = { 2.5f,2.5f };
				Info3->vScaleSpeed = { 50.f,50.f,50.f };
				Info3->vRotation = { 0.f,0.f,-60.f };
				Info3->bRotation = true;
				Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3)))
					return E_FAIL;

				CPlayer01Wind2::WIND2INFO* Info4 = new CPlayer01Wind2::WIND2INFO;
				Info4->fBlurPower = 2.f;
				Info4->vLocalPos = { 0.f,1.f,1.f,1.f };
				Info4->vColor = { 1.f,1.f,0.5f,1.f };
				Info4->vColor2 = { 1.f,0.f,0.f,1.f };
				Info4->vRatio = { 2.f,2.f };
				Info4->vRatioSpeed = { 2.5f,2.5f };
				Info4->vScaleSpeed = { 50.f,50.f,50.f };
				Info4->vRotation = { 0.f,0.f,-60.f };
				Info4->bRotation = true;
				Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4)))
					return E_FAIL;
			}

			break;
		}

		case Client::CPlayer01::ET_LANDING:
		{
			CEffectSpin::EFFECTSPINDESC EffectSpinDesc;
			EffectSpinDesc.iSpinType = (_uint)CEffectSpin::SPINTYPE::TYPE_LANDING;
			if (FAILED(GI->Add_GameObjectToLayer(TEXT("EffectSpin"), GI->Get_Cur_LevelIndex(), TEXT("Layer_KSEFFECT"), &EffectSpinDesc)))
				return E_FAIL;

			break;
		}

		case Client::CPlayer01::ET_SKILL5END:
		{
			if (m_ePlayerMode != PMODE_BURST)
			{
				_float4x4 WorldMatrix = m_pTransformCom->Get_WorldFloat4x4();

				CViewAlpha::VIEWALPHAINFO* Info2 = new CViewAlpha::VIEWALPHAINFO;
				Info2->fBlurPower = 1.f;
				Info2->vColor1 = { 1.f,1.f,1.f,1.f };
				Info2->vColor2 = { 0.f,1.f,1.f,1.f };
				Info2->vRatio = { 0.7f,0.7f };
				Info2->vRatioSpeed = { 2.f,1.f };
				if (FAILED(GI->Add_GameObjectToLayer(L"ViewAlpha", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2)))
					return E_FAIL;

				CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
				Info3->fBlurPower = 5.f;
				Info3->vLocalPos = { 0.f,1.f,1.f,1.f };
				Info3->vColor = { 1.f,1.f,1.f,1.f };
				Info3->vColor2 = { 0.f,1.f,1.f,1.f };
				Info3->vRatio = { 1.f,1.f };
				Info3->vRatioSpeed = { 1.5f,1.5f };
				Info3->vScaleSpeed = { 70.f,100.f,70.f };
				Info3->vRotation = { 90.f,0.f,0.f };
				Info3->bRotation = true;
				Info3->vWorldMatrix = WorldMatrix;
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3)))
					return E_FAIL;

				CPlayer01Wind2::WIND2INFO* Info4 = new CPlayer01Wind2::WIND2INFO;
				Info4->fBlurPower = 5.f;
				Info4->vLocalPos = { 0.f,1.f,1.f,1.f };
				Info4->vColor = { 1.f,1.f,0.9f,1.f };
				Info4->vColor2 = { 0.f,1.f,0.9f,1.f };
				Info4->vRatio = { 1.f,1.f };
				Info4->vRatioSpeed = { 1.5f,1.5f };
				Info4->vScaleSpeed = { 70.f,100.f,70.f };
				Info4->vRotation = { 90.f,0.f,0.f };
				Info4->bRotation = true;
				Info4->vWorldMatrix = WorldMatrix;
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4)))
					return E_FAIL;

				CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
				Info->fBlurPower = 2.f;
				Info->vColor = { 0.7f,0.7f,0.7f,1.f };
				Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
				Info->vLocalPos = { 0.f,0.f,1.f,1.f };
				Info->vRatio = { 1.f,1.f };
				Info->vRatioSpeed = { 2.f,2.f };
				Info->vScale = { 0.1f,0.1f,0.1f };
				Info->vScaleSpeed = { 150.f,150.f,150.f };
				Info->vUvFixSpeed = { 1.f,3.f };
				Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);


				PTM->Create_InstanceParticle(L"Player01Skill5End", m_pTransformCom->Get_WorldFloat4x4());

			}
			else
			{
				CViewAlpha::VIEWALPHAINFO* Info2 = new CViewAlpha::VIEWALPHAINFO;
				Info2->fBlurPower = 1.f;
				Info2->vColor1 = { 1.f,1.f,0.5f,1.f };
				Info2->vColor2 = { 1.f,0.2f,0.f,1.f };
				Info2->vRatio = { 0.7f,0.7f };
				Info2->vRatioSpeed = { 2.f,1.f };
				if (FAILED(GI->Add_GameObjectToLayer(L"ViewAlpha", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2)))
					return E_FAIL;

				CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
				Info3->fBlurPower = 5.f;
				Info3->vLocalPos = { 0.f,1.f,1.f,1.f };
				Info3->vColor = { 1.f,1.f,0.5f,1.f };
				Info3->vColor2 = { 1.f,0.2f,0.f,1.f };
				Info3->vRatio = { 1.f,1.f };
				Info3->vRatioSpeed = { 1.5f,1.5f };
				Info3->vScaleSpeed = { 70.f,100.f,70.f };
				Info3->vRotation = { 90.f,0.f,0.f };
				Info3->bRotation = true;
				Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3)))
					return E_FAIL;

				CPlayer01Wind2::WIND2INFO* Info4 = new CPlayer01Wind2::WIND2INFO;
				Info4->fBlurPower = 5.f;
				Info4->vLocalPos = { 0.f,1.f,1.f,1.f };
				Info4->vColor = { 1.f,1.f,0.5f,1.f };
				Info4->vColor2 = { 1.f,0.2f,0.f,1.f };
				Info4->vRatio = { 1.f,1.f };
				Info4->vRatioSpeed = { 1.5f,1.5f };
				Info4->vScaleSpeed = { 70.f,100.f,70.f };
				Info4->vRotation = { 90.f,0.f,0.f };
				Info4->bRotation = true;
				Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4)))
					return E_FAIL;

				CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
				Info->fBlurPower = 2.f;
				Info->vColor = { 0.7f,0.7f,0.7f,1.f };
				Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
				Info->vLocalPos = { 0.f,0.f,1.f,1.f };
				Info->vRatio = { 1.f,1.f };
				Info->vRatioSpeed = { 2.f,2.f };
				Info->vScale = { 0.1f,0.1f,0.1f };
				Info->vScaleSpeed = { 150.f,150.f,150.f };
				Info->vUvFixSpeed = { 1.f,3.f };
				Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);

				PTM->Create_InstanceParticle(L"Player01Skill5EndB", m_pTransformCom->Get_WorldFloat4x4());
			}
			break;
		}
		
		case Client::CPlayer01::ET_RAGESKILL1:
		{
			if (m_ePlayerMode != PMODE_BURST)
			{
				CPlayer01Rage1Ball::RAGE1BALLINFO* Info = new CPlayer01Rage1Ball::RAGE1BALLINFO;
				Info->fBlurPower = 2.f;
				Info->fLimitTime = 0.5f;
				Info->vColor = { 1.f,1.f,1.f,1.f };
				Info->vColor2 = { 0.f,1.f,1.f,1.f };
				Info->vLocalPos = { 0.f,1.5f,4.f,1.f };
				Info->vRatio = { 0.4f,0.5f };
				Info->vRatioSpeed = { 0.4f,0.f };
				Info->vScale = { 0.1f,0.1f,0.1f };
				Info->vScaleSpeed = { 100.f,100.f,100.f };
				Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				Info->iShaderPass = 4;
				GI->Add_GameObjectToLayer(L"Player01Rage1Ball", TM->Get_CurLevel(), L"Layer_Player01", &Info);

				CPlayer01Wind1::WIND1INFO* Info4 = new CPlayer01Wind1::WIND1INFO;
				Info4->fBlurPower = 5.f;
				Info4->vLocalPos = { 0.f,0.f,4.f,1.f };
				Info4->vColor = { 1.f,1.f,1.f,1.f };
				Info4->vColor2 = { 0.f,1.f,1.f,1.f };
				Info4->vRatio = { 1.f,1.f };
				Info4->vRatioSpeed = { 1.5f,1.5f };
				Info4->vScaleSpeed = { 70.f,70.f,70.f };
				Info4->vScale = { 10.f,10.f,10.f };
				Info4->fTurnSpeed = 30.f;
				Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4)))
					return E_FAIL;

				CPlayer01Wind2::WIND2INFO* Info5 = new CPlayer01Wind2::WIND2INFO;
				Info5->fBlurPower = 5.f;
				Info5->vLocalPos = { 0.f,0.f,4.f,1.f };
				Info5->vColor = { 1.f,1.f,0.9f,1.f };
				Info5->vColor2 = { 0.f,1.f,0.9f,1.f };
				Info5->vRatio = { 1.f,1.f };
				Info5->vRatioSpeed = { 1.5f,1.5f };
				Info5->vScaleSpeed = { 70.f,70.f,70.f };
				Info5->vScale = { 10.f,10.f,10.f };
				Info5->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				Info5->fTurnSpeed = 30.f;
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info5)))
					return E_FAIL;

				CPlayer01Pillar2::PILLAR2INFO* Info6 = new CPlayer01Pillar2::PILLAR2INFO;
				Info6->fBlurPower = 3.f;
				Info6->vColor = { 1.f,1.f,1.f,1.f };
				Info6->vColor2 = { 0.f,1.f,1.f,1.f };
				Info6->vRatio = { 1.f,1.f };
				Info6->vRatioSpeed = { 1.6f,1.6f };
				Info6->vScaleSpeed = { 100.f,30.f,100.f };
				Info6->vLocalPos = { 0.f,0.f,4.f,1.f };
				Info6->vScale = { 10.f,10.f,10.f };
				Info6->vUvFixSpeed = { 2.f,0.f };
				Info6->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				GI->Add_GameObjectToLayer(L"Player01Pillar2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info6);
				PTM->Create_InstanceParticle(L"Player01RageSkill1_2", m_pTransformCom->Get_WorldFloat4x4());

				CPlayer01SmokeBall::SMOKEBALLINFO* Info7 = new CPlayer01SmokeBall::SMOKEBALLINFO;
				Info7->fBlurPower = 0.5f;
				Info7->vColor = { 0.7f,0.7f,0.7f,1.f };
				Info7->vColor2 = { 0.7f,0.7f,0.7f,1.f };
				Info7->vLocalPos = { 0.f,0.f,4.f,1.f };
				Info7->vRatio = { 1.f,1.f };
				Info7->vRatioSpeed = { 1.f,1.f };
				Info7->vScale = { 0.1f,0.1f,0.1f };
				Info7->vScaleSpeed = { 150.f,150.f,150.f };
				Info7->vUvFixSpeed = { 1.f,3.f };
				Info7->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info7);

			}
			else
			{
				CPlayer01Rage1Ball::RAGE1BALLINFO* Info = new CPlayer01Rage1Ball::RAGE1BALLINFO;
				Info->fBlurPower = 2.f;
				Info->fLimitTime = 0.5f;
				Info->vColor = { 1.f,1.f,1.f,1.f };
				Info->vColor2 = { 0.f,1.f,1.f,1.f };
				Info->vLocalPos = { 0.f,1.5f,4.f,1.f };
				Info->vRatio = { 0.4f,0.5f };
				Info->vRatioSpeed = { 0.4f,0.f };
				Info->vScale = { 0.1f,0.1f,0.1f };
				Info->vScaleSpeed = { 100.f,100.f,100.f };
				Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				Info->iShaderPass = 4;
				GI->Add_GameObjectToLayer(L"Player01Rage1Ball", TM->Get_CurLevel(), L"Layer_Player01", &Info);

				CPlayer01Wind1::WIND1INFO* Info4 = new CPlayer01Wind1::WIND1INFO;
				Info4->fBlurPower = 5.f;
				Info4->vLocalPos = { 0.f,0.f,4.f,1.f };
				Info4->vColor = { 1.f,1.f,0.5f,1.f };
				Info4->vColor2 = { 1.f,0.f,0.f,1.f };
				Info4->vRatio = { 1.5f,1.5f };
				Info4->vRatioSpeed = { 2.5f,2.5f };
				Info4->vScaleSpeed = { 70.f,70.f,70.f };
				Info4->vScale = { 10.f,10.f,10.f };
				Info4->fTurnSpeed = 30.f;
				Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4)))
					return E_FAIL;

				CPlayer01Wind2::WIND2INFO* Info5 = new CPlayer01Wind2::WIND2INFO;
				Info5->fBlurPower = 5.f;
				Info5->vLocalPos = { 0.f,0.f,4.f,1.f };
				Info5->vColor = { 1.f,1.f,0.5f,1.f };
				Info5->vColor2 = { 1.f,0.f,0.f,1.f };
				Info5->vRatio = { 1.5f,1.5f };
				Info5->vRatioSpeed = { 2.5f,2.5f };
				Info5->vScaleSpeed = { 70.f,70.f,70.f };
				Info5->vScale = { 10.f,10.f,10.f };
				Info5->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				Info5->fTurnSpeed = 30.f;
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info5)))
					return E_FAIL;

				CPlayer01Pillar2::PILLAR2INFO* Info6 = new CPlayer01Pillar2::PILLAR2INFO;
				Info6->fBlurPower = 3.f;
				Info6->vColor = { 1.f,1.f,0.5f,1.f };
				Info6->vColor2 = { 1.f,0.f,0.f,1.f };
				Info6->vRatio = { 2.f,2.f };
				Info6->vRatioSpeed = { 3.f,3.f };
				Info6->vScaleSpeed = { 100.f,30.f,100.f };
				Info6->vLocalPos = { 0.f,0.f,4.f,1.f };
				Info6->vScale = { 10.f,10.f,10.f };
				Info6->vUvFixSpeed = { 2.f,0.f };
				Info6->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				GI->Add_GameObjectToLayer(L"Player01Pillar2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info6);

				PTM->Create_InstanceParticle(L"Player01RageSkill1_2B", m_pTransformCom->Get_WorldFloat4x4());

				CPlayer01SmokeBall::SMOKEBALLINFO* Info7 = new CPlayer01SmokeBall::SMOKEBALLINFO;
				Info7->fBlurPower = 0.5f;
				Info7->vColor = { 0.7f,0.7f,0.7f,1.f };
				Info7->vColor2 = { 0.7f,0.7f,0.7f,1.f };
				Info7->vLocalPos = { 0.f,0.f,4.f,1.f };
				Info7->vRatio = { 1.f,1.f };
				Info7->vRatioSpeed = { 1.f,1.f };
				Info7->vScale = { 0.1f,0.1f,0.1f };
				Info7->vScaleSpeed = { 150.f,150.f,150.f };
				Info7->vUvFixSpeed = { 1.f,3.f };
				Info7->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info7);
			}
			break;
		}

		case Client::CPlayer01::ET_RAGESKILL1END:
		{
			if (m_ePlayerMode != PMODE_BURST)
				PTM->Create_InstanceParticle(L"Player01RageSkill1", m_pTransformCom->Get_WorldFloat4x4());
			else
				PTM->Create_InstanceParticle(L"Player01RageSkill1B", m_pTransformCom->Get_WorldFloat4x4());

			break;
		}

		case Client::CPlayer01::ET_RAGESKILL2:
		{
			CWind::WINDINFO* Info = new CWind::WINDINFO;
			Info->vColor = { 0.7f,0.7f,0.7f,1.f };
			Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
			Info->vFixPosSpeed = { 0.f,0.f,3.f };
			Info->vFixScaleSpeed = { 30.f,30.f,30.f };
			Info->vRatio = { 1.f,1.f };
			Info->vRatioSpeed = { 2.f,2.f };
			Info->vUVFixSpeed = { 1.f,-3.f };
			_matrix Effectmatrix;
			Effectmatrix.r[0] = { 0.1f, 0.f, 0.f, 0.f };
			Effectmatrix.r[1] = { 0.f, 0.f, 1.f, 0.f };
			Effectmatrix.r[2] = { 0.f, -1.f, 0.f, 0.f };
			Effectmatrix.r[3] = { 0.f, 1.f, 0.5f, 1.f };
			XMStoreFloat4x4(&Info->WorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
			if (FAILED(GI->Add_GameObjectToLayer(L"Wind", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
				return E_FAIL;

			CWind::WINDINFO* Info2 = new CWind::WINDINFO;
			Info2->vColor = { 0.7f,0.7f,0.7f,1.f };
			Info2->vColor2 = { 0.7f,0.7f,0.7f,1.f };
			Info2->vFixPosSpeed = { 0.f,0.f,5.f };
			Info2->vFixScaleSpeed = { 40.f,40.f,40.f };
			Info2->vRatio = { 1.f,1.f };
			Info2->vRatioSpeed = { 2.f,2.f };
			Info2->vUVFixSpeed = { 1.f,-3.f };
			Effectmatrix.r[0] = { 0.1f, 0.f, 0.f, 0.f };
			Effectmatrix.r[1] = { 0.f, 0.f, 1.f, 0.f };
			Effectmatrix.r[2] = { 0.f, -1.f, 0.f, 0.f };
			Effectmatrix.r[3] = { 0.f, 1.f, 0.5f, 1.f };
			XMStoreFloat4x4(&Info2->WorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
			if (FAILED(GI->Add_GameObjectToLayer(L"Wind", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info2)))
				return E_FAIL;

			CWind::WINDINFO* Info3 = new CWind::WINDINFO;
			Info3->vColor = { 0.7f,0.7f,0.7f,1.f };
			Info3->vColor2 = { 0.7f,0.7f,0.7f,1.f };
			Info3->vFixPosSpeed = { 0.f,0.f,5.f };
			Info3->vFixScaleSpeed = { 60.f,60.f,60.f };
			Info3->vRatio = { 1.f,1.f };
			Info3->vRatioSpeed = { 2.f,2.f };
			Info3->vUVFixSpeed = { 1.f,-3.f };
			Effectmatrix.r[0] = { 0.1f, 0.f, 0.f, 0.f };
			Effectmatrix.r[1] = { 0.f, 0.f, 1.f, 0.f };
			Effectmatrix.r[2] = { 0.f, -1.f, 0.f, 0.f };
			Effectmatrix.r[3] = { 0.f, 1.f, 0.5f, 1.f };
			Info3->bDistortion = true;
			XMStoreFloat4x4(&Info3->WorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
			if (FAILED(GI->Add_GameObjectToLayer(L"Wind", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info3)))
				return E_FAIL;

			if (m_ePlayerMode != PMODE_BURST)
				PTM->Create_InstanceParticle(L"Player01Attack5", m_pTransformCom->Get_WorldFloat4x4());
			else
				PTM->Create_InstanceParticle(L"Player01Attack5B", m_pTransformCom->Get_WorldFloat4x4());

			break;
		}

		case Client::CPlayer01::ET_RAGESKILL2_2:
		{
			CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
			Info3->fBlurPower = 2.f;
			Info3->vLocalPos = { 0.f,0.f,0.f,1.f };
			Info3->vColor = { 1.f,1.f,0.5f,1.f };
			Info3->vColor2 = { 1.f,0.1f,0.f,1.f };
			Info3->vRatio = { 2.f,2.f };
			Info3->vRatioSpeed = { 3.f,3.f };
			Info3->vScaleSpeed = { 70.f,70.f,70.f };
			Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			Info3->fTurnSpeed = 10.f;
			GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3);


			CPlayer01Wind2::WIND2INFO* Info4 = new CPlayer01Wind2::WIND2INFO;
			Info4->fBlurPower = 3.f;
			Info4->vLocalPos = { 0.f,0.f,0.f,1.f };
			Info4->vColor = { 1.f,1.f,0.5f,1.f };
			Info4->vColor2 = { 1.f,0.1f,0.f,1.f };
			Info4->vRatio = { 2.f,2.f };
			Info4->vRatioSpeed = { 3.f,3.f };
			Info4->vScaleSpeed = { 50.f,50.f,50.f };
			Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			Info4->fTurnSpeed = 20.f;
			GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4);

			CPlayer01Pillar2::PILLAR2INFO* Info7 = new CPlayer01Pillar2::PILLAR2INFO;
			Info7->fBlurPower = 5.f;
			Info7->vColor = { 1.f,1.f,0.5f,1.f };
			Info7->vColor2 = { 1.f,0.1f,0.f,1.f };
			Info7->vRatio = { 2.f,2.f };
			Info7->vRatioSpeed = { 3.f,3.f };
			Info7->vScaleSpeed = { 50.f,10.f,50.f };
			Info7->vLocalPos = { 0.f,0.f,0.f,1.f };
			Info7->vScale = { 3.f,1.f,3.f };
			Info7->vUvFixSpeed = { 0.f,0.5f };
			Info7->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			GI->Add_GameObjectToLayer(L"Player01Pillar2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info7);

			break;
		}

		case Client::CPlayer01::ET_RAGESKILL2_3:
		{
			CHierarchyNode*      Root = m_pModelCom[MODEL_BURST]->Get_HierarchyNode("RootPart2_M");
			_matrix RootMatrix = Root->Get_CombinedTransformation() *  m_pModelCom[m_eAnimModel]->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
			_float4x4 Matrix;
			XMStoreFloat4x4(&Matrix, RootMatrix);
			PTM->Create_AlphaParticle(L"Player01RageSkill2", Matrix);
			PTM->Create_InstanceParticle(L"Player01RageSkill2Distortion", Matrix);

			CPlayer01SmokeBall::SMOKEBALLINFO* Info = new CPlayer01SmokeBall::SMOKEBALLINFO;
			Info->fBlurPower = 0.f;
			Info->vColor = { 0.7f,0.7f,0.7f,1.f };
			Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
			Info->vLocalPos = { 0.f,0.f,0.f,1.f };
			Info->vRatio = { 1.f,1.f };
			Info->vRatioSpeed = { 2.f,0.f };
			Info->vScale = { 0.1f,0.1f,0.1f };
			Info->vScaleSpeed = { 100.f,100.f,100.f };
			Info->vUvFixSpeed = { 1.f,3.f };
			Info->vWorldMatrix = Matrix;
			GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);

			break;
		}

		case Client::CPlayer01::ET_RAGESKILL2END:
		{
			PTM->Create_InstanceParticle(L"Player01RageSkill2End", m_pTransformCom->Get_WorldFloat4x4());
			PTM->Create_InstanceParticle(L"Player01RageSkill2End_2", m_pTransformCom->Get_WorldFloat4x4());
			PTM->Create_InstanceParticle(L"Player01RageSkill2End_3", m_pTransformCom->Get_WorldFloat4x4());
			PTM->Create_InstanceParticle(L"Player01RageSkill2End_4", m_pTransformCom->Get_WorldFloat4x4());

			CPlayer01Wind1::WIND1INFO* Info = new CPlayer01Wind1::WIND1INFO;
			Info->bRotation = false;
			Info->bPosFix = true;
			Info->fUpSpeed = 0.5f;
			Info->fBlurPower = 0.f;
			Info->fTurnSpeed = 4.f;
			Info->vColor = { 0.5f,0.5f,0.5f,1.f };
			Info->vColor2 = { 0.5f,0.5f,0.5f,1.f };
			Info->vLocalPos = { 0.f,0.f,3.f,1.f };
			Info->vRatio = { 0.6f,1.f };
			Info->vRatioSpeed = { 0.3f,0.f };
			Info->vScale = { 10.f,10.f,10.f };
			Info->vScaleSpeed = { 45.f,75.f,45.f };
			Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);

			CRing::RINGINFO* Info2 = new CRing::RINGINFO;
			Info2->bFixPos = true;
			Info2->fBlurPower = 0.f;
			Info2->fUpSpeed = 0.5f;
			Info2->vColor = { 0.5f,0.5f,0.5f,1.f };
			Info2->vColor2 = { 0.5f,0.5f,0.5f,1.f };
			Info2->vFixScaleSpeed = { 5.f,2.f,5.f };
			Info2->vLocalPos = { 0.f,0.f,3.f,1.f };
			Info2->vRatio = { 0.6f,1.f };
			Info2->vRatioSpeed = { 0.3f,0.f };
			Info2->vScale = { 1.f,1.f,1.f };
			Info2->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			GI->Add_GameObjectToLayer(L"Ring", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2);

			CPlayer01Metamor::METAMORINFO* Info3 = new CPlayer01Metamor::METAMORINFO;
			Info3->fBlurPower = 0.f;
			Info3->vLocalPos = { 0.f,0.f,3.f,1.f };
			Info3->vColor = { 0.5f,0.5f,0.5f,1.f };
			Info3->vColor2 = { 0.5f,0.5f,0.5f,1.f };
			Info3->vLocalPos = { 0.f,0.f,3.f,1.f };
			Info3->vRatio = { 0.6f,1.f };
			Info3->vRatioSpeed = { 0.3f,0.f };
			Info3->vScale = { 1.f,0.5f,1.f };
			Info3->vScaleSpeed = { 3.f,0.5f,3.f };
			Info3->vUVFixSpeed = { 1.f,0.5f };
			Info3->bPosFix = false;
			Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			GI->Add_GameObjectToLayer(L"Player01Metamor", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3);

			CPlayer01Landing::LANDINGINFO* Info4 = new CPlayer01Landing::LANDINGINFO;
			Info4->bPosFix = true;
			Info4->fBlurPower = 0.f;
			Info4->fTurnSpeed = 5.f;
			Info4->fUpSpeed = 0.5f;
			Info4->vColor = { 0.5f,0.5f,0.5f,1.f };
			Info4->vColor2 = { 0.5f,0.5f,0.5f,1.f };
			Info4->vLocalPos = { 0.f,0.f,3.f,1.f };
			Info4->vRatio = { 0.6f,1.f };
			Info4->vRatioSpeed = { 0.3f,0.f };
			Info4->vScale = { 10.f,10.f,10.f };
			Info4->vScaleSpeed = { 40.f,50.f,40.f };
			Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			GI->Add_GameObjectToLayer(L"Player01Landing", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4);
			
			break;
		}
			
		case Client::CPlayer01::ET_CHANGEBURST:
		{
			CEffectSpin::EFFECTSPINDESC EffectSpinDesc;
			EffectSpinDesc.iSpinType = (_uint)CEffectSpin::SPINTYPE::TYPE_METAMORPHOSIS;
			EffectSpinDesc.iPlayerType = (_uint)CEffectSpin::PLAYERTYPE::PLAYERTYPE_YUMO;
			if (FAILED(GI->Add_GameObjectToLayer(TEXT("EffectSpin"), GI->Get_Cur_LevelIndex(), TEXT("Layer_KSEFFECT"), &EffectSpinDesc)))
				return E_FAIL;

			break;
		}

		case Client::CPlayer01::ET_SKILL6START:
		{
			if (m_ePlayerMode != PMODE_BURST)
			{
				CPlayer01Upper::UPPERINFO* Info = new CPlayer01Upper::UPPERINFO;
				Info->bPosFix = false;
				Info->bRotation = false;
				Info->fBlurPower = 1.f;
				Info->fFixSpeed = 2.5f;
				Info->fTurnSpeed = 15.f;
				Info->vColor = { 1.f,1.f,1.f,1.f };
				Info->vColor2 = { 0.f,1.f,1.f,1.f };
				Info->vLocalPos = { 0.f,3.5f,1.f,1.f };
				XMStoreFloat3(&Info->vLookDir, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
				Info->vRatio = { 1.f,1.f };
				Info->vRatioSpeed = { 0.f,1.f };
				Info->vScale = { 0.5f,1.f,0.5f };
				Info->vScaleSpeed = { 2.f,1.f,2.f };
				Info->vUVFixSpeed = { 2.f,0.f };
				Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				GI->Add_GameObjectToLayer(L"Player01Upper", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);

				CPlayer01Wind1::WIND1INFO* Info4 = new CPlayer01Wind1::WIND1INFO;
				Info4->fBlurPower = 5.f;
				Info4->vLocalPos = { 0.f,0.f,0.f,1.f };
				Info4->vColor = { 1.f,1.f,1.f,1.f };
				Info4->vColor2 = { 0.f,1.f,1.f,1.f };
				Info4->vRatio = { 1.f,1.f };
				Info4->vRatioSpeed = { 1.5f,1.5f };
				Info4->vScaleSpeed = { 70.f,70.f,70.f };
				Info4->vScale = { 10.f,10.f,10.f };
				Info4->fTurnSpeed = 30.f;
				Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4)))
					return E_FAIL;

				CPlayer01Wind2::WIND2INFO* Info5 = new CPlayer01Wind2::WIND2INFO;
				Info5->fBlurPower = 5.f;
				Info5->vLocalPos = { 0.f,0.f,0.f,1.f };
				Info5->vColor = { 1.f,1.f,0.9f,1.f };
				Info5->vColor2 = { 0.f,1.f,0.9f,1.f };
				Info5->vRatio = { 1.f,1.f };
				Info5->vRatioSpeed = { 1.5f,1.5f };
				Info5->vScaleSpeed = { 70.f,70.f,70.f };
				Info5->vScale = { 10.f,10.f,10.f };
				Info5->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				Info5->fTurnSpeed = 30.f;
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info5)))
					return E_FAIL;

				CPlayer01Pillar2::PILLAR2INFO* Info6 = new CPlayer01Pillar2::PILLAR2INFO;
				Info6->fBlurPower = 3.f;
				Info6->vColor = { 1.f,1.f,1.f,1.f };
				Info6->vColor2 = { 0.f,1.f,1.f,1.f };
				Info6->vLocalPos = { 0.f,0.f,0.f,1.f };
				Info6->vRatio = { 1.f,1.f };
				Info6->vRatioSpeed = { 1.6f,1.6f };
				Info6->vScaleSpeed = { 100.f,30.f,100.f };
				Info6->vScale = { 10.f,10.f,10.f };
				Info6->vUvFixSpeed = { 2.f,0.f };
				Info6->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				GI->Add_GameObjectToLayer(L"Player01Pillar2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info6);

				PTM->Create_InstanceParticle(L"Player01Upper", m_pTransformCom->Get_WorldFloat4x4());

			}
			else
			{
				CPlayer01Upper::UPPERINFO* Info = new CPlayer01Upper::UPPERINFO;
				Info->bPosFix = false;
				Info->bRotation = false;
				Info->fBlurPower = 1.f;
				Info->fFixSpeed = 2.5f;
				Info->fTurnSpeed = 15.f;
				Info->vColor = { 1.f,1.f,0.5f,1.f };
				Info->vColor2 = { 1.f,0.3f,0.f,1.f };
				Info->vLocalPos = { 0.f,3.5f,1.f,1.f };
				XMStoreFloat3(&Info->vLookDir, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
				Info->vRatio = { 1.f,1.f };
				Info->vRatioSpeed = { 0.f,1.f };
				Info->vScale = { 0.5f,1.f,0.5f };
				Info->vScaleSpeed = { 2.f,1.f,2.f };
				Info->vUVFixSpeed = { 2.f,0.f };
				Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				GI->Add_GameObjectToLayer(L"Player01Upper", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);

				CPlayer01Wind1::WIND1INFO* Info4 = new CPlayer01Wind1::WIND1INFO;
				Info4->fBlurPower = 5.f;
				Info4->vLocalPos = { 0.f,0.f,0.f,1.f };
				Info4->vColor = { 1.f,1.f,0.5f,1.f };
				Info4->vColor2 = { 1.f,0.f,0.f,1.f };
				Info4->vRatio = { 1.5f,1.5f };
				Info4->vRatioSpeed = { 2.5f,2.5f };
				Info4->vScaleSpeed = { 70.f,70.f,70.f };
				Info4->vScale = { 10.f,10.f,10.f };
				Info4->fTurnSpeed = 30.f;
				Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4)))
					return E_FAIL;

				CPlayer01Wind2::WIND2INFO* Info5 = new CPlayer01Wind2::WIND2INFO;
				Info5->fBlurPower = 5.f;
				Info5->vLocalPos = { 0.f,0.f,0.f,1.f };
				Info5->vColor = { 1.f,1.f,0.5f,1.f };
				Info5->vColor2 = { 1.f,0.f,0.f,1.f };
				Info5->vRatio = { 1.5f,1.5f };
				Info5->vRatioSpeed = { 2.5f,2.5f };
				Info5->vScaleSpeed = { 70.f,70.f,70.f };
				Info5->vScale = { 10.f,10.f,10.f };
				Info5->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				Info5->fTurnSpeed = 30.f;
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info5)))
					return E_FAIL;

				CPlayer01Pillar2::PILLAR2INFO* Info6 = new CPlayer01Pillar2::PILLAR2INFO;
				Info6->fBlurPower = 3.f;
				Info6->vColor = { 1.f,1.f,0.5f,1.f };
				Info6->vColor2 = { 1.f,0.f,0.f,1.f };
				Info6->vRatio = { 2.f,2.f };
				Info6->vRatioSpeed = { 3.f,3.f };
				Info6->vScaleSpeed = { 100.f,30.f,100.f };
				Info6->vLocalPos = { 0.f,0.f,0.f,1.f };
				Info6->vScale = { 10.f,10.f,10.f };
				Info6->vUvFixSpeed = { 2.f,0.f };
				Info6->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				GI->Add_GameObjectToLayer(L"Player01Pillar2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info6);

				PTM->Create_InstanceParticle(L"Player01UpperB", m_pTransformCom->Get_WorldFloat4x4());
			}


		}

		case Client::CPlayer01::ET_RUSHKICK:
		{
			if (m_ePlayerMode != PMODE_BURST)
			{
				CWind::WINDINFO* Info = new CWind::WINDINFO;
				Info->vColor = { 0.7f,0.7f,0.7f,1.f };
				Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
				Info->vFixPosSpeed = { 0.f,0.f,0.f };
				Info->vFixScaleSpeed = { 30.f,30.f,30.f };
				Info->vRatio = { 1.f,1.f };
				Info->vRatioSpeed = { 2.f,2.f };
				Info->vUVFixSpeed = { 1.f,-3.f };
				_matrix Effectmatrix;
				Effectmatrix.r[0] = { 1.f, 0.f, 0.f, 0.f };
				Effectmatrix.r[1] = { 0.f, -0.587f, 0.81f, 0.f };
				Effectmatrix.r[2] = { 0.f, -0.81f, -0.588f, 0.f };
				Effectmatrix.r[3] = { 0.f, 1.5f, -0.5f, 1.f };
				XMStoreFloat4x4(&Info->WorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
				if (FAILED(GI->Add_GameObjectToLayer(L"Wind", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
					return E_FAIL;

				CWind::WINDINFO* Info2 = new CWind::WINDINFO;
				Info2->vColor = { 0.7f,0.7f,0.7f,1.f };
				Info2->vColor2 = { 0.7f,0.7f,0.7f,1.f };
				Info2->vFixPosSpeed = { 0.f,0.f,0.f };
				Info2->vFixScaleSpeed = { 40.f,40.f,40.f };
				Info2->vRatio = { 1.f,1.f };
				Info2->vRatioSpeed = { 2.f,2.f };
				Info2->vUVFixSpeed = { 1.f,-3.f };
				Effectmatrix.r[0] = { 1.f, 0.f, 0.f, 0.f };
				Effectmatrix.r[1] = { 0.f, -0.587f, 0.81f, 0.f };
				Effectmatrix.r[2] = { 0.f, -0.81f, -0.588f, 0.f };
				Effectmatrix.r[3] = { 0.f, 1.5f, -0.5f, 1.f };
				XMStoreFloat4x4(&Info2->WorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
				if (FAILED(GI->Add_GameObjectToLayer(L"Wind", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info2)))
					return E_FAIL;

				CPlayer01Pillar2::PILLAR2INFO* Info4 = new CPlayer01Pillar2::PILLAR2INFO;
				Info4->fBlurPower = 3.f;
				Info4->vColor = { 1.f,1.f,1.f,1.f };
				Info4->vColor2 = { 0.f,1.f,1.f,1.f };
				Info4->vRatio = { 1.f,1.f };
				Info4->vRatioSpeed = { 2.f,2.f };
				Info4->vLocalPos = { 0.f,0.f,1.f,1.f };
				Info4->vScaleSpeed = { 50.f,80.f,50.f };
				Info4->vScale = { 0.1f,1.f,0.1f };
				Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				GI->Add_GameObjectToLayer(L"Player01Pillar2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4);

				CPlayer01Pillar3::PILLAR3INFO* Info5 = new CPlayer01Pillar3::PILLAR3INFO;
				Info5->fBlurPower = 3.f;
				Info5->vColor = { 1.f,1.f,1.f,1.f };
				Info5->vColor2 = { 0.f,1.f,1.f,1.f };
				Info5->vRatio = { 1.f,1.f };
				Info5->vRatioSpeed = { 2.f,2.f };
				Info5->vLocalPos = { 0.f,0.f,1.f,1.f };
				Info5->vScaleSpeed = { 50.f,50.f,50.f };
				Info5->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				GI->Add_GameObjectToLayer(L"Player01Pillar3", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info5);

				CPlayer01SmokeBall::SMOKEBALLINFO* Info6 = new CPlayer01SmokeBall::SMOKEBALLINFO;
				Info6->fBlurPower = 2.f;
				Info6->vColor = { 0.7f,0.7f,0.7f,1.f };
				Info6->vColor2 = { 0.7f,0.7f,0.7f,1.f };
				Info6->vLocalPos = { 0.f,0.f,1.f,1.f };
				Info6->vRatio = { 1.f,1.f };
				Info6->vRatioSpeed = { 2.f,2.f };
				Info6->vScale = { 0.1f,0.1f,0.1f };
				Info6->vScaleSpeed = { 250.f,250.f,250.f };
				Info6->vUvFixSpeed = { 1.f,3.f };
				Info6->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info6);

				CPlayer01Wind1::WIND1INFO* Info7 = new CPlayer01Wind1::WIND1INFO;
				Info7->fBlurPower = 5.f;
				Info7->vLocalPos = { 0.f,0.f,1.f,1.f };
				Info7->vColor = { 1.f,1.f,1.f,1.f };
				Info7->vColor2 = { 0.f,1.f,1.f,1.f };
				Info7->vRatio = { 1.f,1.f };
				Info7->vRatioSpeed = { 1.5f,1.5f };
				Info7->vScaleSpeed = { 40.f,100.f,40.f };
				Info7->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info7)))
					return E_FAIL;

				CPlayer01Wind2::WIND2INFO* Info8 = new CPlayer01Wind2::WIND2INFO;
				Info8->fBlurPower = 5.f;
				Info8->vLocalPos = { 0.f,0.f,1.f,1.f };
				Info8->vColor = { 1.f,1.f,0.9f,1.f };
				Info8->vColor2 = { 0.f,1.f,0.9f,1.f };
				Info8->vRatio = { 1.f,1.f };
				Info8->vRatioSpeed = { 1.5f,1.5f };
				Info8->vScaleSpeed = { 40.f,100.f,40.f };
				Info8->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info8)))
					return E_FAIL;

				PTM->Create_InstanceParticle(L"Player01RushKickEnd", m_pTransformCom->Get_WorldFloat4x4());
			}
			
			else
			{
				CWind::WINDINFO* Info = new CWind::WINDINFO;
				Info->vColor = { 0.7f,0.7f,0.7f,1.f };
				Info->vColor2 = { 0.7f,0.7f,0.7f,1.f };
				Info->vFixPosSpeed = { 0.f,0.f,0.f };
				Info->vFixScaleSpeed = { 30.f,30.f,30.f };
				Info->vRatio = { 1.f,1.f };
				Info->vRatioSpeed = { 2.f,2.f };
				Info->vUVFixSpeed = { 1.f,-3.f };
				_matrix Effectmatrix;
				Effectmatrix.r[0] = { 1.f, 0.f, 0.f, 0.f };
				Effectmatrix.r[1] = { 0.f, -0.587f, 0.81f, 0.f };
				Effectmatrix.r[2] = { 0.f, -0.81f, -0.588f, 0.f };
				Effectmatrix.r[3] = { 0.f, 1.5f, -0.5f, 1.f };
				XMStoreFloat4x4(&Info->WorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
				if (FAILED(GI->Add_GameObjectToLayer(L"Wind", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info)))
					return E_FAIL;

				CWind::WINDINFO* Info2 = new CWind::WINDINFO;
				Info2->vColor = { 0.7f,0.7f,0.7f,1.f };
				Info2->vColor2 = { 0.7f,0.7f,0.7f,1.f };
				Info2->vFixPosSpeed = { 0.f,0.f,0.f };
				Info2->vFixScaleSpeed = { 40.f,40.f,40.f };
				Info2->vRatio = { 1.f,1.f };
				Info2->vRatioSpeed = { 2.f,2.f };
				Info2->vUVFixSpeed = { 1.f,-3.f };
				Effectmatrix.r[0] = { 1.f, 0.f, 0.f, 0.f };
				Effectmatrix.r[1] = { 0.f, -0.587f, 0.81f, 0.f };
				Effectmatrix.r[2] = { 0.f, -0.81f, -0.588f, 0.f };
				Effectmatrix.r[3] = { 0.f, 1.5f, -0.5f, 1.f };
				XMStoreFloat4x4(&Info2->WorldMatrix, Effectmatrix * m_pTransformCom->Get_WorldMatrix());
				if (FAILED(GI->Add_GameObjectToLayer(L"Wind", GI->Get_Cur_LevelIndex(), L"Layer_Player01Effect", &Info2)))
					return E_FAIL;

				CPlayer01Pillar2::PILLAR2INFO* Info4 = new CPlayer01Pillar2::PILLAR2INFO;
				Info4->fBlurPower = 3.f;
				Info4->vColor = { 1.f,1.f,0.5f,1.f };
				Info4->vColor2 = { 1.f,0.1f,0.f,1.f };
				Info4->vRatio = { 1.f,1.f };
				Info4->vRatioSpeed = { 2.f,2.f };
				Info4->vLocalPos = { 0.f,0.f,1.f,1.f };
				Info4->vScaleSpeed = { 50.f,80.f,50.f };
				Info4->vScale = { 0.1f,1.f,0.1f };
				Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				GI->Add_GameObjectToLayer(L"Player01Pillar2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4);

				CPlayer01Pillar3::PILLAR3INFO* Info5 = new CPlayer01Pillar3::PILLAR3INFO;
				Info5->fBlurPower = 3.f;
				Info5->vColor = { 1.f,1.f,0.5f,1.f };
				Info5->vColor2 = { 1.f,0.1f,0.f,1.f };
				Info5->vRatio = { 1.f,1.f };
				Info5->vRatioSpeed = { 2.f,2.f };
				Info5->vLocalPos = { 0.f,0.f,1.f,1.f };
				Info5->vScaleSpeed = { 50.f,50.f,50.f };
				Info5->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				GI->Add_GameObjectToLayer(L"Player01Pillar3", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info5);

				CPlayer01SmokeBall::SMOKEBALLINFO* Info6 = new CPlayer01SmokeBall::SMOKEBALLINFO;
				Info6->fBlurPower = 2.f;
				Info6->vColor = { 0.7f,0.7f,0.7f,1.f };
				Info6->vColor2 = { 0.7f,0.7f,0.7f,1.f };
				Info6->vLocalPos = { 0.f,0.f,1.f,1.f };
				Info6->vRatio = { 1.f,1.f };
				Info6->vRatioSpeed = { 2.f,2.f };
				Info6->vScale = { 0.1f,0.1f,0.1f };
				Info6->vScaleSpeed = { 250.f,250.f,250.f };
				Info6->vUvFixSpeed = { 1.f,3.f };
				Info6->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				GI->Add_GameObjectToLayer(L"Player01SmokeBall", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info6);

				CPlayer01Wind1::WIND1INFO* Info7 = new CPlayer01Wind1::WIND1INFO;
				Info7->fBlurPower = 5.f;
				Info7->vLocalPos = { 0.f,0.f,1.f,1.f };
				Info7->vColor = { 1.f,1.f,0.5f,1.f };
				Info7->vColor2 = { 1.f,0.1f,0.f,1.f };
				Info7->vRatio = { 1.f,1.f };
				Info7->vRatioSpeed = { 1.5f,1.5f };
				Info7->vScaleSpeed = { 40.f,100.f,40.f };
				Info7->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info7)))
					return E_FAIL;

				CPlayer01Wind2::WIND2INFO* Info8 = new CPlayer01Wind2::WIND2INFO;
				Info8->fBlurPower = 5.f;
				Info8->vLocalPos = { 0.f,0.f,1.f,1.f };
				Info8->vColor = { 1.f,1.f,0.5f,1.f };
				Info8->vColor2 = { 1.f,0.1f,0.f,1.f };
				Info8->vRatio = { 1.f,1.f };
				Info8->vRatioSpeed = { 1.5f,1.5f };
				Info8->vScaleSpeed = { 40.f,100.f,40.f };
				Info8->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
				if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info8)))
					return E_FAIL;

				PTM->Create_InstanceParticle(L"Player01RushKickEndB", m_pTransformCom->Get_WorldFloat4x4());
			}


		}

		case Client::CPlayer01::ET_SKILL7:
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

		case Client::CPlayer01::ET_SKILL7_2:
		{
			/*CPlayer01Wind2::WIND2INFO* Info4 = new CPlayer01Wind2::WIND2INFO;
			Info4->fBlurPower = 1.f;
			Info4->vLocalPos = { 0.f,0.f,0.f,1.f };
			Info4->vColor = { 1.f,1.f,0.9f,1.f };
			Info4->vColor2 = { 0.f,1.f,0.9f,1.f };
			Info4->vRatio = { 1.f,1.f };
			Info4->vRatioSpeed = { 1.5f,1.5f };
			Info4->vScaleSpeed = { 20.f,30.f,20.f };
			Info4->vScale = { 5.f,5.f,5.f };
			Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			Info4->fTurnSpeed = -5.f;
			if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4)))
				return E_FAIL;

			CPlayer01Wind2::WIND2INFO* Info5 = new CPlayer01Wind2::WIND2INFO;
			Info5->fBlurPower = 5.f;
			Info5->vLocalPos = { 0.f,0.f,0.f,1.f };
			Info5->vColor = { 1.f,1.f,0.9f,1.f };
			Info5->vColor2 = { 0.f,1.f,0.9f,1.f };
			Info5->vRatio = { 1.f,1.f };
			Info5->vRatioSpeed = { 1.5f,1.5f };
			Info5->vScaleSpeed = { 20.f,30.f,20.f };
			Info5->vScale = { 5.f,5.f,5.f };
			Info5->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			Info5->fTurnSpeed = -10.f;
			if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info5)))
				return E_FAIL;*/
			

			break;
		}

		case Client::CPlayer01::ET_SKILL7_3:
		{
			if (m_ePlayerMode != PMODE_BURST)
			{
				PTM->Create_InstanceParticle(L"Player01Skill7", m_pTransformCom->Get_WorldFloat4x4());
				PTM->Create_InstanceParticle(L"Player01Skill7_2", m_pTransformCom->Get_WorldFloat4x4());
			}
			else
			{
				PTM->Create_InstanceParticle(L"Player01Skill7B", m_pTransformCom->Get_WorldFloat4x4());
				PTM->Create_InstanceParticle(L"Player01Skill7_2B", m_pTransformCom->Get_WorldFloat4x4());
			}
		
		}

	}

	return S_OK;
	
	

}

HRESULT CPlayer01::Ready_Trails()
{
	CSwordTrail::TRAILINFO Info;
	Info.HighAndLow.vHigh = { 0.3f,0.f,0.f };
	Info.HighAndLow.vLow = { -0.3f,0.f,0.f };
	Info.TexName = L"SwordTrail01";
	Info.vColor1 = { 1.f,0.f,0.f,1.f };
	Info.vColor2 = { 1.f,0.f,0.f,1.f };
	Info.vRatio = { 1.f,1.f };
	Info.bFixColor = true;
	Info.iShaderPass = 1;
	Info.fBlurPower = 2.f;
	CGameObject*		SwordTrail1 = GI->Clone_GameObject(TEXT("SwordTrailObj"), &Info);
	if (nullptr == SwordTrail1)
		return E_FAIL;
	m_SwordTrails.push_back((CSwordTrailObj*)SwordTrail1);


	/*((CPointTrailObj*)m_PointTrails[0])->Set_Color(_float4{ 1.f,1.f,1.f,1.f });
	((CPointTrailObj*)m_PointTrails[1])->Set_Color(_float4{ 1.f,1.f,1.f,1.f });
	((CPointTrailObj*)m_PointTrails[0])->Set_Color2(_float4{ 1.f,0.6f,0.f,1.f });
	((CPointTrailObj*)m_PointTrails[1])->Set_Color2(_float4{ 1.f,0.6f,0.f,1.f });
	((CPointTrailObj*)m_PointTrails[0])->Set_BlurPower(0.5f);
	((CPointTrailObj*)m_PointTrails[1])->Set_BlurPower(0.5f);
	((CPointTrailObj*)m_PointTrails[0])->Set_Ratio(_float2{ 1.f,1.f });
	((CPointTrailObj*)m_PointTrails[1])->Set_Ratio(_float2{ 1.f,1.f });
	((CPointTrailObj*)m_PointTrails[0])->Set_Size(0.07f);
	((CPointTrailObj*)m_PointTrails[1])->Set_Size(0.07f);
	((CPointTrailObj*)m_PointTrails[0])->Set_MaxIndex(10);
	((CPointTrailObj*)m_PointTrails[1])->Set_MaxIndex(10);*/

	CPointTrail::TRAILCREATEINFO Info2;
	Info2.vColor = { 1.f, 1.f, 0.8f,1.f };
	Info2.vColor2 = { 1.f, 0.5f, 0.f,1.f };
	Info2.bFixColor = true;
	Info2.fBlurPower = 0.5f;
	Info2.iShaderPass = 1;
	Info2.vRatio = { 1.f,1.f };
	Info2.fSize = 0.07f;
	Info2.TexName = L"PointTrail01";
	Info2.iMaxIndex = 10;

	CGameObject*		LKick = GI->Clone_GameObject(TEXT("PointTrailObj"), &Info2);
	if (nullptr == LKick)
		return E_FAIL;
	m_PointTrails.push_back((CPointTrailObj*)LKick);

	CGameObject*		RKick = GI->Clone_GameObject(TEXT("PointTrailObj"), &Info2);
	if (nullptr == RKick)
		return E_FAIL;
	m_PointTrails.push_back((CPointTrailObj*)RKick);


	Info2.vColor = { 1.f, 1.f, 1.f,1.f };
	Info2.vColor2 = { 0.f, 0.1f, 0.4f,1.f };
	Info2.bFixColor = true;
	Info2.fBlurPower = 3.f;
	Info2.iShaderPass = 1;
	Info2.vRatio = { 1.f,1.f };
	Info2.fSize = 0.5f;
	Info2.TexName = L"WeaponTrail";
	Info2.iMaxIndex = 3;
	CGameObject*		Body = GI->Clone_GameObject(TEXT("PointTrailObj"), &Info2);
	if (nullptr == Body)
		return E_FAIL;
	m_PointTrails.push_back((CPointTrailObj*)Body);

	Info2.vColor = { 1.f, 1.f, 1.f,1.f };
	Info2.vColor2 = { 0.f, 1.f, 0.9f,1.f };
	Info2.bFixColor = true;
	Info2.fBlurPower = 3.f;
	Info2.iShaderPass = 1;
	Info2.vRatio = { 1.f,1.f };
	Info2.fSize = 0.5f;
	Info2.TexName = L"PointTrail01";
	Info2.iMaxIndex = 3;
	CGameObject*		LHand = GI->Clone_GameObject(TEXT("PointTrailObj"), &Info2);
	if (nullptr == LHand)
		return E_FAIL;
	m_PointTrails.push_back((CPointTrailObj*)LHand);

	CGameObject*		RHand = GI->Clone_GameObject(TEXT("PointTrailObj"), &Info2);
	if (nullptr == RHand)
		return E_FAIL;
	m_PointTrails.push_back((CPointTrailObj*)RHand);


	CGameObject*		pMotionTrail1 = GI->Clone_GameObject(TEXT("Player01MotionTrail"));
	if (nullptr == pMotionTrail1)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail1);

	CGameObject*		pMotionTrail2 = GI->Clone_GameObject(TEXT("Player01MotionTrail"));
	if (nullptr == pMotionTrail2)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail2);

	CGameObject*		pMotionTrail3 = GI->Clone_GameObject(TEXT("Player01MotionTrail"));
	if (nullptr == pMotionTrail3)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail3);

	CGameObject*		pMotionTrail4 = GI->Clone_GameObject(TEXT("Player01MotionTrail"));
	if (nullptr == pMotionTrail4)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail4);

	CGameObject*		pMotionTrail5 = GI->Clone_GameObject(TEXT("Player01MotionTrail"));
	if (nullptr == pMotionTrail5)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail5);

	CGameObject*		pMotionTrail6 = GI->Clone_GameObject(TEXT("Player01MotionTrail"));
	if (nullptr == pMotionTrail6)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail6);

	CGameObject*		pMotionTrail7 = GI->Clone_GameObject(TEXT("Player01MotionTrail"));
	if (nullptr == pMotionTrail7)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail7);

	CGameObject*		pMotionTrail8 = GI->Clone_GameObject(TEXT("Player01MotionTrail"));
	if (nullptr == pMotionTrail8)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail8);

	CGameObject*		pMotionTrail9 = GI->Clone_GameObject(TEXT("Player01MotionTrail"));
	if (nullptr == pMotionTrail9)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail9);

	CGameObject*		pMotionTrail10 = GI->Clone_GameObject(TEXT("Player01MotionTrail"));
	if (nullptr == pMotionTrail10)
	return E_FAIL;
	m_MotionTrails.push_back(pMotionTrail10);
	
	return S_OK;
}

void CPlayer01::Create_PointTrail(_float fTimeDelta)
{
	Control_KickTrail(fTimeDelta);
	Control_BodyTrail(fTimeDelta);
	Control_HandTrail(fTimeDelta);
}

void CPlayer01::Create_SwordTrail(_float fTimeDelta)
{
	_matrix LHandMatrix;
	if (m_ePlayerMode != PMODE_BURST)
	{
		LHandMatrix = m_Sockets[SOCKET_WEAPON_L]->Get_CombinedTransformation()* m_pModelCom[m_eAnimModel]->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
	}
	else
	{
		LHandMatrix = m_Sockets[SOCKET_WEAPON_L2]->Get_CombinedTransformation() * m_pModelCom[m_eAnimModel]->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
	}

	if(m_SwordTrails[STR_LHAND]->Get_TrailOn())
		m_SwordTrails[STR_LHAND]->Tick(fTimeDelta, LHandMatrix);

}

void CPlayer01::Set_MotionTrail(_float fTimeDelta)
{
	m_fMotionTrailAcc += m_fMotionTrailSpeed * fTimeDelta;
	if (m_fMotionTrailAcc >= m_fMotionTrailLimit)
	{
		switch (m_ePlayerMode)
		{
		case Client::CPlayer01::PMODE_NORMAL:
		{
			CPlayer01MotionTrail::PLAYER01MOTIONTRAILINFO Info;
			Info.WorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			Info.eBodyType = CPlayer01MotionTrail::BODYA;
			Info.eHeadType = CPlayer01MotionTrail::HEADA;
			Info.Bodys = m_pModelCom[MODEL_NORMAL]->Get_HierarchyPointer();
			Info.HeadAMatrix = ((CPlayer01HairA*)m_Parts[PARTS_HAIRA])->Get_ConbineMatrix();
			Info.Heads = ((CPlayer01HairA*)m_Parts[PARTS_HAIRA])->Get_ModelCom()->Get_HierarchyPointer();
			((CPlayer01MotionTrail*)m_MotionTrails[m_iMotionTrailIndex])->Set_OnTrail(Info);
			m_fMotionTrailAcc = 0.f;
			++m_iMotionTrailIndex;
		}
			break;
		case Client::CPlayer01::PMODE_BATTLE:
		{
			CPlayer01MotionTrail::PLAYER01MOTIONTRAILINFO Info;
			Info.WorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			Info.eBodyType = CPlayer01MotionTrail::BODYA;
			Info.eHeadType = CPlayer01MotionTrail::HEADB;
			Info.Bodys = m_pModelCom[MODEL_NORMAL]->Get_HierarchyPointer();
			Info.HeadBMatrix = ((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->Get_ConbineMatrix();
			Info.Heads2 = ((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->Get_ModelCom()->Get_HierarchyPointer();
			((CPlayer01MotionTrail*)m_MotionTrails[m_iMotionTrailIndex])->Set_OnTrail(Info);
			m_fMotionTrailAcc = 0.f;
			++m_iMotionTrailIndex;
		}
			break;
		case Client::CPlayer01::PMODE_BURST:
		{
			CPlayer01MotionTrail::PLAYER01MOTIONTRAILINFO Info;
			Info.WorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
			Info.eBodyType = CPlayer01MotionTrail::BODYB;
			Info.eHeadType = CPlayer01MotionTrail::HEADC;
			Info.Bodys2 = m_pModelCom[MODEL_BURST]->Get_HierarchyPointer();
			Info.HeadCMatrix = ((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->Get_ConbineMatrix();
			Info.Heads3 = ((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->Get_ModelCom()->Get_HierarchyPointer();
			((CPlayer01MotionTrail*)m_MotionTrails[m_iMotionTrailIndex])->Set_OnTrail(Info);
			m_fMotionTrailAcc = 0.f;
			++m_iMotionTrailIndex;
		}
			break;
		}		
		
		if (m_iMotionTrailIndex >= m_MotionTrails.size())
		{
			m_bMotionTrail = false;
			m_iMotionTrailIndex = 0;
		}
	}
}

void CPlayer01::Control_KickTrail(_float fTimeDelta)
{

	if(m_bLKickTrail)
	{
		m_PointTrails[PTR_LKICK]->TrailOn();
		if (m_eCurState != IDLE2)
		{
			_matrix	LKickMatrix;
			if(m_ePlayerMode != PMODE_BURST)
				LKickMatrix = m_Sockets[SOCKET_KICK_L]->Get_CombinedTransformation()* m_pModelCom[m_eAnimModel]->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
			else
				LKickMatrix = m_Sockets[SOCKET_KICK_L2]->Get_CombinedTransformation()* m_pModelCom[m_eAnimModel]->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
			_float4 LKickPos;
			XMStoreFloat4(&LKickPos, LKickMatrix.r[3]);
			m_PointTrails[PTR_LKICK]->Add_Point(LKickPos);
			

			
		}
		m_PointTrails[PTR_LKICK]->Tick(fTimeDelta);
	}
	else
	{
		m_PointTrails[PTR_LKICK]->TrailOff();
	}

	if (m_bRKickTrail)
	{
		m_PointTrails[PTR_RKICK]->TrailOn();
		if (m_eCurState != IDLE2)
		{

			_matrix	LKickMatrix = m_Sockets[SOCKET_KICK_R2]->Get_CombinedTransformation()* m_pModelCom[m_eAnimModel]->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
			_float4 LKickPos;
			XMStoreFloat4(&LKickPos, LKickMatrix.r[3]);
			m_PointTrails[PTR_RKICK]->Add_Point(LKickPos);



		}
		m_PointTrails[PTR_RKICK]->Tick(fTimeDelta);
	}
	else
	{
		m_PointTrails[PTR_RKICK]->TrailOff();
	}

	
}

void CPlayer01::Control_BodyTrail(_float fTimeDelta)
{
	if (m_bBodyTrail)
	{
		m_PointTrails[PTR_BODY]->TrailOn();	
		CHierarchyNode*      Root = m_pModelCom[MODEL_BURST]->Get_HierarchyNode("RootPart2_M");
		_matrix RootMatrix = Root->Get_CombinedTransformation() *  m_pModelCom[m_eAnimModel]->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
		_float4 Pos;
		XMStoreFloat4(&Pos, RootMatrix.r[3]);
		m_PointTrails[PTR_BODY]->Add_Point(Pos);
		m_PointTrails[PTR_BODY]->Tick(fTimeDelta);
		
	}
	else
	{
		m_PointTrails[PTR_BODY]->TrailOff();
	}
}

void CPlayer01::Control_HandTrail(_float fTimeDelta)
{
	if (m_bLHandTrail)
	{
		_matrix RootMatrix;
		m_PointTrails[PTR_LHAND]->TrailOn();
		if(m_ePlayerMode != PMODE_BURST)
			RootMatrix = m_Sockets[SOCKET_WEAPON_L]->Get_CombinedTransformation() *  m_pModelCom[m_eAnimModel]->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
		else
			RootMatrix = m_Sockets[SOCKET_WEAPON_L2]->Get_CombinedTransformation() *  m_pModelCom[m_eAnimModel]->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
		_float4 Pos;
		XMStoreFloat4(&Pos, RootMatrix.r[3]);
		m_PointTrails[PTR_LHAND]->Add_Point(Pos);
		m_PointTrails[PTR_LHAND]->Tick(fTimeDelta);

	}
	else
	{
		m_PointTrails[PTR_LHAND]->TrailOff();
	}

	if (m_bRHandTrail)
	{
		_matrix RootMatrix;
		m_PointTrails[PTR_RHAND]->TrailOn();
		if (m_ePlayerMode != PMODE_BURST)
			RootMatrix = m_Sockets[SOCKET_WEAPON_R]->Get_CombinedTransformation() *  m_pModelCom[m_eAnimModel]->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
		else
			RootMatrix = m_Sockets[SOCKET_WEAPON_R2]->Get_CombinedTransformation() *  m_pModelCom[m_eAnimModel]->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
		_float4 Pos;
		XMStoreFloat4(&Pos, RootMatrix.r[3]);
		m_PointTrails[PTR_RHAND]->Add_Point(Pos);
		m_PointTrails[PTR_RHAND]->Tick(fTimeDelta);

	}
	else
	{
		m_PointTrails[PTR_RHAND]->TrailOff();
	}
}

void CPlayer01::Player_Die()
{
	m_bDie = true;
	m_bSelectPlayer = false;
	PM->Change_Player(CPlayer::PLAYER_03);
}

void CPlayer01::Reset_SlowMotionTrail()
{
	for (auto& iter : m_MotionTrails)
	{
		((CPlayer01MotionTrail*)iter)->Set_AlphaLimit(0.1f);
		((CPlayer01MotionTrail*)iter)->Set_ShaderPass(8);
		((CPlayer01MotionTrail*)iter)->Set_BlurPower(0.3f);
		((CPlayer01MotionTrail*)iter)->Set_DownSpeed(1.f);
		((CPlayer01MotionTrail*)iter)->Set_UpSpeed(1.f);
	}
}

void CPlayer01::Set_SlowMotionTrail()
{
	for (auto& iter : m_MotionTrails)
	{
		((CPlayer01MotionTrail*)iter)->Set_AlphaLimit(1.f);
		((CPlayer01MotionTrail*)iter)->Set_ShaderPass(10);
		((CPlayer01MotionTrail*)iter)->Set_DownSpeed(20.f);
		((CPlayer01MotionTrail*)iter)->Set_UpSpeed(5.f);
	}
}

void CPlayer01::Set_Rage2MotionTrail()
{
	for (auto& iter : m_MotionTrails)
	{
		((CPlayer01MotionTrail*)iter)->Set_AlphaLimit(0.3f);
		((CPlayer01MotionTrail*)iter)->Set_ShaderPass(8);
		((CPlayer01MotionTrail*)iter)->Set_BlurPower(0.5f);
		((CPlayer01MotionTrail*)iter)->Set_DownSpeed(1.f);
		((CPlayer01MotionTrail*)iter)->Set_UpSpeed(3.f);
	}
}

void CPlayer01::Check_PlayerLight(_float fTimeDelta)
{
	if (m_bPlayerLight)
	{
		m_eShaderPass = SHADER_LIGHT;
		((CPlayer01HairA*)m_Parts[PARTS_HAIRA])->Set_PlayerLight(true);
		((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->Set_PlayerLight(true);
		((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->Set_PlayerLight(true);
		m_fPlayerLightAcc += 1.f * fTimeDelta;
		if (m_fPlayerLightAcc >= m_fPlayerLightLimit)
		{
			m_bPlayerLight = false;
			m_fPlayerLightAcc = 0.f;
			m_eShaderPass = SHADER_DEFAULT;
			((CPlayer01HairA*)m_Parts[PARTS_HAIRA])->Set_PlayerLight(false);
			((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->Set_PlayerLight(false);
			((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->Set_PlayerLight(false);
			Create_LightEffect();
		}
	}
}

void CPlayer01::Change_Sound()
{
	
	_uint iRandomVoice = GI->Get_Random(1, 3);
	

	GI->PlaySoundW(L"UI_GodChange.wav", SD_UI, 0.6f);

	switch (iRandomVoice)
	{
	case 1:
		GI->PlaySoundW(L"Hero01_ba_67.wav", SD_PLAYER1_VO, 1.f);
		break;
	case 2:
		GI->PlaySoundW(L"Hero01_ba_68.wav", SD_PLAYER1_VO, 1.f);
		break;
	case 3:
		GI->PlaySoundW(L"Hero01_ba_69.wav", SD_PLAYER1_VO, 1.f);
	}
}

void CPlayer01::Die_Sound()
{
	_uint iRandomVoice = GI->Get_Random(0, 2);

	switch (iRandomVoice)
	{
	case 0:
		GI->PlaySoundW(L"Hero01_ba_85.wav", SD_PLAYER1_VO, 1.f);
		break;
	case 1:
		GI->PlaySoundW(L"Hero01_ba_86.wav", SD_PLAYER1_VO, 1.f);
		break;
	case 2:
		GI->PlaySoundW(L"Hero01_ba_87.wav", SD_PLAYER1_VO, 1.f);
		break;
	default:
		break;
	}
	
}

void CPlayer01::Hit_Effect(CCreture* pOther)
{
	_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		vPos.y += 1.f;
		pOther->Create_HitEffect(vPos);
}

_bool CPlayer01::Get_IsJump()
{
	return m_pTransformCom->Get_Air();
}

void CPlayer01::Set_ScenePos(_float4 vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));
	m_pNavigationCom->Set_NaviIndex(m_pNavigationCom->Find_NaviIndex(XMLoadFloat4(&vPos)));

	Update_Parts();
	m_Parts[PARTS_HAIRB]->LateTick(1.f / 60.f);
	((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->SetInitPos();
	Tick_Col(m_pTransformCom->Get_WorldMatrix());
}

void CPlayer01::Set_SceneLook(_float4 vPos)
{
	Update_Parts();
	m_Parts[PARTS_HAIRB]->LateTick(1.f / 60.f);
	((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->SetInitPos();
	Tick_Col(m_pTransformCom->Get_WorldMatrix());
	m_pTransformCom->LookAt_ForLandObject(XMLoadFloat4(&vPos));
	XMStoreFloat3(&m_vTargetLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	
	
}

void CPlayer01::Set_SceneIdle()
{
	Reset_Collider();
	Set_State(IDLE1);
}

void CPlayer01::Set_SceneHit(HITTYPE eHitType, _float fPushPower, _float fAirPower, _float4 vPos, _float3 vDir)
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

void CPlayer01::Set_SceneRealHit(HITTYPE eHitType, _float fPushPower, _float fAirPower, _float4 vPos, _float3 vDir, _float fDamage)
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

void CPlayer01::ChangeOff()
{
	m_ePlayerMode = PMODE_NORMAL;
	m_eAnimModel = MODEL_NORMAL;
	m_eNextState = IDLE1;
	m_pModelCom[m_eAnimModel]->Set_AnimIndex(IDLE1);
	m_eCurState = IDLE1;
	m_bBattleChange = false;
	m_bLKickTrail = false;
	m_bRKickTrail = false;
	((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->Set_Sleep();
	((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->Set_Sleep();
	((CPlayer01HairA*)m_Parts[PARTS_HAIRA])->Set_Wake();

	Get_Colliders("Player_HairCut")->Set_Sleep();
	Get_Colliders("Player_HairCut_Burst")->Set_Sleep();
	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	Update_Parts();
	m_Parts[PARTS_HAIRA]->LateTick(1.f / 60.f);
	((CPlayer01HairA*)m_Parts[PARTS_HAIRA])->SetInitPos();

	m_bPlayerLight = true;
	m_fPlayerLightLimit = 0.1f;
	m_eShaderPass = SHADER_LIGHT;
	m_vPlayerLightColor = { 1.f,1.f,1.f };	
	((CPlayer01HairA*)m_Parts[PARTS_HAIRA])->Set_PlayerLight(true);
	((CPlayer01HairA*)m_Parts[PARTS_HAIRA])->Set_PlayerLightColor(_float3{ 1.f,1.f,1.f });

	PTM->Create_InstanceParticle(L"Player01BurstParticle", m_pTransformCom->Get_WorldFloat4x4());


}

void CPlayer01::HurtVoice()
{
	_uint iRandomVoice = GI->Get_Random(0, 6);
	switch (iRandomVoice)
	{
	case 0:
		GI->PlaySoundW(L"Hero01_ba_14.wav", SD_PLAYER1_VO, 0.6f);
		break;
	case 1:
		GI->PlaySoundW(L"Hero01_ba_15.wav", SD_PLAYER1_VO, 0.6f);
		break;
	case 2:
		GI->PlaySoundW(L"Hero01_ba_16.wav", SD_PLAYER1_VO, 0.6f);
		break;
	case 3:
		GI->PlaySoundW(L"Hero01_ba_17.wav", SD_PLAYER1_VO, 0.6f);
		break;
	case 4:
		GI->PlaySoundW(L"Hero01_ba_18.wav", SD_PLAYER1_VO, 0.6f);
		break;
	case 5:
		GI->PlaySoundW(L"Hero01_ba_19.wav", SD_PLAYER1_VO, 0.6f);
		break;
	case 6:
		GI->PlaySoundW(L"Hero01_ba_20.wav", SD_PLAYER1_VO, 0.6f);
		break;
	}
}

void CPlayer01::PlayerPortal()
{
	m_bPlayerLight = true;
	m_fPlayerLightLimit = 0.1f;
	m_eShaderPass = SHADER_LIGHT;
	m_vPlayerLightColor = { 1.f,1.f,1.f };
	((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->Set_PlayerLight(true);
	((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->Set_PlayerLight(true);
	((CPlayer01HairC*)m_Parts[PARTS_HAIRC])->Set_PlayerLightColor(_float3{ 1.f,1.f,1.f });
	((CPlayer01HairB*)m_Parts[PARTS_HAIRB])->Set_PlayerLightColor(_float3{ 1.f,1.f,1.f });
	PTM->Create_InstanceParticle(L"Player01BurstParticle", m_pTransformCom->Get_WorldFloat4x4());
}

void CPlayer01::Create_MotionTrail(_float fTimeDelta)
{
	if (m_bMotionTrail)
	{
		if(m_MotionTrails.size() > 0)
			Set_MotionTrail(fTimeDelta);
	}

	for (auto& iter : m_MotionTrails)
	{
		iter->Tick(fTimeDelta);
	}

}

bool CPlayer01::Input_Direction()
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

void CPlayer01::Idle_KeyInput(_float fTimeDelta)
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

void CPlayer01::Run_KeyInput(_float fTimeDelta)
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

	Default_KeyInput(fTimeDelta);
	

}

void CPlayer01::Run_FastStartInput(_float fTimeDelta)
{
	if (CK_DOWN(DIK_SPACE))
	{
		m_fJumpSpeed = 8.f;
		m_pTransformCom->Jump(10.f);
		Set_State(JUMPSTART);
		return;
	}

}

void CPlayer01::Run_FastInput(_float fTimeDelta)
{
	if (CK_DOWN(DIK_SPACE))
	{
		m_fJumpSpeed = 8.f;
		m_pTransformCom->Jump(10.f);
		Set_State(JUMPSTART);
		return;
	}

	if (!Input_Direction())
	{
		m_pModelCom[m_eAnimModel]->Set_AnimIndex(RUNFASTEND);
		m_eCurState = RUNFASTEND;
		GI->PlaySoundW(L"stop-land.wav", SD_PLAYER1_SK, 0.3f);
		return;
	}

	if (CM_PRESS(DIMK_LBUTTON))
	{
		Set_State(ATTACK1);
		return;
	}

	if (CK_PRESS(DIK_Q))
	{
		Set_State(SKILL1);
		return;
	}

	if (CK_PRESS(DIK_E))
	{
		Set_State(SKILL2);
		return;
	}

	if (CK_PRESS(DIK_F))
	{
		Set_State(SKILL3);
		return;
	}

	if (CK_PRESS(DIK_R))
	{
		Set_State(SKILL4_START);
		return;
	}

	if (CK_PRESS(DIK_V))
	{
		Set_State(RAGESKILL1);
		return;
	}

	if (CK_PRESS(DIK_B))
	{
		Set_State(RAGESKILL2);
		return;
	}

	if (CK_PRESS(DIK_T))
	{
		Set_State(RUSHKICK);
		return;
	}

	if (CK_PRESS(DIK_G))
	{
		Set_State(SKILL7);
		return;
	}

	if (CK_PRESS(DIK_1))
	{
		Set_State(SKILL9);
		return;
	}

	if (CK_PRESS(DIK_2))
	{
		Set_State(SKILL5);
		return;
	}

	if (CK_PRESS(DIK_3))
	{
		Set_State(SKILL6);
		return;
	}

	if (CK_PRESS(DIK_TAB))
	{
		Set_State(SKILL8);
		return;
	}

	if (CK_PRESS(DIK_CAPSLOCK))
	{
		m_bVoid = true;
		m_fVoidSpeed = 10.f;
		m_pModelCom[m_eAnimModel]->Set_AnimIndex(VOIDFRONT);
		m_eCurState = VOIDFRONT;
		m_eNextState = m_eCurState;
		m_pModelCom[m_eAnimModel]->SetNextIndex(m_eNextState);
		Create_Effect(ET_DASH);
		return;
	}

	Change_Mode();
	
}

void CPlayer01::JumpLoof_KeyInput(_float fTimeDelta)
{
	Input_Direction();

	if (CM_DOWN(DIMK_LBUTTON))
	{
		Set_State(AIRATTACK1);
		m_pTransformCom->ResetAndJump(1.f);
		return;
	}

	if (CK_DOWN(DIK_4))
	{
		Set_State(SKILL6_START);
		return;
	}

}

void CPlayer01::JumpStart_KeyInput(_float fTimeDelta)
{
	Input_Direction();

	if (CM_DOWN(DIMK_LBUTTON))
	{
		Set_State(AIRATTACK1);
		m_pTransformCom->ResetAndJump(1.5f);
		return;
	}

	if (CK_DOWN(DIK_4))
	{
		Set_State(SKILL6_START);
		return;
	}
}

void CPlayer01::AirAttack1_KeyInput(_float fTimeDelta)
{
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() >= m_pModelCom[m_eAnimModel]->Get_TimeLimit(0))
	{
		if (CM_PRESS(DIMK_LBUTTON))
		{
			Input_Direction();
			Set_State(AIRATTACK2);
			m_pTransformCom->ResetAndJump(1.5f);
			return;
		}

		if (CK_PRESS(DIK_4))
		{
			Set_State(SKILL6_START);
			return;
		}
	}

	
}

void CPlayer01::AirAttack2_KeyInput(_float fTimeDelta)
{
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() >= m_pModelCom[m_eAnimModel]->Get_TimeLimit(0))
	{
		if (CM_PRESS(DIMK_LBUTTON))
		{
			Input_Direction();
			Set_State(AIRATTACK3);
			m_pTransformCom->ResetAndJump(1.5f);
			return;
		}

		if (CK_PRESS(DIK_4))
		{
			Set_State(SKILL6_START);
			return;
		}
	}

	
}

void CPlayer01::AirAttack3_KeyInput(_float fTimeDelta)
{
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() >= m_pModelCom[m_eAnimModel]->Get_TimeLimit(0))
	{
		if (CM_PRESS(DIMK_LBUTTON))
		{
			Input_Direction();
			Set_State(AIRATTACK5START);
			m_pTransformCom->ResetAndJump(1.5f);
			return;
		}

		if (CK_PRESS(DIK_4))
		{
			Set_State(SKILL6_START);
			return;
		}
	}
}

void CPlayer01::AirAttack5_KeyInput(_float fTimeDelta)
{
}

void CPlayer01::Attack1_KeyInput(_float fTimeDelta)
{


	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() >= m_pModelCom[m_eAnimModel]->Get_TimeLimit(0))
	{
		if (CM_PRESS(DIMK_LBUTTON))
		{
			Set_State(ATTACK2);
			return;
		}


	}
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() >= m_pModelCom[m_eAnimModel]->Get_TimeLimit(2))
	{
		Combo_KeyInput(fTimeDelta);
	}

}

void CPlayer01::Attack2_KeyInput(_float fTimeDelta)
{
	
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() >= m_pModelCom[m_eAnimModel]->Get_TimeLimit(0))
	{
		if (CM_PRESS(DIMK_LBUTTON))
		{
			Set_State(ATTACK3);
			return;
		}
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() >= m_pModelCom[m_eAnimModel]->Get_TimeLimit(2))
	{
		Combo_KeyInput(fTimeDelta);
	}

}

void CPlayer01::Attack3_KeyInput(_float fTimeDelta)
{
	
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() >= m_pModelCom[m_eAnimModel]->Get_TimeLimit(0))
	{
		if (CM_PRESS(DIMK_LBUTTON))
		{
			Set_State(ATTACK4);
			return;
		}
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() >= m_pModelCom[m_eAnimModel]->Get_TimeLimit(2))
	{
		Combo_KeyInput(fTimeDelta);
	}

}

void CPlayer01::Attack4_KeyInput(_float fTimeDelta)
{
	
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() >= m_pModelCom[m_eAnimModel]->Get_TimeLimit(0))
	{
		if (CM_PRESS(DIMK_LBUTTON))
		{
			Set_State(ATTACK5);
			return;
		}
	}

	if (CK_DOWN(DIK_CAPSLOCK))
	{
		m_bVoid = true;
		m_fVoidSpeed = 10.f;
		m_pModelCom[m_eAnimModel]->Set_AnimIndex(VOIDFRONT);
		m_eCurState = VOIDFRONT;
		m_eNextState = m_eCurState;
		m_pModelCom[m_eAnimModel]->SetNextIndex(m_eNextState);
		Create_Effect(ET_DASH);
		return;
	}

}

void CPlayer01::Attack5_KeyInput(_float fTimeDelta)
{

	if (CK_DOWN(DIK_CAPSLOCK))
	{
		m_bVoid = true;
		m_fVoidSpeed = 10.f;
		m_pModelCom[m_eAnimModel]->Set_AnimIndex(VOIDFRONT);
		m_eCurState = VOIDFRONT;
		m_eNextState = m_eCurState;
		m_pModelCom[m_eAnimModel]->SetNextIndex(m_eNextState);
		Create_Effect(ET_DASH);
		return;
	}
}

void CPlayer01::Skill4_Loof_KeyInput(_float fTimeDelta)
{
	if (CM_DOWN(DIMK_LBUTTON))
		m_bSkill4Loof = true;
}

void CPlayer01::Skill6_KeyInput(_float fTimeDelta)
{
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() >= m_pModelCom[m_eAnimModel]->Get_TimeLimit(0))
	{
		if (CM_PRESS(DIMK_LBUTTON))
		{
			Set_State(AIRATTACK1);
			m_fAirPower = 0.f;
			m_eHitType = TYPE_HIT;
			return;
		}

		if (CK_PRESS(DIK_4))
		{
			Set_State(SKILL6_START);
			return;
		}
	}
	
}

void CPlayer01::VoidFront_KeyInput(_float fTimeDelta)
{

}

void CPlayer01::Skill8_KeyInput(_float fTimeDelta)
{
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() >= m_pModelCom[m_eAnimModel]->Get_TimeLimit(1))
	{
		Default_KeyInput(fTimeDelta);
	}
}

void CPlayer01::Combo_KeyInput(_float fTimeDelta)
{
	if (CK_PRESS(DIK_LSHIFT))
	{
		Set_State(RUNFASTSTART);
		return;
	}

	if (CK_PRESS(DIK_Q))
	{
		Set_State(SKILL1);
		return;
	}

	if (CK_PRESS(DIK_E))
	{
		Set_State(SKILL2);
		return;
	}

	if (CK_PRESS(DIK_F))
	{
		Set_State(SKILL3);
		return;
	}

	if (CK_PRESS(DIK_R))
	{
		Set_State(SKILL4_START);
		return;
	}

	if (CK_PRESS(DIK_V))
	{
		Set_State(RAGESKILL1);
		return;
	}

	if (CK_PRESS(DIK_B))
	{
		Set_State(RAGESKILL2);
		return;
	}

	if (CK_PRESS(DIK_T))
	{
		Set_State(RUSHKICK);
		return;
	}

	if (CK_PRESS(DIK_G))
	{
		Set_State(SKILL7);
		return;
	}

	if (CK_PRESS(DIK_1))
	{
		Set_State(SKILL9);
		return;
	}

	if (CK_PRESS(DIK_2))
	{
		Set_State(SKILL5);
		return;
	}

	if (CK_PRESS(DIK_3))
	{
		Set_State(SKILL6);
		return;
	}

	if (CK_PRESS(DIK_TAB))
	{
		Set_State(SKILL8);
		return;
	}

	if (CK_PRESS(DIK_CAPSLOCK))
	{
		m_bVoid = true;
		m_fVoidSpeed = 10.f;
		m_pModelCom[m_eAnimModel]->Set_AnimIndex(VOIDFRONT);
		m_eCurState = VOIDFRONT;
		m_eNextState = m_eCurState;
		m_pModelCom[m_eAnimModel]->SetNextIndex(m_eNextState);
		Create_Effect(ET_DASH);
		return;
	}

	Change_Mode();
}

void CPlayer01::RushKick_KeyInput(_float fTimeDelta)
{
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() >= m_pModelCom[m_eAnimModel]->Get_TimeLimit(1))
	{
		Default_KeyInput(fTimeDelta);
	}
}

void CPlayer01::Skill1_KeyInput(_float fTimeDelta)
{
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() >= m_pModelCom[m_eAnimModel]->Get_TimeLimit(1))
	{
		Default_KeyInput(fTimeDelta);
	}
}

void CPlayer01::Skill2_KeyInput(_float fTimeDelta)
{
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() >= m_pModelCom[m_eAnimModel]->Get_TimeLimit(4))
	{
		Default_KeyInput(fTimeDelta);
	}
}

void CPlayer01::Skill3_KeyInput(_float fTimeDelta)
{
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() >= m_pModelCom[m_eAnimModel]->Get_TimeLimit(2))
	{
		Default_KeyInput(fTimeDelta);
	}
}

void CPlayer01::Skill4End_KeyInput(_float fTimeDelta)
{
	
}

void CPlayer01::Skill5_KeyInput(_float fTimeDelta)
{
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() >= m_pModelCom[m_eAnimModel]->Get_TimeLimit(1))
	{
		Default_KeyInput(fTimeDelta);
	}
}

void CPlayer01::Skill7_KeyInput(_float fTimeDelta)
{
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() >= m_pModelCom[m_eAnimModel]->Get_TimeLimit(6))
	{
		if (m_ePlayerMode != PMODE_BURST)
		{
			m_bLKickTrail = false;
			m_bRKickTrail = false;
		}
		else
		{
			m_bLKickTrail = true;
			m_bRKickTrail = true;
		}

		((CPointTrailObj*)m_PointTrails[0])->Set_Color(_float4{ 1.f,1.f,0.8f,1.f });
		((CPointTrailObj*)m_PointTrails[0])->Set_Color2(_float4{ 1.f,0.5f,0.f,1.f });
		((CPointTrailObj*)m_PointTrails[0])->Set_BlurPower(0.5f);
		((CPointTrailObj*)m_PointTrails[0])->Set_Ratio(_float2{ 1.f,1.f });
		((CPointTrailObj*)m_PointTrails[0])->Set_Size(0.07f);
		((CPointTrailObj*)m_PointTrails[0])->Set_MaxIndex(10);
		((CPointTrailObj*)m_PointTrails[0])->Set_TimeLimit(0.017f);
		Default_KeyInput(fTimeDelta);
	}
}

void CPlayer01::Skill9_KeyInput(_float fTimeDelta)
{
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() >= m_pModelCom[m_eAnimModel]->Get_TimeLimit(1))
	{
		Default_KeyInput(fTimeDelta);
	}
}

void CPlayer01::AirAttack1_Update(_float fTimeDelta)
{
	if (!m_pTransformCom->Get_Air())
	{
		m_pModelCom[m_eAnimModel]->Set_AnimIndex(JUMPEND);
		m_eCurState = JUMPEND;
	}
	else
	{
		m_bAttackCol = false;
		if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(1) && !m_bAirAttack1Col)
		{
			GI->PlaySoundW(L"se_HE01_Nattack_2.wav", SD_PLAYER1_SK, 0.8f);
			GI->PlaySoundW(L"Hero01_ba_05.wav", SD_PLAYER1_VO, 1.f);

			m_bAttackCol = true;
			m_bAirAttack1Col = true;
			CRM->Get_Cam()->Shake(0.05f, 0.15f);
			Create_Effect(ET_AIRATTACK1);
			
		}
	}
}

void CPlayer01::AirAttack2_Update(_float fTimeDelta)
{
	if (!m_pTransformCom->Get_Air())
	{
		m_pModelCom[m_eAnimModel]->Set_AnimIndex(JUMPEND);
		m_eCurState = JUMPEND;
	}
	else
	{
		m_bAttackCol = false;
		if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(0)-1 && !m_bAirAttack2Col)
		{
			GI->PlaySoundW(L"se_HE01_Nattack_3_01.wav", SD_PLAYER1_SK, 0.8f);
			m_bAttackCol = true;
			m_bAirAttack2Col = true;
			CRM->Get_Cam()->Shake(0.05f, 0.15f);
			Create_Effect(ET_AIRATTACK2);
		}
	}
}

void CPlayer01::AirAttack3_Update(_float fTimeDelta)
{
	if (!m_pTransformCom->Get_Air())
	{
		m_pModelCom[m_eAnimModel]->Set_AnimIndex(JUMPEND);
		m_eCurState = JUMPEND;
	}
	else
	{
		m_bAttackCol = false;
		if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(1) && !m_bAirAttack3Col)
		{
			GI->PlaySoundW(L"se_HE01_Nattack_3_02.wav", SD_PLAYER1_SK, 0.8f);
			GI->PlaySoundW(L"Hero01_ba_03.wav", SD_PLAYER1_VO, 1.f);
			m_bAttackCol = true;
			m_bAirAttack3Col = true;
			CRM->Get_Cam()->Shake(0.05f, 0.15f);
			Create_Effect(ET_AIRATTACK3);
		}
	}
}

void CPlayer01::AirAttack4_Update(_float fTimeDelta)
{
	if (!m_pTransformCom->Get_Air())
	{
		m_pModelCom[m_eAnimModel]->Set_AnimIndex(JUMPEND);
		m_eCurState = JUMPEND;
	}
}

void CPlayer01::AirAttack5End_Update(_float fTimeDelta)
{
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(0))
	{
		if (CM_PRESS(DIMK_LBUTTON))
		{
			Set_State(SKILL10);
		}
	}
}

void CPlayer01::AirAttack5Loof_Update(_float fTimeDelta)
{
	if (!m_pTransformCom->Get_Air())
	{
		CRM->Get_Cam()->Shake(0.4f, 0.2f, false, 0.1f);
		m_pModelCom[m_eAnimModel]->Set_AnimIndex(AIRATTACK5END);
		m_eCurState = AIRATTACK5END;
		m_eNextState = m_eCurState;
		m_pModelCom[m_eAnimModel]->SetNextIndex(m_eNextState);
		Create_Effect(ET_SUNDER);
		Create_Particle(PT_SUNDER);
	}
	else
	{
		m_bAttackCol = false;
		if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == 0 && !m_bAirAttack5Col)
		{
			GI->PlaySoundW(L"se_HE01_Skill02_4.wav", SD_PLAYER1_SK, 0.8f);
			GI->PlaySoundW(L"Hero01_ba_43.wav", SD_PLAYER1_VO, 1.f);
			m_bAttackCol = true;
			m_bAirAttack5Col = true;
			
		}
	}
}

void CPlayer01::AirAttack5Start_Update(_float fTimeDelta)
{
}

void CPlayer01::Attack1_Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(1) && !m_bAttack1Col)
	{
		GI->PlaySoundW(L"se_HE01_Nattack_1.wav", SD_PLAYER1_SK, 0.8f);
		GI->PlaySoundW(L"Hero01_ba_03.wav", SD_PLAYER1_VO, 1.f);

		m_bAttackCol = true;
		m_bAttack1Col = true;
		CRM->Get_Cam()->Shake(0.05f, 0.15f); // 0.05 0.15
		Create_Effect(ET_ATTACK1);
		
	}
}

void CPlayer01::Attack2_Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() >= m_pModelCom[m_eAnimModel]->Get_TimeLimit(1) && !m_bAttack2Col)
	{
		GI->PlaySoundW(L"se_HE01_Nattack_2.wav", SD_PLAYER1_SK, 0.8f);

		m_bAttackCol = true;
		m_bAttack2Col = true;
		CRM->Get_Cam()->Shake(0.05f, 0.15f);
		Create_Effect(ET_ATTACK2);
		
	}
}

void CPlayer01::Attack3_Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() >= m_pModelCom[m_eAnimModel]->Get_TimeLimit(1) && !m_bAttack3Col)
	{
		GI->PlaySoundW(L"se_HE01_Nattack_3_01.wav", SD_PLAYER1_SK, 0.8f);
		GI->PlaySoundW(L"Hero01_ba_02.wav", SD_PLAYER1_VO, 1.f);

		m_bAttackCol = true;
		m_bAttack3Col = true;
		CRM->Get_Cam()->Shake(0.06f, 0.15f);
		Create_Effect(ET_ATTACK3);
	}
}

void CPlayer01::Attack4_Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() >= m_pModelCom[m_eAnimModel]->Get_TimeLimit(1) && !m_bAttack4Col)
	{
		GI->PlaySoundW(L"se_HE01_Nattack_3_02.wav", SD_PLAYER1_SK, 0.8f);
		GI->PlaySoundW(L"Hero01_ba_01.wav", SD_PLAYER1_VO, 1.f);

		m_bAttackCol = true;
		m_bAttack4Col = true;
		CRM->Get_Cam()->Shake(0.06f, 0.15f);
		Create_Effect(ET_ATTACK4);
	}
}

void CPlayer01::Attack5_Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(0) && !m_bAttack5Col)
	{
		GI->PlaySoundW(L"se_HE01_Skill02_3.wav", SD_PLAYER1_SK, 0.8f);
		GI->PlaySoundW(L"Hero01_ba_05.wav", SD_PLAYER1_VO, 1.f);

		m_bAttackCol = true;
		m_bAttack5Col = true;
		CRM->Get_Cam()->Shake(0.3f, 0.2f, true, 0.01f);
		CRM->Get_Cam()->Fov(-3.f, 0.1f, 0.3f, true);
		Create_Effect(ET_ATTACK5);
		m_eShaderPass = SHADER_LIGHT;
		m_bPlayerLight = true;
		m_fPlayerLightLimit = 0.1f;
	}
}

void CPlayer01::GroggyEnd_Update(_float fTimeDelta)
{

}

void CPlayer01::GroggyLoof_Update(_float fTimeDelta)
{
	
}

void CPlayer01::GroggyStart_Update(_float fTimeDelta)
{
	
}

void CPlayer01::VoidFront_Update(_float fTimeDelta)
{
	if (!m_bVoidOn)
	{
		GI->PlaySoundW(L"Track 02.wav", SD_PLAYER1_SK, 0.5f);
		m_bVoidOn = true;
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(0))
	{
		if (CM_PRESS(DIMK_LBUTTON))
		{
			m_bVoid = false;
			Input_Direction();
			Change_ColRange(_float3{ 1.5f,1.5f,1.5f }, _float3{ 0.f,1.5f,1.5f });
			m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER01);
			m_pModelCom[m_eAnimModel]->Set_AnimIndex(VOIDATTACK);
			m_eCurState = VOIDATTACK;
			m_eNextState = m_eCurState;
			m_pModelCom[m_eAnimModel]->SetNextIndex(m_eNextState);
			m_eHitType = TYPE_HIT;
			m_bVoidAttackCol = false;
			m_bLHandTrail = true;
			m_bRHandTrail = true;
			if (m_ePlayerMode != PMODE_BURST)
			{
				((CPointTrailObj*)m_PointTrails[3])->Set_Color(_float4{ 1.f,1.f,1.f,1.f });
				((CPointTrailObj*)m_PointTrails[3])->Set_Color2(_float4{ 0.f,1.f,0.9f,1.f });
				((CPointTrailObj*)m_PointTrails[3])->Set_BlurPower(1.f);
				((CPointTrailObj*)m_PointTrails[3])->Set_Ratio(_float2{ 1.f,1.f });
				((CPointTrailObj*)m_PointTrails[3])->Set_Size(0.1f);
				((CPointTrailObj*)m_PointTrails[3])->Set_MaxIndex(5);
				((CPointTrailObj*)m_PointTrails[4])->Set_Color(_float4{ 1.f,1.f,1.f,1.f });
				((CPointTrailObj*)m_PointTrails[4])->Set_Color2(_float4{ 0.f,1.f,0.9f,1.f });
				((CPointTrailObj*)m_PointTrails[4])->Set_BlurPower(1.f);
				((CPointTrailObj*)m_PointTrails[4])->Set_Ratio(_float2{ 1.f,1.f });
				((CPointTrailObj*)m_PointTrails[4])->Set_Size(0.1f);
				((CPointTrailObj*)m_PointTrails[4])->Set_MaxIndex(5);
			}
			else
			{
				((CPointTrailObj*)m_PointTrails[3])->Set_Color(_float4{ 1.f,1.f,0.5f,1.f });
				((CPointTrailObj*)m_PointTrails[3])->Set_Color2(_float4{ 1.f,0.5f,0.f,1.f });
				((CPointTrailObj*)m_PointTrails[3])->Set_BlurPower(1.f);
				((CPointTrailObj*)m_PointTrails[3])->Set_Ratio(_float2{ 1.f,1.f });
				((CPointTrailObj*)m_PointTrails[3])->Set_Size(0.1f);
				((CPointTrailObj*)m_PointTrails[3])->Set_MaxIndex(5);
				((CPointTrailObj*)m_PointTrails[4])->Set_Color(_float4{ 1.f,1.f,0.5f,1.f });
				((CPointTrailObj*)m_PointTrails[4])->Set_Color2(_float4{ 1.f,0.5f,0.f,1.f });
				((CPointTrailObj*)m_PointTrails[4])->Set_BlurPower(1.f);
				((CPointTrailObj*)m_PointTrails[4])->Set_Ratio(_float2{ 1.f,1.f });
				((CPointTrailObj*)m_PointTrails[4])->Set_Size(0.1f);
				((CPointTrailObj*)m_PointTrails[4])->Set_MaxIndex(5);
			}
			return;
		}
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() >= m_pModelCom[m_eAnimModel]->Get_TimeLimit(1)
		&& m_pModelCom[m_eAnimModel]->Get_KeyFrame() <= m_pModelCom[m_eAnimModel]->Get_TimeLimit(2))
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

void CPlayer01::VoidAttack_Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	m_bVoidOn = false;
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(0) && !m_bVoidAttackCol)
	{
		_uint iRandomVoice = GI->Get_Random(0, 1);
		switch (iRandomVoice)
		{
		case 0:
			GI->PlaySoundW(L"Hero01_ba_09.wav", SD_PLAYER1_VO, 1.f);
			break;
		case 1:
			GI->PlaySoundW(L"Hero01_ba_10.wav", SD_PLAYER1_VO, 1.f);
			break;
		}

		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		m_bAttackCol = true;
		m_bVoidAttackCol = true;
		
	}
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(1))
	{
		m_bLHandTrail = false;
		m_bRHandTrail = false;
	}
}

void CPlayer01::RushKick_Update(_float fTimeDelta)
{
	m_pTransformCom->ResetGravity();
	m_bAttackCol = false;
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(0) && !m_bRushKickCol)
	{
		GI->PlaySoundW(L"Hero01_ba_06.wav", SD_PLAYER1_VO, 1.f);
		GI->PlaySoundW(L"se_HE01_Skill04_3.wav", SD_PLAYER1_SK, 0.8f);
		CRM->Get_Cam()->Shake(0.2f, 0.2f, false, 0.2f);
		CRM->Get_Cam()->Fov(-3.f, 0.17f, 0.f);
		m_bAttackCol = true;
		m_bRushKickCol = true;
		Create_Effect(ET_RUSHKICK);
		m_eShaderPass = SHADER_LIGHT;
		m_bPlayerLight = true;
		m_fPlayerLightLimit = 0.1f;
	}
}

void CPlayer01::JumpLoof_Update(_float fTimeDelta)
{
	if (!m_pTransformCom->Get_Air())
	{
		m_pModelCom[m_eAnimModel]->Set_AnimIndex(JUMPEND);
		m_eCurState = JUMPEND;
		Create_Effect(ET_LANDING);
	}

	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), m_fJumpSpeed, fTimeDelta, m_pNavigationCom);
}

void CPlayer01::JumpStart_Update(_float fTimeDelta)
{
	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook), m_fJumpSpeed, fTimeDelta, m_pNavigationCom);
}

void CPlayer01::JumpEnd_Update(_float fTimeDelta)
{
}

void CPlayer01::Run_Update(_float fTimeDelta)
{
	m_pTransformCom->Go_Dir(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_fRunSpeed, fTimeDelta, m_pNavigationCom);

	m_fRunSoundAcc += 1.f * fTimeDelta;

	if (!m_bOtherFoot)
	{
		if (m_fRunSoundAcc >= 0.3f)
		{
			GI->PlaySoundW(L"se_flat_run_s_asphalt_03.wav", SD_PLAYER1_SK, 0.7f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = true;
		}
	}

	else
	{
		if (m_fRunSoundAcc >= 0.3f)
		{
			GI->PlaySoundW(L"se_flat_run_s_asphalt_04.wav", SD_PLAYER1_SK, 0.7f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = false;
		}
	}
}

void CPlayer01::RunFast_Update(_float fTimeDelta)
{
	m_pTransformCom->Go_Dir(m_pTransformCom->Get_State(CTransform::STATE_LOOK), m_fRunSpeed, fTimeDelta, m_pNavigationCom);
		
	m_fRunSoundAcc += 1.f * fTimeDelta;

	if (!m_bOtherFoot)
	{
		if (m_fRunSoundAcc >= 0.23f)
		{
			GI->PlaySoundW(L"se_flat_run_s_asphalt_03.wav", SD_PLAYER1_SK, 0.7f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = true;
		}
	}

	else
	{
		if (m_fRunSoundAcc >= 0.23f)
		{
			GI->PlaySoundW(L"se_flat_run_s_asphalt_04.wav", SD_PLAYER1_SK, 0.7f);
			m_fRunSoundAcc = 0.f;
			m_bOtherFoot = false;
		}
	}
}

void CPlayer01::Skill1_Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(0) && !m_bSkill1Col)
	{
		_uint iRandomVoice = GI->Get_Random(0, 1);
		switch (iRandomVoice)
		{
		case 0:
			GI->PlaySoundW(L"Hero01_ba_32.wav", SD_PLAYER1_VO, 1.f);
			break;
		case 1:
			GI->PlaySoundW(L"Hero01_ba_33.wav", SD_PLAYER1_VO, 1.f);
			break;
		}
		GI->PlaySoundW(L"se_HE01_Skill05_4.wav", SD_PLAYER1_SK, 0.8f);
		GI->PlaySoundW(L"se_HE01_Skill02_3.wav", SD_PLAYER1_SK, 0.8f);
		CRM->Get_Cam()->Shake(0.2f, 0.2f, false, 0.2f);
		CRM->Get_Cam()->Fov(-4.f, 0.15f,0.f);
		m_bAttackCol = true;
		m_bSkill1Col = true;
		m_bRHandTrail = false;
		Create_Effect(ET_SKILL1);
		m_eShaderPass = SHADER_LIGHT;
		m_bPlayerLight = true;
		m_fPlayerLightLimit = 0.1f;
		if(m_ePlayerMode != PMODE_BURST)
			PTM->Create_InstanceParticle(L"Player01Skill1End", m_pTransformCom->Get_WorldFloat4x4());
		else
			PTM->Create_InstanceParticle(L"Player01Skill1End_2", m_pTransformCom->Get_WorldFloat4x4());
	}
}

void CPlayer01::Skill2_Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(0) && !m_bSkill2Col_1)
	{
		
		m_bAttackCol = true;
		m_bSkill2Col_1 = true;
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(1) && !m_bSkill2Col_2)
	{
		m_bAttackCol = true;
		m_bSkill2Col_2 = true;
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(2) && !m_bSkill2Col_3)
	{
		m_bAttackCol = true;
		m_bSkill2Col_3 = true;
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(3) && !m_bSkill2Col_4)
	{
		GI->PlaySoundW(L"se_HE01_Skill02_4.wav", SD_PLAYER1_SK, 0.8f);
		CRM->Get_Cam()->Shake(0.3f, 0.2f, false, 0.2f);
		m_eHitType = TYPE_UP;
		m_fAirPower = 6.f;
		m_bAttackCol = true;
		m_bSkill2Col_4 = true;
		Create_Particle(PT_SUNDER);
		Create_Effect(ET_SUNDER);	
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(5) && !m_bSkill2Sound)
	{
		m_bSkill2Sound = true;
		GI->PlaySoundW(L"Hero01_ba_35.wav", SD_PLAYER1_VO, 0.9f);
		GI->PlaySoundW(L"se_HE01_Skill02_1.wav", SD_PLAYER1_SK, 0.8f);
	}
}

void CPlayer01::Skill3_Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(0) && !m_bSkill3Col_1)
	{
		GI->PlaySoundW(L"Hero01_ba_36.wav", SD_PLAYER1_VO, 0.9f);
		GI->PlaySoundW(L"se_HE01_Nattack_3_01.wav", SD_PLAYER1_SK, 0.8f);
		m_bAttackCol = true;
		m_bSkill3Col_1 = true;
		
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(1) && !m_bSkill3Col_2)
	{
		GI->PlaySoundW(L"se_HE01_Hattack_1.wav", SD_PLAYER1_SK, 0.9f);
		m_eHitType = TYPE_FASTDOWN;
		m_fAirPower = -30.f;
		m_bAttackCol = true;
		m_bSkill3Col_2 = true;
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(3) && !m_bSkill3Effect)
	{
		m_bSkill3Effect = true;
		Create_Effect(ET_SKILL3_1);
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(4) && !m_bSkill3Effect2)
	{
		m_bSkill3Effect2 = true;
		Create_Effect(ET_SKILL3_2);

	}
}

void CPlayer01::Skill4Start_Update(_float fTimeDelta)
{
	
}

void CPlayer01::Skill4Loof_Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(0) && !m_bSkill4LoofCol_1)
	{
		GI->PlaySoundW(L"se_HE01_Skill03_1.wav", SD_PLAYER1_SK, 0.8f);
		GI->PlaySoundW(L"Hero01_ba_02.wav", SD_PLAYER1_VO, 1.f);
		CRM->Get_Cam()->Shake(0.05f, 0.15f);
		m_bAttackCol = true;
		m_bSkill4LoofCol_1 = true;
		Create_Effect(ET_SKILL4RIGHT);
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(1) && !m_bSkill4LoofCol_2)
	{
		GI->PlaySoundW(L"se_HE01_Skill03_2.wav", SD_PLAYER1_SK, 0.8f);
		GI->PlaySoundW(L"Hero01_ba_02.wav", SD_PLAYER1_VO, 1.f);
		CRM->Get_Cam()->Shake(0.05f, 0.15f);
		m_bAttackCol = true;
		m_bSkill4LoofCol_2 = true;
		Create_Effect(ET_SKILL4LEFT);
	}
}

void CPlayer01::Skill4End_Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(0) && !m_bSkill4EndCol)
	{
		GI->PlaySoundW(L"se_HE01_Skill04_4.wav", SD_PLAYER1_SK, 0.8f);
		GI->PlaySoundW(L"Hero01_ba_08.wav", SD_PLAYER1_VO, 1.f);
		CRM->Get_Cam()->Shake(0.2f, 0.2f, false, 0.2f);
		CRM->Get_Cam()->Fov(-4.f, 0.15f, 0.f);
		m_bAttackCol = true;
		m_bSkill4EndCol = true;
		Create_Effect(ET_SKILL4END);
		m_eShaderPass = SHADER_LIGHT;
		m_bPlayerLight = true;
		m_fPlayerLightLimit = 0.1f;

	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(1) && !m_bSkill4EndSlowStart)
	{
		m_bSkill4EndSlowStart = true;
		TIM->Fix_All(0.2f, 100.f);
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(2) && !m_bSkill4EndSlowEnd)
	{
		m_bSkill4EndSlowEnd = true;
		TIM->Reset_All();
	}

}

void CPlayer01::Skill5_Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(0) && !m_bSkill5Col)
	{
		_uint iRandomVoice = GI->Get_Random(0, 1);
		switch (iRandomVoice)
		{
		case 0:
			GI->PlaySoundW(L"Hero01_ba_40.wav", SD_PLAYER1_VO, 1.f);
			break;
		case 1:
			GI->PlaySoundW(L"Hero01_ba_41.wav", SD_PLAYER1_VO, 1.f);
			break;
		}
		GI->PlaySoundW(L"se_HE01_Skill01_5.wav", SD_PLAYER1_SK, 0.8f);

		CRM->Get_Cam()->Shake(0.2f, 0.2f, false, 0.2f);
		m_bAttackCol = true;
		m_bSkill5Col = true;
		Create_Effect(ET_SKILL5END);
		m_eShaderPass = SHADER_LIGHT;
		m_bPlayerLight = true;
		m_fPlayerLightLimit = 0.1f;
		
		m_bLHandTrail = false;
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(2) && !m_bSkill5Effect)
	{
		m_bSkill5Effect = true;
		CCircleDistortion::CIRCLEDISTORTIONINFO* Info = new CCircleDistortion::CIRCLEDISTORTIONINFO;
		Info->fBlurPower = 1.f;
		Info->fLimitTime = 2.f;
		Info->vLocalPos = { 0.f,1.f,1.f,1.f };
		Info->vRatio = { 0.15f,1.f };
		Info->vRatioSpeed = { 0.05f,0.f };
		Info->vScale = { 20.f,20.f,20.f };
		Info->vScaleSpeed = { -200.f,-200.f,-200.f };
		Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"CircleDistortion", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(3) && !m_bSkill5Effect2)
	{
		m_bSkill5Effect2 = true;
		CCircleDistortion::CIRCLEDISTORTIONINFO* Info = new CCircleDistortion::CIRCLEDISTORTIONINFO;
		Info->fBlurPower = 1.f;
		Info->fLimitTime = 2.f;
		Info->vLocalPos = { 0.f,1.f,1.f,1.f };
		Info->vRatio = { 0.15f,1.f };
		Info->vRatioSpeed = { 0.05f,0.f };
		Info->vScale = { 20.f,20.f,20.f };
		Info->vScaleSpeed = { -200.f,-200.f,-200.f };
		Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"CircleDistortion", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
	}




	
}

void CPlayer01::Skill6_Update(_float fTimeDelta)
{
	//m_pTransformCom->ResetGravity();
	m_bAttackCol = false;
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(1) && !m_bSkill6Col)
	{
		GI->PlaySoundW(L"se_HE01_Skill06_3.wav", SD_PLAYER1_SK, 0.8f);
		GI->PlaySoundW(L"Hero01_ba_42.wav", SD_PLAYER1_VO, 1.f);
		CRM->Get_Cam()->Shake(0.2f, 0.2f, false, 0.2f);
		CRM->Get_Cam()->Fov(-3.f, 0.1f, 0.f);
		m_bAttackCol = true;
		m_bSkill6Col = true;
		m_pTransformCom->Jump(7.f);
		Create_Effect(ET_SKILL6START);		
		m_eShaderPass = SHADER_LIGHT;
		m_bPlayerLight = true;
		m_fPlayerLightLimit = 0.1f;

	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(0))
	{
		m_bLHandTrail = false;
	}
	
	
}

void CPlayer01::Skill6Start_Update(_float fTimeDelta)
{
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() <= m_pModelCom[m_eAnimModel]->Get_TimeLimit(0))
		m_pTransformCom->ResetGravity();
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(0) + 1 && !m_bSkill6JumpEnd)
	{
		m_bSkill6JumpEnd = true;
		m_pTransformCom->ResetAndJump(-50.f);
		m_eShaderPass = SHADER_LIGHT;
		m_bPlayerLight = true;
		m_fPlayerLightLimit = 0.1f;
	}

	if (!m_pTransformCom->Get_Air() && !m_bSkill6StartEffect)
	{
		m_bSkill6StartEffect = true;
		Create_Particle(PT_SUNDER);
		Create_Effect(ET_SUNDER);

	}

	m_bAttackCol = false;
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(0) && !m_bSkill6StartCol)
	{
		GI->PlaySoundW(L"se_HE01_Skill02_4.wav", SD_PLAYER1_SK, 0.8f);
		CRM->Get_Cam()->Shake(0.3f, 0.2f, false, 0.2f);
		CRM->Get_Cam()->Fov(3.f, 0.2f, 0.f);
		m_bAttackCol = true;
		m_bSkill6StartCol = true;
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(1) && !m_bSkill6StartSlowStart)
	{
		m_bSkill6StartSlowStart = true;
		TIM->Fix_All(0.2f, 100.f);
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(2) && !m_bSkill6StartSlowEnd)
	{
		m_bSkill6StartSlowEnd = true;
		TIM->Reset_All();
	}
	
}

void CPlayer01::Skill6Loof_Update(_float fTimeDelta)
{
	m_pModelCom[m_eAnimModel]->Set_AnimIndex(SKILL6_END);
	m_eCurState = SKILL6_END;
	m_eNextState = m_eCurState;
	m_pModelCom[m_eAnimModel]->SetNextIndex(m_eNextState);
}

void CPlayer01::Skill6End_Update(_float fTimeDelta)
{
}

void CPlayer01::Skill7_Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(0) && !m_bSkill7Col_1)
	{
		m_bVoid = true;
		GI->PlaySoundW(L"se_HE01_Skill07_1.wav", SD_PLAYER1_SK, 0.7f);
		m_bAttackCol = true;
		m_bSkill7Col_1 = true;
		Create_Effect(ET_SKILL7);
		m_bLKickTrail = true;
		m_bRKickTrail = false;
		if (m_ePlayerMode != PMODE_BURST)
		{
			((CPointTrailObj*)m_PointTrails[0])->Set_Color(_float4{ 1.f,1.f,1.f,1.f });
			((CPointTrailObj*)m_PointTrails[0])->Set_Color2(_float4{ 0.f,1.f,0.9f,1.f });
			((CPointTrailObj*)m_PointTrails[0])->Set_BlurPower(1.f);
			((CPointTrailObj*)m_PointTrails[0])->Set_Ratio(_float2{ 1.f,1.f });
			((CPointTrailObj*)m_PointTrails[0])->Set_Size(0.04f);
			((CPointTrailObj*)m_PointTrails[0])->Set_MaxIndex(20);
			((CPointTrailObj*)m_PointTrails[0])->Set_TimeLimit(0.2f);
		}
		else
		{
			((CPointTrailObj*)m_PointTrails[0])->Set_Color(_float4{ 1.f,1.f,0.8f,1.f });
			((CPointTrailObj*)m_PointTrails[0])->Set_Color2(_float4{ 1.f,0.5f,0.f,1.f });
			((CPointTrailObj*)m_PointTrails[0])->Set_BlurPower(1.f);
			((CPointTrailObj*)m_PointTrails[0])->Set_Ratio(_float2{ 1.f,1.f });
			((CPointTrailObj*)m_PointTrails[0])->Set_Size(0.04f);
			((CPointTrailObj*)m_PointTrails[0])->Set_MaxIndex(20);
			((CPointTrailObj*)m_PointTrails[0])->Set_TimeLimit(0.2f);
		}
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(1) && !m_bSkill7Col_2)
	{
		GI->PlaySoundW(L"Hero01_ba_45.wav", SD_PLAYER1_VO, 1.f);
		m_bAttackCol = true;
		m_bSkill7Col_2 = true;
		Create_Effect(ET_SKILL7);

	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(2) && !m_bSkill7Col_3)
	{
		GI->PlaySoundW(L"se_HE01_Skill07_2.wav", SD_PLAYER1_SK, 0.6f);
		m_bAttackCol = true;
		m_bSkill7Col_3 = true;
		m_eHitType = TYPE_SPIN;
		m_fPushPower = 0.f;
		m_fAirPower = 2.f;
		Change_ColRange(_float3{ 2.f,2.f,2.f }, _float3{ 0.f,2.f,0.f });
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER01) * 0.6f;
		((CPointTrailObj*)m_PointTrails[0])->Set_TimeLimit(0.01f);
		Create_Effect(ET_SKILL7_2);

	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(3) && !m_bSkill7Col_4)
	{
		m_bAttackCol = true;
		m_bSkill7Col_4 = true;
		Create_Effect(ET_SKILL7_2);
		
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(4) && !m_bSkill7Col_5)
	{
		m_bAttackCol = true;
		m_bSkill7Col_5 = true;
		Create_Effect(ET_SKILL7_2);

	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(5) && !m_bSkill7Col_6)
	{
		GI->PlaySoundW(L"se_HE01_Skill06_4.wav", SD_PLAYER1_SK, 0.6f);
		CRM->Get_Cam()->Shake(0.2f, 0.2f, false, 0.2f);
		CRM->Get_Cam()->Fov(3.f, 0.2f, 0.f);
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER01) * 1.2f;
		m_bAttackCol = true;
		m_bSkill7Col_6 = true;
		m_eHitType = TYPE_UP;
		m_fAirPower = 10.f;
		m_bVoid = false;
		Create_Effect(ET_SKILL7_3);
	}
}

void CPlayer01::Skill8_Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(0) && !m_bSkill8Col)
	{
		GI->PlaySoundW(L"Hero01_ba_34.wav", SD_PLAYER1_VO, 1.f);
		GI->PlaySoundW(L"se_HE01_Special05_1.wav", SD_PLAYER1_SK, 0.8f);
		CRM->Get_Cam()->Shake(0.2f, 0.2f, false, 0.2f);
		CRM->Get_Cam()->Fov(3.f, 0.2f, 0.f);
		m_bAttackCol = true;
		m_bSkill8Col = true;
		Create_Pillar();
		m_eShaderPass = SHADER_LIGHT;
		m_bPlayerLight = true;
		m_fPlayerLightLimit = 0.1f;
		
	}
}

void CPlayer01::Skill9_Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(0) && !m_bSkill9Col)
	{
		GI->PlaySoundW(L"Hero01_ba_57.wav", SD_PLAYER1_VO, 1.f);
		GI->PlaySoundW(L"se_HE01_Skill02_3.wav", SD_PLAYER1_SK, 0.8f);
		CRM->Get_Cam()->Shake(0.2f, 0.2f, false, 0.2f);
		m_bAttackCol = true;
		m_bSkill9Col = true;
		Create_Effect(ET_SKILL9);
		m_eShaderPass = SHADER_LIGHT;
		m_bPlayerLight = true;
		m_fPlayerLightLimit = 0.1f;
	}
}

void CPlayer01::Skill10_Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(0) && !m_bSkill10Col)
	{
		GI->PlaySoundW(L"Hero01_ba_24.wav", SD_PLAYER1_VO, 1.f);
		GI->PlaySoundW(L"se_HE01_Skill02_2.wav", SD_PLAYER1_SK, 0.8f);
		m_bAttackCol = true;
		m_bSkill10Col = true;
		
	}
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(1))
	{
		m_pModelCom[m_eAnimModel]->Set_AnimIndex(SKILL11);
		m_eCurState = SKILL11;
		m_bSkill11Col = false;
		m_eHitType = TYPE_AWAY;
		m_fPushPower = 10.f;
	}
}

void CPlayer01::Skill11_Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(0) && !m_bSkill11Col)
	{
		GI->PlaySoundW(L"se_HE01_Skill02_3.wav", SD_PLAYER1_SK, 0.8f);
		m_bAttackCol = true;
		m_bSkill11Col = true;
	}
}

void CPlayer01::RageSkill1_Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	m_bHurt = false;



	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == 3 && !m_bHeroTest)
	{
		m_bHeroTest = true;

		CRM->Get_Cam()->Save_World();
		CRM->Play_CutScene(10, true, false, m_pTransformCom);
	}
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == 49 && !m_bHeroTest11)
	{
		m_bHeroTest11 = true;

		//CRM->Get_Cam()->Fov(1.f, 1.f, 0.1f);
	}
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == 53 && !m_bHeroTest1)
	{
		m_bHeroTest1 = true;

		CRM->Get_Cam()->Load_World();
		CRM->Get_Cam()->End_Target();
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() >= m_pModelCom[m_eAnimModel]->Get_TimeLimit(8)
		&& m_pModelCom[m_eAnimModel]->Get_KeyFrame() <= m_pModelCom[m_eAnimModel]->Get_TimeLimit(0))
	{
		m_fRageCutSceneAcc += 1.f;
		if (m_fRageCutSceneAcc >= 0.3f)
		{
			Create_RageCutScene();
			m_fRageCutSceneAcc = 0.f;
		}
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(0) && !m_bRageSkill1Col_1)
	{
		GI->PlaySoundW(L"se_HE01_Super01_1.wav", SD_PLAYER1_SK, 1.f);
		GI->PlaySoundW(L"Hero01_ba_62.wav", SD_PLAYER1_VO, 1.f);
		CRM->Get_Cam()->Shake(0.15f, 1.f, false, 0.15f);
		CRM->Get_Cam()->Fov(-0.3f, 1.f, 0.f);
		m_bAttackCol = true;
		m_bRageSkill1Col_1 = true;
		Create_Effect(ET_RAGESKILL1);
	
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(1) && !m_bRageSkill1Col_2)
	{
		m_bAttackCol = true;
		m_bRageSkill1Col_2 = true;
		Create_Effect(ET_RAGESKILL1);
		

	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(2) && !m_bRageSkill1Col_3)
	{
		m_bAttackCol = true;
		m_bRageSkill1Col_3 = true;
		Create_Effect(ET_RAGESKILL1);
		


	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(3) && !m_bRageSkill1Col_4)
	{
		m_bAttackCol = true;
		m_bRageSkill1Col_4 = true;
		Create_Effect(ET_RAGESKILL1);
		
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(4) && !m_bRageSkill1Col_5)
	{
		m_bAttackCol = true;
		m_bRageSkill1Col_5 = true;
		Create_Effect(ET_RAGESKILL1);
		
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(5) && !m_bRageSkill1Col_6)
	{
		m_bAttackCol = true;
		m_bRageSkill1Col_6 = true;
		Create_Effect(ET_RAGESKILL1);
		
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(6) && !m_bRageSkill1Col_7)
	{
		GI->PlaySoundW(L"se_HE01_Super01_3.wav", SD_PLAYER1_SK, 1.f);
		CRM->Get_Cam()->Shake(0.6f, 0.2f, false, 0.2f);
		CRM->Get_Cam()->Fov(2.f, 0.3f, 0.f);
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER01) * 2.f;
		m_eHitType = TYPE_SPIN;
		m_fAirPower = 7.f;
		m_fPushPower = 7.f;
		m_bAttackCol = true;
		m_bRageSkill1Col_7 = true;
		Create_Effect(ET_RAGESKILL1END);
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(7) && !m_bRageSkill1Effect)
	{
		m_bRageSkill1Effect = true;
		if(m_ePlayerMode != PMODE_BURST)
			PTM->Create_InstanceParticle(L"Player01RageSkill1_3", m_pTransformCom->Get_WorldFloat4x4());
		else
			PTM->Create_InstanceParticle(L"Player01RageSkill1_3B", m_pTransformCom->Get_WorldFloat4x4());
	}

}

void CPlayer01::RageSkill2_Update(_float fTimeDelta)
{
	m_bAttackCol = false;
	m_bHurt = false;



	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == 0 && !m_bHeroTest2)
	{
		m_bHeroTest2 = true;

		CRM->Get_Cam()->Save_World();
		// CRM->Get_Cam()->Shake(0.3f, 0.4f);
		CRM->Play_CutScene(11, true, false, m_pTransformCom);
	} 
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == 20 && !m_bHeroTest10)
	{
		m_bHeroTest10 = true;

		CRM->Get_Cam()->Shake(0.02f, 1.f);
		CRM->Get_Cam()->Fov(-0.5f, 1.f, 0.1f);
	}
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == 67 && !m_bHeroTest3)
	{
		m_bHeroTest3 = true;

		 CRM->Get_Cam()->Load_World();
		CRM->Get_Cam()->End_Target();
	}
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == 112 && !m_bHeroTest4)
	{ // 114
		m_bHeroTest4 = true;

		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(0.f, 0.f, 0.f);
		CamDesc.vLook = _float3(0.f, 0.f, 0.f);
		CamDesc.bLerp = false;
		CamDesc.bLookLerp = false;
		CamDesc.fLerpSpeed = 0.f;
		CamDesc.fLookLerpSpeed = 0.f;
		CamDesc.vPivotLookPos = _float3(0.f, 1.f, 0.f);
		CamDesc.pTran = m_pTransformCom;
		CamDesc.vPosDis = _float3(-0.8f, 1.f, 8.f);
		CamDesc.bPixLocal = true;
		CRM->Get_Cam()->Show_Target(CamDesc);
		//CRM->Get_Cam()->End_Target();

		// 176   154   123

		// CRM->Get_Cam()->End_Target();
	}
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == 114 && !m_bHeroTest5)
	{
		m_bHeroTest5 = true;

		// 176   154   123

		CCamera_Main::CUTDESC CamDesc;
		CamDesc.vPos = _float3(0.f, 0.f, 0.f);
		CamDesc.vLook = _float3(0.f, 0.f, 0.f);
		CamDesc.bLerp = true;
		CamDesc.bLookLerp = true;
		CamDesc.fLerpSpeed = 15.f;
		CamDesc.fLookLerpSpeed = 15.f;
		CamDesc.vPivotLookPos = _float3(0.f, 1.f, 0.f);
		CamDesc.pTran = m_pTransformCom;
		CamDesc.vPosDis = _float3(-0.8f, 1.f, -2.f);
		CamDesc.bPixLocal = true;
		CRM->Get_Cam()->Show_Target(CamDesc);

	}
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == 176 && !m_bHeroTest6)
	{
		m_bHeroTest6 = true;

		// 176   154   123

		CRM->Get_Cam()->Load_World();
		CRM->Get_Cam()->End_Target();
	}



	//14 = 차징 끝 15 = 돌진시작
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() < m_pModelCom[m_eAnimModel]->Get_TimeLimit(14))
	{
		m_fRageSkill1ParticleAcc += 1.f * fTimeDelta;
		if (m_fRageSkill1ParticleAcc >= 0.2f)
		{
			PTM->Create_InstanceParticle(L"Player01RageSkill2", m_pTransformCom->Get_WorldFloat4x4());
			PTM->Create_InstanceParticle(L"Player01RageSkill2_2", m_pTransformCom->Get_WorldFloat4x4());
			Create_Effect(ET_RAGESKILL2_2);
			m_fRageSkill1ParticleAcc = 0.f;
			
		}
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(15) && !m_bRageSkill2Effect)
	{
		m_bRageSkill2Effect = true;
		m_bBodyTrail = true;
		Create_Effect(ET_RAGESKILL2);
		Set_Rage2MotionTrail();
		m_fMotionTrailLimit = 0.2f;
		for (auto& iter : m_MotionTrails)
			((CPlayer01MotionTrail*)iter)->Set_HairRender(false);
		Create_Effect(ET_RAGESKILL2_3);
		m_bBurstEffect = false;
		//PTM->Create_InstanceParticle(L"Player01RageSkill2Hit", m_pTransformCom->Get_WorldFloat4x4());
	}



	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(0) && !m_bRageSkill2Col_1)
	{
		GI->PlaySoundW(L"Hero01_ba_64.wav", SD_PLAYER1_VO, 1.f);
		m_bAttackCol = true;
		m_bRageSkill2Col_1 = true;
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(1) && !m_bRageSkill2Col_2)
	{
		m_bAttackCol = true;
		m_bRageSkill2Col_2 = true;
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		//PTM->Create_InstanceParticle(L"Player01RageSkill2Hit_2", m_pTransformCom->Get_WorldFloat4x4());
		Create_Effect(ET_RAGESKILL2_3);
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(2) && !m_bRageSkill2Col_3)
	{
		m_bAttackCol = true;
		m_bRageSkill2Col_3 = true;
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		//PTM->Create_InstanceParticle(L"Player01RageSkill2Hit_3", m_pTransformCom->Get_WorldFloat4x4());
		Create_Effect(ET_RAGESKILL2_3);
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(3) && !m_bRageSkill2Col_4)
	{
		m_bAttackCol = true;
		m_bRageSkill2Col_4 = true;
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		//PTM->Create_InstanceParticle(L"Player01RageSkill2Hit_4", m_pTransformCom->Get_WorldFloat4x4());
		Create_Effect(ET_RAGESKILL2_3);
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(4) && !m_bRageSkill2Col_5)
	{
		m_bAttackCol = true;
		m_bRageSkill2Col_5 = true;
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		//PTM->Create_InstanceParticle(L"Player01RageSkill2Hit_5", m_pTransformCom->Get_WorldFloat4x4());
		Create_Effect(ET_RAGESKILL2_3);
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(5) && !m_bRageSkill2Col_6)
	{
		m_bAttackCol = true;
		m_bRageSkill2Col_6 = true;
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		//PTM->Create_InstanceParticle(L"Player01RageSkill2Hit_6", m_pTransformCom->Get_WorldFloat4x4());
		Create_Effect(ET_RAGESKILL2_3);
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(6) && !m_bRageSkill2Col_7)
	{
		m_bAttackCol = true;
		m_bRageSkill2Col_7 = true;
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		//PTM->Create_InstanceParticle(L"Player01RageSkill2Hit_7", m_pTransformCom->Get_WorldFloat4x4());
		Create_Effect(ET_RAGESKILL2_3);
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(7) && !m_bRageSkill2Col_8)
	{
		m_bAttackCol = true;
		m_bRageSkill2Col_8 = true;
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		//PTM->Create_InstanceParticle(L"Player01RageSkill2Hit_8", m_pTransformCom->Get_WorldFloat4x4());
		Create_Effect(ET_RAGESKILL2_3);
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(8) && !m_bRageSkill2Col_9)
	{
		m_bAttackCol = true;
		m_bRageSkill2Col_9 = true;
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		//PTM->Create_InstanceParticle(L"Player01RageSkill2Hit_9", m_pTransformCom->Get_WorldFloat4x4());
		Create_Effect(ET_RAGESKILL2_3);
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(9) && !m_bRageSkill2Col_10)
	{
		m_bAttackCol = true;
		m_bRageSkill2Col_10 = true;
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		//PTM->Create_InstanceParticle(L"Player01RageSkill2Hit_10", m_pTransformCom->Get_WorldFloat4x4());
		Create_Effect(ET_RAGESKILL2_3);
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(10) && !m_bRageSkill2Col_11)
	{
		m_bAttackCol = true;
		m_bRageSkill2Col_11 = true;
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		//PTM->Create_InstanceParticle(L"Player01RageSkill2Hit_11", m_pTransformCom->Get_WorldFloat4x4());
		Create_Effect(ET_RAGESKILL2_3);
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(11) && !m_bRageSkill2Col_12)
	{
		m_bAttackCol = true;
		m_bRageSkill2Col_12 = true;
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		//PTM->Create_InstanceParticle(L"Player01RageSkill2Hit_4", m_pTransformCom->Get_WorldFloat4x4());
		Create_Effect(ET_RAGESKILL2_3);
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(12) && !m_bRageSkill2Col_13)
	{
		GI->PlaySoundW(L"Hero01_ba_65.wav", SD_PLAYER1_VO, 1.f);
		m_bAttackCol = true;
		m_bRageSkill2Col_13 = true;
		m_bBodyTrail = false;
		Reset_SlowMotionTrail();
		m_fMotionTrailLimit = 0.04f;
		CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		//PTM->Create_InstanceParticle(L"Player01RageSkill2Hit_12", m_pTransformCom->Get_WorldFloat4x4());
		Create_Effect(ET_RAGESKILL2_3);
		m_bBurstEffect = true;
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(13) && !m_bRageSkill2Col_14)
	{		
		GI->PlaySoundW(L"se_HE01_Super02_3.wav", SD_PLAYER1_SK, 0.6f);
		CRM->Get_Cam()->Shake(0.4f, 0.4f, false, 0.1f);
		m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER01) * 10.f;
		m_bAttackCol = true;
		m_bRageSkill2Col_14 = true;
		m_eHitType = TYPE_SPIN;
		m_fAirPower = 12.f;
		Create_Effect(ET_RAGESKILL2END);
		for (auto& iter : m_MotionTrails)
			((CPlayer01MotionTrail*)iter)->Set_HairRender(true);
		m_fNowBP = 0.f;
	}

	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(17))
	{
		TIM->Fix_All(0.5f, 0.05f);
		
	}
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(18))
	{
		TIM->Fix_All(0.5f, 0.05f);
		
	}
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(19))
	{
		TIM->Fix_All(0.5f, 0.05f);
	
	}
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(20))
	{
		TIM->Fix_All(0.5f, 0.05f);
	
	}
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(21))
	{
		TIM->Fix_All(0.5f, 0.05f);

	}
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(22))
	{
		TIM->Fix_All(0.5f, 0.05f);

	}
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(23))
	{
		TIM->Fix_All(0.5f, 0.05f);

	}
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(24))
	{
		TIM->Fix_All(0.5f, 0.05f);

	}
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(25))
	{
		TIM->Fix_All(0.5f, 0.05f);

	}
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(26))
	{
		TIM->Fix_All(0.5f, 0.05f);

	}
	if (m_pModelCom[m_eAnimModel]->Get_KeyFrame() == m_pModelCom[m_eAnimModel]->Get_TimeLimit(27))
	{
		TIM->Fix_All(0.5f, 0.05f);

	}
	
}

void CPlayer01::FallDown_Update(_float fTimeDelta)
{
	if (!m_pTransformCom->Get_Air())
	{
		m_pModelCom[m_eAnimModel]->Set_AnimIndex(BOUNCE);
		m_eCurState = BOUNCE;
		m_eNextState = m_eCurState;
		m_pModelCom[m_eAnimModel]->SetNextIndex(m_eNextState);
		m_pTransformCom->ResetAndJump(1.f);
	}

	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vPushLook), m_fPushPower, fTimeDelta, m_pNavigationCom);
}

void CPlayer01::HitAway_Update(_float fTimeDelta)
{
	if (!m_pTransformCom->Get_Air())
	{
		Set_State(BOUNCE);
	}

	if (m_fPushPower > 5.f)
		m_fPushPower -= 5.f * fTimeDelta;

	if (m_fPushPower <= 5.f)
		m_fPushPower = 5.f;

	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vPushLook), m_fPushPower, fTimeDelta, m_pNavigationCom);
}

void CPlayer01::HitSpin_Update(_float fTimeDelta)
{
	if (!m_pTransformCom->Get_Air())
	{
		Set_State(BOUNCE);
	}

	if (m_fPushPower >= 0.f)
		m_fPushPower -= 3.f * fTimeDelta;

	if (m_fPushPower <= 0.f)
		m_fPushPower = 0.f;

	m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vPushLook), m_fPushPower, fTimeDelta, m_pNavigationCom);
}

void CPlayer01::HitDown_Update(_float fTimeDelta)
{
	//if (!m_pTransformCom->Get_Air())
	//{
	//	Set_State(BOUNCE);
	//}

	//if (m_fPushPower >= 1.f)
	//	m_fPushPower -= 0.1f;

	//m_pTransformCom->Go_Dir(XMLoadFloat3(&m_vTargetLook) * -1.f, m_fPushPower, fTimeDelta);
}

void CPlayer01::HurtBack_Update(_float fTimeDelta)
{
	if (CK_PRESS(DIK_CAPSLOCK))
	{
		m_bVoid = true;
		m_fVoidSpeed = 10.f;
		m_pModelCom[m_eAnimModel]->Set_AnimIndex(VOIDFRONT);
		m_eCurState = VOIDFRONT;
		m_eNextState = m_eCurState;
		m_pModelCom[m_eAnimModel]->SetNextIndex(m_eNextState);
		Create_Effect(ET_DASH);
		return;
	}
}

void CPlayer01::HurtFront_Update(_float fTimeDelta)
{
	if (CK_PRESS(DIK_CAPSLOCK))
	{
		m_bVoid = true;
		m_fVoidSpeed = 10.f;
		m_pModelCom[m_eAnimModel]->Set_AnimIndex(VOIDFRONT);
		m_eCurState = VOIDFRONT;
		m_eNextState = m_eCurState;
		m_pModelCom[m_eAnimModel]->SetNextIndex(m_eNextState);
		Create_Effect(ET_DASH);
		return;
	}
}

void CPlayer01::Create_HE_1(_float4 vWorldPos)
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

void CPlayer01::Create_HE_2(_float4 vWorldPos)
{
	_float4 vPos;
	XMStoreFloat4(&vPos, XMLoadFloat4(&vWorldPos) +
		XMVector3Normalize((XMLoadFloat4(&GI->Get_CamPosition()) - XMLoadFloat4(&vWorldPos) * 1.f)));
	XMStoreFloat4(&vPos, XMLoadFloat4(&vPos) + _vector{ GI->Get_FloatRandom(-0.5f,0.5f),
		GI->Get_FloatRandom(-0.5f,0.5f) ,  GI->Get_FloatRandom(-0.5f,0.5f) });

	_matrix Matrix = XMMatrixIdentity();
	Matrix.r[3] = XMLoadFloat4(&vPos);
	_float4x4 Matrix2;
	XMStoreFloat4x4(&Matrix2, Matrix);
	PTM->Create_AlphaParticle(L"Player01Hit1", Matrix2);
	PTM->Create_InstanceParticle(L"Player01Hit1", Matrix2);


}

void CPlayer01::Create_RageCutScene()
{
	
	_matrix RootMatrix;

	if (m_ePlayerMode != PMODE_BURST)
	{
		RootMatrix = m_Sockets[SOCKET_WEAPON_L]->Get_CombinedTransformation() *  m_pModelCom[m_eAnimModel]->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();		
		RootMatrix.r[0] = { 1.f,0.f,0.f,0.f };
		RootMatrix.r[1] = { 0.f,1.f,0.f,0.f };
		RootMatrix.r[2] = { 0.f,0.f,1.f,0.f };
		_float4x4 Matrix;
		XMStoreFloat4x4(&Matrix, RootMatrix);
		PTM->Create_InstanceParticle(L"Player01RageCutScene", Matrix);
		PTM->Create_InstanceParticle(L"Player01RageCutScene_2", Matrix);

		RootMatrix = m_Sockets[SOCKET_WEAPON_R]->Get_CombinedTransformation() *  m_pModelCom[m_eAnimModel]->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
		RootMatrix.r[0] = { 1.f,0.f,0.f,0.f };
		RootMatrix.r[1] = { 0.f,1.f,0.f,0.f };
		RootMatrix.r[2] = { 0.f,0.f,1.f,0.f };
		XMStoreFloat4x4(&Matrix, RootMatrix);
		PTM->Create_InstanceParticle(L"Player01RageCutScene", Matrix);
		PTM->Create_InstanceParticle(L"Player01RageCutScene_2", Matrix);


	}
	else
	{
		RootMatrix = m_Sockets[SOCKET_WEAPON_L2]->Get_CombinedTransformation() *  m_pModelCom[m_eAnimModel]->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
		RootMatrix.r[0] = { 1.f,0.f,0.f,0.f };
		RootMatrix.r[1] = { 0.f,1.f,0.f,0.f };
		RootMatrix.r[2] = { 0.f,0.f,1.f,0.f };
		_float4x4 Matrix;
		XMStoreFloat4x4(&Matrix, RootMatrix);
		PTM->Create_InstanceParticle(L"Player01RageCutSceneB", Matrix);
		PTM->Create_InstanceParticle(L"Player01RageCutScene_2B", Matrix);

		RootMatrix = m_Sockets[SOCKET_WEAPON_R2]->Get_CombinedTransformation() *  m_pModelCom[m_eAnimModel]->Get_PivotMatrix() * m_pTransformCom->Get_WorldMatrix();
		RootMatrix.r[0] = { 1.f,0.f,0.f,0.f };
		RootMatrix.r[1] = { 0.f,1.f,0.f,0.f };
		RootMatrix.r[2] = { 0.f,0.f,1.f,0.f };
		XMStoreFloat4x4(&Matrix, RootMatrix);
		PTM->Create_InstanceParticle(L"Player01RageCutSceneB", Matrix);
		PTM->Create_InstanceParticle(L"Player01RageCutScene_2B", Matrix);
	}

}

HRESULT CPlayer01::Create_Sunder()
{
	if (m_ePlayerMode != PMODE_BURST)
	{
		CViewAlpha::VIEWALPHAINFO* Info2 = new CViewAlpha::VIEWALPHAINFO;
		Info2->fBlurPower = 1.f;
		Info2->vColor1 = { 1.f,1.f,1.f,1.f };
		Info2->vColor2 = { 0.f,1.f,1.f,1.f };
		Info2->vRatio = { 0.7f,0.7f };
		Info2->vRatioSpeed = { 2.f,1.f };
		if (FAILED(GI->Add_GameObjectToLayer(L"ViewAlpha", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2)))
			return E_FAIL;

		CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
		Info3->fBlurPower = 5.f;
		Info3->vLocalPos = { 0.f,0.f,1.f,1.f };
		Info3->vColor = { 1.f,1.f,1.f,1.f };
		Info3->vColor2 = { 0.f,1.f,1.f,1.f };
		Info3->vRatio = { 1.f,1.f };
		Info3->vRatioSpeed = { 1.5f,1.5f };
		Info3->vScaleSpeed = { 70.f,70.f,70.f };
		Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3)))
			return E_FAIL;

		CPlayer01Wind2::WIND2INFO* Info4 = new CPlayer01Wind2::WIND2INFO;
		Info4->fBlurPower = 5.f;
		Info4->vLocalPos = { 0.f,0.f,1.f,1.f };
		Info4->vColor = { 1.f,1.f,0.9f,1.f };
		Info4->vColor2 = { 0.f,1.f,0.9f,1.f };
		Info4->vRatio = { 1.f,1.f };
		Info4->vRatioSpeed = { 1.5f,1.5f };
		Info4->vScaleSpeed = { 70.f,70.f,70.f };
		Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4)))
			return E_FAIL;

		CPlayer01Ball::BALLINFO* Info5 = new CPlayer01Ball::BALLINFO;
		Info5->fBlurPower = 5.f;
		Info5->vColor = { 0.f,0.f,0.f,1.f };
		Info5->vColor2 = { 0.f,0.f,0.f,1.f };
		Info5->vRatio = { 1.f,1.f };
		Info5->vRatioSpeed = { 1.5f,1.5f };
		Info5->vScaleSpeed = { 20.f,20.f,20.f };
		Info5->vLocalPos = { 0.f,0.f,1.f,1.f };
		Info5->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		if (FAILED(GI->Add_GameObjectToLayer(L"Player01Ball", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info5)))
			return E_FAIL;

		PTM->Create_InstanceParticle(L"Player01SunderEnd", m_pTransformCom->Get_WorldFloat4x4());
	}
	else
	{
		CViewAlpha::VIEWALPHAINFO* Info2 = new CViewAlpha::VIEWALPHAINFO;
		Info2->fBlurPower = 1.f;
		Info2->vColor1 = { 1.f,1.f,0.5f,1.f };
		Info2->vColor2 = { 1.f,0.f,0.f,1.f };
		Info2->vRatio = { 0.7f,0.7f };
		Info2->vRatioSpeed = { 2.f,1.f };
		if (FAILED(GI->Add_GameObjectToLayer(L"ViewAlpha", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2)))
			return E_FAIL;

		CPlayer01Wind1::WIND1INFO* Info3 = new CPlayer01Wind1::WIND1INFO;
		Info3->fBlurPower = 5.f;
		Info3->vLocalPos = { 0.f,0.f,1.f,1.f };
		Info3->vColor = { 1.f,1.f,0.5f,1.f };
		Info3->vColor2 = { 1.f,0.f,0.f,1.f };
		Info3->vRatio = { 1.f,1.f };
		Info3->vRatioSpeed = { 1.f,1.f };
		Info3->vScaleSpeed = { 70.f,70.f,70.f };
		Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3)))
			return E_FAIL;

		CPlayer01Wind2::WIND2INFO* Info4 = new CPlayer01Wind2::WIND2INFO;
		Info4->fBlurPower = 5.f;
		Info4->vLocalPos = { 0.f,0.f,1.f,1.f };
		Info4->vColor = { 1.f,1.f,0.5f,1.f };
		Info4->vColor2 = { 1.f,0.f,0.f,1.f };
		Info4->vRatio = { 1.f,1.f };
		Info4->vRatioSpeed = { 1.f,1.f };
		Info4->vScaleSpeed = { 70.f,70.f,70.f };
		Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		if (FAILED(GI->Add_GameObjectToLayer(L"Player01Wind2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4)))
			return E_FAIL;

		CPlayer01Ball::BALLINFO* Info5 = new CPlayer01Ball::BALLINFO;
		Info5->fBlurPower = 5.f;
		Info5->vColor = { 0.f,0.f,0.f,1.f };
		Info5->vColor2 = { 0.f,0.f,0.f,1.f };
		Info5->vRatio = { 1.f,1.f };
		Info5->vRatioSpeed = { 1.5f,1.5f };
		Info5->vScaleSpeed = { 20.f,20.f,20.f };
		Info5->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		Info5->vLocalPos = { 0.f,0.f,1.f,1.f };
		if (FAILED(GI->Add_GameObjectToLayer(L"Player01Ball", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info5)))
			return E_FAIL;

		PTM->Create_InstanceParticle(L"Player01SunderEnd_2", m_pTransformCom->Get_WorldFloat4x4());
	}

	return S_OK;

}

void CPlayer01::Create_Pillar()
{
	if (m_ePlayerMode != PMODE_BURST)
	{
		CPlayer01Pillar::PILLARINFO* Info = new CPlayer01Pillar::PILLARINFO;
		Info->fBlurPower = 1.f;
		Info->vColor = { 1.f,1.f,1.f,1.f };
		Info->vColor2 = { 0.f,1.f,1.f,1.f };
		Info->vRatio = { 1.f,1.f };
		Info->vRatioSpeed = { 3.f,3.f };
		Info->vScaleSpeed = { 150.f,300.f,150.f };
		Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Pillar", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
		

		CPlayer01Pillar2::PILLAR2INFO* Info2 = new CPlayer01Pillar2::PILLAR2INFO;
		Info2->fBlurPower = 3.f;
		Info2->vColor = { 1.f,1.f,1.f,1.f };
		Info2->vColor2 = { 0.f,1.f,1.f,1.f };
		Info2->vRatio = { 1.f,1.f };
		Info2->vRatioSpeed = { 2.f,2.f };
		Info2->vScaleSpeed = { 120.f,75.f,120.f };
		Info2->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Pillar2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2);

		CPlayer01Pillar3::PILLAR3INFO* Info3 = new CPlayer01Pillar3::PILLAR3INFO;
		Info3->fBlurPower = 3.f;
		Info3->vColor = { 1.f,1.f,1.f,1.f };
		Info3->vColor2 = { 0.f,1.f,1.f,1.f };
		Info3->vRatio = { 1.f,1.f };
		Info3->vRatioSpeed = { 2.f,2.f };
		Info3->vScaleSpeed = { 75.f,75.f,75.f };
		Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Pillar3", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3);

		PTM->Create_InstanceParticle(L"Player01Skill8", m_pTransformCom->Get_WorldFloat4x4());

	}
	else
	{
		CPlayer01Pillar::PILLARINFO* Info = new CPlayer01Pillar::PILLARINFO;
		Info->fBlurPower = 2.f;
		Info->vColor = { 1.f,1.f,0.5f,1.f };
		Info->vColor2 = { 1.f,0.5f,0.f,1.f };
		Info->vRatio = { 1.f,1.f };
		Info->vRatioSpeed = { 3.f,3.f };
		Info->vScaleSpeed = { 150.f,300.f,150.f };
		Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Pillar", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);


		CPlayer01Pillar2::PILLAR2INFO* Info2 = new CPlayer01Pillar2::PILLAR2INFO;
		Info2->fBlurPower = 3.f;
		Info2->vColor = { 1.f,1.f,0.5f,1.f };
		Info2->vColor2 = { 1.f,0.3f,0.f,1.f };
		Info2->vRatio = { 1.f,1.f };
		Info2->vRatioSpeed = { 2.f,2.f };
		Info2->vScaleSpeed = { 120.f,75.f,120.f };
		Info2->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Pillar2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2);

		CPlayer01Pillar3::PILLAR3INFO* Info3 = new CPlayer01Pillar3::PILLAR3INFO;
		Info3->fBlurPower = 2.f;
		Info3->vColor = { 1.f,1.f,0.5f,1.f };
		Info3->vColor2 = { 1.f,0.3f,0.f,1.f };
		Info3->vRatio = { 1.f,1.f };
		Info3->vRatioSpeed = { 2.f,2.f };
		Info3->vScaleSpeed = { 75.f,75.f,75.f };
		Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Pillar3", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3);

		PTM->Create_InstanceParticle(L"Player01Skill8B", m_pTransformCom->Get_WorldFloat4x4());

	}

}

void CPlayer01::Create_LightEffect()
{
	
	if (m_ePlayerMode == PMODE_BATTLE)
	{
		CPlayer04VoidParticle::COLORINFO* Info = new CPlayer04VoidParticle::COLORINFO;
		Info->fY = m_fParticleY;
		Info->vSize = { 1.f,1.f };
		Info->vColor = { 1.f,1.f,1.f,1.f };
		Info->vColor2 = { 0.f,1.f,0.9f,1.f };
		GI->Add_GameObjectToLayer(L"Player04VoidParticle", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
	}

	else if (m_ePlayerMode == PMODE_NORMAL)
	{
		CPlayer04VoidParticle::COLORINFO* Info = new CPlayer04VoidParticle::COLORINFO;
		Info->fY = m_fParticleY;
		Info->vSize = { 1.f,1.f };
		Info->vColor = { 1.f,1.f,1.f,1.f };
		Info->vColor2 = { 1.f,1.f,1.f,1.f };
		GI->Add_GameObjectToLayer(L"Player04VoidParticle", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
	}
	else
	{
		CPlayer04VoidParticle::COLORINFO* Info = new CPlayer04VoidParticle::COLORINFO;
		Info->fY = m_fParticleY;
		Info->vSize = { 1.f,1.f };
		Info->vColor = { 1.f,1.f,0.5f,1.f };
		Info->vColor2 = { 1.f,0.2f,0.f,1.f };
		GI->Add_GameObjectToLayer(L"Player04VoidParticle", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);
	}
	
}

HRESULT CPlayer01::Ready_Components()
{

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;
	
	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Player01Body"), TEXT("Com_Model_Normal"), (CComponent**)&m_pModelCom[MODEL_NORMAL])))
		return E_FAIL;

	m_pModelCom[MODEL_NORMAL]->Set_RootHierarchy(1);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Player01BodyBurst"), TEXT("Com_Model_Burst"), (CComponent**)&m_pModelCom[MODEL_BURST])))
		return E_FAIL;

	m_pModelCom[MODEL_BURST]->Set_RootHierarchy(1);

	/* For.Com_Navigation */
	CNavigation::NAVIGATIONDESC			NaviDesc;
	ZeroMemory(&NaviDesc, sizeof(CNavigation::NAVIGATIONDESC));
	NaviDesc.iCurrentIndex = 0;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage1Navigation"), TEXT("Com_Navigation"), (CComponent**)&m_pNavigationCom, &NaviDesc)))
		return E_FAIL;

	Ready_Sockets();


	/* For.Com_Collider */
	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vCenter = _float3(0.f, 1.f, 0.f);
	ColDesc.vSize = _float3(5.f, 5.f, 5.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Player_Attack");
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
	ColDesc.vSize = _float3(0.2f, 0.2f, 0.2f);
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "Player_Push_Range");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	CHierarchyNode*      pPlayer_Body2 = m_pModelCom[MODEL_BURST]->Get_HierarchyNode("BreastEnd_L");
	// 금발 위치
	ColDesc.vRotation = _float3(0.f, 0.4f, 0.f);
	ColDesc.vSize = _float3(1.6f, 4.0f, 8.f);
	ColDesc.vCenter = _float3(0.62f, 0.f, 0.1f);
	ColDesc.pSocket = pPlayer_Body2;
	XMStoreFloat4x4(&ColDesc.PivotMatrix, m_pModelCom[MODEL_NORMAL]->Get_PivotMatrix());
	ColDesc.pOwner = this;
	ColDesc.iFlag = 10;
	strcpy(ColDesc.sTag, "Player_HairCut_Burst");
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;



	CHierarchyNode*		pCenter = m_pModelCom[MODEL_NORMAL]->Get_HierarchyNode("Cup_L");
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.2f, 0.2f, 0.2f);
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.pSocket = pCenter;
	XMStoreFloat4x4(&ColDesc.PivotMatrix, m_pModelCom[MODEL_NORMAL]->Get_PivotMatrix());
	ColDesc.pOwner = this;
	ColDesc.iFlag = 0;
	strcpy(ColDesc.sTag, "PhysX_Hand_L");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;
	pCenter = m_pModelCom[MODEL_NORMAL]->Get_HierarchyNode("Cup_R");
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.2f, 0.2f, 0.2f);
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.pSocket = pCenter;
	XMStoreFloat4x4(&ColDesc.PivotMatrix, m_pModelCom[MODEL_NORMAL]->Get_PivotMatrix());
	ColDesc.pOwner = this;
	ColDesc.iFlag = 0;
	strcpy(ColDesc.sTag, "PhysX_Hand_R");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;



	CHierarchyNode* pPlayer_Body = m_pModelCom[MODEL_NORMAL]->Get_HierarchyNode("BreastEnd_L");
	if (nullptr == pPlayer_Body)
		return E_FAIL;
	ColDesc.vRotation = _float3(0.f, 0.4f, 0.4f);
	ColDesc.vSize = _float3(1.6f, 4.f, 8.f);
	ColDesc.vCenter = _float3(0.6f, 0.f, 0.1f);
	ColDesc.pSocket = pPlayer_Body;
	XMStoreFloat4x4(&ColDesc.PivotMatrix, m_pModelCom[MODEL_NORMAL]->Get_PivotMatrix());
	ColDesc.pOwner = this;
	ColDesc.iFlag = 10;
	strcpy(ColDesc.sTag, "Player_HairCut");
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CPlayer01::Ready_Sockets()
{
	CHierarchyNode*		pHead = m_pModelCom[MODEL_NORMAL]->Get_HierarchyNode("Head_M");
	if (nullptr == pHead)
		return E_FAIL;
	m_Sockets.push_back(pHead);

	CHierarchyNode*		pHead2 = m_pModelCom[MODEL_BURST]->Get_HierarchyNode("Head_M");
	if (nullptr == pHead2)
		return E_FAIL;
	m_Sockets.push_back(pHead2);

	CHierarchyNode*		pWeaponL = m_pModelCom[MODEL_NORMAL]->Get_HierarchyNode("Weapon_L");
	if (nullptr == pWeaponL)
		return E_FAIL;
	m_Sockets.push_back(pWeaponL);

	CHierarchyNode*		pWeaponL2 = m_pModelCom[MODEL_BURST]->Get_HierarchyNode("Weapon_L");
	if (nullptr == pWeaponL2)
		return E_FAIL;
	m_Sockets.push_back(pWeaponL2);

	CHierarchyNode*		pWeaponR = m_pModelCom[MODEL_NORMAL]->Get_HierarchyNode("Weapon_R");
	if (nullptr == pWeaponR)
		return E_FAIL;
	m_Sockets.push_back(pWeaponR);

	CHierarchyNode*		pWeaponR2 = m_pModelCom[MODEL_BURST]->Get_HierarchyNode("Weapon_R");
	if (nullptr == pWeaponR2)
		return E_FAIL;
	m_Sockets.push_back(pWeaponR2);

	CHierarchyNode*		pKickL = m_pModelCom[MODEL_NORMAL]->Get_HierarchyNode("Toes_L");
	if (nullptr == pKickL)
		return E_FAIL;
	m_Sockets.push_back(pKickL);

	CHierarchyNode*		pKickL2 = m_pModelCom[MODEL_BURST]->Get_HierarchyNode("Toes_L");
	if (nullptr == pKickL2)
		return E_FAIL;
	m_Sockets.push_back(pKickL2);

	CHierarchyNode*		pKickR = m_pModelCom[MODEL_NORMAL]->Get_HierarchyNode("Toes_R");
	if (nullptr == pKickR)
		return E_FAIL;
	m_Sockets.push_back(pKickR);

	CHierarchyNode*		pKickR2 = m_pModelCom[MODEL_BURST]->Get_HierarchyNode("Toes_R");
	if (nullptr == pKickR2)
		return E_FAIL;
	m_Sockets.push_back(pKickR2);

	return S_OK;
}

HRESULT CPlayer01::Ready_PlayerParts()
{
	CGameObject*		pPlayer01HairA = GI->Clone_GameObject(TEXT("Player01HairA"));
	if (nullptr == pPlayer01HairA)
		return E_FAIL;
	m_Parts.push_back((CParts*)pPlayer01HairA);

	CGameObject*		pPlayer01HairB = GI->Clone_GameObject(TEXT("Player01HairB"));
	if (nullptr == pPlayer01HairB)
		return E_FAIL;
	m_Parts.push_back((CParts*)pPlayer01HairB);

	CGameObject*		pPlayer01HairC = GI->Clone_GameObject(TEXT("Player01HairC"));
	if (nullptr == pPlayer01HairC)
		return E_FAIL;
	m_Parts.push_back((CParts*)pPlayer01HairC);

	

	return S_OK;
}

HRESULT CPlayer01::Update_Parts()
{

	if (m_eAnimModel == MODEL_NORMAL)
	{
		_matrix HeadMatrix = m_Sockets[SOCKET_HEAD]->Get_CombinedTransformation()* m_pModelCom[m_eAnimModel]->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
		switch (m_ePlayerMode)
		{
		case Client::CPlayer01::PMODE_NORMAL:
			m_Parts[PARTS_HAIRA]->SetUp_State(HeadMatrix);
			break;
		case Client::CPlayer01::PMODE_BATTLE:
			m_Parts[PARTS_HAIRB]->SetUp_State(HeadMatrix);
			break;
		case Client::CPlayer01::PMODE_BURST:
			m_Parts[PARTS_HAIRC]->SetUp_State(HeadMatrix);
			break;
		}
	}
	else
	{
		_matrix Head2Matrix = m_Sockets[SOCKET_HEAD2]->Get_CombinedTransformation()* m_pModelCom[m_eAnimModel]->Get_PivotMatrix()* m_pTransformCom->Get_WorldMatrix();
		switch (m_ePlayerMode)
		{
		case Client::CPlayer01::PMODE_NORMAL:
			m_Parts[PARTS_HAIRA]->SetUp_State(Head2Matrix);
			break;
		case Client::CPlayer01::PMODE_BATTLE:
			m_Parts[PARTS_HAIRB]->SetUp_State(Head2Matrix);
			break;
		case Client::CPlayer01::PMODE_BURST:
			m_Parts[PARTS_HAIRC]->SetUp_State(Head2Matrix);
			break;
		}
	}
	
	return S_OK;
}

void CPlayer01::Render_Parts(_float fTimeDelta)
{

	m_Parts[PARTS_HAIRA]->LateTick(fTimeDelta);
	m_Parts[PARTS_HAIRB]->LateTick(fTimeDelta);
	m_Parts[PARTS_HAIRC]->LateTick(fTimeDelta);


	//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_Parts[PARTS_HAIRA]);
	//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, m_Parts[PARTS_HAIRB]);
	//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, m_Parts[PARTS_HAIRC]);


	switch (m_ePlayerMode)
	{
	case Client::CPlayer01::PMODE_NORMAL:
		//m_Parts[PARTS_HAIRA]->LateTick(fTimeDelta);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, m_Parts[PARTS_HAIRA]);
		break;
	case Client::CPlayer01::PMODE_BATTLE:
		//m_Parts[PARTS_HAIRB]->LateTick(fTimeDelta);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, m_Parts[PARTS_HAIRB]);
		break;
	case Client::CPlayer01::PMODE_BURST:
		//m_Parts[PARTS_HAIRC]->LateTick(fTimeDelta);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, m_Parts[PARTS_HAIRC]);
		break;
	}

}

CPlayer01 * CPlayer01::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer01*		pInstance = new CPlayer01(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer01"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer01::Clone(void * pArg)
{
	CPlayer01*		pInstance = new CPlayer01(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer01"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer01::Free()
{
	__super::Free();

	for (int i = MODEL_NORMAL; i < MODEL_END; ++i)
		Safe_Release(m_pModelCom[i]);
	
	for (auto& pPart : m_Parts)
		Safe_Release(pPart);

	for (auto& iter : m_SwordTrails)
		Safe_Release(iter);

	for (auto& iter : m_PointTrails)
		Safe_Release(iter);

	for (auto& iter : m_MotionTrails)
		Safe_Release(iter);
	
	m_Parts.clear();

	Safe_Release(m_pNavigationCom);
	Safe_Release(m_pShaderCom);
}
