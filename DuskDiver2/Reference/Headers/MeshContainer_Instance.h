#pragma once


#include "VIBuffer.h"
#include "Model_Instance.h"


/* 모델을 구성하는 하나의 메시. */
/* 이 메시를 구성하는 정점, 인덱스 버퍼를 보관한다. */

BEGIN(Engine)

class CMeshContainer_Instance final : public CVIBuffer
{
private:
	CMeshContainer_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeshContainer_Instance(const CMeshContainer_Instance& rhs);
	virtual ~CMeshContainer_Instance() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
	virtual HRESULT Initialize_Prototype(const aiMesh* pAIMesh, class CModel_Instance* pModel, _fmatrix PivotMatrix, _uint iNumInstance, VTXMODELINSTANCE* pInstanceInfo);
	virtual HRESULT Bin_Initialize_Prototype(DATA_HEROMETH* pAIMesh, class CModel_Instance* pModel, _fmatrix PivotMatrix, _uint iNumInstance, VTXMODELINSTANCE* pInstanceInfo);

	virtual HRESULT Initialize(void* pArg);
	virtual HRESULT Bin_Initialize(void* pArg);

	HRESULT Bin_SetUp_HierarchyNodes(class CModel_Instance* pModel, DATA_HEROMETH* pAIMesh);

	virtual HRESULT Render() override;

private:
	char				m_szName[MAX_PATH] = "";
	_uint				m_iMaterialIndex = 0;

	VTXANIMMODEL*		m_pAnimVertices = nullptr;


private:
	HRESULT Ready_Vertices(const aiMesh* pAIMesh, _fmatrix PivotMatrix);


private:
	HRESULT Bin_Ready_Vertices(DATA_HEROMETH* pAIMesh, _fmatrix PivotMatrix);
	HRESULT Bin_Ready_AnimVertices(DATA_HEROMETH* pAIMesh, CModel_Instance* pModel);


public:
	static CMeshContainer_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMesh* pAIMesh, class CModel_Instance* pModel, _fmatrix PivotMatrix, _uint iNumInstance, VTXMODELINSTANCE* pInstanceInfo);
	static CMeshContainer_Instance* Bin_Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, DATA_HEROMETH* pAIMesh, class CModel_Instance* pModel, _fmatrix PivotMatrix, _uint iNumInstance, VTXMODELINSTANCE* pInstanceInfo);

	virtual CComponent* Clone(void* pArg = nullptr) override;

	virtual void Free() override;

	void SetUp_BoneMatrices(_float4x4* pBoneMatrices, _fmatrix PivotMatrix);



private:
	/* 이 메시에 영향ㅇ르 주는 뼈의 갯수. */
	_uint							m_iNumBones = 0;

	/* 이 메시에 영향을 주는 뼈들을 모아놓느다. */
	/* why ? : 메시 별로 렌더링할 때 이 메시에 영향ㅇ르 주는 뼏르의 행렬을 모아서 셰{ㅇ디러ㅗ 도ㅓㄴ질려고. */
	vector<class CHierarchyNode*>	m_Bones;



	// For. Data
public:
	void Get_MeshData(DATA_HEROMETH* pMesh);

private:
	VTXMODEL*			m_pNonAnimVertices = nullptr;
	FACEINDICES32*		m_pIndices = nullptr;
	_bool				m_bIsProto = false;

	// For. Instance
	_uint					m_iNumInstance = 0;
	_uint					m_iNumRealPrimitives = 0;
	_uint					m_iInstanceStride = 0;
	ID3D11Buffer*			m_pVBInstance = nullptr;

};

END