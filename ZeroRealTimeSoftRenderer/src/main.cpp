#include "core.h"
#include "scene.h"
#include "pipeline.h"
#include "platform/win_platform.h"
#include "math_ext.h"
#include "debug.h"
using namespace Math;


const int g_windows_width = 800;
const int g_windows_height = 600;

class ZeroRealTimeSoftEngine : public App
{
public:
	ZeroRealTimeSoftEngine() : App() 
	{
		m_color_buffer = std::make_shared<ColorBuffer>(g_windows_width, g_windows_height);
		m_z_buffer = std::make_shared<ZBuffer>(g_windows_width, g_windows_height);
	}

	virtual void Startup()
	{
		//m_scene = std::make_shared<Scene_HelloTriangle>();
		m_scene = std::make_shared<Scene_Skybox>();
		//m_scene = std::make_shared<Scene_Model>();
		m_scene->GenerateScene(m_color_buffer, m_z_buffer);
	}

	virtual void Render(float delta_time)
	{
			m_color_buffer->ClearColorBuffer({ 0.0f, 0.0f, 0.0f, 1.0f });
			m_z_buffer->ClearZBuffer();
			m_scene->Render(delta_time);
			DrawWindow(m_color_buffer);
			first = false;
	}
	
	virtual void ShutDown()
	{
	}
private:
	std::shared_ptr<Scene> m_scene;
	std::shared_ptr<ColorBuffer>  m_color_buffer;
	std::shared_ptr<ZBuffer> m_z_buffer;
	bool first = true;
};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	AllocConsole();
	SetConsoleTitle("debug console");
	freopen("CONOUT$", "w", stdout);
	std::cout << "Hello Zero Real Time Soft Engine!\n";
	


	WindowsParameters windows_parameters = { hInstance, hPrevInstance, pCmdLine, nCmdShow, g_windows_width, g_windows_height };
	ZeroRealTimeSoftEngine* app = new ZeroRealTimeSoftEngine();
		

	app->Init(windows_parameters);

	app->Startup();

	app->Run();

	delete app;
	
	return 0;
}


