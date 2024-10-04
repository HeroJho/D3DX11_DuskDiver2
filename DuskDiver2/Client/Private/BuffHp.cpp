#include "stdafx.h"
#include "..\Public\BuffHp.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Status_Manager.h"
#include "ToolManager.h"
#include "BuffHpNum.h"


CBuffHp::CBuffHp(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CBuffHp::CBuffHp(const CBuffHp & rhs)
	: CUI(rhs)
{
}

HRESULT CBuffHp::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBuffHp::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	if (FAILED(Make_ChildUI(24.f, 17.f, 13.f, 13.f, TEXT("Prototype_GameObject_BuffHpNum"))))
		return E_FAIL;

	vector<CUI*>::iterator iter = m_pChildUIs.begin();
	m_pUINum = (CBuffHpNum*)(*iter);

	return S_OK;
}

void CBuffHp::Tick(_float fTimeDelta)
{
	if (m_fTime > 0.f)
		m_fTime -= fTimeDelta;
	else
	{
		m_fTime = 0;
		m_bEat = false;
	}

	if (UM->Get_FoodHp())
	{
		m_fTime += 99.f;
		UM->Set_FoodHp();
		m_bEat = true;
		m_fAlpha = 0.f;
	}

	if (UM->Get_FoodStrTime() > 0 && UM->Get_FoodHpTime() > 0)
	{
		m_UiInfo.fX = 20.f;
	}
	else
	{
		m_UiInfo.fX = -3.f;
	}
	if (m_fTime <= 11.f && m_bEat)
	{
		for (_int i = 0; i < 11; ++i)
		{
			if ((_int)m_fTime % 2 == 0)
			{
				if (m_fAlpha < 0.4f)
					m_fAlpha += fTimeDelta * 0.02f;
				else
					m_fAlpha = 0.4f;
			}
			else
			{
				if (m_fAlpha > 0)
					m_fAlpha -= fTimeDelta * 0.02f;
				else
					m_fAlpha = 0;
			}
		}
	}

	if ((_int)m_fTime % 5 == 0 && !m_bEffect)
	{
		if (!m_bStart)
		{
			m_bStart = true;
			m_bEffect = true;
		}
		else
		{
			if (FAILED(Make_ChildUI(0.f, 0.f, 40.f, 41.f, TEXT("Prototype_GameObject_BuffEffect"))))
				return;
			m_bEffect = true;
		}
	}
	else if ((_int)m_fTime % 5 != 0)
	{
		m_bEffect = false;
	}


	//UI_InputDebug(fTimeDelta);

	UM->Set_FoodHpTime(m_fTime);

	m_pUINum->Set_Number((_uint)m_fTime);

	__super::Tick(fTimeDelta);
}

void CBuffHp::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CBuffHp::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_TimeDelta", &m_fAlpha, sizeof(_float));


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(7)))
		return E_FAIL;


	if (m_fTime > 0)
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
		STM->Set_FoodHp(300, CStatus_Manager::PLAYER01);
		STM->Set_FoodHp(300, CStatus_Manager::PLAYER03);
		STM->Set_FoodHp(300, CStatus_Manager::PLAYER04);
		STM->Set_FinalHp(CStatus_Manager::PLAYER01);
		STM->Set_FinalHp(CStatus_Manager::PLAYER03);
		STM->Set_FinalHp(CStatus_Manager::PLAYER04);
		__super::Render();
	}
	else
	{
		STM->Set_FoodHp(0, CStatus_Manager::PLAYER01);
		STM->Set_FoodHp(0, CStatus_Manager::PLAYER03);
		STM->Set_FoodHp(0, CStatus_Manager::PLAYER04);
		STM->Set_FinalHp(CStatus_Manager::PLAYER01);
		STM->Set_FinalHp(CStatus_Manager::PLAYER03);
		STM->Set_FinalHp(CStatus_Manager::PLAYER04);
	}





	//UI_RenderDebug();

	// 자식들의 Render 호출

	return S_OK;
}






HRESULT CBuffHp::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BuffHp"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CBuffHp::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CBuffHp * CBuffHp::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBuffHp*		pInstance = new CBuffHp(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBuffHp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBuffHp::Clone(void * pArg)
{
	CBuffHp*		pInstance = new CBuffHp(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBuffHp"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CBuffHp::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
