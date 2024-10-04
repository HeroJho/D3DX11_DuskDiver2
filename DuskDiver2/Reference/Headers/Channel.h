#pragma once

#include "Base.h"

BEGIN(Engine)
class CModel;
class CChannel final : public CBase
{
public:
	CChannel();
	virtual ~CChannel() = default;

public:
	const char* Get_Name() const {
		return m_szName;
	}

public:
	HRESULT Initialize(aiNodeAnim*	pAIChannel);
	_uint Update_Transformation(_float fPlayTime, _uint iCurrentKeyFrame, class CHierarchyNode* pNode);
	_uint Update_ReverseTransformation(_float fPlayTime, _uint iCurrentKeyFrame, class CHierarchyNode* pNode);
	_uint Interpolation(_float fTimeDelta, _uint iNextIndex, _float fPlayTime, _uint iCurrentKeyFrame, class CHierarchyNode* pNode, CModel* pAnimModel);

	int Get_KeyFrameSize() { return m_iNumKeyFrames; }
	KEYFRAME GetKeyFrame(int Index) { return m_KeyFrames[Index]; }

	KEYFRAME	GetFirstKeyFrame() { return m_KeyFrames[0]; }
private:
	char							m_szName[MAX_PATH] = "";

	_uint							m_iNumKeyFrames = 0;
	vector<KEYFRAME>				m_KeyFrames;	

	_uint							m_iCurrentKeyFrame = 0;

	_float m_fInterpolationTime = 0.f;

public:
	static CChannel* Create(aiNodeAnim*	pAIChannel);
	virtual void Free() override;





	// ============================
	//		For. BinModelData
	// ============================
public:
	static CChannel* Bin_Create(DATA_HEROCHANNEL*	pAIChannel, _uint& iMaxFrameIndex);
	HRESULT Bin_Initialize(DATA_HEROCHANNEL*	pAIChannel, _uint& iMaxFrameIndex);

public: // For. Data
	void Get_ChannelData(DATA_HEROCHANNEL* pData);



};

END