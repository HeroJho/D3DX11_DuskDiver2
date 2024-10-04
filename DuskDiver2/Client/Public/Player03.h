#pragma once

#include "Client_Defines.h"
#include "Creture.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CNavigation;
class CHierarchyNode;
END

BEGIN(Client)
class CParts;
class CSwordTrailObj;
class CPointTrailObj;

class CPlayer03 final : public CCreture
{
public:
	enum Parts { PARTS_HAIRA, PARTS_HAIRB, PARTS_LGUN, PARTS_RGUN, PARTS_SHOTGUN, PARTS_LSHOTGUN, PARTS_CANNON, PARTS_LCANNON, PARTS_SNIPER, PARTS_END };
	enum Socket { SOCKET_HEAD, SOCKET_WEAPON_L, SOCKET_WEAPON_R, SOCKET_LKICK, SOCKET_RKICK, SOCKET_END };
	enum PlayerMode {PMODE_NORMAL, PMODE_BATTLE, PMODE_BURST, PMODE_END};
	enum DIR { DIR_UP, DIR_DOWN, DIR_RIGHT, DIR_LEFT, DIR_LU, DIR_RU, DIR_LD, DIR_RD, DIR_END };
	enum STATE {NORMALRUN, NORMALWALK, AIRATTACK1, AIRATTACK2, AIRATTACK2_END, AIRATTACK2_LOOF, AIRATTACK2_LOOFSTART, AIRATTACK2_START,
	ATTACK1, ATTACK2, ATTACK3, ATTACK4, ATTACK5, BOUNCE, BREAK, BROKEN, BURST, GROGGYEND, GROGGYLOOF, GROGGYSTART, DOWN, VOIDFRONT,
	VOIDAIR, VOIDATTACK, RAGESKILL1, FINISHNORMAL, FINISHWALL, GETUP, SKILL1, HURTBACK, HURTFLY, HURTFRONT, IDLE1, IDLE2, IDLE3,
	JUMPLOOF, JUMPSTART, JUMPEND, QICKSTAND, RUN, SKILL2, SKILL3, SKILL3AIR, SKILL4, SKILL5END, SKILL5LOOF, SKILL5START,
	SKILL6, SKILL7END, SKILL7SHOT, SKILL7LOOF, SKILL7START, SKILL8, SLAPDOWN, SKILL9, SKILL10END, SKILL10LOOF, SKILL10START, SKILL11,
	SKILL12, SKILL13, RUNFASTEND, RUNFASTLOOF, RUNFASTSTART, STEPBACK, WALKLEFT, WALKRIGHT, STUN, STUNEND, RAGESKILL2,
	FALLDOWN, HITAWAY, HITSPIN, HITDOWN};
	enum SHADER { SHADER_DEFAULT, SHADER_RIM, SHADER_LIGHT, SHADER_END };
	enum HITEFFECT {HE_1, HE_2, HE_3, HE_END};
	enum PARTICLE {PT_SNIPERSHOT, PT_SKILL13, PT_SKILL1_1, PT_SKILL1_2, PT_SKILL9, PT_SKILL2_1, PT_SKILL2_2,
		PT_ATTACK1, PT_ATTACK2, PT_ATTACK3,
		PT_END};
	enum PARTICLEINSTANCE {PTI_SKILL13, PTI_SKILL1_1, PTI_SKILL1_2, PTI_SKILL9, PTI_SKILL2_1BULLET, PTI_SKILL2_2BULLET,
		PTI_SKILL7, PTI_SKILL7_1, PTI_ATTACK1, PTI_ATTACK2, PTI_ATTACK3, PTI_BURST, PTI_RGUNPARTICLE, PTI_LGUNPARTICLE,
		PTI_LSHOTGUN, PTI_RSHOTGUN, PTI_RAGEGUN, PTI_RAGELSHOTGUN, PTI_RAGERSHOTGUN, 
		PTI_END};
	enum EFFECT { ET_DASH, ET_SKILLCHARGE, ET_RSHOTGUN, ET_LSHOTGUN, ET_SKILL11, 
		ET_CANNONBULLET, ET_RAGECANNON, ET_RAGECANNONBULLET, ET_BOMB,
		ET_END };
	enum SWORDTRAIL { STR_LHAND, STR_RHAND, STR_LKICK, STR_RKICK, STR_END };
	enum POINTTRAIL { PTR_LKICK, PTR_RKICK, PTR_LHAND, PTR_RHAND, PTR_END };

private:
	CPlayer03(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer03(const CPlayer03& rhs);
	virtual ~CPlayer03() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	virtual void On_Hit(CCreture* pOther)override;
	virtual void Create_HitEffect(_float4 vWorldPos)override;

	//매니저용 함수들
public:
	_float4 Get_PlayerPos();
	void Change_PlayerPos(_float4 vChanges);
	void Set_SelectPlayer(_bool bSelectPlayer) { m_bSelectPlayer = bSelectPlayer; }
	DIR Get_Dir() { return m_eDir; }
	void Set_ChangeLook(DIR eDir, _float3 vDir);
	_int Get_NaviIndex();
	void Set_NaviIndex(_int iNaviIndex);
	void Reset_Animation();
	_float Get_PreHP() { return m_fPreHp; }
	_float Get_PreSP() { return m_fPreSp; }
	_float Get_PreEP() { return m_fPreEp; }
	_float Get_SP() { return m_fNowSP; }
	void Set_SP(_float fSP) { m_fNowSP = fSP; }
	_float Get_EP() { return m_fNowEP; }
	void Set_EP(_float fEP) { m_fNowEP = fEP; }
	_float Get_BP() { return m_fNowBP; }
	void Set_BP(_float fBP) { m_fNowBP = fBP; }
	HRESULT Change_Navigation(_uint iNaviNum);
	void Change_Tag();
	void Create_TagEffect();
	void Change_HairCut(_bool bOnOff);
	_bool Get_BurstMode();
	_bool Get_IsIdle();
	_bool Get_PreUp();
	_bool Get_PreDown();
	_bool Get_Die() { return m_bDie; }
	void Set_Revive() { m_bDie = false; m_bReHp = true; m_bVoid = false; m_fNowEP = 0.f; m_fPreEp = 0.f; }
	_bool Get_ReHp() { return m_bReHp; }
	void Change_Sound();
	_bool Get_IsJump();
	void Set_ScenePos(_float4 vPos);
	void Set_SceneLook(_float4 vPos);
	void Set_SceneIdle();
	void Set_SceneHit(HITTYPE eHitType, _float fPushPower, _float fAirPower, _float4 vPos, _float3 vDir);
	void Set_SceneRealHit(HITTYPE eHitType, _float fPushPower, _float fAirPower, _float4 vPos, _float3 vDir, _float fDamage);
	_bool Get_PatternSkill() { return m_bPatternSkill; }
	_bool Get_BurstSkillOn() { return m_bBurstSkillOn; }

private://메인함수
	void Set_State(STATE eState); // 상태를 설정 보간을 하는 애니메이션은 여기서 애니메이션 셋팅
	void Set_Dir(DIR eDir); // 방향을 설정
	void End_Animation(); // 애니메이션이 끝났을 때 작업을 수행 보간을 안할 애니메이션은 여기서 애니메이션 셋팅
	void Get_KeyInput(_float fTimeDelta); // 현재 상태에 따라 키입력 함수 호출
	void Update(_float fTimeDelta); // 현재 상태에 따라 작업수행
	void LateTick_Func(_float fTimeDelta);
	void Push_Colliders();
	void Change_Mode();
	void Change_ColRange(_float3 vSize, _float3 vCenter);
	void Hit_State(CCreture* pOther);
	void Hit_Effect(CCreture* pOther);
	void Reset_Collider();
	void Check_Burst(_float fTimeDelta, _float OriginTimeDelta);
	void Check_Rim(_float fTimeDelta);
	HRESULT Set_RenderPass(SHADER eShader);
	void Reset_Weapon();
	void Check_HP(_float fTimeDelta);
	void Check_SP(_float fTimeDelta);
	void Check_BP(_float fTimeDelta);
	HRESULT Create_Particle(PARTICLE ePT);
	HRESULT Create_ParticleInstance(PARTICLEINSTANCE ePTI);
	HRESULT Create_Effect(EFFECT eET);
	HRESULT Ready_Trails();
	void Create_MotionTrail(_float fTimeDelta);
	void Create_PointTrail(_float fTimeDelta);
	void Create_SwordTrail(_float fTimeDelta);
	void Set_MotionTrail(_float fTimeDelta);
	void Control_KickTrail(_float fTimeDelta);
	void Control_HandTrail(_float fTimeDelta);
	void Player_Die();
	void Reset_SlowMotionTrail();
	void Set_SlowMotionTrail();
	void Check_PlayerLight(_float fTimeDelta);
	void Die_Sound();
private://키입력 함수
	bool Input_Direction();
	void Idle_KeyInput(_float fTimeDelta);
	void Run_KeyInput(_float fTimeDelta);
	void Run_FastStartInput(_float fTimeDelta);
	void Run_FastInput(_float fTimeDelta);
	void JumpLoof_KeyInput(_float fTimeDelta);
	void Attack1_KeyInput(_float fTimeDelta);
	void Attack2_KeyInput(_float fTimeDelta);
	void Attack3_KeyInput(_float fTimeDelta);
	void Attack4_KeyInput(_float fTimeDelta);
	void Attack5_KeyInput(_float fTimeDelta);
	void Skill7Loof_KeyInput(_float fTimeDelta);
	void AirAttack2Loof_KeyInput(_float fTimeDelta);
	void Default_KeyInput(_float fTimeDelta);
private://Update 함수
	void AirAttack1_Update(_float fTimeDelta);
	void AirAttack2_Update(_float fTimeDelta);
	void AirAttack2End_Update(_float fTimeDelta);
	void AirAttack2Loof_Update(_float fTimeDelta);
	void AirAttack2LoofStart_Update(_float fTimeDelta);
	void AirAttack2Start_Update(_float fTimeDelta);
	void Attack1_Update(_float fTimeDelta);
	void Attack2_Update(_float fTimeDelta);
	void Attack3_Update(_float fTimeDelta);
	void Attack4_Update(_float fTimeDelta);
	void Attack5_Update(_float fTimeDelta);
	void Bounce_Update(_float fTimeDelta);
	void Broken_Update(_float fTimeDelta);
	void Burst_Update(_float fTimeDelta);
	void GroggyEnd_Update(_float fTimeDelta);
	void GroggyLoof_Update(_float fTimeDelta);
	void GroggyStart_Update(_float fTimeDelta);
	void Down_Update(_float fTimeDelta);
	void VoidFront_Update(_float fTimeDelta);
	void VoidAir_Update(_float fTimeDelta);
	void VoidAttack_Update(_float fTimeDelta);
	void RageSkill1_Update(_float fTimeDelta);
	void FinishNormal_Update(_float fTimeDelta);
	void FinishWall_Update(_float fTimeDelta);
	void GetUp_Update(_float fTimeDelta);
	void Skill1_Update(_float fTimeDelta);
	void HurtBack_Update(_float fTimeDelta);
	void HurtFly_Update(_float fTimeDelta);
	void HurtFront_Update(_float fTimeDelta);
	void Idle1_Update(_float fTimeDelta);
	void Idle2_Update(_float fTimeDelta);
	void Idle3_Update(_float fTimeDelta);
	void JumpLoof_Update(_float fTimeDelta);
	void JumpStart_Update(_float fTimeDelta);
	void JumpEnd_Update(_float fTimeDelta);
	void QickStand_Update(_float fTimeDelta);
	void Run_Update(_float fTimeDelta);
	void Skill2_Update(_float fTimeDelta);
	void Skill3_Update(_float fTimeDelta);
	void Skill3Air_Update(_float fTimeDelta);
	void Skill4_Update(_float fTimeDelta);
	void Skill5End_Update(_float fTimeDelta);
	void Skill5Loof_Update(_float fTimeDelta);
	void Skill5Start_Update(_float fTimeDelta);
	void Skill6_Update(_float fTimeDelta);
	void Skill7End_Update(_float fTimeDelta);
	void Skill7Shot_Update(_float fTimeDelta);
	void Skill7Loof_Update(_float fTimeDelta);
	void Skill7Start_Update(_float fTimeDelta);
	void Skill8_Update(_float fTimeDelta);
	void SlapDown_Update(_float fTimeDelta);
	void Skill9_Update(_float fTimeDelta);
	void Skill10End_Update(_float fTimeDelta);
	void Skill10Loof_Update(_float fTimeDelta);
	void Skill10Start_Update(_float fTimeDelta);
	void Skill11_Update(_float fTimeDelta);
	void Skill12_Update(_float fTimeDelta);
	void Skill13_Update(_float fTimeDelta);
	void RunFastEnd_Update(_float fTimeDelta);
	void RunFastLoof_Update(_float fTimeDelta);
	void RunFastStart_Update(_float fTimeDelta);
	void StepBack_Update(_float fTimeDelta);
	void Stun_Update(_float fTimeDelta);
	void StunEnd_Update(_float fTimeDelta);
	void RageSkill2_Update(_float fTimeDelta);
	void FallDown_Update(_float fTimeDelta);
	void HitAway_Update(_float fTimeDelta);
	void HitSpin_Update(_float fTimeDelta);
	void HitDown_Update(_float fTimeDelta);

#pragma region Effect함수
	void Create_HE_1(_float4 vWorldPos);
	void Create_HE_2(_float4 vWorldPos);
	void Create_HE_3(_float4 vWorldPos);
	void Create_LightEffect();

#pragma endregion Effect함수

private://컴포넌트, 파츠관련 함수들
	HRESULT Ready_Components();
	HRESULT Ready_Sockets();
	HRESULT Ready_PlayerParts();
	HRESULT Update_Parts();
	void Render_Parts(_float fTimeDelta);

private: //메인플레이어 변수
	STATE m_eCurState; // 현재 상태
	STATE m_eNextState; // 바꿔야할 상태
	_float3 m_vTargetLook; //바라볼 방향
	DIR m_eDir = DIR_UP; // 현재 방향
	PlayerMode m_ePlayerMode = PMODE_NORMAL; // 어떤모드인지 노말 / 전투 / 각성
	HITEFFECT	m_eHE = HE_1;
	vector<CSwordTrailObj*> m_SwordTrails;
	vector<CPointTrailObj*> m_PointTrails;
	_float3 m_vPushLook = { 0.f,0.f,0.f };
	SHADER m_eShaderPass = SHADER_DEFAULT;
	_bool m_bPlayerLight = false;
	_float m_fPlayerLightAcc = 0.f;
	_float m_fPlayerLightLimit = 0.15f;
	_float3 m_vPlayerLightColor = { 1.f,1.f,1.f };
	_bool m_bPatternSkill = false;

	_float fTest1;
	_float fTest2;
	//_float//
	_float m_fGravity = 40.f;
	_float m_fNowSP = 100.f;
	_float m_fNowEP = 100.f;
	_float m_fNowBP = 0.f;
	_float m_fRimAcc = 0.f;
	_float m_fBlurPower = 0.f;
	_float m_fMotionTrailAcc = 0.f;
	_float m_fMotionTrailSpeed = 1.f;
	_float m_fMotionTrailLimit = 0.04f;
	_float m_fBurstParticleAcc = 0.f;
	_float m_fBurstParticleLimit = 0.1f;
	_float m_fRunSoundAcc = 0.f;

	//int//
	_uint m_iMotionTrailIndex = 0;

	//_bool//
	_bool m_bDirKeyInput = false; // 방향을 바꿧는지 안바꿧는지 체크하기위한 변수
	_bool m_bSelectPlayer = false; //선택된 플레이어인지
	_bool m_bRim = false;
	_bool m_bVoid = false;
	_bool m_bSPUse = false;
	_bool m_bHairChange = false;
	_bool m_bMotionTrail = false;
	_bool m_bReHp = false;
	_bool m_bDie = false;
	_bool m_bVoidOn = false; //피할때 사운드 한번만
	_bool m_bAirOn = false; //AirAttack할때 사운드 한번만
	_bool m_bSniperOn = false; //스나이퍼 켜질때 한번만
	_bool m_bSkill3On = false; //skill3 한번만
	_bool m_bSkill9On = false;//skill9 한번만
	_bool m_bSkill9Re = false;//skill9 reload 한번만
	_bool m_bPointTrail = false;
	_bool m_bLHandTrail = false;
	_bool m_bRHandTrail = false;
	_bool m_bOtherFoot = false;
	_bool m_bBurstSkillOn = false;

private: //플레이어 함수용 변수
		//=== matrix ===
	_float4x4 m_HeadMatrix;
	_float4x4 m_WeaponLMatrix;
	_float4x4 m_WeaponRMatrix;

		 //=== _float ===
	_float m_fRunSpeed = 5.f;
	_float m_fJumpSpeed = 5.f;
	_float m_fSkill5Speed = 10.f;
	_float m_fAirAttack1Speed = 5.f;
	_float m_fAirAttack2Speed = 2.f;
	_float m_fSkill7Speed = 10.f;
	_float m_fVoidAttackSpeed = 10.f;
	_float m_fVoidSpeed = 10.f;
	

		//===int ===
	_uint m_iSkill7Count = 0;

		 //===_bool ---
	_bool m_bLGunRender = false;
	_bool m_bRGunRender = false;
	_bool m_bShotGunRender = false;
	_bool m_bLShotGunRender = false;
	_bool m_bCannonRender = false;
	_bool m_bLCannonRender = false;
	_bool m_bSniperRender = false;
	_bool m_bSniperEnd = false;


	//===_bool Hit Collier===
	_bool m_bAttackCol = false;
	//AirAttack1
	_bool m_bAirAttack1Col = false;
	//AirAttack2
	_bool m_bAirAttack2Col_1 = false;
	_bool m_bAirAttack2Col_2 = false;
	//AirAttack2Loof
	_bool m_bAirAttack2LoofCol_1 = false;
	_bool m_bAirAttack2LoofCol_2 = false;
	//Attack1
	_bool m_bAttack1Col = false;
	//Attack2
	_bool m_bAttack2Col = false;
	//Attack3
	_bool m_bAttack3Col = false;
	//Attack4
	_bool m_bAttack4Col_1 = false;
	_bool m_bAttack4Col_2 = false;
	_bool m_bAttack4Col_3 = false;
	_bool m_bAttack4Col_4 = false;
	_bool m_bAttack4Col_5 = false;
	_bool m_bAttack4Col_6 = false;
	//Attack5
	_bool m_bAttack5Col = false;
	//Skill1
	_bool m_bSkill1Col_1 = false;
	_bool m_bSkill1Col_2 = false;
	_bool m_bSkill1Col_3 = false;
	//Skill2
	_bool m_bSkill2Col_1 = false;
	_bool m_bSkill2Col_2 = false;
	_bool m_bSkill2Col_3 = false;
	_bool m_bSkill2Col_4 = false;
	_bool m_bSkill2Col_5 = false;
	_bool m_bSkill2Col_6 = false;
	_bool m_bSkill2Col_7 = false;
	_bool m_bSkill2Col_8 = false;
	_bool m_bSkill2Col_9 = false;
	//Skill5Start
	_bool m_bSkill5StartCol = false;
	//Skill5Loof
	_bool m_bSkill5LoofCol_1 = false;
	_bool m_bSkill5LoofCol_2 = false;
	_bool m_bSkill5LoofCol_3 = false;
	//Skill5End
	_bool m_bSkill5EndCol_1 = false;
	_bool m_bSkill5EndCol_2 = false;
	//Skill6
	_bool m_bSkill6Col_1 = false;
	_bool m_bSkill6Col_2 = false;
	_bool m_bSkill6Col_3 = false;
	//Skill9
	_bool m_bSkill9Col = false;
	//Skill11
	_bool m_bSkill11Col_1 = false;
	_bool m_bSkill11Col_2 = false;
	//Skill12
	_bool m_bSkill12Col_1 = false;
	_bool m_bSkill12Col_2 = false;
	_bool m_bSkill12Col_3 = false;
	_bool m_bSkill12Col_4 = false;
	_bool m_bSkill12Col_5 = false;
	//Skill13
	_bool m_bSkill13Col = false;
	//RageSkill2
	_bool m_bRageSkill2Col_1 = false;
	_bool m_bRageSkill2Col_2 = false;
	_bool m_bRageSkill2Col_3 = false;
	_bool m_bRageSkill2Col_4 = false;
	_bool m_bRageSkill2Col_5= false;
	_bool m_bRageSkill2Col_6 = false;
	_bool m_bRageSkill2Col_7 = false;
	_bool m_bRageSkill2Col_8 = false;
	_bool m_bRageSkill2Col_9 = false;
	_bool m_bRageSkill2Col_10 = false;
	_bool m_bRageSkill2Col_11 = false;
	_bool m_bRageSkill2Col_12 = false;
	_bool m_bRageSkill2Col_13 = false;
	_bool m_bRageSkill2Col_14 = false;
	_bool m_bRageSkill2Trail = false;
	_bool m_bRageChargeEnd = false;
	_bool m_bRageChargeSound = false;
	_float m_fRageParticleAcc = 0.f;
	//VoidAttack
	_bool m_bVoidAttackCol_1 = false;
	_bool m_bVoidAttackCol_2 = false;
	_bool m_bVoidAttackCol_3 = false;
	_bool m_bVoidAttackCol_4 = false;
	//PushRange
	_bool m_bRangeCol = false;

	_bool m_bHeroTest_1 = false;
	_bool m_bHeroTest_2 = false;
	_bool m_bHeroTest_3 = false;



private:// STL 컨테이너
	vector<CParts*>				m_Parts;
	vector<class CHierarchyNode*>		m_Sockets;
	vector<CGameObject*> m_MotionTrails;
private: // 컴포넌트
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;



public:
	static CPlayer03* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END