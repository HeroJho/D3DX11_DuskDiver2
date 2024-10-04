#pragma once
#include "Base.h"
#include "Client_Defines.h"
#include "Camera_Main.h"
#include "CutScene_Tool.h"

BEGIN(Client)

class CCamera_Manager final : public CBase
{
	DECLARE_SINGLETON(CCamera_Manager)

private:
	CCamera_Manager();
	virtual ~CCamera_Manager() = default;

public:
	HRESULT Add_CameraMain(CCamera_Main* pCameraMain);
	void Set_CamMode();
	CCamera_Main::CAMMODE Get_CamMode();

	CCamera_Main* Get_Cam() { return m_pCameraMain; };

private:
	CCamera_Main* m_pCameraMain = nullptr;




	// For. CutSceneTool
public:
	void Set_CutSceneTool(CCutScene_Tool* pCutScene_Tool) { m_pCutScene_Tool = pCutScene_Tool; }
	CCutScene_Tool* Get_CutSceneTool() { return m_pCutScene_Tool; }

private:
	CCutScene_Tool* m_pCutScene_Tool = nullptr;






	// For. CutScene
public:
	HRESULT Init();

	void Tick(_float fTimeDelta);


public:
	_bool Get_Start() { return m_bStart; }
	void Set_Start(_bool bStart);

	_bool Get_IsEnd() { return !m_bStart; }

public:
	void Play_CutScene(_uint iIndex, _bool bAutoEnd, _bool bLerpBack, class CTransform* pTran = nullptr);
	void End_CutScene();


public:
	// For. Common
	void Clear_PosData();
	void Clear_LookData();
	void Clear_Data();

	// For. MarkCube
	void Create_MarkCube(CDataManager::CAMDATA* pData = nullptr);


public:
	// For. LookCube
	void Create_ChaseLookCube();

	void Create_LookCube(CDataManager::CAMDATA* pData = nullptr);


public:
	void PlayCutScene(_float fTimeDelta);

	void PlayMark(_float fTimeDelta);
	void PlayLook(_float fTimeDelta);

	void MakeMarkPos();
	void MakeLookPos();

	_float3 CalculMarkBasi();
	_float3 CalculLookBasi();



	void Load_Data();



	void End_Cut();


private:
	// For. Common
	class CColorCube* m_pChaseLookCube = nullptr;


	// For. Play
	list<class CMarkCube*> m_MarkCubes;
	list<class CMarkCube*> m_TempMarkCubes;
	list<class CMarkCube*> m_pTempBasiMarkCubes;

	list<class CLookCube_HR*> m_LookCubes;
	list<class CLookCube_HR*> m_TempLookCubes;
	list<class CLookCube_HR*> m_pTempBasiLookCubes;

	_bool m_bStart = false;
	_bool m_bAutoEnd = false;
	_bool m_bLerpBack = false;
	_float m_fMarkSpeed = 0.f;
	_float m_fMarkDestDis = 0.f;
	_float m_fMarkTimeAcc = 0.f;
	_float m_fMarkMoveSens = 0.f;
	_float m_fMarkT = 0.f;
	_float3 m_vMarkTempPos;

	_float m_fLookSpeed = 0.f;
	_float m_fLookDestDis = 0.f;
	_float m_fLookTimeAcc = 0.f;
	_float m_fLookMoveSens = 0.f;
	_float m_fLookT = 0.f;
	_float3 m_vLookTempPos;

	_uint m_iCurCutIndex = 0;

private:
	vector<CDataManager::DATA_CAMS*> m_Datas;







public:
	virtual void Free() override;



};
END

