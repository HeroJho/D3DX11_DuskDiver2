#pragma once


#include "VIBuffer.h"
#include "Model.h"


/* ���� �����ϴ� �ϳ��� �޽�. */
/* �� �޽ø� �����ϴ� ����, �ε��� ���۸� �����Ѵ�. */

BEGIN(Engine)

class CMeshContainer final : public CVIBuffer
{
private:
	CMeshContainer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeshContainer(const CMeshContainer& rhs);
	virtual ~CMeshContainer() = default;

public:
	_uint Get_MaterialIndex() const {
		return m_iMaterialIndex;
	}

public:
	virtual HRESULT Initialize_Prototype(CModel::TYPE eModelType, const aiMesh* pAIMesh, class CModel* pModel, _fmatrix PivotMatrix);
	virtual HRESULT Initialize(void* pArg);	

public:
	HRESULT SetUp_HierarchyNodes(class CModel* pModel, aiMesh* pAIMesh);
	void SetUp_BoneMatrices(_float4x4* pBoneMatrices, _fmatrix PivotMatrix);
	void SetUp_BoneMatrices_NoT(_float4x4* pBoneMatrices, _fmatrix PivotMatrix);




private:
	char				m_szName[MAX_PATH] = "";
	_uint				m_iMaterialIndex = 0;
private:
	/* �� �޽ÿ� ���⤷�� �ִ� ���� ����. */
	_uint							m_iNumBones = 0;

	/* �� �޽ÿ� ������ �ִ� ������ ��Ƴ�����. */
	/* why ? : �޽� ���� �������� �� �� �޽ÿ� ���⤷�� �ִ� ������ ����� ��Ƽ� ��{���𷯤� ���ä�������. */
	vector<class CHierarchyNode*>	m_Bones;

private:
	HRESULT Ready_Vertices(const aiMesh* pAIMesh, _fmatrix PivotMatrix);
	HRESULT Ready_AnimVertices(const aiMesh* pAIMesh, CModel* pModel);

public:
	static CMeshContainer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, const aiMesh* pAIMesh, class CModel* pModel, _fmatrix PivotMatrix);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
	virtual HRESULT Render_Stop();


	// ============================
	//		For. StopAnim
	// ============================
public:
	void	Update_DynVertex(CModel* pModel);

	VTXANIMMODEL* Get_StopVtx() { return m_pAnimStopVertices; }
	FACEINDICES32* Get_Indices() { return m_pIndices; }

private:
	ID3D11Buffer*					m_pDynVB = nullptr;

	VTXANIMMODEL*		m_pAnimStopVertices = nullptr;
	FACEINDICES32*		m_pStopIndices = nullptr;

	D3D11_BUFFER_DESC				m_DynBufferDesc;
	D3D11_SUBRESOURCE_DATA			m_DynSubResourceData;



	// ============================
	//		For. BinModelData
	// ============================
public:
	virtual HRESULT Bin_Initialize_Prototype(CModel::TYPE eModelType, DATA_HEROMETH* pAIMesh, class CModel* pModel, _fmatrix PivotMatrix);
	virtual HRESULT Bin_Initialize(void* pArg);
	HRESULT Bin_SetUp_HierarchyNodes(class CModel* pModel, DATA_HEROMETH* pAIMesh);

	HRESULT Bin_Ready_Vertices(DATA_HEROMETH* pAIMesh, _fmatrix PivotMatrix);
	HRESULT Bin_Ready_AnimVertices(DATA_HEROMETH* pAIMesh, CModel* pModel);

	static CMeshContainer* Bin_Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CModel::TYPE eModelType, DATA_HEROMETH* pAIMesh, class CModel* pModel, _fmatrix PivotMatrix);

	// For. Data
public:
	void Get_MeshData(DATA_HEROMETH* pMesh);

private:
	VTXMODEL*			m_pNonAnimVertices = nullptr;
	VTXANIMMODEL*		m_pAnimVertices = nullptr;
	FACEINDICES32*		m_pIndices = nullptr;
	_bool				m_bIsProto = false;


};

END