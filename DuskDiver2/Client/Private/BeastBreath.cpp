#include "stdafx.h"
#include "..\Public\BeastBreath.h"
#include "GameInstance.h"
#include "HockeyReserve.h"
#include "Player.h"
#include "Creture.h"
#include "UI_Manager.h"
#include "Time_Manager.h"
#include "ToolManager.h"
#include "BeastStoneBody.h"
#include "Effect_Manager.h"
#include "Monster_Manager.h"

CBeastBreath::CBeastBreath(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CCreture(pDevice, pContext)
{
}

CBeastBreath::CBeastBreath(const CBeastBreath & rhs)
	: CCreture(rhs)
{
}

HRESULT CBeastBreath::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBeastBreath::Initialize(void * pArg)
{
	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	m_CretureInfo = *(CCreture::CREATEINFO*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Boss";
	Make_ImGui();

	m_eHitType = TYPE_AWAY;

	m_fPushPower = 1.f;
	m_fAirPower = 1.f;
	m_fDamage = MM->Get_BreathDamage();

	return S_OK;
}

void CBeastBreath::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_MONSTER, this);
	fTimeDelta *= m_fTMRatio;
	Emp_Col();

	if (m_bRender)
	{
		
		if (m_bColDelay) //충돌 텀 주기
		{
			m_bPlayerHit = false;
			m_fColDelayTime += 0.1f * fTimeDelta;

			if (m_fColDelayTime > 0.4f)
			{
				m_fColDelayTime = 0.f;
				m_bPlayerHit = true;
				m_bColDelay = false;
			}
		}

		m_vUVFixAcc.x += m_vUVFixSpeed.x * fTimeDelta;
		m_vUVFixAcc.y += m_vUVFixSpeed.y * fTimeDelta;

		m_vUVFixAcc_2.x += m_vUVFixSpeed_2.x * fTimeDelta;
		m_vUVFixAcc_2.y += m_vUVFixSpeed_2.y * fTimeDelta;		
	}

	
	if (m_bRatioFix)
	{
		switch (m_eBreathType)
		{
		case Client::CBeastBreath::BREATH_NORMAL:
			m_vRatio.x -= 2.f * fTimeDelta;
			if (m_vRatio.x <= 0.f)
				m_vRatio.x = 0.f;
			m_vRatio.y -= 2.f * fTimeDelta;
			if (m_vRatio.y <= 0.f)
				m_vRatio.y = 0.f;
			m_vRatio_2.x -= 2.f * fTimeDelta;
			if (m_vRatio_2.x <= 0.f)
				m_vRatio_2.x = 0.f;
			m_vRatio_2.y -= 2.f * fTimeDelta;
			if (m_vRatio_2.y <= 0.f)
				m_vRatio_2.y = 0.f;
			break;
		case Client::CBeastBreath::BREATH_RED:
			m_vRatio.x -= 20.f * fTimeDelta;
			if (m_vRatio.x <= 0.f)
				m_vRatio.x = 0.f;
			m_vRatio.y -= 80.f * fTimeDelta;
			if (m_vRatio.y <= 0.f)
				m_vRatio.y = 0.f;
			m_vRatio_2.x -= 20.f * fTimeDelta;
			if (m_vRatio_2.x <= 0.f)
				m_vRatio_2.x = 0.f;
			m_vRatio_2.y -= 20.f * fTimeDelta;
			if (m_vRatio_2.y <= 0.f)
				m_vRatio_2.y = 0.f;
			break;
		case Client::CBeastBreath::BREATH_WHITE:
			m_vRatio.x -= 4.f * fTimeDelta;
			if (m_vRatio.x <= 0.f)
				m_vRatio.x = 0.f;
			m_vRatio.y -= 4.f * fTimeDelta;
			if (m_vRatio.y <= 0.f)
				m_vRatio.y = 0.f;
			m_vRatio_2.x -= 4.f * fTimeDelta;
			if (m_vRatio_2.x <= 0.f)
				m_vRatio_2.x = 0.f;
			m_vRatio_2.y -= 4.f * fTimeDelta;
			if (m_vRatio_2.y <= 0.f)
				m_vRatio_2.y = 0.f;
			break;
	
		}

	

		if (m_vRatio.x <= 0.f && m_vRatio.y <= 0.f && m_vRatio_2.x <= 0.f && m_vRatio_2.y <= 0.f)
		{
			m_bRatioFix = false;
			m_bRender = false;
		}
	}
	
}

void CBeastBreath::LateTick(_float fTimeDelta)
{	
	if (nullptr == m_pRendererCom)
		return;

	Tick_Col(m_pTransformCom->Get_WorldMatrix_NoScale());

	if (m_bRender) // 렌더 키고 끄기
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
	
	if(m_bAttackCol)
		Choose_Col("Breath_Attack");
	
	Choose_Col("Breath_Dist");

	Render_Col(m_pRendererCom);	

}

HRESULT CBeastBreath::Render()
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
	if (FAILED(m_pShaderCom->Set_RawValue("g_fBlurPower", &m_fBlurPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_vColor1, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor2", &m_vColor2, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio1", &m_vRatio.x, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio2", &m_vRatio.y, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixX", &m_vUVFixAcc.x, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixY", &m_vUVFixAcc.y, sizeof(_float))))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 7)))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_fBlurPower", &m_fBlurPower_2, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_vColor1_2, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor2", &m_vColor2_2, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio1", &m_vRatio_2.x, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio2", &m_vRatio_2.y, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixX", &m_vUVFixAcc_2.x, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixY", &m_vUVFixAcc_2.y, sizeof(_float))))
		return E_FAIL;

	_uint		iNumMeshes2 = m_pBreathCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes2; ++i)
	{
		if (FAILED(m_pBreathCom->SetUp_OnShader(m_pShaderCom, m_pBreathCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pBreathCom->Render(m_pShaderCom, i, 7)))
			return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CBeastBreath::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	//종류 상관없이 플레이어와 충돌은 필요함
	//빨간색 브레스일때만 돌기둥 충돌

	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{
		if (!strcmp("Breath_Attack", Desc.pMyDesc->sTag) && !strcmp("Player_Body", Desc.pOtherDesc->sTag))
		{
			m_bColDelay = true;
			m_fDamage = MM->Get_BreathDamage();
			if(m_bPlayerHit)
				((CCreture*)Desc.pOther)->On_Hit(this);
		}
	}

	if ("Tag_Boss" == Desc.pOther->Get_Tag())
	{
		// 내 손 Sphere 둘중 하나와 상대편의 OBB와 충돌 했다면
		if (!strcmp("Breath_Dist", Desc.pMyDesc->sTag) && !strcmp("Stone_Shield", Desc.pOtherDesc->sTag))
		{
			//어택 타입
			m_bColStone = true;
			m_pColCreture = (CCreture*)Desc.pOther;

		}

		if (!strcmp("Breath_Attack", Desc.pMyDesc->sTag) && !strcmp("Stone_Shield", Desc.pOtherDesc->sTag))
		{
			//어택 타입
			((CBeastStoneBody*)Desc.pOther)->On_Heating(this);
		}
	}

}

HRESULT CBeastBreath::SetUp_State(_fmatrix StateMatrix)
{
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, StateMatrix.r[0]);
	m_pTransformCom->Set_State(CTransform::STATE_UP, StateMatrix.r[1]);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, StateMatrix.r[2]);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, StateMatrix.r[3]);
	return S_OK;
}

void CBeastBreath::Change_ColRange(_float3 vSize, _float3 vCenter)
{
	Get_Colliders("Breath_Attack")->Set_Size(vSize);
	Get_Colliders("Breath_Attack")->Set_Center(vCenter);
}

void CBeastBreath::Change_Size(_float fSize)
{
	m_fSize = fSize;
	m_pTransformCom->Set_Scale(XMVectorSet(m_vScale.x, m_fSize, m_vScale.z, 1.f));
}

void CBeastBreath::Change_BreathSize()
{
	if (m_pColCreture != nullptr)
	{
		_vector vTargetPos = XMLoadFloat4(&m_pColCreture->Get_WorldPos());

		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

		_vector vTargetLook = vTargetPos - vPosition;
		_float fDist = XMVectorGetX(XMVector3Length(vTargetLook));
		m_fBreathDistance = fDist;

		m_pTransformCom->Set_Scale(XMVectorSet(1.f, (m_fBreathDistance / 2.2f), 1.f, 1.f));

		Change_ColRange(_float3{ 0.9f,(m_fBreathDistance / 4.f) * 2.f, 0.9f }, _float3{ 0.f, (m_fBreathDistance / 4.f) * 2.f, 0.f });
	}
	
}

void CBeastBreath::PlayerHit_On()
{
	m_fColDelayTime = 0.f;
	m_bPlayerHit = true;
	m_bColDelay = false;
}

void CBeastBreath::Set_BreathInfo(BREATHINFO Info)
{
	m_vColor1 = Info.vColor1;
	m_vColor1_2 = Info.vColor1_2;
	m_vRatio = Info.vRatio;
	m_fBlurPower = Info.fBlurPower;
	m_vUVFixAcc = Info.vUVFixAcc;
	m_vUVFixSpeed = Info.vUVFixSpeed;

	m_vColor2 = Info.vColor2;
	m_vColor2_2 = Info.vColor2_2;
	m_vRatio_2 = Info.vRatio_2;
	m_fBlurPower_2 = Info.fBlurPower_2;
	m_vUVFixAcc_2 = Info.vUVFixAcc_2;
	m_vUVFixSpeed_2 = Info.vUVFixSpeed_2;
	m_eBreathType = Info.eBreathType;

	m_vScale = Info.vScale;


}

HRESULT CBeastBreath::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Player01MeshEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Breath"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Breath"), TEXT("Com_Model2"), (CComponent**)&m_pBreathCom)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.1f, 1.f, 1.1f);
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Breath_Attack"); //플레이어랑 충돌처리 필요, 돌멩이랑도 충돌처리 필요 상대편의 태그 tag_Player, tag_boss의 stone_shield랑
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;



	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(1.1f, 100.f, 1.1f);
	ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	strcpy(ColDesc.sTag, "Breath_Dist"); //플레이어랑 충돌처리 필요, 돌멩이랑도 충돌처리 필요 상대편의 태그 tag_Player, tag_boss의 stone_shield랑
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;

	return S_OK;
}


CBeastBreath * CBeastBreath::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBeastBreath*		pInstance = new CBeastBreath(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBeastBreath"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CCreture * CBeastBreath::Clone(void * pArg)
{
	CBeastBreath*		pInstance = new CBeastBreath(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBeastBreath"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBeastBreath::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pColCreture);
	Safe_Release(m_pBreathCom);

}




void CBeastBreath::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);

	ImGui::Text("Breathdistance : %f", m_fBreathDistance);

	_float3 vSize = Get_Colliders("Breath_Attack")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Breath_Attack")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	Get_Colliders("Breath_Attack")->Set_Size(vSize);
	Get_Colliders("Breath_Attack")->Set_Center(vCenter);

	ImGui::End();
}
