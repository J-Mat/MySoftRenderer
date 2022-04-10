#pragma once

#include "math_ext.h"
#include "tgaimage.h"
using namespace Math;

typedef struct Cubemap
{
	TGAImage* faces[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
};

namespace Utils
{
	vec3 ConvertPixel2Color(TGAColor color);
	TGAColor ConvertColor2Pixel(vec3 color);
	vec3 TextureSample(vec2 texcorrd, TGAImage* image);
	void CalculateCubeMapUV(vec3 direction, vec2& texcoord, int& face_index);
	vec3 CubemapSample(vec3 direction, Cubemap* cubemap);
}