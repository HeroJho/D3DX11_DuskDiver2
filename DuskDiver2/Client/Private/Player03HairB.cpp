#include "stdafx.h"
#include "..\Public\Player03HairB.h"
#include "GameInstance.h"
#include "Player_Manager.h"
#include "ToolManager.h"
#include "HierarchyNode.h"

CPlayer03HairB::CPlayer03HairB(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CParts(pDevice, pContext)
{
}

CPlayer03HairB::CPlayer03HairB(const CPlayer03HairB & rhs)
	: CParts(rhs)
{
}

HRESULT CPlayer03HairB::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer03HairB::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	m_sTag = "Player01_HairB";

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-1.567f, 0.031f, 0.f, 1.f));
	m_pTransformCom->RotationThree(_float3{ 1.f,0.f,0.f }, 90.f, _float3{ 0.f,1.f,0.f }, 90.f, _float3{ 0.f,0.f,1.f }, 0.f);


	return S_OK;
}

void CPlayer03HairB::Tick(_float fTimeDelta)
{

}

void CPlayer03HairB::LateTick(_float fTimeDelta)
{


}

HRESULT CPlayer03HairB::Render()
{
	if (nullptr == m_pModelCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	_float4x4		WorldMatrix;

	XMStoreFloat4x4(&WorldMatrix, XMMatrixTranspose(m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix()));

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &WorldMatrix, sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &GI->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &GI->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	_float fGama = 1.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;

	if (!m_bPlayerLight)
	{
		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0)))
				return E_FAIL;
		}

		return S_OK;
	}
	
	else
	{
		_float Alpha = 1.f;
		if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &Alpha, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Set_RawValue("g_vPlayerLightColor", &m_vPlayerLightColor, sizeof(_float3))))
			return E_FAIL;

		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 24)))
				return E_FAIL;
		}

		return S_OK;
	}


}


// *[HeroJho]: 디버깅 창입니다. ImGui로 작동됩니다.
void CPlayer03HairB::Render_Debug()
{
	/*ImGui::DragFloat("AX", &AX, 0.01f, 0.f, 360.f, "%.3f");
	ImGui::DragFloat("AY", &AY, 0.01f, 0.f, 360.f, "%.3f");
	ImGui::DragFloat("AZ", &AZ, 0.01f, 0.f, 360.f, "%.3f");

	m_pTransformCom->RotationThree(_float3{ 1.f,0.f,0.f }, AX, _float3{ 0.f,1.f,0.f }, AY, _float3{ 0.f,0.f,1.f }, AZ);*/
}

void CPlayer03HairB::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{


}

_float4x4 CPlayer03HairB::Get_ConbineMatrix()
{
	_float4x4 Matrix;
	XMStoreFloat4x4(&Matrix, m_pTransformCom->Get_WorldMatrix() * m_pParentTransformCom->Get_WorldMatrix());
	return Matrix;
}


HRESULT CPlayer03HairB::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Player03HairB", TEXT("Player03HairB"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	TM->Set_ModelTool(m_pModelCom);

	return S_OK;
}



CPlayer03HairB * CPlayer03HairB::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer03HairB*		pInstance = new CPlayer03HairB(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer04Hair"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer03HairB::Clone(void * pArg)
{
	CPlayer03HairB*		pInstance = new CPlayer03HairB(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer04Hair"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CPlayer03HairB::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);


}
