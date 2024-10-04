#include "stdafx.h"
#include "..\Public\MetalTrashbinTop.h"
#include "GameInstance.h"

#include "ToolManager.h"

CMetalTrashbinTop::CMetalTrashbinTop(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMetalTrashbinTop::CMetalTrashbinTop(const CMetalTrashbinTop & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMetalTrashbinTop::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMetalTrashbinTop::Initialize(void * pArg)
{
	m_MetalTrashbinTopDesc = *(METALTRASHBINTOPDESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Boss";

	Make_ImGui();

	return S_OK;
}

void CMetalTrashbinTop::Tick(_float fTimeDelta)
{
	
}

void CMetalTrashbinTop::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Tick_Col(m_pTransformCom->Get_WorldMatrix());
	Choose_Col(nullptr);
	Render_Col(m_pRendererCom);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	m_pRigidCom->Update_Transform(m_pTransformCom);
	m_pRendererCom->Add_DebugGroup(m_pRigidCom);
}

HRESULT CMetalTrashbinTop::Render()
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

	//m_pTextureCom->Set_SRV(m_pShaderCom, "g_MaskTexture", 0);

	// For.Blur
	_float fBlurPower = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 2)))
			return E_FAIL;
	}

	return S_OK;
}

void CMetalTrashbinTop::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);

	IG->Debug_Rigid(m_pRigidCom);

	ImGui::End();
}

void CMetalTrashbinTop::Add_Force(_float3 vForce)
{
	m_pRigidCom->Add_Force(vForce);
}

HRESULT CMetalTrashbinTop::Ready_Components()
{
	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_PR_MetalTrashbinAa"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Rigid */
	CRigid::RIGIDEDESC RigidDesc;
	RigidDesc.vStartPos = *(_float3*)(&m_MetalTrashbinTopDesc.vPosition);
	RigidDesc.vPos = _float3(0.f, -0.1f, 0.f);
	RigidDesc.vExtense = _float3(0.5f, 0.1f, 0.5f);
	RigidDesc.vRotation = { 0.f, 0.f, 0.f };
	RigidDesc.fWeight = 10.f;
	RigidDesc.fStaticFriction = 1.f;
	RigidDesc.fDynamicFriction = 1.f;
	RigidDesc.fRestitution = 0.1f;
	RigidDesc.fAngleDump = 1.f;

	// Prototype_Component_Rigid_Sphere
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Rigid"), TEXT("Com_Rigid"), (CComponent**)&m_pRigidCom, &RigidDesc)))
		return E_FAIL;

	return S_OK;
}

void CMetalTrashbinTop::InitState()
{
	
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_MetalTrashbinTopDesc.vPosition));
}

CMetalTrashbinTop * CMetalTrashbinTop::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMetalTrashbinTop*		pInstance = new CMetalTrashbinTop(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMetalTrashbinTop"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMetalTrashbinTop::Clone(void * pArg)
{
	CMetalTrashbinTop*		pInstance = new CMetalTrashbinTop(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMetalTrashbinTop"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMetalTrashbinTop::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRigidCom);
}
