#include "stdafx.h"
#include "..\Public\TalkButton.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"



CTalkButton::CTalkButton(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CTalkButton::CTalkButton(const CTalkButton & rhs)
	: CUI(rhs)
{
}

HRESULT CTalkButton::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTalkButton::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CTalkButton::Tick(_float fTimeDelta)
{
	if (UM->Get_TalkButton() && !UM->Get_QuestOn() && !UM->PartyOn_State() && UM->Get_FrontUI() && !UM->Get_ShopState())
	{
		m_fTime += fTimeDelta;
		if (m_fTime > 0.02f)
		{
			if (m_fAlpha <= 0)
				m_fAlpha = 0.f;
			else
				m_fAlpha -= fTimeDelta * 0.4f;
			if (m_UiInfo.fX >= 790.f)
				m_UiInfo.fX = 790.f;
			else
				m_UiInfo.fX += 5.f;

			m_fTime = 0.f;
		}
	}
	else
	{
		m_UiInfo.fX = 750.f;
		m_fAlpha = 0.2f;
	}

	UM->Set_TalkButton(false);


	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CTalkButton::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);

	if (!UM->Get_ShopState() && !UM->Get_Vendor1Select() && !UM->Get_Vendor2Select() && !UM->Get_Vendor3Select()
		&& !UM->Get_Vendor4Select() && !UM->Get_Vendor5Select() && !((UM->Get_PunchTime() > 0 && UM->Get_PunchTime() < 15)))
	{
		//m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
}

HRESULT CTalkButton::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_TimeDelta", &m_fAlpha, sizeof(_float));


	_uint i = UM->Get_TalkButtonType();

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", i)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(7)))
		return E_FAIL;

	_float f = UM->Get_PunchTime();

	if(!UM->Get_ShopState() && !UM->Get_Vendor1Select() && !UM->Get_Vendor2Select() && !UM->Get_Vendor3Select()
		&& !UM->Get_Vendor4Select() && !UM->Get_Vendor5Select() && !((UM->Get_PunchTime() > 0 && UM->Get_PunchTime() < 15)))
		{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
		}




	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CTalkButton::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TalkButton"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CTalkButton::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CTalkButton * CTalkButton::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTalkButton*		pInstance = new CTalkButton(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTalkButton"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTalkButton::Clone(void * pArg)
{
	CTalkButton*		pInstance = new CTalkButton(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTalkButton"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CTalkButton::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
