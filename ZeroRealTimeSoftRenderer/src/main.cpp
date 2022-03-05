#include <iostream>
#include "win_platform.h"
#include "pipeline.h"
#include "tgaimage.h"

const TGAColor Red = TGAColor(255, 0, 0);

class ZeroRealTimeSoftEngine : public App
{
public:
	ZeroRealTimeSoftEngine() : App() 
	{
		m_framebuffer = (unsigned char*)malloc(sizeof(unsigned char) * Pipeline::WINDOWS_WIDTH * Pipeline::WINDOWS_HEIGHT * 4);
		memset(m_framebuffer, 0, sizeof(unsigned char) * Pipeline::WINDOWS_HEIGHT * Pipeline::WINDOWS_WIDTH * 4);
	}

	virtual void Startup()
	{
	}

	virtual void Render()
	{
		Pipeline::ClearFrameBuffer(m_framebuffer, Red);
		DrawWindow(m_framebuffer);
	}
	
	virtual void ShutDown()
	{

	}
private:
	unsigned char* m_framebuffer;
};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	AllocConsole();
	SetConsoleTitle("debug console");
	freopen("CONOUT$", "w", stdout);
	printf("Hello\r\n");//可以在控制台上输出
	std::cout << "Hello Zero Real Time Soft Engine!";
	
	WindowsParameters windows_parameters = { hInstance, hPrevInstance, pCmdLine, nCmdShow };
	ZeroRealTimeSoftEngine* app = new ZeroRealTimeSoftEngine();
		

	app->Init(windows_parameters);

	app->Run(app);

	delete app;
	
	return 0;
}


