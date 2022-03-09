#pragma once
#include "tgaimage.h"
#include "math_ext.h"
using namespace Math;

#define  FLOAT_BUF(buffer) ((float*)buffer)

class FrameBuffer
{
public:
	FrameBuffer(int width, int height, size_t chanel_size, int chanels);
	virtual ~FrameBuffer();

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	
	int GetIndex(int x, int y) { return (x * m_width + y) * m_chanels; };
protected:
	int m_width;
	int m_height;
	int m_chanels;
	void* m_buffer;
};

class ColorBuffer :public FrameBuffer
{
public:
	ColorBuffer(int width, int height);

	void ClearColorBuffer(Color color);
	void SetPixel(int x, int y, Color color);
	Color GetPixel(int x, int y);
};

class ZBuffer :public FrameBuffer
{
public:
	ZBuffer(int width, int height);

	void ClearZBuffer();
	void SetValue(int x, int y, float value);
	float GetValue(int x, int y);
};
