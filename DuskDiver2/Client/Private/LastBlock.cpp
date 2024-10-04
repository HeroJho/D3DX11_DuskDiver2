#include "stdafx.h"
#include "..\Public\LastBlock.h"
#include "GameInstance.h"

#include "ToolManager.h"
#include "UI_Manager.h"


CLastBlock::CLastBlock(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CLastBlock::CLastBlock(const CLastBlock & rhs)
	: CUI(rhs)
{
}

HRESULT CLastBlock::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLastBlock::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CLastBlock::Tick(_float fTimeDelta)
{
	if (CK_DOWN(DIK_F7))
	{
		TM->Set_BossCutBlock(!TM->Get_BossCutBlock());
		
	}

	// 현실 세계
	if (TM->Get_BossCutBlock())
	{
		if (m_bTriger)
		{
			m_bTriger = false;
			m_fAlpha = 0.f;
			UM->Set_FrontUI(false);
		}

		if (1.f > m_fAlpha)
			m_fAlpha += 0.4f * fTimeDelta;
		else
			m_fAlpha = 1.f;
	}
	else
	{
		m_bTriger = true;
		m_fAlpha = 0.f;
	}



	__super::Tick(fTimeDelta);
}

void CLastBlock::LateTick(_float fTimeDelta)
{
	if (!TM->Get_BossCutBlock() && 0.f == m_fAlpha)
		return;

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CLastBlock::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));

	m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float));

	//_float fGama = 0.8f;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_Gama", &fGama, sizeof(_float))))
	//	return E_FAIL;

	//if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
	//	return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(6)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;





	return S_OK;
}






HRESULT CLastBlock::Ready_Components()
{


	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}





void CLastBlock::Handle_Hover()
{
	int a = 0;
}

void CLastBlock::Handle_Down()
{
	int a = 0;
}

void CLastBlock::Handle_Press()
{
	int a = 0;
}

void CLastBlock::Handle_Click()
{

}




void CLastBlock::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	ImGui::End();
}



CLastBlock * CLastBlock::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLastBlock*		pInstance = new CLastBlock(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CLastBlock"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLastBlock::Clone(void * pArg)
{
	CLastBlock*		pInstance = new CLastBlock(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CLastBlock"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CLastBlock::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
}
