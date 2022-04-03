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
	return { (ndc_coord.y / ndc_coord.w / 2.0 + 0.5) * (s_color_buffer->GetHeight()),
			 (ndc_coord.x / ndc_coord.w / 2.0 + 0.5) * (s_color_buffer->GetWidth())};
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

float Pipeline::GetInsectRatio(vec4 pre_vertex, vec4 cur_vertex, ClipPlane plane)
{
	switch (plane)
	{
	case CP_WPlane:
		return (pre_vertex.w - EPSILON) / (pre_vertex.w - cur_vertex.w);
	case CP_Left:
		return (pre_vertex.w + pre_vertex.x) / ((pre_vertex.w + pre_vertex.x) - (cur_vertex.w + cur_vertex.x));
	case CP_Right:
		return (pre_vertex.w - pre_vertex.x) / ((pre_vertex.w - pre_vertex.x) - (cur_vertex.w - cur_vertex.x));
	case CP_Down:
		return (pre_vertex.w + pre_vertex.y) / ((pre_vertex.w + pre_vertex.y) - (cur_vertex.w + cur_vertex.y));
	case CP_Top:
		return (pre_vertex.w - pre_vertex.y) / ((pre_vertex.w - pre_vertex.y) - (cur_vertex.w - cur_vertex.y));
	case CP_Front:
		return (pre_vertex.w + pre_vertex.z) / ((pre_vertex.w + pre_vertex.z) - (cur_vertex.w + cur_vertex.z));
	case CP_Back:
		return (pre_vertex.w - pre_vertex.z) / ((pre_vertex.w - pre_vertex.z) - (cur_vertex.w - cur_vertex.z));
	default:
		break;
	}
	return 0.0f;
}

bool Pipeline::IsInsidePlane(ClipPlane plane, vec4 ndc_vertex)
{
	switch (plane)
	{
	case CP_WPlane:
		return ndc_vertex.w >= EPSILON;
	case CP_Left:
		return -ndc_vertex.w <= ndc_vertex.x;
		break;
	case CP_Right:
		return ndc_vertex.x <= ndc_vertex.w;
		break;
	case CP_Top:
		return ndc_vertex.y <= ndc_vertex.w;
		break;
	case CP_Down:
		return -ndc_vertex.w <= ndc_vertex.y;
		break;
	case CP_Front:
		return -ndc_vertex.w <= ndc_vertex.z;
		break;
	case CP_Back:
		return ndc_vertex.z <= ndc_vertex.w;	
		break;
	default:
		break;
	}
	return false;
}


int Pipeline::ClipThePlane(ClipPlane plane)
{
	int input = s_shader->m_cur_face_idx;
	int output = (input ^ 1);
	int clip_size = 0;
	auto& input_att = s_shader->GetClipAttributeByIndex(input);
	auto& output_att = s_shader->GetClipAttributeByIndex(output);
	for (int i = 0; i < s_shader->vertex_num; ++i)
	{
		int cur_idx = i;
		int pre_idx = (i - 1 + s_shader->vertex_num) % s_shader->vertex_num;
		
		vec4 cur_vertex = input_att.ndc_coord[cur_idx];
		vec4 pre_vertex = input_att.ndc_coord[pre_idx];
		
		bool cur_in_plane = IsInsidePlane(plane, cur_vertex);
		bool pre_in_plane = IsInsidePlane(plane, pre_vertex);
		
		// �д���
		if (cur_in_plane != pre_in_plane)
		{
			float ratio = GetInsectRatio(pre_vertex, cur_vertex, plane);
			output_att.ndc_coord[clip_size] = mix(input_att.ndc_coord[pre_idx], input_att.ndc_coord[cur_idx], ratio);
			output_att.screen_coord[clip_size] = mix(input_att.screen_coord[pre_idx], input_att.screen_coord[cur_idx], ratio);
			output_att.world_pos[clip_size] = mix(input_att.world_pos[pre_idx], input_att.world_pos[cur_idx], ratio);
			output_att.texcoord[clip_size] = mix(input_att.texcoord[pre_idx], input_att.texcoord[cur_idx], ratio);
			output_att.normals[clip_size] = mix(input_att.normals[pre_idx], input_att.normals[cur_idx], ratio);
			output_att.colors[clip_size] = mix(input_att.colors[pre_idx], input_att.colors[cur_idx], ratio);
			
			/*
			DEBUG_INFO("-------------------------------------------\n"); 
			DEBUG_INFO("ndc:  "); DEBUG_POS4(output_att.ndc_coord[clip_size]);
			DEBUG_INFO("src:  "); DEBUG_POS2(output_att.screen_coord[clip_size]);
			DEBUG_INFO("-------------------------------------------\n"); 
			*/

			clip_size++;
		}
		
		if (cur_in_plane)
		{
			output_att.ndc_coord[clip_size] = input_att.ndc_coord[cur_idx];
			output_att.screen_coord[clip_size] = input_att.screen_coord[cur_idx];
			output_att.world_pos[clip_size] = input_att.world_pos[cur_idx];
			output_att.texcoord[clip_size] = input_att.texcoord[cur_idx];
			output_att.normals[clip_size] = input_att.normals[cur_idx];
			output_att.colors[clip_size] = input_att.colors[cur_idx];


			/*
			DEBUG_INFO("-------------------------------------------\n"); 
			DEBUG_INFO("ndc:  "); DEBUG_POS4(output_att.ndc_coord[clip_size]);
			DEBUG_INFO("src:  "); DEBUG_POS2(output_att.screen_coord[clip_size]);
			DEBUG_INFO("-------------------------------------------\n"); 
			*/

			clip_size++;
		}
	}
	
	s_shader->vertex_num = clip_size;
	s_shader->m_cur_face_idx = output;
	//DEBUG_INFO("\n\n\n\n\n\n\n\n\n\n\n\n\n\\n\n\n\n\n\n\n\n\n");
	return 0;
}

void Pipeline::HomoClipping()
{
	for (int plane = ClipPlane::CP_Left; plane < ClipPlane::CP_PlaneNum; ++plane)
	{
		ClipThePlane((ClipPlane)plane);
	} 
}

void Pipeline::InitShaderAttribute(int face_idx)
{	
	s_shader->ResetAttribute();
	for (int i = 0; i < 3; ++i)
	{	
		s_shader->GetClipAttribute().pos[i] = s_vao->Position(face_idx, i);
		s_shader->GetClipAttribute().texcoord[i] = s_vao->Texcoord(face_idx, i);
		s_shader->GetClipAttribute().normals[i] = s_vao->Normal(face_idx, i);
	}
}

void Pipeline::CommitAttribute(int v0, int v1, int v2)
{
	int indexes[3] = { v0, v1, v2 };
	for (int i = 0; i < 3; ++i)
	{
		s_shader->GetAttribute().pos[i] = s_shader->GetClipAttribute().pos[indexes[i]];
		s_shader->GetAttribute().ndc_coord[i] = s_shader->GetClipAttribute().ndc_coord[indexes[i]];
		s_shader->GetAttribute().world_pos[i] = s_shader->GetClipAttribute().world_pos[indexes[i]];
		s_shader->GetAttribute().screen_coord[i] = s_shader->GetClipAttribute().screen_coord[indexes[i]];
		s_shader->GetAttribute().texcoord[i] = s_shader->GetClipAttribute().texcoord[indexes[i]];
		s_shader->GetAttribute().colors[i] = s_shader->GetClipAttribute().colors[indexes[i]];
		s_shader->GetAttribute().normals[i] = s_shader->GetClipAttribute().normals[indexes[i]];

	}
	/*
	auto& output_att = s_shader->GetAttribute();
	DEBUG_INFO("-------------------------------------------\n");
	DEBUG_INFO("--ndc:  "); DEBUG_POS4(output_att.ndc_coord[0]);
	DEBUG_INFO("--ndc:  "); DEBUG_POS4(output_att.ndc_coord[1]);
	DEBUG_INFO("--ndc:  "); DEBUG_POS4(output_att.ndc_coord[2]);

	DEBUG_INFO("--scr:  "); DEBUG_POS2(output_att.screen_coord[0]);
	DEBUG_INFO("--scr:  "); DEBUG_POS2(output_att.screen_coord[1]);
	DEBUG_INFO("--scr:  "); DEBUG_POS2(output_att.screen_coord[2]);
	DEBUG_INFO("-------------------------------------------\n");
	*/
}

void  Pipeline::GetBoundingBox(ivec2& min_box, ivec2& max_box)
{
	for (ivec2 scrren_coord : s_shader->GetAttribute().screen_coord)
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
	s_shader->ResetAttribute();
	for (int i = 0; i < 3; i++)
	{
		s_shader->VertexShader(i);
		s_shader->GetClipAttribute().screen_coord[i] = GetSreenCoord(s_shader->GetClipAttribute().ndc_coord[i]);
	}
}

bool Pipeline::VisibleClip()
{
	return true;
	vec3 face_normal = s_vao->FaceNormal(s_shader->m_cur_face_idx);
	vec3 world_normal = vec3(s_shader->GetUniform().model_mat * vec4(face_normal, 0.0f));
	const mat4& view_mat = s_shader->GetUniform().view_mat;
	vec3 forward = { view_mat[2][0], view_mat[2][1], view_mat[2][2] };
	return dot(forward, world_normal) > 0.0f;
}


void Pipeline::RunFragmentStage()
{	
	float z_value[3];
	vec3 ndc[3];
	//DEBUG_INFO("------------------------\n");
	for (int i = 0; i <  3; ++i) 
	{
		z_value[i] = s_shader->GetAttribute().ndc_coord[i].w;
		ndc[i] = { s_shader->GetAttribute().ndc_coord[i].x / s_shader->GetAttribute().ndc_coord[i].w,
				   s_shader->GetAttribute().ndc_coord[i].y / s_shader->GetAttribute().ndc_coord[i].w,
				   s_shader->GetAttribute().ndc_coord[i].z / s_shader->GetAttribute().ndc_coord[i].w,
		};
		
		//DEBUG_INFO("w     %d:  ", i); DEBUG_VALUE(z_value[i]);
		//DEBUG_INFO("ndc   %d:  ", i); DEBUG_POS3(ndc[i]);
	}
	ivec2 min_box = { s_color_buffer->GetHeight() - 1,  s_color_buffer->GetWidth() - 1};
	ivec2 max_box = { 0, 0};
	GetBoundingBox(min_box, max_box);
	//DEBUG_INFO("min    :  "); DEBUG_POS2(min_box);
	//DEBUG_INFO("max    :  "); DEBUG_POS2(max_box);
	
	//DEBUG_INFO("------------------------\n");
	for (int x = min_box.x; x <= max_box.x; ++x)
	{
		for (int y = min_box.y; y <= max_box.y; ++y)
		{
			ivec2 P = { x, y };
			vec3 barycentric_coord = Pipeline::GetBarycentric(
				s_shader->GetAttribute().screen_coord[0],
				s_shader->GetAttribute().screen_coord[1],
				s_shader->GetAttribute().screen_coord[2],
				P
			);
			if (barycentric_coord.x < 0.0f || barycentric_coord.y < 0.0f || barycentric_coord.z < 0.0f)
			{
				continue;
			}


			// https://zhuanlan.zhihu.com/p/403259571 ͸�ӽ�����ֵ
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



