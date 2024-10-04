#include "..\Public\Animation.h"
#include "Channel.h"
#include "Model.h"
#include "Model_Instance.h"
#include "HierarchyNode.h"

CAnimation::CAnimation()
{
}

CAnimation::CAnimation(const CAnimation & rhs)
	: m_fDuration(rhs.m_fDuration)
	, m_Channels(rhs.m_Channels)
	, m_iNumChannels(rhs.m_iNumChannels)
	, m_fTickPerSecond(rhs.m_fTickPerSecond)
	, m_fPlayTime(rhs.m_fPlayTime)
	,m_ChannelOldKeyFrames(rhs.m_ChannelOldKeyFrames)
	, m_TimeLimit(rhs.m_TimeLimit)
	, m_bLoop(rhs.m_bLoop)
	

{
	for (auto& pChannel : m_Channels)
		Safe_AddRef(pChannel);

	strcpy_s(m_szName, rhs.m_szName);
}

HRESULT CAnimation::Initialize_Prototype(aiAnimation * pAIAnimation)
{
	m_fDuration = (_float)pAIAnimation->mDuration;
	m_fTickPerSecond = (_float)pAIAnimation->mTicksPerSecond;
	strcpy_s(m_szName, pAIAnimation->mName.data);

	/* 현재 애니메이션에서 제어해야할 뼈들의 갯수를 저장한다. */
	m_iNumChannels = pAIAnimation->mNumChannels;


	/* 현재 애니메이션에서 제어해야할 뼈정보들을 생성하여 보관한다. */
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		/* aiNodeAnim : mChannel은 키프레임 정보들을 가지낟. */
		CChannel*		pChannel = CChannel::Create(pAIAnimation->mChannels[i]);
		if (nullptr == pChannel)
			return E_FAIL;

		/* 왜 모아두는데?> 특정 애님에ㅣ션 상태일때 애니메이션을 재생하면 모든 뼈의 상태를 갱신하는건 빡세. 느려. 
		현재 애미에시연을 구동하기위한 뼈대만 상태 갱신해주기 위해. */
		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

HRESULT CAnimation::Initialize(CModel* pModel)
{
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		m_ChannelKeyFrames.push_back(0);
		m_ChannelOldKeyFrames.push_back(0);
		CHierarchyNode*		pNode = pModel->Get_HierarchyNode(m_Channels[i]->Get_Name());

		if (nullptr == pNode)
			return E_FAIL;		

		m_HierarchyNodes.push_back(pNode);
		
		Safe_AddRef(pNode);
	}

	return S_OK;
}


_bool CAnimation::Play_Animation(_float fTimeDelta, CModel* pAnimModel)
{
	m_iChannelIndex = 0;

	m_bAnimEnd = false;

	if (pAnimModel->GetChangeBool())
	{
		pAnimModel->Reset_AnimPos();
		for (auto& pChannel : m_Channels)
		{		
			pChannel->Interpolation(fTimeDelta, pAnimModel->GetNextIndex(), m_fPlayTime, m_ChannelOldKeyFrames[m_iChannelIndex], m_HierarchyNodes[m_iChannelIndex], pAnimModel);

			++m_iChannelIndex;
		}
		return m_bAnimEnd;
	}

	m_fPlayTime += m_fTickPerSecond * fTimeDelta;

	if (m_fPlayTime >= m_fDuration)
	{
		m_fPlayTime = 0.f;
		m_bAnimEnd = true;
		ResetChennel();
		pAnimModel->Reset_AnimPos();
		return m_bAnimEnd;
	}

	for (auto& pChannel : m_Channels)
	{
		m_ChannelKeyFrames[m_iChannelIndex] = pChannel->Update_Transformation(m_fPlayTime, m_ChannelKeyFrames[m_iChannelIndex], m_HierarchyNodes[m_iChannelIndex]);
		m_ChannelOldKeyFrames[m_iChannelIndex] = m_ChannelKeyFrames[m_iChannelIndex];
		++m_iChannelIndex;
	}

	return m_bAnimEnd;
}

_bool CAnimation::Play_Animation(_float fTimeDelta, CModel_Instance * pAnimModel)
{
	m_iChannelIndex = 0;

	m_bAnimEnd = false;

	m_fPlayTime += m_fTickPerSecond * fTimeDelta;

	if (m_fPlayTime >= m_fDuration)
	{
		m_fPlayTime = 0.f;
		m_bAnimEnd = true;
		ResetChennel();
		return m_bAnimEnd;
	}

	for (auto& pChannel : m_Channels)
	{
		m_ChannelKeyFrames[m_iChannelIndex] = pChannel->Update_Transformation(m_fPlayTime, m_ChannelKeyFrames[m_iChannelIndex], m_HierarchyNodes[m_iChannelIndex]);
		++m_iChannelIndex;
	}

	return m_bAnimEnd;
}


_bool CAnimation::Play_AniReverse(_float fTimeDelta, CModel * pAnimModel)
{
	m_iChannelIndex = 0;

	m_fPlayTime -= m_fTickPerSecond * fTimeDelta;

	if (m_fPlayTime <= 0)
	{
		m_fPlayTime = 0.f;
	}

	for (auto& pChannel : m_Channels)
	{
		m_ChannelKeyFrames[m_iChannelIndex] = pChannel->Update_ReverseTransformation(m_fPlayTime, m_ChannelKeyFrames[m_iChannelIndex], m_HierarchyNodes[m_iChannelIndex]);
		m_ChannelOldKeyFrames[m_iChannelIndex] = m_ChannelKeyFrames[m_iChannelIndex];
		++m_iChannelIndex;
	}

	m_iChannelIndex = 0;

	for (auto& pChannel : m_Channels)
	{
		m_ChannelKeyFrames[m_iChannelIndex] = pChannel->Update_Transformation(m_fPlayTime, m_ChannelKeyFrames[m_iChannelIndex], m_HierarchyNodes[m_iChannelIndex]);
		m_ChannelOldKeyFrames[m_iChannelIndex] = m_ChannelKeyFrames[m_iChannelIndex];
		++m_iChannelIndex;
	}

	return m_bAnimEnd;
}

void CAnimation::ResetChennel()
{
	for (auto& iCurrentKeyFrame : m_ChannelKeyFrames)
		iCurrentKeyFrame = 0;
}


void CAnimation::Set_KeyFrame(_float fKeyFrame)
{
	m_fPlayTime += fKeyFrame;
}

void CAnimation::SetTimeLimit(vector<_float>& TimeLimit)
{
	for (int i = 0; i < TimeLimit.size(); ++i)
		m_TimeLimit.push_back(TimeLimit[i]);
}

CChannel * CAnimation::GetChannels(char * sName)
{
	for (auto& iter : m_Channels)
	{
		if (!strcmp(sName, iter->Get_Name()))
		{
			return iter;
		}
	}
	return nullptr;
}

CAnimation * CAnimation::Create(aiAnimation * pAIAnimation)
{
	CAnimation*			pInstance = new CAnimation();

	if (FAILED(pInstance->Initialize_Prototype(pAIAnimation)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation * CAnimation::Clone(CModel* pModel)
{
	CAnimation*			pInstance = new CAnimation(*this);

	if (FAILED(pInstance->Initialize(pModel)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}



// For. BinData
HRESULT CAnimation::Bin_Initialize_Prototype(DATA_HEROANIM* pAIAnimation)
{
	m_fDuration = pAIAnimation->fDuration;
	m_fTickPerSecond = pAIAnimation->fTickPerSecond;
	m_bLoop = pAIAnimation->bLoop;

	strcpy_s(m_szName, pAIAnimation->szName);

	/* 현재 애니메이션에서 제어해야할 뼈들의 갯수를 저장한다. */
	m_iNumChannels = pAIAnimation->iNumChannels;


	/* 현재 애니메이션에서 제어해야할 뼈정보들을 생성하여 보관한다. */
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CChannel*		pChannel = CChannel::Bin_Create(&pAIAnimation->pHeroChannel[i], m_iMaxKeyFrameIndex);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	/* TimeLimit 값을 넣어준다. */
	for (int i = 0; i < pAIAnimation->iNumTimeLimit; ++i)
		m_TimeLimit.push_back(pAIAnimation->pTimeLimits[i]);


	return S_OK;
}

HRESULT CAnimation::Bin_Initialize(CModel* pModel)
{
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		m_ChannelKeyFrames.push_back(0);
		m_ChannelOldKeyFrames.push_back(0);

		CHierarchyNode*		pNode = pModel->Get_HierarchyNode(m_Channels[i]->Get_Name());

		if (nullptr == pNode)
			return E_FAIL;

		m_HierarchyNodes.push_back(pNode);

		Safe_AddRef(pNode);
	}

	return S_OK;
}

HRESULT CAnimation::Bin_Initialize(CModel_Instance* pModel)
{
	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		m_ChannelKeyFrames.push_back(0);
		m_ChannelOldKeyFrames.push_back(0);

		CHierarchyNode*		pNode = pModel->Get_HierarchyNode(m_Channels[i]->Get_Name());

		if (nullptr == pNode)
			return E_FAIL;

		m_HierarchyNodes.push_back(pNode);

		Safe_AddRef(pNode);
	}

	return S_OK;
}

CAnimation * CAnimation::Bin_Create(DATA_HEROANIM* pAIAnimation)
{
	CAnimation*			pInstance = new CAnimation();

	if (FAILED(pInstance->Bin_Initialize_Prototype(pAIAnimation)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation * CAnimation::Bin_Clone(CModel* pModel)
{
	CAnimation*			pInstance = new CAnimation(*this);

	if (FAILED(pInstance->Bin_Initialize(pModel)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CAnimation * CAnimation::Bin_Clone(CModel_Instance * pModel)
{
	CAnimation*			pInstance = new CAnimation(*this);

	if (FAILED(pInstance->Bin_Initialize(pModel)))
	{
		MSG_BOX(TEXT("Failed To Created : CAnimation"));
		Safe_Release(pInstance);
	}

	return pInstance;
}


// For. 데이터 추출
void CAnimation::Get_AnimData(DATA_HEROANIM * pData)
{

	pData->fDuration = m_fDuration;
	pData->iNumChannels = m_iNumChannels;
	pData->fTickPerSecond = m_fTickPerSecond;
	pData->bLoop = m_bLoop;
	
	strcpy_s(pData->szName, m_szName);

	pData->pHeroChannel = new DATA_HEROCHANNEL[m_iNumChannels];

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		m_Channels[i]->Get_ChannelData(&pData->pHeroChannel[i]);
	}

	pData->iNumTimeLimit = (int)m_TimeLimit.size();
	if (0 < pData->iNumTimeLimit)
		pData->pTimeLimits = new float[pData->iNumTimeLimit];
	else
		pData->pTimeLimits = nullptr;

	for (int i = 0; i < m_TimeLimit.size(); ++i)
		pData->pTimeLimits[i] = m_TimeLimit[i];
}




void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();


	for (auto& pHierarchyNode : m_HierarchyNodes)
		Safe_Release(pHierarchyNode);

	m_HierarchyNodes.clear();
}
