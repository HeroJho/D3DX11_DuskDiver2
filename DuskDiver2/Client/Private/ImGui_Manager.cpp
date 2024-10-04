#include "../Default/stdafx.h"
#include "..\Public\ImGui_Manager.h"
#include "GameInstance.h"
#include <DirectXTK\DDSTextureLoader.h>
#include <DirectXTK\WICTextureLoader.h>
#include "DataManager.h"
#include "ToolManager.h"
#include "CameraTool.h"
#include "ModelTool.h"
#include "Mesh.h"
#include "AnimTool.h"
#include "AnimMesh.h"
#include "Renderer.h"
#include "ParticleTool.h"
#include "ParticleInstanceTool.h"
#include "Camera_Manager.h"
#include "Layer.h"


IMPLEMENT_SINGLETON(CImGui_Manager)

CImGui_Manager::CImGui_Manager()
	:m_bShow_Test_Window(false)
{
}


HRESULT CImGui_Manager::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();


	ImGuiIO& io = ImGui::GetIO(); (void)io;
	temp = io.ConfigFlags;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Dear ImGui style
	ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(pDevice,pContext);

	m_TabName.push_back("MainWindow");
	m_TabName.push_back("CameraTool");
	m_TabName.push_back("ModelTool");
	m_TabName.push_back("AnimTool");
	m_TabName.push_back("ParticleTool");
	m_TabName.push_back("ParticleInstanceTool");

	m_pCameraTool = new CCameraTool(m_pDevice, m_pContext);
	m_pCameraTool->Initalize();

	m_pModelTool = new CModelTool(m_pDevice, m_pContext);
	m_pModelTool->Initialize();

	m_pAnimTool = new CAnimTool(m_pDevice, m_pContext);
	m_pAnimTool->Initialize();

	m_pParticleTool = new CParticleTool(m_pDevice, m_pContext);
	m_pParticleTool->Initialize();

	m_pParticleInstanceTool = new CParticleInstanceTool(m_pDevice, m_pContext);
	m_pParticleInstanceTool->Initialize();

	return S_OK;

}

void CImGui_Manager::Tick(_float fTimeDelta)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// 상시 받는 입력입니다.
	Get_KeyInput_Static();

	// 디버그 모드입니다.
	if (m_bDebug)
	{
		Render_Debug();
		Render_Select(fTimeDelta);
	}

	// 특정 레벨 일때만 출력입니다.
	LEVEL eLevel = (LEVEL)GI->Get_Cur_LevelIndex();
	if (LEVEL_TOOL == eLevel)  
		Render_Tool(fTimeDelta);
	else if (LEVEL_LOGO == eLevel)
		Render_Select(fTimeDelta);

	ImGui::EndFrame();
}

void CImGui_Manager::Render()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void CImGui_Manager::Get_KeyInput_Static()
{

	if (K_DOWN(DIK_F1))
		m_bDebug = !m_bDebug;
	

}

void CImGui_Manager::Get_KeyInput_Tool()
{

	if (m_bKeyInput)
	{
		switch (m_eTool)
		{
		case Client::CImGui_Manager::Tool_Main:
			break;
		case Client::CImGui_Manager::Tool_Camera:
			break;
		case Client::CImGui_Manager::Tool_Model:
			break;
		case Client::CImGui_Manager::Tool_AnimModel:
			break;
		case Client::CImGui_Manager::Tool_Particle:
			break;
		case Client::CImGui_Manager::Tool_ParticleInstance:
			break;
		case Client::CImGui_Manager::Tool_END:
			break;
		default:
			break;
		}
	}

}

void CImGui_Manager::Render_Tool(_float fTimeDelta)
{
	m_fTimeDelta = fTimeDelta;

	Get_KeyInput_Tool();

	if (m_bMainWindow)
		MainWindow(fTimeDelta);

	/*if (m_bExampleWindow)
	ShowExampleWindow();*/

	if (m_bModelEditWindow)
		m_pModelTool->ShowModelEditWindow(fTimeDelta);

	if (m_bAnimModelEditWindow)
		m_pAnimTool->ShowAnimModelEditWindow(fTimeDelta);

	if (m_bParticleInfoWindow)
		m_pParticleTool->ShowInfoWindow(fTimeDelta);

	if (m_bParticleInstanceInfoWindow)
		m_pParticleInstanceTool->ShowInfoWindow(fTimeDelta);
	
}

void CImGui_Manager::Render_Select(_float fTimeDelta)
{
	ImGui::Begin("Debug ^-^d ^m^!");
	ImGui::SetWindowSize(ImVec2{ 300.f, 250.f });
	ImVec2 vSize{ 47.f, 17.f };
	ImGui::TextColored(ImVec4(9.f, 0.f, 0.f, 1.f), "Frame: ");  ImGui::SameLine(); ImGui::Text("%d", m_iFrame);
	ImGui::SameLine();
	if (ImGui::Button("Limit", vSize))
		m_bChangeLimit = !m_bChangeLimit;


	ImGui::Spacing();
	ImGui::TextColored(ImVec4(9.f, 0.f, 0.f, 1.f), "InputMode :");
	ImGui::SameLine();
	if (GI->Get_DebugInput() == 0)
	{
		m_sInputMode = "Client";
		ImGui::TextColored(ImVec4{ 1.f,1.f,0.f,1.f }, m_sInputMode.data());
	}
	else if (GI->Get_DebugInput() == 1)
	{
		m_sInputMode = "Tool";
		ImGui::TextColored(ImVec4{ 0.f,1.f,0.f,1.f }, m_sInputMode.data());
	}


	ImGui::TextColored(ImVec4(9.f, 0.f, 0.f, 1.f), "CamMode :");
	ImGui::SameLine();
	if (CRM->Get_CamMode() == CCamera_Main::CAM_PLAYER)
	{
		m_sInputMode = "Player";
		ImGui::TextColored(ImVec4{ 1.f,1.f,0.f,1.f }, m_sInputMode.data());
	}
	else if (CRM->Get_CamMode() == CCamera_Main::CAM_DEBUG)
	{
		m_sInputMode = "Debug";
		ImGui::TextColored(ImVec4{ 0.f,1.f,0.f,1.f }, m_sInputMode.data());
	}

	ImGui::Spacing();
	ImGui::TextColored(ImVec4(9.f, 0.f, 0.f, 1.f), "Level: ");
	if (ImGui::Button("Stage1", vSize)) 
		TM->Change_Level(LEVEL_STAGE1);  ImGui::SameLine();
	if (ImGui::Button("Stage2", vSize))
		TM->Change_Level(LEVEL_STAGE2);  ImGui::SameLine();
	if (ImGui::Button("Stage3", vSize))  
		TM->Change_Level(LEVEL_STAGE3);  ImGui::SameLine();
	if (ImGui::Button("Stage4", vSize))
		TM->Change_Level(LEVEL_STAGE4);
	if (ImGui::Button("Stage5", vSize))  
		TM->Change_Level(LEVEL_STAGE5);  ImGui::SameLine();
	if (ImGui::Button("Tool", vSize))    
		TM->Change_Level(LEVEL_TOOL);    ImGui::SameLine();
	if (ImGui::Button("Test", vSize))  
		TM->Change_Level(LEVEL_TEST);


	ImGui::Spacing();
	ImGui::TextColored(ImVec4(9.f, 0.f, 0.f, 1.f), "Render_Debug: ");
	vSize = { 100.f, 17.f };
	if (ImGui::Button("Render_Col", vSize))
	{
		m_bColMode = !m_bColMode;
		m_pRenderer->Set_RenderColMode(m_bColMode);
	}
	ImGui::SameLine();
	if (ImGui::Button("Render_Target", vSize))
	{
		m_bRenderTarget = !m_bRenderTarget;
		m_pRenderer->Set_RenderTargetMode(m_bRenderTarget);
	}


	ImGui::End();
}

void CImGui_Manager::MainWindow(_float fTimeDelta)
{
	

	// TransformWindow
	
	ImGui::Begin("ImGuiAHSsss");                          // Create a window called "Hello, world!" and append into it.

	

	if (ImGui::Button("WinSizeUp"))
	{
		m_fWinSizeX += 70.f;
		m_fWinSizeY += 50.f;
	}

	ImGui::SameLine();
	if (ImGui::Button("WinSizeDown"))
	{
		m_fWinSizeX -= 70.f;
		m_fWinSizeY -= 50.f;
	}

	ImGui::SetWindowSize(ImVec2{ m_fWinSizeX, m_fWinSizeY });

	ImGui::Checkbox("KeyInput", &m_bKeyInput);
	ImGui::SameLine();
	ImGui::Checkbox("Picking", &m_bPicking);
	ImGui::SameLine();
	ImGui::Checkbox("RenderTarget", &m_bRenderTarget);
	ImGui::SameLine();
	ImGui::Text("  ");


	ImGui::SameLine();
	if (ImGui::Button("Binary All"))
	{
		DM->Conv_Bin_Model_All();
	}
	//m_pRenderer->Set_bRenderTarget(m_bRenderTarget);
	
	static ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_AutoSelectNewTabs | ImGuiTabBarFlags_Reorderable | ImGuiTabBarFlags_FittingPolicyResizeDown;
	
	if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags))
	{
		if (ImGui::BeginTabItem("MainWindow"))
		{
			m_eTool = Tool_Main;
			ChangeTool();		
			ShowExampleWindow(fTimeDelta);
			ImGui::EndTabItem();
		}
		

		if (ImGui::BeginTabItem("CameraTool"))
		{
			m_eTool = Tool_Camera;
			ChangeTool();
			ImGui::TextColored(ImVec4{ 0.5f,1.f,0.3f,1.f }, "YouMin Hi");
			//m_pCameraTool->ShowCameraWindow(fTimeDelta);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("ModelTool"))
		{
			m_eTool = Tool_Model;
			ChangeTool();
			m_pModelTool->ShowModelWindow(fTimeDelta);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("AnimTool"))
		{
			m_eTool = Tool_AnimModel;
			ChangeTool();		
			m_pAnimTool->ShowAnimWindow(fTimeDelta);
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("ParticleTool"))
		{
			m_eTool = Tool_Particle;
			ChangeTool();
			m_pParticleTool->ShowParticleToolWindow(fTimeDelta);
			ImGui::EndTabItem();
		}
		
		if (ImGui::BeginTabItem("ParticleInstanceTool"))
		{
			m_eTool = Tool_ParticleInstance;
			ChangeTool();
			m_pParticleInstanceTool->ShowParticleToolWindow(fTimeDelta);
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();

		
	}

	ImGui::End();

}

void CImGui_Manager::ShowExampleWindow(_float fTimeDelta)
{
	
	ImGui::Text("Example");

	if (ImGui::Button("CheckBoxButton"))
	{
		m_bTest = !m_bTest;
	}

	ImGui::SameLine();
	ImGui::Checkbox("CheckBox", &m_bTest);

	ImGui::ColorEdit3("Color List", (float*)&m_ColorEdit);

	if (ImGui::Button("ChangeTextColor"))
	{
		m_TextColor.x = m_ColorEdit.x;
		m_TextColor.y = m_ColorEdit.y;
		m_TextColor.z = m_ColorEdit.z;
	}

	ImGui::SameLine();

	if (ImGui::Button("ChangeBackBufferColor"))
	{
		m_fBackBufferColor.x = m_ColorEdit.x;
		m_fBackBufferColor.y = m_ColorEdit.y;
		m_fBackBufferColor.z = m_ColorEdit.z;
	}

	ImGui::SameLine();

	if (ImGui::Button("ChangeButtonColor"))
	{
		m_ButtonColor.x = m_ColorEdit.x;
		m_ButtonColor.y = m_ColorEdit.y;
		m_ButtonColor.z = m_ColorEdit.z;
	}


	ImGui::TextColored(m_TextColor, "TextColor");

	ImGui::SameLine();
	ImGui::ColorButton("ButtonColor", m_ButtonColor);

	static int iRadio = 1;
	ImGui::RadioButton("Radio 1", &iRadio, 1); ImGui::SameLine();
	ImGui::RadioButton("Radio 2", &iRadio, 2); ImGui::SameLine();
	ImGui::RadioButton("Radio 3", &iRadio, 3);

	ImGui::Text("RadioSelected : %d", iRadio);

	const char* ComboBoxItem[] = { "Item1", "Item2", "Item3", "Item4", "Item5" };
	static int ItemIndex = 0;
	ImGui::Combo("ComboBox", &ItemIndex, ComboBoxItem, IM_ARRAYSIZE(ComboBoxItem));

	static char Inputstr[128] = "Typing Here!";
	ImGui::InputText("Input text", Inputstr, IM_ARRAYSIZE(Inputstr));

	ImGui::Text(Inputstr);


	static int DragNum = 0;
	ImGui::DragInt("Drag int", &DragNum, 1);

	static int SliderNum = 0;
	ImGui::SliderInt("Slider int", &SliderNum, 0, 100);

	if (ImGui::Button("Close Me"))
		m_bExampleWindow = false;

}


void CImGui_Manager::AddModelObject(CMesh * pMesh)
{
	m_pModelTool->AddModelObject(pMesh);
}

void CImGui_Manager::Create_Model_Object(_float4 fPos)
{
	 m_pModelTool->CreateModelObject(fPos);
}

int CImGui_Manager::Get_ShaderPass()
{
	return m_pModelTool->Get_ShaderPass();
}

_float CImGui_Manager::Get_MaxUVIndexX()
{
	return m_pModelTool->Get_MaxUVIndexX();
}

_float CImGui_Manager::Get_MaxUVIndexY()
{
	return m_pModelTool->Get_MaxUVIndexY();
}

_float CImGui_Manager::Get_UVSpeed()
{
	return m_pModelTool->Get_UVSpeed();
}

void CImGui_Manager::Add_Mesh(CMesh * pMesh)
{
	m_pModelTool->Add_Mesh(pMesh);
}

void CImGui_Manager::AddAnimMesh(CAnimMesh * pAnimMesh)
{
	m_pAnimTool->AddAnimMesh(pAnimMesh);
}

bool CImGui_Manager::GetAnimPlay()
{
	return m_pAnimTool->GetPlay();
}

int CImGui_Manager::Get_AniIndex()
{
	return m_pAnimTool->Get_AniIndex();
}

void CImGui_Manager::ChangeTool()
{
	switch (m_eTool)
	{
	case Client::CImGui_Manager::Tool_Main:
		m_bToolCamera = false;
		m_bModelEditWindow = false;
		m_bAnimModelEditWindow = false;
		m_bAnimTool = false;
		m_bParticleTool = false;
		m_bParticleInfoWindow = false;
		m_bParticleInstanceTool = false;
		m_bParticleInstanceInfoWindow = false;
		break;

	case Client::CImGui_Manager::Tool_Camera:
		m_bToolCamera = true;	
		m_bModelEditWindow = false;
		m_bAnimTool = false;
		m_bAnimModelEditWindow = false;
		m_bParticleTool = false;
		m_bParticleInfoWindow = false;
		m_bParticleInstanceTool = false;
		m_bParticleInstanceInfoWindow = false;
		break;

	case Client::CImGui_Manager::Tool_Model:
		m_bToolCamera = false;
		m_bModelEditWindow = true;
		m_bAnimModelEditWindow = false;
		m_bAnimTool = false;
		m_bParticleTool = false;
		m_bParticleInfoWindow = false;
		m_bParticleInstanceTool = false;
		m_bParticleInstanceInfoWindow = false;
		break;

	case Client::CImGui_Manager::Tool_AnimModel:
		m_bToolCamera = false;
		m_bModelEditWindow = false;
		m_bAnimTool = true;
		m_bAnimModelEditWindow = true;
		m_bParticleTool = false;
		m_bParticleInfoWindow = false;
		m_bParticleInstanceTool = false;
		m_bParticleInstanceInfoWindow = false;
		break;

	case Client::CImGui_Manager::Tool_Particle:
		m_bToolCamera = false;
		m_bModelEditWindow = false;
		m_bAnimTool = false;
		m_bAnimModelEditWindow = false;
		m_bParticleTool = true;
		m_bParticleInfoWindow = true;
		m_bParticleInstanceTool = false;
		m_bParticleInstanceInfoWindow = false;
		break;

	case Client::CImGui_Manager::Tool_ParticleInstance:
		m_bToolCamera = false;
		m_bModelEditWindow = false;
		m_bAnimTool = false;
		m_bAnimModelEditWindow = false;
		m_bParticleTool = false;
		m_bParticleInfoWindow = false;
		m_bParticleInstanceTool = true;
		m_bParticleInstanceInfoWindow = true;
		break;
	}
}

void CImGui_Manager::Add_Renderer(CRenderer * pRenderer)
{
	m_pRenderer = pRenderer;
	Safe_AddRef(m_pRenderer);
}

void CImGui_Manager::Render_Debug()
{
	ImGui::Begin("Layers");
	ImGui::SetWindowSize(ImVec2{ 300.f, 270.f });
	const map<const _tchar*, class CLayer*>* pLayers = GI->Get_Layers(GI->Get_Cur_LevelIndex());
	const map<const _tchar*, class CLayer*>* pStaticLayers = GI->Get_Layers(LEVEL_STATIC);

	ImVec2 vSize{ 150.f, 200.f };
	if (ImGui::BeginListBox("Layers", vSize))
	{
		for (auto& Pair : *pStaticLayers)
		{
			char sTemp[MAX_PATH];
			TM->TCtoC(Pair.first, sTemp);
			ImVec2 vSize{ 100.f, 10.f };
			bool isSelected = !strcmp(m_sSelectedLayer, sTemp);
			if (ImGui::Selectable(sTemp, isSelected, 0, vSize))
				strcpy(m_sSelectedLayer, sTemp);

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}

		for (auto& Pair : *pLayers)
		{
			char sTemp[MAX_PATH];
			TM->TCtoC(Pair.first, sTemp);
			ImVec2 vSize{ 100.f, 10.f };
			bool isSelected = !strcmp(m_sSelectedLayer, sTemp);
			if (ImGui::Selectable(sTemp, isSelected, 0, vSize))
				strcpy(m_sSelectedLayer, sTemp);

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}


		ImGui::EndListBox();
	}

	ImGui::End();



	ImGui::Begin("Objects");
	ImGui::SetWindowSize(ImVec2{ 300.f, 270.f });
	// =========  Layer Obj =========
	if (!strcmp("", m_sSelectedLayer))
	{
		ImGui::End();
		return;
	}

	TCHAR sTTemp[MAX_PATH];
	TM->CtoTC(m_sSelectedLayer, sTTemp);

	auto	iter = find_if(pLayers->begin(), pLayers->end(), CTag_Finder(sTTemp));
	if (iter == pLayers->end())
	{
		iter = find_if(pStaticLayers->begin(), pStaticLayers->end(), CTag_Finder(sTTemp));
		if (iter == pStaticLayers->end())
		{
			ImGui::End();
			return;
		}
	}

	CLayer* pLayer = iter->second;


	const list<CGameObject*>* pObjects = pLayer->Get_GameObjects();
	vSize = { 200.f, 200.f };
	if (ImGui::BeginListBox("Objects", vSize))
	{
		for (auto& pObject : *pObjects)
		{
			if ("" == pObject->Get_ImGuiTag())
				continue;

			ImVec2 vSize{ 300.f, 10.f };
			bool isSelected = m_sSelectedObj == pObject->Get_ImGuiTag();
			if (ImGui::Selectable(pObject->Get_ImGuiTag().data(), isSelected, 0, vSize))
			{
				m_sSelectedObj = pObject->Get_ImGuiTag();



				if (m_pSelectedObject)
				{
					if (!m_pSelectedObject->Get_Dead())
					{
						m_pSelectedObject->Set_Selected(false);

						CTransform* pTran = (CTransform*)m_pSelectedObject->Get_ComponentPtr(TEXT("Com_Transform"));
						_float3 vScale = pTran->Get_Scale();
						vScale.x = m_fX;
						vScale.y = m_fY;
						pTran->Set_Scale(XMLoadFloat3(&vScale));
					}
					else
						m_pSelectedObject = nullptr;


				}

				m_pSelectedObject = pObject;
				m_pSelectedObject->Set_Selected(true);

				// 선택됐을 때 크기 줄였다 표시를 위한 작업 
				if ("Tag_Cam" != m_pSelectedObject->Get_Tag())
				{
					CTransform* pTran = (CTransform*)m_pSelectedObject->Get_ComponentPtr(TEXT("Com_Transform"));
					_float3 vPos = pTran->Get_Scale();
					m_fX = vPos.x; m_fMaxX = m_fX * 2.f;
					m_fY = vPos.y;
				}
			}

			if (isSelected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndListBox();
	}
	if (ImGui::Button("Show_SelectedObj"))
	{
		if (m_pSelectedObject)
		{
			m_bShowSelectedObj = true;
			CTransform* pTran = (CTransform*)m_pSelectedObject->Get_ComponentPtr(TEXT("Com_Transform"));
			_float3 vScale = pTran->Get_Scale();
			vScale.x = m_fX;
			vScale.y = m_fY;
			pTran->Set_Scale(XMLoadFloat3(&vScale));
		}
		else
			m_bShowSelectedObj = false;

		m_bSelectedObj = !m_bSelectedObj;
	}



	ImGui::End();



	if (m_pSelectedObject && "" != m_sSelectedObj)
	{
		if (m_pSelectedObject->Get_Dead())
			return;
		m_pSelectedObject->Render_Debug();

		if (m_bSelectedObj && "Tag_Cam" != m_pSelectedObject->Get_Tag())
		{
			CTransform* pTran = (CTransform*)m_pSelectedObject->Get_ComponentPtr(TEXT("Com_Transform"));
			Debug_SelectedObj(pTran);
		}
	}


	
}

void CImGui_Manager::Reset_DebugData()
{
	ZeroMemory(m_sSelectedLayer, sizeof(char) * MAX_PATH);
	m_sSelectedObj = "";
	if (m_pSelectedObject)
	{
		m_pSelectedObject->Set_Selected(false);
		m_bShowSelectedObj = false;
	}

	m_pSelectedObject = nullptr;
}

void CImGui_Manager::Debug_Transform(CTransform * pTran)
{
	ImGui::Text("===== World Matrix =====");

	_float3 vScale = pTran->Get_Scale();
	ImGui::Text("Scale: %.2f %.2f %.2f", vScale.x, vScale.y, vScale.z);

	_float4x4 mWorld = pTran->Get_WorldFloat4x4();
	ImGui::Text("Right: %.2f %.2f %.2f %.2f", mWorld._11, mWorld._12, mWorld._13, mWorld._14);
	ImGui::Text("Up   : %.2f %.2f %.2f %.2f", mWorld._21, mWorld._22, mWorld._23, mWorld._24);
	ImGui::Text("Look : %.2f %.2f %.2f %.2f", mWorld._31, mWorld._32, mWorld._33, mWorld._34);

	_float4 vPos; XMStoreFloat4(&vPos, pTran->Get_State(CTransform::STATE_POSITION));
	if (ImGui::DragFloat4("Pos", (float*)&vPos, 0.1f))
		pTran->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPos));


	_float3 vRot = pTran->Get_Rotation_Debug();
	if (ImGui::DragFloat4("Rot", (float*)&vRot))
	{
		pTran->Set_Rotation_Debug(vRot);
		pTran->RotationThree(_float3(1.f, 0.f, 0.f), vRot.x, _float3(0.f, 1.f, 0.f), vRot.y, _float3(0.f, 0.f, 1.f), vRot.z);
	}

	_float3 vScale_Debug = pTran->Get_Scale_Debug();
	if (ImGui::DragFloat4("Scale", (float*)&vScale_Debug))
	{
		if (0.01f < vScale_Debug.x && 0.01f < vScale_Debug.y && 0.01f < vScale_Debug.z)
		{
			pTran->Set_Scale_Debug(vScale_Debug);
			pTran->Set_Scale(XMVectorSetW(XMLoadFloat3(&vScale_Debug), 1.f));
		}
	}
}

void CImGui_Manager::Debug_SelectedObj(CTransform * pTran)
{
	_float3 vScale = pTran->Get_Scale();

	if (vScale.x < m_fMaxX)
	{
		vScale.x += 0.005f;
		vScale.y += 0.005f;
	}
	else
	{
		vScale.x = m_fX;
		vScale.y = m_fY;
	}

	pTran->Set_Scale(XMLoadFloat3(&vScale));
}

void CImGui_Manager::Debug_Rigid(CRigid * pRigid)
{
	CRigid::RIGIDEDESC* pDesc = pRigid->Get_Desc();

	ImGui::Text("=======Rigid======");

	if (ImGui::DragFloat3("Pos", (float*)&pDesc->vPos, 0.1f))
		pRigid->Edit_PosRot();

	if (ImGui::DragFloat3("Extense", (float*)&pDesc->vExtense, 0.1f, 0.1f))
	{
		if(0.01f <pDesc->vExtense.x * 0.5f && 0.01f < pDesc->vExtense.y * 0.5f && 0.01f < pDesc->vExtense.z * 0.5f)
			pRigid->Edit_Px();
	}

	if (ImGui::InputFloat("Weight", &pDesc->fWeight))
		pRigid->Edit_Px();

	if (ImGui::InputFloat("StaticFriction", &pDesc->fStaticFriction))
		pRigid->Edit_Px();

	if (ImGui::InputFloat("DynamicFriction", &pDesc->fDynamicFriction))
		pRigid->Edit_Px();

	if (ImGui::InputFloat("Restitution", &pDesc->fRestitution))
		pRigid->Edit_Px();

	if (ImGui::InputFloat("AngleDump", &pDesc->fAngleDump))
		pRigid->Edit_Px();

	if (ImGui::InputFloat("MaxVel", &pDesc->fMaxVel))
		pRigid->Edit_Px();

}

void CImGui_Manager::Debug_Rigid_Sphere(CRigid_Sphere * pRigid)
{
	CRigid_Sphere::RIGIDEDESC* pDesc = pRigid->Get_Desc();

	ImGui::Text("=======Rigid======");

	ImGui::DragFloat3("Pos", (float*)&pDesc->vPos, 0.1f);

	if (ImGui::DragFloat3("Extense", (float*)&pDesc->vExtense, 0.1f))
		pRigid->Edit_Px();

	if (ImGui::InputFloat("Weight", &pDesc->fWeight))
		pRigid->Edit_Px();

	if (ImGui::InputFloat("StaticFriction", &pDesc->fStaticFriction))
		pRigid->Edit_Px();

	if (ImGui::InputFloat("DynamicFriction", &pDesc->fDynamicFriction))
		pRigid->Edit_Px();

	if (ImGui::InputFloat("Restitution", &pDesc->fRestitution))
		pRigid->Edit_Px();

	if (ImGui::InputFloat("AngleDump", &pDesc->fAngleDump))
		pRigid->Edit_Px();

	if (ImGui::Button("Reset_Pos"))
		pRigid->Reset_Pos();

	PxRigidDynamic* pD = pRigid->Get_Rigid();
	PxTransform pT = pD->getGlobalPose();
	_float3 vQ;
	vQ.x = pT.q.x;
	vQ.y = pT.q.y;
	vQ.z = pT.q.z;

	ImGui::DragFloat3("vQ", (float*)&vQ, 0.1f);

}

void CImGui_Manager::Debug_Col(CCollider * pCol)
{
	_float3 vSize = pCol->Get_Desc()->vSize;
	_float3 vCenter = pCol->Get_Desc()->vCenter;
	_float3 vRot = pCol->Get_Desc()->vRotation;

	ImGui::DragFloat3("Size", (float*)&vSize, 0.1f);
	ImGui::DragFloat3("Center", (float*)&vCenter, 0.1f);
	ImGui::DragFloat3("Rot", (float*)&vRot, 0.1f);

	pCol->Set_Size(vSize);
	pCol->Set_Center(vCenter);
	pCol->Set_Rot(vRot);
}



void CImGui_Manager::Free()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pCameraTool);
	Safe_Release(m_pModelTool);
	Safe_Release(m_pAnimTool);
	Safe_Release(m_pParticleTool);
	Safe_Release(m_pParticleInstanceTool);
}
