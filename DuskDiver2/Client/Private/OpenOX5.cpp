#include "stdafx.h"
#include "..\Public\OpenOX5.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"
#include "Creture.h"
#include "Player_Manager.h"

COpenOX5::COpenOX5(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

COpenOX5::COpenOX5(const COpenOX5 & rhs)
	: CGameObject(rhs)
{
}

HRESULT COpenOX5::Initialize_Prototype()
{
	return S_OK;
}

HRESULT COpenOX5::Initialize(void * pArg)
{

	if (FAILED(Ready_Components()))
		return E_FAIL;


	m_fSizeX = 3.f;
	m_fSizeY = 3.f;

	m_pTransformCom->Set_Scale(XMVectorSet(m_fSizeX, m_fSizeY, 0.f, 0.f));
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-113.5f, 0.5f, 106.5f, 1.f));

	return S_OK;
}

void COpenOX5::Tick(_float fTimeDelta)
{
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(-113.5f, 2.5f, 106.5f, 1.f));
	if (UM->Get_Trigger5())
	{
		if (!m_bTriggerCheck)
		{
			m_fAlpha = 0.f;
			m_bTriggerCheck = true;
		}
		m_fO += fTimeDelta;
		if (m_fO > 2.f)
			m_bO = true;
		if (m_fO > 3.f)
			m_fAlpha += fTimeDelta;

		UM->Set_PunchTime(0);
	}
	else
	{
		m_fAlphaTime += fTimeDelta;
		if (m_fAlphaTime > 5.f)
			m_fAlphaTime = 0.f;
		else if (m_fAlphaTime > 4.f && m_fAlphaTime <= 5.f)
			m_fAlpha -= fTimeDelta;
		else if (m_fAlphaTime > 3.f)
			m_fAlpha += fTimeDelta;
	}

	m_fPlayerPos = PM->Get_PlayerPos();
	m_bNear = m_pTransformCom->Check_LimitDistance(XMLoadFloat4(&m_fPlayerPos), 5.f);

	if (m_bNear && !m_bOnce) //가까이 왔을때
	{
		GI->PlaySoundW(L"song054_unknown.wav", SD_NPC, 0.7f);
		m_bOnce = true;
	}

	else if (!m_bNear && m_bOnce && !m_bO) //멀어졌을때
	{
		GI->StopSound(SD_NPC);
		m_bOnce = false;
	}

	else if (!m_bNear && !m_bOnce && m_bO) // 멀고 O로 바뀌었을때
	{
		GI->PlaySoundW(L"song043_ojama_okuri_shou.wav", SD_NPC, 0.7f);
		m_bOnce = true;
	}


	m_pTransformCom->Rotation(XMVectorSet( 0,1.f,0,0), -31.f);

}

void COpenOX5::LateTick(_float fTimeDelta)
{
	if (nullptr == m_pRendererCom)
		return;


	//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT COpenOX5::Render()
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
	if (FAILED(m_pShaderCom->Set_RawValue("g_Alpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	if (m_bO)
	{
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 1)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Begin(12)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;
		if (FAILED(m_pShaderCom->Begin(11)))
			return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;
	
	return S_OK;
}

void COpenOX5::Render_Debug()
{
}

HRESULT COpenOX5::Ready_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_OpenOX"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}





COpenOX5 * COpenOX5::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	COpenOX5*		pInstance = new COpenOX5(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : COpenOX5"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * COpenOX5::Clone(void * pArg)
{
	COpenOX5*		pInstance = new COpenOX5(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : COpenOX5"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void COpenOX5::Free()
{
	__super::Free();


	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pTextureCom);
}
