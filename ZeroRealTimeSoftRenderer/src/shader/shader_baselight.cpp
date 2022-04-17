#include "shader.h"
#include "pipeline.h"
#include "debug.h"
#include "utils.h"


vec3 Fresenlschlick(float h_dot_v, const vec3& f0)
{
	return f0 + (vec3(1.0f) - f0) * pow(1.0f - h_dot_v, 5.0f);
}


float DistributeGGX(vec3 n, vec3 h, float roughness)
{
	float a2 = roughness * roughness;
	float n_dot_h = Math::max(dot(n, h), 0.0f);

	float nom = a2;
	float denom = n_dot_h * n_dot_h * (a2 - 1.0f) + 1.0f;
	denom = Math::pi<float>() * denom * denom;

	return nom / denom;
}


float GeometrySchlickGGX(float n_dot_v, float k)
{
	float nom = n_dot_v;
	float denom = n_dot_v * (1.0f - k) + k;
	return nom / denom;
}


float GeometrySmith(vec3 n, vec3 v, vec3 l, float k)
{
	float n_dot_v = Math::max(dot(n, v), 0.0f);
	float n_dot_l = Math::max(dot(n, l), 0.0f);
	float ggx1 = GeometrySchlickGGX(n_dot_v, k);
	float ggx2 = GeometrySchlickGGX(n_dot_l, k);

	return ggx1 * ggx2;
}

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
	// һЩ׼������
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

	// �ֱ����ÿ����
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