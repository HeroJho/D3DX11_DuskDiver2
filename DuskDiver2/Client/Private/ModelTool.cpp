#include "stdafx.h"
#include "..\Public\ModelTool.h"
#include "Model.h"
#include "Mesh.h"
#include "GameInstance.h"
#include "ToolManager.h"
#include "DataManager.h"
#include "ToolManager.h"

CModelTool::CModelTool(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

void CModelTool::Initialize()
{
	Load_DataList();
}

void CModelTool::ShowModelWindow(_float fTimeDelta)
{

	ImGui::TextColored(ImVec4{ 1.f,0.5f,0.f,1.f }, m_NowLevels.c_str());

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
		CreateModelObject(_float4{ 0.f,0.f,0.f,1.f });
	}

	ImGui::SameLine();

	if (ImGui::Button("Delete Model"))
	{
		if (m_Meshes.size() > 0 && m_iSelectedModel < m_Meshes.size())
			DeleteObject();
	}

	ImGui::SameLine();
	if (m_Meshes.size() > 0)
	{	
		ImGui::Checkbox("Turn", &(m_Meshes[m_iSelectedModel]->Get_bTurn()));
	}

	ImGui::SameLine();

	if (m_Meshes.size() > 0)
	{
		ImGui::Checkbox("UVFix", &m_Meshes[m_iSelectedModel]->Get_UVFix());
	}

	ImGui::SameLine();

	if (m_Meshes.size() > 0)
	{
		ImGui::Checkbox("FixMode", &(m_Meshes[m_iSelectedModel]->Get_FixPosScale()));
	}

	ImGui::SameLine();

	if (m_Meshes.size() > 0)
	{
		ImGui::Checkbox("FixColor", &(m_Meshes[m_iSelectedModel]->Get_FixColor()));
	}

	ImGui::SameLine();	

	if (m_Meshes.size() > 0)
	{
		if (ImGui::Button("Reset Infos"))
		{
			for (int i = 0; i < m_Meshes.size(); ++i)
			{
				m_Meshes[i]->Reset_Scale();
				m_Meshes[i]->Reset_Ratio();
				m_Meshes[i]->Reset_Pos();
			}
		}
	}

	if (m_Meshes.size() > 0)
	{
		ModelFix(fTimeDelta);
	}

	
	
}

int CModelTool::Find_ModelObjectIndex(string sTag)
{
	for (auto& iter : m_ModelObjectIndex)
	{
		if (iter.first == sTag)
			return iter.second++;
	}
	return 0;
}

bool CModelTool::FInd_ModelObjectMap(string sTag)
{
	for (auto& iter : m_ModelObjectIndex)
	{
		if (iter.first == sTag)
			return true;
	}
	return false;
}

void CModelTool::ModelFix(_float fTimeDelta)
{

	ImGui::SliderInt("PassNum", &(m_Meshes[m_iSelectedModel]->Get_iShaderPass()), 0, 10);
	ImGui::DragFloat("UVMaxIndexX", &(m_Meshes[m_iSelectedModel]->Get_fMaxUVIndexX()), 1.f, 0.f, 100.0f);
	ImGui::DragFloat("UVMaxIndexY", &(m_Meshes[m_iSelectedModel]->Get_fMaxUVIndexY()), 1.f, 0.f, 100.0f);
	ImGui::DragFloat("UVSpeed", &(m_Meshes[m_iSelectedModel]->Get_fUVSpeed()), 0.01f, 0.f, 100.0f);
		
	ImGui::DragFloat2("UVSpeed", (float*)&m_Meshes[m_iSelectedModel]->Get_fUVFixSpeed(), 0.001f, -10.f, 10.f, "%.3f");
	ImGui::SameLine();
	if (ImGui::Button("ResetUV"))
	{
		m_Meshes[m_iSelectedModel]->Reset_UVFixAcc();
	}

	if (m_Meshes[m_iSelectedModel]->Get_FixColor())
	{
		ImGui::DragFloat4("Color", (float*)&m_Meshes[m_iSelectedModel]->Get_Color(), 0.001f, 0.f, 1.f, "%.3f");
		ImGui::DragFloat4("Color2", (float*)&m_Meshes[m_iSelectedModel]->Get_Color2(), 0.001f, 0.f, 1.f, "%.3f");
		ImGui::DragFloat2("Ratio", (float*)&m_Meshes[m_iSelectedModel]->Get_Ratio(), 0.001f, -10.f, 10.f, "%.3f");
		ImGui::DragFloat2("RatioSpeed", (float*)&m_Meshes[m_iSelectedModel]->Get_RatioSpeed(), 0.001f, -30.f, 30.f, "%.3f");
		ImGui::SameLine();
		if (ImGui::Button("ResetRatio"))
		{
			m_Meshes[m_iSelectedModel]->Reset_Ratio();
		}
		ImGui::DragFloat("AlphaSpeed", &m_Meshes[m_iSelectedModel]->Get_AlphaSpeed(), 0.001f, -10.f, 10.f, "%.3f");
	}

	ImGui::DragFloat("BlurPower", &m_Meshes[m_iSelectedModel]->Get_BlurPower(), 0.001f, 0.f, 10.f, "%.3f");

	if (m_ModelName.size() > m_iSelectedModel)
	{
		ImGui::DragFloat3("PosSpeed", (float*)&m_Meshes[m_iSelectedModel]->Get_PosFixSpeed(), 0.001f, -50.f, 50.f, "%.3f");
		ImGui::SameLine();
		if (ImGui::Button("ResetPos"))
		{
			m_Meshes[m_iSelectedModel]->Reset_Pos();
		}
		ImGui::DragFloat3("ScaleSpeed", (float*)&m_Meshes[m_iSelectedModel]->Get_ScaleFixSpeed(), 0.001f, -50.f, 50.f, "%.3f");
		ImGui::SameLine();
		if (ImGui::Button("ResetScale"))
		{
			m_Meshes[m_iSelectedModel]->Reset_Scale();
		}

		_float3 Pos = m_Meshes[m_iSelectedModel]->Get_Pos();
		m_fPosX = Pos.x;
		ImGui::DragFloat("PosX", &m_fPosX, 0.01f, -1000.0f, 1000.0f);
		ImGui::SameLine();
		if (ImGui::Button("Reset Scale,Ratio"))
		{
			m_Meshes[m_iSelectedModel]->Reset_Scale();
			m_Meshes[m_iSelectedModel]->Reset_Ratio();
		}

		m_fPosY = Pos.y;
		ImGui::DragFloat("PosY", &m_fPosY, 0.01f, -1000.0f, 1000.0f);

		m_fPosZ = Pos.z;
		ImGui::DragFloat("PosZ", &m_fPosZ, 0.01f, -1000.0f, 1000.0f);
		m_Meshes[m_iSelectedModel]->Set_Pos(_float4{ m_fPosX, m_fPosY, m_fPosZ, 1.f });

		m_fSizeX = m_Meshes[m_iSelectedModel]->Get_Scale().x;
		ImGui::DragFloat("ScaleX", &m_fSizeX, 0.01f, -1.f, 100.0f);
		if(m_fSizeX > 0.f)
		m_Meshes[m_iSelectedModel]->Set_ScaleX(_float3{ m_fSizeX,m_fSizeX,m_fSizeX });

		m_fSizeY = m_Meshes[m_iSelectedModel]->Get_Scale().y;
		ImGui::DragFloat("ScaleY", &m_fSizeY, 0.01f, -1.f, 100.0f);
		if (m_fSizeY > 0.f)
		m_Meshes[m_iSelectedModel]->Set_ScaleY(_float3{ m_fSizeY,m_fSizeY,m_fSizeY });

		m_fSizeZ = m_Meshes[m_iSelectedModel]->Get_Scale().z;
		ImGui::DragFloat("ScaleZ", &m_fSizeZ, 0.01f, -1.f, 100.0f);
		if (m_fSizeZ > 0.f)
		m_Meshes[m_iSelectedModel]->Set_ScaleZ(_float3{ m_fSizeZ,m_fSizeZ,m_fSizeZ });
			
		

		if (!m_Meshes[m_iSelectedModel]->Get_bTurn() && !m_Meshes[m_iSelectedModel]->Get_bTurnStop())
		{
			_float3 Angle = m_Meshes[m_iSelectedModel]->Get_Angles();
			m_fAngleX = Angle.x;
			ImGui::DragFloat("AngleX", &m_fAngleX, 0.f, 0.f, 360.f);

			m_fAngleY = Angle.y;
			ImGui::DragFloat("AngleY", &m_fAngleY, 0.f, 0.f, 360.f);

			m_fAngleZ = Angle.z;
			ImGui::DragFloat("AngleZ", &m_fAngleZ, 0.f, 0.f, 360.f);
			m_Meshes[m_iSelectedModel]->Rotation(_float3{ 1.f,0.f,0.f }, m_fAngleX, _float3{ 0.f,1.f,0.f }, m_fAngleY, _float3{ 0.f,0.f,1.f }, m_fAngleZ);
		}

		else
		{
			ImGui::Checkbox("StopTurn", &(m_Meshes[m_iSelectedModel]->Get_bTurnStop()));
			ImGui::SameLine();
			ImGui::Checkbox("TurnFront", &(m_Meshes[m_iSelectedModel]->Get_bTurnDir()));
			ImGui::SameLine();
			ImGui::PushItemWidth(100.f);
			ImGui::DragFloat("TurnSpeed", &(m_Meshes[m_iSelectedModel]->Get_TurnSpeed()), 0.01f, 0.f, 100.f);
			
			ImGui::PopItemWidth();
		}

		ImGui::DragFloat3("OriginScale", (float*)&m_Meshes[m_iSelectedModel]->Get_OriginScale(), 0.001f, -50.f, 50.f, "%.3f");
		ImGui::DragFloat4("OriginPos", (float*)&m_Meshes[m_iSelectedModel]->Get_OriginPos(), 0.01f, -50.f, 50.f, "%.3f");
		ImGui::DragFloat2("OriginRatio", (float*)&m_Meshes[m_iSelectedModel]->Get_OriginRatio(), 0.001f, 0.f, 50.f, "%.3f");

		_float4x4 Matrix = m_Meshes[m_iSelectedModel]->Get_Matrix();
		ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Right    : ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), " %f , ", Matrix._11);
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), " %f , ", Matrix._12);
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), " %f , ", Matrix._13);

		ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "UP       : ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), " %f , ", Matrix._21);
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), " %f , ", Matrix._22);
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), " %f , ", Matrix._23);

		ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Look     : ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), " %f , ", Matrix._31);
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), " %f , ", Matrix._32);
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), " %f , ", Matrix._33);

		ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Position : ");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), " %f , ", Matrix._41);
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), " %f , ", Matrix._42);
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), " %f , ", Matrix._43);
		
	}
}

void CModelTool::DeleteObject()
{
	m_Meshes[m_iSelectedModel]->Set_Dead();
	Safe_Release(m_Meshes[m_iSelectedModel]);
	m_Meshes.erase(m_Meshes.begin() + m_iSelectedModel);
	delete m_ModelName[m_iSelectedModel];
	m_ModelName.erase(m_ModelName.begin() + m_iSelectedModel);

	if (m_Meshes.size() > 0)
		m_iSelectedModel -= 1;
	if(m_iSelectedModel <= 0)
		m_iSelectedModel = 0;
}

void CModelTool::ShowModelEditWindow(_float fTimeDelta)
{
	ImGui::Begin("ModelListEdit");                          // Create a window called "Hello, world!" and append into it.

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

void CModelTool::Load_DataList()
{
	int Index = 0;
	for (auto& iter : *(DM->Get_NonAnimFileNames()))
	{
		m_ModelDataName.push_back("");
		m_ModelDataName[Index] = iter.data();

		m_ModelObjectIndex.emplace(iter, 1);

		++Index;
	}
}

HRESULT CModelTool::CreateModelObject(_float4 fPos)
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

		CMesh::MESHINFO* MeshInfo;
		MeshInfo = new CMesh::MESHINFO;
		MeshInfo->sTag = sztemp;
		MeshInfo->fAngle = _float3{ 0.f,0.f,0.f };
		MeshInfo->fScale = _float3{ 1.f,1.f,1.f };
		MeshInfo->fPos = _float4{ 0.f,0.f,0.f,1.f };

		if (FAILED(GI->Add_GameObjectToLayer(TEXT("Mesh"), LEVEL_TOOL, TEXT("Layer_Mesh"), MeshInfo)))
		{
			Safe_Delete_Array(sztemp);
			MSG_BOX(TEXT("Failed to Create"));
			return E_FAIL;
		}

		Safe_Delete(MeshInfo);
		Safe_Delete(sztemp);
	}
	return S_OK;
	
}

void CModelTool::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	for (auto& iter : m_ModelName)
	{
		delete(iter);
	}
	m_ModelName.clear();

	for (auto& iter : m_ModelData)
	{
		delete iter.ProtoName;
		delete iter.ModelName;
	}

	m_ModelData.clear();

	m_ModelObjectIndex.clear();


	for (auto& iter : m_Meshes)
	{
		Safe_Release(iter);
	}
	m_Meshes.clear();

}
