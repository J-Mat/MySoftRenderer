#pragma once
#include <Windows.h>

struct WindowInfo
{
	WindowInfo() = default;
	HWND h_windows;
	HDC  men_dc;
	HBITMAP bm_old;
	unsigned char* window_fb = nullptr;
	unsigned int width = 800;
	unsigned int height = 600;
	bool is_close = false;
};

class App
{
public:
	App() = default;
	virtual ~App() = default;

	virtual void Init();
	
	virtual void Run(App* app);
	
	virtual void Render();
	
	virtual void ShutDown();
private: 
	void RegisterWindows();
private: 
	static App* s_app;
	WindowInfo m_window_info;
};
