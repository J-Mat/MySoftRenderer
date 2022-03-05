#pragma once
#include <Windows.h>
#include "tgaimage.h"


struct WindowsParameters
{
	WindowsParameters() = default;
	HINSTANCE hInstance;
	HINSTANCE hPrevInstance;
	PWSTR pCmdLine;
	int nCmdShow;
};

struct WindowInfo
{
	WindowInfo() = default;
	HWND hwnd;
	HDC  hdc;
	HBITMAP hbitmap;
	HBITMAP old_hbitmap;
	unsigned char* frame_buffer = nullptr;
	unsigned int width = 800;
	unsigned int height = 600;
	bool is_close = false;
	char keys[512];
	char buttons[2];
};

class App
{
public:
	App() = default;
	virtual ~App() = default;
	
	static  App* GetApp() { return s_app; }

	const WindowInfo& GetWindowInfo() { return m_window_info; };

	virtual void Init(const WindowsParameters& windows_parameters);
	
	virtual void Startup() = 0;

	virtual void Run(App* app);
	
	virtual void Render() = 0;
	
	virtual void ShutDown();
		
	void DrawWindow(unsigned char* framebuffer); 
private: 
	int GetBufferIndex(int x, int y) { return (x * m_window_info.width + y) * 4; };
	void RegisterWindows(const WindowsParameters& windows_parameters);
	void InitBitMap();
	void InitBitmapHeader(BITMAPINFOHEADER& bitmap_header);
private: 
	static App* s_app;
	WindowInfo m_window_info;
};
