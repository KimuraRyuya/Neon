#pragma once
#include<Windows.h>
//**********************************************
//xbox
//**********************************************
#pragma comment (lib, "xinput.lib")
#include <xinput.h>
//**********************************************
//mouse
//**********************************************
#define DIRECTINPUT_VERSION		0x0800	//DirectInput�̃o�[�W�������
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

struct XboxButton
{
	bool DPAD_LEFT;			//direction left	(�����p�b�h�̍�)
	bool DPAD_RIGHT;		//direction right	(�����p�b�h�̉E)
	bool DPAD_UP;			//direction up		(�����p�b�h�̏�)
	bool DPAD_DOWN;			//direction down	(�����p�b�h�̉�)

	int LEFT_STICK_X;		//left stickX		(���X�e�B�b�Nx -1000~1000)
	int LEFT_STICK_Y;		//left stickY		(���X�e�B�b�Ny -1000~1000)
	int RIGHT_STICK_X;		//right stickX		(�E�X�e�B�b�Nx -1000~1000)
	int RIGHT_STICK_Y;		//right stickY		(�E�X�e�B�b�Ny -1000~1000)

	bool LEFT_BUTTON;		//left button		(���{�^��)
	bool RIGHT_BUTTON;		//right button		(�E�{�^��)

	bool A_BUTTON;			//a button			(A�{�^��)
	bool B_BUTTON;			//b button			(A�{�^��)
	bool X_BUTTON;			//x button			(A�{�^��)
	bool Y_BUTTON;			//y button			(A�{�^��)

	bool MENU_BUTTON;		//menu button		(���j���[�{�^��)
	bool VIEW_BUTTON;		//view button		(�r���[�{�^��)

	int LEFT_TRIGGER;		//left trigger		(���g���K�[)
	int RIGHT_TRIGGER;		//right trigger		(�E�g���K�[)
};

struct MouseButton
{
	bool left;		//���N���b�N
	bool right;		//�E�N���b�N
	bool middle;	//�^��

	int X;			//���WX
	int Y;			//���WY
};

class InputDevice
{
private:
	InputDevice() {}
public:
//******************************************************
//xbox
//******************************************************
	static XINPUT_STATE state;
	static XboxButton xb;
	static int mouseIndex;
	static void input_xbox();
	static void start_vibration(int vib_num);
	static void stop_vibration();

//******************************************************
//�}�E�X
//******************************************************
	static MouseButton				mb;
	static LPDIRECTINPUT8				DInput;		//DirectInput�I�u�W�F�N�g
	static LPDIRECTINPUTDEVICE8		DI_mouse;	//�}�E�X�f�o�C�X
	static DIMOUSESTATE				mouse_state;//�}�E�X���
	static HWND hwnd;
	//DirectInput�̏���������
	bool InitDInput(HINSTANCE hInstApp, HWND hWnd);
	//DirecInput�̏I������
	bool ReleaseDInput();
	//DirectInput�̃}�E�X�f�o�C�X�p�̏���������
	bool InitDInputMouse(HWND hWnd);
	//DirectInput�̃}�E�X�f�o�C�X�p�̉������
	bool ReleaseDInputMouse();
	//DirectInput�̃}�E�X�f�o�C�X���擾����
	static void GetMouseState(HWND hWnd);
	static void InputMouse();
	//�J�[�\���̕\������\��
	static void EnableCursor(bool enable, bool isShow = false);

	static InputDevice* GetInstance()
	{
		static InputDevice instance;
		return &instance;
	}
};

#define PAD (InputDevice::GetInstance())

