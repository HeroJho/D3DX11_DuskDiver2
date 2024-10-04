#include "stdafx.h"
#include "..\Public\Cross_Item.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "ToolManager.h"



CCross_Item::CCross_Item(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CCross_Item::CCross_Item(const CCross_Item & rhs)
	: CUI(rhs)
{
}

HRESULT CCross_Item::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCross_Item::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	if (FAILED(Make_ChildUI(-41.15f, 9.06f, 110.f, 110.f, TEXT("Prototype_GameObject_ItemUseEffect"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-39.f, 2.22f, 110.f, 110.f, TEXT("Prototype_GameObject_ItemEffect"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-1.84f, -28.3f, 110.f, 110.f, TEXT("Prototype_GameObject_ItemUseEffect1"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(2.38f, -38.f, 110.f, 110.f, TEXT("Prototype_GameObject_ItemEffect1"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(35.7f, 9.06f, 110.f, 110.f, TEXT("Prototype_GameObject_ItemUseEffect2"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(41.f, 2.22f, 110.f, 110.f, TEXT("Prototype_GameObject_ItemEffect2"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-1.84f, 44.36f, 110.f, 110.f, TEXT("Prototype_GameObject_ItemUseEffect3"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(2.38f, 39.36f, 110.f, 110.f, TEXT("Prototype_GameObject_ItemEffect3"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-36.538456f, 1.56039f, 38.4f, 38.4f, TEXT("Prototype_GameObject_Cross_Item_0"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(3.15296f, -37.468552f, 38.4f, 38.4f, TEXT("Prototype_GameObject_Cross_Item_1"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(42.218365f, 1.56039f, 38.4f, 38.4f, TEXT("Prototype_GameObject_Cross_Item_2"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(3.15296f, 41.344231f, 38.4f, 38.4f, TEXT("Prototype_GameObject_Cross_Item_3"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-841.f, 82.8f, 164.5f, 71.4f, TEXT("Prototype_GameObject_PartyBuffText"))))
		return E_FAIL;

	return S_OK;
}

void CCross_Item::Tick(_float fTimeDelta)
{
	//UI_InputDebug(fTimeDelta);
	CUI::UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));



	if (!m_bRender && !UM->Get_QuestOn() && !UM->PartyOn_State() && UM->Get_FrontUI() && !UM->Get_ShopState() && !UM->Get_StreetShop())
		__super::Tick(fTimeDelta);
}

void CCross_Item::LateTick(_float fTimeDelta)
{
	//ESC를 눌러서 Menu가 켜졌을 때 Render 끄기
	if (CK_DOWN(DIK_ESCAPE) && !m_bRender)
		m_bRender = true;

	else if (CK_DOWN(DIK_ESCAPE) && m_bRender)
		m_bRender = false;

	if (!m_bRender && !UM->Get_QuestOn() && !UM->PartyOn_State() && UM->Get_FrontUI() && !UM->Get_ShopState() && !UM->Get_StreetShop())
		__super::LateTick(fTimeDelta);

	if (!m_bRender && !UM->Get_QuestOn() && !UM->PartyOn_State() && UM->Get_FrontUI() && !UM->Get_ShopState() && !UM->Get_StreetShop())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CCross_Item::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(3)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;




	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CCross_Item::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Cross_Item_Background"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}


void CCross_Item::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CCross_Item * CCross_Item::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCross_Item*		pInstance = new CCross_Item(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCross_Item"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCross_Item::Clone(void * pArg)
{
	CCross_Item*		pInstance = new CCross_Item(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCross_Item"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CCross_Item::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
