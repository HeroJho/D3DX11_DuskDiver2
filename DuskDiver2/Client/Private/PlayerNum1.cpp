#include "stdafx.h"
#include "..\Public\PlayerNum1.h"
#include "GameInstance.h"


#include "ToolManager.h"



CPlayerNum1::CPlayerNum1(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CPlayerNum1::CPlayerNum1(const CPlayerNum1 & rhs)
	: CUI(rhs)
{
}

HRESULT CPlayerNum1::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerNum1::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();




	return S_OK;
}

void CPlayerNum1::Tick(_float fTimeDelta)
{

	if (CK_DOWN(DIK_I))
	{
		//Open();
	}
	else if (CK_DOWN(DIK_O))
	{
		//Close();
	}





	UI_InputDebug(fTimeDelta);



	__super::Tick(fTimeDelta);
}

void CPlayerNum1::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CPlayerNum1::Render()
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

	UI_RenderDebug();


	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CPlayerNum1::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_TEST, TEXT("Prototype_Component_Texture_PlayerNum1"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}





void CPlayerNum1::Handle_Hover()
{
	int a = 0;
}

void CPlayerNum1::Handle_Down()
{
	int a = 0;
}

void CPlayerNum1::Handle_Press()
{
	int a = 0;
}

void CPlayerNum1::Handle_Click()
{

}




void CPlayerNum1::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CPlayerNum1 * CPlayerNum1::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayerNum1*		pInstance = new CPlayerNum1(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayerNum1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CPlayerNum1::Clone(void * pArg)
{
	CPlayerNum1*		pInstance = new CPlayerNum1(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CPlayerNum1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CPlayerNum1::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
