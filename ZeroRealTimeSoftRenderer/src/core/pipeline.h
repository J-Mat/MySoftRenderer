#pragma once 
#include "core.h"

using namespace Math;

namespace Pipeline
{
	vec3 GetBarycentric(const vec2& A, const vec2& B, const vec2& C, const vec2& P);
	void GetBoundingBox(ivec2& min_box, ivec2& max_box, std::vector<vec4>& pts);
	void DrawTriangle(std::shared_ptr<ColorBuffer> framebuffer, std::vector<glm::vec4>& pts);
};

