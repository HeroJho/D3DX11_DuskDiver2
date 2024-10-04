#include "stdafx.h"
#include "..\Public\Cross.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "ToolManager.h"



CCross::CCross(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CCross::CCross(const CCross & rhs)
	: CUI(rhs)
{
}

HRESULT CCross::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCross::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	if (FAILED(Make_ChildUI(2.f, 1.6f, 16.f, 18.4f, TEXT("Prototype_GameObject_Cross_Point"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, -73.f, 89.6f, 93.1f, TEXT("Prototype_GameObject_Team_Player01"))))
		return E_FAIL;

	/*if (FAILED(Make_ChildUI(-67.619957f, -4.161039f, 128.f, 128.f, TEXT("Prototype_GameObject_TeamOn_Effect"))))
		return E_FAIL;*/

	if (FAILED(Make_ChildUI(-69.5f, -10.f, 89.6f, 93.1f, TEXT("Prototype_GameObject_Team_Player03"))))
		return E_FAIL;

	/*if (FAILED(Make_ChildUI(67.619957f, -4.161039f, 128.f, 128.f, TEXT("Prototype_GameObject_TeamOn_Effect2"))))
		return E_FAIL;*/

	if (FAILED(Make_ChildUI(65.5f, -10.f, 89.6f, 93.1f, TEXT("Prototype_GameObject_Team_Player04"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-77.368858f, -137.639572f, 61.2f, 25.6f, TEXT("Prototype_GameObject_LockOn_Info"))))
		return E_FAIL;

	//UM->LEVIADA_On();
	//UM->DD_On();

	return S_OK;
}

void CCross::Tick(_float fTimeDelta)
{


	//UI_InputDebug(fTimeDelta);

	if (!m_bRender && !UM->PartyOn_State() && UM->Get_FrontUI() && !UM->Get_QuestOn() && !UM->Get_ShopState() && !UM->Get_StreetShop())
		__super::Tick(fTimeDelta);
}

void CCross::LateTick(_float fTimeDelta)
{
	//ESC를 눌러서 Menu가 켜졌을 때 Render 끄기
	if (CK_DOWN(DIK_ESCAPE) && !m_bRender)
		m_bRender = true;

	else if (CK_DOWN(DIK_ESCAPE) && m_bRender)
		m_bRender = false;

	if (!m_bRender && !UM->PartyOn_State() && UM->Get_FrontUI() && !UM->Get_QuestOn() && !UM->Get_ShopState() && !UM->Get_StreetShop())
		__super::LateTick(fTimeDelta);

	if (!m_bRender && !UM->PartyOn_State() && UM->Get_FrontUI() && !UM->Get_QuestOn() && !UM->Get_ShopState() && !UM->Get_StreetShop())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CCross::Render()
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






HRESULT CCross::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Cross00"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}


void CCross::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CCross * CCross::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCross*		pInstance = new CCross(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCross"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCross::Clone(void * pArg)
{
	CCross*		pInstance = new CCross(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCross"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CCross::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
