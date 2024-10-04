#include "stdafx.h"
#include "..\Public\Bear1.h"
#include "GameInstance.h"
#include "HockeyReserve.h"
#include "Player.h"
#include "Creture.h"
#include "UI_Manager.h"
#include "Time_Manager.h"
#include "ToolManager.h"
#include "Particle_Manager.h"
#include "Player01HitEffect_1.h"
#include "Player01HitEffect_2.h"
#include "Player_Manager.h"

CBear1::CBear1(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CBear1::CBear1(const CBear1 & rhs)
	: CCreture(rhs)
{
}

HRESULT CBear1::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBear1::Initialize(void * pArg)
{

	m_CretureInfo = *(CCreture::CREATEINFO*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Boss";

	Make_ImGui();

	m_eHitType = TYPE_HIT;
	m_fAirPower = 2.f;
	m_fPushPower = 4.f;
	m_fDamage = 300.f;
	m_fStamina = 1.f;
	m_fHurtTime = 0.07f;

	//모델 회전

	//베어3 인덱스2, 베어4 인덱스3
	if(m_CretureInfo.iIndex == 2 || m_CretureInfo.iIndex == 3)
		m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(270.f));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_CretureInfo.vPosition));
	m_pTransformCom->Set_Scale(XMVectorSet(10.f, 10.f, 10.f, 1.f));
	m_OriginPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	return S_OK;
}

void CBear1::Tick(_float fTimeDelta)
{
	if (m_bRender)
	{
		TIM->Add_TimeObj(CTime_Manager::OBJ_MONSTER, this);

		Check_Hurt(fTimeDelta);
		Check_Rim(fTimeDelta);
		fTimeDelta *= m_fTMRatio;
		Emp_Col();

		LookAtPlayer();
		if (m_bDeadOn)
		{
			Set_Dead();
			//이펙트 생성
		}

		if (m_bCheck)
		{
			if (m_bFalseBear)
			{
				ShakeBear();
			}
			else
			{
				UM->Plus_Combo();
				UM->Set_Hit(true);
				UM->Set_Up(true);
			}
			m_bCheck = false;
		}
			
		Check_ShakeBear(fTimeDelta);
		
		if (m_fNowHp < 0.f)
			m_fNowHp = 0.f;

		switch (m_BearState)
		{
		case Client::CBear1::STATE_UP:
			Pattern_Up(fTimeDelta);
			break;
		case Client::CBear1::STATE_DOWN:
			Pattern_Down(fTimeDelta);
			break;
		case Client::CBear1::STATE_HIDE:
			Pattern_Hide(fTimeDelta);
			break;
		case Client::CBear1::STATE_HIT:
			Pattern_Hit(fTimeDelta);
			break;
		case Client::CBear1::STATE_END:
			break;
		default:
			break;
		}

		m_pTransformCom->Set_TurnSpeed(fTimeDelta);
	}
	else
	{
		UM->BearHunt_Off();
	}

}

void CBear1::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;
	fTimeDelta *= m_fTMRatio;

	Tick_Col(m_pTransformCom->Get_WorldMatrix_NoScale());

	if (m_bHurt)
		Choose_Col("Monster_Body1");

	if (m_bAttackCol)
		Choose_Col("Monster_Attack");
	
	Choose_Col("Monster_Push_Range");

	if(m_bRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	Render_Col(m_pRendererCom);
}

HRESULT CBear1::Render()
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

	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &m_fBlurPower, sizeof(_float))))
		return E_FAIL;

	_float fRimPower = 3.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_RimPower", &fRimPower, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_RimColor", &m_fRimColor, sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);


	_float fGama = 1.8f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	_bool	bDiffuse = false;
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;


		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, m_iShaderPath)))
			return E_FAIL;
	}

	return S_OK;
}

void CBear1::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		// 내 손 Sphere 둘중 하나와 상대편의 OBB와 충돌 했다면
		if (!strcmp("Monster_Attack", Desc.pMyDesc->sTag) && !strcmp("Player_Body", Desc.pOtherDesc->sTag))
		{
			m_bAttackCol = false;

			m_eHitType = TYPE_SPIN;
			m_fAirPower = 5.f;
			m_fPushPower = 1.5f;
			m_fDamage = 300.f;
			((CCreture*)Desc.pOther)->On_Hit(this);
		}
		else if (!strcmp("Monster_Body1", Desc.pMyDesc->sTag) && !strcmp("Player_Attack", Desc.pOtherDesc->sTag))
		{
			m_bAttackCol = false;
			m_bRim = true;
			m_bCheck = true;
			//플레이어 공격과 내 몸이 충돌했을때
			if (UM->Get_BearType() != m_CretureInfo.iIndex) //충돌된 애가 밑에있는 사진이랑 다른 경우
			{
				m_bFalseBear = true;
				m_eHitType = TYPE_HIT;
				m_fAirPower = 2.f;
				m_fPushPower = 1.f;
				m_fRimLimitTime = 1.f;
				ReflectPlayer();
				((CCreture*)Desc.pOther)->On_Hit(this);
				GI->PlaySoundW(L"HUSEIKAI.wav", SD_NPC, 1.f);
			}
			else
			{
				m_bFalseBear = false;
				m_fRimLimitTime = 0.2f;
				//쿠마 hp깎는 함수 필요
				// TM->Deal_CumaHp(10000.1f);
				TM->Deal_CumaHp(300.1f);
				GI->PlaySoundW(L"HUSEIKAI_On.wav", SD_NPC, 1.f);
			}
		}
	}
	else if ("Tag_Monster" == Desc.pOther->Get_Tag())
	{
		// 내 손 Sphere 둘중 하나와 상대편의 OBB와 충돌 했다면
		if (!strcmp("Monster_Attack", Desc.pMyDesc->sTag) && !strcmp("Monster_Body1", Desc.pOtherDesc->sTag))
		{
			m_bAttackCol = false;

			m_eHitType = TYPE_SPIN;
			m_fAirPower = 5.f;
			m_fPushPower = 1.5f;
			m_fDamage = 300.f;
			((CCreture*)Desc.pOther)->On_Hit(this);
		}
	}


}

void CBear1::On_Hit(CCreture * pOther)
{
	if (m_bHurt && m_bHitAble)
	{
		GI->PlaySoundW(L"SwordHit.ogg", SD_HIT, 1.f);
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		vPos.y += 2.f;
		pOther->Create_HitEffect(vPos);

		m_bHurt = false;
		m_bAttackCol = false;
		
	}
}

HRESULT CBear1::Ready_Components()
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));
	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */

	switch (m_CretureInfo.iIndex)
	{
	case 0:
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_PR_BearA"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 1:

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_PR_BearAa"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 2:

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_PR_BearAc"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	case 3:

		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_PR_FakeBearA"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
			return E_FAIL;
		break;
	default:
		break;
	}

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	if (m_CretureInfo.iIndex == 3)
	{
		ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
		ColDesc.vSize = _float3(1.6f, 1.6f, 1.6f);
		ColDesc.vCenter = _float3(0.f, 1.6f, 0.f);
		ColDesc.pOwner = this;
		ColDesc.iFlag = 2;
		strcpy(ColDesc.sTag, "Monster_Body1");
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;

		strcpy(ColDesc.sTag, "Monster_Attack");
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;

		ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
		ColDesc.vSize = _float3(1.5f, 1.5f, 1.5f);
		ColDesc.vCenter = _float3(0.f, 0.75f, 0.f);
		ColDesc.pOwner = this;
		ColDesc.iFlag = 2;
		strcpy(ColDesc.sTag, "Monster_Push_Range"); // 밀어내기 위함

		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;
	}
	else
	{
		ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
		ColDesc.vSize = _float3(1.8f, 1.8f, 1.8f);
		ColDesc.vCenter = _float3(0.f, 1.8f, 0.f);
		ColDesc.pOwner = this;
		ColDesc.iFlag = 2;
		strcpy(ColDesc.sTag, "Monster_Body1");
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;

		strcpy(ColDesc.sTag, "Monster_Attack");
		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;

		ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
		ColDesc.vSize = _float3(1.9f, 1.9f, 1.9f);
		ColDesc.vCenter = _float3(0.f, 0.95f, 0.f);
		ColDesc.pOwner = this;
		ColDesc.iFlag = 2;
		strcpy(ColDesc.sTag, "Monster_Push_Range"); // 밀어내기 위함

		if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
			return E_FAIL;
	}


	return S_OK;
}

CBear1 * CBear1::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBear1*		pInstance = new CBear1(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBear1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCreture * CBear1::Clone(void * pArg)
{
	CBear1*		pInstance = new CBear1(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBear1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBear1::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pColPlayer);

}




void CBear1::Render_Debug()
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

void CBear1::Create_HitEffect(_float4 vWorldPos)
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
	HitEffect->vColor2 = { 1.f,0.f,0.f,1.f };
	GI->Add_GameObjectToLayer(L"Player01HitEffect_1", GI->Get_Cur_LevelIndex(), L"Layer_PlayerEffect", &HitEffect);



	_matrix Matrix = XMMatrixIdentity();
	Matrix.r[3] = XMLoadFloat4(&vPos);
	_float4x4 Matrix2;
	XMStoreFloat4x4(&Matrix2, Matrix);
	PTM->Create_InstanceParticle(L"Player01Hit1", Matrix2);
	PTM->Create_InstanceParticle(L"Player01Hit1_2", Matrix2);
}

void CBear1::AllDead_Monster()
{
	for (auto& iter : m_pMonster)
	{
		iter->Set_NowHp(0.f);
	}

	
}

void CBear1::Check_Rim(_float fTimeDelta)
{
	if (m_bRim)
	{
		m_fRimAcc += 1.f * fTimeDelta;

		if (m_fRimAcc > m_fRimLimitTime)
		{
			m_bRim = false;
			m_fRimAcc = 0.f;
		}
	}

	if (m_bRim)
	{
		m_fBlurPower = 0.35f;

		m_iShaderPath = 1;

		if (m_bFalseBear)
			m_fRimColor = _float4(1.f, 0.047f, 0.f, 1.f);
		else
			m_fRimColor = _float4(0.f, 0.635f, 0.909f, 1.f);

	}
	else
	{
		m_fBlurPower = 0.f;
		m_iShaderPath = 0;
	}
}

void CBear1::CreateMonster()
{
	CCreture::CREATEINFO _tInfo;
	ZeroMemory(&_tInfo, sizeof(CCreture::CREATEINFO));

	_tInfo.iNaviNum = 4;
	_tInfo.iIndex = 10;

	XMStoreFloat4(&_tInfo.vPosition, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	
	(GI->Add_GameObjectToLayer(TEXT("DisasterFusion"), LEVEL_STAGE4, TEXT("Layer_BearMonster"), &_tInfo));
}

void CBear1::ReflectPlayer()
{
	switch (m_CretureInfo.iIndex)
	{
	case 0:
		m_fDamage = 300.f;
		break;
	case 1: 
		m_fDamage = 300.f;
		break;
	case 2: //몬스터 스폰 //tick에서 해야됨
		m_bMonsterSpawn = true;
		m_fDamage = 300.f;
		break;
	case 3: 
		m_fDamage = 300.f;
		m_bCreateEffect = true;
		break;
	default:
		break;
	}

}

void CBear1::Check_ShakeBear(_float fTimeDelta)
{
	if (m_bShake)
	{
		m_fShakeTimeAcc += 1.f * fTimeDelta;

		if (m_fShakeTime < m_fShakeTimeAcc)
		{
			m_fShakeTimeAcc = 0.f;
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vPrePos));
			m_bShake = false;
			m_BearState = STATE_DOWN;
			m_fHitTime = 0.f;

			if (m_bMonsterSpawn)
			{
				m_bMonsterSpawn = false;
				m_bCreateMonster = true;
				CreateMonster();
			}

			if (m_bCreateEffect)
			{
				m_bCreateEffect = false;

				//이펙트 생성하세요
				UM->Make_Ink();
			}

		}

		_vector vBearPos = XMLoadFloat4(&m_vPrePos);

		_float fTempX = GI->Get_FloatRandom(-0.1f, 0.1f);
		_float fTempZ = GI->Get_FloatRandom(-0.1f, 0.1f);

		vBearPos = XMVectorSetX(vBearPos, XMVectorGetX(vBearPos) + fTempX);
		vBearPos = XMVectorSetZ(vBearPos, XMVectorGetZ(vBearPos) + fTempZ);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vBearPos);
	}
}


void CBear1::ShakeBear()
{
	m_bShake = true;
	XMStoreFloat4(&m_vPrePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_fShakeTime = 0.35f;
}

void CBear1::Pattern_Up(_float fTimeDelta)
{

	if (m_pTransformCom->Move(XMVectorSet(XMVectorGetX(m_Pos), XMVectorGetY(m_Pos) + 4.f, XMVectorGetZ(m_Pos), 1.f), 7.f, fTimeDelta, 0.2f))
	{
		m_BearState = STATE_HIT;
		m_bAttackCol = false;
		m_bDown = false;

		if (!m_bUp)
		{
			_uint iRandom = GI->Get_Random(0, 2);
			switch (iRandom)
			{
			case 0:
				GI->PlaySoundW(L"se_Env12_Mole_Up1.wav", SD_TRAININGBOT2, 0.8f);
				break;
			case 1:
				GI->PlaySoundW(L"se_Env12_Mole_Up2.wav", SD_TRAININGBOT2, 0.8f);
				break;
			case 2:
				GI->PlaySoundW(L"se_Env12_Mole_Up3.wav", SD_TRAININGBOT2, 0.8f);
				break;
			}
			m_bUp = true;
		}
	}
}


void CBear1::Pattern_Down(_float fTimeDelta)
{
	if (m_pTransformCom->Move(XMVectorSet(XMVectorGetX(m_Pos), XMVectorGetY(m_Pos), XMVectorGetZ(m_Pos), 1.f), 7.f, fTimeDelta, 0.2f))
	{
		m_BearState = STATE_HIDE;
		UM->BearOff(m_iNum);
		m_bUp = false;

		if (!m_bDown)
		{
			_uint iRandom = GI->Get_Random(0, 1);
			switch (iRandom)
			{
			case 0:
				GI->PlaySoundW(L"se_Env12_Mole_Down1.wav", SD_TRAININGBOT1, 0.8f);
				break;
			case 1:
				GI->PlaySoundW(L"se_Env12_Mole_Down2.wav", SD_TRAININGBOT1, 0.8f);
				break;
			}
			m_bDown = true;
		}
	}
}

void CBear1::Pattern_Hit(_float fTimeDelta)
{
	m_fHitTime += 1.f * fTimeDelta;

	if (m_fHitTime > 7.f) 
	{
		m_BearState = STATE_DOWN;
		m_fHitTime = 0.f;
	}
}

void CBear1::Pattern_Hide(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (!m_bRandHide)
	{
		m_iRandHide = GI->Get_Random(2, 8);
		m_bRandHide = true;
		while (true)
		{
			m_iNum = GI->Get_Random(0, 11);
			if (!UM->Get_Bear(m_iNum))
				break;
		}

		UM->BearOn(m_iNum);
		switch (m_iNum)
		{
		case 0:
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-206.299f, 34.7f, 125.638f, 1.f));
			m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(270.f));
			break;
		case 1:
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-202.992f, 34.7f, 131.880f, 1.f));
			m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(270.f));
			break;
		case 2:
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-199.228f, 34.7f, 138.422f, 1.f));
			m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(330.f));
			break;
		case 3:
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-195.399f, 34.7f, 145.038f, 1.f));
			m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(330.f));
			break;
		case 4:
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-199.f, 34.7f, 121.538f, 1.f));
			m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(270.f));
			break;
		case 5:
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-195.499f, 34.7f, 127.638f, 1.f));
			m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(270.f));
			break;
		case 6:
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-191.904f, 34.7f, 134.107f, 1.f));
			m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(330.f));
			break;
		case 7:
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-188.051f, 34.7f, 140.833f, 1.f));
			m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(330.f));
			break;
		case 8:
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-180.899f, 34.7f, 136.638f, 1.f));
			m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(330.f));
			break;
		case 9:
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-184.899f, 34.7f, 129.938f, 1.f));
			m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(330.f));
			break;
		case 10:
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-188.540f, 34.7f, 123.563f, 1.f));
			m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(270.f));
			break;
		case 11:
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-192.139f, 34.7f, 117.390f, 1.f));
			m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(270.f));
			break;
		default:
			break;
		}
	}

	RELEASE_INSTANCE(CGameInstance);


	m_fHideTime += 1.f *  fTimeDelta;
	if (m_fHideTime > m_iRandHide)
	{
		m_BearState = STATE_UP;
		m_fHideTime = 0.f;
		m_bRandHide = false;
		m_bAttackCol = true;
	}
}

void CBear1::LookAtPlayer()
{
	_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_POSITION) - XMLoadFloat4(&PM->Get_PlayerPos()));

	XMStoreFloat3(&m_vTargetLook, vLook);
	m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMLoadFloat3(&m_vTargetLook), 0.13f);
}
