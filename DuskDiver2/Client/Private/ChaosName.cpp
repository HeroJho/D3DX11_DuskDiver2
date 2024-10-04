#include "stdafx.h"
#include "..\Public\ChaosName.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"



CChaosName::CChaosName(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CChaosName::CChaosName(const CChaosName & rhs)
	: CUI(rhs)
{
}

HRESULT CChaosName::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CChaosName::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CChaosName::Tick(_float fTimeDelta)
{
	if (UM->Get_ChaosName() && !m_bBeforeRand)
	{
		m_bBeforeRand = true;
	}
	if (m_bBeforeRand)
	{
		if (m_fTime <= 0)
		{
			m_fTime = 0;

			if (!m_bOnce)
			{
				GI->PlaySoundW(L"UI_TitleCard.wav", SD_UI, 0.6f);
				m_bOnce = true;
			}
		}
		else
			m_fTime -= fTimeDelta * 0.2f;

		if (!UM->Get_ChaosName())
		{
			m_fTime2 += fTimeDelta * 0.2f;
		}
	}
	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CChaosName::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);

	if (m_bBeforeRand)
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
}

HRESULT CChaosName::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	if(!UM->Get_ChaosName() && m_bBeforeRand)
		m_pShaderCom->Set_RawValue("g_TimeDelta", &m_fTime2, sizeof(_float));
	else
		m_pShaderCom->Set_RawValue("g_TimeDelta", &m_fTime, sizeof(_float));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(7)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;



	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CChaosName::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ChaosName"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CChaosName::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CChaosName * CChaosName::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CChaosName*		pInstance = new CChaosName(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CChaosName"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CChaosName::Clone(void * pArg)
{
	CChaosName*		pInstance = new CChaosName(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CChaosName"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CChaosName::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
