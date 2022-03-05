#pragma once
#include "core.h"

class FrameBuffer
{
public:
	FrameBuffer(int width, int height, int chanels);
	virtual ~FrameBuffer() = default;

	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	
	int GetIndex(int x, int y) { return (x * m_width + y) * m_chanels; };
protected:
	int m_width;
	int m_height;
	int m_chanels;
};

class ColorBuffer :public FrameBuffer
{
public:
	ColorBuffer(int width, int height);
	virtual ~ColorBuffer();

	void ClearColorBuffer(TGAColor color);
	void SetPixel(int x, int y, TGAColor color);
	const TGAColor& GetPixel(int x, int y);
private:
	unsigned char* m_buffer;
};

class ZBuffer :public FrameBuffer
{
public:
	ZBuffer(int width, int height);
	virtual ~ZBuffer();

	void ClearZBuffer();
	void SetValue(int x, int y, float value);
	float GetValue(int x, int y);
private:
	float* m_buffer;
};
