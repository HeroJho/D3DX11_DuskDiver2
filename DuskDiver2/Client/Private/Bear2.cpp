#include "stdafx.h"
#include "..\Public\Bear2.h"
#include "GameInstance.h"
#include "HockeyReserve.h"
#include "Player.h"
#include "Creture.h"
#include "UI_Manager.h"
#include "Time_Manager.h"
#include "ToolManager.h"

CBear2::CBear2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CBear2::CBear2(const CBear2 & rhs)
	: CCreture(rhs)
{
}

HRESULT CBear2::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBear2::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Boss";

	Make_ImGui();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-184.46f, 38.7f, 129.894f, 1.f));
	 m_pTransformCom->Set_Scale(XMVectorSet(10.f, 10.f, 10.f, 1.f));
	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.f));

	m_eHitType = TYPE_HIT;
	m_fDamage = 0.f;
	m_fAirPower = 2.f;
	m_fPushPower = 4.f;

	m_fHurtTime = 0.04f;

	m_OriginPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	return S_OK;
}

void CBear2::Tick(_float fTimeDelta)
{

	TIM->Add_TimeObj(CTime_Manager::OBJ_MONSTER, this);

	Check_Hurt(fTimeDelta);
	fTimeDelta *= m_fTMRatio;

	Emp_Col();

	switch (m_BearState)
	{
	case Client::CBear2::STATE_UP:
		Pattern_Up(fTimeDelta);
		break;
	case Client::CBear2::STATE_DOWN:
		Pattern_Down(fTimeDelta);
		break;
	case Client::CBear2::STATE_HIDE:
		Pattern_Hide(fTimeDelta);
		break;
	case Client::CBear2::STATE_HIT:
		Pattern_Hit(fTimeDelta);
		break;
	case Client::CBear2::STATE_END:
		break;
	default:
		break;
	}

	//m_pTransformCom->Set_TurnSpeed(fTimeDelta);

}

void CBear2::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;
	fTimeDelta *= m_fTMRatio;

	Tick_Col(m_pTransformCom->Get_WorldMatrix());


	if (m_bHurt)
		Choose_Col("Monster_Body1");

	if (m_bAttackCol)
		Choose_Col("Monster_Attack");

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	Render_Col(m_pRendererCom);

}

HRESULT CBear2::Render()
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
	_uint iPassIndex = 0;
	_float fBlurPower = 0.f;
	_bool	bDiffuse = false;
	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		// 노말 텍스쳐가 있다면
		if (!FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			iPassIndex = 2;



		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iPassIndex)))
			return E_FAIL;
	}

	return S_OK;
}

void CBear2::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
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
			((CCreture*)Desc.pOther)->On_Hit(this);
		}
	}
}

void CBear2::On_Hit(CCreture * pOther)
{
	if (m_bHurt)
	{
		GI->PlaySoundW(L"SwordHit.ogg", SD_HIT, 1.f);
		m_bRim = true;
		m_fRimAcc = 0.f;
		m_bHurt = false;
		m_bAttackCol = false;

		if (UM->Get_BearType() != 1)
		{
			if (m_bOneHit)
			{
				m_eHitType = TYPE_HIT;
				m_fAirPower = 2.f;
				m_fPushPower = 1.f;

				m_bOneHit = false;
				pOther->On_Hit(this); // 반사 데미지 입히려고 넣은 코드 , 피 채우는거, 흙먼지 날리는거, 시간 초 깎기
				_float4 vPos;
				XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
				vPos.y += 2.f;
				pOther->Create_HitEffect(vPos);

				GI->PlaySoundW(L"SwordHit.ogg", SD_HIT, 1.f);
				m_bRim = true;
				m_fRimAcc = 0.f;

			}
		}
		else
		{
			_float4 vPos;
			XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			vPos.y += 2.f;
			pOther->Create_HitEffect(vPos);
			m_fNowHp -= (pOther->Get_Damage() - m_fStamina);
			UM->Plus_Combo();
			UM->Set_Hit(true);
			UM->Set_Up(true);
		}
	}
}

HRESULT CBear2::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_PR_BearAa"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.3f, 0.3f, 0.3f);
	ColDesc.vCenter = _float3(0.f, 0.2f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Body1");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(0.25f, 0.2f, 0.2f);
	ColDesc.vCenter = _float3(0.f, 0.2f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Attack");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	return S_OK;
}

CBear2 * CBear2::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBear2*		pInstance = new CBear2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBear2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCreture * CBear2::Clone(void * pArg)
{
	CBear2*		pInstance = new CBear2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBear2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBear2::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}




void CBear2::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);

	_float3 vSize = Get_Colliders("Monster_Body1")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Monster_Body1")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Get_Colliders("Monster_Body1")->Set_Size(vSize);
	Get_Colliders("Monster_Body1")->Set_Center(vCenter);



	ImGui::End();
}

void CBear2::Pattern_Up(_float fTimeDelta)
{

	if (m_bAttackStart)
	{
		m_bAttackStart = false;
		m_bOneHit = true;
		m_bAttackCol = true;
	}

	if (m_pTransformCom->Move(XMVectorSet(XMVectorGetX(m_Pos), XMVectorGetY(m_Pos) + 4.f, XMVectorGetZ(m_Pos), 1.f), 7.f, fTimeDelta, 0.2f))
	{
		m_BearState = STATE_HIT;
		m_bAttackCol = false;
	}
}

void CBear2::Pattern_Down(_float fTimeDelta)
{
	if (m_pTransformCom->Move(XMVectorSet(XMVectorGetX(m_Pos), XMVectorGetY(m_Pos), XMVectorGetZ(m_Pos), 1.f), 7.f, fTimeDelta, 0.2f))
	{
		m_BearState = STATE_HIDE;
		UM->BearOff(m_iNum);
	}
}

void CBear2::Pattern_Hit(_float fTimeDelta)
{
	m_fHitTime += fTimeDelta;
	if (m_fHitTime > 3.f)
	{
		m_BearState = STATE_DOWN;
		m_fHitTime = 0.f;
		m_bAttackStart = true;
	}
}

void CBear2::Pattern_Hide(_float fTimeDelta)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (!m_bRandHide)
	{
		m_iRandHide = GI->Get_Random(2, 5);
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
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-206.46f, 34.7f, 125.7f, 1.f));
			m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(270.f));
			break;
		case 1:
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-202.87f, 34.7f, 131.9f, 1.f));
			m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(270.f));
			break;
		case 2:
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-199.31f, 34.7f, 138.68f, 1.f));
			m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(330.f));
			break;
		case 3:
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-195.44f, 34.7f, 145.178f, 1.f));
			m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(330.f));
			break;
		case 4:
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-199.35f, 34.7f, 121.656f, 1.f));
			m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(270.f));
			break;
		case 5:
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-195.69f, 34.7f, 127.597f, 1.f));
			m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(270.f));
			break;
		case 6:
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-191.85f, 34.7f, 134.234f, 1.f));
			m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(330.f));
			break;
		case 7:
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-188.1f, 34.7f, 140.8f, 1.f));
			m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(330.f));
			break;
		case 8:
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-180.86f, 34.7f, 136.778f, 1.f));
			m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(330.f));
			break;
		case 9:
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-184.46f, 34.7f, 129.894f, 1.f));
			m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(330.f));
			break;
		case 10:
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-188.6f, 34.7f, 123.665f, 1.f));
			m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(270.f));
			break;
		case 11:
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-192.29f, 34.7f, 117.568f, 1.f));
			m_Pos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(270.f));
			break;
		default:
			break;
		}
	}

	RELEASE_INSTANCE(CGameInstance);


	m_fHideTime += fTimeDelta;
	if (m_fHideTime > m_iRandHide)
	{
		m_BearState = STATE_UP;
		m_fHideTime = 0.f;
		m_bRandHide = false;
	}
}
