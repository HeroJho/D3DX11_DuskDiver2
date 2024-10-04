#include "stdafx.h"
#include "..\Public\MenuBar.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "MenuNumber1.h"
#include "MenuNumber2.h"
#include "MenuNumber3.h"
#include "UI_Manager.h"



CMenuBar::CMenuBar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CMenuBar::CMenuBar(const CMenuBar & rhs)
	: CUI(rhs)
{
}

HRESULT CMenuBar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMenuBar::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	if (FAILED(Make_ChildUI(42.f, -42.8f, 20.f, 20.f, TEXT("Prototype_GameObject_MenuNumber1"))))
		return E_FAIL;

	vector<CUI*>::iterator iter = m_pChildUIs.begin();
	m_pUINum = (CMenuNumber1*)(*iter);

	if (FAILED(Make_ChildUI(221.16f, -42.8f, 20.f, 20.f, TEXT("Prototype_GameObject_MenuNumber2"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter++;
	m_pUINum2 = (CMenuNumber2*)(*iter);

	if (FAILED(Make_ChildUI(400.f, -42.8f, 20.f, 20.f, TEXT("Prototype_GameObject_MenuNumber3"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 2;
	m_pUINum3 = (CMenuNumber3*)(*iter);


	return S_OK;
}

void CMenuBar::Tick(_float fTimeDelta)
{
	m_pUINum->Set_Number(UM->Get_Exp());
	m_pUINum2->Set_Number(UM->Get_Money());
	m_pUINum3->Set_Number(0);
	//UI_InputDebug(fTimeDelta);



	__super::Tick(fTimeDelta);
}

void CMenuBar::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMenuBar::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(0)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	//UI_RenderDebug();


	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CMenuBar::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuBar"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}






void CMenuBar::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CMenuBar * CMenuBar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMenuBar*		pInstance = new CMenuBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMenuBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMenuBar::Clone(void * pArg)
{
	CMenuBar*		pInstance = new CMenuBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMenuBar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CMenuBar::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
