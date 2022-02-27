#include <iostream>
#include "win_platform.h"

class ZeroRealTimeSoftEngine : public App
{
public:
	ZeroRealTimeSoftEngine() : App() 
	{
	}

	virtual void Init()
	{
	}
	
	virtual void Render()
	{
	}
	
	virtual void ShutDown()
	{

	}
};

int main(int argc, const char** argv)
{
	std::cout << "Hello Zero Real Time Soft Engine!";
	
	return 0;
	ZeroRealTimeSoftEngine* app = new ZeroRealTimeSoftEngine();

	app->Run(app);

	delete app;
	
	return 0;
}


