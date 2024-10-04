#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)


class CCutSceneManager final : public CBase
{
	DECLARE_SINGLETON(CCutSceneManager)
public:
	enum CUTSCENE { CUT_1, CUT_END };

private:
	CCutSceneManager();
	virtual ~CCutSceneManager() = default;


	// For. Cut_1
public:
	// void Add_Obj_Cut_1_1(CGameObject* pObj);

private:
	


public:
	virtual void Free() override;
};
END

