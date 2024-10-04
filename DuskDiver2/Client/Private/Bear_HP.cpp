#include "stdafx.h"
#include "..\Public\Bear_HP.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "ToolManager.h"
#include "Creture.h"
#include "Monster_Manager.h"
#include "Bear.h"

#include "ToolManager.h"
#include "Bear_HP_Bar.h"

CBear_HP::CBear_HP(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CBear_HP::CBear_HP(const CBear_HP & rhs)
	: CUI(rhs)
{
}

HRESULT CBear_HP::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBear_HP::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	if (FAILED(Make_ChildUI(-0.478145f, 0.f, 524.f, 42.f, TEXT("Prototype_GameObject_Bear_Name"), pArg)))
		return E_FAIL;

	return S_OK;
}

void CBear_HP::Tick(_float fTimeDelta)
{
	
	if (0 >= TM->Get_CumaHpBar()->Get_HP())
	{
		m_fDeadAcc += 0.1f * fTimeDelta;

		if (m_fDeadAcc >= 0.2f)
		{
			m_fDeadAcc = 0.2f;
			Set_Dead();
		}
	}


	//UI_InputDebug(fTimeDelta);

	if (!m_bRender && !UM->Get_QuestOn() && !UM->PartyOn_State() && UM->Get_FrontUI() && !UM->Get_ShopState())
		__super::Tick(fTimeDelta);
}

void CBear_HP::LateTick(_float fTimeDelta)
{
	//ESC를 눌러서 Menu가 켜졌을 때 Render 끄기
	if (CK_DOWN(DIK_ESCAPE) && !m_bRender)
		m_bRender = true;

	else if (CK_DOWN(DIK_ESCAPE) && m_bRender)
		m_bRender = false;
	
	if (!m_bRender && !UM->Get_QuestOn() && !UM->PartyOn_State() && UM->Get_FrontUI() && !UM->Get_ShopState()/* && m_pMonster->Get_Spawn()*/)
		__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CBear_HP::Render()
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

	if (!m_bRender && UM->Get_FrontUI()/* && m_pMonster->Get_Spawn()*/)
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}


//	UI_RenderDebug();

	if (!m_bRender && UM->Get_FrontUI()/* && m_pMonster->Get_Spawn()*/)
	{
		// 자식들의 Render 호출
		__super::Render();
	}

	return S_OK;
}






HRESULT CBear_HP::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boss_HP"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CBear_HP::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CBear_HP * CBear_HP::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBear_HP*		pInstance = new CBear_HP(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBear_HP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBear_HP::Clone(void * pArg)
{
	CBear_HP*		pInstance = new CBear_HP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBear_HP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CBear_HP::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
