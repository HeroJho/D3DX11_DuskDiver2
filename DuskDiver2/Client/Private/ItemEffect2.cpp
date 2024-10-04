#include "stdafx.h"
#include "..\Public\ItemEffect2.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"


CItemEffect2::CItemEffect2(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CItemEffect2::CItemEffect2(const CItemEffect2 & rhs)
	: CUI(rhs)
{
}

HRESULT CItemEffect2::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItemEffect2::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();




	return S_OK;
}

void CItemEffect2::Tick(_float fTimeDelta)
{

	if (CK_DOWN(DIK_RIGHT))
	{
		m_bRender = true;
		m_fTime = 0.f;


		switch ((UM->Get_BattleItemNum(2)))
		{
		case 0:
			if (UM->Get_BattleItem(0) == 0)
				m_bRed = true;
			else
				m_bRed = false;
			break;
		case 1:
			if (UM->Get_BattleItem(1) == 0)
				m_bRed = true;
			else
				m_bRed = false;
			break;
		case 2:
			if (UM->Get_BattleItem(2) == 0)
				m_bRed = true;
			else
				m_bRed = false;
			break;
		case 3:
			if (UM->Get_BattleItem(3) == 0)
				m_bRed = true;
			else
				m_bRed = false;
			break;
		case 4:
			if (UM->Get_BattleItem(4) == 0)
				m_bRed = true;
			else
				m_bRed = false;
			break;
		case 5:
			if (UM->Get_BattleItem(5) == 0)
				m_bRed = true;
			else
				m_bRed = false;
			break;
		case 6:
			if (UM->Get_BattleItem(6) == 0)
				m_bRed = true;
			else
				m_bRed = false;
			break;
		default:
			m_bRed = false;
			break;
		}
	}

	if(m_bRender)
		m_fTime += fTimeDelta;




	//UI_InputDebug(fTimeDelta);



	__super::Tick(fTimeDelta);
}

void CItemEffect2::LateTick(_float fTimeDelta)
{
	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CItemEffect2::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (m_fTime >= 1.28f)
	{
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 16)))
			return E_FAIL;
	}

	for (int i = 0; i < 16; i++)
	{
		if (m_fTime >= i * 0.08f && m_fTime < (i + 1) * 0.08f)
		{
			if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", i)))
				return E_FAIL;
			break;
		}
	}


		if (FAILED(m_pShaderCom->Begin(2)))
			return E_FAIL;


	if (!m_bRed)
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}

	//UI_RenderDebug();


	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CItemEffect2::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ItemBackEffect"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}







void CItemEffect2::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CItemEffect2 * CItemEffect2::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CItemEffect2*		pInstance = new CItemEffect2(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CItemEffect2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CItemEffect2::Clone(void * pArg)
{
	CItemEffect2*		pInstance = new CItemEffect2(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CItemEffect2"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CItemEffect2::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
