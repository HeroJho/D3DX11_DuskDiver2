#include "stdafx.h"
#include "..\Public\AnimTool.h"
#include "AnimMesh.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "DataManager.h"

CAnimTool::CAnimTool(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

void CAnimTool::Initialize()
{
	m_TabName.push_back("AnimModelObject");
	m_TabName.push_back("EditAnimation");
	Load_DataList();
	LoadObjectList();
}

void CAnimTool::ShowAnimWindow(_float fTimeDelta)
{
	static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyResizeDown;
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("AnimObject"))
		{
			ShowAnimModelObjectWindow(fTimeDelta);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("EditAnimation"))
		{
			if (m_AnimMesh.size() > 0)
			{
				ShowAnimEditWindow(fTimeDelta);
			}
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

void CAnimTool::ShowAnimModelObjectWindow(_float fTimeDelta)
{
	ImGui::PushItemWidth(200.f);
	if (ImGui::BeginCombo(" ", m_ModelObjectList[m_iSelectedObjectList], ImGuiComboFlags_HeightLargest)) {
		for (int i = 0; i < m_ModelObjectList.size(); ++i) {
			const bool isSelected = (m_iSelectedObjectList == i);
			if (ImGui::Selectable(m_ModelObjectList[i], isSelected)) {
				m_iSelectedObjectList = i;
			}
		}
		ImGui::EndCombo();
	}
	ImGui::PopItemWidth();

	ImGui::SameLine();

	if (ImGui::Button("LoadObject"))
	{
		
			char temp[256];
			strcpy_s(temp, sizeof(temp), m_ModelObjectList[m_iSelectedObjectList]);
			LoadObject(temp);
		
	}

	ImGui::TextColored(ImVec4{ 1.f,0.f,0.f,1.f }, m_NowLevels.c_str());

	if (ImGui::BeginListBox("ModelList", ImVec2(400, 150)))
	{
		
		for (int i = 0; i < m_ModelName.size(); i++)
		{
			const bool is_selected = (m_iSelectedModel == i);
			if (ImGui::Selectable(m_ModelName[i], is_selected))
				m_iSelectedModel = i;

			if (is_selected)
				ImGui::SetItemDefaultFocus();

		}

		

		ImGui::EndListBox();
	}


	if (ImGui::Button("Add Model"))
	{
		CreateModelObject();
	}

	ImGui::SameLine();

	if (ImGui::Button("Delete Model"))
	{
		if (m_AnimMesh.size() > 0)
			DeleteObject();
	}

	ModelFix();

	ImGui::PushItemWidth(200.f);
	ImGui::Text("");
	static char ObjectLevelName[256] = "";
	ImGui::InputText("LevelName", ObjectLevelName, IM_ARRAYSIZE(ObjectLevelName));

	if (ImGui::Button("SaveModelObject"))
	{
		if(strcmp(ObjectLevelName, ""))
		SaveObject(ObjectLevelName);
	}
	ImGui::PopItemWidth();
}

int CAnimTool::Find_ModelObjectIndex(string sTag)
{
	for (auto& iter : m_ModelObjectIndex)
	{
		if (iter.first == sTag)
			return iter.second++;
	}
	return 0;
}

bool CAnimTool::FInd_ModelObjectMap(string sTag)
{
	for (auto& iter : m_ModelObjectIndex)
	{
		if (iter.first == sTag)
			return true;
	}
	return false;
}

bool CAnimTool::Find_ModelObjectList(char * ListName)
{
	for (auto& iter : m_ModelObjectList)
	{
		if (!strcmp(iter, ListName))
			return true;
	}
	return false;
}

void CAnimTool::ModelFix()
{
	if (m_ModelName.size() > m_iSelectedModel)
	{
		_float3 Pos = m_AnimMesh[m_iSelectedModel]->Get_Pos();
		m_fPosX = Pos.x;
		ImGui::DragFloat("PosX", &m_fPosX, 0.01f, -1000.0f, 1000.0f);

		m_fPosY = Pos.y;
		ImGui::DragFloat("PosY", &m_fPosY, 0.01f, -1000.0f, 1000.0f);

		m_fPosZ = Pos.z;
		ImGui::DragFloat("PosZ", &m_fPosZ, 0.01f, -1000.0f, 1000.0f);
		m_AnimMesh[m_iSelectedModel]->Set_Pos(_float4{ m_fPosX, m_fPosY, m_fPosZ, 1.f });

		m_fSize = m_AnimMesh[m_iSelectedModel]->Get_Scale().x;
		ImGui::DragFloat("Scale", &m_fSize, 0.f, 0.01f, 100.0f);
		m_AnimMesh[m_iSelectedModel]->Set_Scale(_float3{ m_fSize,m_fSize,m_fSize });

		_float3 Angle = m_AnimMesh[m_iSelectedModel]->Get_Angles();
		m_fAngleX = Angle.x;
		ImGui::DragFloat("AngleX", &m_fAngleX, 0.f, 0.f, 360.f);

		m_fAngleY = Angle.y;
		ImGui::DragFloat("AngleY", &m_fAngleY, 0.f, 0.f, 360.f);

		m_fAngleZ = Angle.z;
		ImGui::DragFloat("AngleZ", &m_fAngleZ, 0.f, 0.f, 360.f);
		m_AnimMesh[m_iSelectedModel]->Rotation(_float3{ 1.f,0.f,0.f }, m_fAngleX, _float3{ 0.f,1.f,0.f }, m_fAngleY, _float3{ 0.f,0.f,1.f }, m_fAngleZ);

	}
}

void CAnimTool::DeleteObject()
{
	m_AnimMesh[m_iSelectedModel]->Set_Dead();
	Safe_Release(m_AnimMesh[m_iSelectedModel]);
	m_AnimMesh.erase(m_AnimMesh.begin() + m_iSelectedModel);
	delete m_ModelName[m_iSelectedModel];
	m_ModelName.erase(m_ModelName.begin() + m_iSelectedModel);
}

void CAnimTool::SaveObject(char * ListFileName)
{
	if (m_AnimMesh.size() < 1)
	{
		MSG_BOX(TEXT("Don't have Data"));
		return;
	}
	string FileSave = ListFileName;

	string temp = "../Data/AnimModelObject/";

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

	_int ModelObject = (_int)m_ModelName.size();
	WriteFile(hFile, &ModelObject, sizeof(int), &dwByte, nullptr);

	for (auto& iter : m_ModelName)
	{
		char temp[256] = "";
		strcpy_s(temp, sizeof(char) * 256, iter);
		WriteFile(hFile, temp, sizeof(char) * 256, &dwByte, nullptr);

	}

	for (auto& Data : m_AnimMesh)
	{
		_tchar ObjectName[256] = { 0 };
		wcscpy_s(ObjectName, Data->Get_Tag());

		WriteFile(hFile, ObjectName, sizeof(_tchar) * 256, &dwByte, nullptr);

		WriteFile(hFile, &Data->Get_Scale(), sizeof(_float3), &dwByte, nullptr);

		WriteFile(hFile, &Data->Get_Pos(), sizeof(_float4), &dwByte, nullptr);

		WriteFile(hFile, &Data->Get_Angles(), sizeof(_float3), &dwByte, nullptr);
	}

	MSG_BOX(TEXT("Succeed Save"));
	char* ListTemp = new char[256];
	strcpy_s(ListTemp, sizeof(char) * 256, ListFileName);
	if (!Find_ModelObjectList(ListTemp))
		m_ModelObjectList.push_back(ListTemp);
	else
		Safe_Delete(ListTemp);

	char Temp2[256] = "";
	strcpy_s(ListFileName, sizeof(Temp2), Temp2);

	SaveObjectList();
	// 3. 파일 소멸
	CloseHandle(hFile);
}

void CAnimTool::LoadObject(char * ListFileName)
{
	string FileSave = ListFileName;

	string temp = "../Data/AnimModelObject/";

	FileSave = temp + FileSave + ".dat";

	wchar_t FilePath[256] = { 0 };

	for (int i = 0; i < FileSave.size(); i++)
	{
		FilePath[i] = FileSave[i];
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

	for (auto& iter : m_AnimMesh)
	{
		iter->Set_Dead();
		Safe_Release(iter);
	}
	m_AnimMesh.clear();

	for (auto& iter : m_ModelName)
	{
		delete(iter);
	}
	m_ModelName.clear();

	m_ModelObjectIndex.clear();


	DWORD		dwByte = 0;

	int ModelObjectSize;
	ReadFile(hFile, &ModelObjectSize, sizeof(int), &dwByte, nullptr);

	for (int i = 0; i < ModelObjectSize; ++i)
	{
		char* Name = new char[256];
		ReadFile(hFile, Name, sizeof(char) * 256, &dwByte, nullptr);
		m_ModelName.push_back(Name);
	}

	while (true)
	{
		_tchar* ModelName = new _tchar[256];
		ReadFile(hFile, ModelName, sizeof(_tchar) * 256, &dwByte, nullptr);

		_float3 fScale;
		ReadFile(hFile, &fScale, sizeof(_float3), &dwByte, nullptr);

		_float4 fPos;
		ReadFile(hFile, &fPos, sizeof(_float4), &dwByte, nullptr);

		_float3 fAngle;
		ReadFile(hFile, &fAngle, sizeof(_float3), &dwByte, nullptr);

		CAnimMesh::MESHINFO* MeshInfo;
		MeshInfo = new CAnimMesh::MESHINFO;
		MeshInfo->sTag = ModelName;
		MeshInfo->fScale = fScale;
		MeshInfo->fPos = fPos;
		MeshInfo->fAngle = fAngle;

		if (0 == dwByte)	// 더이상 읽을 데이터가 없을 경우
		{
			Safe_Delete_Array(ModelName);
			Safe_Delete(MeshInfo);
			break;
		}
		if (FAILED(GI->Add_GameObjectToLayer(L"AnimMesh", LEVEL_TOOL, L"Layer_AnimModelObject", MeshInfo)))
		{
			wstring a = L"Please Load ProtoType";
			wstring b = a + ModelName;
			const _tchar* c = b.c_str();
			MSG_BOX(c);

			Safe_Delete_Array(ModelName);
			Safe_Delete(MeshInfo);

			for (auto& iter : m_AnimMesh)
			{
				Safe_Release(iter);
			}
			m_AnimMesh.clear();

			for (auto& iter : m_ModelName)
			{
				delete(iter);
			}
			m_ModelName.clear();

			m_ModelObjectIndex.clear();
			return;
		}
		char* _char;
		int _tcharSize = WideCharToMultiByte(CP_ACP, 0, ModelName, -1, NULL, 0, NULL, NULL);
		_char = new char[_tcharSize];
		WideCharToMultiByte(CP_ACP, 0, ModelName, -1, _char, _tcharSize, 0, 0);
		string temp = _char;
		if (!FInd_ModelObjectMap(temp))
		{
			m_ModelObjectIndex.emplace(temp, 1);
		}
		else
			Find_ModelObjectIndex(temp);
		Safe_Delete(_char);
		Safe_Delete(ModelName);
		Safe_Delete(MeshInfo);

	}
	m_NowLevels = ListFileName;
	// 3. 파일 소멸
	CloseHandle(hFile);

	m_iSelectedModel = 0;
}

void CAnimTool::SaveObjectList()
{
	string FileSave = "../Data/ListData/AnimModelObjectList.dat";

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
		// 팝업 창을 출력해주는 기능의 함수
		// 1. 핸들 2. 팝업 창에 띄우고자하는 메시지 3. 팝업 창 이름 4. 버튼 속성
		MessageBox(g_hWnd, _T("Save File"), _T("Fail"), MB_OK);
		return;
	}

	DWORD		dwByte = 0;

	for (auto& Data : m_ModelObjectList)
	{

		char ListName[256] = { 0 };
		strcpy_s(ListName, sizeof(char) * 256, Data);

		WriteFile(hFile, ListName, sizeof(char) * 256, &dwByte, nullptr);
	}

	CloseHandle(hFile);
}

void CAnimTool::LoadObjectList()
{
	string FileSave = "../Data/ListData/AnimModelObjectList.dat";

	wchar_t FilePath[256] = { 0 };

	for (int i = 0; i < FileSave.size(); i++)
	{
		FilePath[i] = FileSave[i];
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
		// 팝업 창을 출력해주는 기능의 함수
		// 1. 핸들 2. 팝업 창에 띄우고자하는 메시지 3. 팝업 창 이름 4. 버튼 속성
		MessageBox(g_hWnd, _T("Load File"), _T("Fail"), MB_OK);
		return;
	}

	DWORD		dwByte = 0;

	while (true)
	{

		char* ListName = new char[256];
		ReadFile(hFile, ListName, sizeof(char) * 256, &dwByte, nullptr);

		if (0 == dwByte)	// 더이상 읽을 데이터가 없을 경우
		{
			Safe_Delete_Array(ListName);
			break;
		}

		string FileSave = "../Data/AnimModelObject/";

		string FullPath = FileSave + ListName + ".dat";

		wchar_t FilePath[256] = { 0 };

		for (int i = 0; i < FullPath.size(); i++)
		{
			FilePath[i] = FullPath[i];
		}


		HANDLE		hFileSearch = CreateFile(FilePath,			// 파일 경로와 이름 명시
			GENERIC_READ,				// 파일 접근 모드 (GENERIC_WRITE 쓰기 전용, GENERIC_READ 읽기 전용)
			NULL,						// 공유방식, 파일이 열려있는 상태에서 다른 프로세스가 오픈할 때 허용할 것인가, NULL인 경우 공유하지 않는다
			NULL,						// 보안 속성, 기본값	
			OPEN_EXISTING,				// 생성 방식, CREATE_ALWAYS는 파일이 없다면 생성, 있다면 덮어 쓰기, OPEN_EXISTING 파일이 있을 경우에면 열기
			FILE_ATTRIBUTE_NORMAL,		// 파일 속성(읽기 전용, 숨기 등), FILE_ATTRIBUTE_NORMAL 아무런 속성이 없는 일반 파일 생성
			NULL);						// 생성도리 파일의 속성을 제공할 템플릿 파일, 우리는 사용하지 않아서 NULL

		if (INVALID_HANDLE_VALUE != hFileSearch)
			m_ModelObjectList.push_back(ListName);
		else
			Safe_Delete_Array(ListName);
		CloseHandle(hFileSearch);
	}

	// 3. 파일 소멸
	CloseHandle(hFile);
}

void CAnimTool::CreateModelObject()
{
	if (m_ModelDataName.size() > 0)
	{
		const char* temp = m_ModelDataName[m_iSelectedModelData];
		string Name = temp;
		char Temp[10];
		_itoa_s(Find_ModelObjectIndex(temp), Temp, 10);
		string Index = Temp;

		string FullName = Name + Index;

		char* RealName = new char[256];
		for (int i = 0; i < 255; ++i)
		{
			if (i > FullName.size())
				break;
			RealName[i] = FullName[i];
		}
		m_ModelName.push_back(RealName);


		_tchar			szName[256] = TEXT("");

		MultiByteToWideChar(CP_ACP, 0, temp, (_uint)strlen(temp), szName, 256);

		_tchar* sztemp = new _tchar[256];

		for (int i = 0; i < 256; ++i)
		{
			sztemp[i] = szName[i];
		}

		CAnimMesh::MESHINFO* MeshInfo;
		MeshInfo = new CAnimMesh::MESHINFO;
		MeshInfo->sTag = sztemp;
		MeshInfo->fAngle = _float3{ 0.f,0.f,0.f };
		MeshInfo->fScale = _float3{ 1.f,1.f,1.f };
		MeshInfo->fPos = _float4{ 0.f,0.f,0.f,1.f };

		if (FAILED(GI->Add_GameObjectToLayer(TEXT("AnimMesh"), LEVEL_TOOL, TEXT("Layer_AnimMesh"), MeshInfo)))
		{
			Safe_Delete_Array(sztemp);
			MSG_BOX(TEXT("Failed to Create"));
			return;
		}

		Safe_Delete(MeshInfo);
		Safe_Delete(sztemp);
	}
}

void CAnimTool::ShowAnimEditWindow(_float fTimeDelta)
{
	if (m_iPreSelectedModel != m_iSelectedModel)
	{
		if (m_AnimMesh.size() > 0)
		{
			LoadAnimation();
		}	
	}
	
	m_iSelectedAnimation = m_AnimMesh[m_iSelectedModel]->Get_AnimIndex();
	if (ImGui::BeginListBox("Animationlist", ImVec2(600, 120)))
	{
		for (int i = 0; i < m_Animations.size(); i++)
		{
			const bool is_selected = (m_iSelectedAnimation == i);
			if (ImGui::Selectable(m_Animations[i], is_selected))
				m_iSelectedAnimation = i;

			if (is_selected)
				ImGui::SetItemDefaultFocus();

		}
		ImGui::EndListBox();
	}

	if (ImGui::Button("Load Animation Info"))
	{
		if(m_AnimMesh.size() > 0)
			LoadAnimation();
	}

	ImGui::SameLine();
	ImGui::Checkbox("LocalPos", &m_AnimMesh[m_iSelectedModel]->Get_bLocalPos());

	ImGui::SameLine();
	ImGui::PushItemWidth(80.f);
	ImGui::DragInt("RootIndex", &m_iRootIndex, 1, 0, 300);
	ImGui::SameLine();
	if (ImGui::Button("SetRootIndex"))
	{
		m_AnimMesh[m_iSelectedModel]->Set_RootHierarchy(m_iRootIndex);
	}
	ImGui::PopItemWidth();
	IndexFix();

	AnimFix();

	m_AnimMesh[m_iSelectedModel]->Set_Selected(true);
	
	
}

void CAnimTool::LoadAnimation()
{
	for (auto& iter : m_Animations)
	{
		Safe_Delete(iter);
	}
	m_Animations.clear();

	int Animations = m_AnimMesh[m_iSelectedModel]->Get_NumAnimations();
	for (int i = 0; i < Animations; ++i)
	{
		char temp[256] = "";
		strcpy_s(temp, m_AnimMesh[m_iSelectedModel]->Get_AnimName(i));
		char* Name = new char[256];
		for (int i = 0; i < 256; ++i)
		{
			Name[i] = temp[i];
		}

		m_Animations.push_back(Name);
	}
	m_bPlay = true;
}

void CAnimTool::IndexFix()
{
	ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "NumAnimations : %d", m_Animations.size());
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "NowIndex : %d", m_iSelectedAnimation);
	

	ImGui::SameLine();

	if (ImGui::Button("+"))
	{
		if (m_iSelectedAnimation + 1 >= m_Animations.size())
			m_iSelectedAnimation = 0;
		else
			++m_iSelectedAnimation;
	}

	ImGui::SameLine();

	if (ImGui::Button("-"))
	{
		if (m_iSelectedAnimation <= 0)
			m_iSelectedAnimation = (_int)m_Animations.size() - 1;
		else
			--m_iSelectedAnimation;
	}

	ImGui::SameLine();

	ImGui::Text("");

	ImGui::SameLine();

	if (ImGui::Button("NameIndex+"))
	{
		if (m_iSelectedAnimation + 1 < m_Animations.size())
		{			
			m_AnimMesh[m_iSelectedModel]->Change_AniIndex(m_iSelectedAnimation, m_iSelectedAnimation + 1);
			LoadAnimation();
			++m_iSelectedAnimation;
		}
	}

	ImGui::SameLine();

	if (ImGui::Button("NameIndex-"))
	{
		if (m_iSelectedAnimation > 0)
		{
			m_AnimMesh[m_iSelectedModel]->Change_AniIndex(m_iSelectedAnimation, m_iSelectedAnimation - 1);
			LoadAnimation();
			--m_iSelectedAnimation;
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("DeleteIndex"))
	{		
		if (m_Animations.size() > 0)
		{
			m_AnimMesh[m_iSelectedModel]->DeleteAnimation(m_iSelectedAnimation);
			Safe_Delete(m_Animations[m_iSelectedAnimation]);
			m_Animations.erase(m_Animations.begin() + m_iSelectedAnimation);
			LoadAnimation();
		}
	}
	
	ImGui::PushItemWidth(100.f);
	ImGui::InputInt("Index1", &m_iAniIndex1, 1);

	ImGui::SameLine();
	ImGui::InputInt("Index2", &m_iAniIndex2, 1);
	ImGui::PopItemWidth();

	ImGui::SameLine();
	if (ImGui::Button("Change Index"))
	{
		if (m_iAniIndex1 != m_iAniIndex2)
		{
			m_AnimMesh[m_iSelectedModel]->Change_AniIndex(m_iAniIndex1, m_iAniIndex2);
			LoadAnimation();
			m_iAniIndex1 = 0;
			m_iAniIndex2 = 0;		
		}
	}

	if (m_iAniIndex1 < 0)
		m_iAniIndex1 = 0;
	if (m_iAniIndex2 < 0)
		m_iAniIndex2 = 0;
	if (m_iAniIndex1 >(_int)m_Animations.size() - 1)
		m_iAniIndex1 = (_int)m_Animations.size() - 1;
	if (m_iAniIndex2 >(_int)m_Animations.size() - 1)
		m_iAniIndex2 = (_int)m_Animations.size() - 1;

	ImGui::PushItemWidth(400.f);
	static char AnimName[256] = "";
	ImGui::InputText(" ", AnimName, IM_ARRAYSIZE(AnimName));

	ImGui::SameLine();

	if (ImGui::Button("Change Name"))
	{
		if (strcmp(AnimName, ""))
		{
			if (m_Animations.size() > 0)
			{
				m_AnimMesh[m_iSelectedModel]->Set_AnimName(AnimName, m_iSelectedAnimation);
				char temp[256] = "";
				strcpy_s(AnimName, temp);
				LoadAnimation();
			}
		}
	}
	if (m_AnimMesh[m_iSelectedModel]->Get_NumAnimations() < m_iSelectedAnimation)
		m_AnimMesh[m_iSelectedModel]->Set_AnimIndex(m_AnimMesh[m_iSelectedModel]->Get_NumAnimations() - 1);
	else
		m_AnimMesh[m_iSelectedModel]->Set_AnimIndex(m_iSelectedAnimation);
}

void CAnimTool::AnimFix()
{
	if (m_Animations.size() > 0)
	{
		_float m_fDuration = m_AnimMesh[m_iSelectedModel]->Get_Duration();
		ImGui::DragFloat("Duration", &m_fDuration, 0.1f, 200.f);
		m_AnimMesh[m_iSelectedModel]->Set_Duration(m_fDuration);

		_float m_fTickPerSecond = m_AnimMesh[m_iSelectedModel]->Get_TickPerSecond();
		ImGui::DragFloat("TickPerSecond", &m_fTickPerSecond, 0.1f, 200.f);
		m_AnimMesh[m_iSelectedModel]->Set_TickPerSecond(m_fTickPerSecond);

		_float m_fPlayTime = m_AnimMesh[m_iSelectedModel]->Get_PlayTime();
		ImGui::SliderFloat("PlayTime", &m_fPlayTime, 0.1f, 200.f);
		m_AnimMesh[m_iSelectedModel]->Set_PlayTime(m_fPlayTime);

		int m_iKeyFrame = (int)m_AnimMesh[m_iSelectedModel]->Get_KeyFrame();
		ImGui::SliderInt("KeyFrame", &m_iKeyFrame, 0, 500);

		ImGui::DragFloat("PlaySpeed", &m_AnimMesh[m_iSelectedModel]->Get_PlaySpeed(), 0.01f, 0.f,1.f,"%.3f");

		m_iTimeLimitSize = (_int)m_AnimMesh[m_iSelectedModel]->Get_TimeLimit().size();
		_float* TimeLimits = new _float[m_iTimeLimitSize];
		for (int i = 0; i < m_iTimeLimitSize; ++i)
		{
			TimeLimits[i] = m_AnimMesh[m_iSelectedModel]->Get_TimeLimit(i);
			char Name[64];
			sprintf_s(Name, "TimeLimit%d", i);
			ImGui::DragFloat(Name, &TimeLimits[i], 0.1f, 200.f);
			m_AnimMesh[m_iSelectedModel]->Set_TimeLimit(TimeLimits[i], i);
		}
		Safe_Delete_Array(TimeLimits);

		if (ImGui::Button("Add TimeLimit"))
		{
			m_AnimMesh[m_iSelectedModel]->Add_TimeLimit();
		}

		ImGui::SameLine();

		if (ImGui::Button("Delete TimeLimit"))
		{
			if (m_AnimMesh[m_iSelectedModel]->Get_TimeLimit().size() > 0)
				m_AnimMesh[m_iSelectedModel]->Delete_TimeLimit();
		}

		if (ImGui::Button("   Play   "))
		{
			m_AnimMesh[m_iSelectedModel]->Set_bPlay(true);
		}

		ImGui::SameLine();

		if (ImGui::Button("   Stop   "))
		{
			m_AnimMesh[m_iSelectedModel]->Set_bPlay(false);
		}


		if (ImGui::Button("      ResetTime       "))
		{
			m_fPlayTime = 0.f;
			m_AnimMesh[m_iSelectedModel]->Set_PlayTime(m_fPlayTime);
			m_AnimMesh[m_iSelectedModel]->Reset_KeyFrame();
		}

		if (ImGui::Button("      SaveBinary      "))
		{
			m_AnimMesh[m_iSelectedModel]->SaveBinary();
		}
	}

	
}

void CAnimTool::ShowAnimModelEditWindow(_float fTimeDelta)
{
	ImGui::Begin("AnimModelListEdit");                          // Create a window called "Hello, world!" and append into it.

	ImGui::SetWindowSize(ImVec2{ 500.f, 400.f });

	ImGui::SameLine();
	
	ImGui::TextColored(ImVec4{ 1.f,1.f,0.f,1.f }, m_NowLevel.c_str());


	if (ImGui::BeginListBox(" ", ImVec2(400, 350)))
	{

		for (int i = 0; i < m_ModelDataName.size(); i++)
		{
			const bool is_selected = (m_iSelectedModelData == i);
			if (ImGui::Selectable(m_ModelDataName[i], is_selected))
				m_iSelectedModelData = i;

			if (is_selected)
				ImGui::SetItemDefaultFocus();

		}
		ImGui::EndListBox();
	}


	ImGui::End();
}

void CAnimTool::Load_DataList()
{
	for (auto& iter : *(DM->Get_AnimFileNames()))
	{
		m_ModelDataName.push_back(iter.data());
		
		m_ModelObjectIndex.emplace(iter, 1);
		
	}
	
	 
	
}


void CAnimTool::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	/*for (auto& iter : m_ModelDataName)
	{
		delete(iter);
	}
	m_ModelDataName.clear();*/

	for (auto& iter : m_ModelData)
	{
		delete iter.ProtoName;
		delete iter.ModelName;
	}

	m_ModelData.clear();

	m_ModelObjectIndex.clear();

	for (auto& iter : m_ModelName)
	{
	delete(iter);
	}
	m_ModelName.clear();

	for (auto& iter : m_ModelObjectList)
	{
		Safe_Delete_Array(iter);
	}
	m_ModelObjectList.clear();

	for (auto& iter : m_AnimMesh)
	{
		Safe_Release(iter);
	}
	m_AnimMesh.clear();

	for (auto& iter : m_Animations)
	{
		Safe_Delete(iter);
	}
	m_Animations.clear();
}
