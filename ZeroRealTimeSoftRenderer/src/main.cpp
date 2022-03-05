#include "core.h"
#include "pipeline.h"
#include "win_platform.h"


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
	}

	virtual void Render()
	{
		m_color_buffer->ClearColorBuffer(RED);
		std::vector<glm::vec4> pts = { {0, 0, 0, 1}, {0, 400, 0, 1}, {400, 400, 0, 1} };
		Pipeline::DrawTriangle(m_color_buffer, pts);
		DrawWindow(m_color_buffer);
	}
	
	virtual void ShutDown()
	{

	}
private:
	std::shared_ptr<ColorBuffer>  m_color_buffer;
	std::shared_ptr<ZBuffer> m_z_buffer;
};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	AllocConsole();
	SetConsoleTitle("debug console");
	freopen("CONOUT$", "w", stdout);
	std::cout << "Hello Zero Real Time Soft Engine!";
	
	WindowsParameters windows_parameters = { hInstance, hPrevInstance, pCmdLine, nCmdShow, g_windows_width, g_windows_height };
	ZeroRealTimeSoftEngine* app = new ZeroRealTimeSoftEngine();
		

	app->Init(windows_parameters);

	app->Run(app);

	delete app;
	
	return 0;
}


