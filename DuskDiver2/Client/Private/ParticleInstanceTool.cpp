#include "stdafx.h"
#include "..\Public\ParticleInstanceTool.h"
#include "ParticleInstance.h"
#include "ToolManager.h"
#include "DataManager.h"
#include "GameInstance.h"

CParticleInstanceTool::CParticleInstanceTool(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(pDevice);
	Safe_AddRef(pContext);
}

HRESULT CParticleInstanceTool::Initialize()
{
	m_TabName.push_back("Particle");
	m_TabName.push_back("Texture");
	m_ParticleTabName.push_back("SetParticle");
	m_ParticleTabName.push_back("FixParticle");
	
	LoadTexture();
	LoadParticle("Golem");

	m_ParticleName = "Test";
	m_TextureName = "Null";

	m_NowParticles = "ParticleList";
	m_NowTextures = "TextureList";
	return S_OK;
}

void CParticleInstanceTool::ShowParticleToolWindow(_float fTimeDelta)
{
	static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyResizeDown;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("SetParticle"))
		{
			m_bSetParticle = true;
			m_bFixParticle = false;
			SetParticle(fTimeDelta);

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("FixParticle"))
		{

			m_bSetParticle = false;
			m_bFixParticle = true;
			FixParticle();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
	
}

void CParticleInstanceTool::ShowInfoWindow(_float fTimeDelta)
{
	static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyResizeDown;
	ImGui::Begin("ParticleInfo");
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("Particle"))
		{
			ShowParticleInfoWindow(fTimeDelta);
			
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Texture"))
		{

			ShowTextureInfoWindow(fTimeDelta);
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
}

void CParticleInstanceTool::ShowParticleInfoWindow(_float fTimeDelta)
{

	                    
	ImGui::SetWindowSize(ImVec2{ 500.f, 500.f });

	ImGui::TextColored(ImVec4{ 0.f,1.f,0.f,1.f }, m_NowParticles.c_str());

	if (ImGui::BeginListBox("list", ImVec2(400, 300)))
	{

		for (int i = 0; i < m_ParticleList.size(); i++)
		{
			const bool is_selected = (m_iSelectedParticle == i);
			if (ImGui::Selectable(m_ParticleList[i], is_selected))
				m_iSelectedParticle = i;

			if (is_selected)
				ImGui::SetItemDefaultFocus();

		}
		ImGui::EndListBox();
	}

	ImGui::PushItemWidth(215.f);

	ImGui::Text("");
	static char ParticleName[256] = "";
	ImGui::InputText("ParticleName", ParticleName, IM_ARRAYSIZE(ParticleName));
	ImGui::PopItemWidth();

	if (ImGui::Button("Add Particle"))
	{
		if(!strcmp(ParticleName, ""))
			MSG_BOX(TEXT("Please Push Name"));
		else if (!m_bSetParticle)
			MSG_BOX(TEXT("Please Change SetParticleMode"));
		else
			CreateParticle(ParticleName);
	}

	ImGui::SameLine();
	if (ImGui::Button("Delete Particle"))
	{
		if (m_ParticleList.size() > 0)
			DeleteParticle();
	}

	ImGui::SameLine();
	if (ImGui::Button("Change Name"))
	{
		if (m_ParticleList.size() > 0)
			ChangeName(ParticleName);
	}

	ImGui::PushItemWidth(215.f);
	ImGui::Text("");
	static char FileName[256] = "";
	ImGui::InputText("FileName", FileName, IM_ARRAYSIZE(FileName));
	ImGui::PopItemWidth();

	if (ImGui::Button("SaveParticle"))
	{
		if (!strcmp(FileName, ""))
			MSG_BOX(L"Please Write FileName");
		else
			SaveParticle(FileName);
	}

	ImGui::SameLine();

	if (ImGui::Button("Load Particle"))
	{
		if (!strcmp(FileName, ""))
			MSG_BOX(L"Please Write FileName");
		else
			LoadParticle(FileName);
	}


	
}

void CParticleInstanceTool::CreateParticle(char* ParticleName)
{
	for (auto& iter : m_ParticleList)
	{
		if (!strcmp(iter, ParticleName))
		{
			MSG_BOX(TEXT("Already Use Name"));
			return;
		}
	}

	SetParticleInfo(ParticleName);

}

void CParticleInstanceTool::DeleteParticle()
{

	m_ParticleCreateInfo.erase(m_ParticleCreateInfo.begin() + m_iSelectedParticle);
	m_ParticleList.erase(m_ParticleList.begin() + m_iSelectedParticle);
	

	if (m_ParticleList.size() == 1)
		m_iSelectedParticle = 0;
	
	else if(m_ParticleList.size() == m_iSelectedParticle)
		m_iSelectedParticle -= 1;
}

void CParticleInstanceTool::SaveParticle(char* SaveName)
{
	if (m_ParticleList.size() < 1)
	{
		MSG_BOX(TEXT("Don't have Data"));
		return;
	}

	string FileSave = SaveName;

	string temp = "../Data/ParticleInstanceData/";

	FileSave = temp + FileSave + ".dat";

	wchar_t FilePath[256] = { 0 };

	for (int i = 0; i < FileSave.size(); i++)
	{
		FilePath[i] = FileSave[i];
	}

	HANDLE		hFile = CreateFile(FilePath,			// 파일 경로와 이름 명시
		GENERIC_WRITE,				// 파일 접근 모드 (GENERIC_WRITE 쓰기 전용, GENERIC_READ 읽기 전용)
		NULL,						// 공유방식, 파일이 열려있는 상태에서 다른 프로세스가 오픈할 때 허용할 것인가, NULL인 경우 공유하지 않는다
		NULL,						// 보안 속성, 기본값	
		CREATE_ALWAYS,				// 생성 방식, CREATE_ALWAYS는 파일이 없다면 생성, 있다면 덮어 쓰기, OPEN_EXISTING 파일이 있을 경우에면 열기
		FILE_ATTRIBUTE_NORMAL,		// 파일 속성(읽기 전용, 숨기 등), FILE_ATTRIBUTE_NORMAL 아무런 속성이 없는 일반 파일 생성
		NULL);						// 생성도리 파일의 속성을 제공할 템플릿 파일, 우리는 사용하지 않아서 NULL

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(g_hWnd, _T("Save File"), _T("Fail"), MB_OK);
		return;
	}

	// 2. 파일 쓰기

	DWORD		dwByte = 0;

	_int ParticleSize = (_int)m_ParticleList.size();
	WriteFile(hFile, &ParticleSize, sizeof(int), &dwByte, nullptr);

	for (int i = 0; i < ParticleSize; ++i)
	{
		char ParticleName[256] = "";
		strcpy_s(ParticleName, sizeof(char) * 256, m_ParticleList[i]);
		WriteFile(hFile, &ParticleName, sizeof(char) * 256, &dwByte, nullptr);
		WriteFile(hFile, &m_ParticleCreateInfo[i], sizeof(PARTICLECREATE2), &dwByte, nullptr);

		_tchar TexPath[256] = L"";
		lstrcpy(TexPath, m_ParticleCreateInfo[i].TexPath); 
		WriteFile(hFile, &TexPath, sizeof(_tchar) * 256, &dwByte, nullptr);

		_tchar TexName[256] = L"";
		lstrcpy(TexName, m_ParticleCreateInfo[i].TexName);
		WriteFile(hFile, &TexName, sizeof(_tchar) * 256, &dwByte, nullptr);
	}

	MSG_BOX(TEXT("Succeed Save"));

	// 3. 파일 소멸
	CloseHandle(hFile);
}

void CParticleInstanceTool::LoadParticle(char* LoadName)
{
	string FileLoad = LoadName;

	string temp = "../Data/ParticleInstanceData/";

	FileLoad = temp + FileLoad + ".dat";

	wchar_t FilePath[256] = { 0 };

	for (int i = 0; i < FileLoad.size(); i++)
	{
		FilePath[i] = FileLoad[i];
	}

	HANDLE		hFile = CreateFile(FilePath,			// 파일 경로와 이름 명시
		GENERIC_READ,				// 파일 접근 모드 (GENERIC_WRITE 쓰기 전용, GENERIC_READ 읽기 전용)
		NULL,						// 공유방식, 파일이 열려있는 상태에서 다른 프로세스가 오픈할 때 허용할 것인가, NULL인 경우 공유하지 않는다
		NULL,						// 보안 속성, 기본값	
		OPEN_EXISTING,				// 생성 방식, CREATE_ALWAYS는 파일이 없다면 생성, 있다면 덮어 쓰기, OPEN_EXISTING 파일이 있을 경우에면 열기
		FILE_ATTRIBUTE_NORMAL,		// 파일 속성(읽기 전용, 숨기 등), FILE_ATTRIBUTE_NORMAL 아무런 속성이 없는 일반 파일 생성
		NULL);						// 생성도리 파일의 속성을 제공할 템플릿 파일, 우리는 사용하지 않아서 NULL

	if (INVALID_HANDLE_VALUE == hFile)
	{
		MessageBox(g_hWnd, _T("Load File"), _T("Fail"), MB_OK);
		return;
	}

	// 2. 파일 쓰기

	m_ParticleList.clear();

	m_ParticleCreateInfo.clear();

	m_iSelectedParticle = 0;

	DWORD		dwByte = 0;

	int ParticleSize;
	ReadFile(hFile, &ParticleSize, sizeof(int), &dwByte, nullptr);

	for (int i = 0; i < ParticleSize; ++i)
	{
		char* Name = TM->Get_ManagedChar();
		PARTICLECREATE ParticleCreateInfo;	
		ReadFile(hFile, Name, sizeof(char) * 256, &dwByte, nullptr);
		ReadFile(hFile, &ParticleCreateInfo, sizeof(PARTICLECREATE), &dwByte, nullptr);

		_tchar* TexPath = TM->Get_ManagedTChar();
		_tchar* TexName = TM->Get_ManagedTChar();
		ReadFile(hFile, TexPath, sizeof(_tchar) * 256, &dwByte, nullptr);
		ReadFile(hFile, TexName, sizeof(_tchar) * 256, &dwByte, nullptr);

		ParticleCreateInfo.TexPath = TexPath;
		ParticleCreateInfo.TexName = TexName;
		if (0 == dwByte)	// 더이상 읽을 데이터가 없을 경우
		{
			break;
		}
		m_ParticleList.push_back(Name);
		m_ParticleCreateInfo.push_back(ParticleCreateInfo);
	}

	m_NowParticles = m_ParticleList[0];
	m_iSelectedParticle = 0;
	// 3. 파일 소멸
	CloseHandle(hFile);
}

void CParticleInstanceTool::ChangeName(char * ParticleName)
{
	char* ListName = TM->Get_ManagedChar();
	strcpy_s(ListName, sizeof(char) * MAX_PATH, ParticleName);
	m_ParticleList[m_iSelectedParticle] = ListName;
	ParticleName = "";
}

void CParticleInstanceTool::SetParticle(_float fTimeDelta)
{
	ImGui::Checkbox("UVImage", &m_bUVImage);
	
	ImGui::PushItemWidth(100.f);
	ImGui::DragFloat("MinPosX", &m_fMinPosX, 0.01f, -100.0f, 100.0f);
	ImGui::SameLine();
	ImGui::DragFloat("MaxPosX", &m_fMaxPosX, 0.01f, -100.0f, 100.0f);

	ImGui::SameLine();
	ImGui::Text("   ");
	ImGui::SameLine();
	if (ImGui::Button("ResetInfo"))
		ResetInfo();

	ImGui::DragFloat("MinPosY", &m_fMinPosY, 0.01f, -100.0f, 100.0f);
	ImGui::SameLine();
	ImGui::DragFloat("MaxPosY", &m_fMaxPosY, 0.01f, -100.0f, 100.0f);

	ImGui::DragFloat("MinPosZ", &m_fMinPosZ, 0.01f, -100.0f, 100.0f);
	ImGui::SameLine();
	ImGui::DragFloat("MaxPosZ", &m_fMaxPosZ, 0.01f, -100.0f, 100.0f);

	ImGui::DragFloat("MinSizeX", &m_fMinSizeX, 0.01f, 0.f, 100.0f);
	ImGui::SameLine();
	ImGui::DragFloat("MaxSizeX", &m_fMaxSizeX, 0.01f, 0.f, 100.0f);

	ImGui::DragFloat("MinSizeY", &m_fMinSizeY, 0.01f, 0.f, 100.0f);
	ImGui::SameLine();
	ImGui::DragFloat("MaxSizeY", &m_fMaxSizeY, 0.01f, 0.f, 100.0f);

	ImGui::DragFloat("MinSpeed", &m_fMinSpeed, 0.01f, 0.f, 100.0f);
	ImGui::SameLine();
	ImGui::DragFloat("MaxSpeed", &m_fMaxSpeed, 0.01f, 0.f, 100.0f);

	ImGui::DragFloat("MinLife", &m_fMinLifeTime, 0.01f, 0.f, 100.0f);
	ImGui::SameLine();
	ImGui::DragFloat("MaxLife", &m_fMaxLifeTime, 0.01f, 0.f, 100.0f);

	ImGui::DragFloat("Gravity", &m_fGravity, 0.01f, 0.f, 100.0f);

	ImGui::DragFloat("Release Speed", (float*)&m_vReleaseSpeed, 0.01f, 0.f, 100.0f);

	ImGui::DragFloat3("Color", (float*)&m_vColor, 0.001f, 0.f, 1.f, "%.3f");

	ImGui::PopItemWidth();
	ImGui::DragFloat3("DirAngle", (float*)&m_vDirAngle, 1.f, -360.f, 360.f);
	
	ImGui::DragFloat3("RangeAngle1", (float*)&m_vRangeAngle1, 1.f, -360.f, 360.f);

	ImGui::DragFloat3("RangeAngle2", (float*)&m_vRangeAngle2, 1.f, -360.f, 360.f);

	
	ImGui::SliderInt("Particle Num", &m_iParticleNum, 1, 300);	

}

void CParticleInstanceTool::FixParticle()
{
	if (m_ParticleList.size() > 0)
	{
		ImGui::Checkbox("UVImage", &m_ParticleCreateInfo[m_iSelectedParticle].bUV);
		ImGui::SameLine();
		ImGui::Checkbox("FixColor", &m_ParticleCreateInfo[m_iSelectedParticle].bFixColor);
		ImGui::SameLine();
		ImGui::Checkbox("Charge", &m_ParticleCreateInfo[m_iSelectedParticle].bCharge);
		ImGui::SameLine();
		ImGui::Checkbox("Distortion", &m_ParticleCreateInfo[m_iSelectedParticle].bDistortion);
		ImGui::SameLine();
		ImGui::Checkbox("Follow", &m_ParticleCreateInfo[m_iSelectedParticle].bFollow);
		ImGui::TextColored(ImVec4{ 0.f,1.f,0.f,1.f }, "Particle Name : ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4{ 1.f,1.f,0.f,1.f }, m_ParticleList[m_iSelectedParticle]);
		ImGui::TextColored(ImVec4{ 0.f,1.f,0.f,1.f }, "ParticleTexture Name : ");
		ImGui::SameLine();
		char* TexName = TM->Get_ManagedChar();
		TM->TCtoC(m_ParticleCreateInfo[m_iSelectedParticle].TexName, TexName);
	
		ImGui::TextColored(ImVec4{ 1.f,1.f,0.f,1.f }, TexName);
		ImGui::SameLine();
		ImGui::Text("         ");
		ImGui::SameLine();
		if (ImGui::Button("Change To SelectTexture"))
			ChangeToSelectTexture();

		
		ImGui::TextColored(ImVec4{ 0.f,1.f,0.f,1.f }, "SelectTexture Name : ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4{ 1.f,1.f,0.f,1.f }, m_TextureList[m_iSelectedTexture]);

		PARTICLECREATE ParticleData;
		ParticleData = m_ParticleCreateInfo[m_iSelectedParticle];

		ImGui::PushItemWidth(100.f);
		ImGui::DragFloat("MinPosX", &m_ParticleCreateInfo[m_iSelectedParticle].vMinPos.x, 0.01f, -100.0f, 100.0f);
		ImGui::SameLine();
		ImGui::DragFloat("MaxPosX", &m_ParticleCreateInfo[m_iSelectedParticle].vMaxPos.x, 0.01f, -100.0f, 100.0f);

		ImGui::DragFloat("MinPosY", &m_ParticleCreateInfo[m_iSelectedParticle].vMinPos.y, 0.01f, -100.0f, 100.0f);
		ImGui::SameLine();
		ImGui::DragFloat("MaxPosY", &m_ParticleCreateInfo[m_iSelectedParticle].vMaxPos.y, 0.01f, -100.0f, 100.0f);

		ImGui::DragFloat("MinPosZ", &m_ParticleCreateInfo[m_iSelectedParticle].vMinPos.z, 0.01f, -100.0f, 100.0f);
		ImGui::SameLine();
		ImGui::DragFloat("MaxPosZ", &m_ParticleCreateInfo[m_iSelectedParticle].vMaxPos.z, 0.01f, -100.0f, 100.0f);

		ImGui::DragFloat("MinSizeX", &m_ParticleCreateInfo[m_iSelectedParticle].vMinSize.x, 0.01f, 0.f, 100.0f);
		ImGui::SameLine();
		ImGui::DragFloat("MaxSizeX", &m_ParticleCreateInfo[m_iSelectedParticle].vMaxSize.x, 0.01f, 0.f, 100.0f);

		ImGui::DragFloat("MinSizeY", &m_ParticleCreateInfo[m_iSelectedParticle].vMinSize.y, 0.01f, 0.f, 100.0f);
		ImGui::SameLine();
		ImGui::DragFloat("MaxSizeY", &m_ParticleCreateInfo[m_iSelectedParticle].vMaxSize.y, 0.01f, 0.f, 100.0f);

		ImGui::DragFloat("MinSpeed", &m_ParticleCreateInfo[m_iSelectedParticle].fMinSpeed, 0.01f, 0.f, 100.0f);
		ImGui::SameLine();
		ImGui::DragFloat("MaxSpeed", &m_ParticleCreateInfo[m_iSelectedParticle].fMaxSpeed, 0.01f, 0.f, 100.0f);

		ImGui::DragFloat("MinLife", &m_ParticleCreateInfo[m_iSelectedParticle].fMinLifeTime, 0.01f, 0.f, 100.0f);
		ImGui::SameLine();
		ImGui::DragFloat("MaxLife", &m_ParticleCreateInfo[m_iSelectedParticle].fMaxLifeTime, 0.01f, 0.f, 100.0f);

		ImGui::DragFloat("Gravity", &m_ParticleCreateInfo[m_iSelectedParticle].fGravity, 0.01f, 0.f, 100.0f);

		ImGui::DragFloat2("Release Speed", (float*)&m_ParticleCreateInfo[m_iSelectedParticle].vReleaseSpeed, 0.01f, 0.f, 100.0f);	

		ImGui::PopItemWidth();

		ImGui::DragFloat3("DirAngle", (float*)&m_ParticleCreateInfo[m_iSelectedParticle].vDirAngle, 1.f, -360.f, 360.f);

		ImGui::DragFloat3("RangeAngle1", (float*)&m_ParticleCreateInfo[m_iSelectedParticle].vRangeAngle1, 1.f, -360.f, 360.f);

		ImGui::DragFloat3("RangeAngle2", (float*)&m_ParticleCreateInfo[m_iSelectedParticle].vRangeAngle2, 1.f, -360.f, 360.f);
	
		_float4 vWorldPos = m_ParticleCreateInfo[m_iSelectedParticle].vWorldPos;
		ImGui::DragFloat4("LocalPos", (float*)&vWorldPos, 0.1f);
		m_ParticleCreateInfo[m_iSelectedParticle].vWorldPos = vWorldPos;

		ImGui::SliderInt("Particle Num", &m_ParticleCreateInfo[m_iSelectedParticle].iParticleNum, 1, 300);
		ImGui::DragInt("ShaderPass", &m_ParticleCreateInfo[m_iSelectedParticle].iShaderPass, 1,0, 15);
		ImGui::DragFloat("AlphaTest", &m_ParticleCreateInfo[m_iSelectedParticle].fAlphaTest, 0.001f, 0.f, 1.f, "%.3f");
		
		ImGui::DragFloat("BlurPower", &m_ParticleCreateInfo[m_iSelectedParticle].fBlurPower, 0.001f, 0.f, 30.f, "%.3f");	
		if (m_ParticleCreateInfo[m_iSelectedParticle].bFixColor == true)
		{
			ImGui::DragFloat4("Color1", (float*)&m_ParticleCreateInfo[m_iSelectedParticle].vColor, 0.001f, 0.f, 1.f, "%.3f");
			ImGui::DragFloat4("Color2", (float*)&m_ParticleCreateInfo[m_iSelectedParticle].vColor2, 0.001f, 0.f, 1.f, "%.3f");
			ImGui::DragFloat2("Ratio", (float*)&m_ParticleCreateInfo[m_iSelectedParticle].vRatio, 0.001f, 0.f, 1.f, "%.3f");
			ImGui::DragFloat2("RatioSpeed", (float*)&m_ParticleCreateInfo[m_iSelectedParticle].vRatioSpeed, 0.001f, 0.f, 1.f, "%.3f");
		}

		if (m_ParticleCreateInfo[m_iSelectedParticle].bUV)
		{
			ImGui::DragFloat("UVMaxIndexX", &m_ParticleCreateInfo[m_iSelectedParticle].fUVMaxIndexX, 1.f, 0.f, 100.0f);
			ImGui::DragFloat("UVMaxIndexY", &m_ParticleCreateInfo[m_iSelectedParticle].fUVMaxIndexY, 1.f, 0.f, 100.0f);
			ImGui::DragFloat("UVSpeed", &m_ParticleCreateInfo[m_iSelectedParticle].fUVSpeed, 0.01f, 0.f, 100.0f);
			ImGui::Checkbox("UVLoof", &m_ParticleCreateInfo[m_iSelectedParticle].bUVLoof);
		}		

		if (m_ParticleCreateInfo[m_iSelectedParticle].bCharge)
		{
			ImGui::DragFloat4("ChargePos", (float*)&m_ParticleCreateInfo[m_iSelectedParticle].vChargePos, 0.01f, -100.f, 100.f, "%.3f");
			ImGui::DragFloat("ChargeRange", &m_ParticleCreateInfo[m_iSelectedParticle].fChargeRange, 0.01f, 0.f, 100.0f, "%.3f");
		}

		if (m_ParticleCreateInfo[m_iSelectedParticle].bDistortion)
		{
			ImGui::DragFloat("DistortionPower", &m_ParticleCreateInfo[m_iSelectedParticle].fDistortionPower, 0.01f, 0.f, 100.0f, "%.3f");
		}
		if (ImGui::Button("Play"))
		{
			Play();
		}
	}
	
	else
	{
		ImGui::TextColored(ImVec4{ 0.f,1.f,0.f,1.f }, "Particle Name : ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4{ 1.f,1.f,0.f,1.f }, "Null");
		ImGui::TextColored(ImVec4{ 0.f,1.f,0.f,1.f }, "Texture Name : ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4{ 1.f,1.f,0.f,1.f }, "Null");
		ImGui::TextColored(ImVec4{ 0.f,1.f,0.f,1.f }, "SelectTexture Name : ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4{ 1.f,1.f,0.f,1.f }, "Null");
	}
		

}

void CParticleInstanceTool::ResetInfo()
{
	m_fMinPosX = 0.f;
	m_fMinPosY = 0.f;
	m_fMinPosZ = 0.f;
	m_fMaxPosX = 0.f;
	m_fMaxPosY = 0.f;
	m_fMaxPosZ = 0.f;
	m_fMinSizeX = 0.f;
	m_fMinSizeY = 0.f;
	m_fMaxSizeX = 0.f;
	m_fMaxSizeY = 0.f;
	m_fMinSpeed = 0.f;
	m_fMaxSpeed = 0.f;
	m_fMinLifeTime = 0.f;
	m_fMaxLifeTime = 0.f;
	m_fGravity = 0.f;
	m_vDirAngle = { 0.f,0.f,0.f };
	m_vRangeAngle1 = { 0.f,0.f,0.f };
	m_vRangeAngle2 = { 0.f,0.f,0.f };
	m_iParticleNum = 1;
	m_vReleaseSpeed = { 0.f,0.f };
	m_bUVImage = false;
	/*0 1 0 벡터를 회전시켜주는 float3 으로 회전행렬을 만들어서 회전시켜주고 
		얘를 vBaseDir로 대입해서 즉 위에 구한거로 
		회전행렬을 만들어서 룩을 뽑아낸다 

		DirAngle == 0 0 0  010 
		!= 
		DirAngle Rotaion 010 * vector -> baseDir -> Look

	_vector vBaseDir = XMLoadFloat3(&vDir);

	_float fRandAnlge_X = CToolManager::Get_Instance()->Get_RendomNum(vLimitAngleMin.x, vLimitAngleMax.x);
	_float fRandAnlge_Y = CToolManager::Get_Instance()->Get_RendomNum(vLimitAngleMin.y, vLimitAngleMax.y);
	_float fRandAnlge_Z = CToolManager::Get_Instance()->Get_RendomNum(vLimitAngleMin.z, vLimitAngleMax.z);

	_matrix mRX = XMMatrixRotationAxis(XMVectorSet(1.f, 0.f, 0.f, 0.f), XMConvertToRadians(fRandAnlge_X));
	_matrix mRY = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(fRandAnlge_Y));
	_matrix mRZ = XMMatrixRotationAxis(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(fRandAnlge_Z));

	vBaseDir = XMVector3TransformNormal(vBaseDir, mRX);
	vBaseDir = XMVector3TransformNormal(vBaseDir, mRY);
	vBaseDir = XMVector3TransformNormal(vBaseDir, mRZ);

	XMStoreFloat3(Out_vDir, vBaseDir);*/
}

void CParticleInstanceTool::Play()
{
	if (!lstrcmp(m_ParticleCreateInfo[m_iSelectedParticle].TexName, L"Null"))
	{
		MSG_BOX(TEXT("Don't Have Texture"));
		return;
	}

	if (m_ParticleCreateInfo[m_iSelectedParticle].vMinPos.x > m_ParticleCreateInfo[m_iSelectedParticle].vMaxPos.x
		|| m_ParticleCreateInfo[m_iSelectedParticle].vMinPos.y > m_ParticleCreateInfo[m_iSelectedParticle].vMaxPos.y
		|| m_ParticleCreateInfo[m_iSelectedParticle].vMinPos.z > m_ParticleCreateInfo[m_iSelectedParticle].vMaxPos.z
		|| m_ParticleCreateInfo[m_iSelectedParticle].vMinSize.x > m_ParticleCreateInfo[m_iSelectedParticle].vMaxSize.x
		|| m_ParticleCreateInfo[m_iSelectedParticle].vMinSize.y > m_ParticleCreateInfo[m_iSelectedParticle].vMaxSize.y
		|| m_ParticleCreateInfo[m_iSelectedParticle].fMinLifeTime > m_ParticleCreateInfo[m_iSelectedParticle].fMaxLifeTime
		|| m_ParticleCreateInfo[m_iSelectedParticle].fMinSpeed > m_ParticleCreateInfo[m_iSelectedParticle].fMaxSpeed	
		|| m_ParticleCreateInfo[m_iSelectedParticle].vRangeAngle1.x > m_ParticleCreateInfo[m_iSelectedParticle].vRangeAngle2.x
		|| m_ParticleCreateInfo[m_iSelectedParticle].vRangeAngle1.y > m_ParticleCreateInfo[m_iSelectedParticle].vRangeAngle2.y
		|| m_ParticleCreateInfo[m_iSelectedParticle].vRangeAngle1.z > m_ParticleCreateInfo[m_iSelectedParticle].vRangeAngle2.z)
	{
		MSG_BOX(TEXT("MinData Higher MaxData"));
		return;
	}

	CParticleInstance::PARTICLEINSTANCEINFO* Info = new CParticleInstance::PARTICLEINSTANCEINFO;
	Info->bBlend = m_ParticleCreateInfo[m_iSelectedParticle].bBlend;
	Info->bUV = m_ParticleCreateInfo[m_iSelectedParticle].bUV;
	Info->bUVLoof = m_ParticleCreateInfo[m_iSelectedParticle].bUVLoof;
	Info->fAlphaTest = m_ParticleCreateInfo[m_iSelectedParticle].fAlphaTest;
	Info->fBlurPower = m_ParticleCreateInfo[m_iSelectedParticle].fBlurPower;
	Info->fGravity = m_ParticleCreateInfo[m_iSelectedParticle].fGravity;
	Info->fMaxLifeTime = m_ParticleCreateInfo[m_iSelectedParticle].fMaxLifeTime;
	Info->fMaxSpeed = m_ParticleCreateInfo[m_iSelectedParticle].fMaxSpeed;
	Info->fMinLifeTime = m_ParticleCreateInfo[m_iSelectedParticle].fMinLifeTime;
	Info->fMinSpeed = m_ParticleCreateInfo[m_iSelectedParticle].fMinSpeed;
	Info->fUVMaxIndexX = m_ParticleCreateInfo[m_iSelectedParticle].fUVMaxIndexX;
	Info->fUVMaxIndexY = m_ParticleCreateInfo[m_iSelectedParticle].fUVMaxIndexY;
	Info->fUVSpeed = m_ParticleCreateInfo[m_iSelectedParticle].fUVSpeed;
	Info->iParticleNum = m_ParticleCreateInfo[m_iSelectedParticle].iParticleNum;
	Info->iShaderPass = m_ParticleCreateInfo[m_iSelectedParticle].iShaderPass;
	Info->TexName = m_ParticleCreateInfo[m_iSelectedParticle].TexName;
	Info->TexPath = m_ParticleCreateInfo[m_iSelectedParticle].TexPath;
	Info->vDirAngle = m_ParticleCreateInfo[m_iSelectedParticle].vDirAngle;
	Info->vMaxPos = m_ParticleCreateInfo[m_iSelectedParticle].vMaxPos;
	Info->vMaxSize = m_ParticleCreateInfo[m_iSelectedParticle].vMaxSize;
	Info->vMinPos = m_ParticleCreateInfo[m_iSelectedParticle].vMinPos;
	Info->vMinSize = m_ParticleCreateInfo[m_iSelectedParticle].vMinSize;
	Info->vRangeAngle1 = m_ParticleCreateInfo[m_iSelectedParticle].vRangeAngle1;
	Info->vRangeAngle2 = m_ParticleCreateInfo[m_iSelectedParticle].vRangeAngle2;
	Info->vReleaseSpeed = m_ParticleCreateInfo[m_iSelectedParticle].vReleaseSpeed;
	Info->iShaderPass = m_ParticleCreateInfo[m_iSelectedParticle].iShaderPass;
	Info->vColor = m_ParticleCreateInfo[m_iSelectedParticle].vColor;
	Info->vColor2 = m_ParticleCreateInfo[m_iSelectedParticle].vColor2;
	Info->vRatio = m_ParticleCreateInfo[m_iSelectedParticle].vRatio;
	Info->vRatioSpeed = m_ParticleCreateInfo[m_iSelectedParticle].vRatioSpeed;
	Info->bFixColor = m_ParticleCreateInfo[m_iSelectedParticle].bFixColor;
	Info->bCharge = m_ParticleCreateInfo[m_iSelectedParticle].bCharge;
	Info->vChargePos = m_ParticleCreateInfo[m_iSelectedParticle].vChargePos;
	Info->fChargeRange = m_ParticleCreateInfo[m_iSelectedParticle].fChargeRange;
	Info->bDistortion = m_ParticleCreateInfo[m_iSelectedParticle].bDistortion;
	Info->fDistortionPower = m_ParticleCreateInfo[m_iSelectedParticle].fDistortionPower;
	Info->bFollow = m_ParticleCreateInfo[m_iSelectedParticle].bFollow;
	Info->vLocalPos = m_ParticleCreateInfo[m_iSelectedParticle].vWorldPos;
	_matrix LocalMatrix = XMMatrixIdentity();
	XMStoreFloat4x4(&Info->WorldMatrix, LocalMatrix);

	if (FAILED(GI->Add_GameObjectToLayer(L"ParticleInstance", LEVEL_TOOL, L"Layer_Particle", &Info)))
		return;
	
}

void CParticleInstanceTool::ChangeToSelectTexture()
{
	if (m_TextureList.size() > 0)
	{
		int count = 0;
		for (auto& iter : m_TexInfo)
		{
			if (m_iSelectedTexture == count)
			{
				if (!lstrcmp(m_ParticleCreateInfo[m_iSelectedParticle].TexName, L"Null"))
				{
					_tchar* TPath = TM->Get_ManagedTChar();
					_tchar* TName = TM->Get_ManagedTChar();

					lstrcpy(TPath, iter->TexPath.c_str());
					lstrcpy(TName, iter->TexName.c_str());

					m_ParticleCreateInfo[m_iSelectedParticle].TexPath = TPath;
					m_ParticleCreateInfo[m_iSelectedParticle].TexName = TName;
					return;
				}

				else
				{
					_tchar* TPath = TM->Get_ManagedTChar();
					_tchar* TName = TM->Get_ManagedTChar();

					lstrcpy(TPath, iter->TexPath.c_str());
					lstrcpy(TName, iter->TexName.c_str());

					m_ParticleCreateInfo[m_iSelectedParticle].TexPath = TPath;
					m_ParticleCreateInfo[m_iSelectedParticle].TexName = TName;
					return;

				}
			}
			else
				++count;
		}
	}
}

void CParticleInstanceTool::SetParticleInfo(char* ParticleName)
{

	m_vMinPos.x = m_fMinPosX;
	m_vMinPos.y = m_fMinPosY;
	m_vMinPos.z = m_fMinPosZ;

	m_vMaxPos.x = m_fMaxPosX;
	m_vMaxPos.y = m_fMaxPosY;
	m_vMaxPos.z = m_fMaxPosZ;

	m_vMinSize.x = m_fMinSizeX;
	m_vMinSize.y = m_fMinSizeY;

	m_vMaxSize.x = m_fMaxSizeX;
	m_vMaxSize.y = m_fMaxSizeY;

	

	char* ListName = TM->Get_ManagedChar();
	strcpy_s(ListName, sizeof(char) * 256, ParticleName);

	PARTICLECREATE CreateInfo;
	CreateInfo.vMinPos = m_vMinPos;
	CreateInfo.vMaxPos = m_vMaxPos;
	CreateInfo.vMinSize = m_vMinSize;
	CreateInfo.vMaxSize = m_vMaxSize;
	CreateInfo.fMinLifeTime = m_fMinLifeTime;
	CreateInfo.fMaxLifeTime = m_fMaxLifeTime;
	CreateInfo.fMinSpeed = m_fMinSpeed;
	CreateInfo.fMaxSpeed = m_fMaxSpeed;
	CreateInfo.fGravity = m_fGravity;
	CreateInfo.vDirAngle = m_vDirAngle;
	CreateInfo.vRangeAngle1 = m_vRangeAngle1;
	CreateInfo.vRangeAngle2 = m_vRangeAngle2;
	CreateInfo.iParticleNum = m_iParticleNum;
	CreateInfo.bUV = m_bUV;
	CreateInfo.iShaderPass = m_iShaderPass;
	CreateInfo.fUVMaxIndexX = m_fUVMaxIndexX;
	CreateInfo.fUVMaxIndexY = m_fUVMaxIndexY;
	CreateInfo.fUVSpeed = m_fUVSpeed;
	CreateInfo.vReleaseSpeed = m_vReleaseSpeed;
	
	_tchar* Ctemp = TM->Get_ManagedTChar();
	TM->CtoTC(m_TextureList[m_iSelectedTexture], Ctemp);
	TEXINFO* tex = GetTexInfo(Ctemp);


	if (tex->TexName != L"")
	{
		_tchar* TexPath = TM->Get_ManagedTChar();
		const _tchar* temp = tex->TexPath.c_str();
		_tchar* TexName = TM->Get_ManagedTChar();
		const _tchar* temp2 = tex->TexName.c_str();
		for (int i = 0; i < 256; ++i)
		{
			TexPath[i] = temp[i];
			TexName[i] = temp2[i];
		}
		CreateInfo.TexPath = TexPath;
		CreateInfo.TexName = TexName;
	}
	m_ParticleCreateInfo.push_back(CreateInfo);

	m_ParticleList.push_back(ListName);


	char Temp[256] = "";
	strcpy_s(ParticleName, sizeof(Temp), Temp);
	ResetInfo();
}

HRESULT CParticleInstanceTool::CreateResourceView(wstring TextureTag, const _tchar * pTextureFilePath, _uint iNumTextures)
{
	if (FAILED(CheckViewName(TextureTag)))
	{
		MSG_BOX(TEXT("already Use TexName Please Change"));
	}

	vector<ID3D11ShaderResourceView*> vTemp;
	for (_uint i = 0; i < iNumTextures; ++i)
	{
		_tchar			szFullPath[256] = TEXT("");

		wsprintf(szFullPath, pTextureFilePath, i);

		_tchar			szDrive[256] = TEXT("");
		_tchar			szExt[256] = TEXT("");

		_wsplitpath_s(szFullPath, szDrive, 256, nullptr, 0, nullptr, 0, szExt, 256);

		HRESULT hr = 0;

		ID3D11ShaderResourceView*			pSRV = nullptr;

		if (false == lstrcmp(TEXT(".dds"), szExt))
			hr = DirectX::CreateDDSTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV);

		else if (false == lstrcmp(TEXT(".tga"), szExt))
			hr = E_FAIL;

		else
			hr = DirectX::CreateWICTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV);

		if (FAILED(hr))
			return E_FAIL;
		if (pSRV == nullptr)
			return E_FAIL;

		vTemp.push_back(pSRV);
	}
	m_ResourceView.emplace(TextureTag, vTemp);
	return S_OK;
}

vector<ID3D11ShaderResourceView*> CParticleInstanceTool::FindResourceView(wstring TextureName)
{
	for (auto& iter : m_ResourceView)
	{
		if (iter.first == TextureName)
		{
			return iter.second;
		}
	}
	vector<ID3D11ShaderResourceView*> temp;
	return temp;
}

HRESULT CParticleInstanceTool::CheckViewName(wstring TextureName)
{
	for (auto& View : m_ResourceView)
	{
		if (View.first == TextureName)
		{
			return E_FAIL;
		}
	}
	return S_OK;
}

void CParticleInstanceTool::Delete_ResourceView()
{
	for (auto& iter : m_ResourceView)
	{
		_tchar* temp = TM->Get_ManagedTChar();
		TM->CtoTC(m_TextureList[m_iSelectedTexture], temp);
		wstring Name = temp;
		if (iter.first == Name)
		{
			for (auto& SRV : iter.second)
			{
				SRV->Release();
			}
			iter.second.clear();
		}
		m_ResourceView.erase(Name);
		break;
	}
}

void CParticleInstanceTool::LoadTexture()
{
	for (auto& iter : *(DM->Get_TextureFileNames()))
	{
		_tchar* Name = TM->Get_ManagedTChar();		
		TM->CtoTC(iter.data(), Name);
		string Path = "../Bin/Resources/Particle/";
		Path = Path + iter.data();
		_tchar* tPath = TM->Get_ManagedTChar();
		TM->CtoTC(Path.data(), tPath);

		

		int EndIndex = (int)iter.length() - 4;
		string temp = iter.substr(0, EndIndex);
		char* RealName = TM->Get_ManagedChar();
		for (int i = 0; i < temp.length(); ++i)
			RealName[i] = temp.data()[i];

		_tchar* tagName = TM->Get_ManagedTChar();
		TM->CtoTC(RealName, tagName);

		CreateResourceView(tagName, tPath, 1);		
		
		m_TextureList.push_back(RealName);

		TEXINFO* texInfo = new TEXINFO;
		_tchar* texInfoName = TM->Get_ManagedTChar();
		TM->CtoTC(RealName, texInfoName);
		texInfo->TexPath = tPath;
		texInfo->TexName = texInfoName;

		m_TexInfo.push_back(texInfo);
		
	}
}

void CParticleInstanceTool::ShowTextureInfoWindow(_float fTimeDelta)
{
	ImGui::TextColored(ImVec4{ 0.f,1.f,0.f,1.f }, m_NowTextures.c_str());

	if (ImGui::BeginListBox("Texlist", ImVec2(400, 280)))
	{
		for (int i = 0; i < m_TextureList.size(); i++)
		{
			const bool is_selected = (m_iSelectedTexture == i);
			if (ImGui::Selectable(m_TextureList[i], is_selected))
				m_iSelectedTexture = i;

			if (is_selected)
				ImGui::SetItemDefaultFocus();

		}
		ImGui::EndListBox();
	}

	if (m_TextureList.size() > 0)
	{
		_tchar* TexName = new _tchar[MAX_PATH];
		TM->CtoTC(m_TextureList[m_iSelectedTexture], TexName);

		ImGui::ImageButton(FindResourceView(TexName)[0], ImVec2{ 120.f,120.f });

		delete TexName;
	}
}

CParticleInstanceTool::TEXINFO * CParticleInstanceTool::GetTexInfo(wstring TexName)
{
	for (auto& TexInfo : m_TexInfo)
	{
		if (TexInfo->TexName == TexName)
			return TexInfo;
	}
	return nullptr;
}

void CParticleInstanceTool::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	
	/*for (auto& iter : m_ParticleCreateInfo)
	{
		if (lstrcmp(iter.TexName, L"Null"))
		{
			delete iter.TexPath;
			delete iter.TexName;
		}
	}*/
	m_ParticleCreateInfo.clear();

	for (auto& iter : m_TexInfo)
	{
		Safe_Delete(iter);
	}
	m_TexInfo.clear();

	for (auto& pSRV : m_ResourceView)
	{

		for (auto& iter : pSRV.second)
		{
			iter->Release();
		}
		pSRV.second.clear();
	}

	m_ResourceView.clear();
}
