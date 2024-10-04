#include "stdafx.h"
#include "..\Public\LookCube_HR.h"

#include "GameInstance.h"
#include "Camera_Manager.h"

CLookCube_HR::CLookCube_HR(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CLookCube_HR::CLookCube_HR(const CLookCube_HR & rhs)
	: CGameObject(rhs)
{
}


HRESULT CLookCube_HR::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLookCube_HR::Initialize(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	COLORCUBEDESC* pDesc = (COLORCUBEDESC*)pArg;

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, _vector{ 0.f, 0.f, 0.f, 1.f });

	_vector vTempPos = XMLoadFloat3(&(pDesc->vPos));
	vTempPos = XMVectorSetW(vTempPos, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vTempPos);
	m_pTransformCom->Set_Scale(XMLoadFloat3(&(pDesc->vScale)));

	m_vRGB = pDesc->vColor;

	return S_OK;
}

void CLookCube_HR::Tick(_float fTimeDelta)
{

}

void CLookCube_HR::LateTick(_float fTimeDelta)
{
	if (!CRM->Get_CutSceneTool())
		return;

	// ť�� ������ �����ϱ�
	if (m_fPreCubeSize != CRM->Get_CutSceneTool()->Get_CubeSize())
	{
		m_fPreCubeSize = CRM->Get_CutSceneTool()->Get_CubeSize();
		m_pTransformCom->Set_Scale(XMVectorSet(m_fPreCubeSize, m_fPreCubeSize, m_fPreCubeSize, 1.f));
	}

	if (CRM->Get_CutSceneTool()->Get_ShowCube())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CLookCube_HR::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	return S_OK;
}

void CLookCube_HR::Set_Pos(_float3 vPos)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPos));
}

_float3 CLookCube_HR::Get_Pos()
{
	_float3 vTempPos;
	ZeroMemory(&vTempPos, sizeof(_float3));
	XMStoreFloat3(&vTempPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	return vTempPos;
}

CLookCube_HR::CAMDATA CLookCube_HR::Get_SaveDATA()
{
	CAMDATA Data;
	ZeroMemory(&Data, sizeof(CAMDATA));

	XMStoreFloat3(&Data.vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	Data.fSpeed = m_fSpeedTime;
	Data.fSens = m_fSens;
	Data.fDestDis = m_fDestDis;
	Data.iLinkIndex = m_iLinkIndex;

	return Data;
}
void CLookCube_HR::Set_SaveDATA(CDataManager::CAMDATA * pData)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&pData->vPos), 1.f));
	m_fSpeedTime = pData->fSpeed;
	m_fSens = pData->fSens;
	m_fDestDis = pData->fDestDis;
	m_iLinkIndex = pData->iLinkIndex;
}



HRESULT CLookCube_HR::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_ColorCube"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For. Com_Shader*/
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ColorCube"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;


	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(TransformDesc));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLookCube_HR::SetUp_ShaderResources()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);




	_float4 vTempColor;
	ZeroMemory(&vTempColor, sizeof(_float4));
	if (CRM->Get_CutSceneTool()->Get_SelectedMarkCubeTag() == m_sTag)
	{
		vTempColor.x = 1.f;
		vTempColor.y = 1.f;
		vTempColor.z = 0.f;
		vTempColor.w = 1.f;
	}
	else
	{
		vTempColor = m_vRGB;
	}
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &vTempColor, sizeof(_float4))))
		return E_FAIL;


	return S_OK;
}

CLookCube_HR * CLookCube_HR::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLookCube_HR*		pInstance = new CLookCube_HR(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CLookCube_HR"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLookCube_HR::Clone(void* pArg)
{
	CLookCube_HR*		pInstance = new CLookCube_HR(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CLookCube_HR"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLookCube_HR::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pRendererCom);
}

