#include "stdafx.h"
#include "..\Public\StatusBase.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "UI_Manager.h"
#include "NumExp.h"
#include "NumReserveExp.h"
#include "NumLevel.h"
#include "NumHp.h"
#include "NumMental.h"
#include "NumMeta.h"
#include "NumStr.h"
#include "NumStamina.h"
#include "NumExplosive.h"
#include "NumLuck.h"
#include "Status_Manager.h"


CStatusBase::CStatusBase(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CUI(pDevice, pContext)
{
}

CStatusBase::CStatusBase(const CStatusBase & rhs)
	: CUI(rhs)
{
}

HRESULT CStatusBase::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CStatusBase::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Make_ImGui();


	if (FAILED(Make_ChildUI(0.f, 0.f, 1280.f, 750.f, TEXT("Prototype_GameObject_DuskDiverBackGround"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(0.f, -344.1754f, 1920.f * 0.67f, 160.f * 0.67f, TEXT("Prototype_GameObject_StatusBar"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-100.f, 25.f, 860.f * 0.7f, 820.f * 0.7f, TEXT("Prototype_GameObject_StatusBackGround"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(422.7f, 25.f, 530.f * 0.7f, 820.f * 0.7f, TEXT("Prototype_GameObject_StatusWindow"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(436.02f, 342.f, 452 * 0.9f, 36.f * 0.9f, TEXT("Prototype_GameObject_StatusGuide"))))
		return E_FAIL;


	if (FAILED(Make_ChildUI(-100.f, -198.f , 860.f * 0.7f, 60.f, TEXT("Prototype_GameObject_StatusSelectBar"))))
		return E_FAIL;




	if (FAILED(Make_ChildUI(181.6f, -195.f, 800.f * 0.7f, 60.f * 0.7f, TEXT("Prototype_GameObject_StatusStr"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(181.6f, -129.1f, 800.f * 0.7f, 60.f * 0.7f, TEXT("Prototype_GameObject_StatusStamina"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(181.6f, -64.1f, 800.f * 0.7f, 60.f * 0.7f, TEXT("Prototype_GameObject_StatusHp"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(181.6f, 6.88f, 800.f * 0.7f, 60.f * 0.7f, TEXT("Prototype_GameObject_StatusMental"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(181.6f, 71.91f, 800.f * 0.7f, 60.f * 0.7f, TEXT("Prototype_GameObject_StatusMeta"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(181.6f, 136.1f, 800.f * 0.7f, 60.f * 0.7f, TEXT("Prototype_GameObject_StatusExplosive"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(181.6f, 206.15f, 800.f * 0.7f, 60.f * 0.7f, TEXT("Prototype_GameObject_StatusLuck"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-100.f, 25.f, 860.f * 0.7f, 820.f * 0.7f, TEXT("Prototype_GameObject_StatIcon"))))
		return E_FAIL;



	if (FAILED(Make_ChildUI(-139.2f, -195.f, 60.f, 25.f, TEXT("Prototype_GameObject_Player1StrLv"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-139.2f, -129.1f, 60.f, 25.f, TEXT("Prototype_GameObject_Player1StaminaLv"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-139.2f, -64.1f, 60.f, 25.f, TEXT("Prototype_GameObject_Player1HpLv"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-139.2f, 6.88f, 60.f, 25.f, TEXT("Prototype_GameObject_Player1MentalLv"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-139.2f, 71.91f, 60.f, 25.f, TEXT("Prototype_GameObject_Player1MetaLv"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-139.2f, 136.1f, 60.f, 25.f, TEXT("Prototype_GameObject_Player1ExplosiveLv"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-139.2f, 206.2f, 60.f, 25.f, TEXT("Prototype_GameObject_Player1LuckLv"))))
		return E_FAIL;


	if (FAILED(Make_ChildUI(387.f, -324.f, 128.f * 0.4f, 128.f * 0.4f, TEXT("Prototype_GameObject_PlayerCircle"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(387.f, -324.f, 128.f * 0.4f, 128.f * 0.4f, TEXT("Prototype_GameObject_Player1Icon"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(455.f, -324.f, 128.f * 0.4f, 128.f * 0.4f, TEXT("Prototype_GameObject_Player2Icon"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(527.f, -324.f, 128.f * 0.4f, 128.f * 0.4f, TEXT("Prototype_GameObject_Player3Icon"))))
		return E_FAIL;


	if (FAILED(Make_ChildUI(591.3f, -243.3f, 20.f, 20.f, TEXT("Prototype_GameObject_NumExp"))))
		return E_FAIL;		

	vector<CUI*>::iterator iter = m_pChildUIs.begin();
	iter += 25;
	m_pUINum = (CNumExp*)(*iter);


	if (FAILED(Make_ChildUI(591.3f, -213.2f, 20.f, 20.f, TEXT("Prototype_GameObject_NumReserveExp"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 26;
	m_pNumReserveExp = (CNumReserveExp*)(*iter);


	if (FAILED(Make_ChildUI(562.2f, -31.5f, 20.f, 20.f, TEXT("Prototype_GameObject_NumLevel"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 27;
	m_pNumLevel = (CNumLevel*)(*iter);


	if (FAILED(Make_ChildUI(589.7f, 4.5f, 20.f, 20.f, TEXT("Prototype_GameObject_NumHp"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 28;
	m_pNumHp = (CNumHp*)(*iter);


	if (FAILED(Make_ChildUI(589.7f, 39.5f, 20.f, 20.f, TEXT("Prototype_GameObject_NumMental"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 29;
	m_pNumMental = (CNumMental*)(*iter);

	if (FAILED(Make_ChildUI(589.7f, 74.5f, 20.f, 20.f, TEXT("Prototype_GameObject_NumMeta"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 30;
	m_pNumMeta = (CNumMeta*)(*iter);


	if (FAILED(Make_ChildUI(589.7f, 109.5f, 20.f, 20.f, TEXT("Prototype_GameObject_NumStr"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 31;
	m_pNumStr = (CNumStr*)(*iter);


	if (FAILED(Make_ChildUI(589.7f, 144.5f, 20.f, 20.f, TEXT("Prototype_GameObject_NumStamina"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 32;
	m_pNumStamina = (CNumStamina*)(*iter);



	if (FAILED(Make_ChildUI(589.7f, 179.5f, 20.f, 20.f, TEXT("Prototype_GameObject_NumLuck"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 33;
	m_pNumLuck = (CNumLuck*)(*iter);



	if (FAILED(Make_ChildUI(589.7f, 214.5f, 20.f, 20.f, TEXT("Prototype_GameObject_NumExplosive"))))
		return E_FAIL;

	iter = m_pChildUIs.begin();
	iter += 34;
	m_pNumExplosive = (CNumExplosive*)(*iter);


	if (FAILED(Make_ChildUI(-100.f, -198.f, 860.f * 0.7f, 60.f, TEXT("Prototype_GameObject_LevelUpEffect"))))
		return E_FAIL;


	if (FAILED(Make_ChildUI(-471.1f, -317.3f, 567.f * 0.6f, 184.f * 0.6f, TEXT("Prototype_GameObject_StatusLT1"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-471.1f, -317.3f, 567.f * 0.6f, 184.f * 0.6f, TEXT("Prototype_GameObject_StatusLT2"))))
		return E_FAIL;

	if (FAILED(Make_ChildUI(-471.1f, -317.3f, 567.f * 0.6f, 184.f * 0.6f, TEXT("Prototype_GameObject_StatusLT3"))))
		return E_FAIL;


	return S_OK;
}

void CStatusBase::Tick(_float fTimeDelta)
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
		UM->Set_BackNum(0);
		m_UiInfo.fX = 1920.f;
	}



	if (m_bRender)
	{
		m_pUINum->Set_Number(UM->Get_Exp());

		if (CK_DOWN(DIK_RETURN) && UM->Get_BackNum() == 2 && !m_bSelect)
		{
			GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
			m_bSelect = true;
			m_iPlayerNum = UM->Get_PlayerSelectNum();
		}


		if (CK_DOWN(DIK_BACK) && m_bSelect)
		{
			GI->PlaySoundW(L"UI_Cancel.wav", SD_UI, 0.6f);
			m_bSelect = false;
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



		if (m_bRender)
			__super::Tick(fTimeDelta);

		if (UM->Get_BackNum() == 2)
		{
			if (CK_DOWN(DIK_Q))
			{
				GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
				if (m_iPlayerNum == 1)
					m_iPlayerNum = 3;
				else
					m_iPlayerNum--;

				m_iUp = 0;
				m_iStatusNum = 1;
			}
			else if (CK_DOWN(DIK_E))
			{
				GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
				if (m_iPlayerNum == 3)
					m_iPlayerNum = 1;
				else
					m_iPlayerNum++;

				m_iUp = 0;
				m_iStatusNum = 1;
			}
		}

		UM->Set_PlayerSelectNum(m_iPlayerNum);


		if (UM->Get_BackNum() == 2)
		{
			if (m_iPlayerNum == 1)
			{
				if (CK_DOWN(DIK_DOWN))
				{
					GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
					if (m_iStatusNum == 7)
						m_iStatusNum = 7;
					else
						m_iStatusNum++;


					m_iUp = 0;
				}
				else if (CK_DOWN(DIK_UP))
				{
					GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
					if (m_iStatusNum == 1)
						m_iStatusNum = 1;
					else
						m_iStatusNum--;

					m_iUp = 0;
				}

				if (CK_DOWN(DIK_RETURN))
				{
					if (m_iUp > 0)
					{
						GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
						GI->PlaySoundW(L"UI_SkillUp_Confirm.wav", SD_BULLET, 0.6f);
					}
					switch (m_iStatusNum)
					{
					case 1:
						m_iPlayer1Str = m_iPlayer1Str + m_iUp;
						break;
					case 2:
						m_iPlayer1Stamina = m_iPlayer1Stamina + m_iUp;
						break;
					case 3:
						m_iPlayer1Hp = m_iPlayer1Hp + m_iUp;
						UM->Set_StatUp1(true);
						break;
					case 4:
						m_iPlayer1Mental = m_iPlayer1Mental + m_iUp;
						break;
					case 5:
						m_iPlayer1Meta = m_iPlayer1Meta + m_iUp;
						break;
					case 6:
						m_iPlayer1Explosive = m_iPlayer1Explosive + m_iUp;
						break;
					case 7:
						m_iPlayer1Luck = m_iPlayer1Luck + m_iUp;
						break;
					}
					UM->Use_Exp(m_iUp * 1000);
					if(m_iUp > 0)
						UM->Set_LevelUpEffect(true);
					m_iUp = 0;
				}

				if (UM->Get_Exp() >= (m_iUp + 1) * 1000)
				{
					if (m_iStatusNum == 1)
					{
						if (CK_DOWN(DIK_RIGHT))
						{
							GI->PlaySoundW(L"UI_SkillUp.wav", SD_UI, 0.6f);
							if (m_iPlayer1Str + m_iUp < 20)
							{
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer1Str + m_iUp - 1), -195.f, 31.f * 0.82f, 60.f * 0.72f, TEXT("Prototype_GameObject_ReserveBar"));
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer1Str + m_iUp - 1), -195.f, 256.f * 0.15f, 256.f * 0.15f, TEXT("Prototype_GameObject_ReserveEffect"));
								m_iUp++;
							}
						}
					}
					else if (m_iStatusNum == 2)
					{
						if (CK_DOWN(DIK_RIGHT))
						{
							GI->PlaySoundW(L"UI_SkillUp.wav", SD_UI, 0.6f); 
							{
								if (m_iPlayer1Stamina + m_iUp < 20)
								{
									Make_ChildUI(-74.3f + 14.1f * (m_iPlayer1Stamina + m_iUp - 1), -129.1f, 31.f * 0.82f, 60.f * 0.72f, TEXT("Prototype_GameObject_ReserveBar"));
									Make_ChildUI(-74.3f + 14.1f * (m_iPlayer1Stamina + m_iUp - 1), -129.1f, 256.f * 0.15f, 256.f * 0.15f, TEXT("Prototype_GameObject_ReserveEffect"));
									m_iUp++;
								}
							}
						}
					}
					else if (m_iStatusNum == 3)
					{
						if (CK_DOWN(DIK_RIGHT))
						{
							GI->PlaySoundW(L"UI_SkillUp.wav", SD_UI, 0.6f);
							if (m_iPlayer1Hp + m_iUp < 20)
							{
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer1Hp + m_iUp - 1), -64.1f, 31.f * 0.82f, 60.f * 0.72f, TEXT("Prototype_GameObject_ReserveBar"));
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer1Hp + m_iUp - 1), -64.1f, 256.f * 0.15f, 256.f * 0.15f, TEXT("Prototype_GameObject_ReserveEffect"));
								m_iUp++;
							}
						}
					}
					else if (m_iStatusNum == 4)
					{
						if (CK_DOWN(DIK_RIGHT))
						{
							GI->PlaySoundW(L"UI_SkillUp.wav", SD_UI, 0.6f);
							if (m_iPlayer1Mental + m_iUp < 20)
							{
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer1Mental + m_iUp - 1), 6.88f, 31.f * 0.82f, 60.f * 0.72f, TEXT("Prototype_GameObject_ReserveBar"));
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer1Mental + m_iUp - 1), 6.88f, 256.f * 0.15f, 256.f * 0.15f, TEXT("Prototype_GameObject_ReserveEffect"));
								m_iUp++;
							}
						}
					}
					else if (m_iStatusNum == 5)
					{
						if (CK_DOWN(DIK_RIGHT))
						{
							GI->PlaySoundW(L"UI_SkillUp.wav", SD_UI, 0.6f);
							if (m_iPlayer1Meta + m_iUp < 20)
							{
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer1Meta + m_iUp - 1), 71.91f, 31.f * 0.82f, 60.f * 0.72f, TEXT("Prototype_GameObject_ReserveBar"));
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer1Meta + m_iUp - 1), 71.91f, 256.f * 0.15f, 256.f * 0.15f, TEXT("Prototype_GameObject_ReserveEffect"));
								m_iUp++;
							}
						}
					}
					else if (m_iStatusNum == 6)
					{
						if (CK_DOWN(DIK_RIGHT))
						{
							GI->PlaySoundW(L"UI_SkillUp.wav", SD_UI, 0.6f);
							if (m_iPlayer1Explosive + m_iUp < 20)
							{
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer1Explosive + m_iUp - 1), 136.1f, 31.f * 0.82f, 60.f * 0.72f, TEXT("Prototype_GameObject_ReserveBar"));
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer1Explosive + m_iUp - 1), 136.1f, 256.f * 0.15f, 256.f * 0.15f, TEXT("Prototype_GameObject_ReserveEffect"));
								m_iUp++;
							}
						}
					}
					else if (m_iStatusNum == 7)
					{
						if (CK_DOWN(DIK_RIGHT))
						{
							GI->PlaySoundW(L"UI_SkillUp.wav", SD_UI, 0.6f);
							if (m_iPlayer1Luck + m_iUp < 20)
							{
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer1Luck + m_iUp - 1), 206.2f, 31.f * 0.82f, 60.f * 0.72f, TEXT("Prototype_GameObject_ReserveBar"));
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer1Luck + m_iUp - 1), 206.2f, 256.f * 0.15f, 256.f * 0.15f, TEXT("Prototype_GameObject_ReserveEffect"));
								m_iUp++;
							}
						}
					}
				}
			}


			if (m_iPlayerNum == 2)
			{
				if (CK_DOWN(DIK_DOWN))
				{
					GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
					if (m_iStatusNum == 7)
						m_iStatusNum = 7;
					else
						m_iStatusNum++;


					m_iUp = 0;
				}
				else if (CK_DOWN(DIK_UP))
				{
					GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
					if (m_iStatusNum == 1)
						m_iStatusNum = 1;
					else
						m_iStatusNum--;

					m_iUp = 0;
				}

				if (CK_DOWN(DIK_RETURN))
				{
					if (m_iUp > 0)
					{
						GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
						GI->PlaySoundW(L"UI_SkillUp_Confirm.wav", SD_BULLET, 0.6f);
					}
					switch (m_iStatusNum)
					{
					case 1:
						m_iPlayer2Str = m_iPlayer2Str + m_iUp;
						break;
					case 2:
						m_iPlayer2Stamina = m_iPlayer2Stamina + m_iUp;
						break;
					case 3:
						m_iPlayer2Hp = m_iPlayer2Hp + m_iUp;
						UM->Set_StatUp3(true);
						break;
					case 4:
						m_iPlayer2Mental = m_iPlayer2Mental + m_iUp;
						break;
					case 5:
						m_iPlayer2Meta = m_iPlayer2Meta + m_iUp;
						break;
					case 6:
						m_iPlayer2Explosive = m_iPlayer2Explosive + m_iUp;
						break;
					case 7:
						m_iPlayer2Luck = m_iPlayer2Luck + m_iUp;
						break;
					}
					UM->Use_Exp(m_iUp * 1000);
					if (m_iUp > 0)
						UM->Set_LevelUpEffect(true);
					m_iUp = 0;
				}

				if (UM->Get_Exp() >= (m_iUp + 1) * 1000)
				{
					if (m_iStatusNum == 1)
					{
						if (CK_DOWN(DIK_RIGHT))
						{
							GI->PlaySoundW(L"UI_SkillUp.wav", SD_UI, 0.6f);
							if (m_iPlayer2Str + m_iUp < 20)
							{
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer2Str + m_iUp - 1), -195.f, 31.f * 0.82f, 43.2f, TEXT("Prototype_GameObject_ReserveBar"));
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer2Str + m_iUp - 1), -195.f, 256.f * 0.15f, 256.f * 0.15f, TEXT("Prototype_GameObject_ReserveEffect"));
								m_iUp++;
							}
						}
					}
					else if (m_iStatusNum == 2)
					{
						if (CK_DOWN(DIK_RIGHT))
						{
							GI->PlaySoundW(L"UI_SkillUp.wav", SD_UI, 0.6f);
							if (m_iPlayer2Stamina + m_iUp < 20)
							{
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer2Stamina + m_iUp - 1), -129.1f, 31.f * 0.82f, 60.f * 0.72f, TEXT("Prototype_GameObject_ReserveBar"));
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer2Stamina + m_iUp - 1), -129.1f, 256.f * 0.15f, 256.f * 0.15f, TEXT("Prototype_GameObject_ReserveEffect"));
								m_iUp++;
							}
						}
					}
					else if (m_iStatusNum == 3)
					{
						if (CK_DOWN(DIK_RIGHT))
						{
							GI->PlaySoundW(L"UI_SkillUp.wav", SD_UI, 0.6f);
							if (m_iPlayer2Hp + m_iUp < 20)
							{
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer2Hp + m_iUp - 1), -64.1f, 31.f * 0.82f, 60.f * 0.72f, TEXT("Prototype_GameObject_ReserveBar"));
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer2Hp + m_iUp - 1), -64.1f, 256.f * 0.15f, 256.f * 0.15f, TEXT("Prototype_GameObject_ReserveEffect"));
								m_iUp++;
							}
						}
					}
					else if (m_iStatusNum == 4)
					{
						if (CK_DOWN(DIK_RIGHT))
						{
							GI->PlaySoundW(L"UI_SkillUp.wav", SD_UI, 0.6f);
							if (m_iPlayer2Mental + m_iUp < 20)
							{
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer2Mental + m_iUp - 1), 6.88f, 31.f * 0.82f, 60.f * 0.72f, TEXT("Prototype_GameObject_ReserveBar"));
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer2Mental + m_iUp - 1), 6.88f, 256.f * 0.15f, 256.f * 0.15f, TEXT("Prototype_GameObject_ReserveEffect"));
								m_iUp++;
							}
						}
					}
					else if (m_iStatusNum == 5)
					{
						if (CK_DOWN(DIK_RIGHT))
						{
							GI->PlaySoundW(L"UI_SkillUp.wav", SD_UI, 0.6f);
							if (m_iPlayer2Meta + m_iUp < 20)
							{
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer2Meta + m_iUp - 1), 71.91f, 31.f * 0.82f, 60.f * 0.72f, TEXT("Prototype_GameObject_ReserveBar"));
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer2Meta + m_iUp - 1), 71.91f, 256.f * 0.15f, 256.f * 0.15f, TEXT("Prototype_GameObject_ReserveEffect"));
								m_iUp++;
							}
						}
					}
					else if (m_iStatusNum == 6)
					{
						if (CK_DOWN(DIK_RIGHT))
						{
							GI->PlaySoundW(L"UI_SkillUp.wav", SD_UI, 0.6f);
							if (m_iPlayer2Explosive + m_iUp < 20)
							{
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer2Explosive + m_iUp - 1), 136.1f, 31.f * 0.82f, 60.f * 0.72f, TEXT("Prototype_GameObject_ReserveBar"));
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer2Explosive + m_iUp - 1), 136.1f, 256.f * 0.15f, 256.f * 0.15f, TEXT("Prototype_GameObject_ReserveEffect"));
								m_iUp++;
							}
						}
					}
					else if (m_iStatusNum == 7)
					{
						if (CK_DOWN(DIK_RIGHT))
						{
							GI->PlaySoundW(L"UI_SkillUp.wav", SD_UI, 0.6f);
							if (m_iPlayer2Luck + m_iUp < 20)
							{
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer2Luck + m_iUp - 1), 206.2f, 31.f * 0.82f, 60.f * 0.72f, TEXT("Prototype_GameObject_ReserveBar"));
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer2Luck + m_iUp - 1), 206.2f, 256.f * 0.15f, 256.f * 0.15f, TEXT("Prototype_GameObject_ReserveEffect"));
								m_iUp++;
							}
						}
					}
				}
			}


			if (m_iPlayerNum == 3)
			{
				if (CK_DOWN(DIK_DOWN))
				{
					GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
					if (m_iStatusNum == 7)
						m_iStatusNum = 7;
					else
						m_iStatusNum++;


					m_iUp = 0;
				}
				else if (CK_DOWN(DIK_UP))
				{
					GI->PlaySoundW(L"UI_select.wav", SD_UI, 0.6f);
					if (m_iStatusNum == 1)
						m_iStatusNum = 1;
					else
						m_iStatusNum--;

					m_iUp = 0;
				}

				if (CK_DOWN(DIK_RETURN))
				{
					if (m_iUp > 0)
					{
						GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
						GI->PlaySoundW(L"UI_SkillUp_Confirm.wav", SD_BULLET, 0.6f);
					}
					switch (m_iStatusNum)
					{
					case 1:
						m_iPlayer3Str = m_iPlayer3Str + m_iUp;
						break;
					case 2:
						m_iPlayer3Stamina = m_iPlayer3Stamina + m_iUp;
						break;
					case 3:
						m_iPlayer3Hp = m_iPlayer3Hp + m_iUp;
						UM->Set_StatUp4(true);
						break;
					case 4:
						m_iPlayer3Mental = m_iPlayer3Mental + m_iUp;
						break;
					case 5:
						m_iPlayer3Meta = m_iPlayer3Meta + m_iUp;
						break;
					case 6:
						m_iPlayer3Explosive = m_iPlayer3Explosive + m_iUp;
						break;
					case 7:
						m_iPlayer3Luck = m_iPlayer3Luck + m_iUp;
						break;
					}
					UM->Use_Exp(m_iUp * 1000);
					if (m_iUp > 0)
						UM->Set_LevelUpEffect(true);
					m_iUp = 0;
				}

				if (UM->Get_Exp() >= (m_iUp + 1) * 1000)
				{
					if (m_iStatusNum == 1)
					{
						if (CK_DOWN(DIK_RIGHT))
						{
							GI->PlaySoundW(L"UI_SkillUp.wav", SD_UI, 0.6f);
							if (m_iPlayer3Str + m_iUp < 20)
							{
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer3Str + m_iUp - 1), -195.f, 31.f * 0.82f, 60.f * 0.72f, TEXT("Prototype_GameObject_ReserveBar"));
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer3Str + m_iUp - 1), -195.f, 256.f * 0.15f, 256.f * 0.15f, TEXT("Prototype_GameObject_ReserveEffect"));
								m_iUp++;
							}
						}
					}
					else if (m_iStatusNum == 2)
					{
						if (CK_DOWN(DIK_RIGHT))
						{
							GI->PlaySoundW(L"UI_SkillUp.wav", SD_UI, 0.6f); 
							if (m_iPlayer3Stamina + m_iUp < 20)
							{
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer3Stamina + m_iUp - 1), -129.1f, 31.f * 0.82f, 60.f * 0.72f, TEXT("Prototype_GameObject_ReserveBar"));
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer3Stamina + m_iUp - 1), -129.1f, 256.f * 0.15f, 256.f * 0.15f, TEXT("Prototype_GameObject_ReserveEffect"));
								m_iUp++;
							}
						}
					}
					else if (m_iStatusNum == 3)
					{
						if (CK_DOWN(DIK_RIGHT))
						{
							GI->PlaySoundW(L"UI_SkillUp.wav", SD_UI, 0.6f);
							if (m_iPlayer3Hp + m_iUp < 20)
							{
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer3Hp + m_iUp - 1), -64.1f, 31.f * 0.82f, 60.f * 0.72f, TEXT("Prototype_GameObject_ReserveBar"));
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer3Hp + m_iUp - 1), -64.1f, 256.f * 0.15f, 256.f * 0.15f, TEXT("Prototype_GameObject_ReserveEffect"));
								m_iUp++;
							}
						}
					}
					else if (m_iStatusNum == 4)
					{
						if (CK_DOWN(DIK_RIGHT))
						{
							GI->PlaySoundW(L"UI_SkillUp.wav", SD_UI, 0.6f);
							if (m_iPlayer3Mental + m_iUp < 20)
							{
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer3Mental + m_iUp - 1), 6.88f, 31.f * 0.82f, 60.f * 0.72f, TEXT("Prototype_GameObject_ReserveBar"));
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer3Mental + m_iUp - 1), 6.88f, 256.f * 0.15f, 256.f * 0.15f, TEXT("Prototype_GameObject_ReserveEffect"));
								m_iUp++;
							}
						}
					}
					else if (m_iStatusNum == 5)
					{
						if (CK_DOWN(DIK_RIGHT))
						{
							GI->PlaySoundW(L"UI_SkillUp.wav", SD_UI, 0.6f);
							if (m_iPlayer3Meta + m_iUp < 20)
							{
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer3Meta + m_iUp - 1), 71.91f, 31.f * 0.82f, 60.f * 0.72f, TEXT("Prototype_GameObject_ReserveBar"));
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer3Meta + m_iUp - 1), 71.91f, 256.f * 0.15f, 256.f * 0.15f, TEXT("Prototype_GameObject_ReserveEffect"));
								m_iUp++;
							}
						}
					}
					else if (m_iStatusNum == 6)
					{
						if (CK_DOWN(DIK_RIGHT))
						{
							GI->PlaySoundW(L"UI_SkillUp.wav", SD_UI, 0.6f);
							if (m_iPlayer3Explosive + m_iUp < 20)
							{
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer3Explosive + m_iUp - 1), 136.1f, 31.f * 0.82f, 60.f * 0.72f, TEXT("Prototype_GameObject_ReserveBar"));
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer3Explosive + m_iUp - 1), 136.1f, 256.f * 0.15f, 256.f * 0.15f, TEXT("Prototype_GameObject_ReserveEffect"));
								m_iUp++;
							}
						}
					}
					else if (m_iStatusNum == 7)
					{
						if (CK_DOWN(DIK_RIGHT))
						{
							GI->PlaySoundW(L"UI_SkillUp.wav", SD_UI, 0.6f);
							if (m_iPlayer3Luck + m_iUp < 20)
							{
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer3Luck + m_iUp - 1), 206.2f, 31.f * 0.82f, 60.f * 0.72f, TEXT("Prototype_GameObject_ReserveBar"));
								Make_ChildUI(-74.3f + 14.1f * (m_iPlayer3Luck + m_iUp - 1), 206.2f, 256.f * 0.15f, 256.f * 0.15f, TEXT("Prototype_GameObject_ReserveEffect"));
								m_iUp++;
							}
						}
					}
				}
			}
		}


		UM->Set_Player1Str(m_iPlayer1Str);
		UM->Set_Player1Stamina(m_iPlayer1Stamina);
		UM->Set_Player1Hp(m_iPlayer1Hp);
		UM->Set_Player1CurrentHp(m_iPlayer1Hp * 50 + 500);
		UM->Set_Player1Mental(m_iPlayer1Mental);
		UM->Set_Player1Meta(m_iPlayer1Meta);
		UM->Set_Player1Explosive(m_iPlayer1Explosive);
		UM->Set_Player1Luck(m_iPlayer1Luck);

		UM->Set_Player2Str(m_iPlayer2Str);
		UM->Set_Player2Stamina(m_iPlayer2Stamina);
		UM->Set_Player2Hp(m_iPlayer2Hp);
		UM->Set_Player2CurrentHp(m_iPlayer2Hp * 50 + 500);
		UM->Set_Player2Mental(m_iPlayer2Mental);
		UM->Set_Player2Meta(m_iPlayer2Meta);
		UM->Set_Player2Explosive(m_iPlayer2Explosive);
		UM->Set_Player2Luck(m_iPlayer2Luck);

		UM->Set_Player3Str(m_iPlayer3Str);
		UM->Set_Player3Stamina(m_iPlayer3Stamina);
		UM->Set_Player3Hp(m_iPlayer3Hp);
		UM->Set_Player3CurrentHp(m_iPlayer3Hp * 50 + 500);
		UM->Set_Player3Mental(m_iPlayer3Mental);
		UM->Set_Player3Meta(m_iPlayer3Meta);
		UM->Set_Player3Explosive(m_iPlayer3Explosive);
		UM->Set_Player3Luck(m_iPlayer3Luck);

		STM->Set_Hp(m_iPlayer1Hp, CStatus_Manager::PLAYER01);
		STM->Set_FinalHp(CStatus_Manager::PLAYER01);
		STM->Set_Hp(m_iPlayer2Hp, CStatus_Manager::PLAYER03);
		STM->Set_FinalHp(CStatus_Manager::PLAYER03);
		STM->Set_Hp(m_iPlayer3Hp, CStatus_Manager::PLAYER04);
		STM->Set_FinalHp(CStatus_Manager::PLAYER04);

		STM->Set_Str(m_iPlayer1Str, CStatus_Manager::PLAYER01);
		STM->Set_FinalStr(CStatus_Manager::PLAYER01);
		STM->Set_Str(m_iPlayer2Str, CStatus_Manager::PLAYER03);
		STM->Set_FinalStr(CStatus_Manager::PLAYER03);
		STM->Set_Str(m_iPlayer3Str, CStatus_Manager::PLAYER04);
		STM->Set_FinalStr(CStatus_Manager::PLAYER04);

		STM->Set_Stamina(m_iPlayer1Stamina, CStatus_Manager::PLAYER01);
		STM->Set_FinalStamina(CStatus_Manager::PLAYER01);
		STM->Set_Stamina(m_iPlayer2Stamina, CStatus_Manager::PLAYER03);
		STM->Set_FinalStamina(CStatus_Manager::PLAYER03);
		STM->Set_Stamina(m_iPlayer3Stamina, CStatus_Manager::PLAYER04);
		STM->Set_FinalStamina(CStatus_Manager::PLAYER04);

		STM->Set_Mental(m_iPlayer1Mental, CStatus_Manager::PLAYER01);
		STM->Set_FinalMental(CStatus_Manager::PLAYER01);
		STM->Set_Mental(m_iPlayer2Mental, CStatus_Manager::PLAYER03);
		STM->Set_FinalMental(CStatus_Manager::PLAYER03);
		STM->Set_Mental(m_iPlayer3Mental, CStatus_Manager::PLAYER04);
		STM->Set_FinalMental(CStatus_Manager::PLAYER04);

		STM->Set_Meta(m_iPlayer1Meta, CStatus_Manager::PLAYER01);
		STM->Set_FinalMeta(CStatus_Manager::PLAYER01);
		STM->Set_Meta(m_iPlayer2Meta, CStatus_Manager::PLAYER03);
		STM->Set_FinalMeta(CStatus_Manager::PLAYER03);
		STM->Set_Meta(m_iPlayer3Meta, CStatus_Manager::PLAYER04);
		STM->Set_FinalMeta(CStatus_Manager::PLAYER04);

		STM->Set_Explosive(m_iPlayer1Explosive, CStatus_Manager::PLAYER01);
		STM->Set_FinalExplosive(CStatus_Manager::PLAYER01);
		STM->Set_Explosive(m_iPlayer2Explosive, CStatus_Manager::PLAYER03);
		STM->Set_FinalExplosive(CStatus_Manager::PLAYER03);
		STM->Set_Explosive(m_iPlayer3Explosive, CStatus_Manager::PLAYER04);
		STM->Set_FinalExplosive(CStatus_Manager::PLAYER04);

		STM->Set_Luck(m_iPlayer1Luck, CStatus_Manager::PLAYER01);
		STM->Get_FinalCritical(CStatus_Manager::PLAYER01);
		STM->Set_Luck(m_iPlayer2Luck, CStatus_Manager::PLAYER03);
		STM->Get_FinalCritical(CStatus_Manager::PLAYER03);
		STM->Set_Luck(m_iPlayer3Luck, CStatus_Manager::PLAYER04);
		STM->Get_FinalCritical(CStatus_Manager::PLAYER04);




		UM->Set_StatusNum(m_iStatusNum);
		UM->Set_Reserve(m_iUp);

		m_pNumReserveExp->Set_Number(m_iUp * 1000);

		//m_pNumHp->Set_Number(500 + (m_iPlayer1Hp + m_iUp) * 50);
		//m_pNumMental->Set_Number(m_iPlayer1Mental + m_iUp);
		//m_pNumMeta->Set_Number(m_iPlayer1Meta + m_iUp);
		//m_pNumStr->Set_Number(m_iPlayer1Str + m_iUp);
		//m_pNumStamina->Set_Number(m_iPlayer1Stamina + m_iUp);
		//m_pNumExplosive->Set_Number(m_iPlayer1Explosive + m_iUp);
		//m_pNumLuck->Set_Number(m_iPlayer1Luck + m_iUp);

		if (m_iPlayerNum == 1)
		{
			m_pNumLevel->Set_Number(m_iUp + m_iPlayer1Str + m_iPlayer1Stamina + m_iPlayer1Hp + m_iPlayer1Mental + m_iPlayer1Meta + m_iPlayer1Explosive + m_iPlayer1Luck - 6);
			if (m_iStatusNum == 1)
			{
				m_pNumHp->Set_Number(500 + (m_iPlayer1Hp) * 50);
				m_pNumMental->Set_Number(m_iPlayer1Mental);
				m_pNumMeta->Set_Number(m_iPlayer1Meta);
				m_pNumStr->Set_Number(m_iPlayer1Str + m_iUp);
				m_pNumStamina->Set_Number(m_iPlayer1Stamina);
				m_pNumExplosive->Set_Number(m_iPlayer1Explosive);
				m_pNumLuck->Set_Number(m_iPlayer1Luck);
			}
			else if (m_iStatusNum == 2)
			{
				m_pNumHp->Set_Number(500 + (m_iPlayer1Hp) * 50);
				m_pNumMental->Set_Number(m_iPlayer1Mental);
				m_pNumMeta->Set_Number(m_iPlayer1Meta);
				m_pNumStr->Set_Number(m_iPlayer1Str);
				m_pNumStamina->Set_Number(m_iPlayer1Stamina + m_iUp);
				m_pNumExplosive->Set_Number(m_iPlayer1Explosive);
				m_pNumLuck->Set_Number(m_iPlayer1Luck);
			}
			else if (m_iStatusNum == 3)
			{
				m_pNumHp->Set_Number(500 + (m_iPlayer1Hp + m_iUp) * 50);
				m_pNumMental->Set_Number(m_iPlayer1Mental);
				m_pNumMeta->Set_Number(m_iPlayer1Meta);
				m_pNumStr->Set_Number(m_iPlayer1Str);
				m_pNumStamina->Set_Number(m_iPlayer1Stamina);
				m_pNumExplosive->Set_Number(m_iPlayer1Explosive);
				m_pNumLuck->Set_Number(m_iPlayer1Luck);
			}
			else if (m_iStatusNum == 4)
			{
				m_pNumHp->Set_Number(500 + (m_iPlayer1Hp) * 50);
				m_pNumMental->Set_Number(m_iPlayer1Mental + m_iUp);
				m_pNumMeta->Set_Number(m_iPlayer1Meta);
				m_pNumStr->Set_Number(m_iPlayer1Str);
				m_pNumStamina->Set_Number(m_iPlayer1Stamina);
				m_pNumExplosive->Set_Number(m_iPlayer1Explosive);
				m_pNumLuck->Set_Number(m_iPlayer1Luck);
			}
			else if (m_iStatusNum == 5)
			{
				m_pNumHp->Set_Number(500 + (m_iPlayer1Hp) * 50);
				m_pNumMental->Set_Number(m_iPlayer1Mental);
				m_pNumMeta->Set_Number(m_iPlayer1Meta + m_iUp);
				m_pNumStr->Set_Number(m_iPlayer1Str);
				m_pNumStamina->Set_Number(m_iPlayer1Stamina);
				m_pNumExplosive->Set_Number(m_iPlayer1Explosive);
				m_pNumLuck->Set_Number(m_iPlayer1Luck);
			}
			else if (m_iStatusNum == 6)
			{
				m_pNumHp->Set_Number(500 + (m_iPlayer1Hp) * 50);
				m_pNumMental->Set_Number(m_iPlayer1Mental);
				m_pNumMeta->Set_Number(m_iPlayer1Meta);
				m_pNumStr->Set_Number(m_iPlayer1Str);
				m_pNumStamina->Set_Number(m_iPlayer1Stamina);
				m_pNumExplosive->Set_Number(m_iPlayer1Explosive + m_iUp);
				m_pNumLuck->Set_Number(m_iPlayer1Luck);
			}
			else if (m_iStatusNum == 7)
			{
				m_pNumHp->Set_Number(500 + (m_iPlayer1Hp) * 50);
				m_pNumMental->Set_Number(m_iPlayer1Mental);
				m_pNumMeta->Set_Number(m_iPlayer1Meta);
				m_pNumStr->Set_Number(m_iPlayer1Str);
				m_pNumStamina->Set_Number(m_iPlayer1Stamina);
				m_pNumExplosive->Set_Number(m_iPlayer1Explosive);
				m_pNumLuck->Set_Number(m_iPlayer1Luck + m_iUp);
			}
		}
		else if (m_iPlayerNum == 2)
		{
			m_pNumLevel->Set_Number(m_iUp + m_iPlayer2Str + m_iPlayer2Stamina + m_iPlayer2Hp + m_iPlayer2Mental + m_iPlayer2Meta + m_iPlayer2Explosive + m_iPlayer2Luck - 6);
			if (m_iStatusNum == 1)
			{
				m_pNumHp->Set_Number(500 + (m_iPlayer2Hp) * 50);
				m_pNumMental->Set_Number(m_iPlayer2Mental);
				m_pNumMeta->Set_Number(m_iPlayer2Meta);
				m_pNumStr->Set_Number(m_iPlayer2Str + m_iUp);
				m_pNumStamina->Set_Number(m_iPlayer2Stamina);
				m_pNumExplosive->Set_Number(m_iPlayer2Explosive);
				m_pNumLuck->Set_Number(m_iPlayer2Luck);
			}
			else if (m_iStatusNum == 2)
			{
				m_pNumHp->Set_Number(500 + (m_iPlayer2Hp) * 50);
				m_pNumMental->Set_Number(m_iPlayer2Mental);
				m_pNumMeta->Set_Number(m_iPlayer2Meta);
				m_pNumStr->Set_Number(m_iPlayer2Str);
				m_pNumStamina->Set_Number(m_iPlayer2Stamina + m_iUp);
				m_pNumExplosive->Set_Number(m_iPlayer2Explosive);
				m_pNumLuck->Set_Number(m_iPlayer2Luck);
			}
			else if (m_iStatusNum == 3)
			{
				m_pNumHp->Set_Number(500 + (m_iPlayer2Hp + m_iUp) * 50);
				m_pNumMental->Set_Number(m_iPlayer2Mental);
				m_pNumMeta->Set_Number(m_iPlayer2Meta);
				m_pNumStr->Set_Number(m_iPlayer2Str);
				m_pNumStamina->Set_Number(m_iPlayer2Stamina);
				m_pNumExplosive->Set_Number(m_iPlayer2Explosive);
				m_pNumLuck->Set_Number(m_iPlayer2Luck);
			}
			else if (m_iStatusNum == 4)
			{
				m_pNumHp->Set_Number(500 + (m_iPlayer2Hp) * 50);
				m_pNumMental->Set_Number(m_iPlayer2Mental + m_iUp);
				m_pNumMeta->Set_Number(m_iPlayer2Meta);
				m_pNumStr->Set_Number(m_iPlayer2Str);
				m_pNumStamina->Set_Number(m_iPlayer2Stamina);
				m_pNumExplosive->Set_Number(m_iPlayer2Explosive);
				m_pNumLuck->Set_Number(m_iPlayer2Luck);
			}
			else if (m_iStatusNum == 5)
			{
				m_pNumHp->Set_Number(500 + (m_iPlayer2Hp) * 50);
				m_pNumMental->Set_Number(m_iPlayer2Mental);
				m_pNumMeta->Set_Number(m_iPlayer2Meta + m_iUp);
				m_pNumStr->Set_Number(m_iPlayer2Str);
				m_pNumStamina->Set_Number(m_iPlayer2Stamina);
				m_pNumExplosive->Set_Number(m_iPlayer2Explosive);
				m_pNumLuck->Set_Number(m_iPlayer2Luck);
			}
			else if (m_iStatusNum == 6)
			{
				m_pNumHp->Set_Number(500 + (m_iPlayer2Hp) * 50);
				m_pNumMental->Set_Number(m_iPlayer2Mental);
				m_pNumMeta->Set_Number(m_iPlayer2Meta);
				m_pNumStr->Set_Number(m_iPlayer2Str);
				m_pNumStamina->Set_Number(m_iPlayer2Stamina);
				m_pNumExplosive->Set_Number(m_iPlayer2Explosive + m_iUp);
				m_pNumLuck->Set_Number(m_iPlayer2Luck);
			}
			else if (m_iStatusNum == 7)
			{
				m_pNumHp->Set_Number(500 + (m_iPlayer2Hp) * 50);
				m_pNumMental->Set_Number(m_iPlayer2Mental);
				m_pNumMeta->Set_Number(m_iPlayer2Meta);
				m_pNumStr->Set_Number(m_iPlayer2Str);
				m_pNumStamina->Set_Number(m_iPlayer2Stamina);
				m_pNumExplosive->Set_Number(m_iPlayer2Explosive);
				m_pNumLuck->Set_Number(m_iPlayer2Luck + m_iUp);
			}
		}
		else if (m_iPlayerNum == 3)
		{
			m_pNumLevel->Set_Number(m_iUp + m_iPlayer3Str + m_iPlayer3Stamina + m_iPlayer3Hp + m_iPlayer3Mental + m_iPlayer3Meta + m_iPlayer3Explosive + m_iPlayer3Luck - 6);
			if (m_iStatusNum == 1)
			{
				m_pNumHp->Set_Number(500 + (m_iPlayer3Hp) * 50);
				m_pNumMental->Set_Number(m_iPlayer3Mental);
				m_pNumMeta->Set_Number(m_iPlayer3Meta);
				m_pNumStr->Set_Number(m_iPlayer3Str + m_iUp);
				m_pNumStamina->Set_Number(m_iPlayer3Stamina);
				m_pNumExplosive->Set_Number(m_iPlayer3Explosive);
				m_pNumLuck->Set_Number(m_iPlayer3Luck);
			}
			else if (m_iStatusNum == 2)
			{
				m_pNumHp->Set_Number(500 + (m_iPlayer3Hp) * 50);
				m_pNumMental->Set_Number(m_iPlayer3Mental);
				m_pNumMeta->Set_Number(m_iPlayer3Meta);
				m_pNumStr->Set_Number(m_iPlayer3Str);
				m_pNumStamina->Set_Number(m_iPlayer3Stamina + m_iUp);
				m_pNumExplosive->Set_Number(m_iPlayer3Explosive);
				m_pNumLuck->Set_Number(m_iPlayer3Luck);
			}
			else if (m_iStatusNum == 3)
			{
				m_pNumHp->Set_Number(500 + (m_iPlayer3Hp + m_iUp) * 50);
				m_pNumMental->Set_Number(m_iPlayer3Mental);
				m_pNumMeta->Set_Number(m_iPlayer3Meta);
				m_pNumStr->Set_Number(m_iPlayer3Str);
				m_pNumStamina->Set_Number(m_iPlayer3Stamina);
				m_pNumExplosive->Set_Number(m_iPlayer3Explosive);
				m_pNumLuck->Set_Number(m_iPlayer3Luck);
			}
			else if (m_iStatusNum == 4)
			{
				m_pNumHp->Set_Number(500 + (m_iPlayer3Hp) * 50);
				m_pNumMental->Set_Number(m_iPlayer3Mental + m_iUp);
				m_pNumMeta->Set_Number(m_iPlayer3Meta);
				m_pNumStr->Set_Number(m_iPlayer3Str);
				m_pNumStamina->Set_Number(m_iPlayer3Stamina);
				m_pNumExplosive->Set_Number(m_iPlayer3Explosive);
				m_pNumLuck->Set_Number(m_iPlayer3Luck);
			}
			else if (m_iStatusNum == 5)
			{
				m_pNumHp->Set_Number(500 + (m_iPlayer3Hp) * 50);
				m_pNumMental->Set_Number(m_iPlayer3Mental);
				m_pNumMeta->Set_Number(m_iPlayer3Meta + m_iUp);
				m_pNumStr->Set_Number(m_iPlayer3Str);
				m_pNumStamina->Set_Number(m_iPlayer3Stamina);
				m_pNumExplosive->Set_Number(m_iPlayer3Explosive);
				m_pNumLuck->Set_Number(m_iPlayer3Luck);
			}
			else if (m_iStatusNum == 6)
			{
				m_pNumHp->Set_Number(500 + (m_iPlayer3Hp) * 50);
				m_pNumMental->Set_Number(m_iPlayer3Mental);
				m_pNumMeta->Set_Number(m_iPlayer3Meta);
				m_pNumStr->Set_Number(m_iPlayer3Str);
				m_pNumStamina->Set_Number(m_iPlayer3Stamina);
				m_pNumExplosive->Set_Number(m_iPlayer3Explosive + m_iUp);
				m_pNumLuck->Set_Number(m_iPlayer3Luck);
			}
			else if (m_iStatusNum == 7)
			{
				m_pNumHp->Set_Number(500 + (m_iPlayer3Hp) * 50);
				m_pNumMental->Set_Number(m_iPlayer3Mental);
				m_pNumMeta->Set_Number(m_iPlayer3Meta);
				m_pNumStr->Set_Number(m_iPlayer3Str);
				m_pNumStamina->Set_Number(m_iPlayer3Stamina);
				m_pNumExplosive->Set_Number(m_iPlayer3Explosive);
				m_pNumLuck->Set_Number(m_iPlayer3Luck + m_iUp);
			}
		}

		UM->Set_Player1Level((m_iPlayer1Str + m_iPlayer1Stamina + m_iPlayer1Hp + m_iPlayer1Mental + m_iPlayer1Meta + m_iPlayer1Explosive + m_iPlayer1Luck - 6));
		UM->Set_Player2Level((m_iPlayer2Str + m_iPlayer2Stamina + m_iPlayer2Hp + m_iPlayer2Mental + m_iPlayer2Meta + m_iPlayer2Explosive + m_iPlayer2Luck - 6));
		UM->Set_Player3Level((m_iPlayer3Str + m_iPlayer3Stamina + m_iPlayer3Hp + m_iPlayer3Mental + m_iPlayer3Meta + m_iPlayer3Explosive + m_iPlayer3Luck - 6));
	}
}

void CStatusBase::LateTick(_float fTimeDelta)
{
	if (m_bRender)
		__super::LateTick(fTimeDelta);



	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_UI, this);
}

HRESULT CStatusBase::Render()
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

	if(m_bRender)
		if (FAILED(m_pVIBufferCom->Render()))
			return E_FAIL;

	//UI_RenderDebug();


	// 자식들의 Render 호출
	if (m_bRender)
		__super::Render();


		


	return S_OK;
}






HRESULT CStatusBase::Ready_Components()
{

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuBackGround"), TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_UI"), TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}







void CStatusBase::Render_Debug()
{
	ImGui::Begin(m_sImGuiTag.data());


	if (ImGui::Button("Debug"))
		m_bDebug = !m_bDebug;

	ImGui::End();
}



CStatusBase * CStatusBase::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CStatusBase*		pInstance = new CStatusBase(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Created : CStatusBase"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CStatusBase::Clone(void * pArg)
{
	CStatusBase*		pInstance = new CStatusBase(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Cloned : CStatusBase"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



void CStatusBase::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pShaderCom);
}
