#include "stdafx.h"
#include "..\Public\BeastStoneBody.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "DropItem.h"
#include "Time_Manager.h"
#include "Camera_Manager.h"
#include "BeastStonePart.h"
#include "Player01Wind1.h"
#include "Player01Landing.h"
#include "Particle_Manager.h"

CBeastStoneBody::CBeastStoneBody(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CCreture(pDevice, pContext)
{
}

CBeastStoneBody::CBeastStoneBody(const CBeastStoneBody & rhs)
	: CCreture(rhs)
{
}

HRESULT CBeastStoneBody::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBeastStoneBody::Initialize(void * pArg)
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	m_pCreateInfo = *(CREATEINFO*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Boss";

	Make_ImGui();

	m_fDamage = 50.f;
	m_eHitType = TYPE_AWAY;
	m_fPushPower = 6.f;
	m_fAirPower = 3.f;
	m_fHurtTime = 0.03f;

	return S_OK;
}

void CBeastStoneBody::Tick(_float fTimeDelta)
{
	Emp_Col();
	if (m_bRender)
	{

		TIM->Add_TimeObj(CTime_Manager::OBJ_MONSTER, this);

		Check_Rim(fTimeDelta);
		fTimeDelta *= m_fTMRatio;

		Emp_Col();

		if (m_bParticleOn)
		{
			m_fParticleAcc += 1.f * fTimeDelta;
			if (m_fParticleAcc >= 0.2f)
			{
				_matrix Identity = XMMatrixIdentity();
				Identity.r[3] += m_pTransformCom->Get_State(CTransform::STATE_POSITION);
				_float4x4 WorldMatrix;
				XMStoreFloat4x4(&WorldMatrix, Identity);
				PTM->Create_InstanceParticle(L"BreathStone", WorldMatrix);
				PTM->Create_InstanceParticle(L"BreathStone_2", WorldMatrix);
				m_bParticleOn = false;
				m_fParticleAcc = 0.f;
			}
		}


		if (m_pTransformCom->Get_State(CTransform::STATE_POSITION).m128_f32[1] >= m_fLimitY)
		{
			m_pTransformCom->Go_Dir(XMVectorSet(0.f, -1.f, 0.f, 0.f), m_fSpeed, fTimeDelta);
		}
		else
		{
			m_bAttackCol = false;

			if (m_bOneShake)
			{
				Create_Effect();
				CRM->Get_Cam()->Shake(0.15f, 0.25f, false, 0.15f);
				m_bOneShake = false;
			}
		}

	}

	if (m_iShieldBody != m_pCreateInfo.iIndex) //쉴드가 아닌것만 깨지도록
	{
		if (m_fHeatingAcc >= 1.6f && m_bOneFallen)
		{
			m_bRender = false;
			m_bFallen = true;
			m_bOneFallen = false;
			GI->PlaySoundW(L"EFF_CHR_Impact_Rock_02_A.ogg", SD_BEASTVOICE, 1.f);
		}
	}

	if (m_bFallen)
	{
		Initialize_Parts();

		m_bFallen = false;
		m_bOneCol = true;
		m_bAttackRim = false;
		m_bRim = false;
		m_fHeatingAcc = 0.f;
		m_fRimPower = 0.f;
		m_iShaderPath = 0;
		m_fHeatRim = 15.f;
	}

}

void CBeastStoneBody::LateTick(_float fTimeDelta)
{
	if (m_bRender)
	{
		if (nullptr == m_pRendererCom)
			return;

		Tick_Col(m_pTransformCom->Get_WorldMatrix());

		if (m_bAttackCol)
			Choose_Col("Stone_Attack");

		if(m_bShieldCol)
			Choose_Col("Stone_Shield");

		if(m_bBodyCol)
			Choose_Col("Monster_Body1");

		Choose_Col("Monster_Push_Range");


		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

		Render_Col(m_pRendererCom);

	}
	
}

HRESULT CBeastStoneBody::Render()
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


	_float fGama = 1.4f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;

	_float fBlurPower = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Set_RawValue("g_RimPower", &m_fRimPower, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_RimColor", &m_fRimColor, sizeof(_float4))))
		return E_FAIL;


	//if (FAILED(m_pShaderCom->Set_RawValue("g_Alpha", &m_fAlpa, sizeof(_float))))
	//	return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		// 노말 텍스쳐가 있다면
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, m_iShaderPath)))
			return E_FAIL;
	}


	//for (auto& iter : m_PartALayer)
	//{
	//	iter->Render();
	//}
	//for (auto& iter : m_PartBLayer)
	//{
	//	iter->Render();
	//}

	return S_OK;
}

void CBeastStoneBody::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);

	ImGui::Text("Index : %d", m_pCreateInfo.iIndex);
	ImGui::Text("DeadAcc : %f", m_fDeadAcc);

	ImGui::Checkbox("RenderBody", &m_bRender);

	ImGui::DragFloat("RimPower ", &m_fRimPower, 0.01f);


	_float3 vSize = Get_Colliders("Monster_Push_Range")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Monster_Push_Range")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Get_Colliders("Monster_Push_Range")->Set_Size(vSize);
	Get_Colliders("Monster_Push_Range")->Set_Center(vCenter);



	ImGui::End();

}

void CBeastStoneBody::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{

	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		// 내 손 Sphere 둘중 하나와 상대편의 OBB와 충돌 했다면
		if (!strcmp("Stone_Attack", Desc.pMyDesc->sTag) && !strcmp("Player_Body", Desc.pOtherDesc->sTag))
		{
			((CCreture*)Desc.pOther)->On_Hit(this);
			m_bAttackCol = false;
		}
	}

	//브레스랑 닿을때 행동해야하는거, 브레스 길이 줄이기

	if ("Tag_Boss" == Desc.pOther->Get_Tag())
	{
		// 내 손 Sphere 둘중 하나와 상대편의 OBB와 충돌 했다면
		if (!strcmp("Stone_Attack", Desc.pMyDesc->sTag) && !strcmp("Breath_Attack", Desc.pOtherDesc->sTag))
		{
			//파티클 생성이랑
			
			((CCreture*)Desc.pOther)->On_Hit(this);
			m_bAttackCol = false;
		}
	}


}

void CBeastStoneBody::On_Hit(CCreture * pOther)
{
	//브레스랑 닿았을때 rim라이트 계속 올리고
	//일정시간 지났을때 터져야한다

	if (m_bOneCol)
	{
		m_bOneCol = false;
		m_bRim = true;
		m_bAttackRim = true;
		m_fRimAcc = 0.f;
	}

}

void CBeastStoneBody::Reset_InitState()
{
	m_bOneShake = true;  m_fRimPower = 0.f; m_iShaderPath = 0; m_bOneFallen = true; m_bAttackCol = true;
}

void CBeastStoneBody::Create_Effect()
{
	switch (m_pCreateInfo.iIndex)
	{
	case 1:
	{
		CPlayer01Wind1::WIND1INFO* Info = new CPlayer01Wind1::WIND1INFO;
		Info->fBlurPower = 0.f;
		Info->fLifeTime = 0.f;
		Info->fTurnSpeed = 5.f;
		Info->vColor = { 0.5f,0.5f,0.5f,1.f };
		Info->vColor2 = { 0.5f,0.5f,0.5f,1.f };
		Info->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 1.f,1.f };
		Info->vRatioSpeed = { 2.f,0.f };
		Info->vScale = { 10.f,30.f,10.f };
		Info->vScaleSpeed = { 80.f,50.f,80.f };
		Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_BeastEffect", &Info);

		CPlayer01Landing::LANDINGINFO* Info2 = new CPlayer01Landing::LANDINGINFO;
		Info2->fBlurPower = 0.f;
		Info2->fTurnSpeed = 5.f;
		Info2->vColor = { 0.6f,0.6f,0.6f,1.f };
		Info2->vColor2 = { 0.6f,0.6f,0.6f,1.f };
		Info2->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info2->vRatio = { 1.f,1.f };
		Info2->vRatioSpeed = { 2.f,0.f };
		Info2->vScale = { 10.f,10.f,10.f };
		Info2->vScaleSpeed = { 80.f,50.f,80.f };
		Info2->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Landing", TM->Get_CurLevel(), L"Layer_BeastEffect", &Info2);
	}
		break;

	case 2:
	{
		CPlayer01Wind1::WIND1INFO* Info = new CPlayer01Wind1::WIND1INFO;
		Info->fBlurPower = 0.f;
		Info->fLifeTime = 0.f;
		Info->fTurnSpeed = 5.f;
		Info->vColor = { 0.5f,0.5f,0.5f,1.f };
		Info->vColor2 = { 0.5f,0.5f,0.5f,1.f };
		Info->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 1.f,1.f };
		Info->vRatioSpeed = { 2.f,0.f };
		Info->vScale = { 10.f,30.f,10.f };
		Info->vScaleSpeed = { 80.f,50.f,80.f };
		Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_BeastEffect", &Info);

		CPlayer01Landing::LANDINGINFO* Info2 = new CPlayer01Landing::LANDINGINFO;
		Info2->fBlurPower = 0.f;
		Info2->fTurnSpeed = 5.f;
		Info2->vColor = { 0.6f,0.6f,0.6f,1.f };
		Info2->vColor2 = { 0.6f,0.6f,0.6f,1.f };
		Info2->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info2->vRatio = { 1.f,1.f };
		Info2->vRatioSpeed = { 2.f,0.f };
		Info2->vScale = { 10.f,10.f,10.f };
		Info2->vScaleSpeed = { 80.f,50.f,80.f };
		Info2->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Landing", TM->Get_CurLevel(), L"Layer_BeastEffect", &Info2);
	}
		break;

	case 3:
	{
		CPlayer01Wind1::WIND1INFO* Info = new CPlayer01Wind1::WIND1INFO;
		Info->fBlurPower = 0.f;
		Info->fLifeTime = 0.f;
		Info->fTurnSpeed = 5.f;
		Info->vColor = { 0.5f,0.5f,0.5f,1.f };
		Info->vColor2 = { 0.5f,0.5f,0.5f,1.f };
		Info->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 1.f,1.f };
		Info->vRatioSpeed = { 2.f,0.f };
		Info->vScale = { 10.f,30.f,10.f };
		Info->vScaleSpeed = { 80.f,50.f,80.f };
		Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_BeastEffect", &Info);

		CPlayer01Landing::LANDINGINFO* Info2 = new CPlayer01Landing::LANDINGINFO;
		Info2->fBlurPower = 0.f;
		Info2->fTurnSpeed = 5.f;
		Info2->vColor = { 0.6f,0.6f,0.6f,1.f };
		Info2->vColor2 = { 0.6f,0.6f,0.6f,1.f };
		Info2->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info2->vRatio = { 1.f,1.f };
		Info2->vRatioSpeed = { 2.f,0.f };
		Info2->vScale = { 10.f,10.f,10.f };
		Info2->vScaleSpeed = { 80.f,50.f,80.f };
		Info2->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Landing", TM->Get_CurLevel(), L"Layer_BeastEffect", &Info2);

	}
		break;

	case 4:
	{
		CPlayer01Wind1::WIND1INFO* Info = new CPlayer01Wind1::WIND1INFO;
		Info->fBlurPower = 0.f;
		Info->fLifeTime = 0.f;
		Info->fTurnSpeed = 5.f;
		Info->vColor = { 0.5f,0.5f,0.5f,1.f };
		Info->vColor2 = { 0.5f,0.5f,0.5f,1.f };
		Info->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info->vRatio = { 1.f,1.f };
		Info->vRatioSpeed = { 2.f,0.f };
		Info->vScale = { 10.f,20.f,10.f };
		Info->vScaleSpeed = { 80.f,50.f,80.f };
		Info->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Wind1", TM->Get_CurLevel(), L"Layer_BeastEffect", &Info);

		CPlayer01Landing::LANDINGINFO* Info2 = new CPlayer01Landing::LANDINGINFO;
		Info2->fBlurPower = 0.f;
		Info2->fTurnSpeed = 5.f;
		Info2->vColor = { 0.6f,0.6f,0.6f,1.f };
		Info2->vColor2 = { 0.6f,0.6f,0.6f,1.f };
		Info2->vLocalPos = { 0.f,0.f,0.f,1.f };
		Info2->vRatio = { 1.f,1.f };
		Info2->vRatioSpeed = { 2.f,0.f };
		Info2->vScale = { 12.f,10.f,12.f };
		Info2->vScaleSpeed = { 80.f,50.f, 80.f };
		Info2->vWorldMatrix = m_pTransformCom->Get_WorldFloat4x4();
		GI->Add_GameObjectToLayer(L"Player01Landing", TM->Get_CurLevel(), L"Layer_BeastEffect", &Info2);
	}
		break;
	}
}

void CBeastStoneBody::Set_InitState()
{
	//위치도 랜덤으로

	_float fRandomX = 0.f;
	_float fRandomY = GI->Get_FloatRandom(15.f, 21.f);
	_float fRandomZ = 0.f;

	switch (m_pCreateInfo.iIndex)
	{
	case 1:
		fRandomX = GI->Get_FloatRandom(3.f, 7.6f);
		fRandomZ = GI->Get_FloatRandom(-41.f, -36.f);
		break;
	case 2:
		fRandomX = GI->Get_FloatRandom(-5.f, 4.f);
		fRandomZ = GI->Get_FloatRandom(-36.5f, -34.f);
		break;
	case 3:
		fRandomX = GI->Get_FloatRandom(-7.f, -2.f);
		fRandomZ = GI->Get_FloatRandom(-42.f, -38.f);
		break;
	case 4:
		fRandomX = GI->Get_FloatRandom(-4.f, 6.f);
		fRandomZ = GI->Get_FloatRandom(-45.f, -38.f);
		break;
	}
	
	m_fSpeed = GI->Get_FloatRandom(17.f, 22.f);

	m_pCreateInfo.vPosition = { fRandomX, fRandomY, fRandomZ, 1.f };

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_pCreateInfo.vPosition));
	//회전도 랜덤
	_float fRandomRot = GI->Get_FloatRandom(0.f, 360.f);

	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fRandomRot));

}

void CBeastStoneBody::On_HeatingNotShield(CCreture * pOther)
{
	
	
	m_fRimPower = m_fHeatRim;

	if (m_fRimPower <= 0.5f)
		m_fRimPower = 0.5f;

	m_bRim = true;
}

void CBeastStoneBody::On_Heating(CCreture * pOther)
{
	m_bParticleOn = true;
	m_fHeatRim -= 0.1f;
	//쉴드랑 브레스랑 맞았을때
	m_fHeatingAcc += 0.01f;
	m_fHeatRim -= 0.1f;
	m_fRimPower = m_fHeatRim;
	if (m_fRimPower <= 0.5f)
		m_fRimPower = 0.5f;
	m_bRim = true;
}

HRESULT CBeastStoneBody::Ready_ModelCom()
{
	switch (m_pCreateInfo.iIndex) //모델컴이랑 제한 y축 같이 정해주기
	{
	case 1:
		m_fLimitY = 0.015f;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("BeastStoneBody2"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 2:
		m_fLimitY = 0.025f;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("BeastStoneBody1"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 3:
		m_fLimitY = 0.005f;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("BeastStoneBody3"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 4:
		m_fLimitY =  -0.035f;
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("BeastStoneBody4"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	}

	return S_OK;
}

void CBeastStoneBody::Check_Rim(_float fTimeDelta)
{
	if (m_bAttackRim)
	{
		//바디인지 쉴드인지 구분 필요

		m_fRimAcc += 1.f * fTimeDelta;
		m_fRimPower = 0.2f;

		if (m_fRimAcc > 0.2f)
		{
			m_bRim = false;
			m_bAttackRim = false;
			m_fRimAcc = 0.f;
		}
	}

	if (m_bRim)
	{
		if (m_iShieldBody == m_pCreateInfo.iIndex)
			m_fRimColor = _float4(0.f, 0.635f, 0.909f, 1.f);
		else
			m_fRimColor = _float4(1.f, 0.047f, 0.f, 1.f);

		m_iShaderPath = 1;
	}
	else
		m_iShaderPath = 0;
	
}

HRESULT CBeastStoneBody::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	//인덱스 번호마다 body도 다르고 콜라이더도 다르게해야됨
	if (FAILED(Ready_ModelCom()))
		return E_FAIL;

	/*콜라이더*/

	if (FAILED(Ready_Collider()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBeastStoneBody::Ready_Collider()
{
	CCollider::COLLIDERDESC ColDesc;

	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);

	switch (m_pCreateInfo.iIndex)
	{
	case 1:
		ColDesc.vSize = _float3(1.5f, 4.7f, 1.7f);
		ColDesc.vCenter = _float3(0.f, 2.35f, 0.f);
		break;
	case 2:
		ColDesc.vSize = _float3(1.3f, 3.7f, 1.6f);
		ColDesc.vCenter = _float3(0.f, 1.85f, 0.f);
		break;
	case 3:
		ColDesc.vSize = _float3(1.4f, 4.f, 1.2f);
		ColDesc.vCenter = _float3(0.f, 2.f, 0.f);
		break;
	case 4:
		ColDesc.vSize = _float3(1.5f, 4.7f, 1.6f);
		ColDesc.vCenter = _float3(0.f, 2.35f, 0.f);
		break;
	}
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Stone_Attack"); 

	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;

	strcpy(ColDesc.sTag, "Monster_Body1");

	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;

	switch (m_pCreateInfo.iIndex)
	{
	case 1:
		ColDesc.vSize = _float3(2.6f, 10.f, 2.6f);
		ColDesc.vCenter = _float3(0.f, 2.f, 0.f);
		break;
	case 2:
		ColDesc.vSize = _float3(2.4f, 10.f, 2.7f);
		ColDesc.vCenter = _float3(0.f, 2.f, 0.f);
		break;
	case 3:
		ColDesc.vSize = _float3(2.5f, 10.f, 2.4f);
		ColDesc.vCenter = _float3(0.f, 2.f, 0.f);
		break;
	case 4:
		ColDesc.vSize = _float3(2.7f, 10.f, 2.7f);
		ColDesc.vCenter = _float3(0.f, 2.f, 0.f);
		break;
	}
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;

	strcpy(ColDesc.sTag, "Stone_Shield"); //이렇게 해놓으면 플레이어가 밀지 못함, Monster_body로 하면 충돌처리됨
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;

	switch (m_pCreateInfo.iIndex)
	{
	case 1:
		ColDesc.vSize = _float3(1.3f, 1.3f, 1.3f);
		ColDesc.vCenter = _float3(0.f, 0.65f, 0.f);
		break;
	case 2:
		ColDesc.vSize = _float3(1.1f, 1.1f, 1.1f);
		ColDesc.vCenter = _float3(0.f, 0.55f, 0.f);
		break;
	case 3:
		ColDesc.vSize = _float3(1.2f, 1.2f, 1.2f);
		ColDesc.vCenter = _float3(0.f, 0.6f, 0.f);
		break;
	case 4:
		ColDesc.vSize = _float3(1.3f, 1.3f, 1.3f);
		ColDesc.vCenter = _float3(0.f, 0.65f, 0.f);
		break;
	}
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;

	strcpy(ColDesc.sTag, "Monster_Push_Range"); //이렇게 해놓으면 플레이어가 밀지 못함, Monster_body로 하면 충돌처리됨
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;
	return S_OK;
}

void CBeastStoneBody::Initialize_Parts()
{
	//=====================================================================================================11111
	CBeastStonePart::PARTDESC StonePartDesc;


	StonePartDesc.iStoneType = 1;
	XMStoreFloat4(&StonePartDesc.vLocalPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	StonePartDesc.fAngle = 0.f;
	StonePartDesc.vStartPos = _float3(-0.6f, 0.9f, 0.f); // 모델 위치랑 상관있음, 로컬
	StonePartDesc.vPos = _float3(0.f, 0.f, 0.f); //콜라이더와의 위치
	StonePartDesc.vRotation = _float3(0.f, 0.f, 0.f); //회전은 그냥
	StonePartDesc.vExtense = _float3(0.8f, 1.2f, 0.9f);

	(GI->Add_GameObjectToLayer(TEXT("BeastStonePart"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &StonePartDesc));


	//=====================================================================================================2222
	StonePartDesc.iStoneType = 2;

	StonePartDesc.vStartPos = _float3(-0.6f, 4.f, 0.05f); // 모델 위치랑 상관있음
	StonePartDesc.vExtense = _float3(0.7f, 0.4f, 0.6f);

	(GI->Add_GameObjectToLayer(TEXT("BeastStonePart"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &StonePartDesc));

	StonePartDesc.vStartPos = _float3(-0.6f, 1.6f, 0.f); // 모델 위치랑 상관있음

	(GI->Add_GameObjectToLayer(TEXT("BeastStonePart"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &StonePartDesc));


	//=====================================================================================================33333
	StonePartDesc.iStoneType = 3;

	StonePartDesc.vStartPos = _float3(0.f, 0.6f, 0.f); // 모델 위치랑 상관있음
	StonePartDesc.vExtense = _float3(1.2f, 0.8f, 0.9f);
	
	(GI->Add_GameObjectToLayer(TEXT("BeastStonePart"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &StonePartDesc));


	StonePartDesc.vStartPos = _float3(0.3f, 3.f, 0.f); // 모델 위치랑 상관있음
	StonePartDesc.fAngle = -48.f;

	(GI->Add_GameObjectToLayer(TEXT("BeastStonePart"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &StonePartDesc));

	//=====================================================================================================444444
	StonePartDesc.iStoneType = 4;

	StonePartDesc.fAngle = 0.f;
	StonePartDesc.vStartPos = _float3(-0.1f, 1.7f, -0.05f); // 모델 위치랑 상관있음
	StonePartDesc.vExtense = _float3(0.8f, 0.4f, 0.8f);

	(GI->Add_GameObjectToLayer(TEXT("BeastStonePart"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &StonePartDesc));



	StonePartDesc.vStartPos = _float3(-0.5f, 3.7f, 0.f); // 모델 위치랑 상관있음

	(GI->Add_GameObjectToLayer(TEXT("BeastStonePart"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &StonePartDesc));


	//=====================================================================================================55555
	StonePartDesc.iStoneType = 5;
	XMStoreFloat4(&StonePartDesc.vLocalPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	StonePartDesc.vStartPos = _float3(-0.5f, 3.3f, -0.15f); // 모델 위치랑 상관있음
	StonePartDesc.vExtense = _float3(0.5f, 0.4f, 0.5f);

	(GI->Add_GameObjectToLayer(TEXT("BeastStonePart"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &StonePartDesc));



	//=====================================================================================================66666
	StonePartDesc.iStoneType = 6;

	StonePartDesc.vStartPos = _float3(-0.3f, 4.4f, 0.05f); // 모델 위치랑 상관있음
	StonePartDesc.vExtense = _float3(0.9f, 0.4f, 0.7f);

	(GI->Add_GameObjectToLayer(TEXT("BeastStonePart"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &StonePartDesc));


	StonePartDesc.vStartPos = _float3(0.f, 2.5f, 0.f); // 모델 위치랑 상관있음
	StonePartDesc.vExtense = _float3(0.9f, 0.4f, 0.7f);

	(GI->Add_GameObjectToLayer(TEXT("BeastStonePart"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &StonePartDesc));


	//=====================================================================================================7777
	StonePartDesc.iStoneType = 7;

	StonePartDesc.vStartPos = _float3(0.2f, 4.f, -0.1f); // 모델 위치랑 상관있음
	StonePartDesc.vExtense = _float3(0.7f, 0.9f, 0.8f);

	(GI->Add_GameObjectToLayer(TEXT("BeastStonePart"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &StonePartDesc));


	StonePartDesc.vStartPos = _float3(0.f, 2.1f, -0.1f); // 모델 위치랑 상관있음

	(GI->Add_GameObjectToLayer(TEXT("BeastStonePart"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &StonePartDesc));


	StonePartDesc.vStartPos = _float3(-0.8f, 2.f, -0.15f); // 모델 위치랑 상관있음

	(GI->Add_GameObjectToLayer(TEXT("BeastStonePart"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &StonePartDesc));


	//=====================================================================================================8888
	StonePartDesc.iStoneType = 8;

	StonePartDesc.vStartPos = _float3(0.f, 1.2f, 0.05f); // 모델 위치랑 상관있음
	StonePartDesc.vExtense = _float3(0.9f, 0.8f, 0.9f);

	(GI->Add_GameObjectToLayer(TEXT("BeastStonePart"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &StonePartDesc));


	StonePartDesc.vStartPos = _float3(-0.6f, 2.8f, -0.1f); // 모델 위치랑 상관있음

	(GI->Add_GameObjectToLayer(TEXT("BeastStonePart"), LEVEL_STAGE5, TEXT("Layer_BeastObj"), &StonePartDesc));

	
}

CBeastStoneBody * CBeastStoneBody::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBeastStoneBody*		pInstance = new CBeastStoneBody(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBeastStoneBody"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBeastStoneBody::Clone(void * pArg)
{
	CBeastStoneBody*		pInstance = new CBeastStoneBody(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBeastStoneBody"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBeastStoneBody::Free()
{
	__super::Free();

	//for (auto& iter : m_PartALayer)
	//{
	//	Safe_Release(iter);
	//}

	//m_PartALayer.clear();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}
