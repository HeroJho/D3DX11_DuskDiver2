#include "stdafx.h"
#include "..\Public\EquipmentStatus.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"
#include "Status_Manager.h"
#include "EquipmentStr.h"
#include "EquipmentStamina.h"
#include "EquipmentHp.h"
#include "EquipmentMental.h"
#include "EquipmentMeta.h"
#include "EquipmentExplosive.h"
#include "EquipmentLuck.h"
#include "EquipmentLv.h"



CEquipmentStatus::CEquipmentStatus(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CEquipmentStatus::CEquipmentStatus(const CEquipmentStatus & rhs)
	: CUI(rhs)
{
}

HRESULT CEquipmentStatus::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEquipmentStatus::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	if (FAILED(Make_ChildUI(153.f, 84.5f, 20.f, 20.f, TEXT("Prototype_GameObject_EquipmentStr"))))
		return E_FAIL;

	vector<CUI*>::iterator iter = m_pChildUIs.begin();
	m_pUINum = (CEquipmentStr*)(*iter);

	if (FAILED(Make_ChildUI(153.f, 119.5f, 20.f, 20.f, TEXT("Prototype_GameObject_EquipmentStamina"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter++;
	m_pStaminaNum = (CEquipmentStamina*)(*iter);

	if (FAILED(Make_ChildUI(153.f, -20.5f, 20.f, 20.f, TEXT("Prototype_GameObject_EquipmentHp"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 2;
	m_pHpNum = (CEquipmentHp*)(*iter);

	if (FAILED(Make_ChildUI(153.f,15.5f, 20.f, 20.f, TEXT("Prototype_GameObject_EquipmentMental"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 3;
	m_pMentalNum = (CEquipmentMental*)(*iter);

	if (FAILED(Make_ChildUI(153.f, 50.5f, 20.f, 20.f, TEXT("Prototype_GameObject_EquipmentMeta"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 4;
	m_pMetaNum = (CEquipmentMeta*)(*iter);


	if (FAILED(Make_ChildUI(153.f, 189.5f, 20.f, 20.f, TEXT("Prototype_GameObject_EquipmentExplosive"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 5;
	m_pExplosiveNum = (CEquipmentExplosive*)(*iter);

	if (FAILED(Make_ChildUI(153.f, 154.5f, 20.f, 20.f, TEXT("Prototype_GameObject_EquipmentLuck"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 6;
	m_pLuckNum = (CEquipmentLuck*)(*iter);

	if (FAILED(Make_ChildUI(153.f, -55.5f, 20.f, 20.f, TEXT("Prototype_GameObject_EquipmentLv"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 7;
	m_pLvNum = (CEquipmentLv*)(*iter);


	return S_OK;
}

void CEquipmentStatus::Tick(_float fTimeDelta)
{
	if (UM->Get_EquipmentPlayer() == 1)
	{
		m_pUINum->Set_Number((_uint)(STM->Get_FinalStr(CStatus_Manager::PLAYER01)));
		m_pStaminaNum->Set_Number((_uint)(STM->Get_FinalStamina(CStatus_Manager::PLAYER01)));
		m_pHpNum->Set_Number((_uint)(STM->Get_FinalHp(CStatus_Manager::PLAYER01)));
		m_pMentalNum->Set_Number((_uint)(STM->Get_FinalMental(CStatus_Manager::PLAYER01)));
		m_pMetaNum->Set_Number((_uint)(STM->Get_FinalMeta(CStatus_Manager::PLAYER01)));
		m_pExplosiveNum->Set_Number((_uint)(STM->Get_FinalExplosive(CStatus_Manager::PLAYER01)));
		m_pLuckNum->Set_Number((_uint)(STM->Get_FinalCritical(CStatus_Manager::PLAYER01)));
		m_pLvNum->Set_Number(UM->Get_Player1Level());
	}

	else if(UM->Get_EquipmentPlayer() == 2)
	{
		m_pUINum->Set_Number((_uint)(STM->Get_FinalStr(CStatus_Manager::PLAYER03)));
		m_pStaminaNum->Set_Number((_uint)(STM->Get_FinalStamina(CStatus_Manager::PLAYER03)));
		m_pHpNum->Set_Number((_uint)(STM->Get_FinalHp(CStatus_Manager::PLAYER03)));
		m_pMentalNum->Set_Number((_uint)(STM->Get_FinalMental(CStatus_Manager::PLAYER03)));
		m_pMetaNum->Set_Number((_uint)(STM->Get_FinalMeta(CStatus_Manager::PLAYER03)));
		m_pExplosiveNum->Set_Number((_uint)(STM->Get_FinalExplosive(CStatus_Manager::PLAYER03)));
		m_pLuckNum->Set_Number((_uint)(STM->Get_FinalCritical(CStatus_Manager::PLAYER03)));
		m_pLvNum->Set_Number(UM->Get_Player2Level());
	}

	else if (UM->Get_EquipmentPlayer() == 3)
	{
		m_pUINum->Set_Number((_uint)(STM->Get_FinalStr(CStatus_Manager::PLAYER04)));
		m_pStaminaNum->Set_Number((_uint)(STM->Get_FinalStamina(CStatus_Manager::PLAYER04)));
		m_pHpNum->Set_Number((_uint)(STM->Get_FinalHp(CStatus_Manager::PLAYER04)));
		m_pMentalNum->Set_Number((_uint)(STM->Get_FinalMental(CStatus_Manager::PLAYER04)));
		m_pMetaNum->Set_Number((_uint)(STM->Get_FinalMeta(CStatus_Manager::PLAYER04)));
		m_pExplosiveNum->Set_Number((_uint)(STM->Get_FinalExplosive(CStatus_Manager::PLAYER04)));
		m_pLuckNum->Set_Number((_uint)(STM->Get_FinalCritical(CStatus_Manager::PLAYER04)));
		m_pLvNum->Set_Number(UM->Get_Player3Level());
	}


	//UI_InputDebug(fTimeDelta);

	__super::Tick(fTimeDelta);
}

void CEquipmentStatus::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CEquipmentStatus::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));



	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(2)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;




	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CEquipmentStatus::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EquipmentStatus"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CEquipmentStatus::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CEquipmentStatus * CEquipmentStatus::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEquipmentStatus*		pInstance = new CEquipmentStatus(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEquipmentStatus"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEquipmentStatus::Clone(void * pArg)
{
	CEquipmentStatus*		pInstance = new CEquipmentStatus(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEquipmentStatus"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CEquipmentStatus::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
