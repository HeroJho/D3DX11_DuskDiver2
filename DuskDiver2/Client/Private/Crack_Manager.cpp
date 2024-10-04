#include "stdafx.h"
#include "..\Public\Crack_Manager.h"
#include "GameInstance.h"

#include "Fissure.h"
#include "CrackStone.h"

IMPLEMENT_SINGLETON(CCrack_Manager)

CCrack_Manager::CCrack_Manager()
{
}

void CCrack_Manager::Create_Crack(_float4 vPosition, _float fRange, _uint iFissureIndex, _uint iCurLevel)
{
	CFissure::FISSUREDESC FissureDesc;
	FissureDesc.fRange = fRange;
	FissureDesc.vPos = vPosition;
	FissureDesc.iIndex = iFissureIndex;
	GI->Add_GameObjectToLayer(TEXT("Fissure"), iCurLevel, TEXT("Layer_Fissure"), &FissureDesc);
		
	CCrackStone::CRACKDESC Desc;
	Desc.vPos = *(_float3*)(&vPosition);
	Desc.fRange = fRange;
	GI->Add_GameObjectToLayer(TEXT("CrackStone"), iCurLevel, TEXT("Layer_CrackStone"), &Desc);
}

void CCrack_Manager::BackUp_Fissure(CFissure * pFissure)
{
	m_pFissure = pFissure;
}

void CCrack_Manager::BackUp_CrackStone(CCrackStone * pCrackStone)
{
	m_pCrackStone = pCrackStone;
}

void CCrack_Manager::Clear_Fissure()
{
	if (nullptr != m_pFissure)
		m_pFissure = nullptr;
}

void CCrack_Manager::Clear_CrackStone()
{
	if (nullptr != m_pCrackStone)
		m_pCrackStone = nullptr;
}

_float4 CCrack_Manager::Get_CrackPos()
{
	return m_pFissure->Get_FissurePos();
}

_bool CCrack_Manager::Get_CrackExist()
{
	// 리턴이 true이면 균열이 존재
	if (nullptr != m_pFissure)
		return true;

	return false;
}



void CCrack_Manager::Free()
{
}
