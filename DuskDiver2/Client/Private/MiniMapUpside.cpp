#include "stdafx.h"
#include "..\Public\MiniMapUpside.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"



CMiniMapUpside::CMiniMapUpside(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CMiniMapUpside::CMiniMapUpside(const CMiniMapUpside & rhs)
	: CUI(rhs)
{
}

HRESULT CMiniMapUpside::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMiniMapUpside::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CMiniMapUpside::Tick(_float fTimeDelta)
{



	//UI_InputDebug(fTimeDelta);

	if (UM->Get_FrontUI() && !UM->Get_QuestOn() && !UM->PartyOn_State() && !UM->Get_ShopState() && !UM->Get_StreetShop())
		__super::Tick(fTimeDelta);
}

void CMiniMapUpside::LateTick(_float fTimeDelta)
{
	if (UM->Get_FrontUI() && !UM->Get_QuestOn() && !UM->PartyOn_State() && !UM->Get_ShopState() && !UM->Get_StreetShop())
		__super::LateTick(fTimeDelta);

	if (UM->Get_FrontUI() && !UM->Get_QuestOn() && !UM->PartyOn_State() && !UM->Get_ShopState() && !UM->Get_StreetShop())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMiniMapUpside::Render()
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






HRESULT CMiniMapUpside::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniMapUpside"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CMiniMapUpside::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CMiniMapUpside * CMiniMapUpside::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMiniMapUpside*		pInstance = new CMiniMapUpside(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMiniMapUpside"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMiniMapUpside::Clone(void * pArg)
{
	CMiniMapUpside*		pInstance = new CMiniMapUpside(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMiniMapUpside"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CMiniMapUpside::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
