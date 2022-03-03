#include <iostream>
#include "win_platform.h"

class ZeroRealTimeSoftEngine : public App
{
public:
	ZeroRealTimeSoftEngine() : App() 
	{
	}

	
	virtual void Render()
	{
	}
	
	virtual void ShutDown()
	{

	}
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


