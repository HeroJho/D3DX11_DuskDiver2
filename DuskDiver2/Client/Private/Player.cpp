#include "stdafx.h"
#include "..\Public\Player.h"
#include "GameInstance.h"
#include "HierarchyNode.h"
#include "Player_Manager.h"
#include "DataManager.h"
#include "Creture.h"
#include "Player01.h"
#include "Player03.h"
#include "Player04.h"
#include "Status_Manager.h"
#include "Camera_Manager.h"

CPlayer::CPlayer()
{
}

HRESULT CPlayer::Initialize()
{
	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Player_01"), LEVEL_STATIC, L"Layer_Player")))
		return E_FAIL;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Player_03"), LEVEL_STATIC, L"Layer_Player")))
		return E_FAIL;

	if (FAILED(GI->Add_GameObjectToLayer(TEXT("Player_04"), LEVEL_STATIC, L"Layer_Player")))
		return E_FAIL;

	//STM->Set_Hp(3, CStatus_Manager::PLAYER01);
	//STM->Set_Hp(3, CStatus_Manager::PLAYER03);
	//STM->Set_Hp(3, CStatus_Manager::PLAYER04);
	return S_OK;
}

HRESULT CPlayer::Add_Players(CCreture * pPlayer)
{
	if (pPlayer == nullptr)
		return E_FAIL;

	m_Players.push_back(pPlayer);
	return S_OK;
}

void CPlayer::Change_Player(SelectPlayer eSelectPlayer)
{
	if (eSelectPlayer == m_eSelectPlayer)
		return;

	switch (eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		if (!((CPlayer01*)m_Players[PLAYER_01])->Get_Die())
		{
			((CPlayer01*)m_Players[PLAYER_01])->Set_SelectPlayer(true);
			m_Players[eSelectPlayer]->Set_WorldPos(m_Players[m_eSelectPlayer]->Get_WorldPos());
			((CPlayer01*)m_Players[PLAYER_01])->Reset_Animation();
			((CPlayer01*)m_Players[PLAYER_01])->Create_TagEffect();
			((CPlayer01*)m_Players[PLAYER_01])->Change_HairCut(true);
			((CPlayer01*)m_Players[PLAYER_01])->Change_Sound();

			if (m_eSelectPlayer == PLAYER_03)
			{
				((CPlayer01*)m_Players[PLAYER_01])->Set_ChangeLook(CPlayer01::DIR(((CPlayer03*)m_Players[PLAYER_03])->Get_Dir()),
					m_Players[PLAYER_03]->Get_Dir(_float3{ 0.f,0.f,1.f }));
				((CPlayer01*)m_Players[PLAYER_01])->Set_NaviIndex(((CPlayer03*)m_Players[PLAYER_03])->Get_NaviIndex());
				((CPlayer03*)m_Players[PLAYER_03])->Change_Tag();

			}
			else if (m_eSelectPlayer == PLAYER_04)
			{
				((CPlayer01*)m_Players[PLAYER_01])->Set_ChangeLook(CPlayer01::DIR(((CPlayer04*)m_Players[PLAYER_04])->Get_Dir()),
					m_Players[PLAYER_04]->Get_Dir(_float3{ 0.f,0.f,1.f }));
				((CPlayer01*)m_Players[PLAYER_01])->Set_NaviIndex(((CPlayer04*)m_Players[PLAYER_04])->Get_NaviIndex());
				((CPlayer04*)m_Players[PLAYER_04])->Change_Tag();

			}
			CRM->Get_Cam()->Fov(-1.f, 0.2f, 0.f);
			m_eSelectPlayer = eSelectPlayer;
		}
		else
		{
			CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		}
		break;

	case Client::CPlayer::PLAYER_03:
		if (!((CPlayer03*)m_Players[PLAYER_03])->Get_Die())
		{
			((CPlayer03*)m_Players[PLAYER_03])->Set_SelectPlayer(true);
			m_Players[eSelectPlayer]->Set_WorldPos(m_Players[m_eSelectPlayer]->Get_WorldPos());
			((CPlayer03*)m_Players[PLAYER_03])->Reset_Animation();
			((CPlayer03*)m_Players[PLAYER_03])->Create_TagEffect();
			((CPlayer03*)m_Players[PLAYER_03])->Change_HairCut(true);
			((CPlayer03*)m_Players[PLAYER_03])->Change_Sound();

			if (m_eSelectPlayer == PLAYER_01)
			{
				((CPlayer03*)m_Players[PLAYER_03])->Set_ChangeLook(CPlayer03::DIR(((CPlayer01*)m_Players[PLAYER_01])->Get_Dir()),
					m_Players[PLAYER_01]->Get_Dir(_float3{ 0.f,0.f,1.f }));
				((CPlayer03*)m_Players[PLAYER_03])->Set_NaviIndex(((CPlayer01*)m_Players[PLAYER_01])->Get_NaviIndex());
				((CPlayer01*)m_Players[PLAYER_01])->Change_Tag();

			}
			else if (m_eSelectPlayer == PLAYER_04)
			{
				((CPlayer03*)m_Players[PLAYER_03])->Set_ChangeLook(CPlayer03::DIR(((CPlayer04*)m_Players[PLAYER_04])->Get_Dir()),
					m_Players[PLAYER_04]->Get_Dir(_float3{ 0.f,0.f,1.f }));
				((CPlayer03*)m_Players[PLAYER_03])->Set_NaviIndex(((CPlayer04*)m_Players[PLAYER_04])->Get_NaviIndex());
				((CPlayer04*)m_Players[PLAYER_04])->Change_Tag();

			}
			CRM->Get_Cam()->Fov(-1.f, 0.2f, 0.f);
			m_eSelectPlayer = eSelectPlayer;
		}
		else
		{
			CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		}
		break;

	case Client::CPlayer::PLAYER_04:
		if (!((CPlayer04*)m_Players[PLAYER_04])->Get_Die())
		{
			((CPlayer04*)m_Players[PLAYER_04])->Set_SelectPlayer(true);
			m_Players[eSelectPlayer]->Set_WorldPos(m_Players[m_eSelectPlayer]->Get_WorldPos());
			((CPlayer04*)m_Players[PLAYER_04])->Reset_Animation();
			((CPlayer04*)m_Players[PLAYER_04])->Create_TagEffect();
			((CPlayer04*)m_Players[PLAYER_04])->Change_HairCut(true);
			((CPlayer04*)m_Players[PLAYER_04])->Change_Sound();

			if (m_eSelectPlayer == PLAYER_01)
			{
				((CPlayer04*)m_Players[PLAYER_04])->Set_ChangeLook(CPlayer04::DIR(((CPlayer01*)m_Players[PLAYER_01])->Get_Dir()),
					m_Players[PLAYER_01]->Get_Dir(_float3{ 0.f,0.f,1.f }));
				((CPlayer04*)m_Players[PLAYER_04])->Set_NaviIndex(((CPlayer01*)m_Players[PLAYER_01])->Get_NaviIndex());
				((CPlayer01*)m_Players[PLAYER_01])->Change_Tag();

			}
			else if (m_eSelectPlayer == PLAYER_03)
			{
				((CPlayer04*)m_Players[PLAYER_04])->Set_ChangeLook(CPlayer04::DIR(((CPlayer03*)m_Players[PLAYER_03])->Get_Dir()),
					m_Players[PLAYER_03]->Get_Dir(_float3{ 0.f,0.f,1.f }));
				((CPlayer04*)m_Players[PLAYER_04])->Set_NaviIndex(((CPlayer03*)m_Players[PLAYER_03])->Get_NaviIndex());
				((CPlayer03*)m_Players[PLAYER_03])->Change_Tag();

			}
			CRM->Get_Cam()->Fov(-1.f, 0.2f, 0.f);
			m_eSelectPlayer = eSelectPlayer;
		}

		else
		{
			CRM->Get_Cam()->Shake(0.1f, 0.1f, false, 0.1f);
		}


		break;
	}

	

}

_float4 CPlayer::Get_PlayerPos()
{
	return m_Players[m_eSelectPlayer]->Get_WorldPos();
}

_float4x4 CPlayer::Get_PlayerWorldMatrix()
{
	return m_Players[m_eSelectPlayer]->Get_WorldMatrix();
}

_float CPlayer::Get_PlayerHP(SelectPlayer eSelectPlayer)
{
	switch (eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		return ((CPlayer01*)m_Players[PLAYER_01])->Get_NowHp();		
	case Client::CPlayer::PLAYER_03:
		return ((CPlayer03*)m_Players[PLAYER_03])->Get_NowHp();
	case Client::CPlayer::PLAYER_04:
		return ((CPlayer04*)m_Players[PLAYER_04])->Get_NowHp();
	}

	return _float();
}

void CPlayer::Set_PlayerHP(_float fHP, SelectPlayer eSelectPlayer)
{
	switch (eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		((CPlayer01*)m_Players[PLAYER_01])->Set_NowHp(fHP);
		break;
	case Client::CPlayer::PLAYER_03:
		((CPlayer03*)m_Players[PLAYER_03])->Set_NowHp(fHP);
		break;
	case Client::CPlayer::PLAYER_04:
		((CPlayer04*)m_Players[PLAYER_04])->Set_NowHp(fHP);
		break;
	}
}

_float CPlayer::Get_PlayerSP(SelectPlayer eSelectPlayer)
{
	switch (eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		return ((CPlayer01*)m_Players[PLAYER_01])->Get_SP();
	case Client::CPlayer::PLAYER_03:
		return ((CPlayer03*)m_Players[PLAYER_03])->Get_SP();
	case Client::CPlayer::PLAYER_04:
		return ((CPlayer04*)m_Players[PLAYER_04])->Get_SP();
	}
	return _float();
}

void CPlayer::Set_PlayerSP(_float fSP, SelectPlayer eSelectPlayer)
{
	switch (eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		((CPlayer01*)m_Players[PLAYER_01])->Set_SP(fSP);
		break;
	case Client::CPlayer::PLAYER_03:
		((CPlayer03*)m_Players[PLAYER_03])->Set_SP(fSP);
		break;
	case Client::CPlayer::PLAYER_04:
		((CPlayer04*)m_Players[PLAYER_04])->Set_SP(fSP);
		break;
	}
}

_float CPlayer::Get_PlayerEP(SelectPlayer eSelectPlayer)
{
	switch (eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		return ((CPlayer01*)m_Players[PLAYER_01])->Get_EP();
	case Client::CPlayer::PLAYER_03:
		return ((CPlayer03*)m_Players[PLAYER_03])->Get_EP();
	case Client::CPlayer::PLAYER_04:
		return ((CPlayer04*)m_Players[PLAYER_04])->Get_EP();
	}

	return _float();
}

void CPlayer::Set_PlayerEP(_float fEP, SelectPlayer eSelectPlayer)
{
	switch (eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		((CPlayer01*)m_Players[PLAYER_01])->Set_EP(fEP);
		break;
	case Client::CPlayer::PLAYER_03:
		((CPlayer03*)m_Players[PLAYER_03])->Set_EP(fEP);
		break;
	case Client::CPlayer::PLAYER_04:
		((CPlayer04*)m_Players[PLAYER_04])->Set_EP(fEP);
		break;
	}
}

_float CPlayer::Get_PlayerBP(SelectPlayer eSelectPlayer)
{
	switch (eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		return ((CPlayer01*)m_Players[PLAYER_01])->Get_BP();
	case Client::CPlayer::PLAYER_03:
		return ((CPlayer03*)m_Players[PLAYER_03])->Get_BP();
	case Client::CPlayer::PLAYER_04:
		return ((CPlayer04*)m_Players[PLAYER_04])->Get_BP();
	}

	return _float();
}

void CPlayer::Set_PlayerBP(_float fBP, SelectPlayer eSelectPlayer)
{
	switch (eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		((CPlayer01*)m_Players[PLAYER_01])->Set_BP(fBP);
		break;
	case Client::CPlayer::PLAYER_03:
		((CPlayer03*)m_Players[PLAYER_03])->Set_BP(fBP);
		break;
	case Client::CPlayer::PLAYER_04:
		((CPlayer04*)m_Players[PLAYER_04])->Set_BP(fBP);
		break;
	}
}

_bool CPlayer::Get_Hurt()
{
	return m_Players[m_eSelectPlayer]->Get_Hurt();
}

void CPlayer::Change_Navigation(_uint iNaviNum)
{
	((CPlayer01*)m_Players[PLAYER_01])->Change_Navigation(iNaviNum);
	((CPlayer03*)m_Players[PLAYER_03])->Change_Navigation(iNaviNum);
	((CPlayer04*)m_Players[PLAYER_04])->Change_Navigation(iNaviNum);
	
}

_bool CPlayer::Get_BurstMode(SelectPlayer eSelectPlayer)
{
	switch (eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		return ((CPlayer01*)m_Players[PLAYER_01])->Get_BurstMode();
	case Client::CPlayer::PLAYER_03:
		return ((CPlayer03*)m_Players[PLAYER_03])->Get_BurstMode();
	case Client::CPlayer::PLAYER_04:
		return ((CPlayer04*)m_Players[PLAYER_04])->Get_BurstMode();
	}

	return false;
}

_bool CPlayer::Get_IsIdle()
{
	switch (m_eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		return ((CPlayer01*)m_Players[PLAYER_01])->Get_IsIdle();
		break;
	case Client::CPlayer::PLAYER_03:
		return ((CPlayer03*)m_Players[PLAYER_03])->Get_IsIdle();
		break;
	case Client::CPlayer::PLAYER_04:
		return ((CPlayer04*)m_Players[PLAYER_04])->Get_IsIdle();
		break;
	}
	return false;
}

_bool CPlayer::Get_PlayerDie(SelectPlayer eSelectPlayer)
{
	switch (eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		return ((CPlayer01*)m_Players[PLAYER_01])->Get_Die();
	case Client::CPlayer::PLAYER_03:
		return ((CPlayer03*)m_Players[PLAYER_03])->Get_Die();
	case Client::CPlayer::PLAYER_04:
		return ((CPlayer04*)m_Players[PLAYER_04])->Get_Die();
	}

	return false;
}

void CPlayer::Set_PlayerRevive(SelectPlayer eSelectPlayer)
{
	switch (eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		((CPlayer01*)m_Players[PLAYER_01])->Set_Revive();
	case Client::CPlayer::PLAYER_03:
		((CPlayer03*)m_Players[PLAYER_03])->Set_Revive();
	case Client::CPlayer::PLAYER_04:
		((CPlayer04*)m_Players[PLAYER_04])->Set_Revive();
	}
}

_bool CPlayer::Get_ReHp(SelectPlayer eSelectPlayer)
{
	switch (m_eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		return ((CPlayer01*)m_Players[PLAYER_01])->Get_ReHp();
		break;
	case Client::CPlayer::PLAYER_03:
		return ((CPlayer03*)m_Players[PLAYER_03])->Get_ReHp();
		break;
	case Client::CPlayer::PLAYER_04:
		return ((CPlayer04*)m_Players[PLAYER_04])->Get_ReHp();
		break;
	}
	return false;
}

CTransform * CPlayer::Get_Transform()
{
	switch (m_eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		return (CTransform*)((CPlayer01*)m_Players[PLAYER_01])->Get_ComponentPtr(TEXT("Com_Transform"));
		break;
	case Client::CPlayer::PLAYER_03:
		return (CTransform*)((CPlayer03*)m_Players[PLAYER_03])->Get_ComponentPtr(TEXT("Com_Transform"));
		break;
	case Client::CPlayer::PLAYER_04:
		return (CTransform*)((CPlayer04*)m_Players[PLAYER_04])->Get_ComponentPtr(TEXT("Com_Transform"));
		break;
	}
	return nullptr;
}

_bool CPlayer::Get_PlayerJump()
{
	switch (m_eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		return ((CPlayer01*)m_Players[PLAYER_01])->Get_IsJump();

	case Client::CPlayer::PLAYER_03:
		return ((CPlayer03*)m_Players[PLAYER_03])->Get_IsJump();

	case Client::CPlayer::PLAYER_04:
		return ((CPlayer04*)m_Players[PLAYER_04])->Get_IsJump();
	}

	return false;
}

void CPlayer::Set_ScenePos(SelectPlayer eSelectPlayer, _float4 vPos)
{
	switch (eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		return ((CPlayer01*)m_Players[PLAYER_01])->Set_ScenePos(vPos);
	case Client::CPlayer::PLAYER_03:
		return ((CPlayer03*)m_Players[PLAYER_03])->Set_ScenePos(vPos);
	case Client::CPlayer::PLAYER_04:
		return ((CPlayer04*)m_Players[PLAYER_04])->Set_ScenePos(vPos);
	}
}

void CPlayer::Set_SceneLook(SelectPlayer eSelectPlayer, _float4 vPos)
{
	switch (eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		return ((CPlayer01*)m_Players[PLAYER_01])->Set_SceneLook(vPos);
	case Client::CPlayer::PLAYER_03:
		return ((CPlayer03*)m_Players[PLAYER_03])->Set_SceneLook(vPos);
	case Client::CPlayer::PLAYER_04:
		return ((CPlayer04*)m_Players[PLAYER_04])->Set_SceneLook(vPos);
	}
}

void CPlayer::Set_SceneIdle(SelectPlayer eSelectPlayer)
{
	switch (eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		return ((CPlayer01*)m_Players[PLAYER_01])->Set_SceneIdle();
	case Client::CPlayer::PLAYER_03:
		return ((CPlayer03*)m_Players[PLAYER_03])->Set_SceneIdle();
	case Client::CPlayer::PLAYER_04:
		return ((CPlayer04*)m_Players[PLAYER_04])->Set_SceneIdle();
	}
}

void CPlayer::Set_SceneHit(SelectPlayer eSelectPlayer, CCreture::HITTYPE eHitType, _float fPushPower, _float fAirPower, _float4 vPos, _float3 vDir)
{
	switch (eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		return ((CPlayer01*)m_Players[PLAYER_01])->Set_SceneHit(eHitType, fPushPower, fAirPower, vPos, vDir);
	case Client::CPlayer::PLAYER_03:
		return ((CPlayer03*)m_Players[PLAYER_03])->Set_SceneHit(eHitType, fPushPower, fAirPower, vPos, vDir);
	case Client::CPlayer::PLAYER_04:
		return ((CPlayer04*)m_Players[PLAYER_04])->Set_SceneHit(eHitType, fPushPower, fAirPower, vPos, vDir);
	}
}

void CPlayer::Set_SceneRealHit(SelectPlayer eSelectPlayer, CCreture::HITTYPE eHitType, _float fPushPower, _float fAirPower, _float4 vPos, _float3 vDir, _float fDamage)
{
	switch (eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		return ((CPlayer01*)m_Players[PLAYER_01])->Set_SceneRealHit(eHitType, fPushPower, fAirPower, vPos, vDir, fDamage);
	case Client::CPlayer::PLAYER_03:
		return ((CPlayer03*)m_Players[PLAYER_03])->Set_SceneRealHit(eHitType, fPushPower, fAirPower, vPos, vDir, fDamage);
	case Client::CPlayer::PLAYER_04:
		return ((CPlayer04*)m_Players[PLAYER_04])->Set_SceneRealHit(eHitType, fPushPower, fAirPower, vPos, vDir, fDamage);
	}
}

_bool CPlayer::Get_PreUp()
{
	switch (m_eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		return ((CPlayer01*)m_Players[PLAYER_01])->Get_PreUp();
		break;
	case Client::CPlayer::PLAYER_03:
		return ((CPlayer03*)m_Players[PLAYER_03])->Get_PreUp();
		break;
	case Client::CPlayer::PLAYER_04:
		return ((CPlayer04*)m_Players[PLAYER_04])->Get_PreUp();
		break;
	}
	return false;
}

_bool CPlayer::Get_PreDown()
{
	switch (m_eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		return ((CPlayer01*)m_Players[PLAYER_01])->Get_PreDown();
		break;
	case Client::CPlayer::PLAYER_03:
		return ((CPlayer03*)m_Players[PLAYER_03])->Get_PreDown();
		break;
	case Client::CPlayer::PLAYER_04:
		return ((CPlayer04*)m_Players[PLAYER_04])->Get_PreDown();
		break;
	}
	return false;
}

_float CPlayer::Get_PlayerPreHP(SelectPlayer eSelectPlayer)
{
	switch (eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		return ((CPlayer01*)m_Players[PLAYER_01])->Get_PreHP();
	case Client::CPlayer::PLAYER_03:
		return ((CPlayer03*)m_Players[PLAYER_03])->Get_PreHP();
	case Client::CPlayer::PLAYER_04:
		return ((CPlayer04*)m_Players[PLAYER_04])->Get_PreHP();
	}
	return _float();
}

_float CPlayer::Get_PlayerPreSP(SelectPlayer eSelectPlayer)
{
	switch (eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		return ((CPlayer01*)m_Players[PLAYER_01])->Get_PreSP();
	case Client::CPlayer::PLAYER_03:
		return ((CPlayer03*)m_Players[PLAYER_03])->Get_PreSP();
	case Client::CPlayer::PLAYER_04:
		return ((CPlayer04*)m_Players[PLAYER_04])->Get_PreSP();
	}
	return _float();
}

_float CPlayer::Get_PlayerPreEP(SelectPlayer eSelectPlayer)
{
	switch (eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		return ((CPlayer01*)m_Players[PLAYER_01])->Get_PreEP();
	case Client::CPlayer::PLAYER_03:
		return ((CPlayer03*)m_Players[PLAYER_03])->Get_PreEP();
	case Client::CPlayer::PLAYER_04:
		return ((CPlayer04*)m_Players[PLAYER_04])->Get_PreEP();
	}
	return _float();
}

_bool CPlayer::Get_PatternSkill()
{
	switch (m_eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		return ((CPlayer01*)m_Players[PLAYER_01])->Get_PatternSkill();
		break;
	case Client::CPlayer::PLAYER_03:
		return ((CPlayer03*)m_Players[PLAYER_03])->Get_PatternSkill();
		break;
	case Client::CPlayer::PLAYER_04:
		return ((CPlayer04*)m_Players[PLAYER_04])->Get_PatternSkill();
		break;
	}
	return false;
}

void CPlayer::Player01ChangeOff()
{
	((CPlayer01*)m_Players[PLAYER_01])->ChangeOff();
}

void CPlayer::PlayerPortal()
{
	((CPlayer01*)m_Players[PLAYER_01])->PlayerPortal();
}

void CPlayer::Player01HurtVoice()
{
	((CPlayer01*)m_Players[PLAYER_01])->HurtVoice();
}

void CPlayer::Set_Player04Rage1End()
{
	((CPlayer04*)m_Players[PLAYER_04])->Set_Rage1End();
}
_float CPlayer::Get_PlayerBurstSkillOn(SelectPlayer eSelectPlayer)
{
	switch (eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		return ((CPlayer01*)m_Players[PLAYER_01])->Get_BurstSkillOn();
	case Client::CPlayer::PLAYER_03:
		return ((CPlayer03*)m_Players[PLAYER_03])->Get_BurstSkillOn();
	case Client::CPlayer::PLAYER_04:
		return ((CPlayer04*)m_Players[PLAYER_04])->Get_BurstSkillOn();
	}
	return _float();
}

_int CPlayer::Get_NaviIndex(SelectPlayer eSelectPlayer)
{
	switch (eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		return ((CPlayer01*)m_Players[PLAYER_01])->Get_NaviIndex();
	case Client::CPlayer::PLAYER_03:
		return ((CPlayer03*)m_Players[PLAYER_03])->Get_NaviIndex();
	case Client::CPlayer::PLAYER_04:
		return ((CPlayer04*)m_Players[PLAYER_04])->Get_NaviIndex();
	}

	return -1;
}

void CPlayer::Change_PlayerPos(SelectPlayer eSelectPlayer, _float4 vChanges)
{
	switch (eSelectPlayer)
	{
	case Client::CPlayer::PLAYER_01:
		return ((CPlayer01*)m_Players[PLAYER_01])->Change_PlayerPos(vChanges);
	case Client::CPlayer::PLAYER_03:
		return ((CPlayer03*)m_Players[PLAYER_03])->Change_PlayerPos(vChanges);
	case Client::CPlayer::PLAYER_04:
		return ((CPlayer04*)m_Players[PLAYER_04])->Change_PlayerPos(vChanges);
	}

}

void CPlayer::Free()
{
	//__super::Free();
}
