#include "pipeline.h"
#include "shader/shader.h"

vec3 Pipeline::GetBarycentric(const vec2& A, const vec2& B, const vec2& C, const vec2& P)
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

	void  Pipeline::GetBoundingBox(ivec2& min_box, ivec2& max_box, vec4* pts)
	{
		for (int i = 0;i < 3; ++i)
		{
			vec4 pt = pts[i];
			min_box.x = std::min(min_box.x, int(pt[0] / pt[3]));
			min_box.y = std::min(min_box.y, int(pt[1] / pt[3]));

			max_box.x = std::max(max_box.x, int(pt[0] / pt[3]));
			max_box.y = std::max(max_box.y, int(pt[1] / pt[3]));
		}
	}

	void  Pipeline::DrawTriangle(std::shared_ptr<IShader>shader, std::shared_ptr<ColorBuffer> framebuffer, vec4* pts)
	{
		ivec2 min_box = { framebuffer->GetWidth() - 1,  framebuffer->GetHeight() - 1};
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
				if (shader->fragment_shader(barycentric_coord.x, barycentric_coord.y, barycentric_coord.z))
				{
					framebuffer->SetPixel(x, y, shader->frag_color);
				}
			}
		}
	}

	void  Pipeline::DrawTriangle(std::shared_ptr<ColorBuffer> framebuffer, vec4* pts)
	{
		ivec2 min_box = { framebuffer->GetWidth() - 1,  framebuffer->GetHeight() - 1 };
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
				framebuffer->SetPixel(x, y, BLUE);
			}
		}
	}

