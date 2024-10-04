#include "stdafx.h"
#include "..\Public\Status_Manager.h"
#include "GameInstance.h"


IMPLEMENT_SINGLETON(CStatus_Manager)

CStatus_Manager::CStatus_Manager()
{
	STATUSINFO Player01;
	STATUSINFO Player03;
	STATUSINFO Player04;
	m_PlayerInfos.push_back(Player01);
	m_PlayerInfos.push_back(Player03);
	m_PlayerInfos.push_back(Player04);
}

void CStatus_Manager::Set_Hp(_uint iHp, PLAYERTYPE ePlayer)
{
	m_PlayerInfos[ePlayer].iHp = iHp;
}

void CStatus_Manager::Set_Mental(_uint iMental, PLAYERTYPE ePlayer)
{
	m_PlayerInfos[ePlayer].iMental = iMental;
}

void CStatus_Manager::Set_Meta(_uint iMeta, PLAYERTYPE ePlayer)
{
	m_PlayerInfos[ePlayer].iMeta = iMeta;
}

void CStatus_Manager::Set_Stamina(_uint iStamina, PLAYERTYPE ePlayer)
{
	m_PlayerInfos[ePlayer].iStamina = iStamina;
}

void CStatus_Manager::Set_Str(_uint iStr, PLAYERTYPE ePlayer)
{
	m_PlayerInfos[ePlayer].iStr = iStr;
}

void CStatus_Manager::Set_Explosive(_uint iExplosive, PLAYERTYPE ePlayer)
{
	m_PlayerInfos[ePlayer].iExplosive = iExplosive;
}

void CStatus_Manager::Set_Luck(_uint iLuck, PLAYERTYPE ePlayer)
{
	m_PlayerInfos[ePlayer].iLuck = iLuck;
}

void CStatus_Manager::Set_WeaponStr(_float fWeaponStr, PLAYERTYPE ePlayer)
{
	m_PlayerInfos[ePlayer].fWeaponStr = fWeaponStr;
}

void CStatus_Manager::Set_Hat(_float2 vHat, PLAYERTYPE ePlayer)
{
	m_PlayerInfos[ePlayer].vHat = vHat;
}

void CStatus_Manager::Set_Clothes(_float2 vClothes, PLAYERTYPE ePlayer)
{
	m_PlayerInfos[ePlayer].vClothes = vClothes;
}

void CStatus_Manager::Set_Deco(_float2 vDeco, PLAYERTYPE ePlayer)
{
	m_PlayerInfos[ePlayer].vDeco = vDeco;
}

void CStatus_Manager::Set_FoodStr(_uint iStr, PLAYERTYPE ePlayer)
{
	m_PlayerInfos[ePlayer].FoodStr = iStr;
}

void CStatus_Manager::Set_FoodHp(_float fHp, PLAYERTYPE ePlayer)
{
	m_PlayerInfos[ePlayer].FoodHp = fHp;
}

void CStatus_Manager::Set_FinalHp(PLAYERTYPE ePlayer)
{
	m_PlayerInfos[ePlayer].fFinalHp = 500.f + (50.f * ((_float)m_PlayerInfos[ePlayer].iHp) + m_PlayerInfos[ePlayer].vClothes.y + m_PlayerInfos[ePlayer].FoodHp);
}

void CStatus_Manager::Set_FinalMental(PLAYERTYPE ePlayer)
{
	m_PlayerInfos[ePlayer].fFinalMental = (_float)m_PlayerInfos[ePlayer].iMental + m_PlayerInfos[ePlayer].vDeco.x;
}

void CStatus_Manager::Set_FinalMeta(PLAYERTYPE ePlayer)
{
	m_PlayerInfos[ePlayer].fFinalMeta = (_float)m_PlayerInfos[ePlayer].iMeta + m_PlayerInfos[ePlayer].vDeco.y;
}

void CStatus_Manager::Set_FinalStamina(PLAYERTYPE ePlayer)
{
	m_PlayerInfos[ePlayer].fFinalStamina = 10.f + ((_float)m_PlayerInfos[ePlayer].iStamina * 4.f) + m_PlayerInfos[ePlayer].vClothes.x;
}

void CStatus_Manager::Set_FinalStr(PLAYERTYPE ePlayer)
{
	m_PlayerInfos[ePlayer].fFinalStr = 30.f + ((_float)m_PlayerInfos[ePlayer].iStr * 20.f) + m_PlayerInfos[ePlayer].fWeaponStr + m_PlayerInfos[ePlayer].FoodStr;
}

void CStatus_Manager::Set_FinalExplosive(PLAYERTYPE ePlayer)
{
	m_PlayerInfos[ePlayer].fFinalExplosive = (_float)m_PlayerInfos[ePlayer].iExplosive + m_PlayerInfos[ePlayer].vHat.x;
}

void CStatus_Manager::Set_FinalCritical(PLAYERTYPE ePlayer)
{
	m_PlayerInfos[ePlayer].fFinalCritical = (_float)m_PlayerInfos[ePlayer].iLuck + m_PlayerInfos[ePlayer].vHat.y;
}

_float CStatus_Manager::Get_FinalHp(PLAYERTYPE ePlayer)
{
	return m_PlayerInfos[ePlayer].fFinalHp;
}

_float CStatus_Manager::Get_FinalMental(PLAYERTYPE ePlayer)
{
	return m_PlayerInfos[ePlayer].fFinalMental;
}

_float CStatus_Manager::Get_FinalMeta(PLAYERTYPE ePlayer)
{
	return m_PlayerInfos[ePlayer].fFinalMeta;
}

_float CStatus_Manager::Get_FinalStamina(PLAYERTYPE ePlayer)
{
	return m_PlayerInfos[ePlayer].fFinalStamina;
}

_float CStatus_Manager::Get_FinalStr(PLAYERTYPE ePlayer)
{
	return m_PlayerInfos[ePlayer].fFinalStr;
}

_float CStatus_Manager::Get_FinalExplosive(PLAYERTYPE ePlayer)
{
	return m_PlayerInfos[ePlayer].fFinalExplosive;
}

_float CStatus_Manager::Get_FinalCritical(PLAYERTYPE ePlayer)
{
	return m_PlayerInfos[ePlayer].fFinalCritical;
}


void CStatus_Manager::Free()
{

}
