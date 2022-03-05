#include "framebuffer.h"
#include <iostream>

FrameBuffer::FrameBuffer(int width, int height, int chanels) :
	m_width(width),
	m_height(height)
{
	m_buffer = (unsigned char*)malloc(sizeof(unsigned char) * width * height * chanels);
}

FrameBuffer::~FrameBuffer()
{
	delete[] m_buffer;
}

void FrameBuffer::SetPixel(int x, int y, const TGAColor& color)
{
	int index = GetIndex(x, y); 
	for (int i = 0;i < 4; ++i)
	{
		m_buffer[index + i] = color.bgra[i];
	}
}

