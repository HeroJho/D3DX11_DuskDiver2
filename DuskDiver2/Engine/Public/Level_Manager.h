#pragma once

#include "Base.h"

/* ���� ��������� ������ �����͸� ������ �ִ´�. */
/* ������ ƽ, ������ ����ȣ���Ѵ�. */
/* ��[����ü�ÿ� �������� ����,. ���ο�� ��ü�Ѵ�. */

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
	DECLARE_SINGLETON(CLevel_Manager)
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	_uint Get_Cur_LevelIndex() { return m_iCurrentLevelIndex; }
	_uint Get_Reserve_LevelIndex() { return m_iReserveLevelIndex; }

public:
	void Reserve_Level(_uint iLevelIndex) { m_iReserveLevelIndex = iLevelIndex; }
	
	
	/* �������� ����. ���ο�� ����. */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);



public:
	void Tick(_float fTimeDelta);
	HRESULT Render();


private:
	class CLevel*				m_pCurrentLevel = nullptr;
	_uint						m_iCurrentLevelIndex = 0;

	_uint						m_iReserveLevelIndex = 0;


public:
	virtual void Free() override;
};

END