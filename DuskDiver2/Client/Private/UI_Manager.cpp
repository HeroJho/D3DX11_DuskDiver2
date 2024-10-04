#include "stdafx.h"
#include "..\Public\UI_Manager.h"
#include "GameInstance.h"
#include "DialogTalk.h"
#include "ActiveQuest.h"
#include "BattleDialog.h"
#include "Clue1.h"
#include "Clue2.h"
#include "Blind.h"

IMPLEMENT_SINGLETON(CUI_Manager)

CUI_Manager::CUI_Manager()
{
	m_iMenuNum = 0;
	m_iBackNum = 0;
	m_iLevelUpSelectPageNum = 1;
	m_iStatusNum = 1;
	m_iPlayer1Str = 1;
	m_iPlayer1Stamina = 1;
	m_iPlayer1Hp = 1;
	m_iPlayer1Mental = 1;
	m_iPlayer1Meta = 1;
	m_iPlayer1Explosive = 1;
	m_iPlayer1Luck = 1;
	m_iReserve = 0;
	m_iPlayerSelectNum = 1;

	for (_uint i = 0; i < 10; ++i)
	{
		m_iHat[i] = 6;
		m_iClothes[i] = 6;
		m_iDeco[i] = 6;

		m_iDropItem[i] = 10;
	}
	for(_uint i = 0; i < 7; ++i)
	{
		m_iBattleItem[i] = 1;
		m_iFood[i] = 1;
	}


	//아이템 갯수 수정
	m_iBattleItem[0] = 53;
	m_iBattleItem[2] = 90;
	m_iBattleItem[4] = 85;
	m_iBattleItem[6] = 83;


	for (_uint i = 0; i < 15; ++i)
	{
		m_iEquipItem[i] = 0;
	}

	for (_uint i = 0; i < 12; ++i)
	{
		m_bBear[i] = false;
	}

	m_iBattleItemNum[0] = 0;
	m_iBattleItemNum[1] = 2;
	m_iBattleItemNum[2] = 4;
	m_iBattleItemNum[3] = 6;

	m_QuestPos = { 11.5f, 1.4f, 25.3f };
}


void CUI_Manager::Free()
{
}


void CUI_Manager::BackUp_Blind(CBlind * pUI)
{
	m_pBlind = pUI;
}

void CUI_Manager::MoveBlind(_bool b)
{
	m_pBlind->Move_Blind(b);
}

_bool CUI_Manager::Get_MoveOver()
{
	return m_pBlind->Get_MoveOver();
}

void CUI_Manager::Gain_Hat(_uint iNum)
{
	for (int i = 8; i >= 0; --i)
	{
		if (m_iHat[i] != 6)
		{
			m_iHat[1 + i] = m_iHat[i];
		}
	}
	m_iHat[0] = iNum;
	switch (iNum)
	{
	case 0:
		m_iEquipItem[0] = 1;
		break;
	case 1:
		m_iEquipItem[1] = 1;
		break;
	case 2:
		m_iEquipItem[2] = 1;
		break;
	case 3:
		m_iEquipItem[3] = 1;
		break;
	case 4:
		m_iEquipItem[4] = 1;
		break;
	}
}

void CUI_Manager::Gain_Clothes(_uint iNum)
{
	for (int i = 8; i >= 0; --i)
	{
		if (m_iClothes[i] != 6)
		{
			m_iClothes[1 + i] = m_iClothes[i];
		}
	}
	m_iClothes[0] = iNum;

	switch (iNum)
	{
	case 0:
		m_iEquipItem[5] = 1;
		break;
	case 1:
		m_iEquipItem[6] = 1;
		break;
	case 2:
		m_iEquipItem[7] = 1;
		break;
	case 3:
		m_iEquipItem[8] = 1;
		break;
	case 4:
		m_iEquipItem[9] = 1;
		break;
	}
}

void CUI_Manager::Gain_Deco(_uint iNum)
{
	for (int i = 8; i >= 0; --i)
	{
		if (m_iDeco[i] != 6)
		{
			m_iDeco[1 + i] = m_iDeco[i];
		}
	}
	m_iDeco[0] = iNum;

	switch (iNum)
	{
	case 0:
		m_iEquipItem[10] = 1;
		break;
	case 1:
		m_iEquipItem[11] = 1;
		break;
	case 2:
		m_iEquipItem[12] = 1;
		break;
	case 3:
		m_iEquipItem[13] = 1;
		break;
	case 4:
		m_iEquipItem[14] = 1;
		break;
	}
}

void CUI_Manager::Gain_DropItem(_uint iNum)
{
	m_iDropItem[iNum]++;
}

_uint CUI_Manager::Get_InvenEquipZero(_uint iNum)
{
	_uint iZeroNum = 0;

	for (_uint i = 0; i < iNum; ++i)
	{
		if (m_iEquipItem[i] == 0)
			iZeroNum++;
	}
	return iZeroNum;
}

_uint CUI_Manager::Get_FoodZero(_uint iNum)
{
	_uint iZeroNum = 0;

	for (_uint i = 0; i < iNum; ++i)
	{
		if (m_iFood[i] == 0)
			iZeroNum++;
	}
	return iZeroNum;
}

void CUI_Manager::Make_BattleEnd()
{
	CUI::UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));

	UiInfoDesc.fSizeX = 320.f;
	UiInfoDesc.fSizeY = 0.01f;
	UiInfoDesc.fX = 200.f;
	UiInfoDesc.fY = 160.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_BattleEndBase"), GI->Get_Cur_LevelIndex(), TEXT("Layer_UI"), &UiInfoDesc)))
		return;

	UiInfoDesc.fSizeX = 216.4f;
	UiInfoDesc.fSizeY = 58.4f;
	UiInfoDesc.fX = 0.f;
	UiInfoDesc.fY = 170.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_BattleEnd"), GI->Get_Cur_LevelIndex(), TEXT("Layer_UI"), &UiInfoDesc)))
		return;
}

void CUI_Manager::Make_Ink()
{
	CUI::UIINFODESC UiInfoDesc;
	ZeroMemory(&UiInfoDesc, sizeof(CUI::UIINFODESC));

	UiInfoDesc.fSizeX = 1024.f;
	UiInfoDesc.fSizeY = 720.f;
	UiInfoDesc.fX = 640.f;
	UiInfoDesc.fY = 360.f;
	UiInfoDesc.pDesc = nullptr;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Prototype_GameObject_Ink"), GI->Get_Cur_LevelIndex(), TEXT("Layer_UI"), &UiInfoDesc)))
		return;
}

_float3 CUI_Manager::Get_QuestPos()
{
	if (m_bGetPos)
	{
		if (m_pActiveQuest)
		{
			return m_pActiveQuest->Get_QuestPoint();
		}
		else
			return{ 11.5f, 1.4f, 25.3f };
	}
	else
		return { 11.5f, 1.4f, 25.3f };
}

_uint CUI_Manager::Get_ZeroNum(_uint iNum)
{
	int iZeroNum = 0;

	for (_uint i = 0; i < iNum; ++i)
	{
		if (m_iBattleItem[i] == 0)
			iZeroNum++;
	}
	return iZeroNum;
}

HRESULT CUI_Manager::BackUp_DialogTalk(class CDialogTalk* pUI)
{
	m_pDIalogTalk = pUI;

	return S_OK;
}

HRESULT CUI_Manager::BackUp_ActiveQuest(CActiveQuest * pUI)
{
	m_pActiveQuest = pUI;

	return S_OK;
}

HRESULT CUI_Manager::BackUp_BattleDialog(CBattleDialog * pUI)
{
	m_pBattleDialog = pUI;

	return S_OK;
}

const _bool CUI_Manager::Get_TalkOnOff()
{
	return m_pDIalogTalk->Get_TalkOnOff();
}

const _uint & CUI_Manager::Get_TalkIndex()
{
	return m_pDIalogTalk->Get_TalkIndex();
}

void CUI_Manager::Set_TalkIndex(_uint Index)
{
	if (m_pDIalogTalk)
	{
		m_pDIalogTalk->Set_TalkIndex(Index);
		Set_FrontUI(false);
		GI->PlaySoundW(L"UI_Confirm.wav", SD_UI, 0.6f);
	}
}

void CUI_Manager::Set_QuestPoint(_float3 vPos, _uint iQuestIconIndex)
{
	if (m_pActiveQuest)
	{
		m_pActiveQuest->Set_QuestPoint(vPos, iQuestIconIndex);
		m_QuestPos = vPos;
	}
	m_bGetPos = true;
}

void CUI_Manager::Set_QuestPointOff()
{
	if (m_pActiveQuest)
		m_pActiveQuest->Set_QuestPointOff();
}

_bool CUI_Manager::Get_QuestPointRenderState()
{
	return m_pActiveQuest->Get_RenderState();
}

_bool CUI_Manager::Get_BattleTalkOnOff()
{
	return m_pBattleDialog->Get_BattleTalkOnOff();
}

_uint CUI_Manager::Get_BattleTalkIndex()
{
	return m_pBattleDialog->Get_BattleTalkIndex();
}

void CUI_Manager::Set_BattleTalkIndex(_uint Index)
{
	m_pBattleDialog->Set_BattleTalkIndex(Index);
}

void CUI_Manager::Set_BattleOffTalk()
{
	m_pBattleDialog->Set_BattleOffTalk();
}

HRESULT CUI_Manager::BackUp_Clue1(CClue1 * pUI)
{
	m_pClue1 = pUI;

	return S_OK;
}

HRESULT CUI_Manager::BackUp_Clue2(CClue2 * pUI)
{
	m_pClue2 = pUI;

	return S_OK;
}

void CUI_Manager::Render_Clue1()
{
	if (nullptr == m_pClue1)
		return;

	m_pClue1->Set_Clue1(true);
	Set_FrontUI(false);
}

void CUI_Manager::Render_Clue2()
{
	if (nullptr == m_pClue2)
		return;

	m_pClue2->Set_Clue2(true);
	Set_FrontUI(false);
}


