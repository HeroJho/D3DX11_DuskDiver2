#include "stdafx.h"
#include "..\Public\ShopBase.h"
#include "GameInstance.h"
#include "UI_Manager.h"

#include "ToolManager.h"

#include "NumExp.h"
#include "NumExplosive.h"
#include "NumReserveExp.h"
#include "NumHp.h"
#include "NumMental.h"
#include "NumMeta.h"
#include "NumStr.h"
#include "NumLevel.h"
#include "NumWeapon.h"
#include "NumWeapon1.h"
#include "NumWeapon2.h"
#include "Camera_Manager.h"



CShopBase::CShopBase(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CShopBase::CShopBase(const CShopBase & rhs)
	: CUI(rhs)
{
}

HRESULT CShopBase::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShopBase::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();

	//if (FAILED(Make_ChildUI(0.f, 0.f, 1280.f, 750.f, TEXT("Prototype_GameObject_DuskDiverBackGround"))))
	//	return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, -270.9f, 1280.f, 180.f, TEXT("Prototype_GameObject_ShopUpside"))))
		return E_FAIL;


	if (FAILED(Make_ChildUI(-514.f, -140.f, 475.f * 0.8f, 250.f * 0.8f, TEXT("Prototype_GameObject_ShopBuyBackground"))))
		return E_FAIL;


	if (FAILED(Make_ChildUI(-383.2f, -29.f, 475.f * 0.8f, 250.f * 0.8f, TEXT("Prototype_GameObject_ShopSellBackground"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-457.9f, 121.5f, 380.f , 200.f, TEXT("Prototype_GameObject_ShopUpgradeBackground"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-437.f, 267.6f, 475.f * 0.8f, 250.f * 0.8f, TEXT("Prototype_GameObject_ShopExitBackground"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-514.f, -140.f, 199.f * 0.8f, 111.f * 0.8f, TEXT("Prototype_GameObject_ShopBuySelect"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-383.2f, -29.f, 205.f * 0.8f, 106.f * 0.8f, TEXT("Prototype_GameObject_ShopSellSelect"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-457.9f, 121.5f, 357.f * 0.8f, 157.f * 0.8f, TEXT("Prototype_GameObject_ShopUpgradeSelect"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-437.f, 267.6f, 189.f * 0.8f, 114.f * 0.8f, TEXT("Prototype_GameObject_ShopExitSelect"))))
		return E_FAIL;




	if (FAILED(Make_ChildUI(31.f, 21.f, 860.f * 0.7f, 820.f * 0.7f, TEXT("Prototype_GameObject_ShopWindow"))))
		return E_FAIL;


	if (FAILED(Make_ChildUI(31.f,-210.f, 860.f * 0.7f, 55.f * 0.7f, TEXT("Prototype_GameObject_ShopSelectBar"))))
		return E_FAIL;


	if (FAILED(Make_ChildUI(190.f, -210.f, 20.f, 20.f, TEXT("Prototype_GameObject_NumExp"))))
		return E_FAIL;

	vector<CUI*>::iterator iter = m_pChildUIs.begin();
	iter += 11;
	m_pUINum = (CNumExp*)(*iter);

	if (FAILED(Make_ChildUI(190.f, -175.f, 20.f, 20.f, TEXT("Prototype_GameObject_NumReserveExp"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 12;
	m_pNumReserveExp = (CNumReserveExp*)(*iter);


	if (FAILED(Make_ChildUI(190.f, -140.f, 20.f, 20.f, TEXT("Prototype_GameObject_NumHp"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 13;
	m_pNumHp = (CNumHp*)(*iter);


	if (FAILED(Make_ChildUI(190.f,-105.f, 20.f, 20.f, TEXT("Prototype_GameObject_NumMental"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 14;
	m_pNumMental = (CNumMental*)(*iter);

	if (FAILED(Make_ChildUI(190.f, -70.f, 20.f, 20.f, TEXT("Prototype_GameObject_NumMeta"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 15;
	m_pNumMeta = (CNumMeta*)(*iter);


	if (FAILED(Make_ChildUI(190.f, -35.f, 20.f, 20.f, TEXT("Prototype_GameObject_NumStr"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 16;
	m_pNumStr = (CNumStr*)(*iter);

	if (FAILED(Make_ChildUI(177.f, 0.f, 20.f, 20.f, TEXT("Prototype_GameObject_NumLevel"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 17;
	m_pNumLevel = (CNumLevel*)(*iter);

	if (FAILED(Make_ChildUI(275.2f, -211.f, 20.f, 20.f, TEXT("Prototype_GameObject_NumWeapon"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 18;
	m_pNumWeapon = (CNumWeapon*)(*iter);

	if (FAILED(Make_ChildUI(275.2f, -175.f, 20.f, 20.f, TEXT("Prototype_GameObject_NumWeapon1"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 19;
	m_pNumWeapon1 = (CNumWeapon1*)(*iter);

	if (FAILED(Make_ChildUI(275.2f, -140.f, 20.f, 20.f, TEXT("Prototype_GameObject_NumWeapon2"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 20;
	m_pNumWeapon2 = (CNumWeapon2*)(*iter);


	if (FAILED(Make_ChildUI(91.f, 26.f, 860.f * 0.7f, 820.f * 0.7f, TEXT("Prototype_GameObject_ShopMoneyTexture"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(250.36f, -210.76f, 46.f * 0.7f, 19.f * 0.7f, TEXT("Prototype_GameObject_ShopLvMax"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(250.36f, -175.76f, 46.f * 0.7f, 19.f * 0.7f, TEXT("Prototype_GameObject_ShopLvMax3"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(250.36f, -140.76f, 46.f * 0.7f, 19.f * 0.7f, TEXT("Prototype_GameObject_ShopLvMax4"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(31.f, -210.f, 850.f * 0.7f, 54.f * 0.7f, TEXT("Prototype_GameObject_ShopEffect0"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(275.f, -316.f, 30.f, 30.f, TEXT("Prototype_GameObject_ShopMoney"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(485.f, -24.f, 530.f * 0.55f, 820.f * 0.55f, TEXT("Prototype_GameObject_ShopBattleText0"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, 0.f, 1280.f, 750.f, TEXT("Prototype_GameObject_ShopDark"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, 0.f, 1280.f, 750.f, TEXT("Prototype_GameObject_ShopDark1"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, 0.f, 1028.f * 0.7f, 400.f * 0.7f, TEXT("Prototype_GameObject_ShopBuyCount"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, 0.f, 1028.f * 0.7f, 400.f * 0.7f, TEXT("Prototype_GameObject_ShopSellCount"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, 0.f, 1028.f * 0.7f, 400.f * 0.7f, TEXT("Prototype_GameObject_ShopUpgradeCount"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(31.f, -210.f, 860.f * 0.7f, 55.f * 0.7f, TEXT("Prototype_GameObject_ShopWhite"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, 0.f, 468.f * 1.3f, 302.f * 1.3f, TEXT("Prototype_GameObject_UpgradeBase"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, -40.f, 456.f * 1.3f, 188.f * 1.3f, TEXT("Prototype_GameObject_UpgradeScene"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(17.81f, 95.42f, 21.f * 1.3f, 11.f * 1.3f, TEXT("Prototype_GameObject_UpgradePer"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-165.f, 165.f, 25.f * 1.3f, 11.f * 1.3f, TEXT("Prototype_GameObject_UpgradeWaper"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, 0.f, 468.f * 1.3f, 302.f * 1.3f, TEXT("Prototype_GameObject_UpgradeBlackOut"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, 30.f, 456.f * 2.5f, 188.f * 2.5f, TEXT("Prototype_GameObject_UpgradeSceneFail"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, 5.f, 456.f * 1.4f, 188.f * 1.4f, TEXT("Prototype_GameObject_UpgradeSceneSuccess"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, -30.f, 50.f, 50.f, TEXT("Prototype_GameObject_UpgradeTexture"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, -130.f, 295.f, 44.f, TEXT("Prototype_GameObject_UpgradeFail"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, -178.f, 155.f * 1.3f, 24.f * 1.3f, TEXT("Prototype_GameObject_UpgradeName"))))
		return E_FAIL;

	return S_OK;
}

void CShopBase::Tick(_float fTimeDelta)
{

	if (UM->Get_ShopState())
	{
		if (m_iShopSelect == 0)
		{
			m_pUINum->Set_Number(UM->Get_BattleItem(0));
			m_pNumReserveExp->Set_Number(UM->Get_BattleItem(1));
			m_pNumHp->Set_Number(UM->Get_BattleItem(2));
			m_pNumMental->Set_Number(UM->Get_BattleItem(3));
			m_pNumMeta->Set_Number(UM->Get_BattleItem(4));
			m_pNumStr->Set_Number(UM->Get_BattleItem(5));
			m_pNumLevel->Set_Number(UM->Get_BattleItem(6));
		}
		else if (m_iShopSelect == 1 && m_iPage1 == 0)
		{
			m_pUINum->Set_Number(UM->Get_BattleItem(0));
			m_pNumReserveExp->Set_Number(UM->Get_BattleItem(1));
			m_pNumHp->Set_Number(UM->Get_BattleItem(2));
			m_pNumMental->Set_Number(UM->Get_BattleItem(3));
			m_pNumMeta->Set_Number(UM->Get_BattleItem(4));
			m_pNumStr->Set_Number(UM->Get_BattleItem(5));
			m_pNumLevel->Set_Number(UM->Get_BattleItem(6));
		}
		else if (m_iShopSelect == 1 && m_iPage1 == 1)
		{
			m_pUINum->Set_Number(UM->Get_DropItem(0));
			m_pNumReserveExp->Set_Number(UM->Get_DropItem(1));
			m_pNumHp->Set_Number(UM->Get_DropItem(2));
			m_pNumMental->Set_Number(UM->Get_DropItem(3));
			m_pNumMeta->Set_Number(UM->Get_DropItem(4));
			m_pNumStr->Set_Number(UM->Get_DropItem(5));
		}

		else if (m_iShopSelect == 2)
		{
			switch (UM->Get_Player1Weapon())
			{
			case 0:
				m_pNumWeapon->Set_Number(100);
				break;
			case 1:
				m_pNumWeapon->Set_Number(200);
				break;
			case 2:
				m_pNumWeapon->Set_Number(300);
				break;
			case 3:
				m_pNumWeapon->Set_Number(400);
				break;
			case 4:
				m_pNumWeapon->Set_Number(500);
				break;
			case 5:
				m_pNumWeapon->Set_Number(600);
				break;
			case 6:
				m_pNumWeapon->Set_Number(700);
				break;
			default:
				break;
			}
			switch (UM->Get_Player2Weapon())
			{
			case 0:
				m_pNumWeapon1->Set_Number(100);
				break;
			case 1:
				m_pNumWeapon1->Set_Number(200);
				break;
			case 2:
				m_pNumWeapon1->Set_Number(300);
				break;
			case 3:
				m_pNumWeapon1->Set_Number(400);
				break;
			case 4:
				m_pNumWeapon1->Set_Number(500);
				break;
			case 5:
				m_pNumWeapon1->Set_Number(600);
				break;
			case 6:
				m_pNumWeapon1->Set_Number(700);
				break;
			default:
				break;
			}

			switch (UM->Get_Player3Weapon())
			{
			case 0:
				m_pNumWeapon2->Set_Number(100);
				break;
			case 1:
				m_pNumWeapon2->Set_Number(200);
				break;
			case 2:
				m_pNumWeapon2->Set_Number(300);
				break;
			case 3:
				m_pNumWeapon2->Set_Number(400);
				break;
			case 4:
				m_pNumWeapon2->Set_Number(500);
				break;
			case 5:
				m_pNumWeapon2->Set_Number(600);
				break;
			case 6:
				m_pNumWeapon2->Set_Number(700);
				break;
			default:
				break;
			}
		}

		if (CK_DOWN(DIK_RETURN) && !m_bSelect)
		{
			m_bSelect = true;
		}
		else if(CK_DOWN(DIK_BACK) && m_bSelect)
		{
			GI->PlaySoundW(L"UI_Cancel.wav", SD_UI, 0.6f);
			m_bSelect = false;
			m_iSelectItem = 0;
		}


		else if (m_iShopSelect == 0 && m_bSelect)
		{

			if (m_bSelectPage)
			{
				if (CK_DOWN(DIK_RIGHT))
				{
					GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
					m_iCountItem++;
				}
				else if (CK_DOWN(DIK_LEFT))
				{
					GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
					if (m_iCountItem == 0)
					{
						m_iCountItem = 0;
					}
					else
						m_iCountItem--;
				}
				if (CK_DOWN(DIK_RETURN))
				{
					GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
					GI->PlaySoundW(L"UI_GetGoldExchange.wav", SD_DROPITEM, 0.3f);
					if (m_iSelectItem == 0)
					{
						UM->Gain_BattleItem(0, m_iCountItem);
						UM->Use_Money(m_iCountItem * 100);
						UM->UsedMoney(m_iCountItem * 100);
						UM->Set_UsedMoney(true);
						m_iCountItem = 0;
					}
					else if (m_iSelectItem == 1)
					{
						UM->Gain_BattleItem(1, m_iCountItem);
						UM->Use_Money(m_iCountItem * 300);
						UM->UsedMoney(m_iCountItem * 300);
						UM->Set_UsedMoney(true);
						m_iCountItem = 0;
					}
					else if (m_iSelectItem == 2)
					{
						UM->Gain_BattleItem(2, m_iCountItem);
						UM->Use_Money(m_iCountItem * 100);
						UM->UsedMoney(m_iCountItem * 100);
						UM->Set_UsedMoney(true);
						m_iCountItem = 0;
					}
					else if (m_iSelectItem == 3)
					{
						UM->Gain_BattleItem(3, m_iCountItem);
						UM->Use_Money(m_iCountItem * 300);
						UM->UsedMoney(m_iCountItem * 300);
						UM->Set_UsedMoney(true);
						m_iCountItem = 0;
					}
					else if (m_iSelectItem == 4)
					{
						UM->Gain_BattleItem(4, m_iCountItem);
						UM->Use_Money(m_iCountItem * 100);
						UM->UsedMoney(m_iCountItem * 100);
						UM->Set_UsedMoney(true);
						m_iCountItem = 0;
					}
					else if (m_iSelectItem == 5)
					{
						UM->Gain_BattleItem(5, m_iCountItem);
						UM->Use_Money(m_iCountItem * 300);
						UM->UsedMoney(m_iCountItem * 300);
						UM->Set_UsedMoney(true);
						m_iCountItem = 0;
					}
					else if (m_iSelectItem == 6)
					{
						UM->Gain_BattleItem(6, m_iCountItem);
						UM->Use_Money(m_iCountItem * 1000);
						UM->UsedMoney(m_iCountItem * 1000);
						UM->Set_UsedMoney(true);
						m_iCountItem = 0;
					}
				}
			}

			if (CK_DOWN(DIK_RETURN) && m_bSelectPage == true)
			{
				GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
				m_bSelectPage = false;
				UM->Select_Off();
			}

			else if (CK_DOWN(DIK_RETURN) && m_bSelectPage == false)
			{
				GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
				m_bSelectPage = true;
				UM->Select_On();
			}

			else if (CK_DOWN(DIK_BACK) && m_bSelectPage == true)
			{
				GI->PlaySoundW(L"UI_Cancel.wav", SD_UI, 0.6f);
				m_bSelectPage = false;
				UM->Select_Off();
			}
		}


		else if (m_iShopSelect == 1 && m_bSelect)
		{
			if (CK_DOWN(DIK_Q))
			{
				GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
				if (m_iPage1 == 0)
					m_iPage1 = 0;
				else
					m_iPage1--;
			}
			else if (CK_DOWN(DIK_E))
			{
				GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
				if (m_iPage1 == 1)
					m_iPage1 = 1;
				else
					m_iPage1++;
			}

			if (m_bSelectPage1)
			{
				if (CK_DOWN(DIK_RIGHT))
				{
					GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
					m_iCountItem++;
				}
				else if (CK_DOWN(DIK_LEFT))
				{
					GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
					if (m_iCountItem == 0)
					{
						m_iCountItem = 0;
					}
					else
						m_iCountItem--;
				}
				if (m_iPage1 == 0)
				{
					if (CK_DOWN(DIK_RETURN))
					{
						GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
						GI->PlaySoundW(L"UI_GetGoldExchange.wav", SD_DROPITEM, 0.3f);
						if (m_iSelectItem == 0)
						{
							if (UM->Get_BattleItem(0) >= m_iCountItem)
							{
								UM->Gain_SellBattleItem(0, m_iCountItem);
								UM->Gain_Money(m_iCountItem * 50);
								UM->UsedMoney(m_iCountItem * 50);
								UM->Set_UsedMoney(true);
							}
							m_iCountItem = 0;
						}
						else if (m_iSelectItem == 1)
						{
							if (UM->Get_BattleItem(1) >= m_iCountItem)
							{
								UM->Gain_SellBattleItem(1, m_iCountItem);
								UM->Gain_Money(m_iCountItem * 150);
								UM->UsedMoney(m_iCountItem * 150);
								UM->Set_UsedMoney(true);
							}
							m_iCountItem = 0;
						}
						else if (m_iSelectItem == 2)
						{
							if (UM->Get_BattleItem(2) >= m_iCountItem)
							{
								UM->Gain_SellBattleItem(2, m_iCountItem);
								UM->Gain_Money(m_iCountItem * 50);
								UM->UsedMoney(m_iCountItem * 50);
								UM->Set_UsedMoney(true);
							}
							m_iCountItem = 0;
						}
						else if (m_iSelectItem == 3)
						{
							if (UM->Get_BattleItem(3) >= m_iCountItem)
							{
								UM->Gain_SellBattleItem(3, m_iCountItem);
								UM->Gain_Money(m_iCountItem * 150);
								UM->UsedMoney(m_iCountItem * 150);
								UM->Set_UsedMoney(true);
							}
							m_iCountItem = 0;
						}
						else if (m_iSelectItem == 4)
						{
							if (UM->Get_BattleItem(4) >= m_iCountItem)
							{
								UM->Gain_SellBattleItem(4, m_iCountItem);
								UM->Gain_Money(m_iCountItem * 50);
								UM->UsedMoney(m_iCountItem * 50);
								UM->Set_UsedMoney(true);
							}
							m_iCountItem = 0;
						}
						else if (m_iSelectItem == 5)
						{
							if (UM->Get_BattleItem(5) >= m_iCountItem)
							{
								UM->Gain_SellBattleItem(5, m_iCountItem);
								UM->Gain_Money(m_iCountItem * 150);
								UM->UsedMoney(m_iCountItem * 150);
								UM->Set_UsedMoney(true);
							}
							m_iCountItem = 0;
						}
						else if (m_iSelectItem == 6)
						{
							if (UM->Get_BattleItem(6) >= m_iCountItem)
							{
								UM->Gain_SellBattleItem(6, m_iCountItem);
								UM->Gain_Money(m_iCountItem * 1000);
								UM->UsedMoney(m_iCountItem * 1000);
								UM->Set_UsedMoney(true);
							}
							m_iCountItem = 0;
						}
					}
				}
				else if (m_iPage1 == 1)
				{
					if (CK_DOWN(DIK_RETURN))
					{
						GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
						GI->PlaySoundW(L"UI_GetGoldExchange.wav", SD_DROPITEM, 0.3f);
						if (m_iSelectItem == 0)
						{
							if (UM->Get_DropItem(0) >= m_iCountItem)
							{
								UM->Sell_DropItem(0, m_iCountItem);
								UM->Gain_Money(m_iCountItem * 100);
								UM->UsedMoney(m_iCountItem * 100);
								UM->Set_UsedMoney(true);
							}
							m_iCountItem = 0;
						}
						else if (m_iSelectItem == 1)
						{
							if (UM->Get_DropItem(1) >= m_iCountItem)
							{
								UM->Sell_DropItem(1, m_iCountItem);
								UM->Gain_Money(m_iCountItem * 150);
								UM->UsedMoney(m_iCountItem * 150);
								UM->Set_UsedMoney(true);
							}
							m_iCountItem = 0;
						}
						else if (m_iSelectItem == 2)
						{
							if (UM->Get_DropItem(2) >= m_iCountItem)
							{
								UM->Sell_DropItem(2, m_iCountItem);
								UM->Gain_Money(m_iCountItem * 300);
								UM->UsedMoney(m_iCountItem * 300);
								UM->Set_UsedMoney(true);
							}
							m_iCountItem = 0;
						}
						else if (m_iSelectItem == 3)
						{
							if (UM->Get_DropItem(3) >= m_iCountItem)
							{
								UM->Sell_DropItem(3, m_iCountItem);
								UM->Gain_Money(m_iCountItem * 250);
								UM->UsedMoney(m_iCountItem * 250);
								UM->Set_UsedMoney(true);
							}
							m_iCountItem = 0;
						}
						else if (m_iSelectItem == 4)
						{
							if (UM->Get_DropItem(4) >= m_iCountItem)
							{
								UM->Sell_DropItem(4, m_iCountItem);
								UM->Gain_Money(m_iCountItem * 350);
								UM->UsedMoney(m_iCountItem * 350);
								UM->Set_UsedMoney(true);
							}
							m_iCountItem = 0;
						}
						else if (m_iSelectItem == 5)
						{
							if (UM->Get_DropItem(5) >= m_iCountItem)
							{
								UM->Sell_DropItem(5, m_iCountItem);
								UM->Gain_Money(m_iCountItem * 500);
								UM->UsedMoney(m_iCountItem * 500);
								UM->Set_UsedMoney(true);
							}
							m_iCountItem = 0;
						}
						else if (m_iSelectItem == 6)
						{
							if (UM->Get_DropItem(6) >= m_iCountItem)
							{
								UM->Sell_DropItem(6, m_iCountItem);
								UM->Gain_Money(m_iCountItem * 100);
								UM->UsedMoney(m_iCountItem * 100);
								UM->Set_UsedMoney(true);
							}
							m_iCountItem = 0;
						}
					}
				}
			}

			

			if (CK_DOWN(DIK_RETURN) && m_bSelectPage1 == true)
			{
				GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
				m_bSelectPage1 = false;
				UM->SellSelect_Off();
			}

			else if (CK_DOWN(DIK_RETURN) && m_bSelectPage1 == false)
			{
				GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
				m_bSelectPage1 = true;
				UM->SellSelect_On();
			}

			else if (CK_DOWN(DIK_BACK) && m_bSelectPage1 == true)
			{
				GI->PlaySoundW(L"UI_Cancel.wav", SD_UI, 0.6f);
				m_bSelectPage1 = false;
				UM->SellSelect_Off();
			}
		}


		else if (m_iShopSelect == 2 && m_bSelect)
		{
			if (m_bSelectPage2)
			{
				if (CK_DOWN(DIK_RETURN))
				{
					GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
					GI->PlaySoundW(L"UI_GetGoldExchange.wav", SD_DROPITEM, 0.3f);
					if (m_iSelectItem == 0 && UM->Get_Player1Weapon() < 6)
					{
						UM->UpgradeSelect_On();
						int i = 0;
						if (UM->Get_Player1Weapon() == 0)
						{
							i = GI->Get_Random(1, 1);
							UM->Use_Money(100);
							UM->UsedMoney(100);
							UM->Set_UsedMoney(true);
							UM->Set_BeforeWeapon(0);
						}
						else if (UM->Get_Player1Weapon() == 1)
						{
							i = GI->Get_Random(0, 5);
							UM->Use_Money(200);
							UM->UsedMoney(200);
							UM->Set_UsedMoney(true);
							UM->Set_BeforeWeapon(1);
						}
						else if (UM->Get_Player1Weapon() == 2)
						{
							i = GI->Get_Random(0, 4);
							UM->Use_Money(300);
							UM->UsedMoney(300);
							UM->Set_UsedMoney(true);
							UM->Set_BeforeWeapon(2);
						}
						else if (UM->Get_Player1Weapon() == 3)
						{
							i = GI->Get_Random(0, 3);
							UM->Use_Money(400);
							UM->UsedMoney(400);
							UM->Set_UsedMoney(true);
							UM->Set_BeforeWeapon(3);
						}
						else if (UM->Get_Player1Weapon() == 4)
						{
							i = GI->Get_Random(0, 2);
							UM->Use_Money(500);
							UM->UsedMoney(500);
							UM->Set_UsedMoney(true);
							UM->Set_BeforeWeapon(4);
						}
						else if (UM->Get_Player1Weapon() == 5)
						{
							i = GI->Get_Random(0, 1);
							UM->Use_Money(600);
							UM->UsedMoney(600);
							UM->Set_UsedMoney(true);
							UM->Set_BeforeWeapon(5);
						}

						if (i == 0)
						{
							UM->Set_Player1Weapon(UM->Get_Player1Weapon() - 1);
							UM->Set_Upgrade(false);
						}
						else
						{
							UM->Set_Player1Weapon(UM->Get_Player1Weapon() + 1);
							UM->Set_Upgrade(true);
						}
					}

					else if (m_iSelectItem == 1 && UM->Get_Player2Weapon() < 6)
					{
						int i = 0;

						UM->UpgradeSelect_On();
						if (UM->Get_Player2Weapon() == 0)
						{
							i = GI->Get_Random(1, 1);
							UM->Use_Money(100);
							UM->UsedMoney(100);
							UM->Set_UsedMoney(true);
							UM->Set_BeforeWeapon(0);
						}
						else if (UM->Get_Player2Weapon() == 1)
						{
							i = GI->Get_Random(0, 5);
							UM->Use_Money(200);
							UM->UsedMoney(200);
							UM->Set_UsedMoney(true);
							UM->Set_BeforeWeapon(1);
						}
						else if (UM->Get_Player2Weapon() == 2)
						{
							i = GI->Get_Random(0, 4);
							UM->Use_Money(300);
							UM->UsedMoney(300);
							UM->Set_UsedMoney(true);
							UM->Set_BeforeWeapon(2);
						}
						else if (UM->Get_Player2Weapon() == 3)
						{
							i = GI->Get_Random(0, 3);
							UM->Use_Money(400);
							UM->UsedMoney(400);
							UM->Set_UsedMoney(true);
							UM->Set_BeforeWeapon(3);
						}
						else if (UM->Get_Player2Weapon() == 4)
						{
							i = GI->Get_Random(0, 2);
							UM->Use_Money(500);
							UM->UsedMoney(500);
							UM->Set_UsedMoney(true);
							UM->Set_BeforeWeapon(4);
						}
						else if (UM->Get_Player2Weapon() == 5)
						{
							i = GI->Get_Random(0, 1);
							UM->Use_Money(600);
							UM->UsedMoney(600);
							UM->Set_UsedMoney(true);
							UM->Set_BeforeWeapon(5);
						}

						if (i == 0)
						{
							UM->Set_Player2Weapon(UM->Get_Player2Weapon() - 1);
							UM->Set_Upgrade(false);
						}
						else
						{
							UM->Set_Player2Weapon(UM->Get_Player2Weapon() + 1);
							UM->Set_Upgrade(true);
						}
					}

					else if (m_iSelectItem == 2 && UM->Get_Player3Weapon() < 6)
					{
						UM->UpgradeSelect_On();
						int i = 0;
						if (UM->Get_Player3Weapon() == 0)
						{
							i = GI->Get_Random(1, 1);
							UM->Use_Money(100);
							UM->UsedMoney(100);
							UM->Set_UsedMoney(true);
							UM->Set_BeforeWeapon(0);
						}
						else if (UM->Get_Player3Weapon() == 1)
						{
							i = GI->Get_Random(0, 5);
							UM->Use_Money(200);
							UM->UsedMoney(200);
							UM->Set_UsedMoney(true);
							UM->Set_BeforeWeapon(1);
						}
						else if (UM->Get_Player3Weapon() == 2)
						{
							i = GI->Get_Random(0, 4);
							UM->Use_Money(300);
							UM->UsedMoney(300);
							UM->Set_UsedMoney(true);
							UM->Set_BeforeWeapon(2);
						}
						else if (UM->Get_Player3Weapon() == 3)
						{
							i = GI->Get_Random(0, 3);
							UM->Use_Money(400);
							UM->UsedMoney(400);
							UM->Set_UsedMoney(true);
							UM->Set_BeforeWeapon(3);
						}
						else if (UM->Get_Player3Weapon() == 4)
						{
							i = GI->Get_Random(0, 2);
							UM->Use_Money(500);
							UM->UsedMoney(500);
							UM->Set_UsedMoney(true);
							UM->Set_BeforeWeapon(4);
						}
						else if (UM->Get_Player3Weapon() == 5)
						{
							i = GI->Get_Random(0, 1);
							UM->Use_Money(600);
							UM->UsedMoney(600);
							UM->Set_UsedMoney(true);
							UM->Set_BeforeWeapon(5);
						}
						if (i == 0)
						{
							UM->Set_Player3Weapon(UM->Get_Player3Weapon() - 1);
							UM->Set_Upgrade(false);
						}
						else
						{
							UM->Set_Player3Weapon(UM->Get_Player3Weapon() + 1);
							UM->Set_Upgrade(true);
						}
					}
				}
			}

			if (CK_DOWN(DIK_BACK) && m_bSelectPage2 == true)
			{
				GI->PlaySoundW(L"UI_Cancel.wav", SD_UI, 0.6f);
				m_bSelectPage2 = false;
				UM->UpgradeSelect_Off();
			}

			else if (CK_DOWN(DIK_RETURN) && m_bSelectPage2 == false && !UM->Get_UpgradeFinish())
			{
				if (m_iSelectItem == 0 && UM->Get_Player1Weapon() < 6)
				{
					m_bSelectPage2 = true;
					UM->UpgradeSelect_On();
				}
				else if (m_iSelectItem == 1 && UM->Get_Player2Weapon() < 6)
				{
					m_bSelectPage2 = true;
					UM->UpgradeSelect_On();
				}
				else if (m_iSelectItem == 2 && UM->Get_Player3Weapon() < 6)
				{
					m_bSelectPage2 = true;
					UM->UpgradeSelect_On();
				}
			}

			else if (CK_DOWN(DIK_RETURN) && m_bSelectPage2 == true && !UM->Get_UpgradeFinish())
			{
				m_bSelectPage2 = false;
				UM->UpgradeSelect_Off();
				UM->UpgradeFinish_On();
			}
			else if (CK_DOWN(DIK_RETURN) && UM->Get_UpgradeFinish())
			{
				UM->UpgradeFinish_Off();
			}


		}


		if (!m_bSelectPage1 && m_bSelect)
		{
			if (CK_DOWN(DIK_UP))
			{
				GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
				if (m_iSelectItem == 0)
				{
					m_iSelectItem = 0;
				}
				else
				{
					m_iSelectItem--;
				}

			}
			else if (CK_DOWN(DIK_DOWN))
			{
				GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
				if (m_iSelectItem == 6)
				{
					m_iSelectItem = 6;
				}
				else
				{
					m_iSelectItem++;
				}
			}
		}


		if (!m_bSelect)
		{
			if (CK_DOWN(DIK_DOWN))
			{
				GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
				if (m_iShopSelect == 3)
					m_iShopSelect = 3;
				else
					m_iShopSelect++;
			}
			else if (CK_DOWN(DIK_UP))
			{
				GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
				if (m_iShopSelect == 0)
					m_iShopSelect = 0;
				else
					m_iShopSelect--;
			}
		}
	}





	if (UM->Get_ShopState())
	{
		//UI_InputDebug(fTimeDelta);

		__super::Tick(fTimeDelta);

		UM->Set_ShopItemCount(m_iCountItem);
		UM->Set_ShopItem(m_iSelectItem);
		UM->Set_ShopSelect(m_iShopSelect);
		UM->Set_Page1Select(m_iPage1);
		UM->Set_ShopSelectBar(m_bSelect);
	}

		if (m_iShopSelect == 3)
		{
			if (CK_DOWN(DIK_RETURN))
			{
				GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
				GI->PlaySoundW(L"NA24_com_02.wav", SD_MAP, 0.8f);
				UM->Shop_Off();
				m_iShopSelect = 0;
				m_bSelect = false;
				CRM->Get_Cam()->End_Target();

			}
		}
}

void CShopBase::LateTick(_float fTimeDelta)
{
	if (UM->Get_ShopState())
	{
		__super::LateTick(fTimeDelta);
	}

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CShopBase::Render()
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

	if (UM->Get_ShopState())
	{
		//if (FAILED(m_pVIBufferCom->Render()))
		//	return E_FAIL;


		//UI_RenderDebug();

		 //자식들의 Render 호출
		__super::Render();
	}


	return S_OK;
}






HRESULT CShopBase::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuBackGround"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

void CShopBase::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CShopBase * CShopBase::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CShopBase*		pInstance = new CShopBase(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CShopBase"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CShopBase::Clone(void * pArg)
{
	CShopBase*		pInstance = new CShopBase(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CShopBase"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CShopBase::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
