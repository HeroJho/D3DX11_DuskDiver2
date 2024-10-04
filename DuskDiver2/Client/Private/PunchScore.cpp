#include "stdafx.h"
#include "..\Public\PunchScore.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"
#include "Creture.h"
#include "FunchCuma.h"

CPunchScore::CPunchScore(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CPunchScore::CPunchScore(const CPunchScore & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPunchScore::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CPunchScore::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;
	//몬스터를 받아와 fX, fY 지정해주기

	m_fSizeX = 2.5f;
	m_fSizeY = 2.5f;

	m_pTransformCom->Set_Scale(XMVectorSet(m_fSizeX, m_fSizeY, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(132.5f, -13.f, 82.278f, 1.f));

	return S_OK;
}

void CPunchScore::Tick(_float fTimeDelta)
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

void CPunchScore::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CPunchScore::Render()
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

	if (m_bLight)
	{
		if ((TM->Get_FunchCuma()->Get_Score()) > 0 && (TM->Get_FunchCuma()->Get_Score()) <= 2555)
		{
			if (FAILED(m_pShaderCom->Set_RawValue("g_RGB", &_float3(0.65f, 0.906f, 0.428f), sizeof(_float3))))
				return E_FAIL;
		}
		else if ((TM->Get_FunchCuma()->Get_Score()) > 2555 && (TM->Get_FunchCuma()->Get_Score()) <= 5111)
		{
			if (FAILED(m_pShaderCom->Set_RawValue("g_RGB", &_float3(1.f, 0.95f, 0.05f), sizeof(_float3))))
				return E_FAIL;
		}
		else if ((TM->Get_FunchCuma()->Get_Score()) > 5111 || m_bHighScore)
		{
			if (FAILED(m_pShaderCom->Set_RawValue("g_RGB", &_float3(0.8f, 0.2f, 0.22f), sizeof(_float3))))
				return E_FAIL;
		}
	}
	RELEASE_INSTANCE(CGameInstance);


	if (m_bLight)
	{
		if (TM->Get_FunchCuma()->Get_Score() > 9999 || m_bHighScore)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 9)))
				return E_FAIL;

			if (FAILED(m_pShaderCom->Begin(9)))
				return E_FAIL;

			m_bHighScore = true;
		}
		else
		{

			_int i = (_int)(TM->Get_FunchCuma()->Get_Score()) % 10;


			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", i)))
				return E_FAIL;

			if (TM->Get_FunchCuma()->Get_Score() > 0)
			{
				if (FAILED(m_pShaderCom->Begin(9)))
					return E_FAIL;
			}
			else
			{
				if (FAILED(m_pShaderCom->Begin(10)))
					return E_FAIL;
			}
		}
	}
	else
	{
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;
	}




	if (m_bLight)
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}

	m_bLight = false;

	return S_OK;
}

void CPunchScore::Render_Debug()
{
}

HRESULT CPunchScore::Ready_Components()
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





CPunchScore * CPunchScore::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPunchScore*		pInstance = new CPunchScore(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPunchScore"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPunchScore::Clone(void * pArg)
{
	CPunchScore*		pInstance = new CPunchScore(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPunchScore"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CPunchScore::Free()
{
	__super::Free();


	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
