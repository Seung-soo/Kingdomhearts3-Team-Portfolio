#include "..\public\Input_Device.h"

READY_SINGLETON(CInput_Device)

CInput_Device::CInput_Device()
{
}

_bool CInput_Device::Get_Mouse_Enter(CInput_Device::MOUSEBUTTONSTATE eMouseButtonState)
{
	if (!(m_byOldMouseState[eMouseButtonState]) && (m_byMouseState[eMouseButtonState]))
		return true;

	return false;
}

_bool CInput_Device::Get_Mouse_Up(CInput_Device::MOUSEBUTTONSTATE eMouseButtonState)
{
	if ((m_byOldMouseState[eMouseButtonState]) && !(m_byMouseState[eMouseButtonState]))
		return true;

	return false;
}

_bool CInput_Device::Mouse_Pressing(MOUSEBUTTONSTATE eMouseButtonState)
{
	if (Get_DIMouseButtonState(eMouseButtonState) & 0x80)
		return TRUE;

	return FALSE;
}

_bool CInput_Device::Get_KeyEnter(_ubyte eKeyID)
{
	if (!(0x80 & m_byOldKeyState[eKeyID]) && (0x80 & m_byKeyState[eKeyID]))
		return true;

	return false;
}

_bool CInput_Device::Get_KeyUp(_ubyte eKeyID)
{
	if ((0x80 & m_byOldKeyState[eKeyID]) && !(0x80 & m_byKeyState[eKeyID]))
		return true;

	return false;
}

_bool CInput_Device::Get_KeyPressing(_ubyte eKeyID)
{
	if (Get_DIKeyState(eKeyID) & 0x80)
		return TRUE;

	return FALSE;
}

HRESULT CInput_Device::Ready_Input_Device(HINSTANCE hInst, HWND hWnd)
{
	if (FAILED(DirectInput8Create(hInst, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pInput, nullptr)))
		return E_FAIL;

	if (FAILED(m_pInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, nullptr)))
		return E_FAIL;

	if (FAILED(m_pKeyboard->SetDataFormat(&c_dfDIKeyboard)))
		return E_FAIL;
	
	if (FAILED(m_pKeyboard->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
		return E_FAIL;

	if (FAILED(m_pKeyboard->Acquire()))
		return E_FAIL;

	if (FAILED(m_pInput->CreateDevice(GUID_SysMouse, &m_pMouse, nullptr)))
		return E_FAIL;

	if (FAILED(m_pMouse->SetDataFormat(&c_dfDIMouse)))
		return E_FAIL;

	if (FAILED(m_pMouse->SetCooperativeLevel(hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
		return E_FAIL;

	if (FAILED(m_pMouse->Acquire()))
		return E_FAIL;

	ZeroMemory(m_byOldKeyState, sizeof(_byte) * 256);
	ZeroMemory(m_byOldMouseState, sizeof(_byte) * 4);

	return S_OK;
}

HRESULT CInput_Device::SetUp_InputDeviceState()
{
	if (nullptr == m_pKeyboard)
	{
		MSGBOX("m_pKeyboard is nullptr in CInput_Device::SetUp_InputDeviceState");
		return E_FAIL;
	}

	if (nullptr == m_pMouse)
	{
		MSGBOX("m_pMouse is nullptr in CInput_Device::SetUp_InputDeviceState");
		return E_FAIL;
	}
	memcpy(m_byOldKeyState, m_byKeyState, sizeof(_byte) * 256);
	m_pKeyboard->GetDeviceState(256, m_byKeyState);

	memcpy(m_byOldMouseState, m_byMouseState, sizeof(_byte) * 4);
	m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), &m_MouseState);
	memcpy(m_byMouseState, m_MouseState.rgbButtons, sizeof(_ubyte) * 4);

	return S_OK;
}

void CInput_Device::Free()
{
	Safe_Release(m_pMouse);
	Safe_Release(m_pKeyboard);
	Safe_Release(m_pInput);
}
