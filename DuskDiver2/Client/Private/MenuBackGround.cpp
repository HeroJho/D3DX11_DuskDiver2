#include "stdafx.h"
#include "..\Public\MenuBackGround.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"
#include "Status_Manager.h"



CMenuBackGround::CMenuBackGround(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CMenuBackGround::CMenuBackGround(const CMenuBackGround & rhs)
	: CUI(rhs)
{
}

HRESULT CMenuBackGround::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMenuBackGround::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	if (FAILED(Make_ChildUI(0.f, 75.f, 1280.f, 750.f, TEXT("Prototype_GameObject_DuskDiverBackGround"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, -285.f, 1280.f, 150.f, TEXT("Prototype_GameObject_MenuBar"))))
		return E_FAIL;



	if (FAILED(Make_ChildUI(-100.f + 10.f, 30.f , 256.f * 0.9f, 600.f * 0.9f, TEXT("Prototype_GameObject_IllustYumo"))))
		return E_FAIL;
	if (FAILED(Make_ChildUI(-100.f + 256.f + 30.f, 30.f, 256.f * 0.9f, 600.f * 0.9f, TEXT("Prototype_GameObject_IllustLeviada"))))
		return E_FAIL;
	if (FAILED(Make_ChildUI(-100.f + 512.f + 50.f, 30.f, 256.f * 0.9f, 600.f * 0.9f, TEXT("Prototype_GameObject_IllustDD"))))
		return E_FAIL;
	if (FAILED(Make_ChildUI(493.f, 330.f, 366.f * 0.8f, 34.f * 0.8f, TEXT("Prototype_GameObject_MenuGuide"))))
		return E_FAIL;
	if (FAILED(Make_ChildUI(-149.1f + 10.f, -233.6f, 300.f * 0.6f, 100.f * 0.6f, TEXT("Prototype_GameObject_MenuNum1"))))
		return E_FAIL;
	if (FAILED(Make_ChildUI(-149.1f + 256.f + 30.f, -233.6f, 300.f * 0.6f, 100.f * 0.6f, TEXT("Prototype_GameObject_MenuNum2"))))
		return E_FAIL;
	if (FAILED(Make_ChildUI(-149.1f + 512.f + 50.f, -233.6f, 300.f * 0.6f, 100.f * 0.6f, TEXT("Prototype_GameObject_MenuNum3"))))
		return E_FAIL;
	if (FAILED(Make_ChildUI(10.f, -237.5f, 64.f * 0.5f, 64.f * 0.5f, TEXT("Prototype_GameObject_BattleTag"))))
		return E_FAIL;
	if (FAILED(Make_ChildUI(-426.7f, -157.4f, 399.f * 0.3f, 250.f * 0.7f, TEXT("Prototype_GameObject_LevelUpBackGround"))))
		return E_FAIL;
	if (FAILED(Make_ChildUI(-426.7f, -157.4f, 304.f * 0.7f, 99.f * 0.7f, TEXT("Prototype_GameObject_MenuLevelUp"))))
		return E_FAIL;
	if (FAILED(Make_ChildUI(-478.3f, -29.8f, 475.f * 0.4f, 250.f * 0.8f, TEXT("Prototype_GameObject_LearnSkillsBackGround"))))
		return E_FAIL;
	if (FAILED(Make_ChildUI(-478.3f, -29.8f, 168.f * 0.8f, 87.f * 0.8f, TEXT("Prototype_GameObject_MenuLearnSkills"))))
		return E_FAIL;
	if (FAILED(Make_ChildUI(-405.2f, 105.8f, 500.f * 0.3f, 250.f * 0.7f, TEXT("Prototype_GameObject_EquipMentBackGround"))))
		return E_FAIL;
	if (FAILED(Make_ChildUI(-405.2f, 105.8f, 381.f * 0.7f, 106.f * 0.7f, TEXT("Prototype_GameObject_MenuEquipMent"))))
		return E_FAIL;
	if (FAILED(Make_ChildUI(-461.3f, 241.6f, 450.f * 0.4f, 250.f * 0.8f, TEXT("Prototype_GameObject_ItemsBackGround"))))
		return E_FAIL;
	if (FAILED(Make_ChildUI(-461.3f, 241.6f, 248.f * 0.7f, 86.f * 0.7f, TEXT("Prototype_GameObject_MenuItems"))))
		return E_FAIL;
	if (FAILED(Make_ChildUI(580.83f, -330.182f, 66.f, 28.f, TEXT("Prototype_GameObject_Hunger"))))
		return E_FAIL;


	return S_OK;
}

void CMenuBackGround::Tick(_float fTimeDelta)
{

	if (m_bDebug)
		UI_InputDebug(fTimeDelta);

	if (!m_bRender)
	{
		m_bSelect = false;
		UM->Set_BackNum(0);
		m_UiInfo.fX = 640.f;
	}

	if (m_bRender)
	{
		__super::Tick(fTimeDelta);

		if (UM->Get_BackNum() == 0)
		{

			if (CK_DOWN(DIK_DOWN))
			{
				GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
				if (MenuNum == 4)
					MenuNum = 1;
				else
					MenuNum++;

			}
			else if (CK_DOWN(DIK_UP))
			{
				GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
				if (MenuNum == 1)
					MenuNum = 4;
				else
					MenuNum--;
			}
		}

		UM->Set_MenuNum(MenuNum);


		if (CK_DOWN(DIK_RETURN) && MenuNum == 1 && UM->Get_BackNum() == 0)
		{
			GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
			m_bSelect = true;
			UM->Set_BackNum(1);
		}

		else if (CK_DOWN(DIK_RETURN) && MenuNum == 4 && UM->Get_BackNum() == 0)
		{
			GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
			m_bSelect = true;
			UM->Set_BackNum(5);
		}

		else if (CK_DOWN(DIK_RETURN) && MenuNum == 3 && UM->Get_BackNum() == 0)
		{
			GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
			m_bSelect = true;
			UM->Set_BackNum(3);
		}

		else if (CK_DOWN(DIK_RETURN) && UM->Get_BackNum() == 1)
		{
			GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
			m_bSelect = true;
			UM->Set_BackNum(2);
		}

		else if (CK_DOWN(DIK_RETURN) && UM->Get_BackNum() == 3)
		{
			GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
			m_bSelect = true;
			UM->Set_BackNum(4);
		}

		if (CK_DOWN(DIK_BACK) && UM->Get_BackNum() == 1)
		{
			GI->PlaySoundW(L"UI_Cancel.wav", SD_UI, 0.6f);
			m_bSelect = false;
			UM->Set_BackNum(0);
		}

		if (CK_DOWN(DIK_BACK) && UM->Get_BackNum() == 3)
		{
			GI->PlaySoundW(L"UI_Cancel.wav", SD_UI, 0.6f);
			m_bSelect = false;
			UM->Set_BackNum(0);
		}

		if (CK_DOWN(DIK_BACK) && UM->Get_BackNum() == 5 && UM->Get_InvenType() == 0)
		{
			GI->PlaySoundW(L"UI_Cancel.wav", SD_UI, 0.6f);
			m_bSelect = false;
			UM->Set_BackNum(0);
		}

		if (m_bSelect)
		{
			if (m_UiInfo.fX <= -640.f)
				m_UiInfo.fX = -640.f;
			else
				m_UiInfo.fX -= 128.f;
		}
		else
		{
			if (m_UiInfo.fX >= 640.f)
				m_UiInfo.fX = 640.f;
			else
				m_UiInfo.fX += 128.f;
		}
	}
}

void CMenuBackGround::LateTick(_float fTimeDelta)
{
	if (CK_DOWN(DIK_ESCAPE) && !m_bRender)
	{
		m_bRender = true;
	}
	else if (CK_DOWN(DIK_ESCAPE) && m_bRender)
	{
		GI->PlaySoundW(L"UI_Cancel.wav", SD_UI, 0.6f);
		m_bRender = false;
		MenuNum = 1;
		UM->Set_MenuNum(MenuNum);


	}

	if (m_bRender)
		__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CMenuBackGround::Render()
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

	if (m_bRender)
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}



	//if (m_bDebug)
	//	UI_RenderDebug();


	// 자식들의 Render 호출
	if(m_bRender)
		__super::Render();


	return S_OK;
}






HRESULT CMenuBackGround::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuBackGround"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}





void CMenuBackGround::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CMenuBackGround * CMenuBackGround::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMenuBackGround*		pInstance = new CMenuBackGround(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CMenuBackGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMenuBackGround::Clone(void * pArg)
{
	CMenuBackGround*		pInstance = new CMenuBackGround(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CMenuBackGround"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CMenuBackGround::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
