#include "stdafx.h"
#include "..\Public\BeastMap.h"
#include "GameInstance.h"
#include "Time_Manager.h"
#include "ToolManager.h"

CBeastMap::CBeastMap(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CBeastMap::CBeastMap(const CBeastMap & rhs)
	: CEffect(rhs)
{
}

HRESULT CBeastMap::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBeastMap::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	_matrix Identity = XMMatrixIdentity();
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, Identity);
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
	m_pTransformCom->Set_Scale(_vector{ 50.f,50.f,50.f });
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vector{ 0.f,0.01f,-38.f,1.f });



	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Dissolve"), TEXT("Com_Texture"), (CComponent**)&m_pDissolveTexture)))
		return E_FAIL;



	return S_OK;
}

void CBeastMap::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_EFFECT, this);
	fTimeDelta *= m_fTMRatio;


	if (TM->Get_2Page() && !m_bDissolve)
	{
		m_bDissolve = true;	
		m_fDissolveAcc = 0.5f;
		m_iPassIndex = 9;
		m_bChange = false;
	}

	// 0일 때 안개 0.5일 대 일 반

	if (m_bDissolve)
	{
		m_fDissolveAcc -= 0.1f * fTimeDelta;
		if (m_fDissolveAcc <= 0.f)
		{
			m_fDissolveAcc = 0.f;
			m_iPassIndex = 8;
		}

	}

	
	
}

void CBeastMap::LateTick(_float fTimeDelta)
{	
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
	
	
}

HRESULT CBeastMap::Render()
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
	if (FAILED(m_pShaderCom->Set_RawValue("g_fBlurPower", &m_fBlurPower, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_vColor1, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor2", &m_vColor2, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio1", &m_vRatio.x, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fRatio2", &m_vRatio.y, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixX", &m_vUVFixAcc.x, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_fUVFixY", &m_vUVFixAcc.y, sizeof(_float))))
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_fDissolveAcc", &m_fDissolveAcc, sizeof(float));

	if (FAILED(m_pDissolveTexture->Set_SRV(m_pShaderCom, "g_DissolveTexture", 0)))
		return E_FAIL;

	
	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		// 8
		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, m_iPassIndex)))
			return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


HRESULT CBeastMap::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Player01MeshEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Breath"), TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;


	return S_OK;
}


CBeastMap * CBeastMap::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBeastMap*		pInstance = new CBeastMap(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBeastMap"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEffect * CBeastMap::Clone(void * pArg)
{
	CBeastMap*		pInstance = new CBeastMap(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBeastMap"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBeastMap::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pDissolveTexture);

}



