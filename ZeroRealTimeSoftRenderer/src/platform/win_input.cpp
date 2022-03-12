#include "input.h"
#include "win_platform.h"
#include "debug.h"


vec2 Input::s_end_pos = {-1, -1};
vec2 Input::s_beg_pos = {-1, -1};

std::function<void(void)> Input::OnLeftBtnDownFunc = nullptr;
std::function<void(void)> Input::OnLeftBtnUpFunc = nullptr;

// 键鼠的输入实现在这里
bool Input::IsKeyPressed(char key)
{
	return App::GetApp()->GetWindowInfo().keys[key];
}

void Input::SetStartPos(vec2 pos)
{
	s_beg_pos = pos;
	s_end_pos = pos;
}

void Input::SetEndtPos(vec2 pos)
{
	s_end_pos = pos;
}

vec2 Input::GetMouseOffset()
{
	return s_end_pos - s_beg_pos;
}

bool Input::IsLeftButtonDown()
{
	return App::GetApp()->GetWindowInfo().buttons[ButtonType::BT_Left];
}

void Input::OnLeftBtnDown()
{
	OnLeftBtnDownFunc();
}

void Input::OnLeftBtnUp()
{
	OnLeftBtnUpFunc();
}
