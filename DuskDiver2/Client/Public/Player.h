#pragma once

#include "Client_Defines.h"
#include "Base.h"
#include "Creture.h"

BEGIN(Client)

class CPlayer final : public CBase
{
public:
	enum SelectPlayer { PLAYER_01, PLAYER_03, PLAYER_04, PLAYER_END };
				
public:
	CPlayer();
	virtual ~CPlayer() = default;

public:
	HRESULT Initialize();
	HRESULT Add_Players(CCreture* pPlayer);

	void Change_Player(SelectPlayer eSelectPlayer);
	SelectPlayer Get_SelectPlayer() { return m_eSelectPlayer; }
	_float4 Get_PlayerPos();
	_float4x4 Get_PlayerWorldMatrix();
	_float Get_PlayerHP(SelectPlayer eSelectPlayer);
	void Set_PlayerHP(_float fHP, SelectPlayer eSelectPlayer);
	_float Get_PlayerSP(SelectPlayer eSelectPlayer);
	void Set_PlayerSP(_float fSP, SelectPlayer eSelectPlayer);
	_float Get_PlayerEP(SelectPlayer eSelectPlayer);
	void Set_PlayerEP(_float fEP, SelectPlayer eSelectPlayer);
	_float Get_PlayerBP(SelectPlayer eSelectPlayer);
	void Set_PlayerBP(_float fBP, SelectPlayer eSelectPlayer);
	_bool Get_Hurt();
	void Change_Navigation(_uint iNaviNum);
	_int Get_NaviIndex(SelectPlayer eSelectPlayer);
	void Change_PlayerPos(SelectPlayer eSelectPlayer, _float4 vChanges);
	_bool Get_BurstMode(SelectPlayer eSelectPlayer);
	_bool Get_IsIdle();
	_bool Get_PlayerDie(SelectPlayer eSelectPlayer);
	void Set_PlayerRevive(SelectPlayer eSelectPlayer);
	_bool Get_ReHp(SelectPlayer eSelectPlayer);
	CTransform* Get_Transform();
	_bool Get_PlayerJump();	
	_bool Get_PreUp();
	_bool Get_PreDown();
	_float Get_PlayerPreHP(SelectPlayer eSelectPlayer);
	_float Get_PlayerPreSP(SelectPlayer eSelectPlayer);
	_float Get_PlayerPreEP(SelectPlayer eSelectPlayer);
	_bool Get_PatternSkill();
	void Player01ChangeOff();
	void PlayerPortal();
	void Player01HurtVoice();
	//Player04
	void Set_Player04Rage1End();
	
	//Scene
	void Set_ScenePos(SelectPlayer eSelectPlayer, _float4 vPos);
	void Set_SceneLook(SelectPlayer eSelectPlayer, _float4 vPos);
	void Set_SceneIdle(SelectPlayer eSelectPlayer);
	void Set_SceneHit(SelectPlayer eSelectPlayer, CCreture::HITTYPE eHitType, _float fPushPower, _float fAirPower, _float4 vPos, _float3 vDir);
	void Set_SceneRealHit(SelectPlayer eSelectPlayer, CCreture::HITTYPE eHitType, _float fPushPower, _float fAirPower, _float4 vPos, _float3 vDir, _float fDamage);
	_float Get_PlayerBurstSkillOn(SelectPlayer eSelectPlayer);

private:
	vector<CCreture*> m_Players; //플레이어들을 보관하는 벡터
	SelectPlayer m_eSelectPlayer = PLAYER_01; // 현재 어떤 플레이어가 선택됐는지 알려주는 변수

public:
	virtual void Free() override;
};

END