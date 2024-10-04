#pragma once


namespace Client
{
	static const unsigned int g_iWinSizeX = 1280;
	static const unsigned int g_iWinSizeY = 720;

	enum LEVEL { LEVEL_LOAD, LEVEL_LOADING, LEVEL_LOGO, LEVEL_TEST, LEVEL_TOOL, LEVEL_STAGE1, LEVEL_STAGE2, LEVEL_STAGE3, LEVEL_STAGE4, LEVEL_STAGE5, LEVEL_STATIC, LEVEL_END };

}

#define IG CImGui_Manager::Get_Instance()
#define GI CGameInstance::Get_Instance()
#define TM CToolManager::Get_Instance()
#define DM CDataManager::Get_Instance()
#define PM CPlayer_Manager::Get_Instance()
#define CRM CCamera_Manager::Get_Instance()
#define UM CUI_Manager::Get_Instance()
#define PTM CParticle_Manager::Get_Instance()
#define STM CStatus_Manager::Get_Instance()
#define MM CMonster_Manager::Get_Instance()
#define TIM CTime_Manager::Get_Instance()
#define RTM CRoute_Manager::Get_Instance()
#define EM CEffect_Manager::Get_Instance()
#define CRKM CCrack_Manager::Get_Instance()

#define CK_DOWN(eDIK)		GI->Key_Down(eDIK, 0) == CInput_Device::INPUT_TRUE
#define TK_DOWN(eDIK)		GI->Key_Down(eDIK, 1) == CInput_Device::INPUT_TRUE
#define	K_DOWN(eDIK)		GI->Key_Down(eDIK, 2) == CInput_Device::INPUT_TRUE
#define CK_UP(eDIK)			GI->Key_Up(eDIK, 0) == CInput_Device::INPUT_TRUE
#define TK_UP(eDIK)			GI->Key_Up(eDIK, 1) == CInput_Device::INPUT_TRUE
#define	K_UP(eDIK)			GI->Key_Up(eDIK, 2) == CInput_Device::INPUT_TRUE
#define CK_PRESS(eDIK)		GI->Key_Pressing(eDIK, 0) == CInput_Device::INPUT_TRUE
#define TK_PRESS(eDIK)		GI->Key_Pressing(eDIK, 1) == CInput_Device::INPUT_TRUE
#define	K_PRESS(eDIK)		GI->Key_Pressing(eDIK, 2) == CInput_Device::INPUT_TRUE

#define CM_DOWN(eDIK)		GI->Mouse_Down(eDIK, 0) == CInput_Device::INPUT_TRUE
#define TM_DOWN(eDIK)		GI->Mouse_Down(eDIK, 1) == CInput_Device::INPUT_TRUE
#define	M_DOWN(eDIK)		GI->Mouse_Down(eDIK, 2) == CInput_Device::INPUT_TRUE
#define CM_UP(eDIK)			GI->Mouse_Up(eDIK, 0) == CInput_Device::INPUT_TRUE
#define TM_UP(eDIK)			GI->Mouse_Up(eDIK, 1) == CInput_Device::INPUT_TRUE
#define	M_UP(eDIK)			GI->Mouse_Up(eDIK, 2) == CInput_Device::INPUT_TRUE
#define CM_PRESS(eDIK)		GI->Mouse_Pressing(eDIK, 0) == CInput_Device::INPUT_TRUE
#define TM_PRESS(eDIK)		GI->Mouse_Pressing(eDIK, 1) == CInput_Device::INPUT_TRUE
#define	M_PRESS(eDIK)		GI->Mouse_Pressing(eDIK, 2) == CInput_Device::INPUT_TRUE



#define CK_DOWN_F(eDIK)		GI->Key_Down(eDIK, 0) == CInput_Device::INPUT_FALSE
#define TK_DOWN_F(eDIK)		GI->Key_Down(eDIK, 1) == CInput_Device::INPUT_FALSE
#define	K_DOWN_F(eDIK)		GI->Key_Down(eDIK, 2) == CInput_Device::INPUT_FALSE
#define CK_UP_F(eDIK)		GI->Key_Up(eDIK, 0) == CInput_Device::INPUT_FALSE
#define TK_UP_F(eDIK)		GI->Key_Up(eDIK, 1) == CInput_Device::INPUT_FALSE
#define	K_UP_F(eDIK)		GI->Key_Up(eDIK, 2) == CInput_Device::INPUT_FALSE
#define CK_PRESS_F(eDIK)	GI->Key_Pressing(eDIK, 0) == CInput_Device::INPUT_FALSE
#define TK_PRESS_F(eDIK)	GI->Key_Pressing(eDIK, 1) == CInput_Device::INPUT_FALSE
#define	K_PRESS_F(eDIK)		GI->Key_Pressing(eDIK, 2) == CInput_Device::INPUT_FALSE

#define CM_DOWN_F(eDIK)		GI->Mouse_Down(eDIK, 0) == CInput_Device::INPUT_FALSE
#define TM_DOWN_F(eDIK)		GI->Mouse_Down(eDIK, 1) == CInput_Device::INPUT_FALSE
#define	M_DOWN_F(eDIK)		GI->Mouse_Down(eDIK, 2) == CInput_Device::INPUT_FALSE
#define CM_UP_F(eDIK)			GI->Mouse_Up(eDIK, 0) == CInput_Device::INPUT_FALSE
#define TM_UP_F(eDIK)			GI->Mouse_Up(eDIK, 1) == CInput_Device::INPUT_FALSE
#define	M_UP_F(eDIK)			GI->Mouse_Up(eDIK, 2) == CInput_Device::INPUT_FALSE
#define CM_PRESS_F(eDIK)		GI->Mouse_Pressing(eDIK, 0) == CInput_Device::INPUT_FALSE
#define TM_PRESS_F(eDIK)		GI->Mouse_Pressing(eDIK, 1) == CInput_Device::INPUT_FALSE
#define	M_PRESS_F(eDIK)		GI->Mouse_Pressing(eDIK, 2) == CInput_Device::INPUT_FALSE


#define FAILED(hr) (((HRESULT)(hr)) < 0)

extern HINSTANCE g_hInst;
extern HWND g_hWnd;

using namespace Client;
