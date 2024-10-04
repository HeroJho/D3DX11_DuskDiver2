#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "Creture.h"
#include "TrainingRobot.h"

BEGIN(Client)
class Creture;
class TrainingRobot;
class CBeast;

class CMonster_Manager final : CBase
{
	DECLARE_SINGLETON(CMonster_Manager)

private:
	CMonster_Manager();
	virtual ~CMonster_Manager() = default;

public:
	HRESULT Add_Monster(CCreture* pMonster);
	HRESULT Add_Boss(CCreture* pBoss);

	HRESULT Add_TrainingBot(CTrainingRobot* pMonster);
	HRESULT Add_DisasterBeta(CCreture* pDisasterBeta);
	HRESULT Add_DisasterFusion(CCreture* pDisasterFusion);
	HRESULT Add_OldBombRobot(CCreture* pOldBombRobot);
	HRESULT Add_BombRobot(CCreture* pBombRobot);
	HRESULT Add_DefenceRobot(CCreture* pDefecneRobot);
	HRESULT Add_DisasterBoss(CCreture* pDisasterBoss);
	HRESULT Add_Golem(CCreture* pGolem);
	HRESULT Add_MechanicalOctopus(CCreture* pOctopus);
	HRESULT Add_Bear(CGameObject* pBear);


	void	WakeUp_TrainingBot(_uint iIndex);

	void	Control_Beta();
	void	Control_Fusion();
	void	Control_OldBombRobot();
	void	Control_BombRobot();
	void	Control_DefenceRobot();
	void	Control_DisasterBoss();
	void	Control_Golem();

	_float Get_BreathDamage() { return m_fBreathDamage; }
	void Set_BreathDamage(_float fDamage) { m_fBreathDamage = fDamage; }

	_float4	Get_OctopusPosition();

	void	Event_Stage1();
	int		Get_Stage1Count() { return m_iStage1Count; }

	HRESULT	Spawn_Stage2();
	void	Event_Stage2();
	int		Get_Stage2Count() { return m_iStage2Count; }

	HRESULT	Spawn_Stage3_1();
	HRESULT	Spawn_Stage3_2();
	HRESULT	Spawn_Golem();
	
	void	Event_Stage3_1();
	void	Event_Stage3_2();
	int		Get_Stage3_1Count() { return m_iStage3_1Count; }
	int		Get_Stage3_2Count() { return m_iStage3_2Count; }

	HRESULT	Spawn_Stage4_1();
	HRESULT	Spawn_Stage4_2();
	HRESULT	Spawn_Stage4_3();
	HRESULT	Spawn_Stage4_4();
	HRESULT	Spawn_Stage4_5(); // 볼링장
	HRESULT	Spawn_MechanicalOctopus();

	void	Event_Stage4_1();
	void	Event_Stage4_2();
	void	Event_Stage4_3();
	void	Event_Stage4_4();
	void	Event_Stage4_5();
	int		Get_Stage4_1Count() { return m_iStage4_1Count; } //펀치기계 앞 count 4일때 
	int		Get_Stage4_2Count() { return m_iStage4_2Count; } //하키 count 4
	int		Get_Stage4_3Count() { return m_iStage4_3Count; } //야바위 앞 count 5
	int		Get_Stage4_4Count() { return m_iStage4_4Count; } //종이쌓기 쪽 count 4
	int		Get_Stage4_5Count() { return m_iStage4_5Count; } //볼링장 7


	void	Event_MechanicalLeg() { ++m_iMechaniclaLegCount; }
	int		Get_MechanicalLegCount() { return m_iMechaniclaLegCount; }

	void	Arcade_Event1() { m_bArcadeCapsule1 = true; }
	void	Arcade_Event1Off() { m_bArcadeCapsule1 = false; }
	_bool	Get_Arcade_Event1() { return m_bArcadeCapsule1; }

	void	Arcade_Event2() { m_bArcadeCapsule2 = true; }
	void	Arcade_Event2Off() { m_bArcadeCapsule2 = false; }
	_bool	Get_Arcade_Event2() { return m_bArcadeCapsule2; }

	void	Arcade_Event3() { m_bArcadeCapsule3 = true; }
	void	Arcade_Event3Off() { m_bArcadeCapsule3 = false; }
	_bool	Get_Arcade_Event3() { return m_bArcadeCapsule3; }

	void	Stage4Scene_On() { m_bStage4Scene = true; }
	void	Stage4Scene_Off() { m_bStage4Scene = false; }
	_bool	Get_Stage4Scene() { return m_bStage4Scene; }

	void Set_BeastCry();
	void Add_Beast(CBeast* pBeast);
	void Beast_Spawn();

	void Plus_BeastHp(_float fHp);


	void Release_Beast();

	void	Set_DeadAll();

	HRESULT Release_Boss();
	HRESULT Release_Monster();

	list<CCreture*>* Get_MonsterList() { return &m_pMonster; }
	list<CCreture*>* Get_BossList() { return &m_pBoss; }
	CCreture* Get_Golem() { return m_pGolem; }

	CCreture* Find_NearstMonster(_float fRange = FLT_MAX);

	void Reset_Monster() { m_pMonster.clear(); }

	void	Check_LastMonster();
	int		Get_MonsterCount() { return m_iStagetMonsterCount; }
	void	Set_StageMonsterCount(int iCount) { m_iStagetMonsterCount = iCount; }
	void	Minus_MonsterCount() { --m_iStagetMonsterCount; }

	void Add_Haki(CCreture* pObj) { m_pHaki.push_back(pObj); }
	void DeadAll_Haki();

	CGameObject* Get_DigdaBear() { return m_pBear; }
private:
	list<CCreture*>	m_pMonster;
	list<CCreture*>	m_pBoss;
	list<CCreture*>	m_pDisasterBeta;
	list<CCreture*>	m_pDisasterFusion;
	list<CCreture*>	m_pOldBombRobot;
	list<CCreture*>	m_pHaki;
	list<CCreture*>	m_pBombRobot;
	list<CCreture*>	m_pDefenceRobot;
	CCreture*		m_pDisasterBoss;
	CCreture*		m_pGolem;
	CCreture*		m_pOctopus;
	CGameObject*	m_pBear;
	CBeast* m_pBeast = nullptr;

	_float m_fBreathDamage = 600.f;
	vector<CTrainingRobot*> m_pTrainingBot;

private:
	int		m_iStage1Count = 0;
	int		m_iStage2Count = 0;
	int		m_iStage3_1Count = 0;
	int		m_iStage3_2Count = 0;
	int		m_iStage4_1Count = 0;
	int		m_iStage4_2Count = 0;
	int		m_iStage4_3Count = 0;
	int		m_iStage4_4Count = 0;
	int		m_iStage4_5Count = 0;

	int		m_iStagetMonsterCount = 0;

	int		m_iMechaniclaLegCount = 0;

	_float	m_fBearHp = 10000.f;

	_bool	m_bArcadeCapsule1 = false;
	_bool	m_bArcadeCapsule2 = false;
	_bool	m_bArcadeCapsule3 = false;
	_bool	m_bStage4Scene = false;

public:
	virtual void Free() override;
};

END