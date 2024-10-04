#include "..\Public\MeshContainer.h"
#include "Model.h"
#include "HierarchyNode.h"

CMeshContainer::CMeshContainer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CVIBuffer(pDevice, pContext)
	, m_bIsProto(true)

{
}

CMeshContainer::CMeshContainer(const CMeshContainer & rhs)
	: CVIBuffer(rhs)	
	, m_iMaterialIndex(rhs.m_iMaterialIndex)
	, m_pAnimVertices(rhs.m_pAnimVertices)
	, m_pNonAnimVertices(rhs.m_pNonAnimVertices)
	, m_pIndices(rhs.m_pIndices)
	, m_bIsProto(false)
{
	strcpy_s(m_szName, rhs.m_szName);
}

HRESULT CMeshContainer::Initialize_Prototype(CModel::TYPE eModelType, 
	const aiMesh * pAIMesh, CModel* pModel, _fmatrix PivotMatrix)
{
	strcpy_s(m_szName, pAIMesh->mName.data);
	m_iMaterialIndex = pAIMesh->mMaterialIndex;

	HRESULT		hr = 0;
	if (CModel::TYPE_NONANIM == eModelType) hr = Ready_Vertices(pAIMesh, PivotMatrix);
	else hr = Ready_AnimVertices(pAIMesh, pModel);
	if (FAILED(hr)) return E_FAIL;
	
#pragma region INDEXBUFFER

#pragma region 중략
	m_iNumPrimitives = pAIMesh->mNumFaces;
	m_iIndexSizeofPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesofPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;
#pragma endregion

	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitives];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitives);

	for (_uint i = 0; i < m_iNumPrimitives; ++i)
	{
		// 인덱스 정보 저장
		pIndices[i]._0 = pAIMesh->mFaces[i].mIndices[0];
		pIndices[i]._1 = pAIMesh->mFaces[i].mIndices[1];
		pIndices[i]._2 = pAIMesh->mFaces[i].mIndices[2];
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer())) return E_FAIL;

	m_pIndices = pIndices;

#pragma endregion
	return S_OK;
}

HRESULT CMeshContainer::Initialize(void * pArg)
{
	//Create_DynVertexBuffer();

	return S_OK;
}

HRESULT CMeshContainer::SetUp_HierarchyNodes(CModel * pModel, aiMesh* pAIMesh)
{
	m_iNumBones = pAIMesh->mNumBones;



	/* 현재 메시에 영향ㅇ르 ㅈ2ㅜ는 뼈들을 순회한다ㅏ. */
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		aiBone*		pAIBone = pAIMesh->mBones[i];

		CHierarchyNode*		pHierarchyNode = pModel->Get_HierarchyNode(pAIBone->mName.data);

		_float4x4			OffsetMatrix;

		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));

		pHierarchyNode->Set_OffsetMatrix(XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_Bones.push_back(pHierarchyNode);

		Safe_AddRef(pHierarchyNode);
	}

	if (0 == m_iNumBones)
	{

		CHierarchyNode*		pNode = pModel->Get_HierarchyNode(m_szName);

		if (nullptr == pNode)
			return S_OK;

		m_iNumBones = 1;

		m_Bones.push_back(pNode);

	}

	return S_OK;
}

/* 메시의 정점을 그리기위해 셰이더에 넘기기위한 뼈행렬의 배열을 구성한다. */
void CMeshContainer::SetUp_BoneMatrices(_float4x4 * pBoneMatrices, _fmatrix PivotMatrix)
{
	if (0 == m_iNumBones)
	{
		XMStoreFloat4x4(&pBoneMatrices[0], XMMatrixIdentity());
		return;
	}

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		_matrix mMatrix = m_Bones[i]->Get_OffSetMatrix() * m_Bones[i]->Get_CombinedTransformation() * PivotMatrix;
		XMStoreFloat4x4(&pBoneMatrices[i], XMMatrixTranspose(mMatrix));		
	}
}

void CMeshContainer::SetUp_BoneMatrices_NoT(_float4x4 * pBoneMatrices, _fmatrix PivotMatrix)
{
	if (0 == m_iNumBones)
	{
		XMStoreFloat4x4(&pBoneMatrices[0], XMMatrixIdentity());
		return;
	}

	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		_matrix mMatrix = m_Bones[i]->Get_OffSetMatrix() * m_Bones[i]->Get_CombinedTransformation() * PivotMatrix;
		XMStoreFloat4x4(&pBoneMatrices[i], mMatrix);
	}
}

HRESULT CMeshContainer::Ready_Vertices(const aiMesh* pAIMesh, _fmatrix PivotMatrix)
{

#pragma region 중략
	m_iNumVertexBuffers = 1;
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iStride = sizeof(VTXMODEL);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXMODEL*		pVertices = new VTXMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMODEL) * m_iNumVertices);
#pragma endregion

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		// Assimp의 정점 위치 데이터를 가져오면서 PivotMatrix 적용.
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, 
			XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));
		// 노말 데이터 저장
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, 
			XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PivotMatrix));
		// UV 데이터 저장
		memcpy(&pVertices[i].vTexture, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		// 탄젠트 노말 저장 (노말 매핑을 위해서) 
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vTangent, 
			XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), PivotMatrix));
	}

#pragma region 중략

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	m_pNonAnimVertices = pVertices;

#pragma endregion

	return S_OK;
}

HRESULT CMeshContainer::Ready_AnimVertices(const aiMesh* pAIMesh, CModel* pModel)
{
	m_iNumVertexBuffers = 1;
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iStride = sizeof(VTXANIMMODEL);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXANIMMODEL*		pVertices = new VTXANIMMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		/* 사전변환( x) : 뼈의 행렬과 곱해져서 그려진다. 
		사전변환에 대한 정보를 뼈에게 담아놓을 것이다. */
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexture, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));		
	}

	/* 현재 메시에 영향ㅇ르 ㅈ2ㅜ는 뼈들을 순회한다ㅏ. */
	/* 뼈(aiBone)안에 표현되어있는, 이뼈는 어떤 정점에게 영향을 주는지(mVertexId)를 받아와서. 
	해당 정점에게 이뼈에 영향을 받는다(vBlendIndex), 얼마나(vBlendWeight)를 담아둔다. */

	for (_uint i = 0; i <  pAIMesh->mNumBones; ++i)
	{
		aiBone*		pAIBone = pAIMesh->mBones[i];	
		

		/* i번째 뼈가 어떤 정점들에게 영향ㅇ르 주는지 순회한다. */
		for (_uint j = 0; j < pAIBone->mNumWeights; ++j)
		{
			_uint		iVertexIndex = pAIBone->mWeights[j].mVertexId;



			if(0.0f == pVertices[iVertexIndex].vBlendWeight.x)
			{
				pVertices[iVertexIndex].vBlendIndex.x = i;
				pVertices[iVertexIndex].vBlendWeight.x = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[iVertexIndex].vBlendWeight.y)
			{
				pVertices[iVertexIndex].vBlendIndex.y = i;
				pVertices[iVertexIndex].vBlendWeight.y = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[iVertexIndex].vBlendWeight.z)
			{
				pVertices[iVertexIndex].vBlendIndex.z = i;
				pVertices[iVertexIndex].vBlendWeight.z = pAIBone->mWeights[j].mWeight;
			}

			else if (0.0f == pVertices[iVertexIndex].vBlendWeight.w)
			{
				pVertices[iVertexIndex].vBlendIndex.w = i;
				pVertices[iVertexIndex].vBlendWeight.w = pAIBone->mWeights[j].mWeight;
			}
		}
	}
	
	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	m_pAnimVertices = pVertices;

	return S_OK;
}

CMeshContainer * CMeshContainer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CModel::TYPE eModelType, const aiMesh * pAIMesh, CModel* pModel, _fmatrix PivotMatrix)
{
	CMeshContainer*			pInstance = new CMeshContainer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eModelType, pAIMesh, pModel, PivotMatrix)))
	{
		MSG_BOX(TEXT("Failed To Created : CMeshContainer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CMeshContainer::Clone(void * pArg)
{
	CMeshContainer*			pInstance = new CMeshContainer(*this);

	if (!pArg)
	{
		if (FAILED(pInstance->Bin_Initialize(pArg)))
		{
			MSG_BOX(TEXT("Failed To Cloned : CMeshContainer"));
			Safe_Release(pInstance);
		}
	}
	else
	{
		if (FAILED(pInstance->Initialize(pArg)))
		{
			MSG_BOX(TEXT("Failed To Cloned : Bin_CMeshContainer"));
			Safe_Release(pInstance);
		}
	}


	return pInstance;
}





void CMeshContainer::Update_DynVertex(CModel* pModel)
{
	if (nullptr == m_pDevice)
		return;


	ZeroMemory(&m_DynBufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_DynBufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_DynBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_DynBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_DynBufferDesc.CPUAccessFlags = 0;
	m_DynBufferDesc.MiscFlags = 0;
	m_DynBufferDesc.StructureByteStride = m_iStride;





	// 정점을 계산한다.
	VTXANIMMODEL*		pVertices = new VTXANIMMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);

	_float4x4		BoneMatrices[256];
	SetUp_BoneMatrices_NoT(BoneMatrices, pModel->Get_PivotMatrix());

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		_float		fWeightW = 1.f - (m_pAnimVertices[i].vBlendWeight.x + m_pAnimVertices[i].vBlendWeight.y + m_pAnimVertices[i].vBlendWeight.z);

		_matrix	BoneMatrix =
			XMLoadFloat4x4(&BoneMatrices[m_pAnimVertices[i].vBlendIndex.x]) * m_pAnimVertices[i].vBlendWeight.x +
			XMLoadFloat4x4(&BoneMatrices[m_pAnimVertices[i].vBlendIndex.y]) * m_pAnimVertices[i].vBlendWeight.y +
			XMLoadFloat4x4(&BoneMatrices[m_pAnimVertices[i].vBlendIndex.z]) * m_pAnimVertices[i].vBlendWeight.z +
			XMLoadFloat4x4(&BoneMatrices[m_pAnimVertices[i].vBlendIndex.w]) * fWeightW;

		
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&m_pAnimVertices[i].vPosition), BoneMatrix));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&m_pAnimVertices[i].vNormal), BoneMatrix));

		pVertices[i].vTexture = m_pAnimVertices[i].vTexture;
		pVertices[i].vBlendIndex = m_pAnimVertices[i].vBlendIndex;
		pVertices[i].vBlendWeight = m_pAnimVertices[i].vBlendWeight;
		pVertices[i].vTangent = m_pAnimVertices[i].vTangent;
	}




	ZeroMemory(&m_DynSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_DynSubResourceData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_DynBufferDesc, &m_DynSubResourceData, &m_pDynVB)))
		return;

	

	//// 버퍼를 열어서 결과를 저장한다.
	//D3D11_MAPPED_SUBRESOURCE		MappedSubResource;
	//ZeroMemory(&MappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	//m_pContext->Map(m_pDynVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &MappedSubResource);

	//memcpy(MappedSubResource.pData, pVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);

	//m_pContext->Unmap(m_pDynVB, 0);

	m_pAnimStopVertices = pVertices;
}

// For. BinData
HRESULT CMeshContainer::Bin_Initialize_Prototype(CModel::TYPE eModelType, DATA_HEROMETH* pAIMesh, CModel* pModel, _fmatrix PivotMatrix)
{
	strcpy_s(m_szName, pAIMesh->cName);
	m_iMaterialIndex = pAIMesh->iMaterialIndex;

#pragma region VERTEXBUFFER

	HRESULT		hr = 0;

	if (CModel::TYPE_NONANIM == eModelType)
		hr = Bin_Ready_Vertices(pAIMesh, PivotMatrix);
	else
		hr = Bin_Ready_AnimVertices(pAIMesh, pModel);

	if (FAILED(hr))
		return E_FAIL;

#pragma endregion

#pragma region INDEXBUFFER
	m_iNumPrimitives = pAIMesh->iNumPrimitives;
	m_iIndexSizeofPrimitive = sizeof(FACEINDICES32);
	m_iNumIndicesofPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_eTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumPrimitives * m_iIndexSizeofPrimitive;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = 0;


	FACEINDICES32*		pIndices = new FACEINDICES32[m_iNumPrimitives];
	ZeroMemory(pIndices, sizeof(FACEINDICES32) * m_iNumPrimitives);

	for (_uint i = 0; i < m_iNumPrimitives; ++i)
	{
		pIndices[i]._0 = pAIMesh->pIndices[i]._0;
		pIndices[i]._1 = pAIMesh->pIndices[i]._1;
		pIndices[i]._2 = pAIMesh->pIndices[i]._2;
	}


	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pIndices;

	if (FAILED(__super::Create_IndexBuffer()))
		return E_FAIL;

	m_pIndices = pIndices;

#pragma endregion

	return S_OK;
}

HRESULT CMeshContainer::Bin_Initialize(void * pArg)
{

	return S_OK;
}

HRESULT CMeshContainer::Bin_SetUp_HierarchyNodes(CModel * pModel, DATA_HEROMETH* pAIMesh)
{
	m_iNumBones = pAIMesh->iNumBones;

	/* 현재 메시에 영향ㅇ르 ㅈ2ㅜ는 뼈들을 순회한다ㅏ. */
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		DATA_HEROBONE*		pAIBone = &pAIMesh->pBones[i];

		CHierarchyNode*		pHierarchyNode = pModel->Get_HierarchyNode(pAIBone->cNames);

		_float4x4			OffsetMatrix;

		memcpy(&OffsetMatrix, &pAIBone->mOffsetTransform, sizeof(_float4x4));

		pHierarchyNode->Set_OffsetMatrix(XMLoadFloat4x4(&OffsetMatrix));

		m_Bones.push_back(pHierarchyNode);

		Safe_AddRef(pHierarchyNode);
	}

	if (0 == m_iNumBones)
	{

		CHierarchyNode*		pNode = pModel->Get_HierarchyNode(m_szName);

		if (nullptr == pNode)
			return S_OK;

		m_iNumBones = 1;

		m_Bones.push_back(pNode);

	}

	return S_OK;
}

HRESULT CMeshContainer::Bin_Ready_Vertices(DATA_HEROMETH* pAIMesh, _fmatrix PivotMatrix)
{
	m_iNumVertexBuffers = 1;
	m_iNumVertices = pAIMesh->NumVertices;
	m_iStride = sizeof(VTXMODEL);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXMODEL*		pVertices = new VTXMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		pVertices[i] = pAIMesh->pNonAnimVertices[i];
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PivotMatrix));
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	m_pNonAnimVertices = pVertices;

	return S_OK;
}

HRESULT CMeshContainer::Bin_Ready_AnimVertices(DATA_HEROMETH* pAIMesh, CModel* pModel)
{
	m_iNumVertexBuffers = 1;
	m_iNumVertices = pAIMesh->NumVertices;
	m_iStride = sizeof(VTXANIMMODEL);

	ZeroMemory(&m_BufferDesc, sizeof(D3D11_BUFFER_DESC));
	m_BufferDesc.ByteWidth = m_iNumVertices * m_iStride;
	m_BufferDesc.Usage = D3D11_USAGE_DEFAULT;
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.CPUAccessFlags = 0;
	m_BufferDesc.MiscFlags = 0;
	m_BufferDesc.StructureByteStride = m_iStride;

	VTXANIMMODEL*		pVertices = new VTXANIMMODEL[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMODEL) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i], &pAIMesh->pAnimVertices[i], sizeof(VTXANIMMODEL));
	}

	ZeroMemory(&m_SubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_SubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	m_pAnimVertices = pVertices;

	return S_OK;
}

CMeshContainer * CMeshContainer::Bin_Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, CModel::TYPE eModelType, DATA_HEROMETH* pAIMesh, CModel* pModel, _fmatrix PivotMatrix)
{
	CMeshContainer*			pInstance = new CMeshContainer(pDevice, pContext);

	if (FAILED(pInstance->Bin_Initialize_Prototype(eModelType, pAIMesh, pModel, PivotMatrix)))
	{
		MSG_BOX(TEXT("Failed To Created : CBinMeshContainer"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



// For. 추출
void CMeshContainer::Get_MeshData(DATA_HEROMETH * pMesh)
{
	memcpy(&pMesh->cName, &m_szName, sizeof(char) * MAX_PATH);
	pMesh->iMaterialIndex = m_iMaterialIndex;
	pMesh->NumVertices = m_iNumVertices;
	pMesh->pNonAnimVertices = m_pNonAnimVertices;
	pMesh->pAnimVertices = m_pAnimVertices;
	pMesh->iNumPrimitives = m_iNumPrimitives;
	pMesh->pIndices = m_pIndices;

	pMesh->iNumBones = m_iNumBones;
	pMesh->pBones = new DATA_HEROBONE[m_iNumBones];
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		memcpy(&pMesh->pBones[i].cNames, m_Bones[i]->Get_Name(), sizeof(char) * MAX_PATH);
		pMesh->pBones[i].mOffsetTransform = m_Bones[i]->Get_OffSetMatrixXM();
		int a = 0;
	}


}


void CMeshContainer::Free()
{
	__super::Free();

	for (auto& pHierarchyNode : m_Bones)
		Safe_Release(pHierarchyNode);

	if (m_bIsProto)
	{
		Safe_Delete_Array(m_pIndices);
		Safe_Delete_Array(m_pAnimVertices);
		Safe_Delete_Array(m_pNonAnimVertices);
	}

	Safe_Delete_Array(m_pAnimStopVertices);
	Safe_Release(m_pDynVB);
	m_Bones.clear();
}

HRESULT CMeshContainer::Render_Stop()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer*		pVertexBuffers[] = {
		m_pDynVB,
	};

	_uint			iStrides[] = {
		m_iStride
	};

	_uint			iOffsets[] = {
		0,
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iStrides, iOffsets);

	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);

	m_pContext->IASetPrimitiveTopology(m_eTopology);

	m_pContext->DrawIndexed(m_iNumPrimitives * m_iNumIndicesofPrimitive, 0, 0);

	return S_OK;
}
