#include "framebuffer.h"
#include <iostream>
#include "assert.h"

FrameBuffer::FrameBuffer(int width, int height, size_t chanel_size, int chanels) :
	m_width(width),
	m_height(height),
	m_chanels(chanels)
{
	size_t total_size = chanel_size * m_width * m_height * m_chanels;
	m_buffer = malloc(total_size);
	assert(m_buffer != nullptr);
	memset(m_buffer, 0, total_size);
}



FrameBuffer::~FrameBuffer()
{
	delete[] m_buffer;
}

Color ColorBuffer::GetPixel(int x, int y)
{
	int index = GetIndex(x, y);
	Color color = Color(FLOAT_BUF(m_buffer)[index], FLOAT_BUF(m_buffer)[index + 1], FLOAT_BUF(m_buffer)[index + 2], FLOAT_BUF(m_buffer)[index + 3]);
	return color;
}

ColorBuffer::ColorBuffer(int width, int height) :
	FrameBuffer(width, height, sizeof(float), 4)
{
}


void ColorBuffer::ClearColorBuffer(Color color)
{
	for (int i = 0; i < m_height; ++i)
	{
		for (int j = 0; j < m_width; ++j)
		{
			SetPixel(i, j, color);
		}
	}
}

void ColorBuffer::SetPixel(int x, int y, Color color)
{
	int index = GetIndex(x, y); 
	for (int i = 0;i < m_chanels; ++i)
	{	
		FLOAT_BUF(m_buffer)[index + i] = color[i];
	}
}

ZBuffer::ZBuffer(int width, int height): 
	FrameBuffer(width, height, sizeof(float), 1)
{
}


void ZBuffer::ClearZBuffer()
{
	for (int i = 0; i < m_height; ++i)
	{
		for (int j = 0; j < m_width; ++j)
		{
			int index = GetIndex(i, j); 	
			FLOAT_BUF(m_buffer)[index] = 10000.0f;
		}
	}
}

bool ZBuffer::WriteValue(int x, int y, float value)
{
	int index = GetIndex(x, y);
	float buffer_value = FLOAT_BUF(m_buffer)[index];
	if (value < buffer_value)
	{
		FLOAT_BUF(m_buffer)[index] = value;
		return true;
	}
	return false;
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
