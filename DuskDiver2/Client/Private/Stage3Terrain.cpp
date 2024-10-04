#include "stdafx.h"
#include "..\Public\Stage3Terrain.h"
#include "GameInstance.h"

#include "ToolManager.h"
#include "DataManager.h"
#include "Light.h"

CStage3Terrain::CStage3Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CStage3Terrain::CStage3Terrain(const CStage3Terrain & rhs)
	: CGameObject(rhs)
{
}

HRESULT CStage3Terrain::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStage3Terrain::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	m_sTag = "Tag_Stage_Terrain";

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
	// m_pTransformCom->Set_Scale(XMVectorSet(0.01f, 0.01f, 0.01f, 1.f));
	//m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(45.f));

	m_vDiffuseDir = GI->Get_DirLightDesc()->vDiffuse;

	return S_OK;
}

void CStage3Terrain::Tick(_float fTimeDelta)
{

	if (TM->Get_Stage3_Bool())
	{
		if (0.f < m_fBlurPow)
			m_fBlurPow -= 0.1f * fTimeDelta;
		else
			m_fBlurPow = 0.f;

		if(0.4f < GI->Get_ValueDesc()->fShadowPow)
			GI->Get_ValueDesc()->fShadowPow -= 0.1f * fTimeDelta;
		else
			GI->Get_ValueDesc()->fShadowPow = 0.4f;

		m_fDirRatio -= 0.1f * fTimeDelta;
		if (0.7f < m_fDirRatio)
			XMStoreFloat4(&GI->Get_DirLightDesc()->vDiffuse, XMLoadFloat4(&m_vDiffuseDir) * m_fDirRatio);
		
		if (0.5f < m_fDirRatio)
		{
			//XMStoreFloat4(&(GI->Get_DirLightDesc()->vSpecular), XMVectorSetW(XMLoadFloat4(&GI->Get_DirLight()->Get_OriLightDesc()->vSpecular) * m_fDirRatio, 1.f));
			XMStoreFloat4(&(GI->Get_DirLightDesc()->vSpecular), XMVectorSet(0.3f, 0.f, 0.f, 1.f) * (1.f - m_fDirRatio));
		}
			

	}
	else
	{
		m_fBlurPow = 0.15f;
		GI->Get_ValueDesc()->fShadowPow = 0.5f;
		GI->Get_DirLightDesc()->vDiffuse = m_vDiffuseDir;
		m_fDirRatio = 1.f;

		XMStoreFloat4(&(GI->Get_DirLightDesc()->vSpecular), XMVectorSetW(XMLoadFloat4(&GI->Get_DirLight()->Get_OriLightDesc()->vSpecular) * m_fDirRatio, 1.f));


	}

}

void CStage3Terrain::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);

}

HRESULT CStage3Terrain::Render()
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

	_float fGama = 1.4f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;

	// For.Blur
	// m_fBlurPow = 0.5f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &m_fBlurPow, sizeof(_float))))
		return E_FAIL;

	fTest = 0.7f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_TTTest", &fTest, sizeof(_float))))
		return E_FAIL;
	


	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		_uint iPassIndex = 0;
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		// 노말 텍스쳐가 있다면
		if (!FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
			iPassIndex = 26;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iPassIndex)))
			return E_FAIL;
	}

	return S_OK;
}

void CStage3Terrain::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	ImGui::InputFloat("Blur", &fTest);
	ImGui::InputFloat("Gama", &m_fGama);


	ImGui::End();
}

HRESULT CStage3Terrain::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Stage3Terrain"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	return S_OK;
}

CStage3Terrain * CStage3Terrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStage3Terrain*		pInstance = new CStage3Terrain(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStage3Terrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStage3Terrain::Clone(void * pArg)
{
	CStage3Terrain*		pInstance = new CStage3Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStage3Terrain"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStage3Terrain::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
