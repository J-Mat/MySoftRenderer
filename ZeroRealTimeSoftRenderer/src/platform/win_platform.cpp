#include "win_platform.h"
#include <pipeline.h>

void App::Init()
{
	m_window_info.width = WINDOWS_WIDTH;
	m_window_info.height = WINDOWS_HEIGHT;
	
	RECT rect = { 0, 0, m_window_info.width, m_window_info.height };
	
}

void App::Run(App* app)
{
	while (true)
	{
		Render();
	}
}

void App::Render()
{

}

void App::ShutDown()
{

}

void App::RegisterWindows()
{
	
}


