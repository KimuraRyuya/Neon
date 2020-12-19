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
	//�w�肵���R���g���[���[�̏����擾
	XInputGetState(0, &state);

	// �Q�[���p�b�h�̍��A�i���O�X�e�B�b�N�𑀍삵�����̏���
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
	// �Q�[���p�b�h�̉E�A�i���O�X�e�B�b�N�𑀍삵�����̏���
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

	//�����p�b�h�̍�
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT || GetAsyncKeyState('A') < 0 || GetAsyncKeyState(VK_LEFT) < 0 || xb.LEFT_STICK_X <= -800.0f)
	{
		xb.DPAD_LEFT = true;
	}
	else
	{
		xb.DPAD_LEFT = false;
	}
	//�����p�b�h�̍�
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT || GetAsyncKeyState('D') < 0 || GetAsyncKeyState(VK_RIGHT) < 0 || xb.LEFT_STICK_X >= 800.0f)
	{
		xb.DPAD_RIGHT = true;
	}
	else
	{
		xb.DPAD_RIGHT = false;
	}
	//�����p�b�h�̍�
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP || GetAsyncKeyState('W') < 0 || GetAsyncKeyState(VK_UP) < 0 || xb.LEFT_STICK_Y <= -800.0f)
	{
		xb.DPAD_UP = true;
	}
	else
	{
		xb.DPAD_UP = false;
	}
	//�����p�b�h�̍�
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN || GetAsyncKeyState('S') < 0 || GetAsyncKeyState(VK_DOWN) < 0 || xb.LEFT_STICK_Y >= 800.0f)
	{
		xb.DPAD_DOWN = true;
	}
	else
	{
		xb.DPAD_DOWN = false;
	}


	// LB�{�^��
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER || GetAsyncKeyState(VK_SHIFT) < 0)
	{
		xb.LEFT_BUTTON = true;
	}
	else
	{
		xb.LEFT_BUTTON = false;
	}
	// RB�{�^��
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER || GetAsyncKeyState(VK_CONTROL) < 0)
	{
		xb.RIGHT_BUTTON = true;
	}
	else
	{
		xb.RIGHT_BUTTON = false;
	}

	// A�{�^��
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_A || GetAsyncKeyState('Z') < 0)
	{
		xb.A_BUTTON = true;
	}
	else
	{
		xb.A_BUTTON = false;
	}

	// B�{�^��
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_B)
	{
		xb.B_BUTTON = true;
	}
	else
	{
		xb.B_BUTTON = false;
	}

	// X�{�^��
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_X)
	{
		xb.X_BUTTON = true;
	}
	else
	{
		xb.X_BUTTON = false;
	}

	// Y�{�^��
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y)
	{
		xb.Y_BUTTON = true;
	}
	else
	{
		xb.Y_BUTTON = false;
	}

	// menu�{�^��
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_START || GetAsyncKeyState(VK_SPACE) < 0)
	{
		xb.MENU_BUTTON = true;
	}
	else
	{
		xb.MENU_BUTTON = false;
	}

	// view�{�^��
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
//�o�C�u���[�V�����X�^�[�g
void InputDevice::start_vibration(int vib_num)
{
	XINPUT_VIBRATION vibration;
	vibration.wRightMotorSpeed = vib_num;
	vibration.wLeftMotorSpeed = vib_num;
	XInputSetState(0, &vibration);
}
//�o�C�u���[�V�����X�g�b�v
void InputDevice::stop_vibration()
{
	XINPUT_VIBRATION vibration;
	vibration.wRightMotorSpeed = 0;
	vibration.wLeftMotorSpeed = 0;
	XInputSetState(0, &vibration);
}

//DirectInput�̏���������
bool InputDevice::InitDInput(HINSTANCE hInstApp, HWND hWnd)
{
	HRESULT hr = DirectInput8Create(hInstApp, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&DInput, NULL);
	if (FAILED(hr)) return false;//�쐬���s

	hwnd = hWnd;
	return true;
}
//DirecInput�̏I������
bool InputDevice::ReleaseDInput()
{
	//DirectInput�̃f�o�C�X�����
	if (DInput)
	{
		DInput->Release();
		DInput = NULL;
	}
	return true;
}
//DirectInput�̃}�E�X�f�o�C�X�p�̏���������
bool InputDevice::InitDInputMouse(HWND hWnd)
{
	HRESULT ret = S_FALSE;
	if (DInput == NULL) {
		return false;
	}

	// �}�E�X�p�Ƀf�o�C�X�I�u�W�F�N�g���쐬
	ret = DInput->CreateDevice(GUID_SysMouse, &DI_mouse, NULL);
	if (FAILED(ret)) {
		// �f�o�C�X�̍쐬�Ɏ��s
		return false;
	}

	// �f�[�^�t�H�[�}�b�g��ݒ�
	ret = DI_mouse->SetDataFormat(&c_dfDIMouse);	// �}�E�X�p�̃f�[�^�E�t�H�[�}�b�g��ݒ�
	if (FAILED(ret)) {
		// �f�[�^�t�H�[�}�b�g�Ɏ��s
		return false;
	}

	// ���[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
	ret = DI_mouse->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(ret)) {
		// ���[�h�̐ݒ�Ɏ��s
		return false;
	}

	// �f�o�C�X�̐ݒ�
	DIPROPDWORD diprop;
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwObj = 0;
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.dwData = DIPROPAXISMODE_REL;	// ���Βl���[�h�Őݒ�i��Βl��DIPROPAXISMODE_ABS�j

	ret = DI_mouse->SetProperty(DIPROP_AXISMODE, &diprop.diph);
	if (FAILED(ret)) {
		// �f�o�C�X�̐ݒ�Ɏ��s
		return false;
	}

	// ���͐���J�n
	DI_mouse->Acquire();

	return true;
}
//DirectInput�̃}�E�X�f�o�C�X�p�̉������
bool InputDevice::ReleaseDInputMouse()
{
	//DirectInput�̃f�o�C�X���
	if (DI_mouse)
	{
		DI_mouse->Release();
		DI_mouse = NULL;
	}
	return true;
}
//DirectInput�̃}�E�X�f�o�C�X���擾����
void InputDevice::GetMouseState(HWND hWnd)
{
	// �}�E�X�̏�Ԃ��擾���܂�
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
	// �X�N���[����ł̈ʒu���擾
	// �c�[���o�[�A�^�C�g���o�[�Ȃǂ͖��l��
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