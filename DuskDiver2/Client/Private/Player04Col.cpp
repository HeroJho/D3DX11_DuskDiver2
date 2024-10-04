#include "stdafx.h"
#include "..\Public\Player04Col.h"
#include "GameInstance.h"
#include "Time_Manager.h"
#include "Camera_Manager.h"
#include "ToolManager.h"

CPlayer04Col::CPlayer04Col(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CCreture(pDevice, pContext)
{
}

CPlayer04Col::CPlayer04Col(const CPlayer04Col& rhs)
	: CCreture(rhs)
{
}

HRESULT CPlayer04Col::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer04Col::Initialize(void * pArg)
{
	m_pInfo = (*(COLINFO**)pArg);

	if (FAILED(__super::Ready_Components()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_bDead = false;

	Make_ImGui();

	_matrix Matrix = XMMatrixIdentity();
	Matrix.r[3] = XMLoadFloat4(&m_pInfo->vLocalPos);
	XMStoreFloat4x4(&m_pInfo->vWorldMatrix, Matrix * XMLoadFloat4x4(&m_pInfo->vWorldMatrix));
	m_pTransformCom->Set_WorldMatrix(m_pInfo->vWorldMatrix);

	return S_OK;
}

void CPlayer04Col::Tick(_float fTimeDelta)
{
	Emp_Col();

	m_fLifeTimeAcc += 1.f * fTimeDelta;
	if (m_fLifeTimeAcc >= m_pInfo->fLifeTime)
	{
		Set_Dead();
	}
}

void CPlayer04Col::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;
	Tick_Col(m_pTransformCom->Get_WorldMatrix_NoScale());
	
	Choose_Col(nullptr);
	
	Render_Col(m_pRendererCom);

}

HRESULT CPlayer04Col::Render()
{
	return S_OK;
}

void CPlayer04Col::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());
	_float3 vSize = Get_Colliders("Col")->Get_Desc()->vSize;
	_float3 vCenter = Get_Colliders("Col")->Get_Desc()->vCenter;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);

	Get_Colliders("Col")->Set_Size(vSize);
	Get_Colliders("Col")->Set_Center(vCenter);

	ImGui::End();
}

void CPlayer04Col::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Monster" == Desc.pOther->Get_Tag())
	{
		// 내 OBB와 상대편의 OBB와 충돌 했다면

		if ((!strcmp("Col", Desc.pMyDesc->sTag) &&
			!strcmp("Monster_Body1", Desc.pOtherDesc->sTag)))
		{		
			//((CCreture*)Desc.pOther)->On_Hit(this);
			_vector Look = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_POSITION)
				- ((CTransform*)Desc.pOther->Get_ComponentPtr(L"Com_Transform"))->Get_State(CTransform::STATE_POSITION));
			((CTransform*)Desc.pOther->Get_ComponentPtr(L"Com_Transform"))->Go_Dir(Look, 15.f, TM->Get_TimeDelta());
		}
	}
}

HRESULT CPlayer04Col::Ready_Components()
{
	

	CCollider::COLLIDERDESC ColDesc;
	ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	ColDesc.vCenter = _float3(0.f, 1.f, 0.f);
	ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	ColDesc.vSize = _float3(30.f, 30.f, 30.f);
	strcpy(ColDesc.sTag, "Col");
	ColDesc.pOwner = this;
	ColDesc.iFlag = 2;
	if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
		return E_FAIL;

	Get_Colliders("Col")->Set_Size(_float3{ 7.f,3.f,6.f });
	Get_Colliders("Col")->Set_Center(_float3{ 0.f,3.f,4.f });

	return S_OK;
}

CCreture * CPlayer04Col::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer04Col*		pInstance = new CPlayer04Col(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer04Col"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer04Col::Clone(void * pArg)
{
	CPlayer04Col*		pInstance = new CPlayer04Col(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer04Col"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer04Col::Free()
{
	__super::Free();
	if (m_pInfo != nullptr)
		Safe_Delete(m_pInfo);
}
