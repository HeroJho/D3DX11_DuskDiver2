#include "stdafx.h"
#include "..\Public\PunchScoreText.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"
#include "Creture.h"

CPunchScoreText::CPunchScoreText(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPunchScoreText::CPunchScoreText(const CPunchScoreText & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPunchScoreText::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPunchScoreText::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;

	//몬스터를 받아와 fX, fY 지정해주기

	m_fSizeX = 2.5f * 1.2f;
	m_fSizeY = 2.5f;

	m_pTransformCom->Set_Scale(XMVectorSet(m_fSizeX, m_fSizeY, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(131.5f, -11.f, 82.278f, 1.f));

	return S_OK;
}

void CPunchScoreText::Tick(_float fTimeDelta)
{
	m_bLight = UM->Get_PunchLight();

	if (m_bLight)
	{
		if (m_fTime <= 0.f)
			m_fTime = 0.f;
		else
			m_fTime -= fTimeDelta;
	}
	else
	{
		if (m_fTime >= 1.f)
			m_fTime = 1.f;
		else
			m_fTime += fTimeDelta;
	}

	m_pTransformCom->Rotation(XMVectorSet(0, 1.f, 0, 0), 60.2f);
}

void CPunchScoreText::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CPunchScoreText::Render()
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
	if (FAILED(m_pShaderCom->Set_RawValue("g_Alpha", &m_fTime, sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(10)))
		return E_FAIL;

	if (m_bLight)
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}

	m_bLight = false;

	return S_OK;
}

void CPunchScoreText::Render_Debug()
{
}

HRESULT CPunchScoreText::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Score"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}





CPunchScoreText * CPunchScoreText::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPunchScoreText*		pInstance = new CPunchScoreText(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPunchScoreText"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPunchScoreText::Clone(void * pArg)
{
	CPunchScoreText*		pInstance = new CPunchScoreText(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPunchScoreText"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CPunchScoreText::Free()
{
	__super::Free();


	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
