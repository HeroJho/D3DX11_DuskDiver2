#include "stdafx.h"
#include "..\Public\BossAttackRange.h"
#include "GameInstance.h"
#include "Time_Manager.h"
#include "Monster_Manager.h"
#include "Particle_Manager.h"
#include "Player01Pillar.h"
#include "Player01Pillar2.h"
#include "Player01Pillar3.h"
#include "ToolManager.h"
#include "BossPillar.h"
#include "Player01Wind1.h"
#include "Player01Wind2.h"
#include "Player01Landing.h"
#include "Camera_Manager.h"

CBossAttackRange::CBossAttackRange(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CBossAttackRange::CBossAttackRange(const CBossAttackRange & rhs)
	: CCreture(rhs)
{
}

HRESULT CBossAttackRange::Initialize_Prototype()
{

	return S_OK;
}

HRESULT CBossAttackRange::Initialize(void * pArg)
{

	m_EffectRangeInfo =(*(EFFECTRANGEINFO*)pArg);
	m_bSound = m_EffectRangeInfo.bSound;
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;	
	
	Random_InitPos();

	m_eHitType = TYPE_SPIN;
	m_fPushPower = 0.f;
	m_fAirPower = 15.f;
	m_ePushType = PUSH;
	m_fDamage = 100.f;
	
	
	PTM->Create_InstanceParticle(L"BeastPillar", m_pTransformCom->Get_WorldFloat4x4());
	return S_OK;
}

void CBossAttackRange::Tick(_float fTimeDelta)
{

	TIM->Add_TimeObj(CTime_Manager::OBJ_EFFECT, this);
	fTimeDelta *= m_fTMRatio;

	m_bAttackCol = false;
	Emp_Col();

	m_fCreateAcc += 1.f * fTimeDelta;
	if (m_fCreateAcc >= 1.f && !m_bCreatePillar)
	{
		if (m_EffectRangeInfo.bSound)
		{
			GI->PlaySoundW(L"EFF_CHR_Spell_Ground_17_D.ogg", SD_TRAININGBOT2, 0.4f);
		}


		m_bCreatePillar = true;
		m_bAttackCol = true;

		PTM->Create_InstanceParticle(L"BeastPillarDistortion", m_pTransformCom->Get_WorldFloat4x4());
		CRM->Get_Cam()->Shake(0.05f, 0.05f, false, 0.05f);
		

		CBossPillar::PILLARINFO* Info = new CBossPillar::PILLARINFO;
		Info->fBlurPower = 10.f;
		Info->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info->vColor = { 1.f,0.f,0.f,1.f };
		Info->vColor2 = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 2.f,10.f };
		Info->vScale = { 2.f,15.f,2.f };
		Info->vRatioSpeed = { 0.f,0.f };
		Info->vScaleSpeed = { -4.f,0.f,-4.f };
		Info->vUVFixSpeed = { 1.f,3.f };
		Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"BossPillar", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info);

		CBossPillar::PILLARINFO* Info4 = new CBossPillar::PILLARINFO;
		Info4->fBlurPower = 2.f;
		Info4->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info4->vColor = { 0.f,0.f,0.f,1.f };
		Info4->vColor2 = { 0.f,0.f,0.f,1.f };
		Info4->vRatio = { 1.f,3.f };
		Info4->vScale = { 2.f,15.f,2.f };
		Info4->vRatioSpeed = { 0.f,0.f };
		Info4->vScaleSpeed = { -4.f,0.f,-4.f };
		Info4->vUVFixSpeed = { -1.f,3.f };
		Info4->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"BossPillar", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info4);

		CPlayer01Pillar2::PILLAR2INFO* Info2 = new CPlayer01Pillar2::PILLAR2INFO;
		Info2->fBlurPower = 5.f;
		Info2->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info2->vColor = { 1.f,0.f,0.f,1.f };
		Info2->vColor2 = { 0.f,0.f,0.f,1.f };
		Info2->vRatio = { 1.f,1.f };
		Info2->vRatioSpeed = { 2.f,2.f };
		Info2->vScaleSpeed = { 120.f,30.f,120.f };
		Info2->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Pillar2", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info2);

		CPlayer01Pillar3::PILLAR3INFO* Info3 = new CPlayer01Pillar3::PILLAR3INFO;
		Info3->fBlurPower = 5.f;
		Info3->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info3->vColor = { 1.f,0.f,0.f,1.f };
		Info3->vColor2 = { 0.f,0.f,0.f,1.f };
		Info3->vRatio = { 1.f,1.f };
		Info3->vRatioSpeed = { 2.f,2.f };
		Info3->vScaleSpeed = { 75.f,75.f,75.f };
		Info3->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Pillar3", TM->Get_CurLevel(), L"Layer_Player01Effect", &Info3);

		CPlayer01Wind1::WIND1INFO* Info5 = new CPlayer01Wind1::WIND1INFO;
		Info5->fBlurPower = 0.f;
		Info5->fLifeTime = 0.f;
		Info5->fTurnSpeed = 5.f;
		Info5->vColor = { 1.f,0.f,0.f,1.f };
		Info5->vColor2 = { 0.f,0.f,0.f,1.f };
		Info5->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info5->vRatio = { 1.f,1.f };
		Info5->vRatioSpeed = { 1.f,0.f };
		Info5->vScale = { 2.f,10.f,2.f };
		Info5->vScaleSpeed = { 100.f,30.f,100.f };
		Info5->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_BeastEffect", &Info5);

	}

	if (m_fCreateAcc >= 1.5f)
		Set_Dead();
	
}

void CBossAttackRange::LateTick(_float fTimeDelta)
{
	Tick_Col(m_pTransformCom->Get_WorldMatrix_NoScale());
	if(m_bAttackCol)
		Choose_Col(nullptr);
	Render_Col(m_pRendererCom);

}

HRESULT CBossAttackRange::Render()
{
	

	return S_OK;
}

void CBossAttackRange::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());
	


	ImGui::End();

}

void CBossAttackRange::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		if (!strcmp("Player_Body", Desc.pOtherDesc->sTag) && !strcmp("PillarAttack", Desc.pMyDesc->sTag))
		{	
		
			((CCreture*)Desc.pOther)->On_Hit(this);
		}
	}
	
}

void CBossAttackRange::Random_InitPos()
{
	

	/*switch (m_EffectRangeInfo.iIndex)
	{
	case 1:
		m_fRandomX = GI->Get_FloatRandom(3.f, 7.6f);
		m_fStaticY = 0.2f;
		m_fRandomZ = GI->Get_FloatRandom(-45.f, -37.f);
		break;
	case 2:
		m_fRandomX = GI->Get_FloatRandom(-5.f, 6.f);
		m_fStaticY = 0.665f;
		m_fRandomZ = GI->Get_FloatRandom(-33.f, -30.f);
		break;
	case 3:
		m_fRandomX = GI->Get_FloatRandom(-7.f, -2.f);
		m_fStaticY = 0.2f;
		m_fRandomZ = GI->Get_FloatRandom(-42.f, -38.f);
		break;
	case 4:
		m_fRandomX = GI->Get_FloatRandom(1.f, 6.f);
		m_fStaticY = 0.2f;
		m_fRandomZ = GI->Get_FloatRandom(-47.5f, -42.f);
		break;
	case 5:
		m_fRandomX = GI->Get_FloatRandom(-5.f, 1.6f);
		m_fStaticY = 0.2f;
		m_fRandomZ = GI->Get_FloatRandom(-48.f, -45.f);
		break;
	case 6:
		m_fRandomX = GI->Get_FloatRandom(2.7f, 10.f);
		m_fStaticY = 0.2f;
		m_fRandomZ = GI->Get_FloatRandom(-51.f, -48.f);
		break;
	case 7:
		m_fRandomX = GI->Get_FloatRandom(1.f, 7.6f);
		m_fStaticY = 0.2f;
		m_fRandomZ = GI->Get_FloatRandom(-60.f, -37.f);
		break;
	case 8:
		m_fRandomX = GI->Get_FloatRandom(-4.f, 7.f);
		m_fStaticY = 0.665f;
		m_fRandomZ = GI->Get_FloatRandom(-33.f, -30.f);
		break;
	case 9:
		m_fRandomX = GI->Get_FloatRandom(-7.f, -2.f);
		m_fStaticY = 0.2f;
		m_fRandomZ = GI->Get_FloatRandom(-60.f, -38.f);
		break;
	case 10:
		m_fRandomX = GI->Get_FloatRandom(1.f, 6.f);
		m_fStaticY = 0.2f;
		m_fRandomZ = GI->Get_FloatRandom(-60.5f, -42.f);
		break;
	case 11:
		m_fRandomX = GI->Get_FloatRandom(-5.f, 1.6f);
		m_fStaticY = 0.2f;
		m_fRandomZ = GI->Get_FloatRandom(-60.f, -45.f);
		break;
	case 12:
		m_fRandomX = GI->Get_FloatRandom(2.7f, 10.f);
		m_fStaticY = 0.2f;
		m_fRandomZ = GI->Get_FloatRandom(-60.f, -48.f);
		break;
	}*/
	m_fRandomX = GI->Get_FloatRandom(-13.f, 13.f);
	m_fStaticY = 0.2f;
	m_fRandomZ = GI->Get_FloatRandom(-62.f, -36.f);

	_matrix WorldMatrix = XMMatrixIdentity();
	WorldMatrix.r[3] = { m_fRandomX, m_fStaticY, m_fRandomZ, 1.f };
	_float4x4 Matrix;
	XMStoreFloat4x4(&Matrix, WorldMatrix);
	m_pTransformCom->Set_WorldMatrix(Matrix);

}


HRESULT CBossAttackRange::Ready_Components()
{

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vCenter = _float3(0.f, 1.f, 0.f);
	ColDesc.vSize = _float3(3.5f, 10.f, 3.5f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "PillarAttack");
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;

	return S_OK;
}

CBossAttackRange * CBossAttackRange::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBossAttackRange*		pInstance = new CBossAttackRange(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBossAttackRange"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCreture * CBossAttackRange::Clone(void * pArg)
{
	CBossAttackRange*		pInstance = new CBossAttackRange(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBossAttackRange"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBossAttackRange::Free()
{
	__super::Free();
	

}
