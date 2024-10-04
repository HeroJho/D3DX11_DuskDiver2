#include "stdafx.h"
#include "..\Public\BeastMap2.h"
#include "GameInstance.h"
#include "Time_Manager.h"
#include "ToolManager.h"

CBeastMap2::CBeastMap2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CBeastMap2::CBeastMap2(const CBeastMap2 & rhs)
	: CEffect(rhs)
{
}

HRESULT CBeastMap2::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBeastMap2::Initialize(void * pArg)
{
	if (FAILED(Ready_Components()))
		return E_FAIL;
	_matrix Identity = XMMatrixIdentity();
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, Identity);
	m_pTransformCom->Set_WorldMatrix(WorldMatrix);
	m_pTransformCom->Set_Scale(_vector{ 48.f,50.f,48.f });
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vector{ 0.f,0.011f,-38.f,1.f });
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Dissolve"), TEXT("Com_Texture"), (CComponent**)&m_pDissolveTexture)))
		return E_FAIL;
	return S_OK;
}

void CBeastMap2::Tick(_float fTimeDelta)
{
	TIM->Add_TimeObj(CTime_Manager::OBJ_EFFECT, this);
	fTimeDelta *= m_fTMRatio;



	if (TM->Get_2Page() && !m_bDissolve)
	{
		m_bDissolve = true;
		m_fDissolveAcc = 0.5f;
		m_iPassIndex = 10;
		m_bChange = false;

	}


	if (m_bDissolve)
	{

		m_fDissolveAcc -= 0.1f * fTimeDelta;
		if (0.15f > m_fDissolveAcc)
		{
			m_vUVFixAcc.x += m_vUVFixSpeed.x * fTimeDelta;
			m_vUVFixAcc.y += m_vUVFixSpeed.y * fTimeDelta;
		}

		if (m_fDissolveAcc <= 0.f)
		{
			m_fDissolveAcc = 0.f;
			m_iPassIndex = 7;
		}

	}

}

void CBeastMap2::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);


}

HRESULT CBeastMap2::Render()
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
		//7 
		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, m_iPassIndex)))
			return E_FAIL;
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}


HRESULT CBeastMap2::Ready_Components()
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


CBeastMap2 * CBeastMap2::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBeastMap2*		pInstance = new CBeastMap2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBeastMap2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CEffect * CBeastMap2::Clone(void * pArg)
{
	CBeastMap2*		pInstance = new CBeastMap2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBeastMap2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBeastMap2::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pDissolveTexture);


}



