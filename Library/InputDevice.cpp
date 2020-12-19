#include"InputDevice.h"
#include"MyLib.h"

XINPUT_STATE InputDevice::state;
XboxButton InputDevice::xb;
HWND InputDevice::hwnd;
MouseButton InputDevice::mb;
LPDIRECTINPUT8 InputDevice::DInput = NULL;
LPDIRECTINPUTDEVICE8 InputDevice::DI_mouse = NULL;
DIMOUSESTATE InputDevice::mouse_state;
int InputDevice::mouseIndex = 0;

void InputDevice::input_xbox()
{
	//指定したコントローラーの情報を取得
	XInputGetState(0, &state);

	// ゲームパッドの左アナログスティックを操作した時の処理
	if ((state.Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
		state.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
		(state.Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			state.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE))
	{
		state.Gamepad.sThumbLX = 0;
		state.Gamepad.sThumbLY = 0;
	}

	xb.LEFT_STICK_X = static_cast<int>(state.Gamepad.sThumbLX / 32768.0f * 1000.0f);
	xb.LEFT_STICK_Y = static_cast<int>((state.Gamepad.sThumbLY / 32768.0f * 1000.0f) * -1.0f);
	// ゲームパッドの右アナログスティックを操作した時の処理
	if ((state.Gamepad.sThumbRX < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
		state.Gamepad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) &&
		(state.Gamepad.sThumbRY < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
			state.Gamepad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE))
	{
		state.Gamepad.sThumbRX = 0;
		state.Gamepad.sThumbRY = 0;
	}

	xb.RIGHT_STICK_X = static_cast<int>(state.Gamepad.sThumbRX / 32768.0f * 1000);
	xb.RIGHT_STICK_Y = static_cast<int>((state.Gamepad.sThumbRY / 32768.0f * 1000) * -1);

	//方向パッドの左
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT || GetAsyncKeyState('A') < 0 || GetAsyncKeyState(VK_LEFT) < 0 || xb.LEFT_STICK_X <= -800.0f)
	{
		xb.DPAD_LEFT = true;
	}
	else
	{
		xb.DPAD_LEFT = false;
	}
	//方向パッドの左
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT || GetAsyncKeyState('D') < 0 || GetAsyncKeyState(VK_RIGHT) < 0 || xb.LEFT_STICK_X >= 800.0f)
	{
		xb.DPAD_RIGHT = true;
	}
	else
	{
		xb.DPAD_RIGHT = false;
	}
	//方向パッドの左
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP || GetAsyncKeyState('W') < 0 || GetAsyncKeyState(VK_UP) < 0 || xb.LEFT_STICK_Y <= -800.0f)
	{
		xb.DPAD_UP = true;
	}
	else
	{
		xb.DPAD_UP = false;
	}
	//方向パッドの左
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN || GetAsyncKeyState('S') < 0 || GetAsyncKeyState(VK_DOWN) < 0 || xb.LEFT_STICK_Y >= 800.0f)
	{
		xb.DPAD_DOWN = true;
	}
	else
	{
		xb.DPAD_DOWN = false;
	}


	// LBボタン
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER || GetAsyncKeyState(VK_SHIFT) < 0)
	{
		xb.LEFT_BUTTON = true;
	}
	else
	{
		xb.LEFT_BUTTON = false;
	}
	// RBボタン
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER || GetAsyncKeyState(VK_CONTROL) < 0)
	{
		xb.RIGHT_BUTTON = true;
	}
	else
	{
		xb.RIGHT_BUTTON = false;
	}

	// Aボタン
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A || GetAsyncKeyState('Z') < 0)
	{
		xb.A_BUTTON = true;
	}
	else
	{
		xb.A_BUTTON = false;
	}

	// Bボタン
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_B)
	{
		xb.B_BUTTON = true;
	}
	else
	{
		xb.B_BUTTON = false;
	}

	// Xボタン
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_X)
	{
		xb.X_BUTTON = true;
	}
	else
	{
		xb.X_BUTTON = false;
	}

	// Yボタン
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y)
	{
		xb.Y_BUTTON = true;
	}
	else
	{
		xb.Y_BUTTON = false;
	}

	// menuボタン
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_START || GetAsyncKeyState(VK_SPACE) < 0)
	{
		xb.MENU_BUTTON = true;
	}
	else
	{
		xb.MENU_BUTTON = false;
	}

	// viewボタン
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_BACK || GetAsyncKeyState(VK_SHIFT) < 0 && GetAsyncKeyState(VK_SPACE) < 0)
	{
		xb.VIEW_BUTTON = true;
	}
	else
	{
		xb.VIEW_BUTTON = false;
	}

	// left_trigger
	xb.LEFT_TRIGGER = static_cast<int>(state.Gamepad.bLeftTrigger / 255.0f * 1000);
	// right trigger
	xb.RIGHT_TRIGGER = static_cast<int>(state.Gamepad.bRightTrigger / 255.0f * 1000);

	if (GetAsyncKeyState('A') < 0) xb.LEFT_STICK_X = -1000.0f;
	else if (GetAsyncKeyState('D') < 0)	 xb.LEFT_STICK_X = 1000.0f;


	if (GetAsyncKeyState('W') < 0) xb.LEFT_STICK_Y = -1000.0f;
	else if (GetAsyncKeyState('S') < 0)	 xb.LEFT_STICK_Y = 1000.0f;

}
//バイブレーションスタート
void InputDevice::start_vibration(int vib_num)
{
	XINPUT_VIBRATION vibration;
	vibration.wRightMotorSpeed = vib_num;
	vibration.wLeftMotorSpeed = vib_num;
	XInputSetState(0, &vibration);
}
//バイブレーションストップ
void InputDevice::stop_vibration()
{
	XINPUT_VIBRATION vibration;
	vibration.wRightMotorSpeed = 0;
	vibration.wLeftMotorSpeed = 0;
	XInputSetState(0, &vibration);
}

//DirectInputの初期化処理
bool InputDevice::InitDInput(HINSTANCE hInstApp, HWND hWnd)
{
	HRESULT hr = DirectInput8Create(hInstApp, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DInput, NULL);
	if (FAILED(hr)) return false;//作成失敗

	hwnd = hWnd;
	return true;
}
//DirecInputの終了処理
bool InputDevice::ReleaseDInput()
{
	//DirectInputのデバイスを解放
	if (DInput)
	{
		DInput->Release();
		DInput = NULL;
	}
	return true;
}
//DirectInputのマウスデバイス用の初期化処理
bool InputDevice::InitDInputMouse(HWND hWnd)
{
	HRESULT ret = S_FALSE;
	if (DInput == NULL) {
		return false;
	}

	// マウス用にデバイスオブジェクトを作成
	ret = DInput->CreateDevice(GUID_SysMouse, &DI_mouse, NULL);
	if (FAILED(ret)) {
		// デバイスの作成に失敗
		return false;
	}

	// データフォーマットを設定
	ret = DI_mouse->SetDataFormat(&c_dfDIMouse);	// マウス用のデータ・フォーマットを設定
	if (FAILED(ret)) {
		// データフォーマットに失敗
		return false;
	}

	// モードを設定（フォアグラウンド＆非排他モード）
	ret = DI_mouse->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(ret)) {
		// モードの設定に失敗
		return false;
	}

	// デバイスの設定
	DIPROPDWORD diprop;
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwObj = 0;
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.dwData = DIPROPAXISMODE_REL;	// 相対値モードで設定（絶対値はDIPROPAXISMODE_ABS）

	ret = DI_mouse->SetProperty(DIPROP_AXISMODE, &diprop.diph);
	if (FAILED(ret)) {
		// デバイスの設定に失敗
		return false;
	}

	// 入力制御開始
	DI_mouse->Acquire();

	return true;
}
//DirectInputのマウスデバイス用の解放処理
bool InputDevice::ReleaseDInputMouse()
{
	//DirectInputのデバイス解放
	if (DI_mouse)
	{
		DI_mouse->Release();
		DI_mouse = NULL;
	}
	return true;
}
//DirectInputのマウスデバイス情報取得処理
void InputDevice::GetMouseState(HWND hWnd)
{
	// マウスの状態を取得します
	HRESULT	hr = DI_mouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouse_state);
	if (hr != S_OK) {
		DI_mouse->Acquire();
		hr = DI_mouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouse_state);
	}

	if (mouse_state.rgbButtons[0] & 0x80)
	{
		mb.left = true;
		if(mouseIndex < 0) EnableCursor(true);
		if (mouseIndex >= 0)
		{
			if (FrameWork::GetOnGame())
			{
				ShowCursor(true);
			}
		}
	}
	else
	{
		mb.left = false;
	}
	if (mouse_state.rgbButtons[1])
	{
		mb.right = true;
	}
	else
	{
		mb.right = false;
	}
	if (mouse_state.rgbButtons[2] & 0x80)
	{
		mb.middle = true;
	}
	else
	{
		mb.middle = false;
	}


	mouse_state.rgbButtons[0] = 0;
	mouse_state.rgbButtons[1] = 0;
	mouse_state.rgbButtons[2] = 0;

	RECT rect;
	GetWindowRect(hWnd, &rect);
	POINT MousePos;
	// スクリーン上での位置を取得
	// ツールバー、タイトルバーなどは未考慮
	GetCursorPos(&MousePos);
	mb.X = MousePos.x - rect.left;
	mb.Y = MousePos.y - rect.top;
}

void InputDevice::InputMouse()
{
	GetMouseState(hwnd);
}
void InputDevice::EnableCursor(bool enable, bool isShow)
{
	if (isShow)
	{
		if (enable && mouseIndex >= 0) return;
		if (!enable && mouseIndex < 0) return;
		mouseIndex = ShowCursor(enable);
		return;
	}

	if (!enable && mouseIndex < 0) return;
	if (enable && mouseIndex >= 0) return;
	mouseIndex = ShowCursor(enable);
}

void GetInputXbox()
{
	InputDevice::input_xbox();
}
void GetInputMouse()
{
	InputDevice::InputMouse();
}
void StartVibration(int vib_pal)
{
	InputDevice::start_vibration(vib_pal);
}
void StopVibration()
{
	InputDevice::stop_vibration();
}
void EnableCursor(bool enable)
{
	InputDevice::EnableCursor(enable);
}