#include "stdafx.h"
#include "..\Public\Effect_Manager.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "Player04WhiteBall.h"
#include "BeastTornado.h"

IMPLEMENT_SINGLETON(CEffect_Manager)

CEffect_Manager::CEffect_Manager()
{
}


void CEffect_Manager::Add_Player04WhiteBall(CPlayer04WhiteBall * pWhiteBall)
{
	if (pWhiteBall != nullptr)
		m_pPlayer04WhiteBall = pWhiteBall;
}

void CEffect_Manager::Release_Player04WHiteBall()
{
	m_pPlayer04WhiteBall = nullptr;
}

void CEffect_Manager::Create_RageParticleEffect(_uint iIndex)
{
	if (m_pPlayer04WhiteBall != nullptr)
		m_pPlayer04WhiteBall->Create_RageParticleEffect(iIndex);
}

_float4 CEffect_Manager::Get_Player04WhiteBallPos()
{
	if (m_pPlayer04WhiteBall != nullptr)
		return m_pPlayer04WhiteBall->Get_BallPos();

	return _float4();
}

void CEffect_Manager::Release_RageParticle()
{
	m_iRageParticleCount -= 1;
	if (m_iRageParticleCount == 0)
		m_bRageParticleOn = false;
}

_bool CEffect_Manager::Get_BeastTornadoEnd()
{
	if(m_pBeastTornado != nullptr)
		return m_pBeastTornado->Get_TornadoEnd();

	return false;
}

void CEffect_Manager::Add_BeastTornado(CBeastTornado * pTornado)
{
	if (pTornado != nullptr)
		m_pBeastTornado = pTornado;
}

void CEffect_Manager::ReleaseTornado()
{
	m_pBeastTornado = nullptr;
}

void CEffect_Manager::Free()
{
	if (m_pPlayer04WhiteBall != nullptr)
		Safe_Release(m_pPlayer04WhiteBall);

	if (m_pBeastTornado != nullptr)
		Safe_Release(m_pBeastTornado);
}


