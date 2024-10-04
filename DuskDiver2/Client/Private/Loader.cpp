#include "stdafx.h"
//MainClass
#include "..\Public\Loader.h"
#include "GameInstance.h"
#include "DataManager.h"
#include "ToolManager.h"

//Component
#include "SwordTrail.h"
#include "PointTrail.h"
#include "PointTrailObj.h"
#include "SwordTrailObj.h"

//Model
#include "Item.h"
#include "DropItem.h"
#include "Skybox.h"
#include "Stage1.h"
#include "Stage1Light.h"
#include "Stage2.h"
#include "Stage2Glass.h"
#include "Stage3.h"
#include "Buildings.h"
#include "Stage3Glass.h"
#include "Stage3Water.h"
#include "Stage3Terrain.h"
#include "Stage3Trees.h"
#include "Stage3Ad.h"
#include "Stage3Door.h"
#include "Stage3DoorGlass.h"
#include "Stage3Cars.h"
#include "Stage3Bus.h"
#include "AzitLight.h"
#include "AzitProp.h"
#include "Stage3Event1.h"
#include "Stage3AzitEvent.h"
#include "CluePic.h"
#include "MetalTrashbinBody.h"
#include "MetalTrashbinTop.h"
#include "Trash.h"
#include "StreetStore.h"
#include "Stage4.h"
#include "Stage4Roof.h"
#include "Stage4Face.h"
#include "Stage4Glass.h"
#include "Stage4Light.h"
#include "Stage4Star.h"
#include "Stage4Machine0.h"
#include "Stage4Coin.h"
#include "Stage4Escalator.h"
#include "Stage4Event1.h"
#include "Stage4Event2.h"
#include "Stage4Event3.h"
#include "Stage4Event4.h"
#include "Stage4Event5.h"
#include "Stage5.h"
#include "Stage5Light.h"
#include "Stage5Event1.h"
#include "CrystalBody.h"
#include "CrystalPart.h"
#include "CrystalFake.h"
#include "AnimMesh.h"
#include "Mesh.h"
#include "Player.h"
#include "Player01.h"
#include "Player03.h"
#include "Player04.h"
#include "Stage2Player04.h"
#include "Stage1Event1.h"
#include "Stage1Event2.h"
#include "Stage1Event3.h"
#include "Stage2Event1.h"
#include "Stage2Sound.h"
#include "Stage3Sound.h"
#include "Camera_Main.h"
#include "Player01HairA.h"
#include "Player01HairB.h"
#include "Player01HairC.h"
#include "Player03HairA.h"
#include "Player03HairB.h"
#include "Player03LGun.h"
#include "Player03RGun.h"
#include "Player03ShotGun.h"
#include "Player03LShotGun.h"
#include "Player03Cannon.h"
#include "Player03LCannon.h"
#include "Player03Sniper.h"
#include "Player04Hair.h"
#include "Player04HairBattle.h"
#include "Player04Sword1.h"
#include "Player04Sword2.h"
#include "Player04SwordBack.h"
#include "TrainingRobot.h"
#include "DisasterFusion.h"
#include "DisasterBeta.h"
#include "DisasterBoss.h"
#include "Golem.h"
#include "DefenceRobot.h"
#include "BombRobot.h"
#include "Beast.h"
#include "MechanicalOctopus.h"
#include "MechanicalOctopusLeg.h"
#include "NPCTutorial.h"
#include "NPCWalk.h"
#include "NPCIdle.h"
#include "NPCDog.h"
#include "Dogbox.h"
#include "NPCPartTIme.h"
#include "NPCZengbo.h"
#include "Fissure.h"
#include "CrackStone.h"
#include "MonsterBullet.h"
#include "BossBomb.h"
#include "OldBombRobot.h"
#include "MonsterMissile.h"
#include "BossMissile.h"
#include "PxObj.h"
#include "Px_Tool.h"
#include "PointLight_Tool.h"
#include "AnimMap_Tool.h"
#include "InvenEquipSlot0.h"
#include "InvenEquipSlot1.h"
#include "InvenEquipSlot2.h"
#include "InvenEquipSlot3.h"
#include "InvenEquipSlot4.h"
#include "InvenEquipSlot5.h"
#include "InvenEquipSlot6.h"
#include "InvenEquipSlot7.h"
#include "InvenEquipSlot8.h"
#include "InvenEquipSlot9.h"
#include "InvenEquipSlot10.h"
#include "InvenEquipSlot11.h"
#include "InvenEquipSlot12.h"
#include "InvenEquipSlot13.h"
#include "InvenEquipSlot14.h"
#include "Rock.h"
#include "RollRock.h"
#include "InventorySelectBar.h"
#include "TutorialMinimap.h"
#include "MiniPlayer.h"
#include "MiniQuest.h"
#include "MiniBossMonster.h"
#include "BossBase.h"
#include "SkullBase.h"
#include "SkullLight.h"
#include "SkullVanish.h"
#include "Warning.h"
#include "ThunderBase0.h"
#include "Thunder0.h"
#include "ThunderBase1.h"
#include "Thunder1.h"
#include "ThunderBase2.h"
#include "Thunder2.h"
#include "ThunderBase3.h"
#include "Thunder3.h"
#include "ThunderBase4.h"
#include "Thunder4.h"
#include "ThunderBase5.h"
#include "Thunder5.h"
#include "MenuNumber1.h"
#include "MenuNumber2.h"
#include "MenuNumber3.h"
#include "YMTest.h"
#include "MapName.h"
#include "LastBlock.h"
#include "UpPotal.h"
#include "DownPotal.h"

#include "Holo_1.h"
#include "Holo_2.h"
#include "Holo_3.h"
#include "Holo_4.h"
#include "Holo_5.h"
#include "Holo_5_Blue.h"
#include "Holo_6.h"
#include "Holo_7.h"
#include "Holo_8.h"
#include "Holo_9.h"
#include "Holo_10.h"
#include "RealHolo_1.h"
#include "RealHolo_2.h"
#include "RealHolo_3.h"
#include "RealHolo_4.h"
#include "RealHolo_5.h"
#include "LightObj.h"
#include "Stage2Event2.h"
#include "NPC_T.h"
#include "BlockWin.h"
#include "RedWindow.h"
#include "MiniGame_Block.h"
#include "BeastStoneBody.h"
#include "BeastStonePart.h"
#include "YabaweBall.h"
#include "BeastBreath.h"
#include "Stage4Trigger_1.h"
#include "Stage4Trigger_2.h"
#include "Stage4Trigger_3.h"
#include "Stage4Trigger_4.h"
#include "Stage4Trigger_5.h"
#include "Stage4Trigger_6.h"
#include "Stage4Trigger_7.h"
#include "MiniGame_Funch_Trigger.h"
#include "Boss_Cuma.h"
#include "BeastClaw.h"
#include "BeastPillar.h"

#include "DGCuma.h"
#include "DGCuma_1.h"
#include "DGCuma_2.h"
#include "DGCuma_3.h"
#include "DGTrigger.h"
#include "DGEndCuma.h"
#include "BallingCuma.h"
#include "SenterPotal.h"
#include "OctopusParticle.h"

//Effect
#include "Effect.h"
#include "Player04VoidEffect_2.h"
#include "AlphaParticle.h"
#include "ParticleInstance.h"
#include "Player01HitEffect_1.h"
#include "Player01HitEffect_2.h"
#include "Player01Sunder_1.h"
#include "Player01Sunder_2.h"
#include "Player03SniperEffect_1.h"
#include "Wall.h"
#include "Player01Trail_1.h"
#include "Wind.h"
#include "EffectSpin.h"
#include "EffectAtom.h"
#include "EffectShield.h"
#include "EffectShieldHit.h"
#include "Ring.h"
#include "EffectCraw.h"
#include "BossClaw.h"
#include "Tornado.h"
#include "ViewAlpha.h"
#include "Player01MotionTrail.h"
#include "Player03MotionTrail.h"
#include "Player04MotionTrail.h"
#include "EffectOctopusRange.h"
#include "Player04Slash.h"
#include "FusionThunder.h"
#include "FusionTrail.h"
#include "Player01Wind1.h"
#include "Player01Wind2.h"
#include "Player01Ball.h"
#include "Player01Ring.h"
#include "Player01Pillar.h"
#include "Player01Pillar2.h"
#include "Player01Pillar3.h"
#include "Player01Skill5Ball.h"
#include "CircleDistortion.h"
#include "Player01Rage1Ball.h"
#include "EffectMissileRange.h"
#include "Yabawe.h"
#include "RealYabawe.h"
#include "MiniGame_Yabawe.h"
#include "Player01SmokeBall.h"
#include "Player01Metamor.h"
#include "Player01Landing.h"
#include "Player01Upper.h"
#include "EffectPatternGauge.h"
#include "EffectPatternGaugeCircle.h"
#include "Player03CannonBullet.h"
#include "Player03Bomb.h"
#include "Player04Slash2.h"
#include "Player04SmokeBall.h"
#include "Player04DarkBall.h"
#include "TornadoSide.h"
#include "Player04Aura.h"
#include "Player04VoidParticle.h"
#include "Player04WhiteBall.h"
#include "Player04Rage1Particle.h"
#include "Player04Col.h"
#include "BossAttackRange.h"
#include "BeastTornado.h"
#include "BeastLaser.h"
#include "ReserveTrail.h"
#include "BeastMarvelBall.h"
#include "BeastMarvelObstacle.h"
#include "BeastMarvelPush.h"
#include "MarvelReserveTaril.h"
#include "MonsterSpawn.h"
#include "GolemWeaponRange.h"
#include "GolemAttackRange.h"
#include "PunchText.h"
#include "PunchScoreText.h"
#include "BlockMax.h"
#include "BlockM.h"
#include "KumaImage.h"


#include "Stage4Event1.h"
#include "BossPillar.h"
#include "BeastTornadoSide.h"
#include "BeastBreath2.h"
#include "BeastAura.h"
#include "BeastMap.h"
#include "BeastMap2.h"
#include "BeastMap3.h"

// UI
#include "BattleEndBase.h"
#include "BattleEnd.h"
#include "MenuBackGround.h"
#include "MenuBar.h"
#include "IllustYumo.h"
#include "IllustLeviada.h"
#include "IllustDD.h"
#include "MenuGuide.h"
#include "Player_Status01.h"
#include "Player_Status03.h"
#include "Player_Status04.h"
#include "Cross.h"
#include "Cross_Point.h"
#include "MenuNum1.h"
#include "MenuNum2.h"
#include "MenuNum3.h"
#include "BattleTag.h"
#include "MenuLevelUp.h"
#include "MenuLearnSkills.h"
#include "MenuEquipMent.h"
#include "MenuItems.h"
#include "LevelUpBackGround.h"
#include "LearnSkillsBackGround.h"
#include "EquipMentBackGround.h"
#include "ItemsBackGround.h"
#include "DuskDiverBackGround.h"
#include "LevelUpBase.h"
#include "EscBase.h"
#include "LevelUpBar.h"
#include "Team_Player01.h"
#include "Team_Player03.h"
#include "Team_Player04.h"
#include "Team_bg.h"
#include "LevelUpGuide.h"
#include "LevelUpSelectBackGround1.h"
#include "LevelUpSelectBackGround2.h"
#include "LevelUpSelectBackGround3.h"
#include "LevelUpSelectCharacter1.h"
#include "LevelUpSelectCharacter2.h"
#include "LevelUpSelectCharacter3.h"
#include "LevelUpName1.h"
#include "LevelUpName2.h"
#include "LevelUpName3.h"
#include "StatusBase.h"
#include "StatusBar.h"
#include "StatusBackGround.h"
#include "StatusWindow.h"
#include "StatusGuide.h"
#include "StatusSelectBar.h"
#include "Hit.h"
#include "Hit_Bar.h"
#include "Cross_Info.h"
#include "StatusStr.h"
#include "StatusStamina.h"
#include "StatusHp.h"
#include "StatusMental.h"
#include "StatusMeta.h"
#include "StatusExplosive.h"
#include "StatusLuck.h"
#include "Task_BackGround.h"
#include "Task_BackGround2.h"
#include "Task_Contents.h"
#include "Task_Title.h"
#include "StatIcon.h"
#include "Player1StrLv.h"
#include "Player1HpLv.h"
#include "Player1ExplosiveLv.h"
#include "Player1LuckLv.h"
#include "Player1MentalLv.h"
#include "Player1MetaLv.h"
#include "Player1StaminaLv.h"
#include "ReserveBar.h"
#include "Player1Icon.h"
#include "Player2Icon.h"
#include "Player3Icon.h"
#include "PlayerCircle.h"
#include "UI_Number.h"
#include "UI_Number2.h"
#include "NumExp.h"
#include "NumReserveExp.h"
#include "NumResultExp.h"
#include "NumLevel.h"
#include "NumHp.h"
#include "NumMental.h"
#include "NumMeta.h"
#include "NumStr.h"
#include "NumStamina.h"
#include "NumExplosive.h"
#include "NumLuck.h"
#include "StatusLT1.h"
#include "LT1Level.h"
#include "StatusLT2.h"
#include "PictureBurst.h"
#include "LT2Level.h"
#include "StatusLT3.h"
#include "LT3Level.h"
#include "LT3HP.h"
#include "LT2HP.h"
#include "LT1HP.h"
#include "Dash_Info.h"
#include "Dodge_Info.h"
#include "LockOn_Info.h"
#include "DDStart.h"
#include "PS01_Number.h"
#include "PS03_Number.h"
#include "PS04_Number.h"
#include "MinimapTest.h"
#include "Player_Status01_HP.h"
#include "Player_Status01_SP.h"
#include "Player_Status01_BP.h"
#include "Player_Status03_HP.h"
#include "Player_Status03_SP.h"
#include "Player_Status03_BP.h"
#include "Player_Status04_HP.h"
#include "Player_Status04_SP.h"
#include "Player_Status04_BP.h"
#include "EquipMentPriviousBar.h"
#include "EquipMentPriviousBase.h"
#include "IllustDDHp.h"
#include "IllustDDLv.h"
#include "IllustDDSp.h"
#include "IllustDDAttack.h"
#include "IllustDDArmor.h"
#include "IllustLeviadaHp.h"
#include "IllustLeviadaLv.h"
#include "IllustLeviadaSp.h"
#include "IllustLeviadaAttack.h"
#include "IllustLeviadaAmor.h"
#include "IllustYumoSp.h"
#include "IllustYumoHp.h"
#include "IllustYumoAttack.h"
#include "IllustYumoAmor.h"
#include "IllustYumoLv.h"
#include "Particle_Manager.h"
#include "EquipmentUpside.h"
#include "EquipmentBase.h"
#include "EquipmentCase.h"
#include "EquipmentStatus.h"
#include "EquipmentBox.h"
#include "EquipmentGuide.h"
#include "EquipmentSelectColor.h"
#include "EquipmentSelectCase.h"
#include "EquipmentSelectBar.h"
#include "EquipMentWeaponBackGround.h"
#include "EquipmentHatBackground.h"
#include "EquipmentClothesBackground.h"
#include "EquipmentDecoBackground.h"
#include "HatSlot1.h"
#include "HatSlot2.h"
#include "HatSlot3.h"
#include "HatSlot4.h"
#include "HatSlot5.h"
#include "HatSlot6.h"
#include "HatSlot7.h"
#include "ClothesSlot1.h"
#include "ClothesSlot2.h"
#include "ClothesSlot3.h"
#include "ClothesSlot4.h"
#include "ClothesSlot5.h"
#include "ClothesSlot6.h"
#include "ClothesSlot7.h"
#include "DecoSlot1.h"
#include "DecoSlot2.h"
#include "DecoSlot3.h"
#include "DecoSlot4.h"
#include "DecoSlot5.h"
#include "DecoSlot6.h"
#include "DecoSlot7.h"
#include "Player1EquipmentEHat.h"
#include "EquipmentStr.h"
#include "EquipmentStamina.h"
#include "EquipmentHp.h"
#include "EquipmentMeta.h"
#include "EquipmentMental.h"
#include "EquipmentExplosive.h"
#include "EquipmentLuck.h"
#include "EquipmentLv.h"
#include "Equipment1Info.h"
#include "Equipment2Info.h"
#include "Equipment3Info.h"
#include "Player2EquipmentE.h"
#include "Player3EquipmentE.h"
#include "MinimapBackground.h"
#include "MinimapStage1.h"
#include "MaskMap.h"
#include "MiniMapBase.h"
#include "MiniMapUpside.h"
#include "InventoryBase.h"
#include "InventoryUpside.h"
#include "InventoryWindow.h"
#include "SkillTextBasic.h"
#include "InvenQ.h"
#include "InvenE.h"
#include "InvenBattleItem.h"
#include "InvenDropItem.h"
#include "InvenEquipment.h"
#include "InvenFood.h"
#include "InvenQuestItem.h"
#include "InvenEquipmentBackground.h"
#include "InvenBattleItemBackground.h"
#include "InvenFoodBackground.h"
#include "InvenQuestItemBackground.h"
#include "DropItemSlot0.h"
#include "DropItemSlot1.h"
#include "DropItemSlot2.h"
#include "DropItemSlot3.h"
#include "DropItemSlot4.h"
#include "DropItemSlot5.h"
#include "BattleItemSlot0.h"
#include "BattleItemSlot1.h"
#include "BattleItemSlot2.h"
#include "BattleItemSlot3.h"
#include "BattleItemSlot4.h"
#include "BattleItemSlot5.h"
#include "BattleItemSlot6.h"
#include "Slot_Num.h"
#include "ShopBase.h"
#include "ShopUpside.h"
#include "ShopBuySelect.h"
#include "ShopSellSelect.h"
#include "ShopUpgradeSelect.h"
#include "ShopExitSelect.h"
#include "ShopWindow.h"
#include "ShopSelectBar.h"
#include "ShopBuyCount.h"
#include "ShopSellCount.h"
#include "NumBuy.h"
#include "EquipmentText.h"
#include "SelectSlot0.h"
#include "SelectSlot1.h"
#include "SelectSlot2.h"
#include "SelectSlot3.h"
#include "SelectImage0.h"
#include "SelectImage1.h"
#include "SelectImage2.h"
#include "SelectImage3.h"
#include "Hunger.h"
#include "FoodSlot0.h"
#include "FoodSlot1.h"
#include "FoodSlot2.h"
#include "FoodSlot3.h"
#include "FoodSlot4.h"
#include "FoodSlot5.h"
#include "FoodSlot6.h"
#include "EatText.h"
#include "StreetVendor1.h"
#include "StreetVendor2.h"
#include "StreetVendor3.h"
#include "StreetVendor4.h"
#include "StreetVendor5.h"
#include "VendorSelectBar.h"
#include "VendorText.h"
#include "MiniMonster.h"
#include "GolemName.h"
#include "RiddleName.h"
#include "KumaName.h"
#include "ChaosName.h"
#include "ShopMoney.h"
#include "ItemUseEffect.h"
#include "ItemUseEffect1.h"
#include "ItemUseEffect2.h"
#include "ItemUseEffect3.h"
#include "NumItemEffect.h"
#include "PartyBuffText.h"
#include "BuffStr.h"
#include "BuffHp.h"
#include "BuffHpNum.h"
#include "BuffStrNum.h"
#include "ShopUseMoney.h"
#include "NumBuy2.h"
#include "NumWeapon.h"
#include "NumWeapon1.h"
#include "NumWeapon2.h"
#include "NumResultCombo.h"
#include "NumResultBonus.h"
#include "NumResult.h"
#include "ShopBuyBackground.h"
#include "ShopSellBackground.h"
#include "ShopUpgradeBackground.h"
#include "ShopExitBackground.h"
#include "ShopUpgradeCount.h"
#include "WeaponLevelUp1.h"
#include "ShopSign.h"
#include "LevelUpEffect.h"
#include "Clue1.h"
#include "ReserveEffect.h"
#include "ShopMoneyTexture.h"
#include "ShopEffect0.h"
#include "ShopDark.h"
#include "ShopWhite.h"
#include "ShopLvMax.h"
#include "ShopLvMax3.h"
#include "ShopLvMax4.h"
#include "MiniMinimap.h"
#include "MiniHockey.h"
#include "UpgradeTexture.h"
#include "UpgradeScene.h"
#include "UpgradeSceneFail.h"
#include "UpgradeSceneSuccess.h"
#include "ShopDark1.h"
#include "UpgradeFail.h"
#include "TalkButton.h"
#include "UpgradeBase.h"
#include "UpgradePer.h"
#include "UpgradeWaper.h"
#include "UpgradeBlackOut.h"
#include "UpgradeName.h"
#include "NumWaper.h"
#include "BuffEffect.h"
#include "BuffEffect2.h"
#include "Clue2.h"
#include "Blind.h"
#include "ItemEffect.h"
#include "ItemEffect1.h"
#include "ItemEffect2.h"
#include "ItemEffect3.h"
#include "MapName1.h"
#include "MapName2.h"
#include "OpenOX.h"
#include "OpenOX2.h"
#include "OpenOX3.h"
#include "OpenOX4.h"
#include "OpenOX5.h"
#include "OpenOX6.h"
#include "OpenOX7.h"
#include "MarvelTime.h"
#include "MarvelTimeBackground.h"
#include "Meg.h"
#include "Ink.h"

#include "BearType.h"
#include "PunchScore.h"
#include "PunchScore2.h"
#include "PunchScore3.h"
#include "PunchScore4.h"
#include "PunchTime.h"
#include "PunchTime2.h"
#include "BlockScore.h"
#include "BlockScore2.h"
#include "CraneScene0.h"
#include "CraneScene1.h"
#include "CraneScene2.h"
#include "CapsuleScene.h"
#include "MapNameEffect.h"

#include "ShopBattleText0.h"

#include "DropItemUI.h"
#include "PS01_STNumber.h"
#include "PS03_STNumber.h"
#include "PS04_STNumber.h"
#include "HP.h"
#include "EF.h"
#include "StoryQuest_BackGround1.h"
#include "StoryQuest_BackGround2.h"
#include "StoryQuest_BackGround3.h"
#include "StoryQuest_Point.h"
#include "StoryQuest_Contents.h"
#include "Burst_BackGround.h"
#include "Burst_Bar.h"
#include "Burst_BarBG.h"
#include "Burst_Num.h"
#include "Burst_Text.h"
#include "Burst_Effect.h"
#include "Burst_Effect2.h"
#include "EP_Battle.h"
#include "Cross_Item.h"
#include "Cross_Item_0.h"
#include "Cross_Item_1.h"
#include "Cross_Item_2.h"
#include "Cross_Item_3.h"
#include "Team_Player01_HP.h"
#include "Team_Player01_SP.h"
#include "Team_Player03_HP.h"
#include "Team_Player03_SP.h"
#include "Team_Player04_HP.h"
#include "Team_Player04_SP.h"
#include "Team_SP.h"
#include "Team_SP_Number.h"
#include "PartyOn_BackGround.h"
#include "PartyOn_BackGround2.h"
#include "PartyOn_Contents.h"
#include "Mon_HP.h"
#include "Mon_HP_Bar.h"
#include "Mon_HP2.h"
#include "Mon_HP2_Bar.h"
#include "Mon_PG_Bar.h"
#include "Boss_HP.h"
#include "Boss_HP_Bar.h"
#include "Boss_PG_Bar.h"
#include "Boss_Name.h"
#include "Boss_HP_Number.h"
#include "Bear_HP.h"
#include "Bear_HP_Bar.h"
#include "Bear_PG_Bar.h"
#include "Bear_Name.h"
#include "TeamOn_Effect.h"
#include "TeamOn_Effect2.h"
#include "Coin_BackGround.h"
#include "Coin_BackGround2.h"
#include "Coin_BackGround3.h"
#include "Coin_Icon.h"
#include "Coin_Num.h"
#include "Coin_NowNum.h"
#include "Coin_Effect.h"
#include "NPC_Line00.h"
#include "NPC_Line01.h"
#include "NPC_Line02.h"
#include "NPC_Line03.h"
#include "NPC_Line04.h"
#include "NPC_Line05.h"
#include "NPC_Line06.h"
#include "NPC_Line07.h"
#include "NPC_Line08.h"

#include "DialogProfile.h"
#include "DialogTalk.h"
#include "ActiveQuest.h"
#include "QuestDist.h"
#include "BattleDialog.h"

#include "HockeyReserve.h"
#include "Bear.h"
#include "Bear1.h"
#include "Bear2.h"
#include "Bear3.h"
#include "Bear4.h"
#include "Tower.h"
#include "TowerRail.h"
#include "Crane0.h"
#include "Crane1.h"
#include "Crane2.h"
#include "Capsule.h"
#include "Block.h"
#include "DumiBlock.h"
#include "Stage4Roof.h"
#include "BlockCuma.h"
#include "CutBlock.h"
#include "MiniGame_Block_Triger.h"
#include "FunchCuma.h"
#include "BallDownTrigger.h"
#include "BallDownCuma.h"
#include "QPotal.h"
#include "EPotal.h"
#include "SenterPotal.h"
#include "AzitEvent.h"
#include "AzitEvent_1.h"
#include "AzitEvent_2.h"
#include "AzitEvent_3.h"
#include "AzitEvent_4.h"

// Test
#include "Test.h"
#include "Test1.h"
#include "Shader_Tool.h"
#include "Model_ObjTool.h"
#include "PxBox.h"
#include "ModelObj_Instance.h"

#include "CutScene_Tool.h"
#include "CamSelectingCube.h"
#include "LookCube_HR.h"
#include "MarkCube.h"
#include "RenderCube.h"
#include "ColorCube.h"


CLoader::CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

_uint APIENTRY LoadingMain(void* pArg)
{
	CoInitializeEx(nullptr, 0);

	CLoader*		pLoader = (CLoader*)pArg;

	EnterCriticalSection(&pLoader->Get_CS());

	switch (pLoader->Get_NextLevelID())
	{
	case LEVEL_LOAD:
		if (FAILED(pLoader->Loading_ForStaticLevel()))
			MSG_BOX(L"FailCreate");
		break;
	case LEVEL_LOGO:
		pLoader->Loading_ForLogoLevel();
		break;
	case LEVEL_TEST:
		pLoader->Loading_ForTestLevel();
		break;
	case LEVEL_TOOL:
		pLoader->Loading_ForToolLevel();
		break;
	case LEVEL_STAGE1:
		pLoader->Loading_ForStage1Level();
		break;
	case LEVEL_STAGE2:
		pLoader->Loading_ForStage2Level();
		break;
	case LEVEL_STAGE3:
		pLoader->Loading_ForStage3Level();
		break;
	case LEVEL_STAGE4:
		pLoader->Loading_ForStage4Level();
		break;
	case LEVEL_STAGE5:
		pLoader->Loading_ForStage5Level();
		break;



	//case LEVEL_GAMEPLAY:
	//	pLoader->Loading_ForGamePlayLevel();
	//	break;
	}

	LeaveCriticalSection(&pLoader->Get_CS());

	return 0;
}

HRESULT CLoader::Initialize(LEVEL eNextLevelID)
{
	m_eNextLevelID = eNextLevelID;

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, LoadingMain, this, 0, nullptr);

	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader::Loading_ForStaticLevel()
{

#pragma region 컴포넌트
	lstrcpy(m_szLoadingText, TEXT("컴포넌트를 생성하는 중입니다."));

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle"),
		CVIBuffer_Particle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Particle_Instance"),
		CVIBuffer_Particle_Instance::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("SwordTrail"),
		CSwordTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("PointTrail"),
		CPointTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_AABB */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		CAABB::Create(m_pDevice, m_pContext, CCollider::TYPE_AABB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_OBB */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		COBB::Create(m_pDevice, m_pContext, CCollider::TYPE_OBB))))
		return E_FAIL;

	/* For.Prototype_Component_Collider_Sphere */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Sphere"),
		CSphere::Create(m_pDevice, m_pContext, CCollider::TYPE_SPHERE))))
		return E_FAIL;

	/* For.Prototype_Component_Rigid */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Rigid"),
		CRigid::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_Component_Rigid_Sphere */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Rigid_Sphere"),
		CRigid_Sphere::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("PxObj"),
		CPxObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Px_Tool"),
		CPx_Tool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Shader_Tool"),
		CShader_Tool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Model_Tool"),
		CModel_ObjTool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("CutScene_Tool"),
		CCutScene_Tool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	PTM->Initalize();

#pragma endregion 컴포넌트


#pragma region 셰이더
	lstrcpy(m_szLoadingText, TEXT("셰이더를 생성하는 중입니다."));

	/* For.Prototype_Component_Shader_ColorCube */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ColorCube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Vtx.hlsl"), VTX_DECLARATION::Elements, VTX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Cube */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Cube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBETEX_DECLARATION::Elements, VTXCUBETEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Terrain */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Terrain"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_Model */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_AnimModel */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	///* For.Prototype_Component_Shader_Cube */
	//if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Cube"),
	//	CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxCubeTex.hlsl"), VTXCUBETEX_DECLARATION::Elements, VTXCUBETEX_DECLARATION::iNumElements))))
	//	return E_FAIL;

	/* For.Prototype_Component_Shader_VtxTex */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxTex.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_SwordTrail"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_SwordTrail.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ModelTool"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_ModelTool.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;
	
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AlphaParticle"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_AlphaParticle.hlsl"), VTXPOINT_DECLARATION::Elements, VTXPOINT_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_PointTrail"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_PointTrail.hlsl"), VTXPOINT_DECLARATION::Elements, VTXPOINT_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_ParticleInstance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_ParticleInstance.hlsl"), PARTICLEINSTANCE_DECLARATION::Elements, PARTICLEINSTANCE_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Player04TextureEffect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Player04TextureEffect.hlsl"), VTXPOINT_DECLARATION::Elements, VTXPOINT_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Player04MeshEffect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Player04MeshEffect.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Player01TextureEffect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Player01TextureEffect.hlsl"), VTXPOINT_DECLARATION::Elements, VTXPOINT_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Player01MeshEffect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Player01MeshEffect.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_StaticMeshEffect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_StaticMeshEffect.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Player03MeshEffect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Player03MeshEffect.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Player03TextureEffect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_Player03TextureEffect.hlsl"), VTXPOINT_DECLARATION::Elements, VTXPOINT_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_EffectSpin"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_EffectSpin.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model_Emissive"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel_Emissive.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_AnimModel_Emissive"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel_Emissive.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MonsterMeshEffect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_MonsterMeshEffect.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Model_Instance"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel_Instance.hlsl"), VTXMODELINSTANCE_DECLARATION::Elements, VTXMODELINSTANCE_DECLARATION::iNumElements))))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_BossTextureEffect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_BossTextureEffect.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_MonsterTextureEffect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_MonsterTextureEffect.hlsl"), VTXPOINT_DECLARATION::Elements, VTXPOINT_DECLARATION::iNumElements))))
		return E_FAIL;

#pragma endregion 셰이더



#pragma region 프레임워크 객체원형
	lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩중입니다. "));

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Camera_Main"),
		CCamera_Main::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("PointLight_Tool"),
		CPointLight_Tool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("PointTrailObj"),
		CPointTrailObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("SwordTrailObj"),
		CSwordTrailObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion 프레임워크 객체원형



#pragma region UI 객체원형
	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Test"),
		CTest::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Test1"),
		CTest1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MenuBackGround"),
		CMenuBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MenuBar"),
		CMenuBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_IllustYumo"),
		CIllustYumo::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_IllustLeviada"),
		CIllustLeviada::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_IllustDD"),
		CIllustDD::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MenuGuide"),
		CMenuGuide::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Player_Status01"),
		CPlayer_Status01::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Player_Status03"),
		CPlayer_Status03::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Player_Status04"),
		CPlayer_Status04::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Cross"),
		CCross::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Cross_Point"),
		CCross_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MenuNum1"),
		CMenuNum1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MenuNum2"),
		CMenuNum2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MenuNum3"),
		CMenuNum3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_BattleTag"),
		CBattleTag::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MenuLevelUp"),
		CMenuLevelUp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MenuLearnSkills"),
		CMenuLearnSkills::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MenuEquipMent"),
		CMenuEquipMent::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MenuItems"),
		CMenuItems::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_LevelUpBackGround"),
		CLevelUpBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_LearnSkillsBackGround"),
		CLearnSkillsBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_EquipMentBackGround"),
		CEquipMentBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ItemsBackGround"),
		CItemsBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_DuskDiverBackGround"),
		CDuskDiverBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_LevelUpBase"),
		CLevelUpBase::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_EscBase"),
		CEscBase::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_LevelUpBar"),
		CLevelUpBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Team_Player01"),
		CTeam_Player01::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Team_Player01_HP"),
		CTeam_Player01_HP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Team_Player01_SP"),
		CTeam_Player01_SP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Team_Player03"),
		CTeam_Player03::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Team_Player03_HP"),
		CTeam_Player03_HP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Team_Player03_SP"),
		CTeam_Player03_SP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Team_Player04"),
		CTeam_Player04::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Team_Player04_HP"),
		CTeam_Player04_HP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Team_Player04_SP"),
		CTeam_Player04_SP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Team_bg"),
		CTeam_bg::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_LevelUpGuide"),
		CLevelUpGuide::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_LevelUpSelectBackGround1"),
		CLevelUpSelectBackGround1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_LevelUpSelectBackGround2"),
		CLevelUpSelectBackGround2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_LevelUpSelectBackGround3"),
		CLevelUpSelectBackGround3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_LevelUpSelectCharacter1"),
		CLevelUpSelectCharacter1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_LevelUpSelectCharacter2"),
		CLevelUpSelectCharacter2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_LevelUpSelectCharacter3"),
		CLevelUpSelectCharacter3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_LevelUpName1"),
		CLevelUpName1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_LevelUpName2"),
		CLevelUpName2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_LevelUpName3"),
		CLevelUpName3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_StatusBase"),
		CStatusBase::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_StatusBar"),
		CStatusBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_StatusBackGround"),
		CStatusBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_StatusWindow"),
		CStatusWindow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_BlockMax"),
		CBlockMax::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_KumaImage"),
		CKumaImage::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_StatusGuide"),
		CStatusGuide::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_StatusSelectBar"),
		CStatusSelectBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Hit"),
		CHit::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_BearType"),
		CBearType::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_BlockM"),
		CBlockM::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_PunchText"),
		CPunchText::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_PunchScoreText"),
		CPunchScoreText::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Hit_Bar"),
		CHit_Bar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Cross_Info"),
		CCross_Info::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_LockOn_Info"),
		CLockOn_Info::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_StatusStr"),
		CStatusStr::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_StatusStamina"),
		CStatusStamina::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_StatusHp"),
		CStatusHp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_StatusMental"),
		CStatusMental::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_StatusMeta"),
		CStatusMeta::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_StatusExplosive"),
		CStatusExplosive::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_StatusLuck"),
		CStatusLuck::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Task_BackGround"),
		CTask_BackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Task_BackGround2"),
		CTask_BackGround2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Task_Title"),
		CTask_Title::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Task_Contents"),
		CTask_Contents::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_StoryQuest_BackGround1"),
		CStoryQuest_BackGround1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_StoryQuest_BackGround2"),
		CStoryQuest_BackGround2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_StoryQuest_BackGround3"),
		CStoryQuest_BackGround3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_StoryQuest_Point"),
		CStoryQuest_Point::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_StoryQuest_Contents"),
		CStoryQuest_Contents::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_StatIcon"),
		CStatIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Player1StrLv"),
		CPlayer1StrLv::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Player1HpLv"),
		CPlayer1HpLv::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Player1ExplosiveLv"),
		CPlayer1ExplosiveLv::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Player1LuckLv"),
		CPlayer1LuckLv::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Player1MentalLv"),
		CPlayer1MentalLv::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Player1MetaLv"),
		CPlayer1MetaLv::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Player1StaminaLv"),
		CPlayer1StaminaLv::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ReserveBar"),
		CReserveBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Player1Icon"),
		CPlayer1Icon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Player2Icon"),
		CPlayer2Icon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Player3Icon"),
		CPlayer3Icon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ShopLvMax"),
		CShopLvMax::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ShopLvMax3"),
		CShopLvMax3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ShopLvMax4"),
		CShopLvMax4::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Hunger"),
		CHunger::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_FoodSlot0"),
		CFoodSlot0::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_FoodSlot1"),
		CFoodSlot1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_FoodSlot2"),
		CFoodSlot2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_FoodSlot3"),
		CFoodSlot3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_FoodSlot4"),
		CFoodSlot4::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_FoodSlot5"),
		CFoodSlot5::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_FoodSlot6"),
		CFoodSlot6::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_EatText"),
		CEatText::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_StreetVendor1"),
		CStreetVendor1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_StreetVendor2"),
		CStreetVendor2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_StreetVendor3"),
		CStreetVendor3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_StreetVendor4"),
		CStreetVendor4::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_StreetVendor5"),
		CStreetVendor5::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_VendorSelectBar"),
		CVendorSelectBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_VendorText"),
		CVendorText::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MiniMonster"),
		CMiniMonster::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MiniMapUpside"),
		CMiniMapUpside::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MiniMapBase"),
		CMiniMapBase::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_BossBase"),
		CBossBase::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_SkullBase"),
		CSkullBase::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_SkullLight"),
		CSkullLight::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_SkullVanish"),
		CSkullVanish::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Warning"),
		CWarning::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ThunderBase0"),
		CThunderBase0::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ThunderBase1"),
		CThunderBase1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ThunderBase2"),
		CThunderBase2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ThunderBase3"),
		CThunderBase3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ThunderBase4"),
		CThunderBase4::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ThunderBase5"),
		CThunderBase5::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NumResultExp"),
		CNumResultExp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NumResultCombo"),
		CNumResultCombo::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NumResultBonus"),
		CNumResultBonus::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NumResult"),
		CNumResult::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_UpgradeBase"),
		CUpgradeBase::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_UpgradePer"),
		CUpgradePer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_UpgradeWaper"),
		CUpgradeWaper::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_UpgradeBlackOut"),
		CUpgradeBlackOut::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_UpgradeName"),
		CUpgradeName::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NumWaper"),
		CNumWaper::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_BuffEffect"),
		CBuffEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_BuffEffect2"),
		CBuffEffect2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Clue2"),
		CClue2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Blind"),
		CBlind::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ItemEffect"),
		CItemEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ItemEffect1"),
		CItemEffect1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ItemEffect2"),
		CItemEffect2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ItemEffect3"),
		CItemEffect3::Create(m_pDevice, m_pContext))))
		return E_FAIL;




	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Thunder0"),
		CThunder0::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Thunder1"),
		CThunder1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Thunder2"),
		CThunder2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Thunder3"),
		CThunder3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Thunder4"),
		CThunder4::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Thunder5"),
		CThunder5::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MenuNumber1"),
		CMenuNumber1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MenuNumber2"),
		CMenuNumber2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MenuNumber3"),
		CMenuNumber3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ItemUseEffect"),
		CItemUseEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MiniHockey"),
		CMiniHockey::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_BattleEnd"),
		CBattleEnd::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_BattleEndBase"),
		CBattleEndBase::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ItemUseEffect1"),
		CItemUseEffect1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ItemUseEffect2"),
		CItemUseEffect2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ItemUseEffect3"),
		CItemUseEffect3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NumBuy2"),
		CNumBuy2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_DropItemUI"),
		CDropItemUI::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NumWeapon"),
		CNumWeapon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NumWeapon1"),
		CNumWeapon1::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NumWeapon2"),
		CNumWeapon2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_UpgradeFail"),
		CUpgradeFail::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_PlayerCircle"),
		CPlayerCircle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_UI_Number"),
		CUI_Number::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_UI_Number2"),
		CUI_Number2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NumExp"),
		CNumExp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_PunchScore"),
		CPunchScore::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_PunchTime"),
		CPunchTime::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_PunchTime2"),
		CPunchTime2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NumReserveExp"),
		CNumReserveExp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NumLevel"),
		CNumLevel::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_UpgradeTexture"),
		CUpgradeTexture::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_UpgradeScene"),
		CUpgradeScene::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_UpgradeSceneFail"),
		CUpgradeSceneFail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_UpgradeSceneSuccess"),
		CUpgradeSceneSuccess::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NumHp"),
		CNumHp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NumMental"),
		CNumMental::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NumMeta"),
		CNumMeta::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NumStr"),
		CNumStr::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NumStamina"),
		CNumStamina::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NumExplosive"),
		CNumExplosive::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NumLuck"),
		CNumLuck::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_TalkButton"),
		CTalkButton::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_StatusLT1"),
		CStatusLT1::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_LT1Level"),
		CLT1Level::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_StatusLT2"),
		CStatusLT2::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_LT2Level"),
		CLT2Level::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_StatusLT3"),
		CStatusLT3::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_LT3Level"),
		CLT3Level::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_LT3HP"),
		CLT3HP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_LT2HP"),
		CLT2HP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_LT1HP"),
		CLT1HP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_IllustDDHp"),
		CIllustDDHp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_IllustDDLv"),
		CIllustDDLv::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_IllustDDSp"),
		CIllustDDSp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_IllustDDAttack"),
		CIllustDDAttack::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_IllustDDArmor"),
		CIllustDDArmor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_IllustLeviadaHp"),
		CIllustLeviadaHp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_IllustLeviadaLv"),
		CIllustLeviadaLv::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_IllustLeviadaSp"),
		CIllustLeviadaSp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_IllustLeviadaAttack"),
		CIllustLeviadaAttack::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_IllustLeviadaAmor"),
		CIllustLeviadaAmor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_IllustYumoSp"),
		CIllustYumoSp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_IllustYumoHp"),
		CIllustYumoHp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_IllustYumoLv"),
		CIllustYumoLv::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_IllustYumoAmor"),
		CIllustYumoAmor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_EquipmentText"),
		CEquipmentText::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_SelectSlot0"),
		CSelectSlot0::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_SelectSlot1"),
		CSelectSlot1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_SelectSlot2"),
		CSelectSlot2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_SelectSlot3"),
		CSelectSlot3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_SelectImage0"),
		CSelectImage0::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_SelectImage1"),
		CSelectImage1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_SelectImage2"),
		CSelectImage2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_SelectImage3"),
		CSelectImage3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_GolemName"),
		CGolemName::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_RiddleName"),
		CRiddleName::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_KumaName"),
		CKumaName::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ChaosName"),
		CChaosName::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ShopMoney"),
		CShopMoney::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ShopBuyBackground"),
		CShopBuyBackground::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ShopSellBackground"),
		CShopSellBackground::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ShopUpgradeBackground"),
		CShopUpgradeBackground::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ShopExitBackground"),
		CShopExitBackground::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ShopUpgradeCount"),
		CShopUpgradeCount::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_WeaponLevelUp1"),
		CWeaponLevelUp1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ShopSign"),
		CShopSign::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_IllustYumoAttack"),
		CIllustYumoAttack::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_EquipmentUpside"),
		CEquipmentUpside::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_EquipmentBase"),
		CEquipmentBase::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_EquipmentCase"),
		CEquipmentCase::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_EquipmentBox"),
		CEquipmentBox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_EquipmentStatus"),
		CEquipmentStatus::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_EquipmentGuide"),
		CEquipmentGuide::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_EquipmentSelectColor"),
		CEquipmentSelectColor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_EquipmentSelectCase"),
		CEquipmentSelectCase::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_EquipmentSelectBar"),
		CEquipmentSelectBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_EquipmentWeaponBackground"),
		CEquipmentWeaponBackground::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_EquipmentHatBackground"),
		CEquipmentHatBackground::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_EquipmentClothesBackground"),
		CEquipmentClothesBackground::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_EquipmentDecoBackground"),
		CEquipmentDecoBackground::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_HatSlot1"),
		CHatSlot1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_HatSlot2"),
		CHatSlot2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_HatSlot3"),
		CHatSlot3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_HatSlot4"),
		CHatSlot4::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_HatSlot5"),
		CHatSlot5::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_HatSlot6"),
		CHatSlot6::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_HatSlot7"),
		CHatSlot7::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ClothesSlot1"),
		CClothesSlot1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ClothesSlot2"),
		CClothesSlot2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ClothesSlot3"),
		CClothesSlot3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ClothesSlot4"),
		CClothesSlot4::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ClothesSlot5"),
		CClothesSlot5::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ClothesSlot6"),
		CClothesSlot6::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ClothesSlot7"),
		CClothesSlot7::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MarvelTime"),
		CMarvelTime::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MarvelTimeBackground"),
		CMarvelTimeBackground::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Meg"),
		CMeg::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_DecoSlot1"),
		CDecoSlot1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_DecoSlot2"),
		CDecoSlot2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_DecoSlot3"),
		CDecoSlot3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Ink"),
		CInk::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_DecoSlot4"),
		CDecoSlot4::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_DecoSlot5"),
		CDecoSlot5::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_DecoSlot6"),
		CDecoSlot6::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_DecoSlot7"),
		CDecoSlot7::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_PictureBurst"),
		CPictureBurst::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Player1EquipmentEHat"),
		CPlayer1EquipmentEHat::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_EquipmentStr"),
		CEquipmentStr::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_EquipmentStamina"),
		CEquipmentStamina::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_EquipmentHp"),
		CEquipmentHp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_EquipmentMeta"),
		CEquipmentMeta::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_EquipmentMental"),
		CEquipmentMental::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_EquipmentExplosive"),
		CEquipmentExplosive::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_EquipmentLuck"),
		CEquipmentLuck::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_EquipmentLv"),
		CEquipmentLv::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Equipment1Info"),
		CEquipment1Info::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Equipment2Info"),
		CEquipment2Info::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Equipment3Info"),
		CEquipment3Info::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Player2EquipmentE"),
		CPlayer2EquipmentE::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Player3EquipmentE"),
		CPlayer3EquipmentE::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Dash_Info"),
		CDash_Info::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Dodge_Info"),
		CDodge_Info::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_DDStart"),
		CDDStart::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_PS01_Number"),
		CPS01_Number::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_PS03_Number"),
		CPS03_Number::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_PS04_Number"),
		CPS04_Number::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_PS01_STNumber"),
		CPS01_STNumber::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_PS03_STNumber"),
		CPS03_STNumber::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_PS04_STNumber"),
		CPS04_STNumber::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MinimapTest"),
		CMinimapTest::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_EquipMentPriviousBase"),
		CEquipMentPriviousBase::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_EquipMentPriviousBar"),
		CEquipMentPriviousBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InvenQ"),
		CInvenQ::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InvenE"),
		CInvenE::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InvenBattleItem"),
		CInvenBattleItem::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InvenDropItem"),
		CInvenDropItem::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InvenEquipment"),
		CInvenEquipment::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MiniMinimap"),
		CMiniMinimap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InvenFood"),
		CInvenFood::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InvenQuestItem"),
		CInvenQuestItem::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InvenEquipmentBackground"),
		InvenEquipmentBackground::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InvenBattleItemBackground"),
		CInvenBattleItemBackground::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InvenFoodBackground"),
		CInvenFoodBackground::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InvenQuestItemBackground"),
		CInvenQuestItemBackground::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_DropItemSlot0"),
		CDropItemSlot0::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_DropItemSlot1"),
		CDropItemSlot1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_DropItemSlot2"),
		CDropItemSlot2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_DropItemSlot3"),
		CDropItemSlot3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_DropItemSlot4"),
		CDropItemSlot4::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_DropItemSlot5"),
		CDropItemSlot5::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_BattleItemSlot0"),
		CBattleItemSlot0::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_BattleItemSlot1"),
		CBattleItemSlot1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_BattleItemSlot2"),
		CBattleItemSlot2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_BattleItemSlot3"),
		CBattleItemSlot3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_BattleItemSlot4"),
		CBattleItemSlot4::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_BattleItemSlot5"),
		CBattleItemSlot5::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_BattleItemSlot6"),
		CBattleItemSlot6::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Slot_Num"),
		CSlot_Num::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ShopBase"),
		CShopBase::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ShopUpside"),
		CShopUpside::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ShopBuySelect"),
		CShopBuySelect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ShopSellSelect"),
		CShopSellSelect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ShopUpgradeSelect"),
		CShopUpgradeSelect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ShopExitSelect"),
		CShopExitSelect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ShopWindow"),
		CShopWindow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ShopSelectBar"),
		CShopSelectBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ShopBuyCount"),
		CShopBuyCount::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ShopSellCount"),
		CShopSellCount::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NumBuy"),
		CNumBuy::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MiniPlayer"),
		CMiniPlayer::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ShopBattleText0"),
		CShopBattleText0::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InvenEquipSlot0"),
		CInvenEquipSlot0::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InvenEquipSlot1"),
		CInvenEquipSlot1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InvenEquipSlot2"),
		CInvenEquipSlot2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InvenEquipSlot3"),
		CInvenEquipSlot3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InvenEquipSlot4"),
		CInvenEquipSlot4::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InvenEquipSlot5"),
		CInvenEquipSlot5::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InvenEquipSlot6"),
		CInvenEquipSlot6::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InvenEquipSlot7"),
		CInvenEquipSlot7::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InvenEquipSlot8"),
		CInvenEquipSlot8::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InvenEquipSlot9"),
		CInvenEquipSlot9::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InvenEquipSlot10"),
		CInvenEquipSlot10::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InvenEquipSlot11"),
		CInvenEquipSlot11::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InvenEquipSlot12"),
		CInvenEquipSlot12::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InvenEquipSlot13"),
		CInvenEquipSlot13::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InvenEquipSlot14"),
		CInvenEquipSlot14::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InventorySelectBar"),
		CInventorySelectBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_TutorialMinimap"),
		CTutorialMinimap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MiniQuest"),
		CMiniQuest::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_PunchScore2"),
		CPunchScore2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_PunchScore3"),
		CPunchScore3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_PunchScore4"),
		CPunchScore4::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_BlockScore"),
		CBlockScore::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_BlockScore2"),
		CBlockScore2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ShopMoneyTexture"),
		CShopMoneyTexture::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MiniBossMonster"),
		CMiniBossMonster::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ShopEffect0"),
		CShopEffect0::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ShopDark"),
		CShopDark::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ShopDark1"),
		CShopDark1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ShopWhite"),
		CShopWhite::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_PartyBuffText"),
		CPartyBuffText::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_BuffStr"),
		CBuffStr::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_BuffHp"),
		CBuffHp::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_BuffHpNum"),
		CBuffHpNum::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_BuffStrNum"),
		CBuffStrNum::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ShopUseMoney"),
		CShopUseMoney::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_LevelUpEffect"),
		CLevelUpEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MapName"),
		CMapName::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("LastBlock"),
		CLastBlock::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("UpPotal"),
		CUpPotal::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("DownPotal"),
		CDownPotal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MinimapBackground"),
		CMinimapBackground::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_NumItemEffect"),
		CNumItemEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MinimapStage1"),
		CMinimapStage1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MaskMap"),
		CMaskMap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Clue1"),
		CClue1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ReserveEffect"),
		CReserveEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InventoryBase"),
		CInventoryBase::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InventoryUpside"),
		CInventoryUpside::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_InventoryWindow"),
		CInventoryWindow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_SkillTextBasic"),
		CSkillTextBasic::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Player_Status01_HP"),
		CPlayer_Status01_HP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Player_Status01_SP"),
		CPlayer_Status01_SP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Player_Status01_BP"),
		CPlayer_Status01_BP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Player_Status03_HP"),
		CPlayer_Status03_HP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Player_Status03_SP"),
		CPlayer_Status03_SP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Player_Status03_BP"),
		CPlayer_Status03_BP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Player_Status04_HP"),
		CPlayer_Status04_HP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Player_Status04_SP"),
		CPlayer_Status04_SP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Player_Status04_BP"),
		CPlayer_Status04_BP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_HP"),
		CHP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_EF"),
		CEF::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_EP_Battle"),
		CEP_Battle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_DialogProfile"),
		CDialogProfile::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_DialogTalk"),
		CDialogTalk::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_BattleDialog"),
		CBattleDialog::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ActiveQuest"),
		CActiveQuest::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_QuestDist"),
		CQuestDist::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Burst_BackGround"),
		CBurst_BackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Burst_Bar"),
		CBurst_Bar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Burst_BarBG"),
		CBurst_BarBG::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Burst_Num"),
		CBurst_Num::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Burst_Text"),
		CBurst_Text::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Burst_Effect"),
		CBurst_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Burst_Effect2"),
		CBurst_Effect2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Cross_Item"),
		CCross_Item::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MapNameEffect"),
		CMapNameEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MapName1"),
		CMapName1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MapName2"),
		CMapName2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_OpenOX"),
		COpenOX::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_OpenOX2"),
		COpenOX2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_OpenOX3"),
		COpenOX3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_OpenOX4"),
		COpenOX4::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_OpenOX5"),
		COpenOX5::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_OpenOX6"),
		COpenOX6::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_OpenOX7"),
		COpenOX7::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Cross_Item_0"),
		CCross_Item_0::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Cross_Item_1"),
		CCross_Item_1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Cross_Item_2"),
		CCross_Item_2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Cross_Item_3"),
		CCross_Item_3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Team_SP"),
		CTeam_SP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Team_SP_Number"),
		CTeam_SP_Number::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_TeamOn_Effect"),
		CTeamOn_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_TeamOn_Effect2"),
		CTeamOn_Effect2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_PartyOn_BackGround"),
		CPartyOn_BackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_PartyOn_BackGround2"),
		CPartyOn_BackGround2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_PartyOn_Contents"),
		CPartyOn_Contents::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Boss_HP"),
		CBoss_HP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Boss_HP_Bar"),
		CBoss_HP_Bar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Boss_PG_Bar"),
		CBoss_PG_Bar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Boss_Name"),
		CBoss_Name::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Boss_HP_Number"),
		CBoss_HP_Number::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Bear_HP"),
		CBear_HP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Bear_HP_Bar"),
		CBear_HP_Bar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Bear_PG_Bar"),
		CBear_PG_Bar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Bear_Name"),
		CBear_Name::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Coin_BackGround"),
		CCoin_BackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Coin_BackGround2"),
		CCoin_BackGround2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Coin_BackGround3"),
		CCoin_BackGround3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Coin_Icon"),
		CCoin_Icon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Coin_Num"),
		CCoin_Num::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Coin_NowNum"),
		CCoin_NowNum::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_Coin_Effect"),
		CCoin_Effect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("NPC_Line00"),
		CNPC_Line00::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("NPC_Line01"),
		CNPC_Line01::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("NPC_Line02"),
		CNPC_Line02::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("NPC_Line03"),
		CNPC_Line03::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("NPC_Line04"),
		CNPC_Line04::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("NPC_Line05"),
		CNPC_Line05::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("NPC_Line06"),
		CNPC_Line06::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("NPC_Line07"),
		CNPC_Line07::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("NPC_Line08"),
		CNPC_Line08::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

	if (FAILED(GI->Add_Prototype(TEXT("Holo_1"),
		CHolo_1::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("Holo_2"),
		CHolo_2::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("Holo_3"),
		CHolo_3::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("Holo_4"),
		CHolo_4::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("Holo_5"),
		CHolo_5::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("Holo_5_Blue"),
		CHolo_5_Blue::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("Holo_6"),
		CHolo_6::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("Holo_7"),
		CHolo_7::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("Holo_8"),
		CHolo_8::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("Holo_9"),
		CHolo_9::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("Holo_10"),
		CHolo_10::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("RealHolo_1"),
		CRealHolo_1::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("RealHolo_2"),
		CRealHolo_2::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("RealHolo_3"),
		CRealHolo_3::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("RealHolo_4"),
		CRealHolo_4::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("RealHolo_5"),
		CRealHolo_5::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion UI 객체원형


	
#pragma region Effect 객체원형
	Load_ForTexture();

	if (FAILED(GI->Add_Prototype(TEXT("Effect"),
		CEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("AlphaParticle"),
		CAlphaParticle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("ParticleInstance"),
		CParticleInstance::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player04VoidEffect_2"),
		CPlayer04VoidEffect_2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player01HitEffect_1"),
		CPlayer01HitEffect_1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player01HitEffect_2"),
		CPlayer01HitEffect_2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player01Sunder_1"),
		CPlayer01Sunder_1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player01Sunder_2"),
		CPlayer01Sunder_2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player04Slash"),
		CPlayer04Slash::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player03SniperEffect_1"),
		CPlayer03SniperEffect_1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Wall"),
		CWall::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player01Trail_1"),
		CPlayer01Trail_1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Wind"),
		CWind::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("EffectSpin"),
		CEffectSpin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("EffectAtom"),
		CEffectAtom::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("EffectShield"),
		CEffectShield::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("EffectShieldHit"),
		CEffectShieldHit::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Ring"),
		CRing::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("EffectCraw"),
		CEffectCraw::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(TEXT("BossClaw"),
		CBossClaw::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Tornado"),
		CTornado::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("ViewAlpha"),
		CViewAlpha::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("EffectOctopusRange"),
		CEffectOctopusRange::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("FusionThunder"),
		CFusionThunder::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("FusionTrail"),
		CFusionTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player01Wind1"),
		CPlayer01Wind1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player01Wind2"),
		CPlayer01Wind2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player01Ball"),
		CPlayer01Ball::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player01Ring"),
		CPlayer01Ring::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player01Pillar"),
		CPlayer01Pillar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player01Pillar2"),
		CPlayer01Pillar2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player01Pillar3"),
		CPlayer01Pillar3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player01Skill5Ball"),
		CPlayer01Skill5Ball::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("CircleDistortion"),
		CCircleDistortion::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player01Rage1Ball"),
		CPlayer01Rage1Ball::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(TEXT("EffectMissileRange"),
		CEffectMissileRange::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("MiniGame_Yabawe"),
		CMiniGame_Yabawe::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Yabawe"),
		CYabawe::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("RealYabawe"),
		CRealYabawe::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player01SmokeBall"),
		CPlayer01SmokeBall::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player01Metamor"),
		CPlayer01Metamor::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player01Landing"),
		CPlayer01Landing::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player01Upper"),
		CPlayer01Upper::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("EffectPatternGauge"),
		CEffectPatternGauge::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player03CannonBullet"),
		CPlayer03CannonBullet::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player03Bomb"),
		CPlayer03Bomb::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player04Slash2"),
		CPlayer04Slash2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player04SmokeBall"),
		CPlayer04SmokeBall::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player04DarkBall"),
		CPlayer04DarkBall::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("TornadoSide"),
		CTornadoSide::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player04Aura"),
		CPlayer04Aura::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player04VoidParticle"),
		CPlayer04VoidParticle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player04WhiteBall"),
		CPlayer04WhiteBall::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player04Rage1Particle"),
		CPlayer04Rage1Particle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player04Col"),
		CPlayer04Col::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("MonsterSpawn"),
		CMonsterSpawn::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("GolemWeaponRange"),
		CGolemWeaponRange::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(TEXT("GolemAttackRange"),
		CGolemAttackRange::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("BossPillar"),
		CBossPillar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("EffectPatternGaugeCircle"),
		CEffectPatternGaugeCircle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("BossAttackRange"),
		CBossAttackRange::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("BeastTornado"),
		CBeastTornado::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("ReserveTrail"),
		CReserveTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("BeastLaser"),
		CBeastLaser::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("BeastMarvelBall"),
		CBeastMarvelBall::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("BeastMarvelObstacle"),
		CBeastMarvelObstacle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("BeastMarvelPush"),
		CBeastMarvelPush::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("MarvelReserveTaril"),
		CMarvelReserveTaril::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("BeastTornadoSide"),
		CBeastTornadoSide::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("BeastBreath2"),
		CBeastBreath2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("BeastAura"),
		CBeastAura::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma endregion Effect 객체원형


#pragma region 정적모델 객체원형
	if (FAILED(GI->Add_Prototype(TEXT("Mesh"),
		CMesh::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("BeastMap"),
		CBeastMap::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("BeastMap2"),
		CBeastMap2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("BeastMap3"),
		CBeastMap3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Item"),
		CItem::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Skybox"),
		CSkybox::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Stage1"),
		CStage1::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("Stage1Light"),
		CStage1Light::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player01HairA"),
		CPlayer01HairA::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player01HairB"),
		CPlayer01HairB::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player01HairC"),
		CPlayer01HairC::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player03HairA"),
		CPlayer03HairA::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player03HairB"),
		CPlayer03HairB::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player03LGun"),
		CPlayer03LGun::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player03RGun"),
		CPlayer03RGun::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player03ShotGun"),
		CPlayer03ShotGun::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player03LShotGun"),
		CPlayer03LShotGun::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player03Cannon"),
		CPlayer03Cannon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player03LCannon"),
		CPlayer03LCannon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player03Sniper"),
		CPlayer03Sniper::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player04Hair"),
		CPlayer04Hair::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player04HairBattle"),
		CPlayer04HairBattle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player04Sword1"),
		CPlayer04Sword1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player04Sword2"),
		CPlayer04Sword2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player04SwordBack"),
		CPlayer04SwordBack::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("MonsterBullet"),
		CMonsterBullet::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(TEXT("YMTest"),
		CYMTest::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Bear"),
		CBear::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Bear1"),
		CBear1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Bear2"),
		CBear2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Bear3"),
		CBear3::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Bear4"),
		CBear4::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Tower"),
		CTower::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("TowerRail"),
		CTowerRail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Crane0"),
		CCrane0::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Crane1"),
		CCrane1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Crane2"),
		CCrane2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("CraneScene0"),
		CCraneScene0::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("CraneScene1"),
		CCraneScene1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("CraneScene2"),
		CCraneScene2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("CapsuleScene"),
		CCapsuleScene::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Capsule"),
		CCapsule::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Stage1Event1"),
		CStage1Event1::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("Stage1Event2"),
		CStage1Event2::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("Stage1Event3"),
		CStage1Event3::Create(m_pDevice, m_pContext))))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(TEXT("Stage2Event1"),
		CStage2Event1::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Stage2Event2"),
		CStage2Event2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Stage2Sound"),
		CStage2Sound::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Stage3Sound"),
		CStage3Sound::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("MonsterMissile"),
		CMonsterMissile::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("BossMissile"),
		CBossMissile::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Monster_HP"),
		CMon_HP::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Monster_HP_Bar"),
		CMon_HP_Bar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Monster_HP2"),
		CMon_HP2::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Monster_HP2_Bar"),
		CMon_HP2_Bar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Monster_PG_Bar"),
		CMon_PG_Bar::Create(m_pDevice, m_pContext))))
		return E_FAIL;
		
	if (FAILED(GI->Add_Prototype(TEXT("LightObj"),
		CLightObj::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("RollRock"),
		CRollRock::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("BossBomb"),
		CBossBomb::Create(m_pDevice, m_pContext))))
		return E_FAIL;
		
	if (FAILED(GI->Add_Prototype(TEXT("AnimMap_Tool"),
		CAnimMap_Tool::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("ModelObj_Instance"),
		CModelObj_Instance::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("DropItem"),
		CDropItem::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("CrystalBody"),
		CCrystalBody::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("CrystalPart"),
		CCrystalPart::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("CrystalFake"),
		CCrystalFake::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("MetalTrashbinTop"),
		CMetalTrashbinTop::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("MetalTrashbinBody"),
		CMetalTrashbinBody::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("Trash"),
		CTrash::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("RedWindow"),
		CRedWindow::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("CrackStone"),
		CCrackStone::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("Fissure"),
		CFissure::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_GameObject_CamSelectingCube */
	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_CamSelectingCube"),
		CCamSelectingCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_ColorCube */
	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_RenderCube"),
		CRenderCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_ColorCube */
	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_ColorCube"),
		CColorCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_MarkCube */
	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_MarkCube"),
		CMarkCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	/* For.Prototype_GameObject_LookCube */
	if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_LookCube"),
		CLookCube_HR::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Block"),
		CBlock::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("MiniGame_Block"),
		CMiniGame_Block::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("YavaweBall"),
		CYabaweBall::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("BeastStoneBody"),
		CBeastStoneBody::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("BeastStonePart"),
		CBeastStonePart::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("BeastBreath"),
		CBeastBreath::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Stage4Trigger_1"),
		CStage4Trigger_1::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("Stage4Trigger_2"),
		CStage4Trigger_2::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("Stage4Trigger_3"),
		CStage4Trigger_3::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("Stage4Trigger_4"),
		CStage4Trigger_4::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("Stage4Trigger_5"),
		CStage4Trigger_5::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("Stage4Trigger_6"),
		CStage4Trigger_6::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("Stage4Trigger_7"),
		CStage4Trigger_7::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("MiniGame_Funch_Trigger"),
		CMiniGame_Funch_Triger::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("Boss_Cuma"),
		CBoss_Cuma::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	
	if (FAILED(GI->Add_Prototype(TEXT("BeastClaw"),
		CBeastClaw::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("BeastPillar"),
		CBeastPillar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("DGCuma"),
		CDGCuma::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("DGCuma_1"),
		CDGCuma_1::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("DGCuma_2"),
		CDGCuma_2::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("DGCuma_3"),
		CDGCuma_3::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("DGTriggr"),
		CDGTrigger::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("DGEndCuma"),
		CDGEndCuma::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("BallingCuma"),
		CBallingCuma::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("SenterPotal"),
		CSenterPotal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("OctopusParticle"),
		COctopusParticle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("AzitEvent"),
		CAzitEvent::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("AzitEvent_1"),
		CAzitEvent_1::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("AzitEvent_2"),
		CAzitEvent_2::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("AzitEvent_3"),
		CAzitEvent_3::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(TEXT("AzitEvent_4"),
		CAzitEvent_4::Create(m_pDevice, m_pContext))))
		return E_FAIL;


#pragma endregion 정적모델 객체원형



#pragma region Anim모델 객체원형
	if (FAILED(GI->Add_Prototype(TEXT("AnimMesh"),
		CAnimMesh::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player_01"),
		CPlayer01::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player01MotionTrail"),
		CPlayer01MotionTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player_03"),
		CPlayer03::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player03MotionTrail"),
		CPlayer03MotionTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player_04"),
		CPlayer04::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Player04MotionTrail"),
		CPlayer04MotionTrail::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("TrainingRobot"),
		CTrainingRobot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("DisasterFusion"),
		CDisasterFusion::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("DisasterBeta"),
		CDisasterBeta::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("DisasterBoss"),
		CDisasterBoss::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Golem"),
		CGolem::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("DefenceRobot"),
		CDefenceRobot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("BombRobot"),
		CBombRobot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("NPCTutorial"),
		CNPCTutorial::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Stage2Player04"),
		CStage2Player04::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	
	if (FAILED(GI->Add_Prototype(TEXT("OldBombRobot"),
		COldBombRobot::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Rock"),
		CRock::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("Beast"),
		CBeast::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("MechanicalOctopus"),
		CMechanicalOctopus::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("MechanicalOctopusLeg"),
		CMechanicalOctopusLeg::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("NPC_T"),
		CNPC_T::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("BlockWin"),
		CBlockWin::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("DumiBlock"),
		CDumiBlock::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("BlockCuma"),
		CBlockCuma::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("CutBlock"),
		CCutBlock::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("MiniGame_Block_Triger"),
		CMiniGame_Block_Triger::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("FunchCuma"),
		CFunchCuma::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("BallDownTrigger"),
		CBallDownTrigger::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("BallDownCuma"),
		CBallDownCuma::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("QPotal"),
		CQPotal::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(TEXT("EPotal"),
		CEPotal::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	if (FAILED(GI->Add_Prototype(TEXT("SentorPotal"),
		CSenterPotal::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	

#pragma endregion Anim모델 객체원형



#pragma region 텍스처
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));

	/* For.Prototype_Component_Texture_Foxmask */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Test"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Test.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Foxmask */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Test1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Test1.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_MenuBackGround */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuBackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/MenuBackGround.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_MenuBar */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/MenuBar3.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_IllustYumo */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_IllustYumo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/Yumo2.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BearType"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Bear/B%d.png"), 4))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_IllustLeviada"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/Leviada2.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_IllustDD"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/DD2.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuGuide"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/MenuGuide.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuNum1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/StatusPick1.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuNum2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/StatusPick2.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MenuNum3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/StatusPick3.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BattleTag"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/BattleTag.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LevelUp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/Level_Up.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LearnSkills"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/Map.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Equipment"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/Equipment1.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Time"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Time.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_M"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/M.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_40"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/40.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Score"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Score.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Items"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/Items.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LevelUpBackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/Level_UpBackGround.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LearnSkillsBackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/MapBackGround.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EquipmentBackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/EquipmentBackGround.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ItemsBackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/ItemsBackGround.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DuskDiverBackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Menu/MovingDuskDiver.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LevelUpBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevelUp/LevelUpBar.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpgradeBase"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Upgrade/UpgradeBackground2.png"), 1))))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BattleItemType"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/BattleItemType%d.png"), 7))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpgradePer"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Upgrade/Per%d.png"), 6))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpgradeWaper"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Upgrade/Waper%d.png"), 6))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpgradeBlackOut"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Upgrade/AfterUpgrade.png")))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpgradeName"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Upgrade/Lv%d.png"), 21))))
		return E_FAIL;


	/* For.Prototype_Component_Texture_Player_Status */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Status01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/Player_Status001.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Status03"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/Player_Status003.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Status04"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/Player_Status004.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Status_HP"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/Player_Status_HP.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Status_SP"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/Player_Status_SP.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player_Status_BP"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/Player_Status_BP.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HP"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/HP.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EF"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/btn_burst.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Cross */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Cross00"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/Cross00.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Cross_Point"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/team_icon_cross.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Team_Player */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Team_Player01"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/Team_Player01.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Team_Player03"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/Team_Player03.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Team_Player04"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/Team_Player04.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Team_SP"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/team_sp.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Team_HP"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/team_hp.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Team_SP_X"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/team_sp_x.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Team_bg"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/Team_bg.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LevelUpGuide"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevelUp/LevelUpGuide.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LevelUpSelect1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevelUp/Select1.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LevelUpSelect2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevelUp/Select2.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LevelUpSelect3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevelUp/Select3.png"), 1))))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LevelUpSelectCharacter1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevelUp/Character1-%d.png"), 20))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LevelUpSelectCharacter2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevelUp/Character2-%d.png"), 20))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LevelUpSelectCharacter3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevelUp/Character3-%d.png"), 20))))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LevelUpName1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevelUp/Name1.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LevelUpName2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevelUp/Name2.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LevelUpName3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevelUp/Name3.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StatusBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevelUp/Stat/StatUpSide.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InvenQ"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/Q%d.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InvenE"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/E%d.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InvenBattleItem"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/BattleItem%d.png"), 2))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InvenDropItem"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/DropItem%d.png"), 2))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InvenEquipment"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/Equipment%d.png"), 2))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InvenFood"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/Food%d.png"), 2))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InvenQuestItem"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/QuestItem%d.png"), 2))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InvenEquipmentBackground"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/EquipmentBackground.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopBuyBackground"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/BuyBackGround.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InvenBattleItemBackground"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/ItemBackground%d.png"), 8))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InvenFoodBackground"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/Text%d.png"), 9))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InvenQuestItemBackground"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/QuestItemBackground.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DropItemType"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/DropItemType%d.png"), 6))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopUpside"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/ShopUpside.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopBuySelect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/Buy.png"), 1))))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopSellBackground"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/SellBackGround.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopUpgradeBackground"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/UpgradeWeaponBackGround.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopExitBackground"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/ExitBackGround.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopSellSelect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/Sell.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopUpgradeSelect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/Upgrade_Weapon.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopExitSelect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/Exit.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopWindow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/ShopWindow%d.png"), 5))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopSelectBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/SelectList.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopBuyCount"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/BuyCount.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopSellCount"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/SellCount.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopUpgradeWeaponCount"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/UpgradeWeaponCount.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DropItemUI"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/PopUp/PopUp%d.png"), 12))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ItemText"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/ItemText%d.png"), 7))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InvenEquipSlot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/EquipmentSlot%d.png"), 15))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InventorySelectBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/SelectBar.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniMonster"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Minimap/Player2.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniPlayer"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MiniMap/Hud%d.png"), 3))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TutorialMiniMap"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MiniMap/Stage%d.png"), 10))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EquipmentText"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/Equipment1Info%d.png"), 16))))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BattleItemWindow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/BattleItemWindow%d.png"), 4))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EatText"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/Eat.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_GolemName"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Boss/GolemName.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_RiddleName"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Boss/RiddleName.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_KumaName"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Boss/KumaName.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ChaosName"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Boss/ChaosName.png"), 1))))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_VendorBackground"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/VendorBackground%d.png"), 6))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_WeaponLevelUp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/LevelUp%d.png"), 6))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniQuest"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Minimap/Quest.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniBossMonster"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Minimap/Boss.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BossBase"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Boss/Base.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkullBase"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Boss/SkullBase.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkullLight"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Boss/SkullBase2.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkullVanish"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Boss/SkullVanish.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Warning"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Boss/Warning.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ThunderBase"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Boss/ThunderBase%d.png"), 6))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Thunder"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Boss/Thunder%d.png"), 6))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopMoney"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/Money.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopSign"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/Sign%d.png"), 2))))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StatusBackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevelUp/Stat/StatWindow.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StatusWindow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevelUp/Stat/Status2.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StatusGuide"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevelUp/Stat/StatGuide.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpgradeScene"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Upgrade/%d.png"), 149))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpgradeSceneFail"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Fail/%d.png"), 44))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpgradeSceneSuccess"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Success/%d.png"), 86))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_TalkButton"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Button/Talk%d.png"), 8))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StatusSelectBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevelUp/Stat/StatSelect.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StatusSquare"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevelUp/Stat/Stat%d.png"), 20))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StatIcon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevelUp/Stat/StatWindow3.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Lv"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevelUp/Stat/Lv%d.png"), 21))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ReserveBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevelUp/Stat/LevelUpBar.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player1Icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevelUp/Stat/Player1Icon.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player2Icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevelUp/Stat/Player2Icon.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Player3Icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevelUp/Stat/Player3Icon.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_PlayerCircle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevelUp/Stat/Circle2.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StatusLT1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevelUp/Stat/Player_Status11.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StatusLT2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevelUp/Stat/Player_Status22.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StatusLT3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevelUp/Stat/Player_Status33.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EquipMentPriviousBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/EquipMent/EquipMentBar.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EquipmentCase"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/EquipMent/ItemBackground.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EquipMentWindow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/EquipMent/EquipMentWindow.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EquipmentStatus"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/EquipMent/Status2.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EquipmentGuide"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/EquipMent/WeaponExplain.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EquipmentSelectColor"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/EquipMent/EquipmentSelectColor.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EquipmentSelectCase"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/EquipMent/SelectCase.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BattleEnd"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/BattleEnd/BattleEnd.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BattleEndBase"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/BattleEnd/ResultBase%d.png"), 2))))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpgradeTexture"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/Upgradev%d.png"), 3))))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InventoryUpside"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/ItemUpside2.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopMoneyTexture"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/MoneyTexture%d.png"), 2))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopEffect0"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/UpgradeEffect%d.png"), 16))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopDark"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/Dark.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopDark1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/Dark1.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopWhite"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/WhiteEffect.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Blind"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Blind%d.png"), 2))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ItemBackEffect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Effect/%d.png"), 16))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Guro"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MapName/Guro.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StudyRoom"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MapName/StudyRoom.png"), 1))))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hunger"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/%d.png"), 5))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FoodType"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/FoodType%d.png"), 8))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_PartyBuff"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/PartyBuff.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UpgradeFail"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/Fail%d.png"), 2))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Subway"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MapName/Subway.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BuffStr"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/PartyBuffStr.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BuffHp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/PartyBuffHp.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniHockey"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Minimap/Monster.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MapNameEffect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MapName/%d.png"), 16))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniMapBase"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Minimap/Base.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MiniMapUpside"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Minimap/Upside.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BuffEffectHp"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/PartyEdgeHp.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BuffEffectStr"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/PartyEdgeStr.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Picture"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Picture%d.png"), 4))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_OpenOX"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Stop/%d.png"), 2))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Meg"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Meg.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Ink"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Ink%d.png"), 3))))
		return E_FAIL;


	//Prototype_Component_Texture_HockeyReserve

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HockeyReserve"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/HockeyReserve3.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DDStart"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Loading/GameStart%d.png"), 100))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Hit */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hit"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/Hit.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Hit_Bar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/Hit_Bar.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Task */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Task_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Quest/task_bg.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Task_BackGround2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Quest/task_bg_1.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Task_Title"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Quest/task_title.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_StoryQuest */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StoryQuest_BackGround1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Quest/storyquest_bg1.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StoryQuest_BackGround2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Quest/storyquest_bg2.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StoryQuest_BackGround3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Quest/storyquest_bg3.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StoryQuest_Point"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Quest/storyquest_point.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_StoryQuest_Contents"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Quest/storyquest_contents%d.png"), 15 + 3))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Task_Contents"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Quest/task_contents%d.png"), 16 + 3))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Burst */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Burst_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Burst/hud_burst_bg.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Burst_Bar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Burst/hud_burst_bar.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Burst_BarBG"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Burst/hud_burst_barBG.png"), 1))))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MarvelTime"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MarvelTime.png")))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MarvelTimeBackground"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/MarvelTimeBackground.png")))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Burst_Num"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Burst/hud_burst_num%d.png"), 4))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Burst_Text"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Burst/hud_burst_text%d.png"), 2))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Burst_Effect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Burst/fx_burst_loof%d.png"), 15))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_EP_Battle */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EP_Battle"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/EP_Effect/fx_BP_battle%d.png"), 16))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_PartyOn */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_PartyOn_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/menu_tip_bg.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_PartyOn_Contents"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/menu_tip_contents%d.png"), 2))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_PartyOn_BackGround2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/menu_tip_bg1.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_NPC_Line */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_NPC_Line"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/NpcLines/dialogue0%d.png"), 9))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_UI_Number */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Number"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Number/number_%d.png"), 10))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Number2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Number/Number2/number%d.png"), 10))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Number3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Number/Number3/number%d.png"), 11))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Cross_Item */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Cross_Item_Background"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/Cross01.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Cross_Item"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/Item/Tpye%d.png"), 7))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Monster_HP */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Monster_HP"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/Enemy_Hp_bg.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Monster_HP_Bar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/Enemy_Hp.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Monster_HP2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/Enemy_Hp_bg1.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Monster_HP2_Bar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/Enemy_Hp1.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Monster_PG_Bar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/Enemy_PG1.png"), 1))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Boss_HP */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boss_HP"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/hud_boss_bg.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boss_HP_Bar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/hud_boss_hp.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boss_PG_Bar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/hud_boss_pg.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Boss_Name"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/hud_boss_name%d.png"), 5))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Coin */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Coin_BackGround"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Coin/Coin_bg0.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Coin_BackGround2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Coin/Coin_bg1.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Coin_BackGround3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Coin/Coin_bg2.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Coin_Icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Coin/Coin_bg3.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Coin_Num"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Coin/Coin_num.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Coin_Effect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Coin/Coin_Effect%d.png"), 16))))
		return E_FAIL;

	/* For.Prototype_Component_Texture_Key_Info */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dash_Info"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/btn_Lshift2.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Cross_Info"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/btn_ctrl_l2.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dodge_Info"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/btn_q.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LockOn_Info"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/btn_r.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_PictureBurst"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Burst.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EquipmentUpside"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/EquipMent/EquipMentUpside.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ShopLvMax"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Shop/LvMax.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EquipmentSelectBar"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/EquipMent/SelectBackground2.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EquipmentWeaponBackground"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/EquipMent/Weapon%d.png"), 21))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EquipmentHatBackground"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/EquipMent/Hat%d.png"), 6))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EquipmentClothesBackground"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/EquipMent/Clothes%d.png"), 6))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EquipmentDecoBackground"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/EquipMent/Deco%d.png"), 6))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_HatSlot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/EquipMent/HatName%d.png"), 5))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ClothesSlot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/EquipMent/ClothesName%d.png"), 5))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DecoSlot"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/EquipMent/DecoName%d.png"), 5))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_EquipmentE"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/EquipMent/EquipmentE.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InventoryWindow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/InventoryWindow0.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ReserveEffect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevelUp/Stat/LevelUpEffect2.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_LevelUpEffect"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/LevelUp/Stat/StatUp%d.png"), 16))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Equipment1Info"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/EquipMent/Equipment3Info%d.png"), 5))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Equipment2Info"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/EquipMent/Equipment1Info%d.png"), 5))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Equipment3Info"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/EquipMent/Equipment2Info%d.png"), 5))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SelectItem"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Inventory/Tpye%d.png"), 7))))
		return E_FAIL;

	// 대화창 UI들
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DialogTalk"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialog/T_UI_DialogTalk_%d.png"), 103))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_BattleDialog"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialog/BattleDialog/BattleDialog_%d.png"), 35))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DialogProfile_Hero1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialog/T_UI_DialogProfile_Hero1_%d.png"), 11))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DialogProfile_Hero1_MK1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialog/T_UI_DialogProfile_Hero1_MK1_%d.png"), 11))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DialogProfile_Hero1_MK2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialog/T_UI_DialogProfile_Hero1_MK2_%d.png"), 11))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DialogProfile_Hero3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialog/T_UI_DialogProfile_Hero3_%d.png"), 6))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DialogProfile_Hero4"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialog/T_UI_DialogProfile_Hero4_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DialogProfile_Hero4_MK1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialog/T_UI_DialogProfile_Hero4_MK1_%d.png"), 4))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DialogProfile_NPC_Tutorial"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialog/T_UI_DialogProfile_NPC_Tutorial_%d.png"), 7))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DialogProfile_NPC_PartTime"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialog/T_UI_DialogProfile_PartTime_%d.png"), 6))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DialogProfile_NPC_Zengbo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Dialog/T_UI_DialogProfile_Zengbo_%d.png"), 1))))
		return E_FAIL;


	// 퀘스트 느낌표
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_ActiveQuest"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Front/icon_active_quest%d.png"), 2))))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MinimapBackground"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Minimap/MinimapBackground.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MinimapMask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Minimap/Mask2.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Clue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Clue.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_MinimapStage1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/Minimap/MinimapStage1.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SkillTextBasic"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/UI/SkillText/Basic.png"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_SmokeNoise"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Obj/Noise/smoke.dds"), 1))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Holo_1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Obj/Holo/Holo_1_%d.dds"), 4, false))))
		return E_FAIL;


	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Holo_2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Obj/Holo/Holo_2_%d.dds"), 4, false))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Holo_3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Obj/Holo/Holo_3.dds"), 1, false))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Holo_4"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Obj/Holo/Holo_4.dds"), 1, false))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Holo_5"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Obj/Holo/Holo_5.dds"), 1, false))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Holo_6"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Obj/Holo/Holo_6.dds"), 1, false))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Holo_7"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Obj/Holo/Holo_7.dds"), 1, false))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Sky"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Sky/Sky_%d.dds"), 2))))
		return E_FAIL;

#pragma endregion 텍스처

#pragma region 이펙트 텍스쳐
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Lens"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_FlashLens%d.png"), 3))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Beast_Emi"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/EN0301_EMI.dds"), 1))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Oct_Emi"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_EN0103_Broken_EMI.dds"), 1))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Beta_Emi"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_EN0002_E.dds"), 1))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Fusion_Emi"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_En0009_E.dds"), 1))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_DisBoss_Emi"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_EN0003_E.dds"), 1))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_OldBomb_Emi"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_EN0005_E.dds"), 1))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Bomb_Emi"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/EN0007_E.dds"), 1))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Defence_Emi"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/EN0006_E.dds"), 1))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_GolemA_Emi"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_EN0106_E_A.dds"), 1))))
		return E_FAIL;
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_GolemB_Emi"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_EN0106_E_B.dds"), 1))))
		return E_FAIL;
#pragma endregion 이펙트 텍스쳐



#pragma region 모델 바이너리화

	lstrcpy(m_szLoadingText, TEXT("모델을 로딩중입니다. "));



	/* For.Prototype_Component_VIBuffer_ColorCube */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_ColorCube"),
		CVIBuffer_ColorCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(DM->Create_Try_BinModel_All()))
		return E_FAIL;
	

#pragma endregion 모델 바이너리화

#pragma region 네비게이션
	lstrcpy(m_szLoadingText, TEXT("네비게이션데이터를 생성하는 중입니다."));

	/* For.Prototype_Component_Navigation */
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Stage1Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Data/NavigationData/Stage1Navi.dat")))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Stage2Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Data/NavigationData/Stage2Navi.dat")))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Stage3Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Data/NavigationData/Stage3Navi.dat")))))
		return E_FAIL;

	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Stage4Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Data/NavigationData/Stage4Navi.dat")))))
		return E_FAIL;
		
	if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("Stage5Navigation"),
		CNavigation::Create(m_pDevice, m_pContext, TEXT("../Data/NavigationData/Stage5Navi.dat")))))
		return E_FAIL;

#pragma endregion 네비게이션


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니다.  "));


	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForLogoLevel()
{

#pragma region UI 객체원형
	lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩중입니다. "));

	

#pragma endregion UI 객체원형



#pragma region 정적모델 객체원형
	


#pragma endregion 정적모델 객체원형



#pragma region Anim모델 객체원형

#pragma endregion Anim모델 객체원형



#pragma region 텍스처
	lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));


#pragma endregion 텍스처



#pragma region 네비게이션
	lstrcpy(m_szLoadingText, TEXT("네비게이션데이터를 생성하는 중입니다."));


#pragma endregion 네비게이션


	lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니ㅏㄷ.  "));


	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForTestLevel()
{
	if (!TM->Get_Loaded(LEVEL_TEST))
	{
#pragma region UI 객체원형
		lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩중입니다. "));


		

#pragma endregion UI 객체원형



#pragma region 정적모델 객체원형
		

#pragma endregion 정적모델 객체원형



#pragma region Anim모델 객체원형
		

#pragma endregion Anim모델 객체원형



#pragma region 텍스처
		lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));

	


#pragma endregion 텍스처



#pragma region 네비게이션
		lstrcpy(m_szLoadingText, TEXT("네비게이션데이터를 생성하는 중입니다."));


#pragma endregion 네비게이션


		lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니ㅏㄷ.  "));
		TM->Set_Loaded(LEVEL_TEST);
	}
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForToolLevel()
{
	if (!TM->Get_Loaded(LEVEL_TOOL))
	{
#pragma region UI 객체원형
		lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩중입니다. "));

#pragma endregion UI 객체원형



#pragma region 정적모델 객체원형


#pragma endregion 정적모델 객체원형



#pragma region Anim모델 객체원형

#pragma endregion Anim모델 객체원형



#pragma region 텍스처
		lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));


#pragma endregion 텍스처



#pragma region 네비게이션
		lstrcpy(m_szLoadingText, TEXT("네비게이션데이터를 생성하는 중입니다."));


#pragma endregion 네비게이션


		lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니ㅏㄷ.  "));
		TM->Set_Loaded(LEVEL_TOOL);
	}

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForStage1Level()
{
	if (!TM->Get_Loaded(LEVEL_STAGE1))
	{
#pragma region UI 객체원형
		lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩중입니다. "));

#pragma endregion UI 객체원형



#pragma region 정적모델 객체원형


#pragma endregion 정적모델 객체원형



#pragma region Anim모델 객체원형

#pragma endregion Anim모델 객체원형



#pragma region 텍스처
		lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));


#pragma endregion 텍스처



#pragma region 네비게이션
		lstrcpy(m_szLoadingText, TEXT("네비게이션데이터를 생성하는 중입니다."));


#pragma endregion 네비게이션


		lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니ㅏㄷ.  "));
		TM->Set_Loaded(LEVEL_STAGE1);
	}
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForStage2Level()
{
	if (!TM->Get_Loaded(LEVEL_STAGE2))
	{
#pragma region UI 객체원형
		lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩중입니다. "));

#pragma endregion UI 객체원형



#pragma region 정적모델 객체원형
		if (FAILED(GI->Add_Prototype(TEXT("Stage2"),
			CStage2::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		if (FAILED(GI->Add_Prototype(TEXT("Stage2Glass"),
			CStage2Glass::Create(m_pDevice, m_pContext))))
			return E_FAIL;

#pragma endregion 정적모델 객체원형



#pragma region Anim모델 객체원형


#pragma endregion Anim모델 객체원형



#pragma region 텍스처
		lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));


#pragma endregion 텍스처



#pragma region 네비게이션
		lstrcpy(m_szLoadingText, TEXT("네비게이션데이터를 생성하는 중입니다."));


#pragma endregion 네비게이션


		lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니ㅏㄷ.  "));
		TM->Set_Loaded(LEVEL_STAGE2);
	}

	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForStage3Level()
{
	if (!TM->Get_Loaded(LEVEL_STAGE3))
	{
#pragma region UI 객체원형
		lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩중입니다. "));

#pragma endregion UI 객체원형



#pragma region 정적모델 객체원형
		if (FAILED(GI->Add_Prototype(TEXT("Stage3"),
			CStage3::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("Stage3Terrain"),
			CStage3Terrain::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("Stage3Glass"),
			CStage3Glass::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("Stage3Water"),
			CStage3Water::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("Stage3Trees"),
			CStage3Trees::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("Stage3Door"),
			CStage3Door::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("Stage3DoorGlass"),
			CStage3DoorGlass::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("Stage3Cars"),
			CStage3Cars::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("Stage3Bus"),
			CStage3Bus::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("Stage3AD"),
			CStage3Ad::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("Buildings"),
			CBuildings::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		if (FAILED(GI->Add_Prototype(TEXT("StreetStore"),
			CStreetStore::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("AzitLight"),
			CAzitLight::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("AzitProp"),
			CAzitProp::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		if (FAILED(GI->Add_Prototype(TEXT("Stage3Event1"),
			CStage3Event1::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		if (FAILED(GI->Add_Prototype(TEXT("Stage3AzitEvent"),
			CStage3AzitEvent::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("CluePic"),
			CCluePic::Create(m_pDevice, m_pContext))))
			return E_FAIL;

#pragma endregion 정적모델 객체원형



#pragma region Anim모델 객체원형
		if (FAILED(GI->Add_Prototype(TEXT("NPCIdle"),
			CNPCIdle::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("NPCWalk"),
			CNPCWalk::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("NPCPartTime"),
			CNPCPartTime::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("NPCZengbo"),
			CNPCZengbo::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("NPCDog"),
			CNPCDog::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("Dogbox"),
			CDogbox::Create(m_pDevice, m_pContext))))
			return E_FAIL;

#pragma endregion Anim모델 객체원형



#pragma region 텍스처
		lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));
		if (FAILED(GI->Add_Prototype(LEVEL_STATIC, TEXT("T_BD_acad"),
			CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Textures/Effect/T_BD_acc%d.png"), 2))))
			return E_FAIL;

#pragma endregion 텍스처



#pragma region 네비게이션
		lstrcpy(m_szLoadingText, TEXT("네비게이션데이터를 생성하는 중입니다."));


#pragma endregion 네비게이션


		lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니ㅏㄷ.  "));
		TM->Set_Loaded(LEVEL_STAGE3);
	}
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForStage4Level()
{
	if (!TM->Get_Loaded(LEVEL_STAGE3))
	{
#pragma region UI 객체원형
		lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩중입니다. "));

		if (FAILED(GI->Add_Prototype(TEXT("Stage4"),
			CStage4::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("Stage4Roof"),
			CStage4Roof::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("Stage4Face"),
			CStage4Face::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("Stage4Glass"),
			CStage4Glass::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("Stage4Light"),
			CStage4Light::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("Stage4Star"),
			CStage4Star::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("Stage4Machine0"),
			CStage4Machine0::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("Stage4Coin"),
			CStage4Coin::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("Stage4Escalator"),
			CStage4Escalator::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("Stage4Event1"),
			CStage4Event1::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("Stage4Event2"),
			CStage4Event2::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("Stage4Event3"),
			CStage4Event3::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("Stage4Event4"),
			CStage4Event4::Create(m_pDevice, m_pContext))))
			return E_FAIL;
		if (FAILED(GI->Add_Prototype(TEXT("Stage4Event5"),
			CStage4Event5::Create(m_pDevice, m_pContext))))
			return E_FAIL;

#pragma endregion UI 객체원형



#pragma region 정적모델 객체원형


#pragma endregion 정적모델 객체원형



#pragma region Anim모델 객체원형

#pragma endregion Anim모델 객체원형



#pragma region 텍스처
		lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));

		if (FAILED(GI->Add_Prototype(TEXT("Prototype_GameObject_HockeyReserve"),
			CHockeyReserve::Create(m_pDevice, m_pContext))))
			return E_FAIL;
#pragma endregion 텍스처



#pragma region 네비게이션
		lstrcpy(m_szLoadingText, TEXT("네비게이션데이터를 생성하는 중입니다."));


#pragma endregion 네비게이션


		lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니ㅏㄷ.  "));
		TM->Set_Loaded(LEVEL_STAGE4);
	}
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Loading_ForStage5Level()
{
	if (!TM->Get_Loaded(LEVEL_STAGE5))
	{
#pragma region UI 객체원형
		lstrcpy(m_szLoadingText, TEXT("객체원형을 로딩중입니다. "));

#pragma endregion UI 객체원형



#pragma region 정적모델 객체원형
		if (FAILED(GI->Add_Prototype(TEXT("Stage5"),
			CStage5::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		if (FAILED(GI->Add_Prototype(TEXT("Stage5Light"),
			CStage5Light::Create(m_pDevice, m_pContext))))
			return E_FAIL;

		if (FAILED(GI->Add_Prototype(TEXT("Stage5Event1"),
			CStage5Event1::Create(m_pDevice, m_pContext))))
			return E_FAIL;

#pragma endregion 정적모델 객체원형



#pragma region Anim모델 객체원형

#pragma endregion Anim모델 객체원형



#pragma region 텍스처
		lstrcpy(m_szLoadingText, TEXT("텍스쳐를 로딩중입니다. "));


#pragma endregion 텍스처



#pragma region 네비게이션
		lstrcpy(m_szLoadingText, TEXT("네비게이션데이터를 생성하는 중입니다."));


#pragma endregion 네비게이션


		lstrcpy(m_szLoadingText, TEXT("로딩이 완료되었습니ㅏㄷ.  "));
		TM->Set_Loaded(LEVEL_STAGE5);
	}
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader::Load_ForTexture()
{
	for (auto& iter : *(DM->Get_TextureFileNames()))
	{
		_tchar* Name = TM->Get_ManagedTChar();
		TM->CtoTC(iter.data(), Name);
		string Path = "../Bin/Resources/Particle/";
		Path = Path + iter.data();
		_tchar* tPath = TM->Get_ManagedTChar();
		TM->CtoTC(Path.data(), tPath);

		int EndIndex = (int)iter.length() - 4;
		string temp = iter.substr(0, EndIndex);
		char* RealName = TM->Get_ManagedChar();
		for (int i = 0; i < temp.length(); ++i)
			RealName[i] = temp.data()[i];

		_tchar* tagName = TM->Get_ManagedTChar();
		TM->CtoTC(RealName, tagName);


		if (FAILED(GI->Add_Prototype(LEVEL_STATIC, tagName,
			CTexture::Create(m_pDevice, m_pContext, tPath, 1))))
		{
			MSG_BOX(TEXT("Fail To Load Texture"));
			return E_FAIL;
		}

	}

	return S_OK;
}


CLoader * CLoader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLoader*		pInstance = new CLoader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed To Created : CLoader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLoader::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);

	DeleteCriticalSection(&m_CriticalSection);

	CloseHandle(m_hThread);

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
