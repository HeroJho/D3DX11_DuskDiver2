#include "..\Public\Input_Device.h"

IMPLEMENT_SINGLETON(CInput_Device)

CInput_Device::CInput_Device()
{
}
											
CInput_Device::INPUTMODE CInput_Device::Key_Down(_uchar eKeyID, _int iMode)
{
	
	if ((iMode != 2) && (m_iDebugInput != iMode))
		return INPUT_STOP;

	if (m_preKeyState[eKeyID] != m_byKeyState[eKeyID])
	{
		if (m_byKeyState[eKeyID] & 0x80)
			return INPUT_TRUE;
	}
	return INPUT_FALSE;
}

CInput_Device::INPUTMODE CInput_Device::Key_Up(_uchar eKeyID, _int iMode)
{
	if ((iMode != 2) && (m_iDebugInput != iMode))
		return INPUT_STOP;

	if (m_preKeyState[eKeyID] != m_byKeyState[eKeyID])
	{
		if (m_preKeyState[eKeyID] & 0x80)
			return INPUT_TRUE;
	}
	return INPUT_FALSE;


}

CInput_Device::INPUTMODE CInput_Device::Key_Pressing(_uchar eKeyID, _int iMode)
{
	if ((iMode != 2) && (m_iDebugInput != iMode))
		return INPUT_STOP;

	if (m_preKeyState[eKeyID] == m_byKeyState[eKeyID])
	{
		if (m_preKeyState[eKeyID] & 0x80)
			return INPUT_TRUE;
	}
	return INPUT_FALSE;
}

CInput_Device::INPUTMODE CInput_Device::Mouse_Down(DIMK eMouseKeyID, _int iMode)
{
	if ((iMode != 2) && (m_iDebugInput != iMode))
		return INPUT_STOP;

	if (m_PreMouseState.rgbButtons[eMouseKeyID] != m_MouseState.rgbButtons[eMouseKeyID])
	{
		if (m_MouseState.rgbButtons[eMouseKeyID] & 0x80)
			return INPUT_TRUE;
	}
	return INPUT_FALSE;
}

CInput_Device::INPUTMODE CInput_Device::Mouse_Up(DIMK eMouseKeyID, _int iMode)
{
	if ((iMode != 2) && (m_iDebugInput != iMode))
		return INPUT_STOP;

	if (m_PreMouseState.rgbButtons[eMouseKeyID] != m_MouseState.rgbButtons[eMouseKeyID])
	{
		if (m_PreMouseState.rgbButtons[eMouseKeyID] & 0x80)
			return INPUT_TRUE;
	}
	return INPUT_FALSE;
}

CInput_Device::INPUTMODE CInput_Device::Mouse_Pressing(DIMK eMouseKeyID, _int iMode)
{
	if ((iMode != 2) && (m_iDebugInput != iMode))
		return INPUT_STOP;

	if (m_PreMouseState.rgbButtons[eMouseKeyID] == m_MouseState.rgbButtons[eMouseKeyID])
	{
		if (m_MouseState.rgbButtons[eMouseKeyID] & 0x80)
			return INPUT_TRUE;
	}
	return INPUT_FALSE;
}

HRESULT CInput_Device::Initialize(HINSTANCE hInstance, HWND hWnd)
{
	/* m_pInputSDK */
	if (FAILED(DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInputSDK, nullptr)))
		return E_FAIL;

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr)))
		return E_FAIL;

	if (FAILED(m_pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
		return E_FAIL;
	if (FAILED(m_pKeyboard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE /*| DISCL_NOWINKEY*/)))
		return E_FAIL;
	if (FAILED(m_pKeyboard->Acquire()))
		return E_FAIL;

	if (FAILED(m_pInputSDK->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;


	if (FAILED(m_pMouse->SetDataFormat(&c_dfDIMouse)))
		return E_FAIL;
	if (FAILED(m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE /*| DISCL_NOWINKEY*/)))
		return E_FAIL;
	if (FAILED(m_pMouse->Acquire()))
		return E_FAIL;

	return S_OK;
}

void CInput_Device::Update()
{
	memcpy(&m_preKeyState, &m_byKeyState, sizeof(_char)*256);
	m_pKeyboard->GetDeviceState(256, m_byKeyState);
	memcpy(&m_PreMouseState, &m_MouseState, sizeof(DIMOUSESTATE));
	m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_MouseState);
}

void CInput_Device::Free()
{
	Safe_Release(m_pKeyboard);
	Safe_Release(m_pMouse);

	Safe_Release(m_pInputSDK);
}
