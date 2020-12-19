#pragma once

#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <memory>

#include "Misc.h"
#include "HighResolutionTimer.h"

#include <d3d11.h>
#include <wrl.h>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx11.h>
#include <imgui_internal.h>

#include "Editor.h"

#define SCREEN_WIDTH	1920 / 2
#define SCREEN_HEIGHT	1080 / 2

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);

class FrameWork
{
private:
	const HWND hwnd;
	const HINSTANCE hinstance;
	Microsoft::WRL::ComPtr<ID3D11Device>			device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>		immediateContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain>			swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> defaultRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> defaultDepthStencilView;
	D3D11_VIEWPORT viewPort = {};

	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> defaultDepthStencilState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> defaultRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> defaultBlendState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> defaultSamplerState;

	float elapsedTime = 0.0f;

	std::unique_ptr<MainEditor> editor;
public:
	UINT screenWidth;
	UINT screenHeight;


	FrameWork(HWND hwnd, HINSTANCE hinstance) : hwnd(hwnd), hinstance(hinstance)
	{

	}
	~FrameWork()
	{

	}
	static FrameWork& GetInstance(HWND hwnd = nullptr, HINSTANCE hinstance = nullptr)
	{
		static FrameWork instance(hwnd, hinstance);
		return instance;
	}

	int run()
	{
		MSG msg = {};
		if (!initialize()) return 0;

		ImGui::SetCurrentContext(ImGui::CreateContext());
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(device.Get(), immediateContext.Get());
		ImGui::StyleColorsDark();
		ImGuiIO& io = ImGui::GetIO();
		io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\meiryo.ttc", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());



		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				timer.Tick();
				calculate_frame_stats();
				elapsedTime = timer.TimeInterval();

				update(elapsedTime);
				render(elapsedTime);
			}
		}

#ifdef _DEBUG

		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

#endif // _DEBUG

		finalize();
		return static_cast<int>(msg.wParam);
	}

	LRESULT CALLBACK handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam))
		{
			return 1;
		}

		switch (msg)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			break;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CREATE:
			break;
		case WM_KEYDOWN:
			if (wparam == VK_ESCAPE) PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		case WM_ENTERSIZEMOVE:
			// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
			timer.Stop();
			break;
		case WM_EXITSIZEMOVE:
			// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
			// Here we reset everything based on the new window dimensions.
			timer.Start();
			break;
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return 0;
	}

private:
	bool initialize();
	void finalize();
	void update(float elapsed_time/*Elapsed seconds from last frame*/);
	void render(float elapsed_time/*Elapsed seconds from last frame*/);

private:
	HighResolutionTimer timer;
	void calculate_frame_stats()
	{
		// Code computes the average frames per second, and also the 
		// average time it takes to render one frame.  These stats 
		// are appended to the window caption bar.
		static int frames = 0;
		static float time_tlapsed = 0.0f;

		frames++;

		// Compute averages over one second period.
		if ((timer.TimeStamp() - time_tlapsed) >= 1.0f)
		{
			float fps = static_cast<float>(frames); // fps = frameCnt / 1
			float mspf = 1000.0f / fps;
			std::ostringstream outs;
			outs.precision(6);
			outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
			SetWindowTextA(hwnd, outs.str().c_str());

			// Reset for next average.
			frames = 0;
			time_tlapsed += 1.0f;
		}
	}
public:
	inline static ID3D11Device* GetDevice()
	{
		return	FrameWork::GetInstance().device.Get();
	};
	inline static ID3D11DeviceContext* GetContext()
	{
		return	FrameWork::GetInstance().immediateContext.Get();
	};
	float GetElapsedTime() { return FrameWork::GetInstance().elapsedTime; }
};

#define pFrameWork	 ( FrameWork::GetInstance() )