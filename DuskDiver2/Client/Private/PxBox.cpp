#include "stdafx.h"
#include "..\Public\PxBox.h"
#include "GameInstance.h"

CPxBox::CPxBox(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPxBox::CPxBox(const CPxBox & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPxBox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPxBox::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	m_sTag = "Tag_Item";

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 5.f, 0.f, 1.f));
	// m_pTransformCom->Set_Scale(XMVectorSet(0.01f, 0.01f, 0.01f, 1.f));
	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.f));


	return S_OK;
}

void CPxBox::Tick(_float fTimeDelta)
{

}

void CPxBox::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

 	m_pRigidCom->Update_Transform(m_pTransformCom);


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	m_pRendererCom->Add_DebugGroup(m_pRigidCom);
}

HRESULT CPxBox::Render()
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

void CPxBox::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{


}



HRESULT CPxBox::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("WoodBlock"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Rigid */
	CRigid::RIGIDEDESC RigidDesc;
	RigidDesc.vStartPos = _float3(0.f, 5.f, 0.f); //시작 돌기둥안에서 얘네의 위치, 돌기둥 위치 더해줘야함
	RigidDesc.vPos = _float3(0.f, -.8f, 0.f); //콜라이더랑 모델의 위치가 달라서 맞춰주는것 툴로
	RigidDesc.vExtense = _float3(1.5f, 1.5f, 1.5f); //콜라이더 크기
	RigidDesc.fWeight = 100.f;
	RigidDesc.fStaticFriction = 1.f;
	RigidDesc.fDynamicFriction = 1.f;
	RigidDesc.fRestitution = 0.1f;
	RigidDesc.fAngleDump = 1.f;

	// Prototype_Component_Rigid_Sphere
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Rigid"), TEXT("Com_Rigid"), (CComponent**)&m_pRigidCom, &RigidDesc)))
		return E_FAIL;


	/* For.Com_Shader */
	//if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Mask"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
	//	return E_FAIL;


	///* For.Com_Collider */
	//CCollider::COLLIDERDESC ColDesc;
	//ZeroMemory(&ColDesc, sizeof(CCollider::COLLIDERDESC));

	//ColDesc.vCenter = _float3(0.f, 0.f, 0.f);
	//ColDesc.vRotation = _float3(0.f, 0.f, 0.f);
	//ColDesc.vSize = _float3(1.f, 1.f, 1.f);
	//strcpy(ColDesc.sTag, "Col_Test_OBB");
	//if (FAILED(AddCollider(CCollider::TYPE_OBB, ColDesc)))
	//	return E_FAIL;


	return S_OK;
}





CPxBox * CPxBox::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPxBox*		pInstance = new CPxBox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPxBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPxBox::Clone(void * pArg)
{
	CPxBox*		pInstance = new CPxBox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPxBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CPxBox::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRigidCom);
}



void CPxBox::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);

	IG->Debug_Rigid(m_pRigidCom);

	ImGui::End();
}