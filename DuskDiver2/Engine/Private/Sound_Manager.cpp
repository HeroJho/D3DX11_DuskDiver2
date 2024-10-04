
#include "Sound_Manager.h"

IMPLEMENT_SINGLETON(CSound_Manager)

CSound_Manager::CSound_Manager()
	: CBase()
	, m_pSystem(nullptr)
{
	ZeroMemory(m_pChannelArr, sizeof(m_pChannelArr));
}

HRESULT CSound_Manager::Initialize()
{
	// ���带 ����ϴ� ��ǥ��ü�� �����ϴ� �Լ�
	//FMOD_System_Create(&m_pSystem);
	FMOD::System_Create(&m_pSystem);

	// 1. �ý��� ������, 2. ����� ����ä�� �� , �ʱ�ȭ ���) 
	//FMOD_System_Init(m_pSystem, MAXCHANNEL, FMOD_INIT_NORMAL, NULL);
	m_pSystem->init(32, FMOD_INIT_NORMAL, nullptr);

	if (m_pSystem == nullptr)
		return E_FAIL;

	LoadSoundFile("../../Client/Bin/Resources/Sounds/BGM/");
	LoadSoundFile("../../Client/Bin/Resources/Sounds/HS/");
	LoadSoundFile("../../Client/Bin/Resources/Sounds/Player/Hero01/");
	LoadSoundFile("../../Client/Bin/Resources/Sounds/Player/Hero04/");
	LoadSoundFile("../../Client/Bin/Resources/Sounds/Player/Hero05/");
	LoadSoundFile("../../Client/Bin/Resources/Sounds/UI/");
	LoadSoundFile("../../Client/Bin/Resources/Sounds/Map/");
	LoadSoundFile("../../Client/Bin/Resources/Sounds/Mini/");
	LoadSoundFile("../../Client/Bin/Resources/Sounds/Monster/TrainingRobot/");
	LoadSoundFile("../../Client/Bin/Resources/Sounds/Monster/DisasterBeta/");
	LoadSoundFile("../../Client/Bin/Resources/Sounds/Monster/DisasterFusion/");
	LoadSoundFile("../../Client/Bin/Resources/Sounds/Monster/DefenceRobot/");
	LoadSoundFile("../../Client/Bin/Resources/Sounds/Monster/BombRobot/");
	LoadSoundFile("../../Client/Bin/Resources/Sounds/Monster/OldBombRobot/");
	LoadSoundFile("../../Client/Bin/Resources/Sounds/Monster/DisasterBoss/");
	LoadSoundFile("../../Client/Bin/Resources/Sounds/Monster/Golem/");
	LoadSoundFile("../../Client/Bin/Resources/Sounds/Monster/MechanicalOctopus/");
	LoadSoundFile("../../Client/Bin/Resources/Sounds/Monster/Beast/");


	return S_OK;
}

void CSound_Manager::PlaySound(TCHAR * pSoundKey, const _uint& eID, const float& fVolume)
{
	std::map<TCHAR*, FMOD::Sound*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = std::find_if(m_mapSound.begin(), m_mapSound.end(), 
		[&](auto& iter)->bool 
	{
		return !lstrcmp(pSoundKey, iter.first);
	});
	
	if (iter == m_mapSound.end())
		return;

	FMOD_BOOL bPlay = FALSE; 
	
	m_pSystem->playSound(iter->second, 0, false, &m_pChannelArr[eID]);
	
	//if (FMOD_Channel_IsPlaying(m_pChannelArr[eID], &bPlay))
	//{
	//	FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, FALSE, &m_pChannelArr[eID]);
	//}

	m_pChannelArr[eID]->setVolume(fVolume);
	//FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	m_pSystem->update();
	//FMOD_System_Update(m_pSystem);
}

void CSound_Manager::PlayBGM(TCHAR * pSoundKey, const float& fVolume)
{
	std::map<TCHAR*, FMOD::Sound*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = std::find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
	{
		return !lstrcmp(pSoundKey, iter.first);
	});
	
	if (iter == m_mapSound.end())
		return;

	m_pSystem->playSound(iter->second, 0, false, &m_pChannelArr[0]);

	//FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_FREE, iter->second, FALSE, &m_pChannelArr[0]);
	
	m_pChannelArr[0]->setMode(FMOD_LOOP_NORMAL);
	m_pChannelArr[0]->setVolume(fVolume);
	//FMOD_Channel_SetMode(m_pChannelArr[0], FMOD_LOOP_NORMAL);
	
	//FMOD_Channel_SetVolume(m_pChannelArr[0], fVolume);
	m_pSystem->update();
	//FMOD_System_Update(m_pSystem);
}


void CSound_Manager::StopSound(const _uint& eID)
{
	m_pChannelArr[eID]->stop();
	//FMOD_Channel_Stop(m_pChannelArr[eID]);
}

void CSound_Manager::StopAll()
{
	for (int i = 0; i < SOUND_MAX; ++i)
	{
		m_pChannelArr[i]->stop();
		//FMOD_Channel_Stop(m_pChannelArr[i]);
	}
}

void CSound_Manager::SetChannelVolume(const _uint& eID, const float& fVolume)
{
	m_pChannelArr[eID]->setVolume(fVolume);
	//FMOD_Channel_SetVolume(m_pChannelArr[eID], fVolume);

	//FMOD_System_Update(m_pSystem);
	m_pSystem->update();
}

int CSound_Manager::VolumeUp(const _uint & eID, const _float & _vol)
{
	if (m_volume < SOUND_MAX) {
		m_volume = _vol;
	}

	m_pChannelArr[eID]->setVolume(m_volume);
	//FMOD_Channel_SetVolume(m_pChannelArr[eID], m_volume);

	return 0;
}

int CSound_Manager::VolumeDown(const _uint & eID, const _float & _vol)
{
	if (m_volume > SOUND_MIN) {
		m_volume = _vol;
	}

	m_pChannelArr[eID]->setVolume(m_volume);
	//FMOD_Channel_SetVolume(m_pChannelArr[eID], m_volume);

	return 0;
}

int CSound_Manager::Pause(const _uint & eID)
{
	m_bPause = !m_pChannelArr[eID]->getPaused(&m_bPause);
	m_pChannelArr[eID]->setPaused(m_bPause);

	//FMOD_Channel_SetPaused(m_pChannelArr[eID], m_bPause);

	return 0;
}

void CSound_Manager::LoadSoundFile(const char* FilePath)
{
	// _finddata_t : <io.h>���� �����ϸ� ���� ������ �����ϴ� ����ü
	_finddata_t fd; 
	string Path = FilePath;
	string Exe = "*.*";
	string FullPath = Path + Exe;
	// _findfirst : <io.h>���� �����ϸ� ����ڰ� ������ ��� ������ ���� ù ��° ������ ã�� �Լ�
	//intptr_t handle = _findfirst("../../Client/Bin/Resources/Sounds/*.*", &fd);
	intptr_t handle = _findfirst(FullPath.c_str(), &fd);
	if (handle == -1)
		return; 

	int iResult = 0; 

	//char szCurPath[128] = "../../Client/Bin/Resources/Sounds/";
	char szCurPath[128];
	strcpy_s(szCurPath, sizeof(char) * 128, Path.c_str());
	char szFullPath[128] = ""; 

	while (iResult != -1)
	{
		// ../Sound/"
		strcpy_s(szFullPath, szCurPath); 

		// "../ Sound/Success.wav"
		strcat_s(szFullPath, fd.name);

		//FMOD_SOUND* pSound = nullptr; 
		FMOD::Sound* pSound = nullptr;

		FMOD_RESULT eRes = m_pSystem->createSound(szFullPath, FMOD_LOOP_OFF, 0, &pSound);
		//FMOD_RESULT eRes = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_HARDWARE, 0, &pSound);

		if (eRes == FMOD_OK)
		{
			size_t iLength = strlen(fd.name) + 1; 

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

			// �ƽ�Ű �ڵ� ���ڿ��� �����ڵ� ���ڿ��� ��ȯ�����ִ� �Լ�
			MultiByteToWideChar(CP_ACP, 0, fd.name, (int)iLength, pSoundKey, (int)iLength);

			m_mapSound.emplace(pSoundKey, pSound);
		}
		//_findnext : <io.h>���� �����ϸ� ���� ��ġ�� ������ ã�� �Լ�, ���̻� ���ٸ� -1�� ����
		iResult = _findnext(handle, &fd);
	}

	m_pSystem->update();
	//FMOD_System_Update(m_pSystem);

	_findclose(handle);
}

void CSound_Manager::Free()
{
	for (auto& Mypair : m_mapSound)
	{
		delete[] Mypair.first;
		Mypair.second->release();
		//FMOD_Sound_Release(Mypair.second);
	}
	m_mapSound.clear();

	m_pSystem->release();
	m_pSystem->close();
	//FMOD_System_Release(m_pSystem);
	//FMOD_System_Close(m_pSystem);
}


void CSound_Manager::Tick(_float fTimeDelta)
{
	// ��� ���� ������ �ֳ� ����
	// m_pSystem->getChannelsPlaying(m_iChanners);
	
	for (_int i = 0; i < SD_END; ++i)
	{
		// ����� �� ���ٸ� ����
		if (SD_END == m_LinearSounds[i].eID)
			continue;

		// �� Pre�� 0�� �ƴ϶�� Pre�� ������ ��´�
		if (0.f < m_LinearSounds[i].fPreVolumeAcc)
		{
			m_LinearSounds[i].fPreVolumeAcc -= m_LinearSounds[i].fLinearSpeed * fTimeDelta;
			_float fVolume = 0.f;
			VolumeDown((_uint)i, m_LinearSounds[i].fPreVolume * m_LinearSounds[i].fPreVolumeAcc);
		}
		else if (0.f == m_LinearSounds[i].fVolumeAcc)
		{
			// ������ ����
			m_LinearSounds[i].fPreVolumeAcc = 0.f;
			VolumeDown((_uint)i, 0.f);
			StopSound((_uint)i);

			// ���ο�� ����
			if (0 == i)
				PlayBGM(m_LinearSounds[i].sSound, 0.f);
			else
				PlaySound(m_LinearSounds[i].sSound, i, 0.f);
		}


		// Pre�� 0�̶�� Cur�� ������ �ø���.
		if (0.f == m_LinearSounds[i].fPreVolumeAcc && 1.f != m_LinearSounds[i].fVolumeAcc)
		{

			m_LinearSounds[i].fVolumeAcc += m_LinearSounds[i].fLinearSpeed * fTimeDelta;
			
			if (1.f < m_LinearSounds[i].fVolumeAcc)
			{
				m_LinearSounds[i].fVolumeAcc = 1.f;
				VolumeUp((_uint)i, m_LinearSounds[i].fVolume);
			}
			else 
			{
				VolumeUp((_uint)i, m_LinearSounds[i].fVolume * m_LinearSounds[i].fVolumeAcc);
			}

		}

	}
}

void CSound_Manager::PlaySound_Linear(TCHAR * pSoundKey, const _uint & eID, const float & fVolume, _bool bLoof, float fLinearSpeed)
{
	if (0.f == fLinearSpeed)
	{
		m_LinearSounds[eID].eID = eID;
		lstrcpy(m_LinearSounds[eID].sSound, pSoundKey);
		m_LinearSounds[eID].fVolume = fVolume;
		m_LinearSounds[eID].fLinearSpeed = fLinearSpeed;
		m_LinearSounds[eID].fPreVolumeAcc = 0.f;
		m_LinearSounds[eID].fVolumeAcc = 1.f;

		if (!bLoof)
			PlaySound(pSoundKey, eID, fVolume);
		else
			PlayBGM(pSoundKey, fVolume);
		return;
	}


	m_LinearSounds[eID].eID = eID;
	lstrcpy(m_LinearSounds[eID].sSound, pSoundKey);
	m_LinearSounds[eID].fPreVolume = m_LinearSounds[eID].fVolume;
	m_LinearSounds[eID].fVolume = fVolume;	
	m_LinearSounds[eID].fLinearSpeed = fLinearSpeed;


	// ������ ������̸� ���� 1
	_bool bIsPlaying = false;
	m_pChannelArr[eID]->isPlaying(&bIsPlaying);
	if (bIsPlaying)
	{
		m_LinearSounds[eID].fPreVolumeAcc = 1.f;
		m_LinearSounds[eID].fVolumeAcc = 0.f;
	}
	else
	{
		m_LinearSounds[eID].fPreVolumeAcc = 0.f;
		m_LinearSounds[eID].fVolumeAcc = 0.f;
	}

	// PlaySound(pSoundKey, eID, fVolume);

}
