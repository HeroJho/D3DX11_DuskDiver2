#include "stdafx.h"
#include "..\Public\StoryQuest_Contents.h"
#include "GameInstance.h"
#include "UI_Manager.h"
#include "ToolManager.h"
#include "Monster_Manager.h"


CStoryQuest_Contents::CStoryQuest_Contents(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CStoryQuest_Contents::CStoryQuest_Contents(const CStoryQuest_Contents & rhs)
	: CUI(rhs)
{
}

HRESULT CStoryQuest_Contents::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStoryQuest_Contents::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	return S_OK;
}

void CStoryQuest_Contents::Tick(_float fTimeDelta)
{
	if (!UM->Get_QuestOn())
	{
		m_fOnAcc = 0.f;
		m_bOn = true;
	}

	else
	{
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
			m_fOnAcc -= 2.5f * fTimeDelta;

			if (0.f >= m_fOnAcc)
				m_fOnAcc = 0.f;
		}
	}

	if (UM->Get_QuestOn())
	{
		//스테이지2 넘기기
		if (CK_DOWN(DIK_RETURN) && 1 == UM->Get_QuestNum())
		{
			GI->StopSound(SD_MAP);
			UM->MoveBlind(true);
			UM->Set_FrontUI(false);
		}
		
		//지하철
		if (CK_DOWN(DIK_RETURN) && 3 == UM->Get_QuestNum())
		{
			MM->Control_Beta();
			MM->Control_Fusion();
		}

		//지하철역 밖
		if (CK_DOWN(DIK_RETURN) && 5 == UM->Get_QuestNum())
		{
			/*MM->Control_Beta();
			MM->Control_Fusion();
			MM->Control_OldBombRobot();*/
		}

		//균열 안
		if (CK_DOWN(DIK_RETURN) && 8 == UM->Get_QuestNum())
		{
			/*MM->Control_Beta();
			MM->Control_Fusion();
			MM->Control_OldBombRobot();
			MM->Control_DisasterBoss();*/
		}

	}
	//UI_InputDebug(fTimeDelta);


	__super::Tick(fTimeDelta);
}

void CStoryQuest_Contents::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CStoryQuest_Contents::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_QuestGaugeAcc", &m_fOnAcc, sizeof(_float));


	if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", UM->Get_QuestNum())))
		return E_FAIL;


	if (FAILED(m_pShaderCom->Begin(14)))
		return E_FAIL;

	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;

	//UI_RenderDebug();


	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






HRESULT CStoryQuest_Contents::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StoryQuest_Contents"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CStoryQuest_Contents::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CStoryQuest_Contents * CStoryQuest_Contents::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStoryQuest_Contents*		pInstance = new CStoryQuest_Contents(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStoryQuest_Contents"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStoryQuest_Contents::Clone(void * pArg)
{
	CStoryQuest_Contents*		pInstance = new CStoryQuest_Contents(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStoryQuest_Contents"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CStoryQuest_Contents::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
