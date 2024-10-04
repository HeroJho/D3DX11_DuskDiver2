#pragma once

#include "Client_Defines.h"
#include "Base.h"


BEGIN(Client)

class CUI_Manager final : CBase
{
	DECLARE_SINGLETON(CUI_Manager)

public:
	CUI_Manager();
	virtual ~CUI_Manager() = default;

public:
	virtual void Free() override;

public:
	void Set_MenuNum(_uint iNum) { m_iMenuNum = iNum; }
	_uint Get_MenuNum() { return m_iMenuNum; }

	// ���� ��׶��� UI�� � ������ �޴�����
	void Set_BackNum(_uint iNum) { m_iBackNum = iNum; }
	_uint Get_BackNum() { return m_iBackNum; }

	void Set_LevelUpSelectPageNum(_uint iNum) { m_iLevelUpSelectPageNum = iNum; }
	_uint Get_LevelUpSelectPageNum() { return m_iLevelUpSelectPageNum; }

	void Set_StatusNum(_uint iNum) { m_iStatusNum = iNum; }
	_uint Get_StatusNum() { return m_iStatusNum; }

	// ������ ���������� ���õǾ��ִ� �÷��̾�
	void Set_PlayerSelectNum(_uint iNum) { m_iPlayerSelectNum = iNum; }
	_uint Get_PlayerSelectNum() { return m_iPlayerSelectNum; }

	// ���â���� ���õǾ��ִ� �÷��̾�
	void Set_EquipPlayerSelectNum(_uint iNum) { m_iEquipPlayerSelectNum = iNum; }
	_uint Get_EquipPlayerSelectNum() { return m_iEquipPlayerSelectNum; }

	void Set_EquipSelectPageNum(_uint iNum) { m_iEquipSelectPageNum = iNum; }
	_uint Get_EquipSelectPageNum() { return m_iEquipSelectPageNum; }

	//Player1 ���� getset
	void Set_Player1Str(_uint iNum) { m_iPlayer1Str = iNum; }
	_uint Get_Player1Str() { return m_iPlayer1Str; }

	void Set_Player1Stamina(_uint iNum) { m_iPlayer1Stamina = iNum; }
	_uint Get_Player1Stamina() { return m_iPlayer1Stamina; }
	 
	void Set_Player1Hp(_uint iNum) { m_iPlayer1Hp = iNum; }
	_uint Get_Player1Hp() { return m_iPlayer1Hp; }

	void Set_Player1Mental(_uint iNum) { m_iPlayer1Mental = iNum; }
	_uint Get_Player1Mental() { return m_iPlayer1Mental; }

	void Set_Player1Meta(_uint iNum) { m_iPlayer1Meta = iNum; }
	_uint Get_Player1Meta() { return m_iPlayer1Meta; }

	void Set_Player1Explosive(_uint iNum) { m_iPlayer1Explosive = iNum; }
	_uint Get_Player1Explosive() { return m_iPlayer1Explosive; }

	void Set_Player1Luck(_uint iNum) { m_iPlayer1Luck = iNum; }
	_uint Get_Player1Luck() { return m_iPlayer1Luck; }

	//Player2 ����'
	void Set_Player2Str(_uint iNum) { m_iPlayer2Str = iNum; }
	_uint Get_Player2Str() { return m_iPlayer2Str; }

	void Set_Player2Stamina(_uint iNum) { m_iPlayer2Stamina = iNum; }
	_uint Get_Player2Stamina() { return m_iPlayer2Stamina; }

	void Set_Player2Hp(_uint iNum) { m_iPlayer2Hp = iNum; }
	_uint Get_Player2Hp() { return m_iPlayer2Hp; }

	void Set_Player2Mental(_uint iNum) { m_iPlayer2Mental = iNum; }
	_uint Get_Player2Mental() { return m_iPlayer2Mental; }

	void Set_Player2Meta(_uint iNum) { m_iPlayer2Meta = iNum; }
	_uint Get_Player2Meta() { return m_iPlayer2Meta; }

	void Set_Player2Explosive(_uint iNum) { m_iPlayer2Explosive = iNum; }
	_uint Get_Player2Explosive() { return m_iPlayer2Explosive; }

	void Set_Player2Luck(_uint iNum) { m_iPlayer2Luck = iNum; }
	_uint Get_Player2Luck() { return m_iPlayer2Luck; }

	//Player3 ����'
	void Set_Player3Str(_uint iNum) { m_iPlayer3Str = iNum; }
	_uint Get_Player3Str() { return m_iPlayer3Str; }

	void Set_Player3Stamina(_uint iNum) { m_iPlayer3Stamina = iNum; }
	_uint Get_Player3Stamina() { return m_iPlayer3Stamina; }

	void Set_Player3Hp(_uint iNum) { m_iPlayer3Hp = iNum; }
	_uint Get_Player3Hp() { return m_iPlayer3Hp; }

	void Set_Player3Mental(_uint iNum) { m_iPlayer3Mental = iNum; }
	_uint Get_Player3Mental() { return m_iPlayer3Mental; }

	void Set_Player3Meta(_uint iNum) { m_iPlayer3Meta = iNum; }
	_uint Get_Player3Meta() { return m_iPlayer3Meta; }

	void Set_Player3Explosive(_uint iNum) { m_iPlayer3Explosive = iNum; }
	_uint Get_Player3Explosive() { return m_iPlayer3Explosive; }

	void Set_Player3Luck(_uint iNum) { m_iPlayer3Luck = iNum; }
	_uint Get_Player3Luck() { return m_iPlayer3Luck; }

	void Set_Reserve(_uint iNum) { m_iReserve = iNum; }
	_uint Get_Reserver() { return m_iReserve; }


	void Set_PictureBurst(_bool iNum) { m_bPictureBurst = iNum; }
	_uint Get_PictureBurst() { return m_bPictureBurst; }


	// ����ġ �߰�, ���, Get�Լ�
	_uint Get_Exp() { return m_iExp; }
	void Add_Exp(_uint iExp) { m_iExp += iExp; }
	void Use_Exp(_uint iExp) { m_iExp -= iExp; }
	
	// Combo
	void Set_Combo(_uint iNum) { m_iCombo = iNum; }
	_uint Get_Combo() { return m_iCombo; }

	void Plus_Combo() { m_iCombo += 1; m_bMoreCombo = true; 
	if (m_iMaxCombo < m_iCombo)
	{
		m_iMaxCombo = m_iCombo;
	}
	}
	void Reset_Combo() { m_iCombo = 0; m_bMoreCombo = false; }

	void Set_MoreCombo(_bool bMoreCombo) { m_bMoreCombo = bMoreCombo; }
	_bool Get_MoreCombo() { return m_bMoreCombo; }

	void Reset_MaxCombo() { m_iMaxCombo = 0; }
	_uint Get_MaxCombo() { return m_iMaxCombo; }

	void Set_Hit(_bool bHit) { m_bHit = bHit; }
	_bool Get_Hit() { return m_bHit; }

	void Set_Up(_bool bUp) { m_bUp = bUp; }
	_bool Get_Up() { return m_bUp; }

	void Set_StageNum(_uint Num) { m_iStageNum = Num; }
	_uint Get_StageNum() { return m_iStageNum; }

	//Coin
	void Set_Coin(_uint iNum) { m_iCoin = iNum; }
	_uint Get_Coin() { return m_iCoin; }

	void Plus_Coin() { m_iCoin += 1;}

	void Set_CoinUp(_bool bCoinUp) { m_bCoinUp = bCoinUp; }
	_bool Get_CoinUp() { return m_bCoinUp; }

	void Set_CoinOn(_bool bCoinOn) { m_bCoinOn = bCoinOn; }
	_bool Get_CoinOn() { return m_bCoinOn; }

	void Set_Player1Level(_uint Num) { m_iPlayer1Level = Num; }
	_uint Get_Player1Level() { return m_iPlayer1Level; }

	void Set_Player2Level(_uint Num) { m_iPlayer2Level = Num; }
	_uint Get_Player2Level() { return m_iPlayer2Level; }

	void Set_Player3Level(_uint Num) { m_iPlayer3Level = Num; }
	_uint Get_Player3Level() { return m_iPlayer3Level; }


	void Set_Player1CurrentHp(_uint iNum) { m_iPlayer1CurrentHp = iNum; }
	_uint Get_Player1CurrentHp() { return m_iPlayer1CurrentHp; }
	void Set_Player2CurrentHp(_uint iNum) { m_iPlayer2CurrentHp = iNum; }
	_uint Get_Player2CurrentHp() { return m_iPlayer2CurrentHp; }
	void Set_Player3CurrentHp(_uint iNum) { m_iPlayer3CurrentHp = iNum; }
	_uint Get_Player3CurrentHp() { return m_iPlayer3CurrentHp; }



	// ��� ����
	void Set_EquipmentClassSelect(_uint iNum) { m_iEquipmentClassSelect = iNum; }
	_uint Get_EquipmentClassSelect() { return m_iEquipmentClassSelect; }

	void Set_EquipmentState(_uint iNum) { m_iEquipmentState = iNum; }
	_uint Get_EquipmentState() { return m_iEquipmentState; }

	void Set_EquipmentNum(_uint iNum) { m_iEquipmentNum = iNum; }
	_uint Get_EquipmentNum() { return m_iEquipmentNum; }

	// ��� ȹ��
	void Gain_Hat(_uint iNum);
	_uint Get_Hat(_uint iNum) { return m_iHat[iNum]; }

	void Gain_Clothes(_uint iNum);
	_uint Get_Clothes(_uint iNum) { return m_iClothes[iNum]; }

	void Gain_Deco(_uint iNum);
	_uint Get_Deco(_uint iNum) { return m_iDeco[iNum]; }

	// ��� ������ ȹ��
	void Gain_DropItem(_uint iNum);
	void Sell_DropItem(_uint iIndex, _uint iNum) { m_iDropItem[iIndex] -= iNum; }
	_uint Get_DropItem(_uint iNum) { return m_iDropItem[iNum]; }

	// ��� ȹ���ߴ��� Ȯ��
	_uint Get_InvenEquip(_uint iNum) { return m_iEquipItem[iNum]; }
	_uint Get_InvenEquipZero(_uint iNum);



	// �÷��̾ ������ ��� e
	void Set_Player1SelectEHat(_uint iNum) { m_iPlayer1SelectEHat = iNum; }
	_uint Get_Player1SelectEHat() { return m_iPlayer1SelectEHat; }

	void Set_Player1SelectEClothes(_uint iNum) { m_iPlayer1SelectEClothes = iNum; }
	_uint Get_Player1SelectEClothes() { return m_iPlayer1SelectEClothes; }

	void Set_Player1SelectEDeco(_uint iNum) { m_iPlayer1SelectEDeco = iNum; }
	_uint Get_Player1SelectEDeco() { return m_iPlayer1SelectEDeco; }

	void Set_Player2SelectEHat(_uint iNum) { m_iPlayer2SelectEHat = iNum; }
	_uint Get_Player2SelectEHat() { return m_iPlayer2SelectEHat; }

	void Set_Player2SelectEClothes(_uint iNum) { m_iPlayer2SelectEClothes = iNum; }
	_uint Get_Player2SelectEClothes() { return m_iPlayer2SelectEClothes; }

	void Set_Player2SelectEDeco(_uint iNum) { m_iPlayer2SelectEDeco = iNum; }
	_uint Get_Player2SelectEDeco() { return m_iPlayer2SelectEDeco; }

	void Set_Player3SelectEHat(_uint iNum) { m_iPlayer3SelectEHat = iNum; }
	_uint Get_Player3SelectEHat() { return m_iPlayer3SelectEHat; }

	void Set_Player3SelectEClothes(_uint iNum) { m_iPlayer3SelectEClothes = iNum; }
	_uint Get_Player3SelectEClothes() { return m_iPlayer3SelectEClothes; }

	void Set_Player3SelectEDeco(_uint iNum) { m_iPlayer3SelectEDeco = iNum; }
	_uint Get_Player3SelectEDeco() { return m_iPlayer3SelectEDeco; }

	void Set_InvenSelNum(_uint iNum) { m_iInvenSelNum = iNum; }
	_uint Get_InvenSelNum() { return m_iInvenSelNum; }

	// �÷��̾ �� ���

	void Set_Player1Hat(_uint iNum) { m_iPlayer1Hat = iNum; }
	_uint Get_Player1Hat() { return m_iPlayer1Hat; }

	void Set_Player1Clothes(_uint iNum) { m_iPlayer1Clothes = iNum; }
	_uint Get_Player1Clothes() { return m_iPlayer1Clothes; }

	void Set_Player1Deco(_uint iNum) { m_iPlayer1Deco = iNum; }
	_uint Get_Player1Deco() { return m_iPlayer1Deco; }


	void Set_Player2Hat(_uint iNum) { m_iPlayer2Hat = iNum; }
	_uint Get_Player2Hat() { return m_iPlayer2Hat; }

	void Set_Player2Clothes(_uint iNum) { m_iPlayer2Clothes = iNum; }
	_uint Get_Player2Clothes() { return m_iPlayer2Clothes; }

	void Set_Player2Deco(_uint iNum) { m_iPlayer2Deco = iNum; }
	_uint Get_Player2Deco() { return m_iPlayer2Deco; }


	void Set_Player3Hat(_uint iNum) { m_iPlayer3Hat = iNum; }
	_uint Get_Player3Hat() { return m_iPlayer3Hat; }

	void Set_Player3Clothes(_uint iNum) { m_iPlayer3Clothes = iNum; }
	_uint Get_Player3Clothes() { return m_iPlayer3Clothes; }

	void Set_Player3Deco(_uint iNum) { m_iPlayer3Deco = iNum; }
	_uint Get_Player3Deco() { return m_iPlayer3Deco; }

	// ���â ������ �÷��̾��� ��ȣ
	void Set_EquipmentPlayer(_uint iNum) { m_iEquipmentPlayer = iNum; }
	_uint Get_EquipmentPlayer() { return m_iEquipmentPlayer; }

	void Set_TalkButton(_bool iNum) { m_bTalkButton = iNum; }
	_uint Get_TalkButton() { return m_bTalkButton; }

	void Set_PunchTime(_float iNum) { m_fPunchTime = iNum; }
	_float Get_PunchTime() { return m_fPunchTime; }

	// Quest
	void Set_QuestNum(_uint iNum) { m_iQuestNum = iNum; }
	_uint Get_QuestNum() { return m_iQuestNum; }

	void Plus_QuestNum() { ++m_iQuestNum; }
	void Minus_QuestNum() { --m_iQuestNum; }

	void Set_QuestOn(_bool bQuestOn) { m_bQuestOn = bQuestOn; }
	_bool Get_QuestOn() { return m_bQuestOn; }

	void Set_NextQuest(_bool bNextQuest) { m_bNextQuest = bNextQuest; }
	_bool Get_NextQuest() { return m_bNextQuest; }

	void Set_AccOn(_bool bAccOn) { m_bAccOn = bAccOn; }
	_bool Get_AccOn() { return m_bAccOn; }

	void Set_QuestOff(_bool bQuestOff) { m_bQuestOff = bQuestOff; }
	_bool Get_QuestOff() { return m_bQuestOff; }
	
	_bool Get_Meg() { return m_bMeg; }
	void Set_Meg(_bool Meg) { m_bMeg = Meg; }

	// �����̴� �շ�����
	void LEVIADA_On() { m_bLEVIADA = true; }
	_bool LEVIADA_State() { return m_bLEVIADA; }

	// DD �շ�����
	void DD_On() { m_bDD = true; }
	_bool DD_State() { return m_bDD; }

	// �շ��Ҷ� UI�� ��������
	void PartyOn(_bool bPartyOn) { m_bPartyOn = bPartyOn; }
	_bool PartyOn_State() { return m_bPartyOn; }

	// �շ��Ҷ� UI�� ��������
	void Set_Stage2Start(_bool bStage2Start) { m_bStage2Start = bStage2Start; }
	_bool Get_Stage2Start() { return m_bStage2Start; }

	// Burst
	void Set_BurstOn(_bool bBurstOn) { m_bBurstOn = bBurstOn; }
	_uint Get_BurstOn() { return m_bBurstOn; }

	void Set_BeforeWeapon(_uint bBurstOn) { m_iBeforeWeapon = bBurstOn; }
	_uint Get_BeforeWeapon() { return m_iBeforeWeapon; }

	void Set_PunchDamage(_int Damage) { m_iPunchDamage = Damage; }
	_int Get_PunchDamage() { return m_iPunchDamage; }

	// �κ� ��ȣ
	_uint Get_InvenNum() { return m_iInvenNum; }
	void Set_InvenNum(_uint iNum) { m_iInvenNum = iNum; }

	//�÷��̾� ���� HP ������ �÷ȴ��� ���� �Ǵ�
	_bool Get_StatUp1() { return m_bStatUp1; }
	void Set_StatUp1(_bool bStatUp1) { m_bStatUp1 = bStatUp1; }

	_bool Get_StatUp3() { return m_bStatUp3; }
	void Set_StatUp3(_bool bStatUp3) { m_bStatUp3 = bStatUp3; }

	_bool Get_StatUp4() { return m_bStatUp4; }
	void Set_StatUp4(_bool bStatUp4) { m_bStatUp4 = bStatUp4; }

	//�÷��̾��� BP Text Num
	_uint Get_BurstBarNum() { return m_iBurstBarNum; }
	void Set_BurstBarNum(_uint iNum) { m_iBurstBarNum = iNum; }

	//����Ʈ UI ���� ���
	_bool Get_FrontUI() { return m_bFrontUI; }
	void Set_FrontUI(_bool bFrontUI) { m_bFrontUI = bFrontUI; }

	_bool Get_Trigger1() { return m_bOXTrigger1; }
	void Set_Trigger1(_bool Trigger) { m_bOXTrigger1 = Trigger; }

	_bool Get_Trigger2() { return m_bOXTrigger2; }
	void Set_Trigger2(_bool Trigger) { m_bOXTrigger2 = Trigger; }

	_bool Get_Trigger3() { return m_bOXTrigger3; }
	void Set_Trigger3(_bool Trigger) { m_bOXTrigger3 = Trigger; }

	_bool Get_Trigger4() { return m_bOXTrigger4; }
	void Set_Trigger4(_bool Trigger) { m_bOXTrigger4 = Trigger; }

	_bool Get_Trigger5() { return m_bOXTrigger5; }
	void Set_Trigger5(_bool Trigger) { m_bOXTrigger5 = Trigger; }

	_bool Get_Trigger6() { return m_bOXTrigger6; }
	void Set_Trigger6(_bool Trigger) { m_bOXTrigger6 = Trigger; }

	_bool Get_Trigger7() { return m_bOXTrigger7; }
	void Set_Trigger7(_bool Trigger) { m_bOXTrigger7 = Trigger; }

	//BossNum
	_uint Get_BossNum() { return m_iBossNum; }
	void Set_BossNum(_uint iNum) { m_iBossNum = iNum; }

	//���Ϳ��� �¾Ҵ���
	_bool Get_ShakeOn1() { return m_bShakeOn1; }
	void Set_ShakeOn1(_bool bShakeOn) { m_bShakeOn1 = bShakeOn; }

	_bool Get_ShakeOn2() { return m_bShakeOn2; }
	void Set_ShakeOn2(_bool bShakeOn) { m_bShakeOn2 = bShakeOn; }

	_bool Get_ShakeOn3() { return m_bShakeOn3; }
	void Set_ShakeOn3(_bool bShakeOn) { m_bShakeOn3 = bShakeOn; }

	_bool Get_Gudi() { return m_bStopGudi; }
	void Set_Gudi(_bool bStopGudi) { m_bStopGudi = bStopGudi; }

	_uint Get_InvenType() { return m_iInvenType; }
	void Set_InvenType(_uint iNum) { m_iInvenType = iNum; }

	// ������ ���� ���� ����
	_uint Get_ShopSelect() { return m_iShopSelect; }
	void Set_ShopSelect(_uint iNum) { m_iShopSelect = iNum; }

	void Gain_BattleItem(_uint iNumType, _uint itemCount) { m_iBattleItem[iNumType] += itemCount; }
	void Gain_SellBattleItem(_uint iNumType, _uint itemCount) { m_iBattleItem[iNumType] -= itemCount; }
	void Use_BattleItem(_uint iNumType) { if(m_iBattleItem[iNumType] >= 1)
		m_iBattleItem[iNumType]--; }
	void Sell_BattleItem(_uint iNumtype, _uint iNum) {
		if ((m_iBattleItem[iNumtype] >= iNum))
			m_iBattleItem[iNumtype] -= iNum;
	}
	_uint Get_BattleItem(_uint iNumType) { return m_iBattleItem[iNumType]; }
	_uint Get_ZeroNum(_uint iNum);

	// ���� ������ ����
	void Gain_Food(_uint iNumType, _uint itemCount) { m_iFood[iNumType] += itemCount; }
	void Eat_Food(_uint iNumType) { m_iFood[iNumType]--; m_iBeFull++; }
	_uint Get_Food(_uint iNumType) { return m_iFood[iNumType]; }
	_uint Get_FoodZero(_uint iNum);

	void Reset_BeFull() { m_iBeFull = 0; }
	void Flow_BeFull() {
		if (m_iBeFull > 0)
		{
			m_iBeFull--;
		}
	}
	_uint Get_BeFull() { return m_iBeFull; }

	void Set_EatText(_bool bText) { m_bEatText = bText; }
	_bool Get_EatText() { return m_bEatText; }

	void Shop_On() { m_bShop = true; }
	void Shop_Off() { m_bShop = false; }
	_bool Get_ShopState() { return  m_bShop; }

	_uint Get_ShopItem() { return m_iShopItem; }
	void Set_ShopItem(_uint iNum) { m_iShopItem = iNum; }

	_bool Get_LevelUpEffect() { return m_bLevelUpEffect; }
	void Set_LevelUpEffect(_bool iNum) { m_bLevelUpEffect = iNum; }

	_uint Get_ShopItemCount() { return m_iShopItemCount; }
	void Set_ShopItemCount(_uint iNum) { m_iShopItemCount = iNum; }

	_uint Get_EquipmentType() { return m_iEquipmentType; }
	void Set_EquipmentType(_uint iNum) { m_iEquipmentType = iNum; }

	// c��ư Ÿ��
	_uint Get_TalkButtonType() { return m_iTalkButton; }
	void Set_TalkButtonType(_uint iNum) { m_iTalkButton = iNum; }

	_uint Get_BattleNum() { return m_iBattleNum; }
	void Set_BattleNum(_uint iNum) { m_iBattleNum = iNum; }

	_uint Get_Page1Select() { return m_iPage1Select; }
	void Set_Page1Select(_uint iNum) { m_iPage1Select = iNum; }

	_uint Get_BlockNum() { return m_iBlockNum; }
	void Set_BlockNum(_uint iNum) { m_iBlockNum = iNum; }

	_float Get_BlockPos() { return m_fBlockPos; }
	void Set_BlockPos(_float iNum) { m_fBlockPos = iNum; }

	_float3 Get_BlockRGB() { return m_fRGB; }
	void Set_BlockRGB(_float R, _float G, _float B) { m_fRGB = { R,G,B }; }


	void SubwayOn() { m_bSubway = true; }
	void SubwayOff() { m_bSubway = false; }
	_bool Get_Subway() { return m_bSubway ; }

	void Subway1On() { m_bSubway1 = true; }
	void Subway1Off() { m_bSubway1 = false; }
	_bool Get_Subway1() { return m_bSubway1; }

	void Subway2On() { m_bSubway2 = true; }
	void Subway2Off() { m_bSubway2 = false; }
	_bool Get_Subway2() { return m_bSubway2; }

	//���� ���� �� on off
	_bool Get_ShopSelectBar() { return m_bShopSelectBar; }
	void Set_ShopSelectBar(_bool iNum) { m_bShopSelectBar = iNum; }

	void Select_On() { m_bSelect = true; }
	void Select_Off() { m_bSelect = false; }
	_bool Get_Select() { return  m_bSelect; }

	void UpgradeSelect_On() { m_bUpgradeSelect = true; }
	void UpgradeSelect_Off() { m_bUpgradeSelect = false; }
	_bool Get_UpgradeSelect() { return  m_bUpgradeSelect; }

	void UpgradeFinish_On() { m_bUpgradeFinish = true; }
	void UpgradeFinish_Off() { m_bUpgradeFinish = false; }
	_bool Get_UpgradeFinish() { return  m_bUpgradeFinish; }

	void SellSelect_On() { m_bSellSelect = true; }
	void SellSelect_Off() { m_bSellSelect = false; }
	_bool SellGet_Select() { return  m_bSellSelect; }

	// �δ���
	void BearHunt_On() { m_bBearHunt = true; }
	void BearHunt_Off() { m_bBearHunt = false; }
	_bool Get_BearHunt() { return  m_bBearHunt; }

	void Set_BearType(_uint iNum) { m_iBearType = iNum; }
	_uint Get_BearType() { return m_iBearType; }

	void Vendor1Select_On() { m_bSelect1Vendor = true; }
	void Vendor1Select_Off() { m_bSelect1Vendor = false; }
	_bool Get_Vendor1Select() { return  m_bSelect1Vendor; }

	void Vendor2Select_On() { m_bSelect2Vendor = true; }
	void Vendor2Select_Off() { m_bSelect2Vendor = false; }
	_bool Get_Vendor2Select() { return  m_bSelect2Vendor; }

	void Vendor3Select_On() { m_bSelect3Vendor = true; }
	void Vendor3Select_Off() { m_bSelect3Vendor = false; }
	_bool Get_Vendor3Select() { return  m_bSelect3Vendor; }

	void Vendor4Select_On() { m_bSelect4Vendor = true; }
	void Vendor4Select_Off() { m_bSelect4Vendor = false; }
	_bool Get_Vendor4Select() { return  m_bSelect4Vendor; }

	void Vendor5Select_On() { m_bSelect5Vendor = true; }
	void Vendor5Select_Off() { m_bSelect5Vendor = false; }
	_bool Get_Vendor5Select() { return  m_bSelect5Vendor; }

	void Set_StreetShop(_bool bStreetShop) { m_bStreetShop = bStreetShop; }
	_bool Get_StreetShop() { return  m_bStreetShop; }

	void Set_Inven(_bool bSelect) { m_bInvenSelect = bSelect; }
	_bool Get_Inven() { return  m_bInvenSelect; }

	void Set_UnSelect(_bool bSelect) { m_bUnSelect = bSelect; }
	_bool Get_UnSelect() { return  m_bUnSelect; }

	void Set_BattleType(_uint iNum) { m_iBattleType = iNum; }
	_uint Get_BattleType() { return  m_iBattleType; }

	void Set_FoodType(_uint iNum) { m_iFoodType = iNum; }
	_uint Get_FoodType() { return  m_iFoodType; }

	_uint Get_BattleItemNum(_uint iNum) { return m_iBattleItemNum[iNum]; }
	void Set_BattleItemNum(_uint iIndex,_uint iNum) { m_iBattleItemNum[iIndex] = iNum; }

	void Make_BattleEnd();

	void Make_Ink();

	void Gain_DropItemNum() { m_iDropItemNum++; }
	void Erase_DropItemNum() { 
		if (m_iDropItemNum == 0)
			m_iDropItemNum = 0;
		else
			m_iDropItemNum--; }
	_uint Get_DropItemNum() { return m_iDropItemNum; }
	void DropItem_On() { m_bDropItemOn = true; }
	void DropItem_Off() { m_bDropItemOn = false; }
	_bool Get_DropItemOn() { return m_bDropItemOn; }

	void DropItem4_On() { m_iDropItemNum4 = true; }
	void DropItem4_Off() { m_iDropItemNum4 = false; }
	_bool Get_DropItem4On() { return m_iDropItemNum4; }

	void DropItemVanish_On() { m_bDropItemVanish = true; }
	void DropItemVanish_Off() { m_bDropItemVanish = false; }
	_bool Get_DropItemVanish() { return m_bDropItemVanish; }

	_bool Get_Upgrade() { return m_iUpgrade; }
	void Set_Upgrade(_bool bUpgrade) { m_iUpgrade = bUpgrade; }

	_float3 Get_QuestPos();

	void Eat_Str() { m_bFoodStr = true; }
	void Eat_Hp() { m_bFoodHp = true; }
	_bool Get_FoodStr() { return m_bFoodStr; }
	_bool Get_FoodHp() { return m_bFoodHp; }
	// ������ �԰� �ð��� �þ ����
	void Set_FoodStr() { m_bFoodStr = false; }
	void Set_FoodHp() { m_bFoodHp = false; }

	void Set_FoodStrTime(_float fTime) { m_fFoodStrTime = fTime; }
	_float Get_FoodStrTime() { return m_fFoodStrTime; }

	void Set_FoodHpTime(_float fTime) { m_fFoodHpTime = fTime; }
	_float Get_FoodHpTime() { return m_fFoodHpTime; }

	void UsedMoney(_uint iMoney) { m_iUsedMoney = iMoney; }
	_uint Get_ShopUsedMoney() { return m_iUsedMoney; }
	void Set_UsedMoney(_bool bUse) { m_bUsedMoney = bUse; }
	_bool Get_UsedMoney() { return m_bUsedMoney; }

	void Warning_On() { m_bWarning = true; }
	void Warning_Off() { m_bWarning = false; }
	_bool Get_Warning() { return m_bWarning; }

	void Set_PunchLight(_bool bLight) { m_bPunchLight = bLight; }
	_bool Get_PunchLight() { return m_bPunchLight; }

	void GolemName_On() { m_bGolemName = true; }
	void GolemName_Off() { m_bGolemName = false; }
	_bool Get_GolemName() { return m_bGolemName; }

	void RiddleName_On() { m_bRiddleName = true; }
	void RiddleName_Off() { m_bRiddleName = false; }
	_bool Get_RiddleName() { return m_bRiddleName; }

	void KumaName_On() { m_bKumaName = true; }
	void KumaName_Off() { m_bKumaName = false; }
	_bool Get_KumaName() { return m_bKumaName; }

	void ChaosName_On() { m_bChaosName = true; }
	void ChaosName_Off() { m_bChaosName = false; }
	_bool Get_ChaosName() { return m_bChaosName; }

	void Set_VendorText(_uint iNum) { m_iVendorText = iNum; }
	_uint Get_VendorText() { return  m_iVendorText; }

	void Set_Player1Weapon(_uint iNum) { m_iPlayer1WeaponLv = iNum; }
	_uint Get_Player1Weapon() { return  m_iPlayer1WeaponLv; }

	void Set_Player2Weapon(_uint iNum) { m_iPlayer2WeaponLv = iNum; }
	_uint Get_Player2Weapon() { return  m_iPlayer2WeaponLv; }

	void Set_Player3Weapon(_uint iNum) { m_iPlayer3WeaponLv = iNum; }
	_uint Get_Player3Weapon() { return  m_iPlayer3WeaponLv; }

	_float Get_MinimapPosx() { return m_fMinimapx; }
	void Set_MinimapPosx(_float fNum) { m_fMinimapx = fNum; }
	_float Get_MinimapPosy() { return m_fMinimapy; }
	void Set_MinimapPosy(_float fNum) { m_fMinimapy = fNum; }

	void BearOn(_uint iNum) { m_bBear[iNum] = true; }
	void BearOff(_uint iNum) { m_bBear[iNum] = false; }
	_bool Get_Bear(_uint iNum) { return m_bBear[iNum]; }

	void Gain_Money(_uint iNum) { m_iMoney += iNum; }
	void Use_Money(_uint iNum) { m_iMoney -= iNum; }
	_uint Get_Money() { return m_iMoney; }

	_uint Get_BattleIndex() { return m_iBattleIndex; }
	void Set_BattleIndex(_uint iIndex) { m_iBattleIndex = iIndex; }

private:
	_uint m_iMenuNum = 0;
	_uint m_iBackNum = 0;
	_uint m_iLevelUpSelectPageNum = 1;
	_uint m_iEquipSelectPageNum = 1;
	_uint m_iStatusNum = 1;

	_uint m_iPlayer1Str = 1;
	_uint m_iPlayer1Stamina = 1;
	_uint m_iPlayer1Hp = 1;
	_uint m_iPlayer1Mental = 1;
	_uint m_iPlayer1Meta = 1;
	_uint m_iPlayer1Explosive = 1;
	_uint m_iPlayer1Luck = 1;

	_uint m_iPlayer1Level = 1;

	_bool m_bWarning = false;
	_bool m_bGolemName = false;
	_bool m_bRiddleName = false;
	_bool m_bKumaName = false;
	_bool m_bChaosName = false;

	_uint m_iPlayer2Str = 1;
	_uint m_iPlayer2Stamina = 1;
	_uint m_iPlayer2Hp = 1;
	_uint m_iPlayer2Mental = 1;
	_uint m_iPlayer2Meta = 1;
	_uint m_iPlayer2Explosive = 1;
	_uint m_iPlayer2Luck = 1;

	_uint m_iPlayer2Level = 1;

	_uint m_iPlayer3Str = 1;
	_uint m_iPlayer3Stamina = 1;
	_uint m_iPlayer3Hp = 1;
	_uint m_iPlayer3Mental = 1;
	_uint m_iPlayer3Meta = 1;
	_uint m_iPlayer3Explosive = 1;
	_uint m_iPlayer3Luck = 1;

	_uint m_iPlayer3Level = 1;

	_bool m_bPunchLight = false;

	_uint m_iReserve = 0;

	_bool m_bPictureBurst = false;

	// ���� �Ծ����� ���ݷ�, ���� ����
	_bool m_bFoodStr = false;
	_bool m_bFoodHp = false;
	// ����, ���ݷ� ���� �ð�
	_float m_fFoodStrTime = 0.f;
	_float m_fFoodHpTime = 0.f;

	// ����â���� �÷��̾ ���õ� ��ȣ
	_uint m_iPlayerSelectNum = 1;

	// ���â���� �÷��̾ ���õ� ��ȣ
	_uint m_iEquipPlayerSelectNum = 1;
	_uint m_iShopSelect = 0;

	

	_uint m_iExp = 50210;

	_uint m_iCombo = 0;
	_bool m_bMoreCombo = false;
	_bool m_bHit = false;
	_bool m_bUp = false;

	_uint m_iQuestNum = 0;
	_bool m_bQuestOn = false;
	_bool m_bNextQuest = false;
	_bool m_bPartyOn = false;
	_bool m_bAccOn = false;
	_bool m_bQuestOff = false;

	_bool m_bStopGudi = false;
	_bool m_bBurstOn = false;
	_bool m_bStatUp1 = false;
	_bool m_bStatUp3 = false;
	_bool m_bStatUp4 = false;

	_uint m_iBurstBarNum = 0;
	_uint m_iBossNum = 0;
	_bool m_bFrontUI = false;
	_bool m_bStage2Start = false;
	_bool m_bShakeOn1 = false;
	_bool m_bShakeOn2 = false;
	_bool m_bShakeOn3 = false;

	_uint m_iCoin = 0;
	_bool m_bCoinOn = false;
	_bool m_bCoinUp = false;

	_uint m_iStageNum = 0;


	_float3 m_QuestPos = { 11.5f, 1.4f, 25.3f };
	_bool m_bGetPos = false;

	// ���
	_uint m_iEquipmentClassSelect = 1;
	_uint m_iEquipmentNum = 0;
	_uint m_iEquipmentState = 0;

	// �÷��̾� HP (LV�ƴ�)

	_uint m_iPlayer1CurrentHp = 0;
	_uint m_iPlayer2CurrentHp = 0;
	_uint m_iPlayer3CurrentHp = 0;

	// �÷��̾ ������ e
	_uint m_iPlayer1SelectEHat = 0;
	_uint m_iPlayer1SelectEClothes = 0;
	_uint m_iPlayer1SelectEDeco = 0;

	_uint m_iPlayer2SelectEHat = 0;
	_uint m_iPlayer2SelectEClothes = 0;
	_uint m_iPlayer2SelectEDeco = 0;

	_uint m_iPlayer3SelectEHat = 0;
	_uint m_iPlayer3SelectEClothes = 0;
	_uint m_iPlayer3SelectEDeco = 0;

	// ��θ� ����
	_uint m_iBeFull = 0;


	// ȹ���� ��� ����
	_uint m_iHat[10];
	_uint m_iClothes[10];
	_uint m_iDeco[10];

	// ȹ���� ��Ӿ����� ����
	_uint m_iDropItem[10];

	// �÷��̾�� �� ���
	_uint m_iPlayer1Hat = 0;
	_uint m_iPlayer1Clothes = 0;
	_uint m_iPlayer1Deco = 0;

	_uint m_iPlayer2Hat = 0;
	_uint m_iPlayer2Clothes = 0;
	_uint m_iPlayer2Deco = 0;

	_uint m_iPlayer3Hat = 0;
	_uint m_iPlayer3Clothes = 0;
	_uint m_iPlayer3Deco = 0;

	//�������� ������ ����� ��
	_uint m_iUsedMoney = 0;
	// �������� ���� �Ҹ��Ͽ���

	_bool m_bUsedMoney = false;

	//���â �÷��̾�
	_uint m_iEquipmentPlayer = 1;

	// �κ�â ���� �κ� ��ȣ
	_uint m_iInvenNum = 0;
	// �κ�â�� ���� �����ۿ� ������ ��ȣ
	_uint m_iInvenType = 0;
	// �����ۿ� ���� ���� ��ȣ
	_uint m_iInvenSelNum = 0;

	// �� ����
	_uint m_iBlockNum = 1;
	_float3 m_fRGB;
	_float m_fBlockPos = 0;

	// �κ��� ������ ���� ox
	_bool m_bInvenSelect = false;

	// �κ�â�� ��Ʋ ������
	_uint m_iBattleItem[7];
	// �κ�â ��� ������
	_uint m_iEquipItem[15];
	// �κ�â ��� ������
	_uint m_iFood[7];

	// c ��ȭâ
	_bool m_bTalkButton = false;
	_uint m_iTalkButton = 0;

	// �ʵ� �̸�
	_bool m_bSubway = false;
	_bool m_bSubway1 = false;
	_bool m_bSubway2 = false;

	//����
	_bool m_bBear[12];


	//���� Ű�� ����
	_bool m_bShop = false;
	//������
	_bool m_bStreetShop = false;
	_bool m_bStreetVendor1 = false;
	_bool m_bStreetVendor2 = false;
	_bool m_bStreetVendor3 = false;
	_bool m_bStreetVendor4 = false;
	_bool m_bStreetVendor5 = false;

	// ����â ������ ���� ��ȣ
	_uint m_iShopItem = 0;
	// ������ ����,�ǸŽ� ������ ����
	_uint m_iShopItemCount = 0;
	_bool m_bSelect = false;
	_bool m_bSellSelect = false;
	_bool m_bUpgradeSelect = false;
	_bool m_bUpgradeFinish = false;

	_bool m_bShopSelectBar = false;

	// ���� ������ 1�� ���û�Ȳ
	_uint m_iPage1Select = 0;

	// ������ ����,�ǸŽ� ������ ���� �� â
	_uint m_iVendor1Count = 0;
	_bool m_bSelect1Vendor = false;
	_uint m_iVendor2Count = 0;
	_bool m_bSelect2Vendor = false;
	_uint m_iVendor3Count = 0;
	_bool m_bSelect3Vendor = false;
	_uint m_iVendor4Count = 0;
	_bool m_bSelect4Vendor = false;
	_uint m_iVendor5Count = 0;
	_bool m_bSelect5Vendor = false;

	//����� ������ �� ������ ��
	_uint m_iBeforeWeapon = 0;

	// ������ ����Ʈ
	_bool m_bLevelUpEffect = false;

	// �κ����� Ȯ���� ����� Ÿ��
	_uint m_iEquipmentType = 0;

	//��Ʋ�������� �κ�â���� �簢��
	_uint m_iBattleNum = 0;

	// ��Ʋ ������ �ѹ�
	_uint m_iBattleItemNum[4];

	//��� ������ ����
	_bool m_iDropItemNum4 = false;
	_uint m_iDropItemNum = 0;
	_bool m_bDropItemOn = false;
	_bool m_bDropItemVanish = false;

	// �������� 4 ox Ʈ����
	_bool m_bOXTrigger1 = false;
	_bool m_bOXTrigger2 = false;
	_bool m_bOXTrigger3 = false;
	_bool m_bOXTrigger4 = false;
	_bool m_bOXTrigger5 = false;
	_bool m_bOXTrigger6 = false;
	_bool m_bOXTrigger7 = false;

	_int m_iPunchDamage = 0;

	_bool m_bMeg = false;


	// �� �ִ� ��
	_uint m_iMoney = 50000;

	// ��Ʋ ����â���� �Ⱥ��̰� �ϴ°�
	_bool m_bUnSelect = false;

	_uint m_iBattleType = 0;
	_uint m_iFoodType = 0;

	_bool m_bEatText = false;

	_uint m_iVendorText = 0;

	_float m_fPunchTime = 0.f;

	// �̴ϸ� ��ġ
	_float m_fMinimapx = 0.f;
	_float m_fMinimapy = 0.f;

	_bool m_iUpgrade = false;

	// �δ��� ����
	_bool m_bBearHunt = false;
	_uint m_iBearType = 0;

	_uint m_iPlayer1WeaponLv = 0;
	_uint m_iPlayer2WeaponLv = 0;
	_uint m_iPlayer3WeaponLv = 0;
	
	_uint m_iMaxCombo = 0;

	_uint m_iBattleIndex = 0;

#pragma region ��ȭâ
	// ��ȭâ
	public:
		HRESULT BackUp_DialogTalk(class CDialogTalk* pUI); // UI_Maniger���� ��ȭâ�� ���� �ְ� �ϴ� �Լ�
		HRESULT BackUp_ActiveQuest(class CActiveQuest* pUI); // UI_Manager���� ����Ʈ��ġ UI�� ���� �ְ� �ϴ� �Լ�
		HRESULT BackUp_BattleDialog(class CBattleDialog* pUI); // UI_Maniger���� ������ ��ȭâ�� ���� �ְ� �ϴ� �Լ�

															   // �Ϲ� ��ȭâ
		const _bool Get_TalkOnOff(); // �ٸ� ��ü���� ��ȭâ�� �������¸� ������ �Լ�
		const _uint& Get_TalkIndex(); // �ٸ� ��ü���� ��ȭâ�� �����ε����� ������ �Լ�
		void Set_TalkIndex(_uint Index); // �ٸ� ��ü���� ��ȭâ�� �ε����� �����ϰ� ������ ���۽�Ű�� �Լ�
										 // ����Ʈ ��ġ������
		void Set_QuestPoint(_float3 vPos, _uint iQuestIconIndex); // UI ����Ʈ�� ��ġ�� �������ִ� �Ǵ� �ٲ� �ִ� �Լ�, Stage4�� �ι�° ���ڸ� 1��
		void Set_QuestPointOff(); // UI ����Ʈ ��ġ�� �˷��� UI�� �������� �Լ�.
		_bool Get_QuestPointRenderState(); // ����Ʈ ��ġ�� �˸��� UI�� �������¸� �޾ƿ��� �Լ�
										   // ������ ��ȭ
		_bool Get_BattleTalkOnOff(); // �����ߴ�ȭ�� �����ִ��� �Ǵ�
		_uint Get_BattleTalkIndex(); // ������ ��ȭ�� �ε����� ����
		void Set_BattleTalkIndex(_uint Index); // ������ ��ȭ�� �ε��� ������ ���ÿ� ���� ����
		void Set_BattleOffTalk(); // ������ ��ȭ�� ��


	private:
		class CDialogTalk*		m_pDIalogTalk = nullptr; // ��ȭâ ��ü
		class CActiveQuest*		m_pActiveQuest = nullptr;// ����Ʈ��ġ UI
		class CBattleDialog*	m_pBattleDialog = nullptr;// �����ߴ�ȭ
#pragma endregion ��ȭâ
	
#pragma region �ܼ�
	// �ܼ� ����
	public:
		HRESULT BackUp_Clue1(class CClue1* pUI);
		HRESULT BackUp_Clue2(class CClue2* pUI);

		void Add_ClueCnt(void) { ++m_iClueCnt; }
		_uint Get_ClueCnt(void) { return m_iClueCnt; }

		void Render_Clue1();
		void Render_Clue2();


		void Clear_Clue1() { m_pClue1 = nullptr; }
		void Clear_Clue2() { m_pClue2 = nullptr; }

	private:
		class CClue1*			m_pClue1 = nullptr;
		class CClue2*			m_pClue2 = nullptr;

		_uint					m_iClueCnt = 0; // ������ � �Ծ���? 1�� -> ������ ù���� 234-> ���� 
#pragma endregion �ܼ�

#pragma region Blind
	// ����ε� ����
	public:
		void BackUp_Blind(class CBlind* pUI);
		void MoveBlind(_bool b); // true�̸� ������ false �̸� ������
		_bool Get_MoveOver(); // �ݱ��� �����°�

	private:
		class CBlind*		m_pBlind = nullptr;

#pragma endregion Blind

	
	_bool m_bDD = false;
	_bool m_bLEVIADA = false;
};

END