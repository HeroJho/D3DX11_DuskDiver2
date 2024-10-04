#include "stdafx.h"
#include "..\Public\Hit_Bar.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "ToolManager.h"



CHit_Bar::CHit_Bar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CHit_Bar::CHit_Bar(const CHit_Bar & rhs)
	: CUI(rhs)
{
}

HRESULT CHit_Bar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHit_Bar::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CHit_Bar::Tick(_float fTimeDelta)
{
	if (UM->Get_MoreCombo())
	{
		m_fTimerAcc -= 1.f * fTimeDelta;
		m_fShaderTime = m_fTimerAcc / 5.f;

		//Combo가 시작되고 5초내에 이어질 경우 게이지를 Reset
		if (UM->Get_Hit())
			m_fTimerAcc = 5.f;

		UM->Set_Hit(false);
	}

	if (0 >= m_fTimerAcc)
		UM->Reset_Combo();

	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CHit_Bar::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	if (UM->Get_MoreCombo())
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
}

HRESULT CHit_Bar::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_TimeDelta", &m_fShaderTime, sizeof(_float));


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(11)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;




	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CHit_Bar::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hit_Bar"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}


void CHit_Bar::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CHit_Bar * CHit_Bar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHit_Bar*		pInstance = new CHit_Bar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CHit_Bar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHit_Bar::Clone(void * pArg)
{
	CHit_Bar*		pInstance = new CHit_Bar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CHit_Bar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CHit_Bar::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
