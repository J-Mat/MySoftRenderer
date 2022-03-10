#include "pipeline.h"
#include "shader/shader.h"

std::shared_ptr<ColorBuffer>  Pipeline::s_color_buffer = nullptr;
std::shared_ptr<ZBuffer>      Pipeline::s_zbuffer = nullptr;
std::shared_ptr<IShader>      Pipeline::s_shader = nullptr;
std::shared_ptr<VAO>		  Pipeline::s_vao = nullptr;

ivec2 Pipeline::GetSreenCoord(vec4 ndc_coord)
{
	return { (ndc_coord.y / ndc_coord.w / 2.0 + 0.5) * s_color_buffer->GetHeight(),
			 (ndc_coord.x / ndc_coord.w / 2.0 + 0.5) * s_color_buffer->GetWidth()};
}


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

void Pipeline::InitShaderAttribute(int face_idx)
{	
	for (int i = 0; i < 3; ++i)
	{
		s_shader->m_attribute.pos[i] = s_vao->Position(face_idx, i);
		s_shader->m_attribute.normals[i] = s_vao->Normal(face_idx, i);
		s_shader->m_attribute.texcoord[i] = s_vao->Texcoord(face_idx, i);
	}
}

void  Pipeline::GetBoundingBox(ivec2& min_box, ivec2& max_box)
{
	for (ivec2 scrren_coord : s_shader->m_attribute.screen_coord)
	{
		min_box.x = std::min(min_box.x, scrren_coord.x);
		min_box.y = std::min(min_box.y, scrren_coord.y);

		max_box.x = std::max(max_box.x, scrren_coord.x);
		max_box.y = std::max(max_box.y, scrren_coord.y);
	}
}

void Pipeline::RunVertexStage()
{
	for (int i = 0; i < 3; i++)
	{
		s_shader->VertexShader(i);
	}
}

void Pipeline::NDC2ScreenCoord()
{
	for (int i = 0; i < 3; ++i)
	{
		s_shader->m_attribute.screen_coord[i] = GetSreenCoord(s_shader->m_attribute.ndc_coord[i]);
	}
}

void  Pipeline::RunFragmentStage()
{
	ivec2 min_box = { s_color_buffer->GetWidth() - 1,  s_color_buffer->GetHeight() - 1};
	ivec2 max_box = { 0, 0 };
	GetBoundingBox(min_box, max_box);
	for (int x = min_box.x; x <= max_box.x; ++x)
	{
		for (int y = min_box.y; y <= max_box.y; ++y)
		{
			ivec2 P = { x, y };
			vec3 barycentric_coord = Pipeline::GetBarycentric(
				s_shader->m_attribute.screen_coord[0],
				s_shader->m_attribute.screen_coord[1],
				s_shader->m_attribute.screen_coord[2],
				P
			);
			if (barycentric_coord.x < 0.0f || barycentric_coord.y < 0.0f || barycentric_coord.z < 0.0f)
			{
				continue;
			}
			//float z_P = (pts[0][2] / pts[0][3]) * barycentric_coord.x + (pts[0][2] / pts[1][3]) * barycentric_coord.y + (pts[0][2] / pts[2][3]) * barycentric_coord.z;
			if (s_shader->FragmentShader(barycentric_coord.x, barycentric_coord.y, barycentric_coord.z))
			{
				s_color_buffer->SetPixel(x, y, s_shader->frag_color);
			}
		}
	}
}



