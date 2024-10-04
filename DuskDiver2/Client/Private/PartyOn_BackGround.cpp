#include "stdafx.h"
#include "..\Public\PartyOn_BackGround.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Monster_Manager.h"
#include "ToolManager.h"



CPartyOn_BackGround::CPartyOn_BackGround(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CPartyOn_BackGround::CPartyOn_BackGround(const CPartyOn_BackGround & rhs)
	: CUI(rhs)
{
}

HRESULT CPartyOn_BackGround::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPartyOn_BackGround::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	return S_OK;
}

void CPartyOn_BackGround::Tick(_float fTimeDelta)
{
	//UI_InputDebug(fTimeDelta);
	
	if (UM->PartyOn_State() && !m_bOnce && m_bCheck)
	{
		if (!m_bSoundOn)
		{
			GI->PlaySoundW(L"SE_TCShow.wav", SD_UI, 0.6f);
			m_bSoundOn = true;
		}
		
		if (720.f > m_UiInfo.fSizeY)
		{
			m_UiInfo.fSizeY += 800.f * fTimeDelta;
		}

		else if (720.f <= m_UiInfo.fSizeY)
		{
			m_UiInfo.fSizeY = 720.f;
			m_bCheck = false;
		}
	}

	if (UM->LEVIADA_State() && !UM->PartyOn_State())
	{
		m_bCheck = true;
		m_UiInfo.fSizeY = 500.f;
	}

	if (UM->PartyOn_State() && m_bOnce && !m_bOnce2 && m_bCheck)
	{
		if (m_bSoundOn)
		{
			GI->PlaySoundW(L"SE_TCShow.wav", SD_UI, 0.6f);
			m_bSoundOn = false;
		}

		if (720.f > m_UiInfo.fSizeY)
		{
			m_UiInfo.fSizeY += 800.f * fTimeDelta;
		}

		else if (720.f <= m_UiInfo.fSizeY)
		{
			m_UiInfo.fSizeY = 720.f;
			m_bCheck = false;
		}
	}


	__super::Tick(fTimeDelta);
}

void CPartyOn_BackGround::LateTick(_float fTimeDelta)
{
	if (CK_DOWN(DIK_RETURN) && UM->LEVIADA_State() && !m_bOnce)
	{
		GI->PlaySoundW(L"UI_Cancel.wav", SD_UI, 0.6f);
		UM->PartyOn(false);

		MM->WakeUp_TrainingBot(1);
		MM->WakeUp_TrainingBot(2);
		m_bOnce = true;
	}

	else if (CK_DOWN(DIK_RETURN) && UM->DD_State() && !m_bOnce2)
	{
		GI->PlaySoundW(L"UI_Cancel.wav", SD_UI, 0.6f);
		UM->PartyOn(false);
		UM->Set_QuestOn(true);
		m_bOnce2 = true;
	}


	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CPartyOn_BackGround::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;

	if (UM->PartyOn_State())
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}


	//UI_RenderDebug();


	// �ڽĵ��� Render ȣ��
	__super::Render();


	return S_OK;
}






HRESULT CPartyOn_BackGround::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_PartyOn_BackGround"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CPartyOn_BackGround::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CPartyOn_BackGround * CPartyOn_BackGround::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPartyOn_BackGround*		pInstance = new CPartyOn_BackGround(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPartyOn_BackGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPartyOn_BackGround::Clone(void * pArg)
{
	CPartyOn_BackGround*		pInstance = new CPartyOn_BackGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPartyOn_BackGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CPartyOn_BackGround::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
