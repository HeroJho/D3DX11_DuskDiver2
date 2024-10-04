#include "stdafx.h"
#include "..\Public\BattleTag.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Player_Manager.h"



CBattleTag::CBattleTag(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CBattleTag::CBattleTag(const CBattleTag & rhs)
	: CUI(rhs)
{
}

HRESULT CBattleTag::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBattleTag::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	276;


	return S_OK;
}

void CBattleTag::Tick(_float fTimeDelta)
{


	//UI_InputDebug(fTimeDelta);

	if (CPlayer::PLAYER_01 == PM->Get_SelectPlayer())
		m_UiInfo.fX = 10.f;
	if (CPlayer::PLAYER_03 == PM->Get_SelectPlayer())
		m_UiInfo.fX = 256.f + 10.f;
	if (CPlayer::PLAYER_04 == PM->Get_SelectPlayer())
		m_UiInfo.fX = 512.f + 10.f;


	__super::Tick(fTimeDelta);
}

void CBattleTag::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CBattleTag::Render()
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

	//UI_RenderDebug();


	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CBattleTag::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BattleTag"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}







void CBattleTag::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CBattleTag * CBattleTag::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBattleTag*		pInstance = new CBattleTag(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBattleTag"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBattleTag::Clone(void * pArg)
{
	CBattleTag*		pInstance = new CBattleTag(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBattleTag"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CBattleTag::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
