#include "stdafx.h"
#include "..\Public\EquipmentBase.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Player_Manager.h"
#include "UI_Manager.h"
#include "Status_Manager.h"



CEquipmentBase::CEquipmentBase(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CEquipmentBase::CEquipmentBase(const CEquipmentBase & rhs)
	: CUI(rhs)
{
}

HRESULT CEquipmentBase::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEquipmentBase::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	if (FAILED(Make_ChildUI(0.f, 0.f, 1280.f, 750.f, TEXT("Prototype_GameObject_DuskDiverBackGround"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, -344.1754f, 1280.f, 105.6f, TEXT("Prototype_GameObject_EquipmentUpside"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-415.f, -213.4f, 700.f * 0.6f, 195.f * 0.6f, TEXT("Prototype_GameObject_EquipmentCase"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-415.f, -63.4f, 700.f * 0.6f, 195.f * 0.6f, TEXT("Prototype_GameObject_EquipmentCase"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-415.f, 95.4f, 700.f * 0.6f, 195.f * 0.6f, TEXT("Prototype_GameObject_EquipmentCase"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-415.f, 245.4f, 700.f * 0.6f, 195.f * 0.6f, TEXT("Prototype_GameObject_EquipmentCase"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(15.f, 18.3f, 530.f * 0.7f, 820.f * 0.7f, TEXT("Prototype_GameObject_EquipmentBox"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(425.f, 18.3f, 530.f * 0.7f, 820.f * 0.7f, TEXT("Prototype_GameObject_EquipmentStatus"))))
		return E_FAIL;


	if (FAILED(Make_ChildUI(-415.f, -213.4f, 700.f * 0.6f, 195.f * 0.6f, TEXT("Prototype_GameObject_EquipmentSelectColor"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-415.f, -213.4f, 700.f * 0.6f, 195.f * 0.6f, TEXT("Prototype_GameObject_EquipmentSelectCase"))))
		return E_FAIL;


	if (FAILED(Make_ChildUI(14.8f, -203.3f, 371.f, 55.f, TEXT("Prototype_GameObject_EquipmentSelectBar"))))
		return E_FAIL;


	if (FAILED(Make_ChildUI(-398.3f, -233.5f, 563.f * 0.8f, 83.f * 0.8f, TEXT("Prototype_GameObject_EquipmentWeaponBackground"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-388.3f, -18.4f, 563.f * 0.8f, 250.f * 0.8f, TEXT("Prototype_GameObject_EquipmentHatBackground"))))
		return E_FAIL; //60

	if (FAILED(Make_ChildUI(-388.3f, 140.4f, 563.f * 0.8f, 250.f * 0.8f, TEXT("Prototype_GameObject_EquipmentClothesBackground"))))
		return E_FAIL;


	if (FAILED(Make_ChildUI(-388.3f, 295.4f, 563.f * 0.8f, 250.f * 0.8f, TEXT("Prototype_GameObject_EquipmentDecoBackground"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(472.8f, 333.66f, 530.f * 0.7f, 49.f * 0.9f, TEXT("Prototype_GameObject_EquipmentGuide"))))
		return E_FAIL;






	if (FAILED(Make_ChildUI(19.17f, -202.7f, 371.f, 55.f, TEXT("Prototype_GameObject_HatSlot1"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(19.17f, -149.7f, 371.f, 55.f, TEXT("Prototype_GameObject_HatSlot2"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(19.17f, -96.7f, 371.f, 55.f, TEXT("Prototype_GameObject_HatSlot3"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(19.17f, -43.7f, 371.f, 55.f, TEXT("Prototype_GameObject_HatSlot4"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(19.17f, 9.3f, 371.f, 55.f, TEXT("Prototype_GameObject_HatSlot5"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(19.17f, 62.3f, 371.f, 55.f, TEXT("Prototype_GameObject_HatSlot6"))))
		return E_FAIL;


	if (FAILED(Make_ChildUI(19.17f, 115.3f, 371.f, 55.f, TEXT("Prototype_GameObject_HatSlot7"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(19.17f, -202.7f, 371.f, 55.f, TEXT("Prototype_GameObject_ClothesSlot1"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(19.17f, -149.7f, 371.f, 55.f, TEXT("Prototype_GameObject_ClothesSlot2"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(19.17f, -96.7f, 371.f, 55.f, TEXT("Prototype_GameObject_ClothesSlot3"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(19.17f, -43.7f, 371.f, 55.f, TEXT("Prototype_GameObject_ClothesSlot4"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(19.17f, 9.3f, 371.f, 55.f, TEXT("Prototype_GameObject_ClothesSlot5"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(19.17f, 62.3f, 371.f, 55.f, TEXT("Prototype_GameObject_ClothesSlot6"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(19.17f, 115.3f, 371.f, 55.f, TEXT("Prototype_GameObject_ClothesSlot7"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(19.17f, -202.7f, 371.f, 55.f, TEXT("Prototype_GameObject_DecoSlot1"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(19.17f, -149.7f, 371.f, 55.f, TEXT("Prototype_GameObject_DecoSlot2"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(19.17f, -96.7f, 371.f, 55.f, TEXT("Prototype_GameObject_DecoSlot3"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(19.17f, -43.7f, 371.f, 55.f, TEXT("Prototype_GameObject_DecoSlot4"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(19.17f, 9.3f, 371.f, 55.f, TEXT("Prototype_GameObject_DecoSlot5"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(19.17f, 62.3f, 371.f, 55.f, TEXT("Prototype_GameObject_DecoSlot6"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(19.17f, 115.3f, 371.f, 55.f, TEXT("Prototype_GameObject_DecoSlot7"))))
		return E_FAIL;


	if (FAILED(Make_ChildUI(387.f, -324.f, 128.f * 0.4f, 128.f * 0.4f, TEXT("Prototype_GameObject_PlayerCircle"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(387.f, -324.f, 128.f * 0.4f, 128.f * 0.4f, TEXT("Prototype_GameObject_Player1Icon"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(455.f, -324.f, 128.f * 0.4f, 128.f * 0.4f, TEXT("Prototype_GameObject_Player2Icon"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(527.f, -324.f, 128.f * 0.4f, 128.f * 0.4f, TEXT("Prototype_GameObject_Player3Icon"))))
		return E_FAIL;


	if (FAILED(Make_ChildUI(-471.1f, -317.3f, 567.f * 0.5f, 184.f * 0.5f, TEXT("Prototype_GameObject_StatusLT1"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-471.1f, -317.3f, 567.f * 0.5f, 184.f * 0.5f, TEXT("Prototype_GameObject_StatusLT2"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-471.1f, -317.3f, 567.f * 0.5f, 184.f * 0.5f, TEXT("Prototype_GameObject_StatusLT3"))))
		return E_FAIL;

	//if (FAILED(Make_ChildUI(19.17f, 62.3f, 55.f, 55.f, TEXT("Prototype_GameObject_EquipmentE"))))
	//	return E_FAIL;


	if (FAILED(Make_ChildUI(402.7f, -205.6f, 394.f * 0.7f, 48.f * 0.7f, TEXT("Prototype_GameObject_Equipment1Info"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(402.7f, -205.6f, 420.f * 0.7f, 48.f * 0.7f, TEXT("Prototype_GameObject_Equipment2Info"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(402.7f, -205.6f, 420.f * 0.7f, 48.f * 0.7f, TEXT("Prototype_GameObject_Equipment3Info"))))
		return E_FAIL;


	if (FAILED(Make_ChildUI(185.f, 0.f, 19.f, 29.f, TEXT("Prototype_GameObject_Player1EquipmentEHat"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(185.f, 0.f, 19.f, 29.f, TEXT("Prototype_GameObject_Player2EquipmentE"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(185.f, 0.f, 19., 29.f, TEXT("Prototype_GameObject_Player3EquipmentE"))))
		return E_FAIL;



	return S_OK;
}

void CEquipmentBase::Tick(_float fTimeDelta)
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
		m_iEquipmentState = 0;
	}

	

	if (!m_bEquip)
	{
		for (_uint i = 0; i < 5; ++i)
		{
			UM->Gain_Clothes(i);
			UM->Gain_Deco(i);
			UM->Gain_Hat(i);
		}
		m_bEquip = true;
	}

	else if (CK_DOWN(DIK_RETURN) && UM->Get_BackNum() == 4 && m_iEquipmentState == 1)
	{
		if (m_iEquipmentClass == 2)
		{
			if (m_iPlayerNum == 1)
			{
				UM->Set_Player1Hat(UM->Get_Hat(m_iEquipmentNum - 1) + 1);
				
				_float2 Hat = { 0,0 };

				if (UM->Get_Player1Hat() == 1)
				{
					Hat = { 1, 1 };
				}
				else if (UM->Get_Player1Hat() == 2)
				{
					Hat = { 2, 2 };
				}
				else if (UM->Get_Player1Hat() == 3)
				{
					Hat = { 4, 3 };
				}
				else if (UM->Get_Player1Hat() == 4)
				{
					Hat = { 7, 8 };
				}
				else if (UM->Get_Player1Hat() == 5)
				{
					Hat = { 17, 18 };
				}
				STM->Set_Hat(Hat, CStatus_Manager::PLAYER01);
				STM->Set_FinalStr(CStatus_Manager::PLAYER01);
				STM->Set_FinalCritical(CStatus_Manager::PLAYER01);
				UM->Set_Player1SelectEHat(m_iEquipmentNum);
			}
			else if (m_iPlayerNum == 2)
			{
				UM->Set_Player2Hat(UM->Get_Hat(m_iEquipmentNum - 1) + 1);
				_float2 Hat = { 0,0 };

				if (UM->Get_Player2Hat() == 1)
				{
					Hat = { 1, 1 };
				}
				else if (UM->Get_Player2Hat() == 2)
				{
					Hat = { 2, 2 };
				}
				else if (UM->Get_Player2Hat() == 3)
				{
					Hat = { 4, 3 };
				}
				else if (UM->Get_Player2Hat() == 4)
				{
					Hat = { 7, 8 };
				}
				else if (UM->Get_Player2Hat() == 5)
				{
					Hat = { 17, 18 };
				}
				STM->Set_Hat(Hat, CStatus_Manager::PLAYER03);
				STM->Set_FinalStr(CStatus_Manager::PLAYER03);
				STM->Set_FinalCritical(CStatus_Manager::PLAYER03);
				UM->Set_Player2SelectEHat(m_iEquipmentNum);
			}
			else if (m_iPlayerNum == 3)
			{
				UM->Set_Player3Hat(UM->Get_Hat(m_iEquipmentNum - 1) + 1);
				_float2 Hat = { 0,0 };

				if (UM->Get_Player2Hat() == 1)
				{
					Hat = { 1, 1 };
				}
				else if (UM->Get_Player2Hat() == 2)
				{
					Hat = { 2, 2 };
				}
				else if (UM->Get_Player2Hat() == 3)
				{
					Hat = { 4, 3 };
				}
				else if (UM->Get_Player2Hat() == 4)
				{
					Hat = { 7, 8 };
				}
				else if (UM->Get_Player2Hat() == 5)
				{
					Hat = { 17, 18 };
				}
				STM->Set_Hat(Hat, CStatus_Manager::PLAYER04);
				STM->Set_FinalStr(CStatus_Manager::PLAYER04);
				STM->Set_FinalCritical(CStatus_Manager::PLAYER04);
				UM->Set_Player3SelectEHat(m_iEquipmentNum);
			}
		}
		else if (m_iEquipmentClass == 3)
		{
			if (m_iPlayerNum == 1)
			{
				UM->Set_Player1Clothes(UM->Get_Clothes(m_iEquipmentNum - 1) + 1);

				_float2 Clothes = { 0,0 };

				if (UM->Get_Player1Clothes() == 1)
				{
					Clothes = { 1, 50 };
				}
				else if (UM->Get_Player1Clothes() == 2)
				{
					Clothes = { 1, 100 };
				}
				else if (UM->Get_Player1Clothes() == 3)
				{
					Clothes = { 4, 200 };
				}
				else if (UM->Get_Player1Clothes() == 4)
				{
					Clothes = { 8, 450 };
				}
				else if (UM->Get_Player1Clothes() == 5)
				{
					Clothes = { 16, 800 };
				}
				STM->Set_Clothes(Clothes, CStatus_Manager::PLAYER01);
				STM->Set_FinalStamina(CStatus_Manager::PLAYER01);
				STM->Set_FinalHp(CStatus_Manager::PLAYER01);
				UM->Set_Player1SelectEClothes(m_iEquipmentNum);
			}
			else if (m_iPlayerNum == 2)
			{
				UM->Set_Player2Clothes(UM->Get_Clothes(m_iEquipmentNum - 1) + 1);

				_float2 Clothes = { 0,0 };

				if (UM->Get_Player2Clothes() == 1)
				{
					Clothes = { 1, 50 };
				}
				else if (UM->Get_Player2Clothes() == 2)
				{
					Clothes = { 1, 100 };
				}
				else if (UM->Get_Player2Clothes() == 3)
				{
					Clothes = { 4, 200 };
				}
				else if (UM->Get_Player2Clothes() == 4)
				{
					Clothes = { 8, 450 };
				}
				else if (UM->Get_Player2Clothes() == 5)
				{
					Clothes = { 16, 800 };
				}
				STM->Set_Clothes(Clothes, CStatus_Manager::PLAYER03);
				STM->Set_FinalStamina(CStatus_Manager::PLAYER03);
				STM->Set_FinalHp(CStatus_Manager::PLAYER03);
				UM->Set_Player2SelectEClothes(m_iEquipmentNum);
			}
			else if (m_iPlayerNum == 3)
			{
				UM->Set_Player3Clothes(UM->Get_Clothes(m_iEquipmentNum - 1) + 1);

				_float2 Clothes = { 0,0 };

				if (UM->Get_Player3Clothes() == 1)
				{
					Clothes = { 1, 50 };
				}
				else if (UM->Get_Player3Clothes() == 2)
				{
					Clothes = { 1, 100 };
				}
				else if (UM->Get_Player3Clothes() == 3)
				{
					Clothes = { 4, 200 };
				}
				else if (UM->Get_Player3Clothes() == 4)
				{
					Clothes = { 8, 450 };
				}
				else if (UM->Get_Player3Clothes() == 5)
				{
					Clothes = { 16, 800 };
				}
				STM->Set_Clothes(Clothes, CStatus_Manager::PLAYER04);
				STM->Set_FinalStamina(CStatus_Manager::PLAYER04);
				STM->Set_FinalHp(CStatus_Manager::PLAYER04);
				UM->Set_Player3SelectEClothes(m_iEquipmentNum);
			}
		}
		else if (m_iEquipmentClass == 4)
		{
			if (m_iPlayerNum == 1)
			{
				UM->Set_Player1Deco(UM->Get_Deco(m_iEquipmentNum - 1) + 1);

				_float2 Deco = { 0,0 };

				if (UM->Get_Player1Deco() == 1)
				{
					Deco = { 1, 1 };
				}
				else if (UM->Get_Player1Deco() == 2)
				{
					Deco = { 2, 2 };
				}
				else if (UM->Get_Player1Deco() == 3)
				{
					Deco = { 4, 5 };
				}
				else if (UM->Get_Player1Deco() == 4)
				{
					Deco = { 8, 8 };
				}
				else if (UM->Get_Player1Deco() == 5)
				{
					Deco = { 17, 16 };
				}
				STM->Set_Deco(Deco, CStatus_Manager::PLAYER01);
				STM->Set_FinalMental(CStatus_Manager::PLAYER01);
				STM->Set_FinalMeta(CStatus_Manager::PLAYER01);
				UM->Set_Player1SelectEDeco(m_iEquipmentNum);
			}
			else if (m_iPlayerNum == 2)
			{
				UM->Set_Player2Deco(UM->Get_Deco(m_iEquipmentNum - 1) + 1);

				_float2 Deco = { 0,0 };

				if (UM->Get_Player2Deco() == 1)
				{
					Deco = { 1, 1 };
				}
				else if (UM->Get_Player2Deco() == 2)
				{
					Deco = { 2, 2 };
				}
				else if (UM->Get_Player2Deco() == 3)
				{
					Deco = { 4, 5 };
				}
				else if (UM->Get_Player2Deco() == 4)
				{
					Deco = { 8, 8 };
				}
				else if (UM->Get_Player2Deco() == 5)
				{
					Deco = { 17, 16 };
				}
				STM->Set_Deco(Deco, CStatus_Manager::PLAYER03);
				STM->Set_FinalMental(CStatus_Manager::PLAYER03);
				STM->Set_FinalMeta(CStatus_Manager::PLAYER03);
				UM->Set_Player2SelectEDeco(m_iEquipmentNum);
			}
			else if (m_iPlayerNum == 3)
			{
				UM->Set_Player3Deco(UM->Get_Clothes(m_iEquipmentNum - 1) + 1);

				_float2 Deco = { 0,0 };

				if (UM->Get_Player3Deco() == 1)
				{
					Deco = { 1, 1 };
				}
				else if (UM->Get_Player3Deco() == 2)
				{
					Deco = { 2, 2 };
				}
				else if (UM->Get_Player3Deco() == 3)
				{
					Deco = { 4, 5 };
				}
				else if (UM->Get_Player3Deco() == 4)
				{
					Deco = { 8, 8 };
				}
				else if (UM->Get_Player3Deco() == 5)
				{
					Deco = { 17, 16 };
				}
				STM->Set_Deco(Deco, CStatus_Manager::PLAYER04);
				STM->Set_FinalMental(CStatus_Manager::PLAYER04);
				STM->Set_FinalMeta(CStatus_Manager::PLAYER04);
				UM->Set_Player3SelectEDeco(m_iEquipmentNum);
			}
		}
	}

	if (CK_DOWN(DIK_RETURN) && UM->Get_BackNum() == 4 && !m_bSelect && m_iEquipmentState == 0)
	{
		GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
		m_bSelect = true;
		m_iPlayerNum = UM->Get_EquipPlayerSelectNum();
	}
	else if (CK_DOWN(DIK_RETURN) && m_iEquipmentState == 0 && m_bSelect == true && UM->Get_BackNum() == 4)
	{
		GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
		m_iEquipmentState = 1;
	}

	if (CK_DOWN(DIK_BACK) && m_bSelect && m_iEquipmentState == 0)
	{
		GI->PlaySoundW(L"UI_Cancel.wav", SD_UI, 0.6f);
		m_bSelect = false;
	}

	if (CK_DOWN(DIK_BACK) && m_bSelect && m_iEquipmentState == 1)
	{
		GI->PlaySoundW(L"UI_Cancel.wav", SD_UI, 0.6f);
		m_iEquipmentState = 0;
		m_iEquipmentNum = 1;
	}

	if (!m_bSelect)
	{
		if (m_UiInfo.fX >= 1920.f)
			m_UiInfo.fX = 1920.f;
		else
			m_UiInfo.fX += 128.f;
	}
	else if (m_bSelect)
	{
		if (m_UiInfo.fX <= 640.f)
			m_UiInfo.fX = 640.f;
		else
			m_UiInfo.fX -= 128.f;
	}

	if (CK_DOWN(DIK_DOWN) && m_iEquipmentState == 0 && UM->Get_BackNum() == 4)
	{
		GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
		if(m_iEquipmentClass != 4)
			m_iEquipmentClass++;
	}

	if (CK_DOWN(DIK_UP) && m_iEquipmentState == 0 && UM->Get_BackNum() == 4)
	{
		GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
		if (m_iEquipmentClass != 1)
			m_iEquipmentClass--;
	}

	if (CK_DOWN(DIK_DOWN) && m_iEquipmentState == 1 && UM->Get_BackNum() == 4)
	{
		GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
		if (m_iEquipmentNum != 7)
			m_iEquipmentNum++;
	}

	if (CK_DOWN(DIK_UP) && m_iEquipmentState == 1 && UM->Get_BackNum() == 4)
	{
		GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
		if (m_iEquipmentNum != 1)
			m_iEquipmentNum--;
	}

	if (m_bRender)
	{
		__super::Tick(fTimeDelta);
	}


	if (UM->Get_BackNum() == 4)
	{
		if (CK_DOWN(DIK_Q))
		{
			GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
			if (m_iPlayerNum == 1)
				m_iPlayerNum = 3;
			else
				m_iPlayerNum--;
		}
		else if (CK_DOWN(DIK_E))
		{
			GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
			if (m_iPlayerNum == 3)
				m_iPlayerNum = 1;
			else
				m_iPlayerNum++;
		}
	}


	
}

void CEquipmentBase::LateTick(_float fTimeDelta)
{
	UM->Set_EquipmentPlayer(m_iPlayerNum);
	UM->Set_EquipmentClassSelect(m_iEquipmentClass);
	UM->Set_EquipmentState(m_iEquipmentState);
	UM->Set_EquipmentNum(m_iEquipmentNum);

	if (m_bRender)
	{
		__super::LateTick(fTimeDelta);
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CEquipmentBase::Render()
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






HRESULT CEquipmentBase::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuBackGround"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}







void CEquipmentBase::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CEquipmentBase * CEquipmentBase::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEquipmentBase*		pInstance = new CEquipmentBase(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CEquipmentBase"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEquipmentBase::Clone(void * pArg)
{
	CEquipmentBase*		pInstance = new CEquipmentBase(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CEquipmentBase"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CEquipmentBase::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
