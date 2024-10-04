#include "stdafx.h"
#include "..\Public\Player03LShotGun.h"
#include "GameInstance.h"

CPlayer03LShotGun::CPlayer03LShotGun(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CParts(pDevice, pContext)
{
}

CPlayer03LShotGun::CPlayer03LShotGun(const CPlayer03LShotGun & rhs)
	: CParts(rhs)
{
}

HRESULT CPlayer03LShotGun::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPlayer03LShotGun::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Player03ShotGun";

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.1f, -0.09f, 1.f));
	m_pTransformCom->RotationThree(_float3{ 1.f,0.f,0.f }, 0.f, _float3{ 0.f,1.f,0.f }, 0.f, _float3{ 0.f,0.f,1.f }, 0.f);
	// 0 0.108 -0.066 30
	return S_OK;
}

void CPlayer03LShotGun::Tick(_float fTimeDelta)
{
	
}

void CPlayer03LShotGun::LateTick(_float fTimeDelta)
{

}

HRESULT CPlayer03LShotGun::Render()
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

void CPlayer03LShotGun::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{


}

void CPlayer03LShotGun::Render_Debug()
{
	/*ImGui::DragFloat("PosX", &PosX, 0.001f, -10.f, 10.f, "%.3f");
	ImGui::DragFloat("PosY", &PosY, 0.001f, -10.f, 10.f, "%.3f");
	ImGui::DragFloat("PosZ", &PosZ, 0.001f, -10.f, 10.f, "%.3f");
	ImGui::DragFloat("AX", &AX, 0.01f, 0.f, 360.f, "%.3f");
	ImGui::DragFloat("AY", &AY, 0.01f, 0.f, 360.f, "%.3f");
	ImGui::DragFloat("AZ", &AZ, 0.01f, 0.f, 360.f, "%.3f");

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(PosX, PosY, PosZ, 1.f));
	m_pTransformCom->RotationThree(_float3{ 1.f,0.f,0.f }, AX, _float3{ 0.f,1.f,0.f }, AY, _float3{ 0.f,0.f,1.f }, AZ);*/
}

HRESULT CPlayer03LShotGun::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, L"Player03ShotGun", TEXT("Player03ShotGun"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}


CPlayer03LShotGun * CPlayer03LShotGun::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer03LShotGun*		pInstance = new CPlayer03LShotGun(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer03LShotGun"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayer03LShotGun::Clone(void * pArg)
{
	CPlayer03LShotGun*		pInstance = new CPlayer03LShotGun(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayer03LShotGun"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CPlayer03LShotGun::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
}
