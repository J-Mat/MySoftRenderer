#pragma once
#include "math_ext.h"
#include <functional>
using namespace Math;

//  定义一套接口，在platform中实现

enum ButtonType
{
	BT_Left,
	BT_Mid,
	BT_Right,
};


class Input
{
public:
	static std::function<void(void)> OnLeftBtnDownFunc;
	static std::function<void(void)> OnLeftBtnUpFunc;
	static bool IsKeyPressed(char key);
	static void SetStartPos(vec2 pos);
	static void SetEndtPos(vec2 pos);
	static bool IsLeftButtonDown();
	
	static void OnLeftBtnDown();
	static void OnLeftBtnUp();

	static vec2 GetMouseOffset();


private:
	static vec2 s_beg_pos;
	static vec2 s_end_pos;
};




