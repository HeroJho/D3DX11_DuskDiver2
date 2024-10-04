#include "stdafx.h"
#include "..\Public\Time_Manager.h"
#include "GameInstance.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CTime_Manager)

CTime_Manager::CTime_Manager()
{

}

HRESULT CTime_Manager::Add_TimeObj(OBJGROUP eObj, CGameObject * pObject)
{
	if (pObject == nullptr)
		return E_FAIL;
	m_Objects[eObj].push_back(pObject);
	return S_OK;
}

HRESULT CTime_Manager::Fix_ObjTimer(OBJGROUP eObj, _float fRatio, _float fTimerLimit)
{
	for (auto& iter : m_Objects[eObj])
	{
		iter->Fix_Timer(fRatio, fTimerLimit);
	}
	return S_OK;

}

HRESULT CTime_Manager::Fix_All(_float fRatio, _float fTimerLimit)
{
	for (int i = 0; i < OBJ_END; ++i)
	{
		for (auto& iter : m_Objects[i])
		{
			iter->Fix_Timer(fRatio, fTimerLimit);
		}
	}

	return S_OK;
}

HRESULT CTime_Manager::Fix_PlayerSP(_float fRatio, _float fTimerLimit)
{
	for (int i = 0; i < OBJ_END; ++i)
	{
		if (i != OBJ_PLAYER)
		{
			for (auto& iter : m_Objects[i])
			{
				iter->Fix_Timer(fRatio, fTimerLimit);
			}
		}
	}
	return S_OK;
}

HRESULT CTime_Manager::Reset_All()
{
	for (int i = 0; i < OBJ_END; ++i)
	{
		for (auto& iter : m_Objects[i])
		{
			iter->Reset_Timer();
		}
	}
	
	return S_OK;
}

void CTime_Manager::Tick_TimeManager(_float fTimeDelta)
{
	for (int i = 0; i < OBJ_END; ++i)
	{
		for (auto& iter : m_Objects[i])
		{
			iter->Tick_Timer(fTimeDelta);
		}

		m_Objects[i].clear();
	}


}




void CTime_Manager::Free()
{
	
}
