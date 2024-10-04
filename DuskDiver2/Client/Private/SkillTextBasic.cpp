#include "stdafx.h"
#include "..\Public\SkillTextBasic.h"
#include "GameInstance.h"


#include "ToolManager.h"



CSkillTextBasic::CSkillTextBasic(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CSkillTextBasic::CSkillTextBasic(const CSkillTextBasic & rhs)
	: CUI(rhs)
{
}

HRESULT CSkillTextBasic::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkillTextBasic::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CSkillTextBasic::Tick(_float fTimeDelta)
{

	/*if (CK_DOWN(DIK_V) && !m_bRender)
	{
		m_bRender = true;
	}
	else if (CK_DOWN(DIK_V) && m_bRender)
	{
		m_bRender = false;
	}*/


	if (CK_DOWN(DIK_RETURN) && m_bRender)
	{
		Set_Dead();
	}

	//UI_InputDebug(fTimeDelta);


	if(m_bRender)
		__super::Tick(fTimeDelta);
}

void CSkillTextBasic::LateTick(_float fTimeDelta)
{
	if (m_bRender)
		__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CSkillTextBasic::Render()
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

	if (m_bRender)	
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;




	//UI_RenderDebug();

	// �ڽĵ��� Render ȣ��
	__super::Render();

	return S_OK;
}






HRESULT CSkillTextBasic::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkillTextBasic"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CSkillTextBasic::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CSkillTextBasic * CSkillTextBasic::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSkillTextBasic*		pInstance = new CSkillTextBasic(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CSkillTextBasic"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSkillTextBasic::Clone(void * pArg)
{
	CSkillTextBasic*		pInstance = new CSkillTextBasic(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CSkillTextBasic"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CSkillTextBasic::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
