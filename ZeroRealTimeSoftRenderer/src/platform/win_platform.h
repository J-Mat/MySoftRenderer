#pragma once
#include "core.h"
#include <Windows.h>
#pragma comment(lib, "winmm.lib ")

using namespace Math;


struct WindowsParameters
{
	WindowsParameters() = default;
	HINSTANCE hInstance;
	HINSTANCE hPrevInstance;
	PWSTR pCmdLine;
	int nCmdShow;
	int width;
	int height;
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
	bool keys[0xff];
	char buttons[3];
};

class App
{
public:
	App();
	virtual ~App() = default;
	
	static  App* GetApp() { return s_app; }

	WindowInfo& GetWindowInfo() { return m_window_info; };
	
	ivec2 GetWindowsSize() { return { m_window_info.width, m_window_info.height }; }

	virtual void Init(const WindowsParameters& windows_parameters);
	
	virtual void Startup() = 0;

	virtual void Run();
	
	virtual void Render(float delta_time) = 0;
	
	virtual void ShutDown();
		
	void DrawWindow(std::shared_ptr<ColorBuffer> framebuffer); 
private: 
	int GetBufferIndex(int x, int y) { return (x * m_window_info.width + y) * 4; };
	void RegisterWindows(const WindowsParameters& windows_parameters);
	void InitBitMap();
	void InitBitmapHeader(BITMAPINFOHEADER& bitmap_header);
	vec2 GetMousePos();
private: 
	static App* s_app;
	WindowInfo m_window_info;
};
