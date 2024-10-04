#include "stdafx.h"
#include "..\Public\Boss_HP.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "ToolManager.h"
#include "Creture.h"
#include "Monster_Manager.h"

CBoss_HP::CBoss_HP(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CBoss_HP::CBoss_HP(const CBoss_HP & rhs)
	: CUI(rhs)
{
}

HRESULT CBoss_HP::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_HP::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	m_pMonster = (CCreture*)m_UiInfo.pDesc;

	if (FAILED(Make_ChildUI(-0.478145f, 0.f, 524.f, 42.f, TEXT("Prototype_GameObject_Boss_Name"), pArg)))
		return E_FAIL;

	//if (FAILED(Make_ChildUI(0.f, 0.f, 467.f, 42.f, TEXT("Prototype_GameObject_Boss_HP_Bar"), pArg)))
	//	return E_FAIL;

	//if (FAILED(Make_ChildUI(0.f, 0.f, 467.f, 42.f, TEXT("Prototype_GameObject_Boss_PG_Bar"), pArg)))
	//	return E_FAIL;

	return S_OK;
}

void CBoss_HP::Tick(_float fTimeDelta)
{
	if (0 >= m_pMonster->Get_NowHp())
	{
		m_fDeadAcc += 0.1f * fTimeDelta;

		if (m_fDeadAcc >= 0.2f)
		{
			m_fDeadAcc = 0.2f;
			Set_Dead();
		}
	}


	//UI_InputDebug(fTimeDelta);

	if (!m_bRender && !UM->Get_QuestOn() && !UM->PartyOn_State() && UM->Get_FrontUI() && m_pMonster->Get_Spawn()
		)
		__super::Tick(fTimeDelta);
}

void CBoss_HP::LateTick(_float fTimeDelta)
{
	//ESC를 눌러서 Menu가 켜졌을 때 Render 끄기
	if (CK_DOWN(DIK_ESCAPE) && !m_bRender)
		m_bRender = true;

	else if (CK_DOWN(DIK_ESCAPE) && m_bRender)
		m_bRender = false;
	
	if (!m_bRender && !UM->Get_QuestOn() && !UM->PartyOn_State() && UM->Get_FrontUI() && m_pMonster->Get_Spawn())
		__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CBoss_HP::Render()
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

	if (!m_bRender && UM->Get_FrontUI() && m_pMonster->Get_Spawn() && !UM->Get_QuestOn())
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}


//	UI_RenderDebug();

	if (!m_bRender && UM->Get_FrontUI() && m_pMonster->Get_Spawn() && !UM->Get_QuestOn())
	{
		// 자식들의 Render 호출
		__super::Render();
	}

	return S_OK;
}






HRESULT CBoss_HP::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boss_HP"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CBoss_HP::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CBoss_HP * CBoss_HP::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBoss_HP*		pInstance = new CBoss_HP(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBoss_HP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBoss_HP::Clone(void * pArg)
{
	CBoss_HP*		pInstance = new CBoss_HP(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBoss_HP"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CBoss_HP::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
