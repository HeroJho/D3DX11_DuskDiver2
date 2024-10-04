#pragma once
#include "Client_Defines.h"
#include "Base.h"
BEGIN(Client)

class CModelTool final : public CBase
{
public:
	CModelTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CModelTool() = default;


#pragma region ModelObject
public:
	void Initialize();
	void ShowModelWindow(_float fTimeDelta);
	void AddModelObject(CMesh* pMesh) { m_Meshes.push_back(pMesh); Safe_AddRef(pMesh); }
	HRESULT CreateModelObject(_float4 fPos);
	vector<CMesh*> Get_Meshs() { return m_Meshes; }
	int Get_ShaderPass() { return m_iShaderPass; }
	_float Get_MaxUVIndexX() { return m_fUVMaxIndexX; }
	_float Get_MaxUVIndexY() { return m_fUVMaxIndexY; }
	_float Get_UVSpeed() { return m_fUVSpeed; }
	void Add_Mesh(CMesh* pMesh) { m_Meshes.push_back(pMesh); Safe_AddRef(pMesh); }

private:
	int Find_ModelObjectIndex(string sTag);
	bool FInd_ModelObjectMap(string sTag);
	void ModelFix(_float fTimeDelta);
	void DeleteObject();

	
	

private:
	_float m_fPosX = 0.f;
	_float m_fPosY = 0.f;
	_float m_fPosZ = 0.f;
	_float m_fSize = 1.f;
	_float m_fSizeX = 1.f;
	_float m_fSizeY = 1.f;
	_float m_fSizeZ = 1.f;
	_float m_fAngleX = 0.f;
	_float m_fAngleY = 0.f;
	_float m_fAngleZ = 0.f;
	_bool m_bScaleDetail = false;
	_float m_fAxisX = 1.f;
	_float m_fAxisY = 0.f;
	_float m_fAxisZ = 0.f;
	_float m_fTurnSpeed = 0.f;
	_bool m_bTurnDir = false;
	_bool m_bTurn = false;
	_bool m_bViewMatrix = false;
	_bool m_bStopTurn = false;
	_bool m_bFixShaderPass = false;
	_float m_fUVSpeed = 0.f;
	_int m_iShaderPass = 0;
	_float m_fUVMaxIndexX = 0;
	_float m_fUVMaxIndexY = 0;

	int m_iSelectedModel = 0;
	vector<CMesh*> m_Meshes;
	vector<const char*> m_ModelName;
	map<string, int> m_ModelObjectIndex;

	string m_NowLevels;

#pragma endregion ModelObject

#pragma region ModelData
public:
	typedef struct tagModelData
	{
		_tchar* ProtoName;
		char* ModelName;
	}MODELDATA;

public:
	void ShowModelEditWindow(_float fTimeDelta);

private:
	void Load_DataList();

private:
	int m_iSelectedModelData = 0;
	vector<const char*> m_ModelDataName;
	vector<MODELDATA> m_ModelData;

	string m_NowLevel;
	
#pragma endregion ModelData



private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

public:
	virtual void Free() override;
};

END

