#pragma once

#include "Base.h"

BEGIN(Engine)
class CModel;
class CModel_Instance;
class CChannel;

class CAnimation final : public CBase
{
private:
	CAnimation();
	CAnimation(const CAnimation& rhs);
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize_Prototype(aiAnimation* pAIAnimation);
	HRESULT Initialize(class CModel* pModel);
	_bool Play_Animation(_float fTimeDelta, CModel* pAnimModel);
	_bool Play_Animation(_float fTimeDelta, CModel_Instance* pAnimModel);


	_bool Play_AniReverse(_float fTimeDelta, CModel* pAnimModel);
	void ResetChennel();

	const char* Get_Name() const {
		return m_szName;
	}

	void Set_Name(const char* AnimName)
	{
		strcpy_s(m_szName, sizeof(char) * 256, AnimName);
	}

	_float Get_TickPerSecond() { return m_fTickPerSecond; }
	void Set_TickPerSecond(_float fSecond) { m_fTickPerSecond = fSecond; }

	_float Get_Duration() { return m_fDuration; }
	void Set_Duration(_float fDuration) { m_fDuration = fDuration; }

	_float GetPlayTime() { return m_fPlayTime; }
	void SetPlayTime(_float fPlayTime) { m_fPlayTime = fPlayTime; }

	_uint Get_KeyFrame() { return m_ChannelKeyFrames[m_iMaxKeyFrameIndex]; }
	void Set_KeyFrame(_float iKeyFrame);

	vector<_float>& GetTimeLimit() { return m_TimeLimit; }
	void SetTimeLimit(vector<_float>& TimeLimit);
	_float GetTimeLimit(int iIndex) { return m_TimeLimit[iIndex]; }
	void SetTimeLimit(_float fTimeLimit, int iIndex) { m_TimeLimit[iIndex] = fTimeLimit; }

	_int Get_ChannelSize() { return (_int)m_Channels.size(); }
	CChannel* GetChannel(int Index) { return m_Channels[Index]; }
	CChannel* GetChannels(char* sName);

private:
	/* 이 애니메이션을 구동하기위해 사용되는 뼈의 갯수. */
	_uint						m_iNumChannels = 0;
	vector<class CChannel*>		m_Channels;

	/* 애니메이션 재생하는데 걸리는 전체시간. */
	_float						m_fDuration = 0.f;

	/* 애니메이션의 초당 재생 속도. */
	_float						m_fTickPerSecond = 0.f;

	_float						m_fPlayTime = 0.f;
	_uint						m_iMaxKeyFrameIndex = 0;
	
private: /* 복제된 애니메이션 마다 따로 가진다. */
	vector<_float> m_TimeLimit;
	_uint		m_iChannelIndex = 0;
	_bool m_bAnimEnd = false;
	vector<class CHierarchyNode*>	m_HierarchyNodes;
	vector<_uint>					m_ChannelKeyFrames;
	vector<_uint>					m_ChannelOldKeyFrames;

public:
	static CAnimation* Create(aiAnimation* pAIAnimation);
	CAnimation* Clone(class CModel* pModel);
	virtual void Free() override;


	// ============================
	//		For. BinModelData
	// ============================
public:
	HRESULT Bin_Initialize_Prototype(DATA_HEROANIM* pAIAnimation);
	HRESULT Bin_Initialize(class CModel* pModel);
	HRESULT Bin_Initialize(class CModel_Instance* pModel);

	static CAnimation* Bin_Create(DATA_HEROANIM* pAIAnimation);
	CAnimation* Bin_Clone(class CModel* pModel);
	CAnimation* Bin_Clone(class CModel_Instance* pModel);

public: // For. Data
	void Get_AnimData(DATA_HEROANIM* pData);


private:
	char						m_szName[MAX_PATH];
	_bool						m_bLoop = false;


};

END