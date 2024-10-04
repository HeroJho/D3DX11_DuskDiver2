#include "stdafx.h"
#include "..\Public\PxObj.h"
#include "GameInstance.h"

#include "ImGui_Manager.h"
#include "Px_Tool.h"
#include "ToolManager.h"

CPxObj::CPxObj(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPxObj::CPxObj(const CPxObj & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPxObj::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPxObj::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;
	memcpy(&m_Desc, pArg, sizeof(PXOBJDESC));

	Make_ImGui();

	if (FAILED(Ready_Components()))
		return E_FAIL;

	// m_pTransformCom->Set_Scale(XMVectorSet(m_Desc.vScale.x, m_Desc.vScale.y, m_Desc.vScale.z, 1.f));



	return S_OK;
}

void CPxObj::Tick(_float fTimeDelta)
{
	// 이세계일 때
	if (!TM->Get_Stage3_Bool() && !TM->Get_Stage2_2Bool())
	{
		if (!m_bOther)
		{
			m_pRigidCom->Edit_PosRot();
			m_bOther = true;
		}
	}
	else
	{
		m_bOther = false;
	}



}

void CPxObj::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRigidCom->Update_Transform(m_pTransformCom);


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

	m_pRendererCom->Add_DebugGroup(m_pRigidCom);
}

HRESULT CPxObj::Render()
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

	_float fGama = 1.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;

	// For.Blur
	_float fBlurPower = 0.f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &fBlurPower, sizeof(_float))))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		_uint iPassIndex = 0;
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

void CPxObj::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{


}

void CPxObj::Set_Static(_bool bStatic)
{
	m_pRigidCom->Get_PxBody()->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, bStatic);
}



HRESULT CPxObj::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_Desc.sModelName, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	// Prototype_Component_Rigid_Sphere
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Rigid"), TEXT("Com_Rigid"), (CComponent**)&m_pRigidCom, &m_Desc.RigidDesc)))
		return E_FAIL;



	return S_OK;
}





CPxObj * CPxObj::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPxObj*		pInstance = new CPxObj(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPxBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPxObj::Clone(void * pArg)
{
	CPxObj*		pInstance = new CPxObj(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPxBox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CPxObj::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRigidCom);
}



void CPxObj::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::DragFloat3("GlPos", (float*)&m_pRigidCom->Get_Desc()->vStartPos, 0.1f))
		m_pRigidCom->Edit_PosRot();
	if (ImGui::DragFloat3("GlRot", (float*)&m_pRigidCom->Get_Desc()->vRotation, 0.1f))
		m_pRigidCom->Edit_PosRot();


	// IG->Debug_Transform(m_pTransformCom);
	IG->Debug_Rigid(m_pRigidCom);

	if (ImGui::Button("On"))
		m_pRigidCom->Get_PxBody()->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, false);
	ImGui::SameLine();
	if (ImGui::Button("Off"))
		m_pRigidCom->Get_PxBody()->setActorFlag(PxActorFlag::eDISABLE_SIMULATION, true);
	ImGui::SameLine();
	if (ImGui::Button("ResetPos"))
		m_pRigidCom->Edit_PosRot();

	if (ImGui::Button("Edit_Delete"))
		IG->Get_Px_Tool()->Delete_Obj(m_sImGuiTag);
	if (ImGui::Button("Edit_SameModel"))
		IG->Get_Px_Tool()->Edit_SameName();


	PxVec3 vVel = m_pRigidCom->Get_PxBody()->getLinearVelocity();
	_float3 fMa(vVel.x, vVel.y, vVel.z);
	ImGui::InputFloat3("vVel", (_float*)&fMa);

	if (IG->Get_ShowSelectedObj())
		m_pRigidCom->Set_GreenColor(true);

	ImGui::End();
}