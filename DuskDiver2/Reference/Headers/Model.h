#pragma once

#include "Component.h"

BEGIN(Engine)
class CAnimation;

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum TYPE { TYPE_NONANIM, TYPE_ANIM, TYPE_BONE, TYPE_END };
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	class CHierarchyNode* Get_HierarchyNode(const char* pNodeName);

	_uint Get_NumMeshes() const {
		return m_iNumMeshes;
	}

	_uint Get_MaterialIndex(_uint iMeshIndex);

	void Set_AnimIndex(_uint iAnimIndex);

	_matrix Get_PivotMatrix() {
		return XMLoadFloat4x4(&m_PivotMatrix);
	}


public:
	virtual HRESULT Initialize_Prototype(TYPE eType, const char* pModelFilePath, const char* pModelFileName, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);




public:
	HRESULT SetUp_OnShader(class CShader* pShader, _uint iMaterialIndex, aiTextureType eTextureType, const char* pConstantName);
	HRESULT Play_Animation(_float fTimeDelta, class CTransform* pTran = nullptr, class CNavigation* pNavi = nullptr);
	HRESULT Play_Animation_NoMove(_float fTimeDelta);
	HRESULT Play_AniReverse(_float fTimeDelta);
	HRESULT Play_AniReverse_NoMove(_float fTimeDelta);
	HRESULT Render(class CShader* pShader, _uint iMeshIndex, _uint iPassIndex = 0);
	HRESULT Render_CanStop(class CShader* pShader, _uint iMeshIndex, _uint iPassIndex = 0, _bool bStop = false);

	const char* Get_Name(int AnimIndex);
	void Set_Name(const char* AnimName, int AnimIndex);
	void ChangeAnimIndex(int Index1, int Index2);

	_float Get_TickPerSecond();
	void Set_TickPerSecond(_float fSecond);

	_float Get_Duration();
	void Set_Duration(_float fDuration);

	_float Get_PlayTime();
	void SetPlayTime(_float fPlayTime);

	_uint Get_KeyFrame();
	void Set_KeyFrame(_float fKeyFrame);

	vector<_float>& GetTimeLimit();
	_float Get_TimeLimit(int iIndex);
	void Add_TimeLimit();
	void Delete_TimeLimit();
	void SetTimeLimit(_float TimeLimit, int iIndex);
	void SetTimeLimit(vector<_float>&TimeLimit);

	bool GetAniEnd() { return m_bAniEnd; }

	void ResetKeyFrame();
	bool GetChangeBool() { return m_bChangeAni; }
	void SetChangeBool(bool bChange) { m_bChangeAni = bChange; }

	_uint GetNextIndex() { return m_iNextAnimIndex; }
	void SetNextIndex(_uint iNextIndex, _bool bAnimLook = false);

	CAnimation* GetNextAnimation();

	_uint Get_NumAnimations() { return m_iNumAnimations; }

	void DeleteAnimation(int Index);
	vector<class CHierarchyNode*>* Get_HierarchyPointer() { return m_pHierarchyPointer; }

	vector<class CMeshContainer*>* Get_Meshs() { return &m_Meshes; }

private:
	const aiScene* m_pAIScene = nullptr;
	Assimp::Importer			m_Importer;

	_float4x4					m_PivotMatrix;
	TYPE						m_eModelType = TYPE_END;

private:
	_uint									m_iNumMeshes = 0;
	vector<class CMeshContainer*>			m_Meshes;
	typedef vector<class CMeshContainer*>	MESHES;

private:
	_uint									m_iNumMaterials = 0;
	vector<MATERIALDESC>					m_Materials;

private:
	vector<class CHierarchyNode*>			m_HierarchyNodes;
	vector<class CHierarchyNode*>* m_pHierarchyPointer;
private:
	_uint								m_iCurrentAnimIndex = 0;
	_uint								m_iNumAnimations = 0;
	vector<class CAnimation*>			m_Animations;
	_uint								m_iNextAnimIndex = 0;
	bool m_bAniEnd = false;
	bool m_bChangeAni = false;

private:
	HRESULT Ready_MeshContainers(_fmatrix PivotMatrix);
	HRESULT Ready_Materials(const char* pModelFilePath);
	HRESULT Ready_HierarchyNodes(aiNode * pNode, class CHierarchyNode* pParent, _uint iDepth);
	HRESULT Ready_Animations();




public:
	static CModel* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType, const char* pModelFilePath, const char* pModelFileName, _fmatrix PivotMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;


	// ============================
	//		For. Anim_World
	// ============================
	public:
		_vector Get_RootPos();
		void Set_RootHierarchy(_uint iIndex);

		void Reset_AnimPos() {
			ZeroMemory(&m_vPreAnimPos, sizeof(_float3));
			m_bFristPivotPos = false;
		}


	private:
		class CHierarchyNode* m_pRoot_HierarchyNode = nullptr;
		const char* m_pRoot_HierarchyName = nullptr;
		_float3									m_vPreAnimPos;

		_bool									m_bFristPivotPos = false;



		// ============================
		//		For. Bone_Model
		// ============================
		public:
			const vector<class CHierarchyNode*>* Get_HierarchyNodes() { return &m_HierarchyNodes; }
			HRESULT Play_Bone();




			// ============================
			//		For. NoneModelVtx
			// ============================
		public:
			void Make_NonModelVtx(_uint iIndex);



			// ============================
			//		For. Debug
			// ============================
			public:


			private:



				// ============================
				//		For. BinModelData
				// ============================
				public:
					static CModel* Bin_Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, DATA_HEROSCENE * pScene, TYPE eType, const char* pModelFilePath, const char* pModelFileName, _fmatrix PivotMatrix = XMMatrixIdentity());
					virtual HRESULT Bin_Initialize(void* pArg);
					virtual HRESULT Bin_Initialize_Prototype(DATA_HEROSCENE * pScene, TYPE eType, const char* pModelFilePath, const char* pModelFileName, _fmatrix PivotMatrix);
				private:
					HRESULT Bin_Ready_MeshContainers(_fmatrix PivotMatrix);
					HRESULT Bin_Ready_Materials(const char* pModelFilePath);
					HRESULT Bin_Ready_HierarchyNodes();
					HRESULT Bin_Ready_Animations();

					HRESULT Safe_Release_Scene();


				private:
					DATA_HEROSCENE* m_pBin_AIScene = nullptr;

					vector<DATA_HEROMATERIAL>				m_DataMaterials;

					_bool						m_bIsProto = false;
					_bool						m_bIsBin = false;



					// Bin화를 위해 추출용 함수입니다. (Tool 에서 사용)
				public:
					HRESULT Get_HierarchyNodeData(DATA_HEROSCENE * pNodeData);
					HRESULT Get_MaterialData(DATA_HEROSCENE * pNodeData);
					HRESULT Get_MeshData(DATA_HEROSCENE * pNodeData);
					HRESULT Get_AnimData(DATA_HEROSCENE * pAnimData);
};

END