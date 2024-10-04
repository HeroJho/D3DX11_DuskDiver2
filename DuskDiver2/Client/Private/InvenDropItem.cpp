#include "stdafx.h"
#include "..\Public\InvenDropItem.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"



CInvenDropItem::CInvenDropItem(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CInvenDropItem::CInvenDropItem(const CInvenDropItem & rhs)
	: CUI(rhs)
{
}

HRESULT CInvenDropItem::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CInvenDropItem::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	return S_OK;
}

void CInvenDropItem::Tick(_float fTimeDelta)
{
	m_iNum = UM->Get_InvenNum();


	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CInvenDropItem::LateTick(_float fTimeDelta)
{
	if (m_iNum != UM->Get_InvenNum() && (m_iNum == 10 || m_iNum == 3 || m_iNum == 4))
	{
		if (m_iNum > UM->Get_InvenNum())
			m_Side = R;
		else
			m_Side = L;

		if (UM->Get_InvenNum() == 2)
			m_Side = R;
	}


	if (m_iNum == 3)
	{
		if (m_UiInfo.fSizeX >= 201.6f)
		{
			m_UiInfo.fSizeX = 201.6f;
			m_UiInfo.fSizeY = 48.3f;
		}
		else if (m_UiInfo.fSizeX < 201.6f)
		{
			m_fTime += fTimeDelta;
			if (m_fTime > 0.01f)
			{
				m_UiInfo.fSizeX += 20.f;
				m_UiInfo.fSizeY += 4.f;
				m_bTexture = false;
				m_fTime = 0;
			}
		}

	}

	else
	{
		if (m_UiInfo.fSizeX <= 28.f)
		{
			m_UiInfo.fSizeX = 28.f;
			m_UiInfo.fSizeY = 28.f;
			m_bTexture = true;
		}
		else if (m_UiInfo.fSizeX > 28.f)
		{
			m_fTime += fTimeDelta;
			if (m_fTime > 0.01f)
			{
				m_UiInfo.fSizeX -= 20.f;
				m_UiInfo.fSizeY -= 4.f;
				m_fTime = 0;
			}

		}

	}

	if (m_iNum != 3)
	{
		if (m_Side == L)
		{
			if (m_UiInfo.fX <= 5.f)
				m_UiInfo.fX = 5.f;
			else
			{
				m_fTime2 += fTimeDelta;
				if (m_fTime2 > 0.01f)
				{
					m_UiInfo.fX -= 30.f;
				}
			}
		}
		if (m_Side == R)
		{
			if (m_UiInfo.fX >= 120.f)
				m_UiInfo.fX = 120.f;
			else
			{
				m_fTime2 += fTimeDelta;
				if (m_fTime2 > 0.01f)
				{
					m_UiInfo.fX += 30.f;
				}
			}
		}
	}
	else
	{
		if (m_UiInfo.fX >= 65.f)
			m_UiInfo.fX = 65.f;
		else
		{
			m_fTime2 += fTimeDelta;
			if (m_fTime2 > 0.01f)
			{
				m_UiInfo.fX += 30.f;
			}
		}
	}

	if (m_iNum == 2 || m_iNum == 0 || m_iNum == 1)
		m_UiInfo.fX = 120.5f;

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CInvenDropItem::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (!m_bTexture)
	{
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 1)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;
	}


	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;




	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CInvenDropItem::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InvenDropItem"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CInvenDropItem::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CInvenDropItem * CInvenDropItem::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CInvenDropItem*		pInstance = new CInvenDropItem(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CInvenDropItem"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CInvenDropItem::Clone(void * pArg)
{
	CInvenDropItem*		pInstance = new CInvenDropItem(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CInvenDropItem"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CInvenDropItem::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
