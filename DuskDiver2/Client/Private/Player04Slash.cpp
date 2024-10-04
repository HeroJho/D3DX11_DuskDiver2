#include "stdafx.h"
#include "..\Public\Player04Slash.h"
#include "GameInstance.h"
#include "Time_Manager.h"
#include "Camera_Manager.h"
#include "Status_Manager.h"

CPlayer04Slash::CPlayer04Slash(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCreture(pDevice, pContext)
{
}

CPlayer04Slash::CPlayer04Slash(const CPlayer04Slash& rhs)
	: CCreture(rhs)
{
}

HRESULT CPlayer04Slash::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer04Slash::Initialize(void * pArg)
{
	m_pSlashInfo = (*(SLASHINFO**)pArg);

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_bDead = false;

	m_pTransformCom->Set_WorldMatrix(m_pSlashInfo->Worldmatrix);
	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pSlashInfo->vScale));
	m_pTransformCom2->Set_WorldMatrix(m_pSlashInfo->Worldmatrix);
	m_pTransformCom2->Set_Scale(XMLoadFloat3(&m_pSlashInfo->vScale));

	m_fMaxUVIndexX = 0.f;
	m_fMaxUVIndexY = 0.f;
	m_vUVFixAcc = { 0.f,0.f };
	m_vUVFixSpeed = { 0.f,0.f };
	XMStoreFloat3(&m_vTargetLook, m_pTransformCom->Get_State(CTransform::STATE_LOOK));
	m_eHitType = TYPE_SPIN;
	m_fPushPower = 6.f;
	m_fAirPower = 2.f;
	m_fDamage = STM->Get_FinalStr(CStatus_Manager::PLAYER04) * 1.5f;
	return S_OK;
}

void CPlayer04Slash::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_EFFECT, this);
	fTimeDelta *= m_fTMRatio;
	Emp_Col();
	m_pTransformCom2->Go_Dir(XMVector3Normalize(XMLoadFloat3(&m_vTargetLook)), 30.f, fTimeDelta);
	m_pTransformCom2->Turn(_vector{ 0.f,1.f,0.f }, 25.f, fTimeDelta);
	
	if (!m_bAttackCol)
	{
		m_fColAcc += 1.f * fTimeDelta;
		if (m_fColAcc >= 0.2f)
		{
			m_bAttackCol = true;
			m_fColAcc = 0.f;
		}
	}

	m_fLifeTimeAcc += 1.f * fTimeDelta;
	if (m_fLifeTimeAcc >= 2.f)
	{
		m_vRatio.x -= 5.f * fTimeDelta;
		m_vRatio.y -= 5.f * fTimeDelta;
		if (m_vRatio.x <= 0.f)
			Set_Dead();
	}
}

void CPlayer04Slash::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;
	Tick_Col(m_pTransformCom2->Get_WorldMatrix());
	if (m_bAttackCol)
	{
		Choose_Col("Player_Attack");
	}
	Compute_CamZ(m_pTransformCom2->Get_State(CTransform::STATE_POSITION));
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);	
	Render_Col(m_pRendererCom);

}

HRESULT CPlayer04Slash::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom2->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_vColor1, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_vColor2", &m_vColor2, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio1", &m_vRatio.x, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio2", &m_vRatio.y, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fBlurPower", &m_fBlurPower, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fMaxUVIndexX", &m_fMaxUVIndexX, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fMaxUVIndexY", &m_fMaxUVIndexY, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixX", &m_vUVFixAcc.x, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixY", &m_vUVFixAcc.y, sizeof(_float))))
			return E_FAIL;
		if (FAILED(m_pModelCom->Render(m_pShaderCom,i, 2)))
			return E_FAIL;
	}

	return S_OK;
}

void CPlayer04Slash::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());
	_float3 vSize = Get_Colliders("Player_Attack")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Player_Attack")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);

	Get_Colliders("Player_Attack")->Set_Size(vSize);
	Get_Colliders("Player_Attack")->Set_Center(vCenter);

	ImGui::End();
}

void CPlayer04Slash::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Monster" == Desc.pOther->Get_Tag())
	{
		// 내 OBB와 상대편의 OBB와 충돌 했다면

		if ((!strcmp("Player_Attack", Desc.pMyDesc->sTag) &&
			!strcmp("Monster_Body1", Desc.pOtherDesc->sTag)))
		{		
			((CCreture*)Desc.pOther)->On_Hit(this);
			CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
			m_bAttackCol = false;
			m_fColAcc = 0.f;
			
		}
	}
}

HRESULT CPlayer04Slash::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Player04MeshEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Player04Slash"), TEXT("Player04Slash"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform2"), (CComponent**)&m_pTransformCom2)))
		return E_FAIL;

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));
	ColDesc.vCenter = _float3(0.f, 0.5f, 0.f);
	ColDesc.vSize = _float3(6.f, 0.f, 0.f);
	ColDesc.pOwner = this;
	strcpy(ColDesc.sTag, "Player_Attack");

	if (FAILED(AddCollider(CCollider::TYPE_SPHERE, ColDesc)))
		return E_FAIL;

	return S_OK;
}

CCreture * CPlayer04Slash::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer04Slash*		pInstance = new CPlayer04Slash(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer04Slash"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer04Slash::Clone(void * pArg)
{
	CPlayer04Slash*		pInstance = new CPlayer04Slash(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer04Slash"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer04Slash::Free()
{
	__super::Free();
	if(m_pSlashInfo != nullptr)
		Safe_Delete(m_pSlashInfo);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pTransformCom2);
	Safe_Release(m_pShaderCom);
	

}
