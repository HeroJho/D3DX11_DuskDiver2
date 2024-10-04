#include "stdafx.h"
#include "..\Public\Boss_HP_Bar.h"
#include "GameInstance.h"
#include "Creture.h"
#include "Monster_Manager.h"
#include "ToolManager.h"
#include "Boss_HP_Number.h"
#include "UI_Manager.h"

CBoss_HP_Bar::CBoss_HP_Bar(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CBoss_HP_Bar::CBoss_HP_Bar(const CBoss_HP_Bar & rhs)
	: CUI(rhs)
{
}

HRESULT CBoss_HP_Bar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBoss_HP_Bar::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	if (FAILED(Make_ChildUI(223.235291f, -11.98f, 14.2f, 14.2f, TEXT("Prototype_GameObject_Boss_HP_Number"))))
		return E_FAIL;

	vector<CUI*>::iterator iter = m_pChildUIs.begin();
	m_pUINum = (CBoss_HP_Number*)(*iter);

	m_pMonster = (CCreture*)m_UiInfo.pDesc;
	m_fMaxHp = m_pMonster->Get_MaxHp();

	return S_OK;
}

void CBoss_HP_Bar::Tick(_float fTimeDelta)
{
	_float fHpPercent = m_fHpGauge * 100;

	if(0 < fHpPercent && 1 > fHpPercent)
		m_pUINum->Set_Number(1);
	else
		m_pUINum->Set_Number((_uint)fHpPercent);

	//UI_InputDebug(fTimeDelta);


	if (!m_bRender && !UM->Get_QuestOn() && !UM->PartyOn_State() && UM->Get_FrontUI() && m_pMonster->Get_Spawn())
		__super::Tick(fTimeDelta);
}

void CBoss_HP_Bar::LateTick(_float fTimeDelta)
{
	//ESC를 눌러서 Menu가 켜졌을 때 Render 끄기
	if (CK_DOWN(DIK_ESCAPE) && !m_bRender)
		m_bRender = true;

	else if (CK_DOWN(DIK_ESCAPE) && m_bRender)
		m_bRender = false;

	m_fNowHp = m_pMonster->Get_NowHp();
	m_fHpGauge = m_fNowHp / m_fMaxHp;

	m_fPreHp = m_pMonster->Get_PreHp();
	m_fPreHpGauge = m_fPreHp / m_fMaxHp;

	if (0 >= m_fNowHp)
		Set_Dead();

	if (!m_bRender && !UM->Get_QuestOn() && !UM->PartyOn_State() && UM->Get_FrontUI() && m_pMonster->Get_Spawn())
	{
		__super::LateTick(fTimeDelta);
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
	}
}

HRESULT CBoss_HP_Bar::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_BossHP", &m_fHpGauge, sizeof(_float));
	m_pShaderCom->Set_RawValue("g_BossPreHP", &m_fPreHpGauge, sizeof(_float));


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(33)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	//UI_RenderDebug();


	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}

HRESULT CBoss_HP_Bar::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boss_HP_Bar"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CBoss_HP_Bar::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CBoss_HP_Bar * CBoss_HP_Bar::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBoss_HP_Bar*		pInstance = new CBoss_HP_Bar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBoss_HP_Bar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBoss_HP_Bar::Clone(void * pArg)
{
	CBoss_HP_Bar*		pInstance = new CBoss_HP_Bar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBoss_HP_Bar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CBoss_HP_Bar::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
