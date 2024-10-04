#include "stdafx.h"
#include "..\Public\Buildings.h"
#include "GameInstance.h"
#include "Player_Manager.h"

#include "ToolManager.h"
#include "Camera_Manager.h"

CBuildings::CBuildings(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CBuildings::CBuildings(const CBuildings & rhs)
	:CGameObject(rhs)
{
}

HRESULT CBuildings::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBuildings::Initialize(void * pArg)
{
	m_BDDesc = *(BDDESC*)pArg;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	m_sTag = "Tag_Stage";


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOWDEPTH, this);

	return S_OK;
}

void CBuildings::Tick(_float fTimeDelta)
{
	if (TM->Get_Stage3_Bool())
	{
		if (0.f < m_fBlurPow)
		{
			m_fBlurPow -= 0.1f * fTimeDelta;
		}
		else
			m_fBlurPow = 0.f;
	}
	else
	{
		m_fBlurPow = 0.1f;
	}


	Update_ModelType();
}

void CBuildings::LateTick(_float fTimeDelta)
{
	if (true == m_bRender)
	{
		if (nullptr == m_pRendererCom)
			return;

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	}

}

HRESULT CBuildings::Render()
{
	if (true == m_bRender)
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

		_float fGama = 1.2f;
		if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
			return E_FAIL;

		// For.Blur
		if (FAILED(m_pShaderCom->Set_RawValue("g_BlurPower", &m_fBlurPow, sizeof(_float))))
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
	}
	

	return S_OK;
}

HRESULT CBuildings::Render_ShadowDepth()
{
	if (nullptr == m_pShaderCom ||
		nullptr == m_pTransformCom)
		return E_FAIL;
	
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);


	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_ShadowLightViewMatrix(GI->Get_Cur_LevelIndex()), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	//m_pTextureCom->Set_SRV(m_pShaderCom, "g_MaskTexture", 0);



	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 18)))
			return E_FAIL;
	}

	return S_OK;
}


void CBuildings::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	ImGui::InputFloat("Blur", &m_fBlurPow);


	ImGui::End();
}

HRESULT CBuildings::Ready_Components()
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
	switch (m_BDDesc.iIndex)
	{
	case AREA_IT:
		if (MODEL_LOD0 == m_BDDesc.iModelType)
		{
			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("BD_IT_LOD0"), TEXT("Com_Model0"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}
		else if (MODEL_LOD1 == m_BDDesc.iModelType)
		{
			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("BD_IT_LOD1"), TEXT("Com_Model1"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}
		break;
	case AREA_NC:
		if (MODEL_LOD0 == m_BDDesc.iModelType)
		{
			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("BD_NC_LOD0"), TEXT("Com_Model0"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}
		else if (MODEL_LOD1 == m_BDDesc.iModelType)
		{
			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("BD_NC_LOD1"), TEXT("Com_Model1"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}
		break;
	case AREA_AZIT:
		if (MODEL_LOD0 == m_BDDesc.iModelType)
		{
			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("BD_AZIT_LOD0"), TEXT("Com_Model0"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}
		else if (MODEL_LOD1 == m_BDDesc.iModelType)
		{
			if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("BD_AZIT_LOD1"), TEXT("Com_Model1"), (CComponent**)&m_pModelCom)))
				return E_FAIL;
		}
		break;
	default:
		break;
	}


	return S_OK;
}

void CBuildings::Update_ModelType()
{
	
	_uint iAreaIndex = 100;

	// ver1: 플레이어의 위치를 판단
	//_float4 vPlayerPos = PM->Get_PlayerPos();
	// ver2: 카메라의 위치를 판단
	_float4 vPlayerPos = GI->Get_CamPosition();//CRM->Get_Cam()->Get_CamPos();
	
	
	if (45.f > vPlayerPos.z)
	{
		iAreaIndex = AREA_IT;
	}
	else if ((50.f <= vPlayerPos.z) && (135.f > vPlayerPos.z))
	{
		iAreaIndex = AREA_NC;
	}
	else if (140.f <= vPlayerPos.z)
	{
		iAreaIndex = AREA_AZIT;
	}
	else if (((45.f <= vPlayerPos.z) && (50.f > vPlayerPos.z)) || ((135.f <= vPlayerPos.z) && (140.f > vPlayerPos.z)))
	{
		iAreaIndex = AREA_END;
	}

	// 플레이어위 위치랑 본인의 인덱스가 같다면 LOD0을 아니면 LOD1을
	/*if (m_BDDesc.iIndex == iAreaIndex)
	{
		m_BDDesc.iModelType = MODEL_LOD0;
	}
	else
	{
		m_BDDesc.iModelType = MODEL_LOD1;
	}*/
	if (0 == m_BDDesc.iModelType)
	{
		if (m_BDDesc.iIndex == iAreaIndex)
		{
			m_bRender = true;
		}
		else
			m_bRender = false;
	}
	else if (1 == m_BDDesc.iModelType)
	{
		if (m_BDDesc.iIndex != iAreaIndex)
		{
			m_bRender = true;
		}
		else
			m_bRender = false;
	}
	

}

CBuildings * CBuildings::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBuildings*		pInstance = new CBuildings(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStage3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBuildings::Clone(void * pArg)
{
	CBuildings*		pInstance = new CBuildings(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStage3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBuildings::Free()
{
	__super::Free();

	/*for (_uint i = 0; MOEDL_END > i; ++i)
	{
		Safe_Release(m_pModelCom[i]);
	}*/
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
