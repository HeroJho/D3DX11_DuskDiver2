#pragma once

#include "Base.h"
#include "fmod.h"
#include "fmod.hpp"
#include "fmod_errors.h"

//using namespace ;

#define SOUND_MAX 1.0f
#define SOUND_MIN 0.0f
#define SOUND_DEFAULT 0.5f
#define SOUND_WEIGHT 0.1f

BEGIN(Engine)

class CSound_Manager : public CBase
{
	DECLARE_SINGLETON(CSound_Manager)

private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT Initialize();


public:
	void PlaySound(TCHAR* pSoundKey, const _uint& eID, const float& fVolume);
	void PlayBGM(TCHAR * pSoundKey, const float& fVolume);
	void StopSound(const _uint& eID);
	void StopAll();

	void SetChannelVolume(const _uint& eID, const float& fVolume);

	int  VolumeUp(const _uint& eID, const _float& _vol);
	int  VolumeDown(const _uint& eID, const _float& _vol);

	int  Pause(const _uint& eID);


private:
	void LoadSoundFile(const char* FilePath); 

private:
	float m_volume = SOUND_DEFAULT;
	float m_BGMvolume = SOUND_DEFAULT;
	FMOD_BOOL m_bool;

private:
	// ���� ���ҽ� ������ ���� ��ü 
	std::map<TCHAR*, FMOD::Sound*> m_mapSound;
	
	enum { MAXCHANNEL = 32};
	// FMOD_CHANNEL : ����ϰ� �ִ� ���带 ������ ��ü 
	FMOD::Channel* m_pChannelArr[MAXCHANNEL];
	  
	// ���� ,ä�� ��ü �� ��ġ�� �����ϴ� ��ü 
	FMOD::System * m_pSystem;
	_bool		m_bPause = false;

public:
	// CBase��(��) ���� ��ӵ�
	virtual void Free() override;




public:
	void Tick(_float fTimeDelta);


public:
	//					  � �ָ�			� ID��		  � ��������          true�� ���           � ���� ���ǵ��(0�̸� ������ ����)
	void PlaySound_Linear(TCHAR* pSoundKey, const _uint& eID, const float& fVolume, _bool bLoof = false, float fLinearSpeed = 0.f);


private:
	typedef struct tagSoundData
	{
		// � ID��   -> END�� ������ �� ���°�
		_uint eID = SD_END;
		// � �����
		TCHAR sSound[MAX_PATH];
		// � ��������
		_float fVolume = 0.f;
		_float fPreVolume = 0.f;
		// � ���ǵ��
		_float fLinearSpeed = 0.f;

		// ��� ����
		_float fPreVolumeAcc = 0.f;
		_float fVolumeAcc = 0.f;


	}SOUNDDATA;
 
	SOUNDDATA	m_LinearSounds[SD_END];


};

END