#include "stdafx.h"
#include "..\Public\ItemUseEffect3.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"


CItemUseEffect3::CItemUseEffect3(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CItemUseEffect3::CItemUseEffect3(const CItemUseEffect3 & rhs)
	: CUI(rhs)
{
}

HRESULT CItemUseEffect3::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CItemUseEffect3::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();




	return S_OK;
}

void CItemUseEffect3::Tick(_float fTimeDelta)
{

	if (CK_DOWN(DIK_DOWN))
	{
		m_bRender = true;
		m_fTime = 0.f;

		if (0.f == m_fTime && m_bSoundOn)
			m_bSoundOn = false;

		switch ((UM->Get_BattleItemNum(3)))
		{
		case 0:
			if (UM->Get_BattleItem(0) == 0)
			{
				m_bRed = true;
				Cancel_Sound();
			}
			else
			{
				m_bRed = false;
				Use_Sound();
			}
			break;
		case 1:
			if (UM->Get_BattleItem(1) == 0)
			{
				m_bRed = true;
				Cancel_Sound();
			}
			else
			{
				m_bRed = false;
				Use_Sound();
			}
			break;
		case 2:
			if (UM->Get_BattleItem(2) == 0)
			{
				m_bRed = true;
				Cancel_Sound();
			}
			else
			{
				m_bRed = false;
				Use_Sound();
			}
			break;
		case 3:
			if (UM->Get_BattleItem(3) == 0)
			{
				m_bRed = true;
				Cancel_Sound();
			}
			else
			{
				m_bRed = false;
				Use_Sound();
			}
			break;
		case 4:
			if (UM->Get_BattleItem(4) == 0)
			{
				m_bRed = true;
				Cancel_Sound();
			}
			else
			{
				m_bRed = false;
				Use_Sound();
			}
			break;
		case 5:
			if (UM->Get_BattleItem(5) == 0)
			{
				m_bRed = true;
				Cancel_Sound();
			}
			else
			{
				m_bRed = false;
				Use_Sound();
			}
			break;
		case 6:
			if (UM->Get_BattleItem(6) == 0)
			{
				m_bRed = true;
				Cancel_Sound();
			}
			else
			{
				m_bRed = false;
				Use_Sound();
			}
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

void CItemUseEffect3::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CItemUseEffect3::Render()
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



	if (m_bRed)
	{
		if (FAILED(m_pShaderCom->Begin(8)))
			return E_FAIL;
	}

	else
	{
		if (FAILED(m_pShaderCom->Begin(36)))
			return E_FAIL;
	}

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	//UI_RenderDebug();


	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CItemUseEffect3::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_LOADING, TEXT("Prototype_Component_Texture_LoadingBar"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CItemUseEffect3::Use_Sound()
{
	if (!m_bSoundOn)
	{
		GI->PlaySoundW(L"UI_GetRecoverItem.wav", SD_UI, 0.6f);
		m_bSoundOn = true;
	}
}

void CItemUseEffect3::Cancel_Sound()
{
	if (!m_bSoundOn)
	{
		GI->PlaySoundW(L"UI_Alert.wav", SD_UI, 0.2f);
		m_bSoundOn = true;
	}
}





void CItemUseEffect3::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CItemUseEffect3 * CItemUseEffect3::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CItemUseEffect3*		pInstance = new CItemUseEffect3(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CItemUseEffect3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CItemUseEffect3::Clone(void * pArg)
{
	CItemUseEffect3*		pInstance = new CItemUseEffect3(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CItemUseEffect3"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CItemUseEffect3::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
