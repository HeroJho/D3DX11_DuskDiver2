#pragma once
#include "Base.h"
#include "Client_Defines.h"


BEGIN(Client)

class CStatus_Manager final : public CBase
{
	DECLARE_SINGLETON(CStatus_Manager)

public:
	enum PLAYERTYPE {PLAYER01, PLAYER03, PLAYER04, PLAYEREND};
public:
	typedef struct tagStatusInfo
	{
		_uint iHp = 1;
		_uint iMental = 1; //SP소모량감소
		_uint iMeta = 1; // SP회복력
		_uint iStamina = 1; //방어력
		_uint iStr = 1; //공격력
		_uint iExplosive = 1; //버스트
		_uint iLuck = 1; //크리티컬
		_float fWeaponStr = 0; //무기공격력
		_float2 vHat = { 0, 0 }; //머리 : 버스트, 크리티컬
		_float2 vClothes = { 0, 0 }; // 옷 : 체력, 방어력
		_float2 vDeco = { 0, 0 }; // 장신구 : SP소모량감소, SP회복력증가
		_uint FoodStr = 0; // 음식 공격력
		_float FoodHp = 0; // 음식 체력
		_float fFinalHp = 0.f;
		_float fFinalMental = 0.f;
		_float fFinalMeta = 0.f;
		_float fFinalStamina = 0.f;
		_float fFinalStr = 0.f;
		_float fFinalExplosive = 0.f;
		_float fFinalCritical = 0.f;
	}STATUSINFO;
private:
	CStatus_Manager();
	virtual ~CStatus_Manager() = default;

public:
	void Set_Hp(_uint iHp, PLAYERTYPE ePlayer);
	void Set_Mental(_uint iMental, PLAYERTYPE ePlayer);
	void Set_Meta(_uint iMeta, PLAYERTYPE ePlayer);
	void Set_Stamina(_uint iStamina, PLAYERTYPE ePlayer);
	void Set_Str(_uint iStr, PLAYERTYPE ePlayer);
	void Set_Explosive(_uint iExplosive, PLAYERTYPE ePlayer);
	void Set_Luck(_uint iLuck, PLAYERTYPE ePlayer);
	void Set_WeaponStr(_float fWeaponStr, PLAYERTYPE ePlayer);
	void Set_Hat(_float2 vHat, PLAYERTYPE ePlayer);
	void Set_Clothes(_float2 vClothes, PLAYERTYPE ePlayer);
	void Set_Deco(_float2 vDeco, PLAYERTYPE ePlayer);
	void Set_FoodStr(_uint iStr, PLAYERTYPE ePlayer);
	void Set_FoodHp(_float fHp, PLAYERTYPE ePlayer);
	void Set_FinalHp(PLAYERTYPE ePlayer);
	void Set_FinalMental(PLAYERTYPE ePlayer);
	void Set_FinalMeta(PLAYERTYPE ePlayer);
	void Set_FinalStamina(PLAYERTYPE ePlayer);
	void Set_FinalStr(PLAYERTYPE ePlayer);
	void Set_FinalExplosive(PLAYERTYPE ePlayer);
	void Set_FinalCritical(PLAYERTYPE ePlayer);
	_float Get_FinalHp(PLAYERTYPE ePlayer);
	_float Get_FinalMental(PLAYERTYPE ePlayer);
	_float Get_FinalMeta(PLAYERTYPE ePlayer);
	_float Get_FinalStamina(PLAYERTYPE ePlayer);
	_float Get_FinalStr(PLAYERTYPE ePlayer);
	_float Get_FinalExplosive(PLAYERTYPE ePlayer);
	_float Get_FinalCritical(PLAYERTYPE ePlayer);
	

private:
	vector<STATUSINFO> m_PlayerInfos;

public:
	virtual void Free() override;
};
END

