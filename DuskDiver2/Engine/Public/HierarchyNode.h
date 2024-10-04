#pragma once

#include "Base.h"

/* ��������(�������� ��Ӱ���)�� ǥ���ϱ����� aiNode�̴�. */

BEGIN(Engine)

class ENGINE_DLL CHierarchyNode final : public CBase
{
private:
	CHierarchyNode();
	virtual ~CHierarchyNode() = default;

public:
	char* Get_Name() {
		return m_szName;
	}
	const char* Get_ParentName() const {
		if (nullptr == m_pParent)
			return "";
		return m_pParent->Get_Name();
	}

	_uint Get_Depth() const {
		return m_iDepth;
	}

	_matrix Get_OffSetMatrix() {
		return XMLoadFloat4x4(&m_OffsetMatrix);
	}
	XMFLOAT4X4 Get_OffSetMatrixXM() {
		return m_OffsetMatrix;
	}

	_matrix Get_CombinedTransformation() {
		return XMLoadFloat4x4(&m_CombinedTransformation);
	}

	_float4x4 Get_OriTransformation() { return m_OriTransformation; }


public:
	void Set_Transformation(_fmatrix Transformation) {
		XMStoreFloat4x4(&m_Transformation, Transformation);
	}
	void Set_CombinedTransformation(_fmatrix Transformation) {
		XMStoreFloat4x4(&m_CombinedTransformation, Transformation);
	}

public:
	HRESULT Initialize(aiNode* pAINode, class CHierarchyNode* pParent, _uint iDepth);
	void Set_CombinedTransformation(const char* pRootNodeName = nullptr);
	void Set_OriTransformation();
	void Set_OffsetMatrix(_fmatrix OffsetMatrix);

private:
	char				m_szName[MAX_PATH] = "";
	_float4x4			m_OffsetMatrix;
	_float4x4			m_Transformation;
	_float4x4			m_CombinedTransformation;
	CHierarchyNode*		m_pParent = nullptr;
	_uint				m_iDepth = 0;


	// ============================
	//		For. Anim_World
	// ============================
public:
	_matrix Get_RootAnim_CombinedTransformation() {
		return XMLoadFloat4x4(&m_RootAnim_CombinedTransformation);
	}

private:
	_float4x4			m_RootAnim_CombinedTransformation;


public:
	static CHierarchyNode* Create(aiNode* pAINode, class CHierarchyNode* pParent, _uint iDepth);
	virtual void Free();




	// ============================
	//		For. BinModelData
	// ============================
public:
	CHierarchyNode* Get_Parent() { return m_pParent; }
	void Set_FindParent(CHierarchyNode* pNode);

public:
	static CHierarchyNode* Bin_Create(DATA_HERONODE* pNode);
	HRESULT Bin_Initialize(DATA_HERONODE* pNode);

private:
	char				m_szParentName[MAX_PATH] = "";
	_float4x4			m_OriTransformation;


};

END