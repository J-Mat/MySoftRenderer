#include "pipeline.h"

namespace Pipeline
{

	int GetIndex(int x, int y)
	{
		return (x * WINDOWS_WIDTH + y) * 4;
	}

	void ClearFrameBuffer(unsigned char* framebuffer, const TGAColor& color)
	{
		for (int i = 0; i < WINDOWS_HEIGHT; ++i)
		{
			for (int j = 0; j < WINDOWS_WIDTH; ++j)
			{
				int index = Pipeline::GetIndex(i, j);
				for (int k = 0; k < 3; ++k)
				{
					framebuffer[index + k] = color.bgra[k];
				}
			}
		}
	}
};
