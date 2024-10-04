#include "stdafx.h"
#include "..\Public\BattleEndBase.h"
#include "GameInstance.h"
#include "NumResultExp.h"
#include "NumResultCombo.h"
#include "NumResultBonus.h"
#include "NumResult.h"
#include "UI_Manager.h"


#include "ToolManager.h"
#include "Player_Manager.h"



CBattleEndBase::CBattleEndBase(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CBattleEndBase::CBattleEndBase(const CBattleEndBase & rhs)
	: CUI(rhs)
{
}

HRESULT CBattleEndBase::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBattleEndBase::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	if (FAILED(Make_ChildUI(112.f, -52.7f, 17.f, 17.f, TEXT("Prototype_GameObject_NumResultExp"))))
		return E_FAIL;

	vector<CUI*>::iterator iter = m_pChildUIs.begin();
	m_pUINum = (CNumResultExp*)(*iter);

	if (FAILED(Make_ChildUI(137.f, 14.7f, 17.f, 17.f, TEXT("Prototype_GameObject_NumResultCombo"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter++;
	m_pUICombo = (CNumResultCombo*)(*iter);

	if (FAILED(Make_ChildUI(137.f, 40.f, 17.f, 17.f, TEXT("Prototype_GameObject_NumResultBonus"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 2;
	m_pUIBonus = (CNumResultBonus*)(*iter);

	if (FAILED(Make_ChildUI(137.f, 86.f, 17.f, 17.f, TEXT("Prototype_GameObject_NumResult"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 3;
	m_pUIResult = (CNumResult*)(*iter);

	m_iNum0 = GI->Get_Random(0, 9);
	m_iNum1 = GI->Get_Random(0, 9);
	m_iNum2 = GI->Get_Random(0, 9);
	m_iNum3 = GI->Get_Random(1, 9);


	return S_OK;
}

void CBattleEndBase::Tick(_float fTimeDelta)
{
	m_fEndTime += fTimeDelta;
	m_fDeadTime += fTimeDelta;
	if (m_fEndTime > 2.f)
	{
		m_fTime += fTimeDelta;
		if (m_fTime > 0.01f && m_UiInfo.fSizeY < 240.f)
		{
			m_UiInfo.fSizeY += 13.f;
			m_UiInfo.fY += 6.5f;
			m_fTime = 0.f;
		}
		else if (m_UiInfo.fSizeY >= 240.f)
		{
			m_fRandom += fTimeDelta;
			m_fRandomTime += fTimeDelta;
			if (m_fRandomTime > 1.f && m_fRandomTime < 2.f)
			{

				m_pUICombo->Set_Number(UM->Get_MaxCombo());
			}
			else if (m_fRandomTime >= 2.f && m_fRandomTime < 3.f)
			{
				m_pUICombo->Set_Number(UM->Get_MaxCombo());
				m_iBonus = GI->Get_Random(100, 1000);
				m_pUIBonus->Set_Number(m_iBonus);
			}
			else if (m_fRandomTime >= 4.f)
			{
				m_pUICombo->Set_Number(UM->Get_MaxCombo());
				m_pUIBonus->Set_Number(m_iBonus);
				m_pUIResult->Set_Number(m_iBonus + UM->Get_MaxCombo());
			}

			if (m_fRandomTime >= 4.f)
			{
				m_pUINum->Set_Number(m_iNum0 + m_iNum1 * 10 + m_iNum2 * 100 + m_iNum3 * 1000);
			}
			else if (m_fRandom > 0.5f)
			{
				int i = GI->Get_Random(0, 9);
				int j = GI->Get_Random(0, 9);
				int k = GI->Get_Random(0, 9);
				int l = GI->Get_Random(0, 9);
				if (m_fRandomTime > 1.f && m_fRandomTime < 2.f)
				{
					m_pUINum->Set_Number(m_iNum0 + j * 10 + k * 100 + l * 1000);
				}
				else if (m_fRandomTime >= 2.f && m_fRandomTime < 3.f)
				{
					m_pUINum->Set_Number(m_iNum0 + m_iNum1 * 10 + k * 100 + l * 1000);
				}
				else if (m_fRandomTime >= 3.f && m_fRandomTime < 4.f)
				{
					m_pUINum->Set_Number(m_iNum0 + m_iNum1 * 10 + m_iNum2 * 100 + l * 1000);
				}
			}
		}
	}
	if (m_fDeadTime > 9.f) 
	{
		m_fAlpha += 0.01f;
		m_UiInfo.fX -= 1.f;
		if ((m_fAlpha > 0.5f) || (CK_DOWN(DIK_RETURN)))
		{
			UM->Reset_MaxCombo();

			UM->Set_FrontUI(true);
			Check_Result();
				
			Set_Dead();
		}
	}

	__super::Tick(fTimeDelta);

	//UI_InputDebug(fTimeDelta);
}

void CBattleEndBase::LateTick(_float fTimeDelta)
{

	__super::LateTick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CBattleEndBase::Render()
{
	if (nullptr == m_pVIBufferCom ||
		nullptr == m_pShaderCom)
		return E_FAIL;


	m_pShaderCom->Set_RawValue("g_WorldMatrix", &m_pTransformCom->Get_WorldFloat4x4_TP(), sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ViewMatrix", &m_ViewMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_ProjMatrix", &m_ProjMatrix, sizeof(_float4x4));
	m_pShaderCom->Set_RawValue("g_TimeDelta", &m_fAlpha, sizeof(_float));




	if (m_fRandomTime >= 3.f)
	{
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 1)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTextureCom->Set_SRV(m_pShaderCom, "g_DiffuseTexture", 0)))
			return E_FAIL;
	}


	if (FAILED(m_pShaderCom->Begin(7)))
		return E_FAIL;


	if (FAILED(m_pVIBufferCom->Render()))
		return E_FAIL;




	//UI_RenderDebug();

	// 자식들의 Render 호출
	__super::Render();

	return S_OK;
}






void CBattleEndBase::Check_Result()
{
	switch (UM->Get_BattleIndex())
	{
	case 0:
		TM->Set_Stage2_2Bool(false);
		UM->Set_FrontUI(true);
		UM->Set_QuestPoint({ -10.f, 0.f, -16.f }, 0);
		//GI->PlaySound_Linear(L"SubWay.wav", SD_BGM, 0.3f, true, 0.4f);
		GI->PlaySoundW(L"ActHuman_41_kirakira.wav", SD_MAP, 0.6f);
		PM->Player01ChangeOff();
		break;
	case 1:
		TM->Set_Stage3_Bool(false);
		UM->Set_TalkIndex(30);
		//TM->Set_Stage3_Bool(false);
		//GI->PlaySound_Linear(L"BGM_Day.wav", SD_BGM, 0.3f, true, 0.4f);
		GI->PlaySoundW(L"ActHuman_41_kirakira.wav", SD_MAP, 0.6f);
		PM->Player01ChangeOff();
		break;
	case 2:
		TM->Set_Stage3_Bool(false);
		//GI->PlaySound_Linear(L"BGM_Day.wav", SD_BGM, 0.3f, true, 0.4f);
		GI->PlaySoundW(L"ActHuman_41_kirakira.wav", SD_MAP, 0.6f);
		PM->Player01ChangeOff();
		break;
	case 3:
		TM->Set_Stage3_Bool(false);
		//GI->PlaySound_Linear(L"BGM_Day.wav", SD_BGM, 0.3f, true, 0.4f);
		GI->PlaySoundW(L"ActHuman_41_kirakira.wav", SD_MAP, 0.6f);
		UM->Set_TalkIndex(51);
		UM->Set_FrontUI(false);
		PM->Player01ChangeOff();
		break;
	case 4:
		UM->Set_FrontUI(false); // 첫 에너지 장벽 부순뒤 결과

		TM->Open_Stage4Trigger(2);
		break;
	case 5:
		UM->Set_FrontUI(false); // 볼링장 몬스터 죽인뒤 결과
		TM->Open_Stage4Trigger(3);
		break;
	case 6:
		UM->Set_FrontUI(false); // 야바위전 몬스터 죽인뒤 결과
		TM->Open_Stage4Trigger(5);
		break;
	case 7:
		UM->Set_FrontUI(false); // 색종이전 몬스터 죽인뒤 결과
		TM->Open_Stage4Trigger(7);
		break;
	default:
		break;
	}
}

HRESULT CBattleEndBase::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BattleEndBase"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CBattleEndBase::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CBattleEndBase * CBattleEndBase::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBattleEndBase*		pInstance = new CBattleEndBase(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CBattleEndBase"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBattleEndBase::Clone(void * pArg)
{
	CBattleEndBase*		pInstance = new CBattleEndBase(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CBattleEndBase"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CBattleEndBase::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
