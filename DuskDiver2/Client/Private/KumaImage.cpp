#include "stdafx.h"
#include "..\Public\KumaImage.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"
#include "Player_Manager.h"
#include "Creture.h"

CKumaImage::CKumaImage(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CKumaImage::CKumaImage(const CKumaImage & rhs)
	: CGameObject(rhs)
{
}

HRESULT CKumaImage::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CKumaImage::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = 0.4f;
	m_fSizeY = 0.5f;

	m_pTransformCom->Set_Scale(XMVectorSet(m_fSizeX, m_fSizeY, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(PM->Get_PlayerPos().x, PM->Get_PlayerPos().y, PM->Get_PlayerPos().z + 3 , 1.f));

	return S_OK;
}

void CKumaImage::Tick(_float fTimeDelta)
{
	m_pTransformCom->Rotation(XMVectorSet( 0,1.f,0,0), 31.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(PM->Get_PlayerPos().x + 0.4f, PM->Get_PlayerPos().y + 1.f, PM->Get_PlayerPos().z + 0.6f, 1.f));

}

void CKumaImage::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	Billboard();
	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CKumaImage::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);


	if (FAILED(m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_RGB", &m_fRGB, sizeof(_float3))))
	//	return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	_int i = UM->Get_BearType();
	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", i)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;

	if (UM->Get_BearHunt())
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}
	
	return S_OK;
}

void CKumaImage::Render_Debug()
{
}

HRESULT CKumaImage::Ready_Components()
{
	/* For.Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"), TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;
	
	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BearType"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}



HRESULT CKumaImage::Billboard()
{
	_vector vRight = XMLoadFloat3(&GI->Get_CamDir(CPipeLine::DIR_RIGHT));
	_vector vUp = XMLoadFloat3(&GI->Get_CamDir(CPipeLine::DIR_UP));
	_vector vLook = XMLoadFloat3(&GI->Get_CamDir(CPipeLine::DIR_LOOK));

	vRight = m_pTransformCom->Get_Scale().x * XMVector3Normalize(vRight);
	vUp = m_pTransformCom->Get_Scale().y * XMVector3Normalize(vUp);
	vLook = m_pTransformCom->Get_Scale().z * XMVector3Normalize(vLook);

	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, vUp);
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, vLook);

	return S_OK;
}

CKumaImage * CKumaImage::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CKumaImage*		pInstance = new CKumaImage(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CKumaImage"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CKumaImage::Clone(void * pArg)
{
	CKumaImage*		pInstance = new CKumaImage(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CKumaImage"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CKumaImage::Free()
{
	__super::Free();


	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
