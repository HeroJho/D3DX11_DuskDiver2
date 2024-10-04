#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "NPC_T.h"
#include "DataManager.h"

BEGIN(Engine)
class CTransform;
class CRenderer;
END


BEGIN(Client)

class CCutScene_Tool : public CGameObject
{
public:
	typedef struct CamData {

		_float3 vPos;
		_float fSpeedAndStopTime;
		_float fSens;
		_float fDestDis;
		_bool bMark;

	}CAMDATA;


private:
	CCutScene_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCutScene_Tool(const CCutScene_Tool& rhs);
	virtual ~CCutScene_Tool() = default;


public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void LateTick(_float fTimeDelta);
	virtual HRESULT Render();

	virtual void Render_Debug() override;



private:
	HRESULT Ready_Components();


private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;

public:
	static CCutScene_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg);
	virtual void Free() override;



public:
	_float3 Get_SelectingCubePos();
	void Set_SelectingCube(_float3 vPos);

	string Get_SelectedMarkCubeTag() { return m_sSelectedMarkCubeTag; }
	void Set_SelectedMarkCubeTag(string sSelectedMarkCubeTag) { m_sSelectedMarkCubeTag = sSelectedMarkCubeTag; }

	_bool Get_Start() { return m_bStart; }
	void Set_Start(_bool bStart);

	_bool Get_ShowCube() { return m_bShowCube; }
	void Set_ShowCube(_bool bShowCube) { m_bShowCube = bShowCube; }

	_uint Get_Index() { return m_iSaveIndex; }
	void Set_Index(_uint iSaveIndex) { m_iSaveIndex = iSaveIndex; }

	_float Get_CubeSize() { return m_fCubeSize; }
	void Set_CubeSize(_float fCubeSize) { m_fCubeSize = fCubeSize; }

	const list<class CMarkCube*>* Get_MarkCubes() { return &m_MarkCubes; }
	const list<class CLookCube_HR*>* Get_LookCubes() { return &m_LookCubes; }

public:
	// For. Common
	void Create_SelectingCube();
	void Set_SelectingCubePosToSelectedCube();
	void Set_SelectingCubePosToCamPos();
	CGameObject* Get_SelectedCube();


	void Clear_PosData();
	void Clear_LookData();
	void Clear_Data();

	// For. MarkCube
	void Create_MarkCube(CDataManager::CAMDATA* pData = nullptr);
	void Delete_MarkCube();
	// For. RenderCube
	void Create_RenderPosCube(_float3 vPos, _bool vMark);

public:
	// For. LookCube
	void Create_ChaseLookCube();

	void Create_LookCube(CDataManager::CAMDATA* pData = nullptr);
	void Delete_LookCube();


public: // .For CutScene
	void PlayCutScene(_float fTimeDelta);

	void PlayMark(_float fTimeDelta);
	void PlayLook(_float fTimeDelta);

	void MakeMarkPos();
	void MakeLookPos();

	_float3 CalculMarkBasi();
	_float3 CalculLookBasi();


	// For. RenderCutScene
	void MakeRenderPos();
	void MakeMarkRenderPos();
	void MakeLookRenderPos();
	void RenderPos(_int iSens);

	void CalculRenderBasi(list<CAMDATA> Poss);

	void GetBesierRenderPos(CAMDATA vPos1);
	void GetBesierRenderPos(CAMDATA vPos1, CAMDATA vPos2);
	void GetBesierRenderPos(CAMDATA vPos1, CAMDATA vPos2, CAMDATA vPos3);
	void GetBesierRenderPos(CAMDATA vPos1, CAMDATA vPos2, CAMDATA vPos3, CAMDATA vPos4);


	void Save_Data();
	void Load_Data();


	private:
		// For. Common
		class CCamSelectingCube* m_pSelectingCube = nullptr;
		class CColorCube* m_pChaseLookCube = nullptr;
		string m_sSelectedMarkCubeTag;
		_uint m_iTagIndex = 0;
		_bool m_bShowCube = false;

		_uint m_iSaveIndex = 0;

		_float m_fCubeSize = 1.f;


		// For. Play
		list<class CMarkCube*> m_MarkCubes;
		list<class CMarkCube*> m_TempMarkCubes;
		list<class CMarkCube*> m_pTempBasiMarkCubes;

		list<class CLookCube_HR*> m_LookCubes;
		list<class CLookCube_HR*> m_TempLookCubes;
		list<class CLookCube_HR*> m_pTempBasiLookCubes;

		_bool m_bStart = false;
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

		// For. Render
		list<CAMDATA> m_PlayPosTemp;
		list<class CRenderCube*> m_RenderCubes;

};

END