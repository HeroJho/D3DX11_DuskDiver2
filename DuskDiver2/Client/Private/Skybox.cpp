#include "stdafx.h"
#include "..\Public\Skybox.h"
#include "GameInstance.h"
#include "ToolManager.h"

CSkybox::CSkybox(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CSkybox::CSkybox(const CSkybox & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSkybox::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSkybox::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_Desc = *((SKYDESC*)pArg);


	Make_ImGui();

	m_sTag = "Tag_Skybox";

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.f));

	return S_OK;
}

void CSkybox::Tick(_float fTimeDelta)
{
	if (!m_Desc.bDisolve)
		return;


	if (TM->Get_Stage3_Bool())
	{
		m_fDissolveAcc += 0.15f * fTimeDelta;
		if (m_fDissolveAcc >= 1.f)
			m_fDissolveAcc = 1.f;
	}
	else
		m_fDissolveAcc = 0.f;

	m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), 0.01f, fTimeDelta);

}

void CSkybox::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	//Tick_Col(m_pTransformCom->Get_WorldMatrix());
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&GI->Get_CamPosition()));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
	/*Choose_Col(nullptr);
	Render_Col(m_pRendererCom);*/
}

HRESULT CSkybox::Render()
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
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &m_fBlurPow, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", m_Desc.iSkyIndex)))
		return E_FAIL;
	if (FAILED(m_pDissolveTexture->Set_SRV(m_pShaderCom, "g_DissolveTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fDissolveAcc", &m_fDissolveAcc, sizeof(float))))
		return E_FAIL;


	_float fGama = 2.2f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		//if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
		//	return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 17)))
			return E_FAIL;
	}

	return S_OK;
}

void CSkybox::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	ImGui::InputFloat("Blur", &m_fBlurPow);


	ImGui::End();
}

void CSkybox::OnCollision(CCollider::OTHERTOMECOLDESC Desc)
{
}



HRESULT CSkybox::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("S3Sky"), TEXT("S3Sky"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sky"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Dissolve"), TEXT("Com_Texture1"), (CComponent**)&m_pDissolveTexture)))
		return E_FAIL;
	

	return S_OK;
}





CSkybox * CSkybox::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSkybox*		pInstance = new CSkybox(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSkybox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSkybox::Clone(void * pArg)
{
	CSkybox*		pInstance = new CSkybox(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CSkybox"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CSkybox::Free()
{
	__super::Free();


	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pDissolveTexture);
	Safe_Release(m_pTextureCom); 
}
