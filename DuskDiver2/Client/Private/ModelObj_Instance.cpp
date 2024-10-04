#include "stdafx.h"
#include "..\Public\ModelObj_Instance.h"
#include "GameInstance.h"
#include "ToolManager.h"

CModelObj_Instance::CModelObj_Instance(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CModelObj_Instance::CModelObj_Instance(const CModelObj_Instance & rhs)
	: CGameObject(rhs)
{
}

HRESULT CModelObj_Instance::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CModelObj_Instance::Initialize(void * pArg)
{

	m_Desc = *((MODELINSTANCEDESC*)pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	m_sTag = "Tag_Potal";

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.f));

	m_pModelCom->Set_AnimIndex(m_Desc.iAnimIndex);

	return S_OK;
}

void CModelObj_Instance::Tick(_float fTimeDelta)
{
	if (TM->Get_Stage3_Bool() || TM->Get_Stage2_2Bool())
	{
		m_fDissolveAcc += 0.15f * fTimeDelta;
		if (m_fDissolveAcc >= 1.f)
		{
			m_fDissolveAcc = 1.f;
			m_bRender = true;
		}

	}
	else
	{
		m_bRender = false;
		m_fDissolveAcc = 0.f;
	}


}

void CModelObj_Instance::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	 m_pModelCom->Play_Animation(fTimeDelta);


	if(!m_bRender)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	
}

HRESULT CModelObj_Instance::Render()
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


	if (FAILED(m_pDissolveTexture->Set_SRV(m_pShaderCom, "g_DissolveTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveAcc", &m_fDissolveAcc, sizeof(float))))
		return E_FAIL;

	_float fGama = 1.3f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;

	
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 1)))
			return E_FAIL;
	}

	return S_OK;
}

void CModelObj_Instance::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());

	IG->Debug_Transform(m_pTransformCom);



	ImGui::End();
}

void CModelObj_Instance::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
	if ("Tag_Player" == Desc.pOther->Get_Tag())
	{


	}
}



HRESULT CModelObj_Instance::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model_Instance"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, m_Desc.sModelName, TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Texture*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Dissolve"), TEXT("Com_Texture"), (CComponent**)&m_pDissolveTexture)))
		return E_FAIL;



	return S_OK;
}





CModelObj_Instance * CModelObj_Instance::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CModelObj_Instance*		pInstance = new CModelObj_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CModelObj_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CModelObj_Instance::Clone(void * pArg)
{
	CModelObj_Instance*		pInstance = new CModelObj_Instance(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CModelObj_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CModelObj_Instance::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pDissolveTexture);
}
