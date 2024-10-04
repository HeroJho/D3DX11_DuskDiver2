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
	// 사운드 리소스 정보를 갖는 객체 
	std::map<TCHAR*, FMOD::Sound*> m_mapSound;
	
	enum { MAXCHANNEL = 32};
	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD::Channel* m_pChannelArr[MAXCHANNEL];
	  
	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD::System * m_pSystem;
	_bool		m_bPause = false;

public:
	// CBase을(를) 통해 상속됨
	virtual void Free() override;




public:
	void Tick(_float fTimeDelta);


public:
	//					  어떤 애를			어떤 ID로		  어떤 볼륨으로          true면 브금           어떤 보간 스피드로(0이면 기존과 동일)
	void PlaySound_Linear(TCHAR* pSoundKey, const _uint& eID, const float& fVolume, _bool bLoof = false, float fLinearSpeed = 0.f);


private:
	typedef struct tagSoundData
	{
		// 어떤 ID에   -> END면 보간할 사운가 없는것
		_uint eID = SD_END;
		// 어떤 사운드로
		TCHAR sSound[MAX_PATH];
		// 어떤 볼륨으로
		_float fVolume = 0.f;
		_float fPreVolume = 0.f;
		// 어떤 스피드로
		_float fLinearSpeed = 0.f;

		// 멤버 변수
		_float fPreVolumeAcc = 0.f;
		_float fVolumeAcc = 0.f;


	}SOUNDDATA;
 
	SOUNDDATA	m_LinearSounds[SD_END];


};

END