#include "stdafx.h"
#include "..\Public\TowerRail.h"
#include "GameInstance.h"
#include "HockeyReserve.h"
#include "Player.h"
#include "Creture.h"
#include "UI_Manager.h"
#include "Monster_Manager.h"
#include "ToolManager.h"

CTowerRail::CTowerRail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CTowerRail::CTowerRail(const CTowerRail & rhs)
	: CCreture(rhs)
{
}

HRESULT CTowerRail::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTowerRail::Initialize(void * pArg)
{

	m_CretureInfo = *(CCreture::CREATEINFO*)pArg;
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Boss";


	Make_ImGui();
	//m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION,XMLoadFloat4(&m_CretureInfo.vPosition));
	
	m_pTransformCom->Set_Scale(XMVectorSet(1.f, 1.f, 1.f, 1.f));
	m_fBlurPower = 1.26f;

	return S_OK;
}

void CTowerRail::Tick(_float fTimeDelta)
{
	//m_pTransformCom->Set_TurnSpeed(fTimeDelta);

	//스폰된 몬스터 다 죽으면 색 바꿔주기, 인덱스에 따라서 체크, m_iColor = 0으로 해주기

	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(Turn));

	Turn += fTimeDelta * 50.f;

	if (m_bSpawn)
	{
		m_bSpawn = false;

		//몬스터 스폰
		Spawn_Monster();
	}

	if (!m_bHitAble)
	{
		m_fBlurPower = 1.26f;
	}

	Check_MonsterDeath();

	if (MM->Get_Stage4_5Count() == 6)
	{
		if (!m_bBalling)
		{
			m_bBalling = true;
			UM->Set_FrontUI(false);
			UM->Set_BattleIndex(5);
			UM->Make_BattleEnd();
		}
	}
}

void CTowerRail::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Tick_Col(m_pTransformCom->Get_WorldMatrix());

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	Choose_Col(nullptr);
	Render_Col(m_pRendererCom);

}

HRESULT CTowerRail::Render()
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
	if (FAILED(m_pShaderCom->Set_RawValue("g_ColorType", &m_iColor, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &m_fBlurPower, sizeof(_float))))
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

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 19)))
			return E_FAIL;
	}

	return S_OK;
}

void CTowerRail::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
}

void CTowerRail::On_Hit(CCreture * pOther)
{
	if (m_bHitAble)
	{
		m_bHitAble = false;

		m_bSpawn = true;
		//인덱스 따라서 몬스터 스폰할수있도록
		_float4 vPos;
		XMStoreFloat4(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		vPos.y += 1.f;
		pOther->Create_HitEffect(vPos);

		//GI->PlaySoundW(L"SwordHit.ogg", SD_HIT, 0.5f);
		GI->PlaySoundW(L"se_Env12_Tower_Startup.wav", SD_MAP, 0.4f);
		m_bDeadSound = true;

		m_iColor = 1;

		m_fBlurPower = 1.45f;
	}
}

HRESULT CTowerRail::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_VA_TowerAa"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.7f, 1.7f, 1.7f);
	ColDesc.vCenter = _float3(0.f, 1.6f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Monster_Body1");
	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;


	return S_OK;
}


CTowerRail * CTowerRail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTowerRail*		pInstance = new CTowerRail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTowerRail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCreture * CTowerRail::Clone(void * pArg)
{
	CTowerRail*		pInstance = new CTowerRail(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTowerRail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTowerRail::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}




void CTowerRail::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);

	ImGui::DragFloat("BlurPower ", &m_fBlurPower, 0.01f);


	_float3 vSize = Get_Colliders("Monster_Body1")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Monster_Body1")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Get_Colliders("Monster_Body1")->Set_Size(vSize);
	Get_Colliders("Monster_Body1")->Set_Center(vCenter);



	ImGui::End();
}

void CTowerRail::Check_MonsterDeath()
{
	if (m_bDeadSound)
	{
		GI->PlaySoundW(L"se_Env12_Tower_Disarm.wav", SD_MAP, 0.4f);
		m_bDeadSound = false;
	}

	switch (m_CretureInfo.iIndex)
	{
	case 0: //왼쪽 상단
	{
		if (MM->Get_Stage4_4Count() == 5)
		{
			m_iColor = 0;
			m_fBlurPower = 0.f;

			if (!m_bResult)
			{
				m_bResult = true;
				UM->Set_FrontUI(false);
				UM->Set_BattleIndex(7);
				UM->Make_BattleEnd();
			}
		}
	}
		break;
	case 1: //우상단
		if (MM->Get_Stage4_1Count() == 5)
		{
			m_iColor = 0;
			m_fBlurPower = 0.f;

			if (!m_bResult)
			{
				m_bResult = true;
				UM->Set_FrontUI(false);
				UM->Set_BattleIndex(4);
				UM->Make_BattleEnd();
			}
		}
		break;
	case 2://좌하단
		if (MM->Get_Stage4_3Count() == 6)
		{
			m_iColor = 0;
			m_fBlurPower = 0.f;

			if (!m_bResult)
			{
				m_bResult = true;
				UM->Set_FrontUI(false);
				UM->Set_BattleIndex(6);
				UM->Make_BattleEnd();
			}
		}
		break;
	default:
		break;
	}


}

void CTowerRail::Spawn_Monster()
{
	switch (m_CretureInfo.iIndex)
	{
	case 0: //왼쪽 상단
		MM->Spawn_Stage4_4();
		break;
	case 1: //우상단
		MM->Spawn_Stage4_1();
		break;
	case 2://좌하단
		MM->Spawn_Stage4_3();
		break;
	case 3://볼링장
		MM->Spawn_Stage4_5();
		break;
	default:
		break;
	}
}
