#include "stdafx.h"
#include "..\Public\DDStart.h"
#include "GameInstance.h"
#include "ToolManager.h"



CDDStart::CDDStart(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CDDStart::CDDStart(const CDDStart & rhs)
	: CUI(rhs)
{
}

HRESULT CDDStart::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDDStart::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();




	return S_OK;
}

void CDDStart::Tick(_float fTimeDelta)
{
	if (!m_bOn)
	{
		GI->PlaySoundW(L"SE_Title.wav", SD_UI, 0.6f);
		m_bOn = true;
	}
	
	m_fTime += fTimeDelta;

	//UI_InputDebug(fTimeDelta);

	if (m_fTime > 7.5f)
		m_fVanish += fTimeDelta * 0.2f;

	if (m_fTime > 9.f)
		Set_Dead();


	__super::Tick(fTimeDelta);
}

void CDDStart::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CDDStart::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_TimeDelta", &m_fVanish, sizeof(m_fVanish));

	for (int i = 0; i < 100; i++)
	{
		if (m_fTime >= 2 + i * 0.045f && m_fTime < 2 + (i + 1) * 0.045f)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", i)))
				return E_FAIL;
			break;
		}
	}

	if (m_fTime < 2.f)
	{
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;
	}

	if (m_fTime > 6.5f)
	{
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 99)))
			return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Begin(7)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	//UI_RenderDebug();


	// �ڽĵ��� Render ȣ��
	__super::Render();

	return S_OK;
}






HRESULT CDDStart::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DDStart"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}







void CDDStart::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CDDStart * CDDStart::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDDStart*		pInstance = new CDDStart(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CDDStart"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDDStart::Clone(void * pArg)
{
	CDDStart*		pInstance = new CDDStart(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CDDStart"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CDDStart::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
