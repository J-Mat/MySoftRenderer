#pragma once 
#include "tgaimage.h"
#include <vector>
#include "maths.h"


namespace Pipeline
{
	const int WINDOWS_WIDTH = 800;
	const int WINDOWS_HEIGHT = 600;
	inline int GetIndex(int x, int y);
	void ClearFrameBuffer(unsigned char* framebuffer, const TGAColor& color);
	void DrawTriangle(unsigned char* framebuffer, e ye );
};

