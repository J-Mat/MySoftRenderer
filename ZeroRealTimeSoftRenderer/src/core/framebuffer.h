#pragma once
#include "tgaimage.h"

class FrameBuffer
{
public:
	FrameBuffer(int width, int height, int chanels);
	~FrameBuffer();

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	
	int GetIndex(int x, int y) { return x * m_width + y; };
	void SetPixel(int x, int y, const TGAColor& color);

private:
	int m_width;
	int m_height;
	unsigned char* m_buffer;
};

