#include "stdafx.h"
#include "..\Public\Particle_Manager.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "ParticleInstance.h"

IMPLEMENT_SINGLETON(CParticle_Manager)

CParticle_Manager::CParticle_Manager()
{
}

void CParticle_Manager::Initalize()
{
	LoadAll();
	
}

void CParticle_Manager::LoadAll()
{
	m_ParticleCreateInfo.clear();
	m_ParticleInstanceCreateInfo.clear();
	LoadParticle("Player01");
	LoadParticle("Player03");
	LoadParticle("Player04");
	LoadParticle("KCH");
	LoadParticle("Obj");
	LoadParticle("Beast");
	LoadParticle("Arcade");
	LoadParticle("Golem");
	LoadParticleInstance("Player01");
	LoadParticleInstance("Player03");
	LoadParticleInstance("Player04");
	LoadParticleInstance("KCH");
	LoadParticleInstance("Obj");
	LoadParticleInstance("Beast");
	LoadParticleInstance("Arcade");
	LoadParticleInstance("Golem");
}

void CParticle_Manager::LoadParticle(char* LoadName)
{
	string FileLoad = LoadName;

	string temp = "../Data/ParticleData/";

	FileLoad = temp + FileLoad + ".dat";
	
	wchar_t FilePath[256] = { 0 };

	for (int i = 0; i < FileLoad.size(); i++)
	{
		FilePath[i] = FileLoad[i];
	}

	HANDLE		hFile = CreateFile(FilePath,			// ���� ��ο� �̸� ���
		GENERIC_READ,				// ���� ���� ��� (GENERIC_WRITE ���� ����, GENERIC_READ �б� ����)
		NULL,						// �������, ������ �����ִ� ���¿��� �ٸ� ���μ����� ������ �� ����� ���ΰ�, NULL�� ��� �������� �ʴ´�
		NULL,						// ���� �Ӽ�, �⺻��	
		OPEN_EXISTING,				// ���� ���, CREATE_ALWAYS�� ������ ���ٸ� ����, �ִٸ� ���� ����, OPEN_EXISTING ������ ���� ��쿡�� ����
		FILE_ATTRIBUTE_NORMAL,		// ���� �Ӽ�(�б� ����, ���� ��), FILE_ATTRIBUTE_NORMAL �ƹ��� �Ӽ��� ���� �Ϲ� ���� ����
		NULL);						// �������� ������ �Ӽ��� ������ ���ø� ����, �츮�� ������� �ʾƼ� NULL

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(g_hWnd, _T("Load File"), _T("Fail"), MB_OK);
		return;
	}

	DWORD		dwByte = 0;

	int ParticleSize;
	ReadFile(hFile, &ParticleSize, sizeof(int), &dwByte, nullptr);

	for (int i = 0; i < ParticleSize; ++i)
	{
		char Name[256];
		PARTICLECREATE ParticleCreateInfo;
		ReadFile(hFile, Name, sizeof(char) * 256, &dwByte, nullptr);
		ReadFile(hFile, &ParticleCreateInfo, sizeof(PARTICLECREATE), &dwByte, nullptr);

		

		_tchar* ParticleName = TM->Get_ManagedTChar();
		TM->CtoTC(Name, ParticleName);
		_tchar* TexPath = TM->Get_ManagedTChar();
		_tchar* TexName = TM->Get_ManagedTChar();
		ReadFile(hFile, TexPath, sizeof(_tchar) * 256, &dwByte, nullptr);
		ReadFile(hFile, TexName, sizeof(_tchar) * 256, &dwByte, nullptr);

		ParticleCreateInfo.TexPath = TexPath;
		ParticleCreateInfo.TexName = TexName;
		if (0 == dwByte)	// ���̻� ���� �����Ͱ� ���� ���
		{
			delete ParticleName;
			break;		
		}

		m_ParticleCreateInfo.emplace(ParticleName, ParticleCreateInfo);

	}


	// 3. ���� �Ҹ�
	CloseHandle(hFile);
}

void CParticle_Manager::LoadParticleInstance(char* LoadName)
{
	string FileLoad = LoadName;

	string temp = "../Data/ParticleInstanceData/";

	FileLoad = temp + FileLoad + ".dat";

	wchar_t FilePath[256] = { 0 };

	for (int i = 0; i < FileLoad.size(); i++)
	{
		FilePath[i] = FileLoad[i];
	}

	HANDLE		hFile = CreateFile(FilePath,			// ���� ��ο� �̸� ���
		GENERIC_READ,				// ���� ���� ��� (GENERIC_WRITE ���� ����, GENERIC_READ �б� ����)
		NULL,						// �������, ������ �����ִ� ���¿��� �ٸ� ���μ����� ������ �� ����� ���ΰ�, NULL�� ��� �������� �ʴ´�
		NULL,						// ���� �Ӽ�, �⺻��	
		OPEN_EXISTING,				// ���� ���, CREATE_ALWAYS�� ������ ���ٸ� ����, �ִٸ� ���� ����, OPEN_EXISTING ������ ���� ��쿡�� ����
		FILE_ATTRIBUTE_NORMAL,		// ���� �Ӽ�(�б� ����, ���� ��), FILE_ATTRIBUTE_NORMAL �ƹ��� �Ӽ��� ���� �Ϲ� ���� ����
		NULL);						// �������� ������ �Ӽ��� ������ ���ø� ����, �츮�� ������� �ʾƼ� NULL

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(g_hWnd, _T("Load File"), _T("Fail"), MB_OK);
		return;
	}

	DWORD		dwByte = 0;

	int ParticleSize;
	ReadFile(hFile, &ParticleSize, sizeof(int), &dwByte, nullptr);

	for (int i = 0; i < ParticleSize; ++i)
	{
		char Name[256];
		PARTICLEINSTANCEINFO ParticleCreateInfo;
		ReadFile(hFile, Name, sizeof(char) * 256, &dwByte, nullptr);
		ReadFile(hFile, &ParticleCreateInfo, sizeof(PARTICLEINSTANCEINFO), &dwByte, nullptr);

		_tchar* ParticleName = TM->Get_ManagedTChar();
		TM->CtoTC(Name, ParticleName);
		_tchar* TexPath = TM->Get_ManagedTChar();
		_tchar* TexName = TM->Get_ManagedTChar();
		ReadFile(hFile, TexPath, sizeof(_tchar) * 256, &dwByte, nullptr);
		ReadFile(hFile, TexName, sizeof(_tchar) * 256, &dwByte, nullptr);

		ParticleCreateInfo.TexPath = TexPath;
		ParticleCreateInfo.TexName = TexName;
		if (0 == dwByte)	// ���̻� ���� �����Ͱ� ���� ���
		{
			delete ParticleName;
			break;
		}

		m_ParticleInstanceCreateInfo.emplace(ParticleName, ParticleCreateInfo);

	}


	// 3. ���� �Ҹ�
	CloseHandle(hFile);
}


CParticle_Manager::PARTICLECREATE CParticle_Manager::SearchParticle(_tchar * ParticleName)
{
	for (auto& iter : m_ParticleCreateInfo)
	{
		if (!lstrcmp(iter.first, ParticleName))
			return iter.second;
	}
	MSG_BOX(ParticleName);
	PARTICLECREATE Fail;
	Fail.TexName = L"Null";
	return Fail;
}

CParticle_Manager::PARTICLEINSTANCEINFO CParticle_Manager::SearchParticleInstance(_tchar * ParticleName)
{
	for (auto& iter : m_ParticleInstanceCreateInfo)
	{
		if (!lstrcmp(iter.first, ParticleName))
			return iter.second;
	}
	MSG_BOX(ParticleName);
	PARTICLEINSTANCEINFO Fail;
	Fail.TexName = L"Null";
	return Fail;
}

HRESULT CParticle_Manager::Create_AlphaParticle(_tchar * ParticleName, _float4x4 WorldMatrix)
{
	PARTICLECREATE ParticleCreateInfo = SearchParticle(ParticleName);
	for (int i = 0; i < ParticleCreateInfo.iParticleNum; ++i)
	{
		m_vPos.x = GI->Get_FloatRandom(ParticleCreateInfo.vMinPos.x, ParticleCreateInfo.vMaxPos.x);
		m_vPos.y = GI->Get_FloatRandom(ParticleCreateInfo.vMinPos.y, ParticleCreateInfo.vMaxPos.y);
		m_vPos.z = GI->Get_FloatRandom(ParticleCreateInfo.vMinPos.z, ParticleCreateInfo.vMaxPos.z);
		m_vSize.x = GI->Get_FloatRandom(ParticleCreateInfo.vMinSize.x, ParticleCreateInfo.vMaxSize.x);
		m_vSize.y = GI->Get_FloatRandom(ParticleCreateInfo.vMinSize.y, ParticleCreateInfo.vMaxSize.y);
		m_fSpeed = GI->Get_FloatRandom(ParticleCreateInfo.fMinSpeed, ParticleCreateInfo.fMaxSpeed);
		m_fLifeTime = GI->Get_FloatRandom(ParticleCreateInfo.fMinLifeTime, ParticleCreateInfo.fMaxLifeTime);

		CAlphaParticle::PARTICLEINFO* ParticleInfo = new CAlphaParticle::PARTICLEINFO;
		ParticleInfo->vSize = m_vSize;
		ParticleInfo->fSpeed = m_fSpeed;
		ParticleInfo->fLifeTime = m_fLifeTime;
		ParticleInfo->fGravity = ParticleCreateInfo.fGravity;
		ParticleInfo->vDirection = ParticleCreateInfo.vDir;
		ParticleInfo->TexPath = ParticleCreateInfo.TexPath;
		ParticleInfo->TexName = ParticleCreateInfo.TexName;
		ParticleInfo->iShaderPass = ParticleCreateInfo.iShaderPass;
		ParticleInfo->bUV = ParticleCreateInfo.bUV;
		ParticleInfo->bUVLoof = ParticleCreateInfo.bUVLoof;
		ParticleInfo->fMaxUVIndexX = ParticleCreateInfo.fUVMaxIndexX;
		ParticleInfo->fMaxUVIndexY = ParticleCreateInfo.fUVMaxIndexY;
		ParticleInfo->fUVSpeed = ParticleCreateInfo.fUVSpeed;
		ParticleInfo->vScaleSpeed = ParticleCreateInfo.vScaleSpeed;
		ParticleInfo->bBlend = ParticleCreateInfo.bBlend;
		ParticleInfo->vUVFixSpeed = ParticleCreateInfo.vUVFixSpeed;
		ParticleInfo->bFixColor = ParticleCreateInfo.bFixColor;
		ParticleInfo->fBlurPower = ParticleCreateInfo.fBlurPower;
		ParticleInfo->vUVFixSpeed = ParticleCreateInfo.vUVFixSpeed;
		ParticleInfo->vColor = ParticleCreateInfo.vColor1;
		ParticleInfo->vColor2 = ParticleCreateInfo.vColor2;
		ParticleInfo->vRatio = ParticleCreateInfo.vRatio;
		ParticleInfo->vRatioSpeed = ParticleCreateInfo.vRatioSpeed;
		ParticleInfo->bUVFix = ParticleCreateInfo.bUVFix;
		ParticleInfo->fRotationSpeed = ParticleCreateInfo.fRotationSpeed;
		ParticleInfo->fRotationRatio = ParticleCreateInfo.fRotationRatio;
		ParticleInfo->fRotationTime = ParticleCreateInfo.fRotationTime;
		ParticleInfo->bFollow = ParticleCreateInfo.bFollow;
		ParticleInfo->vLocalPos = ParticleCreateInfo.vWorldPos;
		ParticleInfo->vRandomPos = m_vPos;
		ParticleInfo->WorldMatrix = WorldMatrix;

		if (FAILED(GI->Add_GameObjectToLayer(L"AlphaParticle", GI->Get_Cur_LevelIndex(), L"Layer_Particle", &ParticleInfo)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CParticle_Manager::Create_InstanceParticle(_tchar * ParticleName, _float4x4 WorldMatrix)
{
	PARTICLEINSTANCEINFO ParticleInstanceCreateInfo = SearchParticleInstance(ParticleName);

	CParticleInstance::PARTICLEINSTANCEINFO* Info = new CParticleInstance::PARTICLEINSTANCEINFO;

	Info->bBlend = ParticleInstanceCreateInfo.bBlend;
	Info->bUV = ParticleInstanceCreateInfo.bUV;
	Info->bUVLoof = ParticleInstanceCreateInfo.bUVLoof;
	Info->fAlphaTest = ParticleInstanceCreateInfo.fAlphaTest;
	Info->fBlurPower = ParticleInstanceCreateInfo.fBlurPower;
	Info->fGravity = ParticleInstanceCreateInfo.fGravity;
	Info->fMaxLifeTime = ParticleInstanceCreateInfo.fMaxLifeTime;
	Info->fMaxSpeed = ParticleInstanceCreateInfo.fMaxSpeed;
	Info->fMinLifeTime = ParticleInstanceCreateInfo.fMinLifeTime;
	Info->fMinSpeed = ParticleInstanceCreateInfo.fMinSpeed;
	Info->fUVMaxIndexX = ParticleInstanceCreateInfo.fUVMaxIndexX;
	Info->fUVMaxIndexY = ParticleInstanceCreateInfo.fUVMaxIndexY;
	Info->fUVSpeed = ParticleInstanceCreateInfo.fUVSpeed;
	Info->iParticleNum = ParticleInstanceCreateInfo.iParticleNum;
	Info->iShaderPass = ParticleInstanceCreateInfo.iShaderPass;
	Info->TexName = ParticleInstanceCreateInfo.TexName;
	Info->TexPath = ParticleInstanceCreateInfo.TexPath;
	Info->vLocalPos = ParticleInstanceCreateInfo.vWorldPos;
	Info->WorldMatrix = WorldMatrix;
	Info->vDirAngle = ParticleInstanceCreateInfo.vDirAngle;
	Info->vMaxPos = ParticleInstanceCreateInfo.vMaxPos;
	Info->vMaxSize = ParticleInstanceCreateInfo.vMaxSize;
	Info->vMinPos = ParticleInstanceCreateInfo.vMinPos;
	Info->vMinSize = ParticleInstanceCreateInfo.vMinSize;
	Info->vRangeAngle1 = ParticleInstanceCreateInfo.vRangeAngle1;
	Info->vRangeAngle2 = ParticleInstanceCreateInfo.vRangeAngle2;
	Info->vReleaseSpeed = ParticleInstanceCreateInfo.vReleaseSpeed;
	Info->bFixColor = ParticleInstanceCreateInfo.bFixColor;
	Info->vColor = ParticleInstanceCreateInfo.vColor;
	Info->vColor2 = ParticleInstanceCreateInfo.vColor2;
	Info->vRatio = ParticleInstanceCreateInfo.vRatio;
	Info->vRatioSpeed = ParticleInstanceCreateInfo.vRatioSpeed;
	Info->bCharge = ParticleInstanceCreateInfo.bCharge;
	Info->vChargePos = ParticleInstanceCreateInfo.vChargePos;
	Info->fChargeRange = ParticleInstanceCreateInfo.fChargeRange;
	Info->bDistortion = ParticleInstanceCreateInfo.bDistortion;
	Info->fDistortionPower = ParticleInstanceCreateInfo.fDistortionPower;
	Info->bFollow = ParticleInstanceCreateInfo.bFollow;

	if (FAILED(GI->Add_GameObjectToLayer(L"ParticleInstance", GI->Get_Cur_LevelIndex(), L"Layer_Particle", &Info)))
		return E_FAIL;

	return S_OK;
}


void CParticle_Manager::Free()
{
	m_ParticleCreateInfo.clear();
	m_ParticleInstanceCreateInfo.clear();
}


