#include "stdafx.h"
#include "..\Public\IllustYumo.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "IllustYumoSp.h"
#include "ToolManager.h"
#include "IllustYumoHp.h"
#include "IllustYumoLv.h"
#include "IllustYumoAmor.h"
#include "IllustYumoAttack.h"
#include "Status_Manager.h"



CIllustYumo::CIllustYumo(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CIllustYumo::CIllustYumo(const CIllustYumo & rhs)
	: CUI(rhs)
{
}

HRESULT CIllustYumo::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CIllustYumo::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	if (FAILED(Make_ChildUI(12.f, 205.f, 20.f, 20.f, TEXT("Prototype_GameObject_IllustYumoSp"))))
		return E_FAIL;

	vector<CUI*>::iterator iter = m_pChildUIs.begin();
	m_pSpNum = (CIllustYumoSp*)(*iter);

	if (FAILED(Make_ChildUI(-38.f, 169.f, 20.f, 20.f, TEXT("Prototype_GameObject_IllustYumoHp"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter++;
	m_pUINum = (CIllustYumoHp*)(*iter);

	if (FAILED(Make_ChildUI(-38.f, 143.f, 20.f, 20.f, TEXT("Prototype_GameObject_IllustYumoLv"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 2;
	m_pLvNum = (CIllustYumoLv*)(*iter);

	if (FAILED(Make_ChildUI(-40.f, 228.f, 18.f, 18.f, TEXT("Prototype_GameObject_IllustYumoAttack"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 3;
	m_pAttackNum = (CIllustYumoAttack*)(*iter);


	if (FAILED(Make_ChildUI(-40.f, 252.f, 18.f, 18.f, TEXT("Prototype_GameObject_IllustYumoAmor"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 4;
	m_pAmorNum = (CIllustYumoAmor*)(*iter);



	return S_OK;
}

void CIllustYumo::Tick(_float fTimeDelta)
{

	m_pUINum->Set_Number((_uint)(STM->Get_FinalHp(CStatus_Manager::PLAYER01)));
	m_pLvNum->Set_Number(UM->Get_Player1Level());
	m_pSpNum->Set_Number(100);
	m_pAttackNum->Set_Number(UM->Get_Player1Str());
	m_pAmorNum->Set_Number(UM->Get_Player1Stamina());



	if (m_fSlide < 10.f)
	{
		m_UiInfo.fX -= 2.f;
		m_fSlide += 2.f;
	}


	if (CK_DOWN(DIK_ESCAPE))
	{
		m_UiInfo.fX = -100.f + 10.f;
		m_fSlide = 0.f;
	}

	//UI_InputDebug(fTimeDelta);



	__super::Tick(fTimeDelta);
}

void CIllustYumo::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CIllustYumo::Render()
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






HRESULT CIllustYumo::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_IllustYumo"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}








void CIllustYumo::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CIllustYumo * CIllustYumo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CIllustYumo*		pInstance = new CIllustYumo(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CIllustYumo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CIllustYumo::Clone(void * pArg)
{
	CIllustYumo*		pInstance = new CIllustYumo(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CIllustYumo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CIllustYumo::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
