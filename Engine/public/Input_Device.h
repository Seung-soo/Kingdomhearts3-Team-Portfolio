#pragma once

#include "Base.h"

BEGIN(Engine)

class CInput_Device final : public CBase
{
	MAKE_SINGLETON(CInput_Device)

public:
	enum MOUSEMOVESTATE { MMS_X, MMS_Y, MMS_WHEEL, MMS_END };
	enum MOUSEBUTTONSTATE { MBS_LBUTTON, MBS_RBUTTON, MBS_WHEEL, MBS_END };

private:
	CInput_Device();
	virtual ~CInput_Device() = default;

public:
	_byte Get_DIKeyState(_ubyte eKeyID) {
		return m_byKeyState[eKeyID];
	}
	_long Get_DIMouseMoveState(MOUSEMOVESTATE eMouseMoveState) {
		return *((_long*)&m_MouseState + eMouseMoveState);
	}
	_byte Get_DIMouseButtonState(MOUSEBUTTONSTATE eMouseButtonState) {
		return m_MouseState.rgbButtons[eMouseButtonState];
	}

public:
	_bool Get_Mouse_Enter(MOUSEBUTTONSTATE eMouseButtonState);
	_bool Get_Mouse_Up(MOUSEBUTTONSTATE eMouseButtonState);
	_bool Mouse_Pressing(MOUSEBUTTONSTATE eMouseButtonState);

	_bool Get_KeyEnter(_ubyte eKeyID);
	_bool Get_KeyUp(_ubyte eKeyID);
	_bool Get_KeyPressing(_ubyte eKeyID);

public:
	HRESULT Ready_Input_Device(HINSTANCE hInst, HWND hWnd);
	HRESULT SetUp_InputDeviceState();

private:
	LPDIRECTINPUT8				m_pInput = nullptr;
	LPDIRECTINPUTDEVICE8		m_pKeyboard = nullptr;
	LPDIRECTINPUTDEVICE8		m_pMouse = nullptr;	


	
private:
	_byte					m_byKeyState[256];
	_byte					m_byOldKeyState[256];
	DIMOUSESTATE			m_MouseState;
	_ubyte					m_byMouseState[4];
	_ubyte					m_byOldMouseState[4];
public:
	virtual void Free() override;
};

END