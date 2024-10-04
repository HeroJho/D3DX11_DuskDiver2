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
		_uint iMental = 1; //SP�Ҹ𷮰���
		_uint iMeta = 1; // SPȸ����
		_uint iStamina = 1; //����
		_uint iStr = 1; //���ݷ�
		_uint iExplosive = 1; //����Ʈ
		_uint iLuck = 1; //ũ��Ƽ��
		_float fWeaponStr = 0; //������ݷ�
		_float2 vHat = { 0, 0 }; //�Ӹ� : ����Ʈ, ũ��Ƽ��
		_float2 vClothes = { 0, 0 }; // �� : ü��, ����
		_float2 vDeco = { 0, 0 }; // ��ű� : SP�Ҹ𷮰���, SPȸ��������
		_uint FoodStr = 0; // ���� ���ݷ�
		_float FoodHp = 0; // ���� ü��
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

