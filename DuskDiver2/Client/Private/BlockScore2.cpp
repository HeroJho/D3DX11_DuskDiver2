#include "stdafx.h"
#include "..\Public\BlockScore2.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"
#include "Creture.h"

CBlockScore2::CBlockScore2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CBlockScore2::CBlockScore2(const CBlockScore2 & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBlockScore2::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CBlockScore2::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//몬스터를 받아와 fX, fY 지정해주기

	m_fSizeX = 3.f;
	m_fSizeY = 3.f;

	m_pTransformCom->Set_Scale(XMVectorSet(m_fSizeX, m_fSizeY, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-137.76f, 34.275f, 108.577f, 1.f));

	return S_OK; 
}

void CBlockScore2::Tick(_float fTimeDelta)
{
	m_pTransformCom->Rotation(XMVectorSet( 0,1.f,0,0), 270.f);
	
	m_fRGB = UM->Get_BlockRGB();

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-137.76f, UM->Get_BlockPos(), 108.577f, 1.f));
}

void CBlockScore2::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CBlockScore2::Render()
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
	if (FAILED(m_pShaderCom->Set_RawValue("g_RGB", &m_fRGB, sizeof(_float3))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);


	_uint i = ((_uint)UM->Get_BlockNum() - 2) / 10;

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", i)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Begin(9)))
		return E_FAIL;

	if (UM->Get_BlockNum() - 2 >= 10)
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}
	
	return S_OK;
}

void CBlockScore2::Render_Debug()
{
}

HRESULT CBlockScore2::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Number"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}





CBlockScore2 * CBlockScore2::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBlockScore2*		pInstance = new CBlockScore2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBlockScore2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBlockScore2::Clone(void * pArg)
{
	CBlockScore2*		pInstance = new CBlockScore2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBlockScore2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CBlockScore2::Free()
{
	__super::Free();


	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
