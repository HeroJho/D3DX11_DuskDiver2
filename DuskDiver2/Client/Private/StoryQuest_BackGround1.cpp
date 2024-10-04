#include "stdafx.h"
#include "..\Public\StoryQuest_BackGround1.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "Monster_Manager.h"
#include "ToolManager.h"



CStoryQuest_BackGround1::CStoryQuest_BackGround1(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CStoryQuest_BackGround1::CStoryQuest_BackGround1(const CStoryQuest_BackGround1 & rhs)
	: CUI(rhs)
{
}

HRESULT CStoryQuest_BackGround1::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStoryQuest_BackGround1::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	if (FAILED(Make_ChildUI(0.f, -41.f, 1280.f, 720.f, TEXT("Prototype_GameObject_StoryQuest_BackGround3"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, -41.f, 1280.f, 720.f, TEXT("Prototype_GameObject_StoryQuest_BackGround2"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, -41.f, 1280.f, 720.f, TEXT("Prototype_GameObject_StoryQuest_Contents"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, -41.f, 1280.f, 720.f, TEXT("Prototype_GameObject_StoryQuest_Point"))))
		return E_FAIL;
	 
	return S_OK;
}

void CStoryQuest_BackGround1::Tick(_float fTimeDelta)
{
	//UI_InputDebug(fTimeDelta);

	if (!UM->Get_QuestOn())
	{
		m_fOnAcc = 0.f;
		m_bOn = true;
	}

	else
	{
		if (!m_bSoundOn)
		{
			GI->PlaySoundW(L"UI_TitleCard.wav", SD_UI, 0.6f);
			m_bSoundOn = true;
		}

		if (m_bOn)
		{
			m_fOnAcc += 2.5f * fTimeDelta;

			if (1.f <= m_fOnAcc)
			{
				m_bOn = false;
				m_fOnAcc = 1.f;
			}
		}

		if (UM->Get_QuestOff())
		{
			m_fOnAcc -= 3.f * fTimeDelta;

			if (0.f >= m_fOnAcc)
			{
				UM->Set_QuestOff(false);
				m_fOnAcc = 0.f;
				m_bSoundOn = false;

				UM->Set_QuestOn(false);
				UM->Set_NextQuest(true);
				m_bRender = false;
			}
		}
	}



	//if (m_bRender)
		__super::Tick(fTimeDelta);
}

void CStoryQuest_BackGround1::LateTick(_float fTimeDelta)
{
	//ESC를 눌러서 Menu가 켜졌을 때 Render 끄기
	if (UM->Get_QuestOn() && !m_bRender)
		m_bRender = true;

	if (CK_DOWN(DIK_RETURN) && m_bRender)
	{
		UM->Set_QuestOff(true);
		GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
	}

	//if (m_bRender)
		__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CStoryQuest_BackGround1::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_QuestGaugeAcc", &m_fOnAcc, sizeof(_float));


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(14)))
		return E_FAIL;

	if (m_bRender)
	{
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;
	}


	//UI_RenderDebug();

	if (m_bRender)
	{
		// 자식들의 Render 호출
		__super::Render();
	}

	return S_OK;
}






HRESULT CStoryQuest_BackGround1::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StoryQuest_BackGround1"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CStoryQuest_BackGround1::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CStoryQuest_BackGround1 * CStoryQuest_BackGround1::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStoryQuest_BackGround1*		pInstance = new CStoryQuest_BackGround1(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStoryQuest_BackGround1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStoryQuest_BackGround1::Clone(void * pArg)
{
	CStoryQuest_BackGround1*		pInstance = new CStoryQuest_BackGround1(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStoryQuest_BackGround1"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CStoryQuest_BackGround1::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
