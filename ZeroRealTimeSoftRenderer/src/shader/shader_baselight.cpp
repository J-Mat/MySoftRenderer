#include "shader.h"
#include "pipeline.h"
#include "debug.h"
#include "utils.h"


void Shader_BaseLight::VertexShader(int vertex_idx)
{
	const mat4& model = GetUniform().model_mat;
	const mat4& view = GetUniform().view_mat;
	const mat4& projection = GetUniform().project_mat;

	const vec3& pos = GetClipAttribute().pos[vertex_idx];
	const vec3& normal = GetClipAttribute().normals[vertex_idx];
	// MVP
	GetClipAttribute().world_pos[vertex_idx] = model * vec4(pos, 1.0);
	GetClipAttribute().normals[vertex_idx] = mat3(transpose(inverse(model))) * normal;
	GetClipAttribute().ndc_coord[vertex_idx] = projection * view * vec4(pos, 1.0);
}

bool Shader_BaseLight::FragmentShader(float alpha, float beta, float gamma)
{
	// 一些准备工作
	const vec3& view_pos = GetUniform().view_pos;
	vec3 world_pos = GET_BA_VALUE(vec3, GetAttribute().world_pos);
	vec2 texcoord = GET_BA_VALUE(vec2, GetAttribute().texcoord);
	vec3 normal = GET_BA_VALUE(vec3, GetAttribute().normals);
	normal = normalize(normal);
	if (Pipeline::GetBindVAO()->m_normal_map != nullptr)
	{
		vec3 T, B, N;
		Math::GetTBN(T, B, N, normal, GetAttribute().texcoord, GetAttribute().world_pos);
		vec3 normal_map = TEXTURE(Normal, texcoord);
		normal_map = Math::Remap<vec3>(normal_map, vec3(0.0f), vec3(1.0f), vec3(-1.0f), vec3(1.0f));
		normal = mat3(T, B, N) * normal_map;
		normal = normalize(normal);
	}
	vec3 view_dir = normalize(view_pos - world_pos);
	vec3 base_color = TEXTURE(Diffuse, texcoord);
	vec3 light_dir = normalize(-m_dir_light.direction);

	// 分别计算每部分
	// diffuse shading 
	float diff = Math::max(dot(normal, light_dir), 0.0f);
	// specular shading
	vec3 reflect_dir = reflect(-light_dir, normal);
	float spec = pow(Math::max(dot(view_dir, reflect_dir), 0.0f), 5);
	// combine results
	vec3 ambient = m_dir_light.ambient * base_color;
	vec3 diffuse = m_dir_light.diffuse * diff * base_color;
	vec3 specular = m_dir_light.specular * spec;


	vec3 color = vec3(ambient + diffuse + specular);
	//color = base_color;
	color = pow(color, vec3(1.0 / 2.2));
	frag_color = vec4(color, 1.0f);
	frag_color = clamp(frag_color, vec4(0.0f), vec4(1.0f));
	return true;
}
