#include "stdafx.h"
#include "..\Public\StatusLT3.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "LT3Level.h"
#include "LT3HP.h"
#include "UI_Manager.h"
#include "Status_Manager.h"



CStatusLT3::CStatusLT3(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CStatusLT3::CStatusLT3(const CStatusLT3 & rhs)
	: CUI(rhs)
{
}

HRESULT CStatusLT3::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStatusLT3::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	if (FAILED(Make_ChildUI(-113.3f, 40.84f, 10.f, 10.f, TEXT("Prototype_GameObject_LT3Level"))))
		return E_FAIL;

	vector<CUI*>::iterator iter = m_pChildUIs.begin();
	m_pUINum = (CLT3Level*)(*iter);



	if (FAILED(Make_ChildUI(100.f, 3.3f, 20.f, 20.f, TEXT("Prototype_GameObject_LT3HP"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter++;
	m_pUINum2 = (CLT3HP*)(*iter);

	return S_OK;
}

void CStatusLT3::Tick(_float fTimeDelta)
{
	m_pUINum->Set_Number(UM->Get_Player3Level());

	m_pUINum2->Set_Number((_uint)STM->Get_FinalHp(CStatus_Manager::PLAYER04));

	//UI_InputDebug(fTimeDelta);


	if (UM->DD_State() == true)
		__super::Tick(fTimeDelta);
}

void CStatusLT3::LateTick(_float fTimeDelta)
{
	if (UM->DD_State() == true)
		__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CStatusLT3::Render()
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

	if (UM->DD_State() == true)
	{
		if (UM->Get_BackNum() == 2)
		{
			if (UM->Get_PlayerSelectNum() == 3)
			{
				if (FAILED(m_pVIBufferCom->Render()))
					return E_FAIL;

				__super::Render();
			}

		}
		else if (UM->Get_BackNum() == 4)
		{
			if (UM->Get_EquipmentPlayer() == 3)
			{
				if (FAILED(m_pVIBufferCom->Render()))
					return E_FAIL;

				__super::Render();
			}
		}
	}

	// 자식들의 Render 호출


	return S_OK;
}






HRESULT CStatusLT3::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StatusLT3"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}







void CStatusLT3::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CStatusLT3 * CStatusLT3::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStatusLT3*		pInstance = new CStatusLT3(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStatusLT3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStatusLT3::Clone(void * pArg)
{
	CStatusLT3*		pInstance = new CStatusLT3(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStatusLT3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CStatusLT3::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
