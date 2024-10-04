#include "stdafx.h"
#include "..\Public\IllustDD.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"
#include "IllustDDHp.h"
#include "IllustDDLv.h"
#include "IllustDDSp.h"
#include "IllustDDAttack.h"
#include "IllustDDArmor.h"
#include "Status_Manager.h"



CIllustDD::CIllustDD(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CIllustDD::CIllustDD(const CIllustDD & rhs)
	: CUI(rhs)
{
}

HRESULT CIllustDD::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CIllustDD::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	if (FAILED(Make_ChildUI(-38.f, 169.f, 20.f, 20.f, TEXT("Prototype_GameObject_IllustDDHp"))))
		return E_FAIL;

	vector<CUI*>::iterator iter = m_pChildUIs.begin();
	m_pUINum = (CIllustDDHp*)(*iter);

	if (FAILED(Make_ChildUI(-38.f, 143.f, 20.f, 20.f, TEXT("Prototype_GameObject_IllustDDLv"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter++;
	m_pLvNum = (CIllustDDLv*)(*iter);

	if (FAILED(Make_ChildUI(10.f, 207.f, 20.f, 20.f, TEXT("Prototype_GameObject_IllustDDSp"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 2;
	m_pSpNum = (CIllustDDSp*)(*iter);

	if (FAILED(Make_ChildUI(-42.f, 228.f, 18.f, 18.f, TEXT("Prototype_GameObject_IllustDDAttack"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 3;
	m_pAttackNum = (CIllustDDAttack*)(*iter);

	if (FAILED(Make_ChildUI(-42.f, 252.f, 18.f, 18.f, TEXT("Prototype_GameObject_IllustDDArmor"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 4;
	m_pArmorNum = (CIllustDDArmor*)(*iter);

	return S_OK;
}

void CIllustDD::Tick(_float fTimeDelta)
{
	m_pUINum->Set_Number((_uint)(STM->Get_FinalHp(CStatus_Manager::PLAYER04)));
	m_pLvNum->Set_Number(UM->Get_Player3Level());
	m_pSpNum->Set_Number(100);
	m_pAttackNum->Set_Number(UM->Get_Player3Str());
	m_pArmorNum->Set_Number(UM->Get_Player3Stamina());
	//UI_InputDebug(fTimeDelta);

	if (m_fSlide < 50.f)
	{
		m_UiInfo.fX -= 5.f;
		m_fSlide += 5.f;
	}

	if (CK_DOWN(DIK_ESCAPE))
	{
		m_UiInfo.fX = -100.f + 512.f + 50.f;
		m_fSlide = 0.f;
	}


	if (UM->DD_State() == true)
		__super::Tick(fTimeDelta);
}

void CIllustDD::LateTick(_float fTimeDelta)
{
	if (UM->DD_State() == true)
		__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CIllustDD::Render()
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
	if (UM->DD_State() == true)
		__super::Render();

	return S_OK;
}






HRESULT CIllustDD::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_IllustDD"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}







void CIllustDD::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CIllustDD * CIllustDD::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CIllustDD*		pInstance = new CIllustDD(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CIllustDD"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CIllustDD::Clone(void * pArg)
{
	CIllustDD*		pInstance = new CIllustDD(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CIllustDD"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CIllustDD::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
