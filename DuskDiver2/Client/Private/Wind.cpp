#include "stdafx.h"
#include "..\Public\Wind.h"
#include "GameInstance.h"
#include "Player_Manager.h"
CWind::CWind(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CEffect(pDevice, pContext)
{
}

CWind::CWind(const CWind& rhs)
	: CEffect(rhs)
{
}

HRESULT CWind::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	return S_OK;
}

HRESULT CWind::Initialize(void * pArg)
{
	m_WindInfo = (*(WINDINFO**)pArg);

	if (FAILED(Ready_Components()))
		return E_FAIL;
	
	m_bDead = false;
	m_fMaxUVIndexX = 0.f;
	m_fMaxUVIndexX = 0.f;
	m_vUVFixSpeed = m_WindInfo->vUVFixSpeed;
	m_pTransformCom->Set_WorldMatrix(m_WindInfo->WorldMatrix);
	if (m_WindInfo->bRotation)
	{
		m_pTransformCom->TurnAngle(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(m_WindInfo->vRotation.x));
		m_pTransformCom->TurnAngle(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(m_WindInfo->vRotation.y));
		m_pTransformCom->TurnAngle(m_pTransformCom->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(m_WindInfo->vRotation.z));
	}

	return S_OK;
}

void CWind::Tick(_float fTimeDelta)
{
	fTimeDelta *= m_fTMRatio;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION)
		+ (XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP)) * (m_WindInfo->vFixPosSpeed.z * fTimeDelta)));
	m_pTransformCom->Set_Scale(XMLoadFloat3(&m_pTransformCom->Get_Scale()) + (XMLoadFloat3(&m_WindInfo->vFixScaleSpeed) * fTimeDelta));
	Set_UVFix(fTimeDelta);

	if (m_WindInfo->vRatio.x > 0.f)
	{
		m_WindInfo->vRatio.x -= m_WindInfo->vRatioSpeed.x * fTimeDelta;
	}

	if (m_WindInfo->vRatio.y > 0.f)
	{
		m_WindInfo->vRatio.y -= m_WindInfo->vRatioSpeed.y * fTimeDelta;
	}

	if (m_WindInfo->vRatio.x <= 0.f && m_WindInfo->vRatio.y <= 0.f)
		Set_Dead();
	
}

void CWind::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Compute_CamZ(m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	if(!m_WindInfo->bDistortion)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	else
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_DISTORTION, this);
	

}

HRESULT CWind::Render()
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

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->SetUp_OnShader(m_pShaderCom, m_pModelCom->Get_MaterialIndex(i), aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;
		m_pShaderCom->Set_RawValue("g_fMaxUVIndexX", &m_fMaxUVIndexX, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_fMaxUVIndexY", &m_fMaxUVIndexY, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_fUVFixX", &m_vUVFixAcc.x, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_fUVFixY", &m_vUVFixAcc.y, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_vColor", &m_WindInfo->vColor, sizeof(_float4));
		m_pShaderCom->Set_RawValue("g_vColor2", &m_WindInfo->vColor2, sizeof(_float4));
		m_pShaderCom->Set_RawValue("g_fRatio1", &m_WindInfo->vRatio.x, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_fRatio2", &m_WindInfo->vRatio.y, sizeof(_float));
		m_pShaderCom->Set_RawValue("g_fBlurPower", &m_WindInfo->fBlurPower, sizeof(_float));

		if (!m_WindInfo->bDistortion)
		{
			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 3)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 4)))
				return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CWind::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_StaticMeshEffect"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Wind"), TEXT("Wind"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

CEffect * CWind::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWind*		pInstance = new CWind(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CWind"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWind::Clone(void * pArg)
{
	CWind*		pInstance = new CWind(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CWind"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWind::Free()
{
	__super::Free();
	Safe_Delete(m_WindInfo);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	

}
