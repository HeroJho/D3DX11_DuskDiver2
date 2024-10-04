#include "stdafx.h"
#include "..\Public\Player_Manager.h"
#include "Player.h"
#include "GameInstance.h"
#include "Creture.h"
#include "Particle_Manager.h"

IMPLEMENT_SINGLETON(CPlayer_Manager)

CPlayer_Manager::CPlayer_Manager()
{
}


HRESULT CPlayer_Manager::Create_Players()
{
	m_pPlayer = new CPlayer();

	if (FAILED(m_pPlayer->Initialize()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Manager::Add_Player(CCreture * pPlayers)
{
	if (m_pPlayer == nullptr)
		return E_FAIL;
	
	m_pPlayer->Add_Players(pPlayers);
	
	return S_OK;
}

HRESULT CPlayer_Manager::Release_Player()
{
	delete(m_pPlayer);
	m_pPlayer = nullptr;
	return S_OK;
}


_float4 CPlayer_Manager::Get_PlayerPos()
{
	if (m_pPlayer == nullptr)
		return _float4{ 0.f,0.f,0.f,1.f };

	return m_pPlayer->Get_PlayerPos();
}

void CPlayer_Manager::Change_Player(CPlayer::SelectPlayer eSelectPlayer)
{
	if (m_pPlayer == nullptr)
		return;
	m_pPlayer->Change_Player(eSelectPlayer);
}

CPlayer::SelectPlayer CPlayer_Manager::Get_SelectPlayer()
{
	if (m_pPlayer == nullptr)
		return CPlayer::PLAYER_END;

	return m_pPlayer->Get_SelectPlayer();
}

_float4x4 CPlayer_Manager::Get_PlayerWorldMatrix()
{
	if (m_pPlayer == nullptr)
	{
		return  _float4x4();
	}

	return m_pPlayer->Get_PlayerWorldMatrix();
}

_float CPlayer_Manager::Get_PlayerHP(CPlayer::SelectPlayer eSelectPlayer)
{
	if (m_pPlayer == nullptr)
	{
		return  _float();
	}

	return m_pPlayer->Get_PlayerHP(eSelectPlayer);
}

void CPlayer_Manager::Set_PlayerHP(_float fHP, CPlayer::SelectPlayer eSelectPlayer)
{
	if (m_pPlayer == nullptr)
	{
		return;
	}

	m_pPlayer->Set_PlayerHP(fHP, eSelectPlayer);
}

_float CPlayer_Manager::Get_PlayerSP(CPlayer::SelectPlayer eSelectPlayer)
{
	if (m_pPlayer == nullptr)
	{
		return  _float();
	}

	return m_pPlayer->Get_PlayerSP(eSelectPlayer);
}

void CPlayer_Manager::Set_PlayerSP(_float fSP, CPlayer::SelectPlayer eSelectPlayer)
{
	if (m_pPlayer == nullptr)
	{
		return;
	}

	m_pPlayer->Set_PlayerSP(fSP, eSelectPlayer);
}

_float CPlayer_Manager::Get_PlayerEP(CPlayer::SelectPlayer eSelectPlayer)
{
	if (m_pPlayer == nullptr)
	{
		return  _float();
	}

	return m_pPlayer->Get_PlayerEP(eSelectPlayer);
}

void CPlayer_Manager::Set_PlayerEP(_float fEP, CPlayer::SelectPlayer eSelectPlayer)
{
	if (m_pPlayer == nullptr)
	{
		return;
	}

	m_pPlayer->Set_PlayerEP(fEP, eSelectPlayer);
}

_float CPlayer_Manager::Get_PlayerBP(CPlayer::SelectPlayer eSelectPlayer)
{
	if (m_pPlayer == nullptr)
	{
		return  _float();
	}

	return m_pPlayer->Get_PlayerBP(eSelectPlayer);
}

void CPlayer_Manager::Set_PlayerBP(_float fBP, CPlayer::SelectPlayer eSelectPlayer)
{
	if (m_pPlayer == nullptr)
	{
		return;
	}

	m_pPlayer->Set_PlayerBP(fBP, eSelectPlayer);
}

void CPlayer_Manager::Change_Navigation(_uint iNaviNum)
{
	if (m_pPlayer == nullptr)
		return;
	m_pPlayer->Change_Navigation(iNaviNum);
}

_bool CPlayer_Manager::Get_BurstMode(CPlayer::SelectPlayer eSelectPlayer)
{
	if (m_pPlayer == nullptr)
		return false;
	return m_pPlayer->Get_BurstMode(eSelectPlayer);
}

_bool CPlayer_Manager::Get_PlayerIsIdle()
{
	if (m_pPlayer == nullptr)
		return false;
	return m_pPlayer->Get_IsIdle();
}

_bool CPlayer_Manager::Get_PlayerDie(CPlayer::SelectPlayer eSelectPlayer)
{
	if (m_pPlayer == nullptr)
		return false;
	return m_pPlayer->Get_PlayerDie(eSelectPlayer);
}

void CPlayer_Manager::Set_PlayerRevive(CPlayer::SelectPlayer eSelectPlayer)
{
	if (m_pPlayer == nullptr)
		return;
	 m_pPlayer->Set_PlayerRevive(eSelectPlayer);
}

_bool CPlayer_Manager::Get_ReHp(CPlayer::SelectPlayer eSelectPlayer)
{
	if (m_pPlayer == nullptr)
		return false;
	return m_pPlayer->Get_ReHp(eSelectPlayer);
}

CTransform * CPlayer_Manager::Get_Transform()
{
	return m_pPlayer->Get_Transform();
}

_bool CPlayer_Manager::Get_PlayerPreUp()
{
	if (m_pPlayer == nullptr)
		return false;
	return m_pPlayer->Get_PreUp();
}

_bool CPlayer_Manager::Get_PlayerPreDown()
{
	if (m_pPlayer == nullptr)
		return false;
	return m_pPlayer->Get_PreDown();
}

_float CPlayer_Manager::Get_PlayerPreHP(CPlayer::SelectPlayer eSelectPlayer)
{
	if (m_pPlayer == nullptr)
	{
		return  _float();
	}

	return m_pPlayer->Get_PlayerPreHP(eSelectPlayer);
}

_float CPlayer_Manager::Get_PlayerPreSP(CPlayer::SelectPlayer eSelectPlayer)
{
	if (m_pPlayer == nullptr)
	{
		return  _float();
	}

	return m_pPlayer->Get_PlayerPreSP(eSelectPlayer);
}

_float CPlayer_Manager::Get_PlayerPreEP(CPlayer::SelectPlayer eSelectPlayer)
{
	if (m_pPlayer == nullptr)
	{
		return  _float();
	}

	return m_pPlayer->Get_PlayerPreEP(eSelectPlayer);
}

_bool CPlayer_Manager::Get_PlayerJump()
{
	return m_pPlayer->Get_PlayerJump();
}

void CPlayer_Manager::Set_ScenePos(CPlayer::SelectPlayer eSelectPlayer, _float4 vPos)
{
	if (m_pPlayer == nullptr)
		return;

	m_pPlayer->Set_ScenePos(eSelectPlayer, vPos);
}

void CPlayer_Manager::Set_SceneLook(CPlayer::SelectPlayer eSelectPlayer, _float4 vPos)
{
	if (m_pPlayer == nullptr)
		return;

	m_pPlayer->Set_SceneLook(eSelectPlayer, vPos);
}

void CPlayer_Manager::Set_SceneIdle(CPlayer::SelectPlayer eSelectPlayer)
{
	if (m_pPlayer == nullptr)
		return;

	m_pPlayer->Set_SceneIdle(eSelectPlayer);
}

void CPlayer_Manager::Set_SceneHit(CPlayer::SelectPlayer eSelectPlayer, CCreture::HITTYPE eHitType, _float fPushPower, _float fAirPower, _float4 vPos, _float3 vDir)
{
	if (m_pPlayer == nullptr)
		return;

	m_pPlayer->Set_SceneHit(eSelectPlayer, eHitType, fPushPower, fAirPower, vPos, vDir);
}

void CPlayer_Manager::Set_SceneRealHit(CPlayer::SelectPlayer eSelectPlayer, CCreture::HITTYPE eHitType, _float fPushPower, _float fAirPower, _float4 vPos, _float3 vDir, _float fDamage)
{
	if (m_pPlayer == nullptr)
		return;

	m_pPlayer->Set_SceneRealHit(eSelectPlayer, eHitType, fPushPower, fAirPower, vPos, vDir, fDamage);
}

_float CPlayer_Manager::Get_PlayerBurstSkillOn(CPlayer::SelectPlayer eSelectPlayer)
{
	if (m_pPlayer == nullptr)
	{
		return  _float();
	}

	return m_pPlayer->Get_PlayerBurstSkillOn(eSelectPlayer);
}

_int CPlayer_Manager::Get_NaviIndex(CPlayer::SelectPlayer eSelectPlayer)
{
	if (m_pPlayer == nullptr)
	{
		return -1;
	}

	return m_pPlayer->Get_NaviIndex(eSelectPlayer);
}

void CPlayer_Manager::Change_PlayerPos(CPlayer::SelectPlayer eSelectPlayer, _float4 vChanges)
{
	if (m_pPlayer == nullptr)
	{
		return;
	}

	return m_pPlayer->Change_PlayerPos(eSelectPlayer, vChanges);
}

void CPlayer_Manager::Create_PortionEffect(PORTIONTYPE eType)
{
	switch (eType)
	{
	case Client::CPlayer_Manager::PORTION_HP:	
		PTM->Create_AlphaParticle(L"HPPortion", PM->Get_PlayerWorldMatrix());
		PTM->Create_InstanceParticle(L"HPPortion", PM->Get_PlayerWorldMatrix());
		break;
	case Client::CPlayer_Manager::PORTION_SP:
		PTM->Create_AlphaParticle(L"SPPortion", PM->Get_PlayerWorldMatrix());
		PTM->Create_InstanceParticle(L"SPPortion", PM->Get_PlayerWorldMatrix());
		break;
	case Client::CPlayer_Manager::PORTION_EP:
		PTM->Create_AlphaParticle(L"EPPortion", PM->Get_PlayerWorldMatrix());
		PTM->Create_InstanceParticle(L"EPPortion", PM->Get_PlayerWorldMatrix());
		break;
	}
}

void CPlayer_Manager::Create_ItemGetEffect(ITEMGETTYPE eType)
{
	switch (eType)
	{
	case Client::CPlayer_Manager::ITEMGET_NORMAL:
		PTM->Create_InstanceParticle(L"NormalItemGet", PM->Get_PlayerWorldMatrix());
		break;
	case Client::CPlayer_Manager::ITEMGET_RARE:
		PTM->Create_AlphaParticle(L"RareItemGet", PM->Get_PlayerWorldMatrix());
		PTM->Create_InstanceParticle(L"RareItemGet", PM->Get_PlayerWorldMatrix());
		break;

	}

}

_bool CPlayer_Manager::Get_PatternSkill()
{
	return m_pPlayer->Get_PatternSkill();
}

void CPlayer_Manager::Player01ChangeOff()
{
	m_pPlayer->Player01ChangeOff();
}

void CPlayer_Manager::PlayerPortal()
{
	m_pPlayer->PlayerPortal();
}

void CPlayer_Manager::Player01HurtVoice()
{
	m_pPlayer->Player01HurtVoice();
}

void CPlayer_Manager::Set_Player04Rage1End()
{
	m_pPlayer->Set_Player04Rage1End();
}

void CPlayer_Manager::Free()
{
	if (m_pPlayer != nullptr)
	{
		delete(m_pPlayer);
		m_pPlayer = nullptr;
	}
}
