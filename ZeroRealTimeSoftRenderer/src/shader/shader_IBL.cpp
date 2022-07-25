#include "shader.h"
#include "pipeline.h"
#include "debug.h"
#include "utils.h"
using namespace Math;


void Shader_IBL::VertexShader(int vertex_idx)
{
	const mat4& model = GetUniform().model_mat;
	const mat4& view = GetUniform().view_mat;
	const mat4& projection = GetUniform().project_mat;

	const vec3 pos = GetClipAttribute().pos[vertex_idx];
	const vec3 normal = GetClipAttribute().normals[vertex_idx];
	// MVP
	GetClipAttribute().world_pos[vertex_idx] = pos;
	GetClipAttribute().normals[vertex_idx] = mat3(transpose(inverse(model))) * normal;
	GetClipAttribute().ndc_coord[vertex_idx] = projection * view * vec4(GetClipAttribute().world_pos[vertex_idx], 1.0f);
}

bool Shader_IBL::FragmentShader(float alpha, float beta, float gamma)
{
	vec3 view_pos = GetUniform().view_pos;
	vec3 world_pos = GET_BA_VALUE(vec3, GetAttribute().world_pos);
	vec2 texcoord = GET_BA_VALUE(vec2, GetAttribute().texcoord);
	vec3 normal = GET_BA_VALUE(vec3, GetAttribute().normals);
	normal = normalize(normal);
	/*
	if (Pipeline::GetBindVAO()->m_normal_map != nullptr)
	{
		vec3 T, B, N;
		Math::GetTBN(T, B, N, normal, GetAttribute().texcoord, GetAttribute().world_pos);
		vec3 normal_map = TEXTURE(Normal, texcoord);
		normal_map = Math::Remap<vec3>(normal_map, vec3(0.0f), vec3(1.0f), vec3(-1.0f), vec3(1.0f));
		normal = mat3(T, B, N) * normal_map;
		normal = normalize(normal);
	}
	*/
	vec3 n = normalize(normal);
	vec3 v = normalize(view_pos - world_pos);
	//vec3 base_color = TEXTURE(Diffuse, texcoord);
	float n_dot_v = Math::max(dot(n, v), 0.0f);
	vec3 color(0.0f);

	if (n_dot_v > 0)
	{
		float roughness = TEXTURE(Roughness, texcoord);
		float metalness = TEXTURE(Metalness, texcoord);
		float occlusion = TEXTURE(Occlusion, texcoord);
		vec3 emission = TEXTURE(Emission, texcoord);

		//get albedo
		vec3 albedo = TEXTURE(Diffuse, texcoord);;
		vec3 temp = vec3(0.04f, 0.04f, 0.04f);
		vec3 temp2 = vec3(1.0f, 1.0f, 1.0f);
		vec3 f0 = mix(temp, albedo, metalness);

		vec3 F = Utils::Fresenlschlick_Roughness(n_dot_v, f0, roughness);
		vec3 kD = (vec3(1.0f, 1.0f, 1.0f) - F) * (1 - metalness);

		//diffuse color
		vec3 irradiance = Utils::CubemapSample(n, GetIBLMap().irradiance_map);
		for (int i = 0; i < 3; i++)
			irradiance[i] = pow(irradiance[i], 2.0f);
		vec3 diffuse = irradiance * kD * albedo;

		//specular color
		vec3 r = normalize(2.0f * dot(v, n) * n - v);
		vec2 lut_uv = vec2(n_dot_v, roughness);
		vec3 lut_sample = Utils::TextureSample(lut_uv, &GetIBLMap().brdf_lut);
		float specular_scale = lut_sample.x;
		float specular_bias = lut_sample.y;
		vec3 specular = f0 * specular_scale + vec3(specular_bias, specular_bias, specular_bias);
		float max_mip_level = (float)(GetIBLMap().mip_levels - 1);
		int specular_miplevel = (int)(roughness * max_mip_level + 0.5f);
		vec3 prefilter_color = Utils::CubemapSample(r, GetIBLMap().prefilter_maps[specular_miplevel]);
		for (int i = 0; i < 3; i++)
			prefilter_color[i] = pow(prefilter_color[i], 2.0f);
		specular = prefilter_color *  specular;

		color = (diffuse + specular) + emission;
		for (int i = 0; i < 3; ++i)
		{
			color[i] = clamp(color[i], 0.0f, 1.0f);
		}
	}

	// gamma correct
	color = pow(color, vec3(1.0 / 2.2));

	frag_color = vec4(color, 1.0f);

	return true;
}