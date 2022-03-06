#include "win_platform.h"
#include <pipeline.h>
#include <assert.h>
#include "iostream"
#include <algorithm>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM  wParam, LPARAM lParam);


void App::Init(const WindowsParameters& windows_parameters)
{
	m_window_info.width = windows_parameters.width;
	m_window_info.height = windows_parameters.height;

	RegisterWindows(windows_parameters);

	InitBitMap();

	RECT rect = { 0, 0, m_window_info.width, m_window_info.height };

	// GetWindowLong �õ�����ԭ��������; AdjustWindowRect��������ԭ���Ĵ�С
	AdjustWindowRect(&rect, GetWindowLong(m_window_info.hwnd, GWL_STYLE), 0);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	int start_x = (width - GetSystemMetrics(SM_CXSIZE));
	int start_y = (height - GetSystemMetrics(SM_CYSIZE));
	start_y = max(0, start_y);

	
	SetWindowPos(m_window_info.hwnd, nullptr, start_x, start_y, width, height, (SWP_NOCOPYBITS | SWP_NOZORDER | SWP_SHOWWINDOW));
	SetForegroundWindow(m_window_info.hwnd);
	ShowWindow(m_window_info.hwnd, SW_NORMAL);
	
	memset(m_window_info.frame_buffer, 0, m_window_info.width * m_window_info.height * 4);
	memset(m_window_info.keys, 0, sizeof(char) * 512);
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

void App::DrawWindow(std::shared_ptr<ColorBuffer> framebuffer)
{
	for (int i = 0; i < m_window_info.height; ++i)
	{
		for (int j = 0; j < m_window_info.width; ++j)
		{
			int index = GetBufferIndex(i, j);
			TGAColor color = framebuffer->GetPixel(i, j);
			m_window_info.frame_buffer[index + 2] = color.bgra[0];
			m_window_info.frame_buffer[index + 1] = color.bgra[1];
			m_window_info.frame_buffer[index + 0] = color.bgra[2];
		}
	}
	
	HDC hdc = GetDC(m_window_info.hwnd);
	BitBlt(hdc, 0, 0, m_window_info.width, m_window_info.height, m_window_info.hdc, 0, 0, SRCCOPY);
	ReleaseDC(m_window_info.hwnd, hdc);
}



LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM  wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_CHAR:
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
	bitmap_header.biHeight = m_window_info.height;
	bitmap_header.biPlanes = 1;
	bitmap_header.biBitCount = 32;
	bitmap_header.biCompression = BI_RGB;
	bitmap_header.biSizeImage = m_window_info.width * m_window_info.height * 4;
}


void App::InitBitMap()
{
	BITMAPINFOHEADER bitmap_header;
	InitBitmapHeader(bitmap_header);
	HDC hdc = GetDC(m_window_info.hwnd);
	m_window_info.hdc = CreateCompatibleDC(hdc);
	ReleaseDC(m_window_info.hwnd, hdc);
	
	LPVOID ptr;
	m_window_info.hbitmap = CreateDIBSection(m_window_info.hdc,  (BITMAPINFO*)&bitmap_header, DIB_RGB_COLORS, &ptr, 0, 0);
	assert(m_window_info.hbitmap != nullptr);
	
	// ���´�����λͼ���д��mem_dc
	m_window_info.old_hbitmap = (HBITMAP)SelectObject(m_window_info.hdc, m_window_info.hbitmap);
	m_window_info.frame_buffer = (unsigned char*)ptr;
}



