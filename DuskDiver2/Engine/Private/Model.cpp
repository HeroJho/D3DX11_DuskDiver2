#include "..\Public\Model.h"
#include "MeshContainer.h"
#include "Texture.h"
#include "HierarchyNode.h"
#include "Animation.h"
#include "Shader.h"

#include "Transform.h"
#include "Navigation.h"


CModel::CModel(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext), m_bIsProto(true)
{
}

CModel::CModel(const CModel & rhs)
	: CComponent(rhs)
	, m_pAIScene(rhs.m_pAIScene)
	, m_iNumMeshes(rhs.m_iNumMeshes)
	, m_iNumMaterials(rhs.m_iNumMaterials)
	, m_Meshes(rhs.m_Meshes)
	, m_Materials(rhs.m_Materials)
	, m_eModelType(rhs.m_eModelType)
	, m_Animations(rhs.m_Animations)
	, m_iCurrentAnimIndex(rhs.m_iCurrentAnimIndex)
	, m_PivotMatrix(rhs.m_PivotMatrix)
	, m_iNumAnimations(rhs.m_iNumAnimations)
	// For. BinData
	, m_bIsBin(rhs.m_bIsBin)
	, m_pBin_AIScene(rhs.m_pBin_AIScene)
	, m_DataMaterials(rhs.m_DataMaterials)
	
{
	for (auto& pMeshContainer : m_Meshes)
		Safe_AddRef(pMeshContainer);


	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_AddRef(Material.pTexture[i]);
	}

	for (auto& pAnimation : m_Animations)
		Safe_AddRef(pAnimation);

	Reset_AnimPos();

}



CHierarchyNode * CModel::Get_HierarchyNode(const char * pNodeName)
{
	auto	iter = find_if(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [&](CHierarchyNode* pNode) 
	{
		return !strcmp(pNodeName, pNode->Get_Name());
	});

	if (iter == m_HierarchyNodes.end())
		return nullptr;

	return *iter;	
}

_uint CModel::Get_MaterialIndex(_uint iMeshIndex)
{
	return m_Meshes[iMeshIndex]->Get_MaterialIndex();
}

void CModel::Set_AnimIndex(_uint iAnimIndex)
{
	m_iCurrentAnimIndex = iAnimIndex;
	m_Animations[m_iCurrentAnimIndex]->ResetChennel();
	m_Animations[m_iCurrentAnimIndex]->SetPlayTime(0.f);
	Reset_AnimPos();
}

HRESULT CModel::SetUp_OnShader(CShader * pShader, _uint iMaterialIndex, aiTextureType eTextureType, const char * pConstantName)
{
	if (iMaterialIndex >= m_iNumMaterials)
		return E_FAIL;

	CTexture* pTex = m_Materials[iMaterialIndex].pTexture[eTextureType];
	if (pTex)
		return m_Materials[iMaterialIndex].pTexture[eTextureType]->Set_SRV(pShader, pConstantName);
	else
		return E_FAIL;
}

HRESULT CModel::Play_Animation(_float fTimeDelta, CTransform* pTran, CNavigation* pNavi)
{
	if (m_iCurrentAnimIndex >= m_iNumAnimations)
		return E_FAIL;

	// 보간이 끝나면 들어온다.
	/* 현재 재생하고자하는 애니메이션이 제어해야할 뼈들의 지역행렬을 갱신해낸다. */
	m_bAniEnd = m_Animations[m_iCurrentAnimIndex]->Play_Animation(fTimeDelta, this);

	if (m_bAniEnd)
	{
		Reset_AnimPos();
	}
	// 애니메이션 포스를 월드 포스에 적용
	if (pTran)
	{
		/* 지역행렬을 순차적으로(부모에서 자식으로) 누적하여 m_CombinedTransformation를 만든다.  */
		for (auto& pHierarchyNode : m_HierarchyNodes)
		{
			pHierarchyNode->Set_CombinedTransformation(m_pRoot_HierarchyName);
		}

		_vector vPos = pTran->Get_State(CTransform::STATE_POSITION);
		_matrix mWorldMatrix = pTran->Get_WorldMatrix();

		// 애니 로컬상의 이동 값을 가져온다.
		_vector vAnimPos = Get_RootPos();
		_vector vPreAnimPos = XMLoadFloat3(&m_vPreAnimPos);

		_vector vAnimLook = XMVector3Normalize(vPreAnimPos - vAnimPos);

		_vector vTotalLook = XMVector3Normalize(XMVector3TransformNormal(vAnimLook, mWorldMatrix));
		vTotalLook = XMVectorSetY(vTotalLook, XMVectorGetY(vTotalLook) * -1.f);
		// 첫번째로 이동 값이 바꼈다 -> 그 이동 값 만큼 빼서 시작 
		_float fPivotDis = XMVectorGetX(XMVector3Length(vAnimPos));
		if (0.f < fPivotDis && !m_bFristPivotPos)
		{
			vPos -= vTotalLook * fPivotDis;
			m_bFristPivotPos = true;
		}

		_float fMoveDis = XMVectorGetX(XMVector3Length(vAnimPos - vPreAnimPos));
		
		vPos += vTotalLook * fMoveDis;
		vPos = XMVectorSetW(vPos, 1.f);

		_vector vNormal = { 0.f,0.f,0.f,0.f };
		if (pNavi && pNavi->isMove(vPos, &vNormal))
			pTran->Set_State(CTransform::STATE_POSITION, vPos);
		else if (!pNavi)
			pTran->Set_State(CTransform::STATE_POSITION, vPos);

		XMStoreFloat3(&m_vPreAnimPos, vAnimPos);
	}
	else
	{
		/* 지역행렬을 순차적으로(부모에서 자식으로) 누적하여 m_CombinedTransformation를 만든다.  */
		for (auto& pHierarchyNode : m_HierarchyNodes)
		{
			pHierarchyNode->Set_CombinedTransformation();
		}
	}

	return S_OK;
}

HRESULT CModel::Play_Animation_NoMove(_float fTimeDelta)
{
	if (m_iCurrentAnimIndex >= m_iNumAnimations)
		return E_FAIL;

	// 보간이 끝나면 들어온다.
	/* 현재 재생하고자하는 애니메이션이 제어해야할 뼈들의 지역행렬을 갱신해낸다. */
	m_bAniEnd = m_Animations[m_iCurrentAnimIndex]->Play_Animation(fTimeDelta, this);

	/* 지역행렬을 순차적으로(부모에서 자식으로) 누적하여 m_CombinedTransformation를 만든다.  */
	for (auto& pHierarchyNode : m_HierarchyNodes)
	{
		pHierarchyNode->Set_CombinedTransformation(m_pRoot_HierarchyName);
	}

	return S_OK;
}

HRESULT CModel::Play_AniReverse(_float fTimeDelta)
{
	if (m_iCurrentAnimIndex >= m_iNumAnimations)
		return E_FAIL;

	/* 현재 재생하고자하는 애니메이션이 제어해야할 뼈들의 지역행렬을 갱신해낸다. */
	m_Animations[m_iCurrentAnimIndex]->Play_AniReverse(fTimeDelta, this);

	/* 지역행렬을 순차적으로(부모에서 자식으로) 누적하여 m_CombinedTransformation를 만든다.  */
	for (auto& pHierarchyNode : m_HierarchyNodes)
	{
		pHierarchyNode->Set_CombinedTransformation();
	}

	return S_OK;
}

HRESULT CModel::Play_AniReverse_NoMove(_float fTimeDelta)
{
	if (m_iCurrentAnimIndex >= m_iNumAnimations)
		return E_FAIL;

	/* 현재 재생하고자하는 애니메이션이 제어해야할 뼈들의 지역행렬을 갱신해낸다. */
	m_Animations[m_iCurrentAnimIndex]->Play_AniReverse(fTimeDelta, this);

	/* 지역행렬을 순차적으로(부모에서 자식으로) 누적하여 m_CombinedTransformation를 만든다.  */
	for (auto& pHierarchyNode : m_HierarchyNodes)
	{
		pHierarchyNode->Set_CombinedTransformation(m_pRoot_HierarchyName);
	}

	return S_OK;
}


HRESULT CModel::Render(CShader* pShader, _uint iMeshIndex, _uint iPassIndex)
{
	_float4x4		BoneMatrices[256];

	if (TYPE_ANIM == m_eModelType) 
	{
		m_Meshes[iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

		/* 모델 정점의 스키닝. */
		if (FAILED(pShader->Set_RawValue("g_BoneMatrices", BoneMatrices, sizeof(_float4x4) * 256)))
			return E_FAIL;
	}

	pShader->Begin(iPassIndex);
	
	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

HRESULT CModel::Render_CanStop(CShader * pShader, _uint iMeshIndex, _uint iPassIndex, _bool bStop)
{
	_float4x4		BoneMatrices[256];


	if (TYPE_ANIM == m_eModelType)
	{

		if (!bStop)
		{
			m_Meshes[iMeshIndex]->SetUp_BoneMatrices(BoneMatrices, XMLoadFloat4x4(&m_PivotMatrix));

			/* 모델 정점의 스키닝. */
			if (FAILED(pShader->Set_RawValue("g_BoneMatrices", BoneMatrices, sizeof(_float4x4) * 256)))
				return E_FAIL;

			pShader->Begin(iPassIndex);

			m_Meshes[iMeshIndex]->Render();
		}
		else
		{
			pShader->Begin(iPassIndex);
			m_Meshes[iMeshIndex]->Render_Stop();
			return S_OK;
		}

	}

	pShader->Begin(iPassIndex);

	m_Meshes[iMeshIndex]->Render();

	return S_OK;
}

const char * CModel::Get_Name(int AnimIndex)
{
	return m_Animations[AnimIndex]->Get_Name();
}

void CModel::Set_Name(const char * AnimName, int AnimIndex)
{
	m_Animations[AnimIndex]->Set_Name(AnimName);
}

void CModel::ChangeAnimIndex(int Index1, int Index2)
{
	CAnimation* temp = m_Animations[Index1];
	m_Animations[Index1] = m_Animations[Index2];
	m_Animations[Index2] = temp;
}

_float CModel::Get_TickPerSecond()
{
	return m_Animations[m_iCurrentAnimIndex]->Get_TickPerSecond();
}

void CModel::Set_TickPerSecond(_float fSecond)
{
	m_Animations[m_iCurrentAnimIndex]->Set_TickPerSecond(fSecond);
}

_float CModel::Get_Duration()
{
	return m_Animations[m_iCurrentAnimIndex]->Get_Duration();
}

void CModel::Set_Duration(_float fDuration)
{
	m_Animations[m_iCurrentAnimIndex]->Set_Duration(fDuration);
}

_float CModel::Get_PlayTime()
{
	return m_Animations[m_iCurrentAnimIndex]->GetPlayTime();
}

void CModel::SetPlayTime(_float fPlayTime)
{
	m_Animations[m_iCurrentAnimIndex]->SetPlayTime(fPlayTime);
}

_uint CModel::Get_KeyFrame()
{
	return m_Animations[m_iCurrentAnimIndex]->Get_KeyFrame();
}

void CModel::Set_KeyFrame(_float fKeyFrame)
{
	m_Animations[m_iCurrentAnimIndex]->Set_KeyFrame(fKeyFrame);
}

vector<_float>& CModel::GetTimeLimit()
{
	return m_Animations[m_iCurrentAnimIndex]->GetTimeLimit();
}

_float CModel::Get_TimeLimit(int iIndex)
{
	return m_Animations[m_iCurrentAnimIndex]->GetTimeLimit(iIndex);
}

void CModel::Add_TimeLimit()
{
	m_Animations[m_iCurrentAnimIndex]->GetTimeLimit().push_back(0.f);
}

void CModel::Delete_TimeLimit()
{
	if (m_Animations[m_iCurrentAnimIndex]->GetTimeLimit().size() > 0)
		m_Animations[m_iCurrentAnimIndex]->GetTimeLimit().pop_back();
}

void CModel::SetTimeLimit(_float TimeLimit, int iIndex)
{
	m_Animations[m_iCurrentAnimIndex]->SetTimeLimit(TimeLimit, iIndex);
}

void CModel::SetTimeLimit(vector<_float>& TimeLimit)
{
	m_Animations[m_iCurrentAnimIndex]->SetTimeLimit(TimeLimit);
}

void CModel::ResetKeyFrame()
{
	m_Animations[m_iCurrentAnimIndex]->ResetChennel();
}

void CModel::SetNextIndex(_uint iNextIndex, _bool bAnimLook)
{
	m_iNextAnimIndex = iNextIndex;
}

CAnimation * CModel::GetNextAnimation()
{
	return m_Animations[m_iNextAnimIndex];
}

void CModel::DeleteAnimation(int Index)
{
	Safe_Release(m_Animations[Index]);
	m_Animations.erase(m_Animations.begin() + Index);
	m_iNumAnimations = (_uint)m_Animations.size();
}






CComponent * CModel::Clone(void * pArg)
{
	CModel*			pInstance = new CModel(*this);

	if (!m_pBin_AIScene)
	{
		if (FAILED(pInstance->Initialize(pArg)))
		{
			MSG_BOX(TEXT("Failed To Cloned : CModel"));
			Safe_Release(pInstance);
		}
	}
	else
	{
		if (FAILED(pInstance->Bin_Initialize(pArg)))
		{
			MSG_BOX(TEXT("Failed To Cloned : Bin_CModel"));
			Safe_Release(pInstance);
		}
	}

	return pInstance;
}





CModel * CModel::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType, const char * pModelFilePath, const char * pModelFileName, _fmatrix PivotMatrix)
{
	CModel*			pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pModelFilePath, pModelFileName, PivotMatrix)))
	{
		MSG_BOX(TEXT("Failed To Created : CTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CModel::Initialize_Prototype(TYPE eType, const char * pModelFilePath, const char * pModelFileName, _fmatrix PivotMatrix)
{
	m_bIsBin = false;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);

	char		szFullPath[MAX_PATH] = "";

	strcpy_s(szFullPath, pModelFilePath);
	strcat_s(szFullPath, pModelFileName);

	_uint		iFlag = 0;

	m_eModelType = eType;

	// aiProcess_PreTransformVertices : 모델을 구성하는 메시 중, 이 메시의 이름과 뼈의 이름이 같은 상황이라면 이 뼈의 행렬을 메시의 정점에 다 곱해서 로드한다. 
	// 모든 애니메이션 정보는 폐기된다. 
	if (TYPE_NONANIM == eType)
		iFlag |= aiProcess_CalcTangentSpace | aiProcess_GlobalScale | aiProcess_ConvertToLeftHanded | aiProcess_PreTransformVertices;// | aiProcessPreset_TargetRealtime_MaxQuality;
	else if(TYPE_BONE == eType)
		iFlag |= aiProcess_CalcTangentSpace | aiProcess_GlobalScale | aiProcess_ConvertToLeftHanded;
	else
		iFlag |= aiProcess_CalcTangentSpace | aiProcess_GlobalScale | aiProcess_ConvertToLeftHanded;

	/* 파일의 정보를 읽어서 aiScene안에 보관한다.  */
	m_pAIScene = m_Importer.ReadFile(szFullPath, iFlag);

	if (nullptr == m_pAIScene)
		return E_FAIL;

	/* 모델을 구성하는 메시들을 만든다. */
	/* 모델은 여러개의 메시로 구성되어있다. */
	/* 각 메시의 정점들과 인덱스들을 구성한다. */
	if (FAILED(Ready_MeshContainers(PivotMatrix)))
		return E_FAIL;

	/* 머테리얼정보다.(빛을 받았을때 리턴해야할 색상정보.) */
	/* 모델마다정의?, 정점마다정의? 픽셀마다 정의(o) 텍스쳐로 표현된다. */
	if (FAILED(Ready_Materials(pModelFilePath)))
		return E_FAIL;


	/* 애니메이션의 정보를 읽어서 저장한다.  */
	/* 애니메이션 정보 : 애니메이션이 재생되는데 걸리는 총 시간(Duration),  애니메이션의 재생속도( mTickPerSecond), 몇개의 채널(mNumChannels) 에 영향르 주는가. 각채널의 정보(aiNodeAnim)(mChannels) */
	/* mChannel(aiNodeAnim, 애니메이션이 움직이는 뼈) 에 대한 정보를 구성하여 객체화한다.(CChannel) */
	/* 채널 : 뼈. 이 뼈는 한 애니메이션 안에서 사용된다. 그 애니메이션 안에서 어떤 시간, 시간, 시간, 시간대에 어떤 상태를 표현하면 되는지에 대한 정보(keyframe)들을 다므낟. */
	/* keyframe : 어떤시간?, 상태(vScale, vRotation, vPosition) */
	if (FAILED(Ready_Animations()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void * pArg)
{
	/* 뼈대 정볼르 로드하낟. */
	/* 이 모델 전체의 뼈의 정보를 로드한다. */
	/* HierarchyNode : 뼈의 상태를 가진다.(offSetMatrix, Transformation, CombinedTransformation */
	Ready_HierarchyNodes(m_pAIScene->mRootNode, nullptr, 0);

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
	{
		return pSour->Get_Depth() < pDest->Get_Depth();
	});

	m_pHierarchyPointer = &m_HierarchyNodes;

	// 매쉬마다 자신한테 영향을 주는 뼈들을 담는다.
	if (TYPE_ANIM == m_eModelType || TYPE_BONE == m_eModelType)
	{
		_uint		iNumMeshes = 0;

		vector<CMeshContainer*>		MeshContainers;

		for (auto& pPrototype : m_Meshes)
		{
			CMeshContainer*		pMeshContainer = (CMeshContainer*)pPrototype->Clone();
			if (nullptr == pMeshContainer)
				return E_FAIL;

			MeshContainers.push_back(pMeshContainer);

			Safe_Release(pPrototype);
		}

		m_Meshes.clear();

		m_Meshes = MeshContainers;

		for (auto& pMeshContainer : m_Meshes)
		{
			if (nullptr != pMeshContainer)
				pMeshContainer->SetUp_HierarchyNodes(this, m_pAIScene->mMeshes[iNumMeshes++]);
		}
	}

	vector<CAnimation*>		Animations;

	for (auto& pPrototype : m_Animations)
	{
		CAnimation*		pAnimation = pPrototype->Clone(this);
		if (nullptr == pAnimation)
			return E_FAIL;

		Animations.push_back(pAnimation);

		Safe_Release(pPrototype);
	}

	m_Animations.clear();

	m_Animations = Animations;

	return S_OK;
}




HRESULT CModel::Ready_MeshContainers(_fmatrix PivotMatrix)
{
	/* 메시의 갯수를 얻어온다. */
	m_iNumMeshes = m_pAIScene->mNumMeshes;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		CMeshContainer*		pMeshContainer = CMeshContainer::Create(m_pDevice, m_pContext, m_eModelType, m_pAIScene->mMeshes[i], this, PivotMatrix);
		if (nullptr == pMeshContainer)
			return E_FAIL;

		m_Meshes.push_back(pMeshContainer);
	}

	return S_OK;
}

HRESULT CModel::Ready_Materials(const char* pModelFilePath)
{
	if (nullptr == m_pAIScene)
		return E_FAIL;

	m_iNumMaterials = m_pAIScene->mNumMaterials;

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MATERIALDESC		MaterialDesc;
		ZeroMemory(&MaterialDesc, sizeof(MATERIALDESC));

		// *
		DATA_HEROMATERIAL		DataMaterialDesc;
		ZeroMemory(&DataMaterialDesc, sizeof(DATA_HEROMATERIAL));


		aiMaterial*			pAIMaterial = m_pAIScene->mMaterials[i];

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			aiString		strPath;

			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strPath)))
				continue;

			char			szFullPath[MAX_PATH] = "";
			char			szFileName[MAX_PATH] = "";
			char			szExt[MAX_PATH] = "";

			_splitpath_s(strPath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			char			szSaveName[MAX_PATH] = "";
			strcat_s(szSaveName, szFileName);
			strcat_s(szSaveName, szExt);
			
			memcpy(&DataMaterialDesc.cNames[j], &szSaveName, sizeof(char) * MAX_PATH);

			strcpy_s(szFullPath, pModelFilePath);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);

			_tchar			szWideFullPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szFullPath, (_int)strlen(szFullPath), szWideFullPath, MAX_PATH);


			MaterialDesc.pTexture[j] = CTexture::Create(m_pDevice, m_pContext, szWideFullPath);
			if (nullptr == MaterialDesc.pTexture[j])
			{
				return E_FAIL;
			}
				
		}

		m_Materials.push_back(MaterialDesc);


		m_DataMaterials.push_back(DataMaterialDesc);
	}

	return S_OK;
}

HRESULT CModel::Ready_HierarchyNodes(aiNode* pNode, CHierarchyNode* pParent, _uint iDepth)
{
	// pNode: Assimp 계층뼈 정보,   pParent: 부모 계층뼈,   iDepth: 부모부터 내림차순으로 정렬하기 위해 사용
	CHierarchyNode*		pHierarchyNode = CHierarchyNode::Create(pNode, pParent, iDepth++);

	if (nullptr == pHierarchyNode)
		return E_FAIL;

	m_HierarchyNodes.push_back(pHierarchyNode);

	for (_uint i = 0; i < pNode->mNumChildren; ++i)
	{
		// 재귀 호출
		Ready_HierarchyNodes(pNode->mChildren[i], pHierarchyNode, iDepth);
	}	

	return S_OK;
}

HRESULT CModel::Ready_Animations()
{
	m_iNumAnimations = m_pAIScene->mNumAnimations;

	for (_uint i = 0; i < m_pAIScene->mNumAnimations; ++i)
	{
		aiAnimation*		pAIAnimation = m_pAIScene->mAnimations[i];

		/*I 애니메이션 마다 객체화 하는 이유 : 현재 재생 시간에 맞는 채널들의 뼈 상태를 셋팅한다. (조난 빡세다) 
		함수로 만들어야지뭐. */
		CAnimation*			pAnimation = CAnimation::Create(pAIAnimation);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}	
	return S_OK;
}




// For. Anim_World
_vector CModel::Get_RootPos()
{
	return XMVectorSetW(m_pRoot_HierarchyNode->Get_RootAnim_CombinedTransformation().r[3] + m_pRoot_HierarchyNode->Get_OffSetMatrix().r[3], 1.f);
}

void CModel::Set_RootHierarchy(_uint iIndex)
{
	m_pRoot_HierarchyNode = m_HierarchyNodes[iIndex];
	if (m_pRoot_HierarchyNode)
		m_pRoot_HierarchyName = m_pRoot_HierarchyNode->Get_Name();
}




// For. Bone_Model
HRESULT CModel::Play_Bone()
{
	for (auto& pHierarchyNode : m_HierarchyNodes)
		pHierarchyNode->Set_OriTransformation();

	return S_OK;
}




// For. NoneModelVtx
void CModel::Make_NonModelVtx(_uint iIndex)
{
	Set_AnimIndex(iIndex);
	Play_Animation(0.1f);

	_uint		iNumMeshes = Get_NumMeshes();
	

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_Meshes[i]->Update_DynVertex(this);
	}

}





// For. BinModelData
CModel * CModel::Bin_Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, DATA_HEROSCENE* pScene, TYPE eType, const char * pModelFilePath, const char * pModelFileName, _fmatrix PivotMatrix)
{
	CModel*			pInstance = new CModel(pDevice, pContext);

	if (FAILED(pInstance->Bin_Initialize_Prototype(pScene, eType, pModelFilePath, pModelFileName, PivotMatrix)))
	{
		MSG_BOX(TEXT("Failed To Created : CTexture"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CModel::Bin_Initialize(void * pArg)
{
	Bin_Ready_HierarchyNodes();

	sort(m_HierarchyNodes.begin(), m_HierarchyNodes.end(), [](CHierarchyNode* pSour, CHierarchyNode* pDest)
	{
		return pSour->Get_Depth() < pDest->Get_Depth();
	});

	m_pHierarchyPointer = &m_HierarchyNodes;

	for (int i = 0; i < m_pBin_AIScene->iNodeCount; ++i)
	{
		for (int j = 0; j < m_pBin_AIScene->iNodeCount; ++j)
		{
			if (nullptr != m_HierarchyNodes[i]->Get_Parent())
				break;

			m_HierarchyNodes[i]->Set_FindParent(m_HierarchyNodes[j]);
		}
	}

	int iBin = 1;
	// 매쉬마다 자신한테 영향을 주는 뼈들을 담는다.
	if (TYPE_ANIM == m_eModelType || TYPE_BONE == m_eModelType)
	{
		_uint		iNumMeshes = 0;

		vector<CMeshContainer*>		MeshContainers;

		for (auto& pPrototype : m_Meshes)
		{

			CMeshContainer*		pMeshContainer = (CMeshContainer*)pPrototype->Clone(&iBin);
			if (nullptr == pMeshContainer)
				return E_FAIL;

			MeshContainers.push_back(pMeshContainer);

			Safe_Release(pPrototype);
		}

		m_Meshes.clear();

		m_Meshes = MeshContainers;

		for (auto& pMeshContainer : m_Meshes)
		{
			if (nullptr != pMeshContainer)
				pMeshContainer->Bin_SetUp_HierarchyNodes(this, &m_pBin_AIScene->pHeroMesh[iNumMeshes++]);
		}
	}

	vector<CAnimation*>		Animations;

	for (auto& pPrototype : m_Animations)
	{
		CAnimation*		pAnimation = pPrototype->Bin_Clone(this);
		if (nullptr == pAnimation)
			return E_FAIL;

		Animations.push_back(pAnimation);

		Safe_Release(pPrototype);
	}

	m_Animations.clear();

	m_Animations = Animations;
	return S_OK;
}

HRESULT CModel::Bin_Initialize_Prototype(DATA_HEROSCENE* pScene, TYPE eType, const char* pModelFilePath, const char * pModelFileName, _fmatrix PivotMatrix)
{

	m_bIsBin = true;

	XMStoreFloat4x4(&m_PivotMatrix, PivotMatrix);
	m_eModelType = eType;

	m_pBin_AIScene = pScene;

	if (nullptr == m_pBin_AIScene)
		return E_FAIL;



	/* 모델을 구성하는 메시들을 만든다. */
	if (FAILED(Bin_Ready_MeshContainers(PivotMatrix)))
		return E_FAIL;

	if (FAILED(Bin_Ready_Materials(pModelFilePath)))
		return E_FAIL;

	if (FAILED(Bin_Ready_Animations()))
		return E_FAIL;

	return S_OK;
}


HRESULT CModel::Bin_Ready_MeshContainers(_fmatrix PivotMatrix)
{
	m_iNumMeshes = m_pBin_AIScene->iMeshCount;

	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{

		CMeshContainer*		pMeshContainer = CMeshContainer::Bin_Create(m_pDevice, m_pContext, m_eModelType, &m_pBin_AIScene->pHeroMesh[i], this, PivotMatrix);
		if (nullptr == pMeshContainer)
			return E_FAIL;

		m_Meshes.push_back(pMeshContainer);
	}

	return S_OK;
}

HRESULT CModel::Bin_Ready_Materials(const char* pModelFilePath)
{
	if (nullptr == m_pBin_AIScene)
		return E_FAIL;

	m_iNumMaterials = m_pBin_AIScene->iMaterialCount;

	for (_uint i = 0; i < m_iNumMaterials; ++i)
	{
		MATERIALDESC		MaterialDesc;
		ZeroMemory(&MaterialDesc, sizeof(MATERIALDESC));

		DATA_HEROMATERIAL		DataMaterialDesc;
		ZeroMemory(&DataMaterialDesc, sizeof(DATA_HEROMATERIAL));

		DATA_HEROMATERIAL			pAIMaterial = m_pBin_AIScene->pHeroMaterial[i];

		for (_uint j = 0; j < AI_TEXTURE_TYPE_MAX; ++j)
		{
			if (!strcmp(pAIMaterial.cNames[j], ""))
				continue;


			char			szFullPath[MAX_PATH] = "";
			char			szFileName[MAX_PATH] = "";
			char			szExt[MAX_PATH] = "";

			_splitpath_s(pAIMaterial.cNames[j], nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			memcpy(&DataMaterialDesc.cNames[j], &szFileName, sizeof(char) * MAX_PATH);

			strcpy_s(szFullPath, pModelFilePath);
			strcat_s(szFullPath, szFileName);
			// strcat_s(szFullPath, pAIMaterial.cNames[j]);

			if (!strcmp("", szExt))
			{
				strcpy_s(szExt, ".dds");
				strcat_s(szFullPath, szExt);
			}
			else
				strcat_s(szFullPath, szExt);

			_tchar			szWideFullPath[MAX_PATH] = TEXT("");

			MultiByteToWideChar(CP_ACP, 0, szFullPath, (int)strlen(szFullPath), szWideFullPath, MAX_PATH);


			MaterialDesc.pTexture[j] = CTexture::Create(m_pDevice, m_pContext, szWideFullPath);
			if (nullptr == MaterialDesc.pTexture[j])
				return E_FAIL;
		}

		m_Materials.push_back(MaterialDesc);

		m_DataMaterials.push_back(DataMaterialDesc);
	}

	return S_OK;
}

HRESULT CModel::Bin_Ready_HierarchyNodes()
{
	for (int i = 0; i < m_pBin_AIScene->iNodeCount; ++i)
	{
		DATA_HERONODE Node = m_pBin_AIScene->pHeroNodes[i];

		CHierarchyNode*		pHierarchyNode = CHierarchyNode::Bin_Create(&Node);
		if (nullptr == pHierarchyNode)
			return E_FAIL;

		m_HierarchyNodes.push_back(pHierarchyNode);
	}

	return S_OK;
}

HRESULT CModel::Bin_Ready_Animations()
{
	m_iNumAnimations = m_pBin_AIScene->iNumAnimations;

	for (_uint i = 0; i < (_uint)m_pBin_AIScene->iNumAnimations; ++i)
	{
		DATA_HEROANIM*		pAIAnimation = &m_pBin_AIScene->pHeroAnim[i];

		CAnimation*			pAnimation = CAnimation::Bin_Create(pAIAnimation);
		if (nullptr == pAnimation)
			return E_FAIL;

		m_Animations.push_back(pAnimation);
	}
	return S_OK;
}


HRESULT CModel::Safe_Release_Scene()
{
	Safe_Delete_Array(m_pBin_AIScene->pHeroNodes);
	Safe_Delete_Array(m_pBin_AIScene->pHeroMaterial);

	for (_int i = 0; i < m_pBin_AIScene->iMeshCount; ++i)
	{
		Safe_Delete_Array(m_pBin_AIScene->pHeroMesh[i].pAnimVertices);
		Safe_Delete_Array(m_pBin_AIScene->pHeroMesh[i].pNonAnimVertices);
		Safe_Delete_Array(m_pBin_AIScene->pHeroMesh[i].pIndices);
		Safe_Delete_Array(m_pBin_AIScene->pHeroMesh[i].pBones);
	}
	Safe_Delete_Array(m_pBin_AIScene->pHeroMesh);


	for (_int i = 0; i < m_pBin_AIScene->iNumAnimations; ++i)
	{
		for (_int j = 0; j < m_pBin_AIScene->pHeroAnim[i].iNumChannels; ++j)
		{
			Safe_Delete_Array(m_pBin_AIScene->pHeroAnim[i].pHeroChannel[j].pKeyFrames);
		}
		Safe_Delete_Array(m_pBin_AIScene->pHeroAnim[i].pHeroChannel);

		Safe_Delete_Array(m_pBin_AIScene->pHeroAnim[i].pTimeLimits);
	}
	Safe_Delete_Array(m_pBin_AIScene->pHeroAnim);

	Safe_Delete(m_pBin_AIScene);

	return S_OK;
}






// For. Bin화 추출 함수
HRESULT CModel::Get_HierarchyNodeData(DATA_HEROSCENE * pHeroScene)
{
	if (0 == (_int)m_HierarchyNodes.size())
	{
		pHeroScene->pHeroNodes = nullptr;
		return S_OK;
	}

	pHeroScene->pHeroNodes = new DATA_HERONODE[m_HierarchyNodes.size()];
	pHeroScene->iNodeCount = (_int)m_HierarchyNodes.size();

	for (_int i = 0; i < m_HierarchyNodes.size(); ++i)
	{
		DATA_HERONODE Hero_Node;
		ZeroMemory(&Hero_Node, sizeof(DATA_HERONODE));

		const char* pMyName = m_HierarchyNodes[i]->Get_Name();
		const char* pParentName = m_HierarchyNodes[i]->Get_ParentName();
		memcpy(&Hero_Node.cName, pMyName, sizeof(char) * MAX_PATH);
		memcpy(&Hero_Node.cParent, pParentName, sizeof(char) * MAX_PATH);

		Hero_Node.iDepth = m_HierarchyNodes[i]->Get_Depth();

		Hero_Node.mTransform = m_HierarchyNodes[i]->Get_OriTransformation();

		pHeroScene->pHeroNodes[i] = Hero_Node;
	}

	return S_OK;
}

HRESULT CModel::Get_MaterialData(DATA_HEROSCENE * pHeroScene)
{
	pHeroScene->iMaterialCount = m_iNumMaterials;
	pHeroScene->pHeroMaterial = new DATA_HEROMATERIAL[m_iNumMaterials];

	for (_uint i = 0; i < m_DataMaterials.size(); ++i)
	{
		memcpy(&pHeroScene->pHeroMaterial[i], &m_DataMaterials[i], sizeof(DATA_HEROMATERIAL));
	}

	return S_OK;
}

HRESULT CModel::Get_MeshData(DATA_HEROSCENE * pNodeData)
{
	pNodeData->iMeshCount = m_iNumMeshes;
	pNodeData->pHeroMesh = new DATA_HEROMETH[m_iNumMeshes];
	for (_uint i = 0; i < m_iNumMeshes; ++i)
	{
		DATA_HEROMETH HeroMesh;
		m_Meshes[i]->Get_MeshData(&HeroMesh);
		memcpy(&pNodeData->pHeroMesh[i], &HeroMesh, sizeof(DATA_HEROMETH));
	}

	return S_OK;
}

HRESULT CModel::Get_AnimData(DATA_HEROSCENE * pSceneData)
{
	if (0 == m_iNumAnimations)
	{
		pSceneData->pHeroAnim = nullptr;
		pSceneData->iNumAnimations = 0;
		return S_OK;
	}

	pSceneData->iNumAnimations = m_iNumAnimations;
	pSceneData->pHeroAnim = new DATA_HEROANIM[m_iNumAnimations];

	for (_uint i = 0; i < m_iNumAnimations; ++i)
	{
		m_Animations[i]->Get_AnimData(&pSceneData->pHeroAnim[i]);
	}

	return S_OK;
}








void CModel::Free()
{
	__super::Free();

	for (auto& pHierarchyNode : m_HierarchyNodes)
		Safe_Release(pHierarchyNode);

	m_HierarchyNodes.clear();

	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
			Safe_Release(Material.pTexture[i]);
	}
	m_Materials.clear();

	for (auto& pMeshContainer : m_Meshes)
		Safe_Release(pMeshContainer);

	m_Meshes.clear();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);

	m_Animations.clear();

	m_Importer.FreeScene();

	if (m_pBin_AIScene && m_bIsProto)
		Safe_Release_Scene();
}
