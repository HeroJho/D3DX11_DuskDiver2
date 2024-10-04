#include "..\Public\HierarchyNode.h"

CHierarchyNode::CHierarchyNode()
{

}

HRESULT CHierarchyNode::Initialize(aiNode * pAINode, CHierarchyNode* pParent, _uint iDepth)
{
	/* 뼈 이름 보관. */
	strcpy_s(m_szName, pAINode->mName.data);

	/* 씬객체로 부터 행렬정보를 받아올때는 반드시 전치해서 받아와라. */
	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());

	memcpy(&m_Transformation, &pAINode->mTransformation, sizeof(_float4x4));
	XMStoreFloat4x4(&m_Transformation, XMMatrixTranspose(XMLoadFloat4x4(&m_Transformation)));
	m_OriTransformation = m_Transformation;

	m_iDepth = iDepth;
	m_pParent = pParent;

	XMStoreFloat4x4(&m_CombinedTransformation, XMMatrixIdentity());

	Safe_AddRef(m_pParent);

	return S_OK;
}

void CHierarchyNode::Set_CombinedTransformation(const char* pRootNodeName)
{

	if (nullptr != m_pParent)
	{

		if (pRootNodeName && !strcmp(pRootNodeName, m_szName))
		{
			XMStoreFloat4x4(&m_RootAnim_CombinedTransformation, XMLoadFloat4x4(&m_Transformation) * XMLoadFloat4x4(&m_pParent->m_CombinedTransformation));

			*((_float4*)m_Transformation.m[3]) = _float4{ 0.f, 0.f, 0.f, 1.f };
			XMStoreFloat4x4(&m_CombinedTransformation, XMLoadFloat4x4(&m_Transformation) * XMLoadFloat4x4(&m_pParent->m_CombinedTransformation));
		}
		else
			XMStoreFloat4x4(&m_CombinedTransformation, XMLoadFloat4x4(&m_Transformation) * XMLoadFloat4x4(&m_pParent->m_CombinedTransformation));
	}
	else
	{
		m_CombinedTransformation = m_Transformation;
	}

}

void CHierarchyNode::Set_OriTransformation()
{
	if (nullptr != m_pParent)
	{
		XMStoreFloat4x4(&m_CombinedTransformation, XMLoadFloat4x4(&m_OriTransformation) * XMLoadFloat4x4(&m_pParent->m_CombinedTransformation));
	}
	else
	{
		m_CombinedTransformation = m_OriTransformation;
	}
}

void CHierarchyNode::Set_OffsetMatrix(_fmatrix OffsetMatrix)
{
	XMStoreFloat4x4(&m_OffsetMatrix, OffsetMatrix);
}

CHierarchyNode * CHierarchyNode::Create(aiNode * pAINode, CHierarchyNode* pParent, _uint iDepth)
{
	CHierarchyNode*			pInstance = new CHierarchyNode();

	if (FAILED(pInstance->Initialize(pAINode, pParent, iDepth)))
	{
		MSG_BOX(TEXT("Failed To Created : CHierarchyNode"));
		Safe_Release(pInstance);
	}

	return pInstance;
}









// For. BinData
void CHierarchyNode::Set_FindParent(CHierarchyNode* pNode)
{
	const char* pName = pNode->Get_Name();
	if (!strcmp(m_szParentName, pName))
	{
		m_pParent = pNode;
		Safe_AddRef(m_pParent);
		return;
	}
	m_pParent = nullptr;
}

CHierarchyNode * CHierarchyNode::Bin_Create(DATA_HERONODE* pNode)
{
	CHierarchyNode*			pInstance = new CHierarchyNode();

	if (FAILED(pInstance->Bin_Initialize(pNode)))
	{
		MSG_BOX(TEXT("Failed To Created : CBinHierarchyNode"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


HRESULT CHierarchyNode::Bin_Initialize(DATA_HERONODE* pNode)
{
	strcpy_s(m_szName, pNode->cName);
	strcpy_s(m_szParentName, pNode->cParent);

	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());

	memcpy(&m_Transformation, &pNode->mTransform, sizeof(_float4x4));
	m_OriTransformation = m_Transformation;
	// XMStoreFloat4x4(&m_Transformation, XMMatrixTranspose(XMLoadFloat4x4(&m_Transformation)));

	m_iDepth = pNode->iDepth;


	XMStoreFloat4x4(&m_CombinedTransformation, XMMatrixIdentity());

	return S_OK;
}





void CHierarchyNode::Free()
{
	Safe_Release(m_pParent);
}
