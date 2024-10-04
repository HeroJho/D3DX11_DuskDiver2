#include "stdafx.h"
#include "..\Public\Coin_Num.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"
#include "Coin_NowNum.h"

CCoin_Num::CCoin_Num(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CCoin_Num::CCoin_Num(const CCoin_Num & rhs)
	: CUI(rhs)
{
}

HRESULT CCoin_Num::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCoin_Num::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	if (FAILED(Make_ChildUI(17.34f, 3.5f, 19.8f, 26.1f, TEXT("Prototype_GameObject_Coin_NowNum"))))
		return E_FAIL;

	vector<CUI*>::iterator iter = m_pChildUIs.begin();
	m_pUINum = (CCoin_NowNum*)(*iter);

	return S_OK;
}

void CCoin_Num::Tick(_float fTimeDelta)
{
	m_pUINum->Set_Number(UM->Get_Coin());

	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CCoin_Num::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);


	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CCoin_Num::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(10)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;



	//UI_RenderDebug();


	// 자식들의 Render 호출
	__super::Render();
 

	return S_OK;
}






HRESULT CCoin_Num::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Coin_Num"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CCoin_Num::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CCoin_Num * CCoin_Num::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCoin_Num*		pInstance = new CCoin_Num(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CCoin_Num"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCoin_Num::Clone(void * pArg)
{
	CCoin_Num*		pInstance = new CCoin_Num(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CCoin_Num"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CCoin_Num::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
