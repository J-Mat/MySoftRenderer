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

	InitBitMap();


	ShowWindow(m_window_info.hwnd, windows_parameters.nCmdShow);
}

void App::Run(App* app)
{
	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		Render();
	}
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
	bitmap_header.biSize = sizeof(BITMAPINFOHEADER);
	bitmap_header.biWidth = m_window_info.width;
	bitmap_header.biHeight = -m_window_info.height;
	bitmap_header.biPlanes = 1;
}


void App::InitBitMap()
{
	BITMAPINFOHEADER bitmap_header;
	InitBitmapHeader(bitmap_header);
	HDC hdc = GetDC(m_window_info.hwnd);
	m_window_info.hdc = CreateCompatibleDC(hdc);
	ReleaseDC(m_window_info.hwnd, hdc);
	
	LPVOID ptr;
	m_window_info.hbitmap = CreateDIBSection(m_window_info.hdc, (BITMAPINFOHEADER*)&bitmap_header, DIB_RGB_COLORS, &ptr, 0, 0);
	assert(m_window_info.hbitmap != nullptr);
	
	m_window_info.old_hbitmap = (HBITMAP)SelectObject(m_window_info.hdc, m_window_info.hbitmap);
	m_window_info.frame_buffer = (unsigned char*)ptr;
}



