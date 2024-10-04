#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "Camera.h"

BEGIN(Engine)
class CTexture;
class CSphere;
class CRenderer;
class CRigid;
class CRigid_Sphere;
END

BEGIN(Client)
class CUI;
class CCameraTool;
class CModelTool;
class CMesh;
class CAnimMesh;
class CAnimTool;
class CParticleTool;
class CParticleInstanceTool;


class CImGui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImGui_Manager)

public:
	CImGui_Manager();
	virtual ~CImGui_Manager() = default;

public:
	enum ToolType {Tool_Main, Tool_Camera, Tool_Model, Tool_AnimModel, Tool_Particle, Tool_ParticleInstance, Tool_END};

public:
	HRESULT Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void Tick(_float fTimeDelta);
	void Render();

public:
	_float4 Get_BackBufferColor() { return m_fBackBufferColor; }
	bool Get_bPicking() { return m_bPicking; }
	ToolType GetToolType() { return m_eTool; }

private:
	void Get_KeyInput_Static();
	void Get_KeyInput_Tool();

private:
	void Render_Tool(_float fTimeDelta);
	void Render_Select(_float fTimeDelta);

	void MainWindow(_float fTimeDelta);
	void ShowExampleWindow(_float fTimeDelta);


	ToolType m_eTool = Tool_Main;

#pragma region Tab
	void ChangeTool();
	list<const char*> m_TabName;
#pragma endregion Tab


	// MainWindow
#pragma region MainWindow
public:
	void Add_Renderer(CRenderer* pRenderer);

private:
	_float4 m_fBackBufferColor = { 0.5f,0.5f,0.5f,1.f };
private:
	bool m_bMainWindow = true;
	
	bool m_bKeyInput = false;
	bool m_bPicking = false;
	bool m_bRenderTarget = false;
	string m_sInputMode = "Client";
	float m_fWinSizeX = 700.f; // WinsizeX
	float m_fWinSizeY = 600.f; // WinsizeY
	float m_fTimeDelta;
	CRenderer* m_pRenderer = nullptr;
#pragma endregion MainWindow

#pragma region Example

private:
	bool m_bShow_Test_Window;
	bool m_bTest;
	bool m_bExampleWindow = false;

	ImVec4 m_ColorEdit; // ColorEdit float
	ImVec4 m_TextColor = { 255,0,0,1 }; // TextColor
	ImVec4 m_ButtonColor = { 0,255,0,1 }; // ButtonColor

#pragma endregion Example

#pragma region Camera
public:
	bool GetToolCamera() { return m_bToolCamera; }
private:
	CCameraTool* m_pCameraTool = nullptr;
	bool m_bToolCamera = false;
#pragma endregion Camera

#pragma region Model
public:
	void AddModelObject(CMesh* pMesh);
	void Create_Model_Object(_float4 fPos);
	_bool Get_ModelTool() { return m_bModelEditWindow; }
	int Get_ShaderPass();
	_float Get_MaxUVIndexX();
	_float Get_MaxUVIndexY();
	_float Get_UVSpeed();
	void Add_Mesh(CMesh* pMesh);
private:
	bool m_bModelEditWindow = false;
	CModelTool* m_pModelTool = nullptr;

#pragma endregion Model

#pragma region Animation
public:
	void AddAnimMesh(CAnimMesh* pAnimMesh);
	bool Get_AnimToolWindow() { return m_bAnimTool; }
	bool GetAnimPlay();
	int Get_AniIndex();
private:
	bool m_bAnimTool = false;
	bool m_bAnimModelEditWindow = false;
	CAnimTool* m_pAnimTool;
#pragma endregion Animation

#pragma region Particle
public:
	_bool Get_ParticleTool() { return m_bParticleTool; }

private:
	_bool m_bParticleTool = false;
	_bool m_bParticleInfoWindow = false;
	CParticleTool* m_pParticleTool = nullptr;
#pragma endregion Particle

#pragma region ParticleInstance
//public:
//	_bool Get_ParticleTool() { return m_bParticleTool; }
//
private:
	_bool m_bParticleInstanceTool = false;
	_bool m_bParticleInstanceInfoWindow = false;
	CParticleInstanceTool* m_pParticleInstanceTool = nullptr;
#pragma endregion ParticleInstance


private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;
	CTexture* m_pTextureCom;
	ImGuiConfigFlags temp;




	// ===================
	//     For.Debug
	// ===================
public:
	void Reset_DebugData();

	void Set_Frame(_uint iFrame) { m_iFrame = iFrame; }
	_bool Get_ChangeLimit() { return m_bChangeLimit; }

	/* [HeroJho]: 월드 행렬을 디버깅 합니다. */
	void Debug_Transform(class CTransform* pTran);
	/* [HeroJho]: 클릭된 객체를 표시합니다. */
	void Debug_SelectedObj(class CTransform* pTran);
	/* [HeroJho]: 피직스를 수정합니다. */
	void Debug_Rigid(class CRigid* pRigid);
	void Debug_Rigid_Sphere(class CRigid_Sphere* pRigid);
	/* [HeroJho]: 피직스를 수정합니다. */
	void Debug_Col(class CCollider* pCol);

	_bool Get_ShowCursor() { return m_bShowCursor; }
	void Set_ShowCursor() { m_bShowCursor = !m_bShowCursor; }

	_bool Get_ClipWindow() { return m_bClipWindow; }
	void Set_ClipWindow() { m_bClipWindow = !m_bClipWindow; }

	CGameObject* Get_SelectedObj() { return m_pSelectedObject; }

	void Set_Px_Tool(class CPx_Tool* pPx_Tool) { m_pPx_Tool = pPx_Tool; };
	class CPx_Tool* Get_Px_Tool() { return m_pPx_Tool; };

	_bool Get_ShowSelectedObj() { return m_bSelectedObj; }

private:
	void Render_Debug();


private:
	_bool m_bDebug = false;
	char m_sSelectedLayer[MAX_PATH] = "";
	string m_sSelectedObj = "";
	CGameObject* m_pSelectedObject = nullptr;

	_uint m_iFrame = 0;
	_bool m_bChangeLimit = false;

	_bool		m_bSelectedObj = false;
	_float		m_fMaxX = 0.f;
	_float		m_fX = 0.f;
	_float		m_fY = 0.f;

	_bool		m_bColMode = false;
	_bool		m_bTargetMode = false;
	_bool		m_bShowSelectedObj = false;

	_bool m_bShowCursor = true;
	_bool m_bClipWindow = false;

	class CPx_Tool* m_pPx_Tool = nullptr;

public:
	virtual void Free() override;
};

END

