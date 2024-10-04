#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)


class CTime_Manager final : public CBase
{
	DECLARE_SINGLETON(CTime_Manager)
	
public:
	enum OBJGROUP {OBJ_PLAYER, OBJ_MONSTER, OBJ_EFFECT, OBJ_PARTICLE, OBJ_END};
private:
	CTime_Manager();
	virtual ~CTime_Manager() = default;

public:
	HRESULT Add_TimeObj(OBJGROUP eObj, CGameObject* pObject);
	HRESULT Fix_ObjTimer(OBJGROUP eObj, _float fRatio, _float fTimerLimit);
	HRESULT Fix_All(_float fRatio, _float fTimerLimit);
	HRESULT Fix_PlayerSP(_float fRatio, _float fTimerLimit);
	HRESULT Reset_All();
	void Tick_TimeManager(_float fTimeDelta);

private:
	list<CGameObject*> m_Objects[OBJ_END];

public:
	virtual void Free() override;
};
END

