#include "stdafx.h"
#include "..\Public\IllustLeviada.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "IllustLeviadaHp.h"
#include "UI_Manager.h"
#include "IllustLeviadaLv.h"
#include "IllustLeviadaSp.h"
#include "IllustLeviadaAttack.h"
#include "IllustLeviadaAmor.h"
#include "Status_Manager.h"


CIllustLeviada::CIllustLeviada(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CIllustLeviada::CIllustLeviada(const CIllustLeviada & rhs)
	: CUI(rhs)
{
}

HRESULT CIllustLeviada::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CIllustLeviada::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	if (FAILED(Make_ChildUI(-38.f, 169.f, 20.f, 20.f, TEXT("Prototype_GameObject_IllustLeviadaHp"))))
		return E_FAIL;

	vector<CUI*>::iterator iter = m_pChildUIs.begin();
	m_pUINum = (CIllustLeviadaHp*)(*iter);

	if (FAILED(Make_ChildUI(-38.f, 143.f, 20.f, 20.f, TEXT("Prototype_GameObject_IllustLeviadaLv"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter++;
	m_pLvNum = (CIllustLeviadaLv*)(*iter);

	if (FAILED(Make_ChildUI(13.f, 207.f, 20.f, 20.f, TEXT("Prototype_GameObject_IllustLeviadaSp"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 2;
	m_pSpNum = (CIllustLeviadaSp*)(*iter);

	if (FAILED(Make_ChildUI(-38.f, 228.f, 18.f, 18.f, TEXT("Prototype_GameObject_IllustLeviadaAttack"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 3;
	m_pAttackNum = (CIllustLeviadaAttack*)(*iter);


	if (FAILED(Make_ChildUI(-38.f, 252.f, 18.f, 18.f, TEXT("Prototype_GameObject_IllustLeviadaAmor"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 4;
	m_pAmorNum = (CIllustLeviadaAmor*)(*iter);


	return S_OK;
}

void CIllustLeviada::Tick(_float fTimeDelta)
{

	m_pUINum->Set_Number((_uint)(STM->Get_FinalHp(CStatus_Manager::PLAYER03)));
	m_pLvNum->Set_Number(UM->Get_Player2Level());
	m_pSpNum->Set_Number(100);
	m_pAttackNum->Set_Number(UM->Get_Player2Str());
	m_pAmorNum->Set_Number(UM->Get_Player2Stamina());

	if (m_fSlide < 30.f)
	{
		m_UiInfo.fX -= 5.f;
		m_fSlide += 5.f;
	}

	if (CK_DOWN(DIK_ESCAPE))
	{
		m_UiInfo.fX = -100.f + 256.f + 30.f;
		m_fSlide = 0.f;
	}

	//UI_InputDebug(fTimeDelta);



	__super::Tick(fTimeDelta);
}

void CIllustLeviada::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CIllustLeviada::Render()
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






HRESULT CIllustLeviada::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_IllustLeviada"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}








void CIllustLeviada::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CIllustLeviada * CIllustLeviada::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CIllustLeviada*		pInstance = new CIllustLeviada(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CIllustLeviada"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CIllustLeviada::Clone(void * pArg)
{
	CIllustLeviada*		pInstance = new CIllustLeviada(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CIllustLeviada"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CIllustLeviada::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
