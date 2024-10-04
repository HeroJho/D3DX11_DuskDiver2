#pragma once

#include "Base.h"

BEGIN(Engine)

class CColliderManager final : public CBase
{
	DECLARE_SINGLETON(CColliderManager)

public:
	enum COLLIDERGROUP { COLLIDER_PLAYER, COLLIDER_MONSTER, COLLIDER_NPC, COLLIDER_ITEM, COLLIDER_EM, COLLIDER_END };
	enum COLTYPE { COLTYPE_SPHARE, COLTYPE_AABB, COLTYPE_OBB };

private:
	CColliderManager();
	virtual ~CColliderManager() = default;

public:
	void Add_ColGroup(COLLIDERGROUP eGroup, class CGameObject* pObj);
	void Calcul_ColGroup(COLLIDERGROUP eGroupL, COLLIDERGROUP eGroupR);
	void Clear_ColGroup();


private:
	list<class CGameObject*> m_ObjGroups[COLLIDER_END];


public:
	virtual void Free() override;
};

END