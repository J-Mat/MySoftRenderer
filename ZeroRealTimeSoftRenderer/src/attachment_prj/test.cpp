#include <iostream>
#include <windows.h>
using namespace std;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM  wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	// Register the window class.
	const char CLASS_NAME[] = "Sample Window Class";

	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hCursor = LoadCursor(0, IDC_ARROW);

	RegisterClass(&wc);

	// Create the window.

	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		"Learn to Program Windows",    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		nullptr,       // Parent window    
		nullptr,       // Menu
		hInstance,  // Instance handle
		nullptr        // Additional application data
	);

	if (hwnd == nullptr)
	{
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);

	// Run the message loop.

	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		cout << "xoxo0-00" << endl;
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	srand(time(NULL));

	return (int)msg.wParam;
}

void DrawPixels(HWND hwnd)
{
	PAINTSTRUCT ps;
	RECT r;
	
	GetClientRect(hwnd, &r);
	
	if (r.bottom == 0)
	{
		return;
	}
	HDC hdc = BeginPaint(hwnd, &ps);
	for (int i = 0; i < 1000; ++i)
	{
		int x = rand() % r.right;
		int y = rand() % r.bottom;
		
		SetPixel(hdc, x, y,	RGB(255, 0, 0));
	}
	EndPaint(hwnd, &ps);
}

void DrawLines(HWND hwnd)
{
	HDC hdc;
	PAINTSTRUCT ps;
	hdc = BeginPaint(hwnd, &ps);
	
	MoveToEx(hdc, 50, 50, nullptr);
	LineTo(hdc, 250, 50);

	HPEN hBlackPen = (HPEN)GetStockObject(BLACK_PEN);
	HPEN hOldPen = (HPEN)SelectObject(hdc, hBlackPen);

	EndPaint(hwnd, &ps);
}

void DrawRectangle(HWND hwnd)
{
	HDC hdc;
	PAINTSTRUCT ps;
	hdc = BeginPaint(hwnd, &ps);
	Rectangle(hdc, 50, 50, 700, 100);
	EndPaint(hwnd, &ps);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM  wParam, LPARAM lParam)
{
	static HBITMAP hBitmap;
	HDC hdc;
	PAINTSTRUCT ps;
	BITMAP bitmap;
	HDC hdcMem;
	HGDIOBJ oldBitmap;
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_CREATE:
		hBitmap = (HBITMAP)LoadImageW(NULL, L"C:\\prog\\slovakia.bmp",
			IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

		if (hBitmap == NULL) {
			MessageBoxW(hwnd, L"Failed to load image", L"Error", MB_OK);
		}

		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);

		hdcMem = CreateCompatibleDC(hdc);
		oldBitmap = SelectObject(hdcMem, hBitmap);

		GetObject(hBitmap, sizeof(bitmap), &bitmap);
		BitBlt(hdc, 5, 5, bitmap.bmWidth, bitmap.bmHeight,
			hdcMem, 0, 0, SRCCOPY);

		SelectObject(hdcMem, oldBitmap);
		DeleteDC(hdcMem);

		EndPaint(hwnd, &ps);

		break;
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

