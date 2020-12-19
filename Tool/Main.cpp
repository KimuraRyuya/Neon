#include <windows.h>
#include <memory>
#include <assert.h>
#include <tchar.h>
#include "FrameWork.h"

const LONG MAIN_SCREEN_WIDTH = 1920 / 2;
const LONG MAIN_SCREEN_HEIGHT = 1080 / 2;

LRESULT CALLBACK fnWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	FrameWork* f = reinterpret_cast<FrameWork*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	return f ? f->handle_message(hwnd, msg, wparam, lparam) : DefWindowProc(hwnd, msg, wparam, lparam);
}

INT WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev_instance, LPWSTR cmd_line, INT cmd_show)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//_CrtSetBreakAlloc(47591);
#endif
	//for (int i = 0; i < 1; ++i)
	//{
	//	cmd_line += wcslen(cmd_line) + 1;
	//}
	//MessageBox(NULL, cmd_line, TEXT("Kitty"), MB_ICONINFORMATION);

	//wchar_t path[256];
	//GetCurrentDirectory(256, path);
	//MessageBox(NULL, path, TEXT("Kitty"), MB_ICONINFORMATION);

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = fnWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = instance;
	wcex.hIcon = 0;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = _T("Editor");
	wcex.hIconSm = 0;
	RegisterClassEx(&wcex);

	RECT rc = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
	HWND hwnd = CreateWindow(_T("Editor"), _T(""), WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, NULL, NULL, instance, NULL);

#ifdef _DEBUG

#else
	HMONITOR hMonitor;
	MONITORINFOEX monitorInfoEx;
	POINT pt = { MAIN_SCREEN_WIDTH, MAIN_SCREEN_HEIGHT };

	// ptで指定部分のモニターのハンドルを取得する
	hMonitor = MonitorFromPoint(pt, MONITOR_DEFAULTTOPRIMARY);

	// モニター情報を取得する
	monitorInfoEx.cbSize = sizeof(monitorInfoEx);
	GetMonitorInfo(hMonitor, &monitorInfoEx);

	int x = monitorInfoEx.rcMonitor.left;
	int y = monitorInfoEx.rcMonitor.top;
	int w = monitorInfoEx.rcMonitor.right;
	int h = monitorInfoEx.rcMonitor.bottom;

	SetMenu(hwnd, NULL); // メニューを隠す

	SetWindowLong(hwnd, GWL_STYLE, WS_VISIBLE | WS_POPUP); // ウィンドウのスタイルを変更
	MoveWindow(hwnd, x, y, w, h, TRUE);

#endif // _DUBUG

	FrameWork& f = FrameWork::GetInstance(hwnd, instance);

	SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&f));
	return f.run();
}
