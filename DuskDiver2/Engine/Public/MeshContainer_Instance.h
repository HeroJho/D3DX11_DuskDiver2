#pragma once


#include "VIBuffer.h"
#include "Model_Instance.h"


/* ���� �����ϴ� �ϳ��� �޽�. */
/* �� �޽ø� �����ϴ� ����, �ε��� ���۸� �����Ѵ�. */

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
	/* �� �޽ÿ� ���⤷�� �ִ� ���� ����. */
	_uint							m_iNumBones = 0;

	/* �� �޽ÿ� ������ �ִ� ������ ��Ƴ�����. */
	/* why ? : �޽� ���� �������� �� �� �޽ÿ� ���⤷�� �ִ� ������ ����� ��Ƽ� ��{���𷯤� ���ä�������. */
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