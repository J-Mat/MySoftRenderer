#include "shader.h"
#include "pipeline.h"
#include "debug.h"

#define TEXTURE(Type, texcoord)  Pipeline::GetBindVAO()->Type(texcoord)

void Shader_HelloTriangle::VertexShader(int vertex_idx)
{
}

bool Shader_HelloTriangle::FragmentShader(float alpha, float beta, float gamma)
{
	frag_color = GET_BA_VALUE(Color, m_attribute.colors);
	return true;
}

void Shader_BaseLight::VertexShader(int vertex_idx)
{	
	const mat4& model = m_uniform.model_mat;
	const mat4& view = m_uniform.view_mat;
	const mat4& projection = m_uniform.perspect_mat;
	
	const vec3& pos = m_attribute.pos[vertex_idx];
	const vec3& normal = m_attribute.normals[vertex_idx];
	// MVP
	m_attribute.world_pos[vertex_idx] = model * vec4(pos, 1.0);
	m_attribute.normals[vertex_idx] = mat3(transpose(inverse(model)))* normal;
	m_attribute.ndc_coord[vertex_idx] = projection * view * vec4(pos, 1.0);
}

bool Shader_BaseLight::FragmentShader(float alpha, float beta, float gamma)
{
	// 一些准备工作
	const vec3& view_pos = m_uniform.view_pos;
	vec3 world_pos = GET_BA_VALUE(vec3, m_attribute.world_pos);
	vec3 normal = GET_BA_VALUE(vec3, m_attribute.normals);
	normal = normalize(normal);
	vec3 view_dir = normalize(view_pos - world_pos);
	vec2 texcoord = GET_BA_VALUE(vec2, m_attribute.texcoord);
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
	
	frag_color = vec4(ambient + diffuse + specular, 1.0f);
	return true;
}

