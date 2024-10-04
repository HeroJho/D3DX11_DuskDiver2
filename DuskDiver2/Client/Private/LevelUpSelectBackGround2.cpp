#include "stdafx.h"
#include "..\Public\LevelUpSelectBackGround2.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"


CLevelUpSelectBackGround2::CLevelUpSelectBackGround2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CLevelUpSelectBackGround2::CLevelUpSelectBackGround2(const CLevelUpSelectBackGround2 & rhs)
	: CUI(rhs)
{
}

HRESULT CLevelUpSelectBackGround2::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevelUpSelectBackGround2::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();




	return S_OK;
}

void CLevelUpSelectBackGround2::Tick(_float fTimeDelta)
{


	//UI_InputDebug(fTimeDelta);



	__super::Tick(fTimeDelta);
}

void CLevelUpSelectBackGround2::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CLevelUpSelectBackGround2::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (UM->Get_BackNum() == 1)
	{
		if (UM->Get_LevelUpSelectPageNum() == 2)
		{
			if (FAILED(m_pShaderCom->Begin(2)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pShaderCom->Begin(6)))
				return E_FAIL;
		}
	}

	else if (UM->Get_BackNum() == 3)
	{
		if (UM->Get_EquipSelectPageNum() == 2)
		{
			if (FAILED(m_pShaderCom->Begin(2)))
				return E_FAIL;
		}
		else
		{
			if (FAILED(m_pShaderCom->Begin(6)))
				return E_FAIL;
		}
	}

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	//UI_RenderDebug();


	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CLevelUpSelectBackGround2::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LevelUpSelect2"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}







void CLevelUpSelectBackGround2::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CLevelUpSelectBackGround2 * CLevelUpSelectBackGround2::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevelUpSelectBackGround2*		pInstance = new CLevelUpSelectBackGround2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevelUpSelectBackGround2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLevelUpSelectBackGround2::Clone(void * pArg)
{
	CLevelUpSelectBackGround2*		pInstance = new CLevelUpSelectBackGround2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CLevelUpSelectBackGround2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CLevelUpSelectBackGround2::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
