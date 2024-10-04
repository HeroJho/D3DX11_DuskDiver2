#include "stdafx.h"
#include "..\Public\Task_BackGround.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "ToolManager.h"



CTask_BackGround::CTask_BackGround(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CTask_BackGround::CTask_BackGround(const CTask_BackGround & rhs)
	: CUI(rhs)
{
}

HRESULT CTask_BackGround::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTask_BackGround::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	if (FAILED(Make_ChildUI(-8.34f, 0.83f, 263.4f, 67.8f, TEXT("Prototype_GameObject_Task_BackGround2"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-4.135f, 1.f, 285.35f, 73.45f, TEXT("Prototype_GameObject_Task_Contents"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-47.1f, -27.46f, 111.f, 43.5f, TEXT("Prototype_GameObject_Task_Title"))))
		return E_FAIL;



	return S_OK;
}

void CTask_BackGround::Tick(_float fTimeDelta)
{



	//UI_InputDebug(fTimeDelta);

	if (!m_bRender && UM->Get_FrontUI() && !UM->Get_QuestOn() && !UM->PartyOn_State() && !UM->Get_ShopState() && !UM->Get_StreetShop())
		__super::Tick(fTimeDelta);
}

void CTask_BackGround::LateTick(_float fTimeDelta)
{
	//ESC를 눌러서 Menu가 켜졌을 때 Render 끄기
	if (CK_DOWN(DIK_ESCAPE) && !m_bRender)
		m_bRender = true;

	else if (CK_DOWN(DIK_ESCAPE) && m_bRender)
		m_bRender = false;
	
	if (!m_bRender && UM->Get_FrontUI() && !UM->Get_QuestOn() && !UM->PartyOn_State() && !UM->Get_ShopState() && !UM->Get_StreetShop())
		__super::LateTick(fTimeDelta);

	if (!m_bRender && UM->Get_FrontUI() && !UM->Get_QuestOn() && !UM->PartyOn_State() && !UM->Get_ShopState() && !UM->Get_StreetShop())
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CTask_BackGround::Render()
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






HRESULT CTask_BackGround::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Task_BackGround"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CTask_BackGround::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CTask_BackGround * CTask_BackGround::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTask_BackGround*		pInstance = new CTask_BackGround(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CTask_BackGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CTask_BackGround::Clone(void * pArg)
{
	CTask_BackGround*		pInstance = new CTask_BackGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CTask_BackGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CTask_BackGround::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
