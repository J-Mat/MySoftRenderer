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
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM  wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		TextOut(hdc, 0, 0, "Hello, Windows!", 15);
		EndPaint(hwnd, &ps);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
