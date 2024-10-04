#include "stdafx.h"
#include "..\Public\Meg.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"
#include "Player_Manager.h"
#include "Creture.h"

CMeg::CMeg(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMeg::CMeg(const CMeg & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMeg::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CMeg::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	m_fSizeX = 0.5f;
	m_fSizeY = 0.5f;

	m_pTransformCom->Set_Scale(XMVectorSet(m_fSizeX, m_fSizeY, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(PM->Get_PlayerPos().x, PM->Get_PlayerPos().y, PM->Get_PlayerPos().z + 3 , 1.f));

	return S_OK;
}

void CMeg::Tick(_float fTimeDelta)
{
	m_pTransformCom->Rotation(XMVectorSet( 0,1.f,0,0), 30.8f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(PM->Get_PlayerPos().x + 0.6f, PM->Get_PlayerPos().y + 1.f, PM->Get_PlayerPos().z + 0.6f, 1.f));

	if (CK_DOWN(DIK_I))
		UM->Set_Meg(true);
	if (CK_DOWN(DIK_U))
		UM->Set_Meg(false);

	if (UM->Get_Meg())
	{
		m_fTime += fTimeDelta;
		if (m_fTime > 0.02f)
		{
			if (m_fSizeY >= 0.5f)
				m_fSizeY = 0.5f;
			else
				m_fSizeY += 0.01f;
		}
	}
	else
	{
			m_fTime += fTimeDelta;
		if (m_fTime > 0.02f)
		{
			if (m_fSizeY <= 0.f)
				m_fSizeY = 0.f;
			else
				m_fSizeY -= 0.01f;
		}
	}


}

void CMeg::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CMeg::Render()
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

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture",0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	
	return S_OK;
}

void CMeg::Render_Debug()
{
}

HRESULT CMeg::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Meg"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}



HRESULT CMeg::Billboard()
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

CMeg * CMeg::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMeg*		pInstance = new CMeg(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMeg"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMeg::Clone(void * pArg)
{
	CMeg*		pInstance = new CMeg(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMeg"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CMeg::Free()
{
	__super::Free();


	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
