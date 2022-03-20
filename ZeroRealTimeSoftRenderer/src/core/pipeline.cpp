#include "pipeline.h"
#include "shader/shader.h"
#include "debug.h"

std::shared_ptr<ColorBuffer>  Pipeline::s_color_buffer = nullptr;
std::shared_ptr<ZBuffer>      Pipeline::s_zbuffer = nullptr;
std::shared_ptr<IShader>      Pipeline::s_shader = nullptr;
std::shared_ptr<VAO>		  Pipeline::s_vao = nullptr;

ivec2 Pipeline::GetSreenCoord(vec4 ndc_coord)
{
	//float y = Math::clamp(ndc_coord.y / ndc_coord.w, -5.0f, 5.0f);
	//float x = Math::clamp(ndc_coord.x / ndc_coord.w, -5.0f, 5.0f);
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
	s_shader->m_cur_face_idx = face_idx;
	for (int i = 0; i < 3; ++i)
	{	
		s_shader->m_attribute.pos[i] = s_vao->Position(face_idx, i);
		s_shader->m_attribute.texcoord[i] = s_vao->Texcoord(face_idx, i);
		s_shader->m_attribute.normals[i] = s_vao->Normal(face_idx, i);
	}
}

void  Pipeline::GetBoundingBox(ivec2& min_box, ivec2& max_box)
{
	for (ivec2 scrren_coord : s_shader->m_attribute.screen_coord)
	{
		scrren_coord.x = clamp(scrren_coord.x ,0, s_color_buffer->GetHeight() - 1);
		scrren_coord.y = clamp(scrren_coord.y ,0, s_color_buffer->GetWidth() - 1);
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

		s_shader->m_attribute.screen_coord[i] = GetSreenCoord(s_shader->m_attribute.ndc_coord[i]);

		//ivec2 pos = { x, y };
		//DEBUG_INFO("screenpos: "); DEBUG_POS2(s_shader->m_attribute.screen_coord[i]);
		//DEBUG_INFO("------------------\n");
	}
}

bool Pipeline::VisibleClip()
{
	return true;
	vec3 face_normal = s_vao->FaceNormal(s_shader->m_cur_face_idx);
	vec3 world_normal = vec3(s_shader->m_uniform.model_mat * vec4(face_normal, 0.0f));
	const mat4& view_mat = s_shader->m_uniform.view_mat;
	vec3 forward = { view_mat[2][0], view_mat[2][1], view_mat[2][2] };
	return dot(forward, world_normal) > 0.0f;
}


void  Pipeline::RunFragmentStage()
{
	float z_value[3];
	vec2 ndc[3];
	for (int i = 0; i < 3; ++i) 
	{
		z_value[i] = s_shader->m_attribute.ndc_coord[i].z / s_shader->m_attribute.ndc_coord[i].w;
		ndc[i] = { s_shader->m_attribute.ndc_coord[i].x / s_shader->m_attribute.ndc_coord[i].w,
				   s_shader->m_attribute.ndc_coord[i].y / s_shader->m_attribute.ndc_coord[i].w,
		};
		
		DEBUG_INFO("------------------------\n");
		DEBUG_POS2(ndc[i]);
		std::cout << z_value[i] << std::endl;
		std::cout << s_shader->m_attribute.ndc_coord[i].w << " " << s_shader->m_attribute.ndc_coord[i].z << "  " << -s_shader->m_attribute.ndc_coord[i].w << std::endl;
	
	}
	//std::cout << s_shader->m_attribute.ndc_coord[0].z / s_shader->m_attribute.ndc_coord[0].w << "  " << s_shader->m_attribute.ndc_coord[0].w << "\n";
	//std::cout << s_shader->m_attribute.ndc_coord[1].z / s_shader->m_attribute.ndc_coord[1].w << "  " << s_shader->m_attribute.ndc_coord[1].w << "\n";
	//std::cout << s_shader->m_attribute.ndc_coord[2].z / s_shader->m_attribute.ndc_coord[2].w << "  " << s_shader->m_attribute.ndc_coord[2].w << "\n";
	ivec2 min_box = { s_color_buffer->GetHeight() - 1,  s_color_buffer->GetWidth() - 1};
	ivec2 max_box = { 0, 0};
	//ivec2 min_box = { 1000,  1000};
	//ivec2 max_box = { -1000, -1000};
	GetBoundingBox(min_box, max_box);
	//DEBUG_INFO("min: ");DEBUG_POS2(min_box);
	//DEBUG_INFO("max: ");DEBUG_POS2(max_box);
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


			// https://zhuanlan.zhihu.com/p/403259571 Í¸ÊÓ½ÃÕý²åÖµ
			float alpha = barycentric_coord.x / z_value[0];
			float beta = barycentric_coord.y / z_value[1];
			float gamma = barycentric_coord.z / z_value[2];
			float z_n = 1.0f / (alpha + beta + gamma);
			alpha *= z_n;
			beta *= z_n;
			gamma *= z_n;

			//float z_ba = Math::Remap<float>(GET_BA_VALUE(float, z_value), -1.0f, 1.0f, 0.0f, 1.0);
			float z_ba = GET_BA_VALUE(float, z_value);
			if (s_zbuffer->WriteValue(P.x, P.y, z_ba) && s_shader->FragmentShader(alpha, beta, gamma))
			{
				s_color_buffer->SetPixel(x, y, s_shader->frag_color);
			}
		}
	}
}



