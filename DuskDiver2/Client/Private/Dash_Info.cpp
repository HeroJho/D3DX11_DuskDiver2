#include "stdafx.h"
#include "..\Public\Dash_Info.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "ToolManager.h"



CDash_Info::CDash_Info(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CDash_Info::CDash_Info(const CDash_Info & rhs)
	: CUI(rhs)
{
}

HRESULT CDash_Info::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDash_Info::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	if (FAILED(Make_ChildUI(-125.69f, 0.f, 153.6f, 25.6f, TEXT("Prototype_GameObject_Cross_Info"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(2.90168f, -193.49353f, 61.2f, 25.6f, TEXT("Prototype_GameObject_Dodge_Info"))))
		return E_FAIL;

	return S_OK;
}

void CDash_Info::Tick(_float fTimeDelta)
{

	//UI_InputDebug(fTimeDelta);

	if(!m_bRender && !UM->Get_QuestOn() && !UM->PartyOn_State() && UM->Get_FrontUI() && !UM->Get_ShopState() && !UM->Get_StreetShop())
		__super::Tick(fTimeDelta);
}

void CDash_Info::LateTick(_float fTimeDelta)
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

HRESULT CDash_Info::Render()
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






HRESULT CDash_Info::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dash_Info"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CDash_Info::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CDash_Info * CDash_Info::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDash_Info*		pInstance = new CDash_Info(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CDash_Info"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDash_Info::Clone(void * pArg)
{
	CDash_Info*		pInstance = new CDash_Info(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CDash_Info"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CDash_Info::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
