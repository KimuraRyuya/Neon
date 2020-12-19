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
#define DIRECTINPUT_VERSION		0x0800	//DirectInputのバージョン情報
#include <dinput.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

struct XboxButton
{
	bool DPAD_LEFT;			//direction left	(方向パッドの左)
	bool DPAD_RIGHT;		//direction right	(方向パッドの右)
	bool DPAD_UP;			//direction up		(方向パッドの上)
	bool DPAD_DOWN;			//direction down	(方向パッドの下)

	int LEFT_STICK_X;		//left stickX		(左スティックx -1000~1000)
	int LEFT_STICK_Y;		//left stickY		(左スティックy -1000~1000)
	int RIGHT_STICK_X;		//right stickX		(右スティックx -1000~1000)
	int RIGHT_STICK_Y;		//right stickY		(右スティックy -1000~1000)

	bool LEFT_BUTTON;		//left button		(左ボタン)
	bool RIGHT_BUTTON;		//right button		(右ボタン)

	bool A_BUTTON;			//a button			(Aボタン)
	bool B_BUTTON;			//b button			(Aボタン)
	bool X_BUTTON;			//x button			(Aボタン)
	bool Y_BUTTON;			//y button			(Aボタン)

	bool MENU_BUTTON;		//menu button		(メニューボタン)
	bool VIEW_BUTTON;		//view button		(ビューボタン)

	int LEFT_TRIGGER;		//left trigger		(左トリガー)
	int RIGHT_TRIGGER;		//right trigger		(右トリガー)
};

struct MouseButton
{
	bool left;		//左クリック
	bool right;		//右クリック
	bool middle;	//真ん中

	int X;			//座標X
	int Y;			//座標Y
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
//マウス
//******************************************************
	static MouseButton				mb;
	static LPDIRECTINPUT8				DInput;		//DirectInputオブジェクト
	static LPDIRECTINPUTDEVICE8		DI_mouse;	//マウスデバイス
	static DIMOUSESTATE				mouse_state;//マウス状態
	static HWND hwnd;
	//DirectInputの初期化処理
	bool InitDInput(HINSTANCE hInstApp, HWND hWnd);
	//DirecInputの終了処理
	bool ReleaseDInput();
	//DirectInputのマウスデバイス用の初期化処理
	bool InitDInputMouse(HWND hWnd);
	//DirectInputのマウスデバイス用の解放処理
	bool ReleaseDInputMouse();
	//DirectInputのマウスデバイス情報取得処理
	static void GetMouseState(HWND hWnd);
	static void InputMouse();
	//カーソルの表示か非表示
	static void EnableCursor(bool enable, bool isShow = false);

	static InputDevice* GetInstance()
	{
		static InputDevice instance;
		return &instance;
	}
};

#define PAD (InputDevice::GetInstance())

