#pragma once
#include "math_ext.h"
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
	static bool IsKeyPressed(char key);
	static vec2 GetMouseOffset();
	static void SetMousePos(int x, int y);

private: 
	static int s_cur_x;
	static int s_cur_y;
	static int s_last_x;
	static int s_last_y;
};



