#include "..\Public\Channel.h"
#include "Model.h"
#include "HierarchyNode.h"
#include "Channel.h"
#include "Animation.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(aiNodeAnim * pAIChannel)
{
	/* 특정 애니메이션에서 사용되는 뼈의 정보이다. */
	/* 이 이름은 모델이 가지고 있는 HierarchyNodes의 뼈대 중 한놈과 이름이 같을 것이다. */
	/* 이 이름으로 같은 이름을 가진 HierarchyNodes를 채널에 보관해둔다. */
	/* 왜 보관하니? : 채널이 가진 키프레임중 시간대에 맞는 키플에ㅣㅁ 상태를 만들고. 이걸로 행렬 만들고. 
	이렇게 만든 행렬을 erarchyNodes에 저장해놔야해. */


	strcpy_s(m_szName, pAIChannel->mNodeName.data);


	//m_pHierarchyNode = pModel->Get_HierarchyNode(m_szName);
	//if (nullptr == m_pHierarchyNode)
	//	return E_FAIL;

	// Safe_AddRef(m_pHierarchyNode);


	/* 키프레임 정보들를 로드한다. */
	/* 키프레임 : 전체애니메이션 동작 중, 특정 시간대에 이 ㅜ뼈가 표현해야할 동작의 상태 행렬정보이다. */

	/* 이 키프레임 갯수는 크기, 회전, 이동마다 갯수가 다를 수 있다. */
	/* 개숫가 다르다? : 부족한놈은 이전 시간대에 키프레임ㅇ 상태를 가지기 때문이다. */
	// 크기, 회전, 이동 중 가장 많은 키프레임 찾기
	m_iNumKeyFrames = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
	m_iNumKeyFrames = max(m_iNumKeyFrames, pAIChannel->mNumPositionKeys);

	/* 이 변수를 루프 밖에 선언한이유? 없는 상태인 경우 저장해놨던 
	이전 루프의 상태를 이용해서 키프레임 상태를 만들기 위해. */
	_float3			vScale;
	_float4			vRotation;
	_float3			vPosition;

	for (_uint i = 0; i < m_iNumKeyFrames; ++i)
	{
		KEYFRAME			KeyFrame;
		ZeroMemory(&KeyFrame, sizeof(KEYFRAME));

		if(i < pAIChannel->mNumScalingKeys)
		{
			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTime = (_float)pAIChannel->mScalingKeys[i].mTime;
		}
		if (i < pAIChannel->mNumRotationKeys)
		{
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
			KeyFrame.fTime = (_float)pAIChannel->mRotationKeys[i].mTime;
		}
		if (i < pAIChannel->mNumPositionKeys)
		{
			memcpy(&vPosition, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.fTime = (_float)pAIChannel->mPositionKeys[i].mTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vPosition = vPosition;

		m_KeyFrames.push_back(KeyFrame);
	}	

	return S_OK;
}

_uint CChannel::Update_Transformation(_float fPlayTime, _uint iCurrentKeyFrame, CHierarchyNode* pNode)
{
#pragma region 중략
	_float3			vScale;
	_float4			vRotation;
	_float3			vPosition;

	
	if (fPlayTime >= m_KeyFrames.back().fTime)
	{
		vScale = m_KeyFrames.back().vScale;
		vRotation = m_KeyFrames.back().vRotation;
		vPosition = m_KeyFrames.back().vPosition;		
	}
#pragma endregion

	else // 현재 KeyFrame과 다음 KeyFrame을 보간하여 행렬을 구한다.
	{
		while (fPlayTime >= m_KeyFrames[iCurrentKeyFrame + 1].fTime)
			++iCurrentKeyFrame;

		_float		fRatio = (fPlayTime - m_KeyFrames[iCurrentKeyFrame].fTime) /
			(m_KeyFrames[iCurrentKeyFrame + 1].fTime - m_KeyFrames[iCurrentKeyFrame].fTime);

		_float3		vSourScale, vDestScale;
		_float4		vSourRotation, vDestRotation;
		_float3		vSourPosition, vDestPosition;

		vSourScale = m_KeyFrames[iCurrentKeyFrame].vScale;
		vDestScale = m_KeyFrames[iCurrentKeyFrame + 1].vScale;

		vSourRotation = m_KeyFrames[iCurrentKeyFrame].vRotation;
		vDestRotation = m_KeyFrames[iCurrentKeyFrame + 1].vRotation;

		vSourPosition = m_KeyFrames[iCurrentKeyFrame].vPosition;
		vDestPosition = m_KeyFrames[iCurrentKeyFrame + 1].vPosition;

		XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio));
		XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio));
		XMStoreFloat3(&vPosition, XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), fRatio));
	}

	_matrix		TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), 
		XMLoadFloat4(&vRotation), XMVectorSetW(XMLoadFloat3(&vPosition), 1.f));	

	// Channel이 가지는 계층뼈의 위치를 갱신한다.
	if (nullptr != pNode)
		pNode->Set_Transformation(TransformationMatrix);

	return iCurrentKeyFrame;
}

_uint CChannel::Update_ReverseTransformation(_float fPlayTime, _uint iCurrentKeyFrame, CHierarchyNode * pNode)
{
	_float3			vScale;
	_float4			vRotation;
	_float3			vPosition;

	if (fPlayTime <= 0.f)
	{
		vScale = m_KeyFrames.front().vScale;
		vRotation = m_KeyFrames.front().vRotation;
		vPosition = m_KeyFrames.front().vPosition;
	}

	else
	{
		if (iCurrentKeyFrame > 0)
		{
			while (fPlayTime <= m_KeyFrames[iCurrentKeyFrame - 1].fTime)		
				--iCurrentKeyFrame;		
		}
	}

	return iCurrentKeyFrame;
}

_uint CChannel::Interpolation(_float fTimeDelta, _uint iNextIndex, _float fPlayTime, _uint iCurrentKeyFrame, CHierarchyNode * pNode, CModel * pAnimModel)
{
	_float3			vScale;
	_float4			vRotation;
	_float3			vPosition;
	m_fInterpolationTime += fTimeDelta * 1.f;

	if (m_fInterpolationTime >= 0.1f)
	{
		pAnimModel->SetChangeBool(false);
		pAnimModel->Set_AnimIndex(iNextIndex);
		m_fInterpolationTime = 0.f;

		return iCurrentKeyFrame;
	}

	else
	{
		CChannel* p = pAnimModel->GetNextAnimation()->GetChannels(m_szName);

		KEYFRAME NextKeyFrame = p->GetFirstKeyFrame();

		_float		fRatio = m_fInterpolationTime / 0.1f;

		_float3		vSourScale, vDestScale;
		_float4		vSourRotation, vDestRotation;
		_float3		vSourPosition, vDestPosition;

		vSourScale = m_KeyFrames[iCurrentKeyFrame].vScale;
		vDestScale = NextKeyFrame.vScale;

		vSourRotation = m_KeyFrames[iCurrentKeyFrame].vRotation;
		vDestRotation = NextKeyFrame.vRotation;

		vSourPosition = m_KeyFrames[iCurrentKeyFrame].vPosition;
		vDestPosition = NextKeyFrame.vPosition;

		XMStoreFloat3(&vScale, XMVectorLerp(XMLoadFloat3(&vSourScale), XMLoadFloat3(&vDestScale), fRatio));
		XMStoreFloat4(&vRotation, XMQuaternionSlerp(XMLoadFloat4(&vSourRotation), XMLoadFloat4(&vDestRotation), fRatio));
		XMStoreFloat3(&vPosition, XMVectorLerp(XMLoadFloat3(&vSourPosition), XMLoadFloat3(&vDestPosition), fRatio));
	}

	_matrix		TransformationMatrix = XMMatrixAffineTransformation(XMLoadFloat3(&vScale), XMVectorSet(0.f, 0.f, 0.f, 1.f), XMLoadFloat4(&vRotation), XMVectorSetW(XMLoadFloat3(&vPosition), 1.f));

	if (nullptr != pNode)
		pNode->Set_Transformation(TransformationMatrix);

	return iCurrentKeyFrame;
}

CChannel * CChannel::Create(aiNodeAnim * pAIChannel)
{
	CChannel*			pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pAIChannel)))
	{
		MSG_BOX(TEXT("Failed To Created : CChannel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}





CChannel * CChannel::Bin_Create(DATA_HEROCHANNEL* pAIChannel, _uint& iMaxFrameIndex)
{
 	CChannel*			pInstance = new CChannel();

	if (FAILED(pInstance->Bin_Initialize(pAIChannel, iMaxFrameIndex)))
	{
		MSG_BOX(TEXT("Failed To Created : CBinChannel"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

HRESULT CChannel::Bin_Initialize(DATA_HEROCHANNEL* pAIChannel, _uint& iMaxFrameIndex)
{
	/* 특정 애니메이션에서 사용되는 뼈의 정보이다. */
	/* 이 이름은 모델이 가지고 있는 HierarchyNodes의 뼈대 중 한놈과 이름이 같을 것이다. */
	strcpy_s(m_szName, pAIChannel->szName);

	//m_pHierarchyNode = pModel->Get_HierarchyNode(m_szName);
	//if (nullptr == m_pHierarchyNode)
	//	return E_FAIL;

	// Safe_AddRef(m_pHierarchyNode);


	/* 키프레임 정보들를 로드한다. */
	/* 키프레임 : 전체애니메이션 동작 중, 특정 시간대에 이뼈가 표현해야할 동작의 상태 행렬정보이다. */

	m_iNumKeyFrames = pAIChannel->iNumKeyFrames;
	if (m_iNumKeyFrames > iMaxFrameIndex)
		iMaxFrameIndex = m_iNumKeyFrames;

	for (_uint i = 0; i < m_iNumKeyFrames; ++i)
	{
		KEYFRAME			KeyFrame;
		ZeroMemory(&KeyFrame, sizeof(KEYFRAME));

		KeyFrame = pAIChannel->pKeyFrames[i];

		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}


// For. 데이터 추출
void CChannel::Get_ChannelData(DATA_HEROCHANNEL * pData)
{
	memcpy(&pData->szName, m_szName, sizeof(char)*MAX_PATH);
	pData->iNumKeyFrames = m_iNumKeyFrames;

	pData->pKeyFrames = new KEYFRAME[m_iNumKeyFrames];

	for (_uint i = 0; i < m_iNumKeyFrames; ++i)
	{
		memcpy(&pData->pKeyFrames[i], &m_KeyFrames[i], sizeof(KEYFRAME));
	}
}



void CChannel::Free()
{

}

