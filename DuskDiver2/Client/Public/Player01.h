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

class CPlayer01 final : public CCreture
{
public:
	enum AnimModel { MODEL_NORMAL, MODEL_BURST, MODEL_END };
	enum Parts { PARTS_HAIRA, PARTS_HAIRB, PARTS_HAIRC, PARTS_END };
	enum Socket { SOCKET_HEAD, SOCKET_HEAD2, SOCKET_WEAPON_L, SOCKET_WEAPON_L2, SOCKET_WEAPON_R, SOCKET_WEAPON_R2, 
		SOCKET_KICK_L, SOCKET_KICK_L2, SOCKET_KICK_R, SOCKET_KICK_R2, SOCKET_END };
	enum PlayerMode {PMODE_NORMAL, PMODE_BATTLE, PMODE_BURST, PMODE_END};
	enum DIR { DIR_UP, DIR_DOWN, DIR_RIGHT, DIR_LEFT, DIR_LU, DIR_RU, DIR_LD, DIR_RD, DIR_END };
	enum STATE {AIRATTACK1, AIRATTACK2, AIRATTACK3, AIRATTACK4, AIRATTACK5END, AIRATTACK5LOOF, AIRATTACK5START,
				ATTACK1, ATTACK2, ATTACK3, ATTACK4, ATTACK5, BOUNCE, CHANGEBATTLE, DASH, GROGGYEND, GROGGYLOOF, GROGGYSTART, DOWN,
				VOIDFRONT, VOIDAIR, VOIDATTACK, CHANGEBURST, FINISHNORMAL, FINISHKICK, GETUP, RUSHKICK, HURTBACK, HURTFLY, HURTFRONT,
				IDLE1, IDLE2, JUMPLOOF, JUMPSTART, JUMPEND, FALLUP, RUN, SKILL1, SKILL2, SKILL3, SKILL4_START, SKILL4_LOOF, SKILL4_END,
				SKILL5, SKILL6, SKILL6_END, SKILL6_LOOF, SKILL6_START, SKILL7, SKILL8, FALL, SKILL9, SKILL10, SKILL11, RUNFASTEND,
				RUNFAST, RUNFASTSTART, STEPBACK, LEFTSTEP, RIGHTSTEP, STUN, STUNEND, RAGESKILL1, RAGESKILL2, FALLDOWN, HITAWAY,
				HITSPIN, HITDOWN };
	enum SHADER {SHADER_DEFAULT, SHADER_RIM, SHADER_LIGHT, SHADER_END};
	enum HITEFFECT { HE_1, HE_2, HE_END };
	enum PARTICLE { PT_SUNDER, PT_END };
	enum PARTICLEINSTANCE { PTI_BURST, PTI_END};
	enum EFFECT {ET_SUNDER, ET_ATTACK1, ET_ATTACK2, ET_ATTACK3, ET_ATTACK4, ET_ATTACK5, ET_DASH, ET_AIRATTACK1,
		ET_AIRATTACK2, ET_AIRATTACK3, ET_SKILL4LEFT, ET_SKILL4RIGHT, ET_SKILL3_1, ET_SKILL3_2, ET_SKILL4END,
		ET_SKILLCHARGE, ET_SKILL5, ET_SKILL9, ET_SKILL1, ET_LANDING, ET_SKILL5END, ET_RAGESKILL1, ET_RAGESKILL1END,
		ET_CHANGEBURST, ET_RAGESKILL2, ET_RAGESKILL2_2, ET_RAGESKILL2_3, ET_RAGESKILL2END, ET_SKILL6START,
		ET_RUSHKICK, ET_SKILL7, ET_SKILL7_2, ET_SKILL7_3,
		
		ET_END};
	enum SWORDTRAIL {STR_LHAND, STR_RHAND, STR_LKICK, STR_RKICK, STR_END};
	enum POINTTRAIL {PTR_LKICK, PTR_RKICK, PTR_BODY, PTR_LHAND, PTR_RHAND, PTR_END };
private:
	CPlayer01(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer01(const CPlayer01& rhs);
	virtual ~CPlayer01() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();
	virtual HRESULT Render_ShadowDepth();
	virtual void Render_Debug() override;
	virtual void OnCollision(CCollider::OTHERTOMECOLDESC Desc) override;
	virtual void On_Hit(CCreture* pOther)override;
	virtual void Create_HitEffect(_float4 vWorldPos)override;

#pragma region For Manager함수
public://매니저용 함수들
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
	void Set_BP(_float fBP) {m_fNowBP = fBP; }
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
	void PlayerPortal();
	void ChangeOff();
	void HurtVoice();

#pragma region Main함수
private: //플레이어 메인 함수
	void LateTick_Func(_float fTimeDelta);
	void Set_State(STATE eState); // 상태를 설정 보간을 하는 애니메이션은 여기서 애니메이션 셋팅
	void Set_Dir(DIR eDir); // 방향을 설정
	void End_Animation(); // 애니메이션이 끝났을 때 작업을 수행 보간을 안할 애니메이션은 여기서 애니메이션 셋팅
	void Get_KeyInput(_float fTimeDelta); // 현재 상태에 따라 키입력 함수 호출
	void Update(_float fTimeDelta); // 현재 상태에 따라 작업수행
	void Set_AniInfo(); // 애니메이션 정보 동기화	
	void Change_Mode(); // 변신 키인풋
	void Push_Collider(); // 콜라이더 매니저등록함수
	void Change_Update(STATE eState, _float fTimeDelta); //변신 시 동작 함수
	void Change_ColRange(_float3 vSize, _float3 vCenter);
	void Hit_State(CCreture* pOther);
	void Reset_Collider();
	void Check_Burst(_float fTimeDelta, _float OriginTimeDelta);
	void Check_Rim(_float fTimeDelta);
	HRESULT Set_RenderPass(SHADER eShader);
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
	void Control_BodyTrail(_float fTimeDelta);
	void Control_HandTrail(_float fTimeDelta);
	void Player_Die();
	void Reset_SlowMotionTrail();
	void Set_SlowMotionTrail();
	void Set_Rage2MotionTrail();
	void Check_PlayerLight(_float fTimeDelta);
	void Die_Sound();
	void Hit_Effect(CCreture* pOther);
#pragma endregion Main함수

#pragma region keyInput함수
private://키입력 함수
	void Default_KeyInput(_float fTimeDelta);
	bool Input_Direction();
	void Idle_KeyInput(_float fTimeDelta);
	void Run_KeyInput(_float fTimeDelta);
	void Run_FastStartInput(_float fTimeDelta);
	void Run_FastInput(_float fTimeDelta);
	void JumpLoof_KeyInput(_float fTimeDelta);
	void JumpStart_KeyInput(_float fTimeDelta);
	void AirAttack1_KeyInput(_float fTimeDelta);
	void AirAttack2_KeyInput(_float fTimeDelta);
	void AirAttack3_KeyInput(_float fTimeDelta);
	void AirAttack5_KeyInput(_float fTimeDelta);
	void Attack1_KeyInput(_float fTimeDelta);
	void Attack2_KeyInput(_float fTimeDelta);
	void Attack3_KeyInput(_float fTimeDelta);
	void Attack4_KeyInput(_float fTimeDelta);
	void Attack5_KeyInput(_float fTimeDelta);
	void Skill4_Loof_KeyInput(_float fTimeDelta);
	void Skill6_KeyInput(_float fTimeDelta);
	void VoidFront_KeyInput(_float fTimeDelta);
	void Skill8_KeyInput(_float fTimeDelta);
	void Combo_KeyInput(_float fTimeDelta);
	void RushKick_KeyInput(_float fTimeDelta);
	void Skill1_KeyInput(_float fTimeDelta);
	void Skill2_KeyInput(_float fTimeDelta);
	void Skill3_KeyInput(_float fTimeDelta);
	void Skill4End_KeyInput(_float fTimeDelta);
	void Skill5_KeyInput(_float fTimeDelta);
	void Skill7_KeyInput(_float fTimeDelta);
	void Skill9_KeyInput(_float fTimeDelta);
#pragma endregion KeyInput함수

#pragma region Update함수
private://Update 함수
	void AirAttack1_Update(_float fTimeDelta);
	void AirAttack2_Update(_float fTimeDelta);
	void AirAttack3_Update(_float fTimeDelta);
	void AirAttack4_Update(_float fTimeDelta);
	void AirAttack5End_Update(_float fTimeDelta);
	void AirAttack5Loof_Update(_float fTimeDelta);
	void AirAttack5Start_Update(_float fTimeDelta);
	void Attack1_Update(_float fTimeDelta);
	void Attack2_Update(_float fTimeDelta);
	void Attack3_Update(_float fTimeDelta);
	void Attack4_Update(_float fTimeDelta);
	void Attack5_Update(_float fTimeDelta);
	void GroggyEnd_Update(_float fTimeDelta);
	void GroggyLoof_Update(_float fTimeDelta);
	void GroggyStart_Update(_float fTimeDelta);
	void VoidFront_Update(_float fTimeDelta);
	void VoidAttack_Update(_float fTimeDelta);
	void RushKick_Update(_float fTimeDelta);
	void JumpLoof_Update(_float fTimeDelta);
	void JumpStart_Update(_float fTimeDelta);
	void JumpEnd_Update(_float fTimeDelta);
	void Run_Update(_float fTimeDelta);
	void RunFast_Update(_float fTimeDelta);
	void Skill1_Update(_float fTimeDelta);
	void Skill2_Update(_float fTimeDelta);
	void Skill3_Update(_float fTimeDelta);
	void Skill4Start_Update(_float fTimeDelta);
	void Skill4Loof_Update(_float fTimeDelta);
	void Skill4End_Update(_float fTimeDelta);
	void Skill5_Update(_float fTimeDelta);
	void Skill6_Update(_float fTimeDelta);
	void Skill6Start_Update(_float fTimeDelta);
	void Skill6Loof_Update(_float fTimeDelta);
	void Skill6End_Update(_float fTimeDelta);
	void Skill7_Update(_float fTimeDelta);
	void Skill8_Update(_float fTimeDelta);
	void Skill9_Update(_float fTimeDelta);
	void Skill10_Update(_float fTimeDelta);
	void Skill11_Update(_float fTimeDelta);
	void RageSkill1_Update(_float fTimeDelta);
	void RageSkill2_Update(_float fTimeDelta);
	void FallDown_Update(_float fTimeDelta);
	void HitAway_Update(_float fTimeDelta);
	void HitSpin_Update(_float fTimeDelta);
	void HitDown_Update(_float fTimeDelta);
	void HurtBack_Update(_float fTimeDelta);
	void HurtFront_Update(_float fTimeDelta);
#pragma endregion Update함수

#pragma region Effect함수
	void Create_HE_1(_float4 vWorldPos);
	void Create_HE_2(_float4 vWorldPos);
	void Create_RageCutScene();
	HRESULT Create_Sunder();
	void Create_Pillar();
	void Create_LightEffect();

#pragma endregion Effect함수


private://컴포넌트, 파츠관련 함수들
	HRESULT Ready_Components();
	HRESULT Ready_Sockets();
	HRESULT Ready_PlayerParts();
	HRESULT Update_Parts();
	void Render_Parts(_float fTimeDelta);

private: //메인플레이어 변수
	STATE		m_eCurState = IDLE1; // 현재 상태
	STATE		m_eNextState = IDLE1; // 바꿔야할 상태
	DIR			m_eDir = DIR_UP; // 현재 방향
	PlayerMode	m_ePlayerMode = PMODE_NORMAL; // 어떤모드인지 노말 / 전투 / 각성
	AnimModel	m_eAnimModel = MODEL_NORMAL; // 어떤 애니메이션 모델을 사용할것인지 노말 / 각성
	HITEFFECT	m_eHE = HE_1;
	vector<CSwordTrailObj*> m_SwordTrails;
	vector<CPointTrailObj*> m_PointTrails;
	_float3 m_vPushLook = { 0.f,0.f,0.f };
	SHADER m_eShaderPass = SHADER_DEFAULT;
	_bool m_bPlayerLight = false;
	_float m_fPlayerLightAcc = 0.f;
	_float m_fPlayerLightLimit = 0.15f;
	_float3 m_vPlayerLightColor = { 1.f,1.f,1.f };
	_float m_fChangeEffectAcc = 0.f;

	//===_bool===
	_bool m_bDirKeyInput = false; // 방향을 바꿧는지 안바꿧는지 체크하기위한 변수
	_bool m_bSelectPlayer = true; // 현재 선택된 플레이어인지
	_bool m_bAction = false; // 상태가 변할 수 있는 상태인지 아닌지
	_bool m_bAttackCol = false; // 범위기 콜라이더 충돌
	_bool m_bRangeCol = false; // 밀어내는 콜라이더 충돌
	_bool m_bRim = false;
	_bool m_bVoid = false;
	_bool m_bSPUse = false;
	_bool ShakeBool = false;
	_bool m_bReHp = false;
	_bool m_bMotionTrail = false;
	_bool m_bDie = false;
	_bool m_bVoidOn = false; //피할때 사운드 한번만
	_bool m_bTestPower = false;
	_bool m_bLKickTrail = false;
	_bool m_bRKickTrail = false;
	_bool m_bBodyTrail = false;
	_bool m_bRageSkill2 = false;
	_bool m_bBurstEffect = true;
	_bool m_bLHandTrail = false;
	_bool m_bRHandTrail = false;
	_bool m_bOtherFoot = false;
	_bool m_bPatternSkill = false;
	_bool m_bBurstSkillOn = false;
	_bool m_bDebugRun = false;
	_bool m_bDebugHit = false;

	//===_float===
	_float m_fGravity = 40.f; //중력
	_float m_fNowSP = 100.f; // 현재 SP
	_float m_fNowEP = 100.f; // 현재 EP
	_float m_fNowBP = 0.f; // 현재 BP
	_float m_fHitAirPower = 0.f; // 피격 시 공중으로 뜨는 힘
	_float m_fHitPushPower = 0.f; // 피격 시 밀려나는힘
	_float m_fRimAcc = 0.f; //림라이트 지속시간
	_float m_fMotionTrailAcc = 0.f;
	_float m_fMotionTrailSpeed = 1.f;
	_float m_fMotionTrailLimit = 0.04f;
	_float m_fBurstParticleAcc = 0.f;
	_float m_fBurstParticleLimit = 0.1f;
	_float m_fRunSoundAcc = 0.f;
	_float m_fRageCutSceneAcc = 0.f;
	
	//uint
	_uint m_iMotionTrailIndex = 0;

	//===_float3,4//
	_float3	m_vTargetLook; //바라볼 방향
	_float3	m_fHitDir; //맞은 방향

#pragma region 함수용 변수
private: //플레이어 함수용 변수
	//=== _float ===
	_float m_fRunSpeed = 5.f;
	_float m_fJumpSpeed = 5.f;
	_float m_fVoidSpeed = 10.f;
	_float m_fParticleY = 1.f;

	//=== _bool ===
	_bool m_bSkill4Loof = false;
	_bool m_bBurstChange = false;
	_bool m_bBattleChange = false;
	_bool m_bShadow = false;

	//===_bool Hit Collier===
	//AirAttack
	_bool m_bAirAttack1Col = false;
	_bool m_bAirAttack2Col = false;
	_bool m_bAirAttack3Col = false;
	_bool m_bAirAttack5Col = false;
	//Attack
	_bool m_bAttack1Col = false;
	_bool m_bAttack2Col = false;
	_bool m_bAttack3Col = false;
	_bool m_bAttack4Col = false;
	_bool m_bAttack5Col = false;
	//VoidAttack
	_bool m_bVoidAttackCol = false;
	//RushKick
	_bool m_bRushKickCol = false;
	//Skill1
	_bool m_bSkill1Col = false;
	//SKill2
	_bool m_bSkill2Col_1 = false;
	_bool m_bSkill2Col_2 = false;
	_bool m_bSkill2Col_3 = false;
	_bool m_bSkill2Col_4 = false;
	_bool m_bSkill2Sound = false;
	//SKill3
	_bool m_bSkill3Col_1 = false;
	_bool m_bSkill3Col_2 = false;
	_bool m_bSkill3Effect = false;
	_bool m_bSkill3Effect2 = false;
	//SKill4
	_bool m_bSkill4LoofCol_1 = false;
	_bool m_bSkill4LoofCol_2 = false;
	_bool m_bSkill4EndCol = false;
	_bool m_bSkill4EndSlowStart = false;
	_bool m_bSkill4EndSlowEnd = false;
	//SKill5
	_bool m_bSkill5Col = false;
	_bool m_bSkill5Effect = false;
	_bool m_bSkill5Effect2 = false;
	//SKill6
	_bool m_bSkill6Col = false;
	_bool m_bSkill6StartCol = false;
	_bool m_bSkill6StartSlowStart = false;
	_bool m_bSkill6StartEffect = false;
	_bool m_bSkill6StartSlowEnd = false;
	_bool m_bSkill6JumpEnd = false;
	//Skill7
	_bool m_bSkill7Col_1 = false;
	_bool m_bSkill7Col_2 = false;
	_bool m_bSkill7Col_3 = false;
	_bool m_bSkill7Col_4 = false;
	_bool m_bSkill7Col_5 = false;
	_bool m_bSkill7Col_6 = false;
	//Skill8
	_bool m_bSkill8Col = false;
	//Skill9
	_bool m_bSkill9Col = false;
	//Skill10, Skill11
	_bool m_bSkill10Col = false;
	_bool m_bSkill11Col = false;
	//RageSkill1
	_bool m_bRageSkill1Col_1 = false;
	_bool m_bRageSkill1Col_2 = false;
	_bool m_bRageSkill1Col_3 = false;
	_bool m_bRageSkill1Col_4 = false;
	_bool m_bRageSkill1Col_5 = false;
	_bool m_bRageSkill1Col_6 = false;
	_bool m_bRageSkill1Col_7 = false;
	_bool m_bRageSkill1Effect = false;
	//RageSkill2
	_float m_fRageSkill1ParticleAcc = 0.f;
	_bool m_bRageSkill2Effect = false;
	_bool m_bRageSkill2Col_1 = false;
	_bool m_bRageSkill2Col_2 = false;
	_bool m_bRageSkill2Col_3 = false;
	_bool m_bRageSkill2Col_4 = false;
	_bool m_bRageSkill2Col_5 = false;
	_bool m_bRageSkill2Col_6 = false;
	_bool m_bRageSkill2Col_7 = false;
	_bool m_bRageSkill2Col_8 = false;
	_bool m_bRageSkill2Col_9 = false;
	_bool m_bRageSkill2Col_10 = false;
	_bool m_bRageSkill2Col_11 = false;
	_bool m_bRageSkill2Col_12 = false;
	_bool m_bRageSkill2Col_13 = false;
	_bool m_bRageSkill2Col_14 = false;


	_bool m_bChangeBurstSound = false;
	
	_float fShakeTest1 = 0.f;
	_float fShakeTest2 = 0.f;
	_float fShakeTest3 = 0.f;
#pragma endregion 함수용변수


	_bool m_bHeroTest = false;
	_bool m_bHeroTest1 = false;
	_bool m_bHeroTest2 = false;
	_bool m_bHeroTest3 = false;
	_bool m_bHeroTest4 = false;
	_bool m_bHeroTest5 = false;
	_bool m_bHeroTest6 = false;
	_bool m_bHeroTest7 = false;
	_bool m_bHeroTest8 = false;
	_bool m_bHeroTest9 = false;
	_bool m_bHeroTest10 = false;
	_bool m_bHeroTest11 = false;


private:// STL 컨테이너
	vector<CParts*>				m_Parts;
	vector<class CHierarchyNode*>		m_Sockets;
	vector<CGameObject*> m_MotionTrails;

private: //컴포넌트
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom[MODEL_END];
	CNavigation*			m_pNavigationCom = nullptr;

public:
	static CPlayer01* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END