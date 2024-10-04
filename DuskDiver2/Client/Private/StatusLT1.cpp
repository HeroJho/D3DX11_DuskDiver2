#include "stdafx.h"
#include "..\Public\StatusLT1.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "LT1Level.h"
#include "LT1HP.h"
#include "UI_Manager.h"
#include "Status_Manager.h"



CStatusLT1::CStatusLT1(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CStatusLT1::CStatusLT1(const CStatusLT1 & rhs)
	: CUI(rhs)
{
}

HRESULT CStatusLT1::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStatusLT1::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	if (FAILED(Make_ChildUI(-113.3f, 40.84f, 10.f, 10.f, TEXT("Prototype_GameObject_LT1Level"))))
		return E_FAIL;

	vector<CUI*>::iterator iter = m_pChildUIs.begin();
	m_pUINum = (CLT1Level*)(*iter);


	if (FAILED(Make_ChildUI(100.f, 3.3f, 20.f, 20.f, TEXT("Prototype_GameObject_LT1HP"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter++;
	m_pUINum2 = (CLT1HP*)(*iter);


	return S_OK;
}

void CStatusLT1::Tick(_float fTimeDelta)
{
	m_pUINum->Set_Number((_uint)UM->Get_Player1Level());

	m_pUINum2->Set_Number((_uint)STM->Get_FinalHp(CStatus_Manager::PLAYER01));

	//UI_InputDebug(fTimeDelta);



	__super::Tick(fTimeDelta);
}

void CStatusLT1::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CStatusLT1::Render()
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

	if (UM->Get_BackNum() == 2)
	{
		if (UM->Get_PlayerSelectNum() == 1)
		{
			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;

			__super::Render();
		}
	}
	else if (UM->Get_BackNum() == 4)
	{
		if (UM->Get_EquipmentPlayer() == 1)
		{
			if (FAILED(m_pVIBufferCom->Render()))
				return E_FAIL;

			__super::Render();
		}
	}

	//UI_RenderDebug();


	return S_OK;
}






HRESULT CStatusLT1::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StatusLT1"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}







void CStatusLT1::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CStatusLT1 * CStatusLT1::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStatusLT1*		pInstance = new CStatusLT1(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStatusLT1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStatusLT1::Clone(void * pArg)
{
	CStatusLT1*		pInstance = new CStatusLT1(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStatusLT1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CStatusLT1::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
