#pragma once
#include "Client_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CModel;
class CEditor;
END

BEGIN(Client)

class CToolManager final : public CBase
{
	DECLARE_SINGLETON(CToolManager)

public:
	CToolManager();
	virtual ~CToolManager() = default;

public:
	HRESULT Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;


public:
	virtual void Free() override;


	// =====================
	//		For. Utility
	// =====================
public:
	/* A와 B 포지션을 보간합니다. */
	_vector Lerp_Pos(_fvector vPos, _fvector vTargetPos, _float fTimeDelta);
	_vector Lerp_PosByT(_fvector vPos, _fvector vTargetPos, _float fT);

	_vector Lerp_Look(_fvector vLook, _fvector vTargetLook, _float fT);


	/* char <-> TChar 변환 함수입니다. */
	void TCtoC(const TCHAR* pTC, char*pC);
	void CtoTC(const char* pC, TCHAR* pTC);
	
	/* 매니저에서 관리해 주는 힙 공간의 char를 반환합니다. 게임 종료 시 자동 해제 */
	_tchar* Get_ManagedTChar();
	char* Get_ManagedChar();
	void ClearManagedChar();



private:
	list<_tchar*> m_ManagedTChar;
	list<char*> m_ManagedChar;


	// =====================
	//		For. Math
	// =====================
public:
	/* fT (0~1)에 따른 베지어 곡선 상 좌표를 구합니다. */
	_float3 GetBesierPos(_float3 vPos1, _float3 vPos2, _float fT);
	_float3 GetBesierPos(_float3 vPos1, _float3 vPos2, _float3 vPos3, _float fT);
	_float3 GetBesierPos(_float3 vPos1, _float3 vPos2, _float3 vPos3, _float3 vPos4, _float fT);

	_bool Get_Loaded(LEVEL eLevel);
	void Set_Loaded(LEVEL eLevel);
	_bool Get_PlayerCreate() { return m_bPlayerCreate; }
	void Set_PlayerCreate() { m_bPlayerCreate = true; }

	// =====================
	//		For. Level
	// =====================
public:
	/* 레벨을 전환합니다. 전환할 때 정리해야할 작업이 있다면 여기서 하세요.*/
	void Change_Level(LEVEL eLevel);
	LEVEL Get_CurLevel() { return m_eLevel; }

private:
	LEVEL m_eLevel = LEVEL_END;

	// =====================
	//		For. Camera
	// =====================
public:
	bool Get_CamaraMove() { return m_bCameraMove; }
	void Set_CameraMove() { m_bCameraMove = !m_bCameraMove; }
private:
	
	bool m_bCameraMove = false;

	_bool m_bLoadTest = false;
	_bool m_bLoadStage1 = false;
	_bool m_bLoadStage2 = false;
	_bool m_bLoadStage3 = false;
	_bool m_bLoadStage4 = false;
	_bool m_bLoadStage5 = false;
	_bool m_bLoadTool = false;
	_bool m_bPlayerCreate = false;

	// =====================
	//		For. TimeDelta
	// =====================
public:
	_float& Get_TimeDelta() { return m_fTimeDelta; }
	void Set_TimeDelta(_float& fTimeDelta) { m_fTimeDelta = fTimeDelta; }
private:
	_float m_fTimeDelta;

	// =====================
	//	For. Tool
	// =====================
public:
	CModel* Get_ModelTool() { return m_pModelTool; }
	void    Set_ModelTool(CModel* pModel) { m_pModelTool = pModel; }

	class CAnimMap_Tool* Get_AnimMap_Tool() { return m_pAnimMap_Tool; }
	void Set_AnimMpa_Tool(class CAnimMap_Tool* pAnimMap_Tool) { m_pAnimMap_Tool = pAnimMap_Tool; }

private:
	CModel* m_pModelTool = nullptr;
	class CAnimMap_Tool* m_pAnimMap_Tool = nullptr;



	// =====================
	//	For. Event
	// =====================
public:
	void Set_Stage2_2Bool(_bool bBool) { m_bBool = bBool; }
	_bool Get_Stage2_2Bool() { return m_bBool; }

	void Set_Stage3_Bool(_bool bBool) {m_bBool3 = bBool; }
	_bool Get_Stage3_Bool() { return m_bBool3; }

private:
	_bool m_bBool = false;
	_bool m_bBool3 = false;


	// =====================
	//	For. MiniGame
	// =====================
public:
	// For. 종이쌓기
	void Start_MiniGame_Block_CutScene();
	void Start_MiniGame_Block();
	class CMiniGame_Block* Get_MiniGameBlock() { return m_pMiniGame_Block; }
	class CBlockCuma* Get_MiniGame_Block_Cuma() { return m_pCuma; }

	// For. 블럭
	void Start_MiniGame_Yabawe();
	class CMiniGame_Yabawe* Get_MiniGameYabawe() { return m_pMiniGame_Yabawe; }
	void End_MiniGame_Block();

	// For. 펀치 게임
	void Start_MiniGame_Funch();
	void Create_MiniGame_Machine();
	void End_MiniGame_Funch();
	class CStage4Machine0* Get_FunchMachine() { return m_pFunchMachine; }
	class CFunchCuma* Get_FunchCuma() { return m_pFunchCuma; }

	// For. 두더지 컷 신
	void Start_CutScene_DG();
	void End_CutScene_DG();

	// For. Potal
	void Make_Potal();
	void Make_EPotal();

	// For. CumaHp
	void Make_CumaHp();
	class CBear_HP* Get_CumaHp() { return m_pCumaHp; }
	class CBear_HP_Bar* Get_CumaHpBar() { return m_pCumaHpBar; }
	void Deal_CumaHp(_float fAt);

	// For. Haki
	void Set_BallingCuma(class CBallingCuma* pObj) { m_pBallingCuma = pObj; }
	class CBallingCuma* Get_BallingCuma() { return m_pBallingCuma; }

	void Set_StartBossCutScene(_bool bBool) { m_bStartBossCutScene = bBool; }
	_bool Get_StartBossCutScene() { return m_bStartBossCutScene; }

	void Set_YavaweBallStart(_bool bBool) { m_bYavaweBallStart = bBool; }
	_bool Get_YavaweBallStart() { return m_bYavaweBallStart; }

	void Set_BossGro(_bool bBossGro) { m_bBossGro = bBossGro; }
	_bool Get_BossGro() { return m_bBossGro; }

	void Set_BossDie(_bool bBossDie) { m_bBossDie = bBossDie; }
	_bool Get_BossDie() { return m_bBossDie; }

	void Set_EndDG() { m_bEndDG = true; }
	_bool Get_EndDG() { return m_bEndDG; }

	// For. Potal
	class CEPotal* Get_EPotal() { return m_pEPotal; };
	class CQPotal* Get_QPotal() { return m_pQPotal; };

	_bool Get_2Page() { return m_b2Page; }
	void Set_2Page(_bool b2Page) { m_b2Page = b2Page; }



private:
	class CMiniGame_Block* m_pMiniGame_Block = nullptr;
	class CMiniGame_Yabawe* m_pMiniGame_Yabawe = nullptr;

	class CBlockCuma* m_pCuma = nullptr;
	class CFunchCuma* m_pFunchCuma = nullptr;
	class CStage4Machine0* m_pFunchMachine = nullptr;

	class CBallingCuma* m_pBallingCuma = nullptr;

	// For. CumaHp
	class CBear_HP* m_pCumaHp = nullptr;
	class CBear_HP_Bar* m_pCumaHpBar = nullptr;

	_bool m_bStartBossCutScene = false;

	_bool m_bYavaweBallStart = false;

	_bool m_bBossGro = false;
	_bool m_bBossDie = false;

	class CEPotal* m_pEPotal = nullptr;
	class CQPotal* m_pQPotal = nullptr;

	_bool m_bEndDG = false;



	_bool m_b2Page = false;

	// =====================
	//	For. Stage4 Trigger
	// =====================
public:
	void Make_Stage4Triggers();
	void Open_Stage4Trigger(_uint iIndex);

private:
	class CStage4Trigger_1* m_pStage4Trigger_1 = nullptr;
	class CStage4Trigger_2* m_pStage4Trigger_2 = nullptr;
	class CStage4Trigger_3* m_pStage4Trigger_3 = nullptr;
	class CStage4Trigger_4* m_pStage4Trigger_4 = nullptr;
	class CStage4Trigger_5* m_pStage4Trigger_5 = nullptr;
	class CStage4Trigger_6* m_pStage4Trigger_6 = nullptr;
	class CStage4Trigger_7* m_pStage4Trigger_7 = nullptr;





public:
	void Set_BossCutBlock(_bool bBossCutBlock) { m_bBossCutBlock = bBossCutBlock; }
	_bool Get_BossCutBlock() { return m_bBossCutBlock; }

private:
	_bool m_bBossCutBlock = false;

};

END

