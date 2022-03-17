#pragma once

#include "math_ext.h"
#include "tgaimage.h"
#include "shader/shader.h"
using namespace Math;

namespace Utils
{
	vec3 ConvertPixel2Color(TGAColor color);
	vec3 TextureSample(vec2 texcorrd, TGAImage* image);
	void CalculateCubeMapUV(vec3 direction, vec2& texcoord, int& face_index);
	vec3 CubemapSample(vec3 direction, Cubemap* cubemap);
}