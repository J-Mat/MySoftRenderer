#pragma once 
#include "tgaimage.h"
#include <vector>
#include <glm/glm.hpp>

using namespace glm;

namespace Pipeline
{
	const int WINDOWS_WIDTH = 800;
	const int WINDOWS_HEIGHT = 600;
	inline int GetIndex(int x, int y);
	void ClearFrameBuffer(unsigned char* framebuffer, const TGAColor& color);
	vec3 GetBarycentric(const vec2& A, const vec2& B, const vec2& C, const vec2& P);
	void GetBoundingBox(vec2& min_box, ivec2& max_box, std::vector<vec4>& pts);
	void DrawTriangle(unsigned char* framebuffer, std::vector<glm::vec4> pts);
};

