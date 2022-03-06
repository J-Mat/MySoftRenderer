#include "framebuffer.h"
#include <iostream>

FrameBuffer::FrameBuffer(int width, int height, size_t chanel_size, int chanels) :
	m_width(width),
	m_height(height),
	m_chanels(chanels)
{
	m_buffer = malloc(chanel_size * m_width * m_height * m_chanels);
	if (m_buffer != nullptr)
	{
		memset(m_buffer, 0, chanel_size * m_width * m_height * m_chanels);
	}
}



const TGAColor& ColorBuffer::GetPixel(int x, int y)
{
	int index = GetIndex(x, y);
	TGAColor color = { USCHAR_BUF(m_buffer)[index],  USCHAR_BUF(m_buffer)[index + 1],  USCHAR_BUF(m_buffer)[index + 2],  USCHAR_BUF(m_buffer)[index + 3] };
	return std::move(color);
}

ColorBuffer::ColorBuffer(int width, int height) :
	FrameBuffer(width, height, sizeof(unsigned char), 4)
{
}

ColorBuffer::~ColorBuffer()
{
	delete[] m_buffer;
}

void ColorBuffer::ClearColorBuffer(TGAColor color)
{
	for (int i = 0; i < m_height; ++i)
	{
		for (int j = 0; j < m_width; ++j)
		{
			SetPixel(i, j, color);
		}
	}
}

void ColorBuffer::SetPixel(int x, int y, TGAColor color)
{
	int index = GetIndex(x, y); 
	for (int i = 0;i < m_chanels; ++i)
	{
		USCHAR_BUF(m_buffer)[index + i] = color.bgra[i];
	}
}

ZBuffer::ZBuffer(int width, int height): 
	FrameBuffer(width, height, sizeof(float), 1)
{
	m_buffer = (float*)malloc(sizeof(float) * m_width * m_height * m_chanels);
	memset(m_buffer, 0, sizeof(float) * m_width * m_height * m_chanels);
}

ZBuffer::~ZBuffer()
{
	delete[] m_buffer;
}

void ZBuffer::ClearZBuffer()
{
	for (int i = 0; i < m_height; ++i)
	{
		for (int j = 0; j < m_width; ++j)
		{
			int index = GetIndex(i, j); 	
			FLOAT_BUF(m_buffer)[index] = 1000.0f;
		}
	}
}

void ZBuffer::SetValue(int x, int y, float value)
{
	int index = GetIndex(x, y);
	FLOAT_BUF(m_buffer)[index] = value;
}

float ZBuffer::GetValue(int x, int y)
{
	int index = GetIndex(x, y);
	return FLOAT_BUF(m_buffer)[index];
}
