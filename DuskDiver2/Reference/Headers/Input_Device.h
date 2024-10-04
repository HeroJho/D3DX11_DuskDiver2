#pragma once

#include "Base.h"

BEGIN(Engine)

class CInput_Device final : public CBase
{
	DECLARE_SINGLETON(CInput_Device)	

public:
	enum INPUTMODE { INPUT_TRUE, INPUT_FALSE, INPUT_STOP };
private:
	CInput_Device();
	virtual ~CInput_Device() = default;

public:
	_char Get_DIKState(_uchar eKeyID) {
		return m_byKeyState[eKeyID];
	}

	_char Get_DIMKeyState(DIMK eMouseKeyID) {
		return m_MouseState.rgbButtons[eMouseKeyID];
	}

	_long Get_DIMMoveState(DIMM eMouseMoveID) {
		return ((_long*)&m_MouseState)[eMouseMoveID];
	}

	INPUTMODE Key_Down(_uchar eKeyID, _int iMode = 0);
	INPUTMODE Key_Up(_uchar eKeyID, _int iMode = 0);
	INPUTMODE Key_Pressing(_uchar eKeyID, _int iMode = 0);

	INPUTMODE Mouse_Down(DIMK eMouseKeyID, _int iMode = 0);
	INPUTMODE Mouse_Up(DIMK eMouseKeyID, _int iMode = 0);
	INPUTMODE Mouse_Pressing(DIMK eMouseKeyID, _int iMode = 0);

	_int Get_DebugInput() { return m_iDebugInput; }
	void Set_DebugInput(_int iDebugInput) { m_iDebugInput = iDebugInput; }

public:
	HRESULT Initialize(HINSTANCE hInstance, HWND hWnd);
	void Update();

private:
	LPDIRECTINPUT8				m_pInputSDK = nullptr;
	LPDIRECTINPUTDEVICE8		m_pKeyboard = nullptr;
	LPDIRECTINPUTDEVICE8		m_pMouse = nullptr;

private:	
	_char				m_preKeyState[256] = { 0 };
	_char				m_byKeyState[256] = { 0 };
	DIMOUSESTATE		m_PreMouseState;
	DIMOUSESTATE		m_MouseState;
	_int				m_iDebugInput = 0;

public:
	virtual void Free();
};

END