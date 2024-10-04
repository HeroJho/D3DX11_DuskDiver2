#include "stdafx.h"
#include "..\Public\Monster_Manager.h"
#include "Creture.h"
#include "TrainingRobot.h"
#include "DisasterBeta.h"
#include "DisasterFusion.h"
#include "OldBombRobot.h"
#include "DisasterBoss.h"
#include "Golem.h"
#include "Bear.h"
#include "UI_Manager.h"
#include "GameInstance.h"
#include "Player_Manager.h"
#include "ToolManager.h"
#include "Layer.h"
#include "DefenceRobot.h"
#include "BombRobot.h"
#include "Beast.h"


IMPLEMENT_SINGLETON(CMonster_Manager)

CMonster_Manager::CMonster_Manager()
{
}

HRESULT CMonster_Manager::Add_Monster(CCreture * pMonster)
{
	if (pMonster == nullptr)
		return E_FAIL;

	m_pMonster.push_back(pMonster);

	return S_OK;
}

HRESULT CMonster_Manager::Add_Boss(CCreture * pBoss)
{
	if (pBoss == nullptr)
		return E_FAIL;

	m_pBoss.push_back(pBoss);

	return S_OK;
}

HRESULT CMonster_Manager::Add_TrainingBot(CTrainingRobot * pMonster)
{
	if (pMonster == nullptr)
		return E_FAIL;

	m_pTrainingBot.push_back(pMonster);

	return S_OK;
}

HRESULT CMonster_Manager::Add_DisasterBeta(CCreture * pDisasterBeta)
{
	if (pDisasterBeta == nullptr)
		return E_FAIL;

	m_pDisasterBeta.push_back(pDisasterBeta);

	return S_OK;
}

HRESULT CMonster_Manager::Add_DisasterFusion(CCreture * pDisasterFusion)
{
	if (pDisasterFusion == nullptr)
		return E_FAIL;

	m_pDisasterFusion.push_back(pDisasterFusion);

	return S_OK;
}

HRESULT CMonster_Manager::Add_OldBombRobot(CCreture * pOldBombRobot)
{
	if (pOldBombRobot == nullptr)
		return E_FAIL;

	m_pOldBombRobot.push_back(pOldBombRobot);

	return S_OK;
}

HRESULT CMonster_Manager::Add_BombRobot(CCreture * pBombRobot)
{
	if (pBombRobot == nullptr)
		return E_FAIL;

	m_pBombRobot.push_back(pBombRobot);

	return S_OK;
}

HRESULT CMonster_Manager::Add_DefenceRobot(CCreture * pDefecneRobot)
{
	if (pDefecneRobot == nullptr)
		return E_FAIL;

	m_pDefenceRobot.push_back(pDefecneRobot);

	return S_OK;
}

HRESULT CMonster_Manager::Add_DisasterBoss(CCreture * pDisasterBoss)
{
	if (pDisasterBoss == nullptr)
		return E_FAIL;

	m_pDisasterBoss = pDisasterBoss;

	return S_OK;
}

HRESULT CMonster_Manager::Add_Golem(CCreture * pGolem)
{
	if (pGolem == nullptr)
		return E_FAIL;

	m_pGolem = pGolem;

	return S_OK;
}

HRESULT CMonster_Manager::Add_MechanicalOctopus(CCreture * pOctopus)
{
	if (pOctopus == nullptr)
		return E_FAIL;

	m_pOctopus = pOctopus;

	return S_OK;
}

HRESULT CMonster_Manager::Add_Bear(CGameObject * pBear)
{
	if (pBear == nullptr)
		return E_FAIL;

	m_pBear = pBear;

	return S_OK;
}


void CMonster_Manager::WakeUp_TrainingBot(_uint iIndex)
{
	m_pTrainingBot[iIndex]->Set_WakeUP();

}

void CMonster_Manager::Control_Beta()
{
	for (auto& pBetaList : m_pDisasterBeta)
	{
		static_cast<CDisasterBeta*>(pBetaList)->Control_DisasterBeta();
	}
}

void CMonster_Manager::Control_Fusion()
{
	for (auto& pFusionList : m_pDisasterFusion)
	{
		static_cast<CDisasterFusion*>(pFusionList)->Control_DisasterFusion();
	}
}

void CMonster_Manager::Control_OldBombRobot()
{
	for (auto& pOldRobotList : m_pOldBombRobot)
	{
		static_cast<COldBombRobot*>(pOldRobotList)->Control_OldBombRobot();
	}
}

void CMonster_Manager::Control_BombRobot()
{
	for (auto& pBombRobotList : m_pBombRobot)
	{
		static_cast<CBombRobot*>(pBombRobotList)->Control_BombRobot();
	}
}

void CMonster_Manager::Control_DefenceRobot()
{
	for (auto& pDefenceRobotList : m_pDefenceRobot)
	{
		static_cast<CDefenceRobot*>(pDefenceRobotList)->Control_DefenceRobot();
	}
}

void CMonster_Manager::Control_DisasterBoss()
{
	static_cast<CDisasterBoss*>(m_pDisasterBoss)->Control_DisasterBoss();
}

void CMonster_Manager::Control_Golem()
{
	static_cast<CGolem*>(m_pGolem)->Control_Golem();
}


_float4 CMonster_Manager::Get_OctopusPosition()
{
	return m_pOctopus->Get_WorldPos();
}


void CMonster_Manager::Event_Stage1()
{
	++m_iStage1Count;

	if (m_iStage1Count == 1)
	{
		UM->Set_TalkIndex(4);
		UM->Set_FrontUI(false);
	}
}

void CMonster_Manager::Event_Stage2()
{
	++m_iStage2Count;

}

HRESULT CMonster_Manager::Spawn_Stage3_1()
{
	CCreture::CREATEINFO _tInfo;
	ZeroMemory(&_tInfo, sizeof(CCreture::CREATEINFO));

	_tInfo.iIndex = 3;
	_tInfo.iNaviNum = 3;
	_tInfo.vDir = { 6.648f, 0.f, 0.51f };


	_tInfo.vPosition = { 30.122f, 0.f, -7.523f, 1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DisasterBeta"), LEVEL_STAGE3, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	_tInfo.vPosition = { 18.478f, 0.f, 1.692f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DisasterFusion"), LEVEL_STAGE3, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;


	_tInfo.vPosition = { 18.599f, 0.f, -4.472f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DisasterFusion"), LEVEL_STAGE3, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;


	_tInfo.vPosition = { 30.652f, 0.f, 1.396f, 1.f };
	

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("OldBombRobot"), LEVEL_STAGE3, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	m_iStagetMonsterCount = 4;

	return S_OK;

}

HRESULT CMonster_Manager::Spawn_Stage3_2()
{
	CCreture::CREATEINFO _tInfo;
	ZeroMemory(&_tInfo, sizeof(CCreture::CREATEINFO));

	_tInfo.iIndex = 4;
	_tInfo.iNaviNum = 3;
	_tInfo.vDir = { 41.932f, 0.f, 156.092f };

	_tInfo.vPosition = { 50.167f, 0.f, 129.992f, 1.f };
	
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DisasterBeta"), LEVEL_STAGE3, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	_tInfo.vPosition = { 40.395f, 0.f, 130.448f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("OldBombRobot"), LEVEL_STAGE3, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;


	_tInfo.vPosition = { 39.743f, 0.f, 124.497f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DisasterFusion"), LEVEL_STAGE3, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;


	_tInfo.vPosition = { 44.622f, 0.f, 123.412f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DisasterBoss"), LEVEL_STAGE3, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	m_iStagetMonsterCount = 4;

	return S_OK;
}


HRESULT CMonster_Manager::Spawn_Stage2()
{
	CCreture::CREATEINFO _tInfo;
	ZeroMemory(&_tInfo, sizeof(CCreture::CREATEINFO));

	_tInfo.iIndex = 2;
	_tInfo.iNaviNum = 2;
	_tInfo.vPosition = { -7.5f, 0.f, 2.f, 1.f };
	_tInfo.vDir = { -10.73f, 0.f, -12.65f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DisasterBeta"), LEVEL_STAGE2, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	_tInfo.vPosition = { -11.84f, 0.f, -6.85f, 1.f };
	_tInfo.vDir = { -2.73f, 0.f, -12.65f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DisasterBeta"), LEVEL_STAGE2, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	_tInfo.vPosition = { -4.25f, 0.f, -7.7f, 1.f };
	_tInfo.vDir = { -7.73f, 0.f, -12.65f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DisasterFusion"), LEVEL_STAGE2, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	m_iStagetMonsterCount = 3;

	return S_OK;
}

HRESULT CMonster_Manager::Spawn_Golem()
{
	CCreture::CREATEINFO _tInfo;
	ZeroMemory(&_tInfo, sizeof(CCreture::CREATEINFO));

	_tInfo.iIndex = 4;
	_tInfo.iNaviNum = 3;
	_tInfo.vPosition = _float4{ -20.638f, 0.f,141.499f,1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Golem"), LEVEL_STAGE3, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	m_iStagetMonsterCount = 1;

	return S_OK;
}

void CMonster_Manager::Event_Stage3_1()
{
	++m_iStage3_1Count;

	if (m_iStage3_1Count == 4)
	{
		UM->Set_FrontUI(false);

		UM->Set_BattleIndex(1);
		UM->Make_BattleEnd();
	}
}

void CMonster_Manager::Event_Stage3_2()
{
	++m_iStage3_2Count;

	/*if (m_iStage3_2Count == 4)
	{
		UM->Set_FrontUI(false);
		UM->Set_BattleIndex(2);
		UM->Make_BattleEnd();
	}*/
}

void CMonster_Manager::Set_BeastCry()
{
	m_pBeast->Set_BeastCry();
}

void CMonster_Manager::Add_Beast(CBeast * pBeast)
{
	m_pBeast = pBeast;
}

void CMonster_Manager::Beast_Spawn()
{

	m_pBeast->Spawn_Beast();
}

void CMonster_Manager::Plus_BeastHp(_float fHp)
{
	m_pBeast->Plus_BeastHP(fHp);
}

void CMonster_Manager::Release_Beast()
{
	m_pBeast = nullptr;
}

void CMonster_Manager::Set_DeadAll()
{
	for (auto& pMonster : m_pMonster)
	{
		pMonster->On_Hit(nullptr);
	}
}

HRESULT CMonster_Manager::Release_Boss()
{

	for (list<CCreture*>::iterator iter = m_pBoss.begin();  iter != m_pBoss.end();)
	{
		if ((*iter)->Get_NowHp() <= 0.f)
			m_pBoss.erase(iter);
		else
			++iter;
	}

	return S_OK;
}

HRESULT CMonster_Manager::Release_Monster()
{
	for (list<CCreture*>::iterator iter = m_pMonster.begin(); iter != m_pMonster.end();)
	{
		if ((*iter)->Get_NowHp() <= 0.f)
			m_pMonster.erase(iter);
		else
			++iter;
	}

	return S_OK;
}

CCreture * CMonster_Manager::Find_NearstMonster(_float fRange)
{
	_vector vPlayerPos = XMLoadFloat4(&PM->Get_PlayerPos());
	CLayer* pLayer = (GI->Get_Layers(TM->Get_CurLevel())->find(TEXT("Layer_Monster")))->second;


	CCreture* pNearstMonster = nullptr;
	_float fNeastDis = fRange;
	for (auto& pMonster : *pLayer->Get_GameObjects())
	{
		_vector vMonsterPos = XMLoadFloat4(&((CCreture*)pMonster)->Get_WorldPos());
		_float fDis = XMVectorGetX(XMVector3Length(vMonsterPos - vPlayerPos));
		if (fDis < fNeastDis)
		{
			pNearstMonster = (CCreture*)pMonster;
			fNeastDis = fDis;
		}
	}

	return pNearstMonster;
}

void CMonster_Manager::Check_LastMonster()
{
	for (auto& pMonsterList : m_pMonster)
	{
		if (pMonsterList->Get_NowHp() <= 0.f && m_iStagetMonsterCount == 1)
		{
			
			int i = 0;
		}
	}

}

void CMonster_Manager::DeadAll_Haki()
{
	for (auto& pMonster : m_pHaki)
	{
		pMonster->On_Hit(nullptr);
		
	}
}

void CMonster_Manager::Free()
{

}

HRESULT CMonster_Manager::Spawn_Stage4_1()
{

	CCreture::CREATEINFO _tInfo;
	ZeroMemory(&_tInfo, sizeof(CCreture::CREATEINFO));

	_tInfo.iIndex = 5;
	_tInfo.iNaviNum = 4;
	_tInfo.vDir = _float3{ 0.f,0.f,1.f };

	_tInfo.vPosition = { 93.4f, 3.9f, 198.815f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("BombRobot"), LEVEL_STAGE4, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;


	_tInfo.vPosition = { 101.306f, 3.902f, 189.618f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("BombRobot"), LEVEL_STAGE4, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	_tInfo.vPosition = { 103.4f, 3.9f, 201.715f, 1.f };
	_tInfo.vDir = _float3{ 0.96f,0.f, -0.29f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DefenceRobot"), LEVEL_STAGE4, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	_tInfo.vPosition = { 84.3f, 3.9f, 191.915f, 1.f };
	_tInfo.vDir = _float3{ -0.16f, -0.99f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DefenceRobot"), LEVEL_STAGE4, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	_tInfo.vPosition = { 108.4f, 3.9f, 188.715f, 1.f };
	_tInfo.vDir = _float3{ -0.16f, -0.99f,1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DisasterFusion"), LEVEL_STAGE4, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	m_iStagetMonsterCount = 5;

	return S_OK;
}


HRESULT CMonster_Manager::Spawn_Stage4_2() //гое╟
{
	CCreture::CREATEINFO _tInfo;
	ZeroMemory(&_tInfo, sizeof(CCreture::CREATEINFO));

	_tInfo.iIndex = 6;
	_tInfo.iNaviNum = 4;
	_tInfo.vDir = _float3{ 0.f,0.f,1.f };

	_tInfo.vPosition = { 99.f, -32.1f, 218.915f, 1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DisasterBeta"), LEVEL_STAGE4, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	_tInfo.vPosition = { 105.200f, -32.1f, 223.915f, 1.f };
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DisasterBeta"), LEVEL_STAGE4, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	_tInfo.vPosition = { 110.4f, -32.1f, 220.515f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DisasterFusion"), LEVEL_STAGE4, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	_tInfo.vPosition = { 113.f, -32.1f, 231.515f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DisasterBoss"), LEVEL_STAGE4, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	m_iStagetMonsterCount = 4;

	return S_OK;
}

HRESULT CMonster_Manager::Spawn_Stage4_3()
{
	CCreture::CREATEINFO _tInfo;
	ZeroMemory(&_tInfo, sizeof(CCreture::CREATEINFO));

	_tInfo.iIndex = 7;
	_tInfo.iNaviNum = 4;
	_tInfo.vDir = _float3{ 0.f,0.f,1.f };


	_tInfo.vPosition = { -126.930f, 0.f, 72.069f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DisasterFusion"), LEVEL_STAGE4, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	_tInfo.vPosition = { -138.130f, 0.f, 81.269f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DisasterFusion"), LEVEL_STAGE4, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	_tInfo.vPosition = { -121.130f, 0.f, 100.869f, 1.f };
	_tInfo.vDir = _float3{ -0.16f,0.f, -0.99f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("OldBombRobot"), LEVEL_STAGE4, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	_tInfo.vPosition = { -118.13f, 0.f, 92.869f, 1.f };
	_tInfo.vDir = _float3{ -0.86f,0.f, -0.5f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DefenceRobot"), LEVEL_STAGE4, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	_tInfo.vPosition = { -121.010f, -0.002f, 88.734f, 1.f };
	_tInfo.vDir = _float3{ -0.86f,0.f, -0.5f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DefenceRobot"), LEVEL_STAGE4, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	_tInfo.vPosition = { -133.630f, 0.f, 87.069f, 1.f };
	_tInfo.vDir = _float3{ -0.86f,0.f, -0.5f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("BombRobot"), LEVEL_STAGE4, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	m_iStagetMonsterCount = 6;

	return S_OK;
}

HRESULT CMonster_Manager::Spawn_Stage4_4()
{
	CCreture::CREATEINFO _tInfo;
	ZeroMemory(&_tInfo, sizeof(CCreture::CREATEINFO));

	_tInfo.iIndex = 8;
	_tInfo.iNaviNum = 4;
	_tInfo.vDir = _float3{ 0.f,0.f,1.f };
	_tInfo.vPosition = { -87.827f, 0.f, 187.280f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DisasterFusion"), LEVEL_STAGE4, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	_tInfo.vPosition = { -93.027f, 0.f, 184.280f, 1.f };
	_tInfo.vDir = _float3{ 0.68f,0.f, -0.74f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DefenceRobot"), LEVEL_STAGE4, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	_tInfo.vPosition = { -79.927f, 0.f, 185.38f, 1.f };
	_tInfo.vDir = _float3{ -0.31f,0.f, -0.63f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("BombRobot"), LEVEL_STAGE4, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	_tInfo.vPosition = { -82.890f, 27.317f, 168.780f, 1.f };
	_tInfo.vDir = _float3{ -0.31f,0.f, -0.63f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("BombRobot"), LEVEL_STAGE4, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	_tInfo.vPosition = { -79.327f, 27.219f, 187.880f, 1.f };
	_tInfo.vDir = _float3{ -0.31f,0.f, -0.63f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DefenceRobot"), LEVEL_STAGE4, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	m_iStagetMonsterCount = 5;
	return S_OK;
}

HRESULT CMonster_Manager::Spawn_Stage4_5()
{
	CCreture::CREATEINFO _tInfo;
	ZeroMemory(&_tInfo, sizeof(CCreture::CREATEINFO));

	_tInfo.iIndex = 9;
	_tInfo.iNaviNum = 4;
	_tInfo.vDir = _float3{ 0.f,0.f,1.f };
	_tInfo.vPosition = { 179.1f, 3.9f, 148.615f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DisasterBeta"), LEVEL_STAGE4, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	_tInfo.vPosition = { 172.f, 3.9f, 142.615f, 1.f };
	_tInfo.vDir = _float3{ 0.68f,0.f, -0.74f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DefenceRobot"), LEVEL_STAGE4, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	_tInfo.vPosition = { 166.8f, 3.9f, 144.015f, 1.f };
	_tInfo.vDir = _float3{ -0.31f,0.f, -0.63f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("BombRobot"), LEVEL_STAGE4, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	_tInfo.vPosition = { 163.3f, 3.9f, 156.715f, 1.f };
	_tInfo.vDir = _float3{ -0.31f,0.f, -0.63f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("BombRobot"), LEVEL_STAGE4, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	_tInfo.vPosition = { 166.7f, 3.9f, 165.215f, 1.f };
	_tInfo.vDir = _float3{ -0.31f,0.f, -0.63f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DisasterBeta"), LEVEL_STAGE4, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	_tInfo.vPosition = { 171.700f, 3.9f, 134.915f, 1.f };
	_tInfo.vDir = _float3{ -0.31f,0.f, -0.63f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("DisasterFusion"), LEVEL_STAGE4, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	m_iStagetMonsterCount = 6;

	return S_OK;
}

HRESULT CMonster_Manager::Spawn_MechanicalOctopus()
{
	CCreture::CREATEINFO _tInfo;
	ZeroMemory(&_tInfo, sizeof(CCreture::CREATEINFO));

	_tInfo.iIndex = 0;
	_tInfo.iNaviNum = 4;
	_tInfo.vDir = _float3{ 0.f,0.f,1.f };
	_tInfo.vPosition = { 4.978f, 15.531f, 163.164f, 1.f };

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("MechanicalOctopus"), LEVEL_STAGE4, TEXT("Layer_Monster"), &_tInfo)))
		return E_FAIL;

	m_iStagetMonsterCount = 1;

	return S_OK;
}

void CMonster_Manager::Event_Stage4_1()
{
	++m_iStage4_1Count;

}

void CMonster_Manager::Event_Stage4_2()
{
	++m_iStage4_2Count;

}

void CMonster_Manager::Event_Stage4_3()
{
	++m_iStage4_3Count;
}

void CMonster_Manager::Event_Stage4_4()
{
	++m_iStage4_4Count;
}

void CMonster_Manager::Event_Stage4_5()
{
	++m_iStage4_5Count;

}
