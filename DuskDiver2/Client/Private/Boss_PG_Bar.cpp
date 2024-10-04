#include "stdafx.h"
#include "..\Public\Boss_PG_Bar.h"
#include "GameInstance.h"
#include "Creture.h"
#include "Monster_Manager.h"
#include "ToolManager.h"
#include "UI_Manager.h"


CBoss_PG_Bar::CBoss_PG_Bar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CBoss_PG_Bar::CBoss_PG_Bar(const CBoss_PG_Bar & rhs)
	: CUI(rhs)
{
}

HRESULT CBoss_PG_Bar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_PG_Bar::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	m_pMonster = (CCreture*)m_UiInfo.pDesc;

	if (2000 > m_pMonster->Get_PatternGauge())
	{
		m_fMaxPG = m_pMonster->Get_MaxHp() * 0.4f;
		m_bFirst = true;
	}

	else
		m_fMaxPG = m_pMonster->Get_PatternGauge();

	return S_OK;
}

void CBoss_PG_Bar::Tick(_float fTimeDelta)
{

	
	//UI_InputDebug(fTimeDelta);


	if (!m_bRender && !UM->Get_QuestOn() && !UM->PartyOn_State() && UM->Get_FrontUI() && m_pMonster->Get_Spawn())
		__super::Tick(fTimeDelta);
}

void CBoss_PG_Bar::LateTick(_float fTimeDelta)
{
	//ESC를 눌러서 Menu가 켜졌을 때 Render 끄기
	if (CK_DOWN(DIK_ESCAPE) && !m_bRender)
		m_bRender = true;

	else if (CK_DOWN(DIK_ESCAPE) && m_bRender)
		m_bRender = false;
	
	m_fNowPG = m_pMonster->Get_PatternGauge();
	m_fPatternGauge = m_fNowPG / m_fMaxPG;

	m_fPrePG = m_pMonster->Get_PrePatternGauge();
	m_fPrePatternGauge = m_fPrePG / m_fMaxPG;


	if (0 >= m_pMonster->Get_NowHp())
		Set_Dead();

	if (!m_bRender && !UM->Get_QuestOn() && !UM->PartyOn_State() && UM->Get_FrontUI() && m_pMonster->Get_Spawn())
	{
		__super::LateTick(fTimeDelta);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
}

HRESULT CBoss_PG_Bar::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_BossPG", &m_fPatternGauge, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_BossPrePG", &m_fPrePatternGauge, sizeof(_float));

	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (!m_bFirst)
	{
		if (FAILED(m_pShaderCom->Begin(28)))
			return E_FAIL;
	}

	else
	{
		if (FAILED(m_pShaderCom->Begin(34)))
			return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	//UI_RenderDebug();


	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CBoss_PG_Bar::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boss_PG_Bar"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CBoss_PG_Bar::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CBoss_PG_Bar * CBoss_PG_Bar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBoss_PG_Bar*		pInstance = new CBoss_PG_Bar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBoss_PG_Bar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBoss_PG_Bar::Clone(void * pArg)
{
	CBoss_PG_Bar*		pInstance = new CBoss_PG_Bar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBoss_PG_Bar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CBoss_PG_Bar::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
