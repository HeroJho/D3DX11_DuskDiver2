 #pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Player.h"
#include "Creture.h"

BEGIN(Client)
class CPlayer_Manager final : public CBase
{
	DECLARE_SINGLETON(CPlayer_Manager)
public:
	enum PORTIONTYPE {PORTION_HP, PORTION_SP, PORTION_EP, PORTION_END};
	enum ITEMGETTYPE{ ITEMGET_NORMAL, ITEMGET_RARE, ITEM_END };

private:
	CPlayer_Manager();
	virtual ~CPlayer_Manager() = default;

public:
	HRESULT Create_Players();
	HRESULT Add_Player(CCreture* pPlayers);
	HRESULT Release_Player();
	_float4 Get_PlayerPos();
	void Change_Player(CPlayer::SelectPlayer eSelectPlayer);
	CPlayer::SelectPlayer Get_SelectPlayer();
	_float4x4 Get_PlayerWorldMatrix();
	_float Get_PlayerHP(CPlayer::SelectPlayer eSelectPlayer);
	void Set_PlayerHP(_float fHP, CPlayer::SelectPlayer eSelectPlayer);
	_float Get_PlayerSP(CPlayer::SelectPlayer eSelectPlayer);
	void Set_PlayerSP(_float fSP, CPlayer::SelectPlayer eSelectPlayer);
	_float Get_PlayerEP(CPlayer::SelectPlayer eSelectPlayer);
	void Set_PlayerEP(_float fEP, CPlayer::SelectPlayer eSelectPlayer);
	_float Get_PlayerBP(CPlayer::SelectPlayer eSelectPlayer);
	void Set_PlayerBP(_float fBP, CPlayer::SelectPlayer eSelectPlayer);
	_bool Get_Hurt() { return m_pPlayer->Get_Hurt(); }
	void Change_Navigation(_uint iNaviNum);
	_bool Get_BurstMode(CPlayer::SelectPlayer eSelectPlayer);
	_bool Get_PlayerIsIdle();
	_bool Get_PlayerDie(CPlayer::SelectPlayer eSelectPlayer);
	void Set_PlayerRevive(CPlayer::SelectPlayer eSelectPlayer);
	_bool Get_ReHp(CPlayer::SelectPlayer eSelectPlayer);
	CTransform* Get_Transform();
	_bool Get_PlayerPreUp();
	_bool Get_PlayerPreDown();
	_float Get_PlayerPreHP(CPlayer::SelectPlayer eSelectPlayer);
	_float Get_PlayerPreSP(CPlayer::SelectPlayer eSelectPlayer);
	_float Get_PlayerPreEP(CPlayer::SelectPlayer eSelectPlayer);
	_bool Get_PlayerJump();
	_float Get_PlayerBurstSkillOn(CPlayer::SelectPlayer eSelectPlayer);

	_int Get_NaviIndex(CPlayer::SelectPlayer eSelectPlayer);
	void Change_PlayerPos(CPlayer::SelectPlayer eSelectPlayer, _float4 vChanges);
	void Create_PortionEffect(PORTIONTYPE eType);
	void Create_ItemGetEffect(ITEMGETTYPE eType);
	_bool Get_PatternSkill();
	void Player01ChangeOff();
	void PlayerPortal();
	void Player01HurtVoice();

	//Player04
	void Set_Player04Rage1End();

	void Set_PlayerTick(_bool bPlayerTick) { m_bPlayerTick = bPlayerTick; }
	_bool Get_PlayerTick() { return m_bPlayerTick; }



	//Scene
	void Set_ScenePos(CPlayer::SelectPlayer eSelectPlayer, _float4 vPos);
	void Set_SceneLook(CPlayer::SelectPlayer eSelectPlayer, _float4 vPos);
	void Set_SceneIdle(CPlayer::SelectPlayer eSelectPlayer);
	void Set_SceneHit(CPlayer::SelectPlayer eSelectPlayer, CCreture::HITTYPE eHitType, _float fPushPower, _float fAirPower, _float4 vPos, _float3 vDir);
	void Set_SceneRealHit(CPlayer::SelectPlayer eSelectPlayer, CCreture::HITTYPE eHitType, _float fPushPower, _float fAirPower, _float4 vPos, _float3 vDir, _float fDamage);


private:
	CPlayer* m_pPlayer = nullptr;
	_bool m_bPlayerTick = true;

public:
	virtual void Free() override;
};
END

