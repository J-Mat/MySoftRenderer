#pragma once 
#include "core.h"
#include "shader/shader.h"

using namespace Math;

class Pipeline
{
public: 
	static vec3 GetBarycentric(const vec2& A, const vec2& B, const vec2& C, const vec2& P);
	static void GetBoundingBox(ivec2& min_box, ivec2& max_box, vec4* pts);
	static void DrawTriangle(std::shared_ptr<IShader>shader, std::shared_ptr<ColorBuffer> framebuffer, vec4* pts);
	static void DrawTriangle(std::shared_ptr<ColorBuffer> framebuffer, vec4* pts);
};

