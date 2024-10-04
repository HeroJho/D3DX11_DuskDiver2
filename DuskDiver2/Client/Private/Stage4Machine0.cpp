#include "stdafx.h"
#include "..\Public\Stage4Machine0.h"
#include "GameInstance.h"

CStage4Machine0::CStage4Machine0(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CStage4Machine0::CStage4Machine0(const CStage4Machine0 & rhs)
	:CGameObject(rhs)
{
}

void CStage4Machine0::Set_Start(_bool bStart)
{
	m_bStart = bStart;
	if (!m_bStart)
	{
		m_fUV.y = 0.f;
	}
}

void CStage4Machine0::Set_Gage(_float Ratio)
{
	m_fUV.y = Ratio;
}

HRESULT CStage4Machine0::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CStage4Machine0::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_sTag = "Tag_Stage_prop";

	Make_ImGui();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CStage4Machine0::Tick(_float fTimeDelta)
{
	// 테스트 코드
	if (CK_DOWN(DIK_UP))
		m_fUV.y += 0.05f;
	else if (CK_DOWN(DIK_DOWN))
		m_fUV.y -= 0.05f; 
	
	// 여기 밑에 uv를 이동시킬 코드를 추가해라.
	// m_fUV.y     +0.05f마다 한칸씩 올라간다.(초록->노랑->빨강)


	if (m_bStart)
	{
		m_fTimeAcc += fTimeDelta;


		m_fBlurPow += fTimeDelta * 0.7f;
		if (0.6f < m_fBlurPow)
			m_fBlurPow = 0.6f;

		m_fColorRatio += fTimeDelta * 0.7f;
		if (1.f < m_fColorRatio)
			m_fColorRatio = 1.f;
		

		m_fUV.y += fTimeDelta;
		if (1.f < m_fUV.y)
			m_fUV.y = 0.f;

	}
	else
	{




	}


}

void CStage4Machine0::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this); //
}

HRESULT CStage4Machine0::Render()
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

	// _float fBlurPower = 0.6f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &m_fBlurPow, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fEmRatio", &m_fColorRatio, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_UVMoveu", &m_fUV.x, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_UVMovev", &m_fUV.y, sizeof(_float))))
		return E_FAIL;

	m_pTextureCom->Set_SRV(m_pShaderCom, "g_EmMask", 0);

	_float fGama = 0.8f;
	if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();
	_uint iPassIndex = 21;

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		// 노말 텍스쳐가 있다면
		/*if (!FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_NORMALS, "g_NormalTexture")))
		iPassIndex = 2;*/

		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_EMISSIVE, "g_EmissiveTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, iPassIndex)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CStage4Machine0::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model_Emissive"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("SM_VA_Machine0"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("T_VA_Machine0_Mask"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

CStage4Machine0 * CStage4Machine0::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStage4Machine0*		pInstance = new CStage4Machine0(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStage4Machine0"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStage4Machine0::Clone(void * pArg)
{
	CStage4Machine0*		pInstance = new CStage4Machine0(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStage4Machine0"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CStage4Machine0::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
