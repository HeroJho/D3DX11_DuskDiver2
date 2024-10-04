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
class CPointTrailObj;
class CSwordTrailObj;

class CPlayer04 final : public CCreture
{
public:
	enum Parts { PARTS_HAIR, PARTS_HAIRBATTLE, PARTS_SWORD, PARTS_SWORD2, PARTS_SWORDBACK, PARTS_END };
	enum Socket { SOCKET_HEAD, SOCKET_WEAPON_R, SOCKET_WEAPONBACK, SOCKET_LKICK, SOCKET_RKICK, SOCKET_BODY, SOCKET_END };
	enum PlayerMode {PMODE_NORMAL, PMODE_BATTLE, PMODE_BURST, PMODE_END};
	enum DIR { DIR_UP, DIR_DOWN, DIR_RIGHT, DIR_LEFT, DIR_LU, DIR_RU, DIR_LD, DIR_RD, DIR_END };
	enum STATE {RUN, WALK, AIRATTACK1, AIRATTACK2, AIRATTACK3, ATTACK1, ATTACK2, ATTACK3, ATTACK4, ATTACK5, BOUNCE, BROKEN, BURST, DASH,
	GROGGYEND, GROGGYLOOF, GROGGYSTART, DOWN, VOIDFRONT, VOIDAIR, VOIDATTACK, FINALBURST, FINISHNORMAL, FINISHWALL, GETUP, SKILL1, HURTBACK,
	HURTFLY, HURTFRONT, IDLE1, IDLE2, JUMPBACK, JUMPLOOF, JUMPSTART, JUMPEND, QICKSTAND, SKILL2, SKILL3START, SKILL3LOOF, SKILL3END,
	SKILL4START, SKILL4END, SKILL5, SKILL6, SKILL7START, SKILL7END, SKILL8, SLAPDOWN, SKILL9, SKILL10, SKILL11START, SKILL11LOOF,
	SKILL11END, SKILL12START, SKILL12END, SKILL13START, SKILL13END, RUNFASTEND, RUNFASTLOOF, RUNFASTSTART, STEPBACK, WALKLEFT, WALKRIGHT,
	STUN, STUNEND, RAGESKILL1, RAGESKILL2, FALLDOWN, HITAWAY, HITSPIN, HITDOWN, WALKBACK			
	};
	enum SHADER { SHADER_DEFAULT, SHADER_RIM, SHADER_LIGHT, SHADER_END };
	enum HITEFFECT { HE_1, HE_RAGE1, HE_END };
	enum EFFECT { ET_ATTACK1, ET_ATTACK2, ET_ATTACK3, ET_ATTACK4, ET_ATTACK5, ET_AIRATTACK1, ET_AIRATTACK2, ET_AIRATTACK3, ET_DASH, 
		ET_SKILL9, ET_SKILLCHARGE, ET_SKILL8, ET_SKILL5, ET_SKILL2_1, ET_SKILL2_2, ET_SKILL11, ET_VOIDEND,
		ET_RAGESKILL1, ET_RAGESKILL1_2, ET_RAGESKILLCUT, ET_SKILL10,
		ET_END };
	enum PARTICLE {PT_VOID, PT_END};
	enum PARTICLEINSTANCE {PTI_SKILL1, PTI_SKILL1_1, PTI_SKILL1_2, PTI_SKILL12, PTI_SKILL12_1, PTI_SKILL12_2, 
		PTI_SKILL4, PTI_SKILL4_1, PTI_BURST,
		PTI_END};
	enum SWORDTRAIL { STR_SWORD, STR_LHAND, STR_RHAND, STR_LKICK, STR_RKICK, STR_END };
	enum POINTTRAIL { PTR_LKICK, PTR_RKICK, PTR_BODY, PTR_END };
private:
	CPlayer04(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer04(const CPlayer04& rhs);
	virtual ~CPlayer04() = default;

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


public:	//매니저용 함수들
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
	void Set_Rage1End();
	_bool Get_PatternSkill() { return m_bPatternSkill; }
	_bool Get_BurstSkillOn() { return m_bBurstSkillOn; }

private://메인 플레이어함수
	void Set_State(STATE eState); // 상태를 설정 보간을 하는 애니메이션은 여기서 애니메이션 셋팅
	void End_Animation(); // 애니메이션이 끝났을 때 작업을 수행 보간을 안할 애니메이션은 여기서 애니메이션 셋팅
	void Set_Dir(DIR eDir); // 방향을 설정
	void Get_KeyInput(_float fTimeDelta); // 현재 상태에 따라 키입력 함수 호출
	void Update(_float fTimeDelta); // 현재 상태에 따라 작업수행
	void Change_Mode();
	void LateTick_Func(_float fTimeDelta);
	void Push_Colliders();
	void Change_ColRange(_float3 vSize, _float3 vCenter);
	void Hit_State(CCreture* pOther);
	void Hit_Effect(CCreture* pOther);
	void Reset_Collider();
	void Check_Burst(_float fTimeDelta, _float OriginTimeDelta);
	void Check_Rim(_float fTimeDelta);
	HRESULT Set_RenderPass(SHADER eShader);
	void Check_HP(_float fTimeDelta);
	void Check_SP(_float fTimeDelta);
	void Check_BP(_float fTimeDelta);
	HRESULT Create_Effect(EFFECT eET);
	HRESULT Create_Particle(PARTICLE ePT);
	HRESULT Create_ParticleInstance(PARTICLEINSTANCE ePT);
	HRESULT Ready_Trails();
	void Create_MotionTrail(_float fTimeDelta);
	void Create_PointTrail(_float fTimeDelta);
	void Create_SwordTrail(_float fTimeDelta);
	void Set_MotionTrail(_float fTimeDelta);
	void Control_KickTrail(_float fTimeDelta);
	void Control_BodyTrail(_float fTimeDelta);
	void Player_Die();
	void Reset_SlowMotionTrail();
	void Set_SlowMotionTrail();
	void Check_PlayerLight(_float fTimeDelta);
	void Die_Sound();
	
private://키입력 함수
	void Default_KeyInput(_float fTimeDelta);
	bool Input_Direction();
	void Idle_KeyInput(_float fTimeDelta);
	void Run_KeyInput(_float fTimeDelta);
	void Run_FastStartInput(_float fTimeDelta);
	void Run_FastInput(_float fTimeDelta);
	void Attack1_KeyInput(_float fTimeDelta);
	void Attack2_KeyInput(_float fTimeDelta);
	void Attack3_KeyInput(_float fTimeDelta);
	void Attack4_KeyInput(_float fTimeDelta);
	void Attack5_KeyInput(_float fTimeDelta);
	void JumpStart_KeyInput(_float fTimeDelta);
	void JumpLoof_KeyInput(_float fTimeDelta);
	void AirAttack1_KeyInput(_float fTimeDelta);
	void AirAttack2_KeyInput(_float fTimeDelta);
	void AirAttack3_KeyInput(_float fTimeDelta);
	void Skill11Loof_KeyInput(_float fTimeDelta);
	void Skill13End_KeyInput(_float fTimeDelta);
	void Combo_KeyInput(_float fTimeDelta);
	void Skill10_KeyInput(_float fTimeDelta);
	void Skill9_KeyInput(_float fTimeDelta);
	void Skill2_KeyInput(_float fTimeDelta);
	void Skill5_KeyInput(_float fTimeDelta);
	void Cancle_KeyInput(_float fTimeDelta);

private://업데이트 함수 
	void Run_Update(_float fTimeDelta);
	void Walk_Update(_float fTimeDelta);
	void AirAttack1_Update(_float fTimeDelta);
	void AirAttack2_Update(_float fTimeDelta);
	void AirAttack3_Update(_float fTimeDelta);
	void Attack1_Update(_float fTimeDelta);
	void Attack2_Update(_float fTimeDelta);
	void Attack3_Update(_float fTimeDelta);
	void Attack4_Update(_float fTimeDelta);
	void Attack5_Update(_float fTimeDelta);
	void Bounce_Update(_float fTimeDelta);
	void Broken_Update(_float fTimeDelta);
	void Burst_Update(_float fTimeDelta);
	void Dash_Update(_float fTimeDelta);
	void GroggyEnd_Update(_float fTimeDelta);
	void GroggyLoof_Update(_float fTimeDelta);
	void GroggyStart_Update(_float fTimeDelta);
	void Down_Update(_float fTimeDelta);
	void VoidFront_Update(_float fTimeDelta);
	void VoidAir_Update(_float fTimeDelta);
	void VoidAttack_Update(_float fTimeDelta);
	void FinalBurst_Update(_float fTimeDelta);
	void FinishNormal_Update(_float fTimeDelta);
	void FinishWall_Update(_float fTimeDelta);
	void GetUp_Update(_float fTimeDelta);
	void Skill1_Update(_float fTimeDelta);
	void HurtBack_Update(_float fTimeDelta);
	void HurtFly_Update(_float fTimeDelta);
	void HurtFront_Update(_float fTimeDelta);
	void Idle1_Update(_float fTimeDelta);
	void Idle2_Update(_float fTimeDelta);
	void JumpBack_Update(_float fTimeDelta);
	void JumpLoof_Update(_float fTimeDelta);
	void JumpStart_Update(_float fTimeDelta);
	void JumpEnd_Update(_float fTimeDelta);
	void QickStand_Update(_float fTimeDelta);
	void Skill2_Update(_float fTimeDelta);
	void Skill3Start_Update(_float fTimeDelta);
	void Skill3Loof_Update(_float fTimeDelta);
	void Skill3End_Update(_float fTimeDelta);
	void Skill4Start_Update(_float fTimeDelta);
	void Skill4End_Update(_float fTimeDelta);
	void Skill5_Update(_float fTimeDelta);
	void Skill6_Update(_float fTimeDelta);
	void Skill7Start_Update(_float fTimeDelta);
	void Skill7End_Update(_float fTimeDelta);
	void Skill8_Update(_float fTimeDelta);
	void SlapDown_Update(_float fTimeDelta);
	void Skill9_Update(_float fTimeDelta);
	void Skill10_Update(_float fTimeDelta);
	void Skill11Start_Update(_float fTimeDelta);
	void Skill11Loof_Update(_float fTimeDelta);
	void Skill11End_Update(_float fTimeDelta);
	void Skill12Start_Update(_float fTimeDelta);
	void Skill12End_Update(_float fTimeDelta);
	void Skill13Start_Update(_float fTimeDelta);
	void Skill13End_Update(_float fTimeDelta);
	void RunFastEnd_Update(_float fTimeDelta);
	void RunFastLoof_Update(_float fTimeDelta);
	void RunFastStart_Update(_float fTimeDelta);
	void StepBack_Update(_float fTimeDelta);
	void Stun_Update(_float fTimeDelta);
	void StunEnd_Update(_float fTimeDelta);
	void RageSkill1_Update(_float fTimeDelta);
	void RageSkill2_Update(_float fTimeDelta);
	void FallDown_Update(_float fTimeDelta);
	void HitAway_Update(_float fTimeDelta);
	void HitSpin_Update(_float fTimeDelta);
	void HitDown_Update(_float fTimeDelta);

#pragma region Effect함수
	void Create_HE_1(_float4 vWorldPos);
	void Create_HE_RAGE1(_float4 vWorldPos);
	void Create_LightEffect();
	void Create_PlayerVoidParticle();

#pragma endregion Effect함수

private://컴포넌트, 파츠관련 함수들
	HRESULT Ready_Components();
	HRESULT Ready_Sockets();
	HRESULT Ready_PlayerParts();
	HRESULT Update_Parts();
	void Render_Parts(_float fTimeDelta);

private: //플레이어 메인변수
	STATE m_eCurState; // 현재 상태
	STATE m_eNextState; // 바꿔야할 상태
	_float3 m_vTargetLook; //바라볼 방향
	DIR m_eDir = DIR_UP; // 현재 방향
	PlayerMode m_ePlayerMode = PMODE_NORMAL; // 어떤모드인지 노말 / 전투 / 각성
	HITEFFECT	m_eHE = HE_1;
	vector<CSwordTrailObj*> m_SwordTrails;
	vector<CPointTrailObj*> m_PointTrails;
	SHADER m_eShaderPass = SHADER_DEFAULT;
	_float3 m_vPushLook = { 0.f,0.f,0.f };
	_bool m_bPlayerLight = false;
	_float m_fPlayerLightAcc = 0.f;
	_float m_fPlayerLightLimit = 0.15f;
	_float3 m_vPlayerLightColor = { 1.f,1.f,1.f };
	_float4 m_vVoidParticleColor = { 1.f,1.f,1.f,1.f };
	_float4 m_vVoidParticleColor2 = { 0.f,0.5f,1.f,1.f };
	_float m_fVoidParticleY = 1.f;

	//_float//
	_float m_fGravity = 40.f;
	_float m_fNowSP = 100.f;
	_float m_fNowEP = 100.f;
	_float m_fNowBP = 0.f;
	_float m_fRimAcc = 0.f;	
	_float m_fMotionTrailAcc = 0.f;
	_float m_fMotionTrailSpeed = 1.f;
	_float m_fMotionTrailLimit = 0.04f;
	_float m_fBurstParticleAcc = 0.f;
	_float m_fBurstParticleLimit = 0.1f;
	_float m_fRunSoundAcc = 0.f;
	_float m_fChargeAcc = 0.f;

	//_uint
	_uint m_iMotionTrailIndex = 0;
	_uint m_iChargeCount = 0;

	//_bool//
	_bool m_bRim = false;
	_bool m_bVoid = false;
	_bool m_bSPUse = false;
	_bool m_bRenderPlayer = true; //플레이어를 랜더 할 것인지
	_bool m_bSelectPlayer = false; //선택된 플레이어인지
	_bool m_bDirKeyInput = false; // 방향을 바꿧는지 안바꿧는지 체크하기위한 변수
	_bool m_bReHp = false;
	_bool m_bDie = false;
	_bool m_bMotionTrail = false;
	_bool m_bVoidOn = false; //피할때 사운드 한번만
	_bool m_bSkill3On = false; //Skill3 사운드 한번만
	_bool m_bPointTrail = false;
	_bool m_bBodyTrail = false;
	_bool m_bOtherFoot = false;
	_bool m_bSwordTrail = false;
	_bool m_bPatternSkill = false;
	_bool m_bSkill5Sound = false;
	_bool m_bBurstSkillOn = false;
	_bool m_bRageSkill1On = false;

private: //플레이어변수
		 //=== _float ===
	_float m_fRunSpeed = 5.f;
	_float m_fJumpSpeed = 5.f;
	_float m_fSkill12Speed = 10.f;
	_float m_fVoidSpeed = 10.f;
	

		//===_bool===
	_bool m_bSwordRender = false;
	_bool m_bNormalChange = false;
	_bool m_bBurstChange = false;
	_bool m_bSkill4Change = false;
	_bool m_bSkill7Change = false;
	_bool m_bSkill1On = false;
	_bool m_bRangeSkill1On = false;

		//===_matrix===
	_float4x4 m_HeadMatrix;
	_float4x4 m_WeaponBack;
	_float4x4 m_Weapon_R;


	//===_bool Hit Collier===
	_bool m_bAttackCol = false;
	//AirAttack1
	_bool m_bAirAttack1Col = false;
	//AirAttack2
	_bool m_bAirAttack2Col = false;
	//AirAttack3
	_bool m_bAirAttack3Col = false;
	//Skill13End
	_bool m_bSkill13End_Col = false;
	//Attack1
	_bool m_bAttack1Col = false;
	//Attack2
	_bool m_bAttack2Col = false;
	//Attack3
	_bool m_bAttack3Col = false;
	//Attack4
	_bool m_bAttack4Col = false;
	//Attack5
	_bool m_bAttack5Col = false;
	//Skill3Start
	_bool m_bSkill3StartCol = false;
	_bool m_bSkill3Jump = false;
	_bool m_bSkill3TickPerSecond = false;
	//Skill3Loof
	_bool m_bSkill3LoofCol = false;
	//Skill3End
	_bool m_bSkill3EndCol = false;
	//VoidAttack
	_bool m_bVoidAttackCol_1 = false;
	_bool m_bVoidAttackCol_2 = false;
	_bool m_bVoidAttackCol_3 = false;
	_bool m_bVoidAttackCol_4 = false;
	_bool m_bVoidAttackCol_5 = false;
	//Skill1
	_bool m_bSkill1Col = false;
	_bool m_bSkill1TrailOn = false;
	//Skill2
	_bool m_bSkill2Col_1 = false;
	_bool m_bSkill2Col_2 = false;
	//Skill4
	_bool m_bSkill4StartCol = false;
	_bool m_bSkill4EndCol = false;
	//Skill5
	_bool m_bSkill5Col_1 = false;
	_bool m_bSkill5Col_2 = false;
	_bool m_bSkill5Col_3 = false;
	_bool m_bSkill5Col_4 = false;
	//Skill6
	_bool m_bSkill6Col = false;
	_bool m_bSkill6Effect = false;
	//Skill7
	_bool m_bSkill7EndCol = false;
	//SKill8
	_bool m_bSkill8Effect = false;
	//Skill9
	_bool m_bSkill9Col = false;
	//Skill10
	_bool m_bSkill10Col = false;
	//Skill11
	_bool m_bSkill11EndCol = false;
	_bool m_bSkill11Charge1 = false;
	_bool m_bSkill11Charge2 = false;
	_bool m_bSkill11Charge3 = false;
	_bool m_bSkill11Charge4 = false;
	_bool m_bSkill11ChargeEnd = false;
	_bool m_bSkill11PlayerLight = false;
	_float m_fSkill11ChargeEndAcc = 0.f;
	//Skill12
	_bool m_bSkill12StartCol = false;
	_bool m_bSkill12EndCol = false;
	//RageSkill1
	_bool m_bRageSkill1Effect = false;
	_bool m_bRageSkill1Effect2 = false;

	_bool m_bHeroTest_1 = false;
	_bool m_bHeroTest_2 = false;
	_bool m_bHeroTest_3 = false;

	//PushRange
	_bool m_bRangeCol = false;
private:// STL 컨테이너
	vector<CParts*>				m_Parts;
	vector<class CHierarchyNode*>		m_Sockets;
	vector<CGameObject*> m_MotionTrails;
private: //컴포넌트
	CShader*				m_pShaderCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;



public:
	static CPlayer04* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;
};

END