#include "stdafx.h"
#include "..\Public\Trashbin_1.h"
#include "GameInstance.h"

CTrashbin_1::CTrashbin_1(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CTrashbin_1::CTrashbin_1(const CTrashbin_1 & rhs)
	: CGameObject(rhs)
{
}

HRESULT CTrashbin_1::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CTrashbin_1::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	m_sTag = "Tag_Item";

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 5.f, 0.f, 1.f));
	// m_pTransformCom->Set_Scale(XMVectorSet(10.f, 10.f, 10.f, 1.f));
	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.f));



	return S_OK;
}

void CTrashbin_1::Tick(_float fTimeDelta)
{

}

void CTrashbin_1::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRigidCom->Update_Transform(m_pTransformCom);


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	m_pRendererCom->Add_DebugGroup(m_pRigidCom);
}

HRESULT CTrashbin_1::Render()
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


	// For.Blur
	_float fBlurPower = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
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

void CTrashbin_1::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{


}



HRESULT CTrashbin_1::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;


	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_Metro_Trashbin2"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Rigid */
	CRigid::RIGIDEDESC RigidDesc;
	RigidDesc.vStartPos = _float3(0.f, 5.f, 2.f);
	RigidDesc.vPos = _float3(0.f, -.5f, 0.f);
	RigidDesc.vExtense = _float3(0.5f, 1.f, 0.5f);
	RigidDesc.fWeight = 10.f;
	RigidDesc.fStaticFriction = 2.f;
	RigidDesc.fDynamicFriction = 2.f;
	RigidDesc.fRestitution = 0.1f;
	RigidDesc.fAngleDump = 1.f;

	// Prototype_Component_Rigid_Sphere
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Rigid"), TEXT("Com_Rigid"), (CComponent**)&m_pRigidCom, &RigidDesc)))
		return E_FAIL;



	return S_OK;
}





CTrashbin_1 * CTrashbin_1::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTrashbin_1*		pInstance = new CTrashbin_1(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPxBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTrashbin_1::Clone(void * pArg)
{
	CTrashbin_1*		pInstance = new CTrashbin_1(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPxBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CTrashbin_1::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRigidCom);
}



void CTrashbin_1::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);
	IG->Debug_Rigid(m_pRigidCom);

	if(ImGui::Button("On"))
		m_pRigidCom->Get_PxBody()->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false);
	if (ImGui::Button("Off"))
		m_pRigidCom->Get_PxBody()->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);

	ImGui::End();
}