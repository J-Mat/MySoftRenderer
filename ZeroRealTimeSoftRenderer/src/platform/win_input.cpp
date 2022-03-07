#include "input.h"
#include "win_platform.h"


int Input::s_cur_x = -1;
int Input::s_cur_y = -1;
int Input::s_last_x = -1;
int Input::s_last_y = -1;


// 键鼠的输入实现在这里
bool Input::IsKeyPressed(char key)
{
	return App::GetApp()->GetWindowInfo().keys[key];
}



glm::vec2 Input::GetMouseMovment()
{
	int x_offset = s_last_x < 0 ? 0 : s_cur_x - s_last_x;
	int y_offset = s_last_x < 0 ? 0 : s_cur_x - s_last_x;
	
	return Math::vec2(x_offset, y_offset);
}
