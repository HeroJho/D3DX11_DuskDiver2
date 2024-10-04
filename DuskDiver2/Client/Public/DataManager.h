#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "Model.h"
#include "PxObj.h"

BEGIN(Engine)

class CModel;
class CCell;

END

BEGIN(Client)

class CDataManager final : public CBase
{
	DECLARE_SINGLETON(CDataManager)
public:
	typedef struct tagCamData
	{
		_float3 vPos;
		_float fSpeed;
		_float fSens;
		_float fDestDis;
		_uint iLinkIndex;
	}CAMDATA;
	typedef struct dataCams
	{

		_uint iID;

		_uint iPosNum;
		CAMDATA* pPosDatas;

		_uint iLookNum;
		CAMDATA* pLookDatas;

	}DATA_CAMS;




public:
	enum DATA_TYPE { DATA_ANIM, DATA_NOEANIM, DATA_BONE, DATA_END };

public:
	CDataManager();
	virtual ~CDataManager() = default;

private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;


public:
	HRESULT Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);






	// ==========================
	//		For. Bin_Data
	// ==========================
public:
	/* Bin 데이터를 불러옵니다. */
	HRESULT Create_Try_BinModel(const _tchar* pModelName, LEVEL eLEVEL, DATA_TYPE eTYPE, _fmatrix mPivot);
	/* Bin 데이터 모두를 읽어옵니다. */
	HRESULT Create_Try_BinModel_All();

	/* Bin 데이터로 저장합니다. */
	HRESULT Conv_Bin_Model(CModel* pModel, char* cModelName, DATA_TYPE eTYPE);
	/* 지정한 폴더의 모델들을 전부 바이너리화 합니다.(이미 있다면 Pass) */
	HRESULT Conv_Bin_Model_All();


public:
	HRESULT ReadSceneData(char* pFileName, DATA_HEROSCENE* pScene, DATA_TYPE eTYPE);
	HRESULT SaveSceneData(DATA_HEROSCENE* pScene, char* cModelName, DATA_TYPE eTYPE);
	class CModel* Create_Model_Without_Bin(const _tchar* pModelName, LEVEL eLEVEL, DATA_TYPE eTYPE);






	// ==========================
	//		For. Pass
	// ==========================
public:
	const list<string>* Get_AnimFileNames() { return &m_AnimFilePaths; }
	const list<string>* Get_NonAnimFileNames() { return &m_NonAnimFilePaths; }
	const list<string>* Get_BoneFileNames() { return &m_BoneFilePaths; }
	const list<string>* Get_TextureFileNames() { return &m_TexturePaths; }
private:
	/* 해당 폴더 경로의 파일 이름을 읽어옵니다.*/
	HRESULT  LoadModelPass();
	HRESULT LoadTexturePass();

private:
	list<string>	m_AnimFilePaths;
	list<string>	m_NonAnimFilePaths;
	list<string>	m_BoneFilePaths;
	list<string>	m_TexturePaths;

	list<string>	m_FileNames;





	// ==========================
	//		For. PhysObj && AnimIns
	// ==========================
public:
	void Load_PhysObjData(_int iLevelIndex);
	void Reset_MapRigidStatic();

	void Load_ByInstancing(_int iLevelIndex);
	

private:
	void Create_PxObj_Load(CPxObj::PXOBJDESC PxDesc);


private:
	list<PxRigidStatic*> m_pMapRigidStatics;



	// ==========================
	//		For. Light
	// ==========================
public:
	void Load_LightData(_int iLevelIndex);




	// ==========================
	//		For. CutScene
	// ==========================
public:
	HRESULT Save_Cam(_int iCamID);
	CDataManager::DATA_CAMS* Load_Cam(_int iCamID);


public:
	virtual void Free() override;

};

END

