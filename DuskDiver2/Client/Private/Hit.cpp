#include "stdafx.h"
#include "..\Public\Hit.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "ToolManager.h"
#include "UI_Number2.h"
#include "UI_Number.h"


CHit::CHit(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CHit::CHit(const CHit & rhs)
	: CUI(rhs)
{
}

HRESULT CHit::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CHit::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	if (FAILED(Make_ChildUI(-1.668646f, 0.f, 132.8f, 38.4f, TEXT("Prototype_GameObject_Hit_Bar"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(43.9f, -19.8f, 40.f, 40.f, TEXT("Prototype_GameObject_UI_Number2"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(40.9f, -20.8f, 40.f, 40.f, TEXT("Prototype_GameObject_UI_Number"))))
		return E_FAIL;

	vector<CUI*>::iterator iter = m_pChildUIs.begin();
	++iter;
	m_pUINum2 = (CUI_Number2*)(*iter);
	++iter;
	m_pUINum = (CUI_Number*)(*iter);

	return S_OK;
}

void CHit::Tick(_float fTimeDelta)
{
	m_pUINum2->Set_Number(UM->Get_Combo());
	m_pUINum->Set_Number(UM->Get_Combo());

	//UI_InputDebug(fTimeDelta);

	if (!m_bRender && !UM->PartyOn_State() && UM->Get_FrontUI() && !UM->Get_QuestOn() && !UM->Get_ShopState() && !UM->Get_StreetShop())
		__super::Tick(fTimeDelta);
}

void CHit::LateTick(_float fTimeDelta)
{
	//ESC를 눌러서 Menu가 켜졌을 때 Render 끄기
	if (CK_DOWN(DIK_ESCAPE) && !m_bRender)
		m_bRender = true;

	else if (CK_DOWN(DIK_ESCAPE) && m_bRender)
		m_bRender = false;

	if (!m_bRender && !UM->PartyOn_State() && UM->Get_FrontUI() && !UM->Get_QuestOn() && !UM->Get_ShopState() && !UM->Get_StreetShop())
		__super::LateTick(fTimeDelta);

	if (!m_bRender && UM->Get_MoreCombo() && 0 != UM->Get_Combo() && !UM->PartyOn_State() && UM->Get_FrontUI() && !UM->Get_QuestOn() && !UM->Get_ShopState() && !UM->Get_StreetShop())
	{
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
}

HRESULT CHit::Render()
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

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;




	//UI_RenderDebug();


	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CHit::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hit"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}


void CHit::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();

	m_pUINum->Render_Debug();
}



CHit * CHit::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CHit*		pInstance = new CHit(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CHit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CHit::Clone(void * pArg)
{
	CHit*		pInstance = new CHit(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CHit"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CHit::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
