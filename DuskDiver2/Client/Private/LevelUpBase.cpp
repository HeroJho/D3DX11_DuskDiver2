#include "stdafx.h"
#include "..\Public\LevelUpBase.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "ToolManager.h"



CLevelUpBase::CLevelUpBase(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CLevelUpBase::CLevelUpBase(const CLevelUpBase & rhs)
	: CUI(rhs)
{
}

HRESULT CLevelUpBase::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevelUpBase::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	if (FAILED(Make_ChildUI(0.f, 0.f, 1280.f, 750.f, TEXT("Prototype_GameObject_DuskDiverBackGround"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, -320.f, 1280.f, 150.f, TEXT("Prototype_GameObject_LevelUpBar"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(422.7f, 323.5f, 437.f, 47.f, TEXT("Prototype_GameObject_LevelUpGuide"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-280.f, 0.f, 256.f * 0.8f, 512.f * 1.1f, TEXT("Prototype_GameObject_LevelUpSelectBackGround1"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI( 30.f, 0.f, 256.f * 0.8f, 512.f * 1.1f, TEXT("Prototype_GameObject_LevelUpSelectBackGround2"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(340.f, 0.f, 256.f * 0.8f, 512.f * 1.1f, TEXT("Prototype_GameObject_LevelUpSelectBackGround3"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-280.f, 30.f, 290.f * 0.68f, 730.f * 0.68f, TEXT("Prototype_GameObject_LevelUpSelectCharacter1"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(30.f, 30.f, 290.f * 0.68f, 730.f * 0.68f, TEXT("Prototype_GameObject_LevelUpSelectCharacter2"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(340.f, 30.f, 290.f * 0.68f, 730.f * 0.68f, TEXT("Prototype_GameObject_LevelUpSelectCharacter3"))))
		return E_FAIL;

	
	if (FAILED(Make_ChildUI(-280.f, 91.6f, 247.f * 0.7f, 100.f * 0.7f, TEXT("Prototype_GameObject_LevelUpName1"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(30.f, 91.6f, 230.f * 0.7f, 124.f * 0.7f, TEXT("Prototype_GameObject_LevelUpName2"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(340.f, 91.6f, 123.f * 0.8f, 104.f * 0.8f, TEXT("Prototype_GameObject_LevelUpName3"))))
		return E_FAIL;

	return S_OK;
}

void CLevelUpBase::Tick(_float fTimeDelta)
{
	if (CK_DOWN(DIK_ESCAPE) && !m_bRender)
	{
		m_bRender = true;
	}
	else if (CK_DOWN(DIK_ESCAPE) && m_bRender)
	{
		m_bRender = false;
		GI->PlaySoundW(L"UI_Cancel.wav", SD_UI, 0.6f);
	}

	if (!m_bRender)
	{
		m_bSelect = false;
		m_UiInfo.fX = 1920.f;
		m_bLevelUp = false;
	}

	if (m_bRender)
	{
		if (CK_DOWN(DIK_RETURN) && UM->Get_BackNum() == 1)
		{
			GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
			m_bSelect = true;
			m_eDir = DIR_RIGHT;
		}

		if (CK_DOWN(DIK_RETURN) && UM->Get_BackNum() == 2)
		{
			GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
			m_bLevelUp = true;
		}


		if (CK_DOWN(DIK_BACK) && m_bSelect)
		{
			GI->PlaySoundW(L"UI_Cancel.wav", SD_UI, 0.6f);
			m_bSelect = false;
		}

		if (CK_DOWN(DIK_BACK) && m_bLevelUp)
		{
			GI->PlaySoundW(L"UI_Cancel.wav", SD_UI, 0.6f);
			m_bLevelUp = false;
			m_bSelect = true;
			UM->Set_BackNum(1);
			m_eDir = DIR_LEFT;
		}



		if (!m_bSelect && !m_bLevelUp)
		{
			if (m_UiInfo.fX >= 1920.f)
				m_UiInfo.fX = 1920.f;
			else
				m_UiInfo.fX += 128.f;
		}
		else if (m_bSelect && !m_bLevelUp && m_eDir == DIR_RIGHT)
		{
			if (m_UiInfo.fX <= 640.f)
				m_UiInfo.fX = 640.f;
			else
				m_UiInfo.fX -= 128.f;
		}
		else if (m_bSelect && !m_bLevelUp && m_eDir == DIR_LEFT)
		{
			if (m_UiInfo.fX >= 640.f)
				m_UiInfo.fX = 640.f;
			else
				m_UiInfo.fX += 128.f;
		}
		// levelup 창에서 캐릭터를 선택했을 시
		else if (m_bLevelUp)
		{
			if (m_UiInfo.fX <= -640.f)
				m_UiInfo.fX = -640.f;
			else
				m_UiInfo.fX -= 128.f;
		}

		//UI_InputDebug(fTimeDelta);


		__super::Tick(fTimeDelta);


		if (UM->Get_BackNum() == 1)
		{
			if (CK_DOWN(DIK_RIGHT))
			{
				GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
				if (m_iSelectNum == 3)
					m_iSelectNum = 1;
				else
					m_iSelectNum++;

			}
			else if (CK_DOWN(DIK_LEFT))
			{
				GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
				if (m_iSelectNum == 1)
					m_iSelectNum = 3;
				else
					m_iSelectNum--;
			}
		}

		UM->Set_LevelUpSelectPageNum(m_iSelectNum);
		UM->Set_PlayerSelectNum(m_iSelectNum);
	}
}

void CLevelUpBase::LateTick(_float fTimeDelta)
{
	if (m_bRender)
		__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CLevelUpBase::Render()
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

	//UI_RenderDebug();


	// 자식들의 Render 호출
	if (m_bRender)
	{
		__super::Render();
	}

	return S_OK;
}






HRESULT CLevelUpBase::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuBackGround"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}






void CLevelUpBase::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CLevelUpBase * CLevelUpBase::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevelUpBase*		pInstance = new CLevelUpBase(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevelUpBase"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CLevelUpBase::Clone(void * pArg)
{
	CLevelUpBase*		pInstance = new CLevelUpBase(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CLevelUpBase"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CLevelUpBase::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
