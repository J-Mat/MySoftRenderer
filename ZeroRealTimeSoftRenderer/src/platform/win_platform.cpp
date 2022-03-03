#include "win_platform.h"
#include <pipeline.h>
#include <assert.h>
#include "iostream"
using namespace std;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM  wParam, LPARAM lParam);

void App::Init(const WindowsParameters& windows_parameters)
{
	m_window_info.width = WINDOWS_WIDTH;
	m_window_info.height = WINDOWS_HEIGHT;

	RegisterWindows(windows_parameters);
	RECT rect = { 0, 0, m_window_info.width, m_window_info.height };
	
	ShowWindow(m_window_info.hwnd, windows_parameters.nCmdShow);
	
}

void App::Run(App* app)
{
	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		cout << "1122--------  "<< endl;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		Render();
	}
}

void App::Render()
{

}

void App::ShutDown()
{

}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM  wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


void App::RegisterWindows(const WindowsParameters& windows_parameters)
{	
	const char CLASS_NAME[] = "Sample Window Class";
	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = windows_parameters.hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hCursor = LoadCursor(0, IDC_ARROW);

	RegisterClass(&wc);

	m_window_info.hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		"Zero RealTime Soft Renderer",    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		nullptr,       // Parent window    
		nullptr,       // Menu
		windows_parameters.hInstance,  // Instance handle
		nullptr        // Additional application data
	);
	assert(m_window_info.hwnd != nullptr);
}

void App::InitBitmapHeader(BITMAPINFOHEADER& bitmap_header)
{
	memset(&bitmap_header, 0, sizeof(BITMAPINFOHEADER));
}

