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

	vec3 GetBarycentric(const vec2& A, const vec2& B, const vec2& C, const vec2& P)
	{
		vec3 x_vec = { B.x - A.x, C.x - A.x, A.x - P.x };
		vec3 y_vec = { B.y - A.y, C.y - A.y, A.y - P.y };
		vec3 s = glm::cross(x_vec, y_vec);
		if (std::abs(s.z) > 1e-8)
		{
			s = { s.x / s.z, s.y / s.z, 1.0f };
			return  { 1.0f - (s.x + s.y), s.x, s.y };
		}
		return { -1, 1, 1 };
	}
	}

	void GetBoundingBox(ivec2& min_box, ivec2& max_box, std::vector<vec4>& pts)
	{
		for (glm::vec4 pt : pts)
		{
			min_box.x = std::min(min_box.x, int(pt[0] / pt[3]));
			min_box.y = std::min(min_box.y, int(pt[1] / pt[3]));

			max_box.x = std::max(max_box.x, int(pt[0] / pt[3]));
			max_box.y = std::max(max_box.y, int(pt[1] / pt[3]));
		}
	}

	void DrawTriangle(unsigned char* framebuffer, std::vector<glm::vec4> pts)
	{
		ivec2 min_box = { Pipeline::WINDOWS_WIDTH - 1,  Pipeline::WINDOWS_HEIGHT - 1 };
		ivec2 max_box = { 0, 0 };
		GetBoundingBox(min_box, max_box, pts);
		for (int x = min_box.x; x <= max_box.x; ++x)
		{
			for (int y = min_box.y; y <= max_box.y; ++y)
			{
				ivec2 P = { x, y };
				vec3 barycentric_coord = Pipeline::GetBarycentric(vec2(pts[0] / pts[0].w), vec2(pts[1] / pts[1].w), vec2(pts[2] / pts[2].w), P);
				if (barycentric_coord.x < 0.0f || barycentric_coord.y < 0.0f || barycentric_coord.z < 0.0f)
				{
					continue;
				}
				float z_P = (pts[0][2] / pts[0][3]) * barycentric_coord.x + (pts[0][2] / pts[1][3]) * barycentric_coord.y + (pts[0][2] / pts[2][3]) * barycentric_coord.z;
				framebuffer[x][y]
			}
		}
	}

};
