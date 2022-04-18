#include "shader.h"
#include "pipeline.h"
#include "debug.h"
#include "utils.h"


void Shader_PBR::VertexShader(int vertex_idx)
{
	const mat4& model = GetUniform().model_mat;
	const mat4& view = GetUniform().view_mat;
	const mat4& projection = GetUniform().project_mat;

	const vec3& pos = GetClipAttribute().pos[vertex_idx];
	//const vec3& normal = GetClipAttribute().normals[vertex_idx];
	// MVP
	GetClipAttribute().world_pos[vertex_idx] = model * vec4(pos, 1.0);
	//GetClipAttribute().normals[vertex_idx] = mat3(transpose(inverse(model))) * normal;
	GetClipAttribute().ndc_coord[vertex_idx] = projection * view * vec4(GetClipAttribute().world_pos[vertex_idx], 1.0f);
}

// 船部都是抄了learnopengl的
bool Shader_PBR::FragmentShader(float alpha, float beta, float gamma)
{
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
	vec3 half = normalize(light_dir + view_dir);

	float n_dot_v = Math::max(dot(normal, view_dir), 0.0f);
	float n_dot_h = Math::max(dot(normal, half), 0.0f);
	float h_dot_v = Math::max(dot(half, view_dir), 0.0f);
	float n_dot_l = Math::max(dot(normal, light_dir), 0.0f);

	float roughness = TEXTURE(Roughness, texcoord);
	float metalness = TEXTURE(Metalness, texcoord);
	float occlusion = TEXTURE(Occlusion, texcoord);
	vec3 emission = TEXTURE(Emission, texcoord);
	vec3 albedo = base_color;

	// 相当于glsl的mix, hlsl的lerp， 就是插值
	vec3 F0 = mix(vec3(0.04f), albedo, metalness);

	// Cook-Torrance BRDF
	float NDF = Utils::DistributeGGX(normal, half, roughness);
	float G = Utils::GeometrySmith(normal, view_dir, light_dir, roughness);
	vec3 F = Utils::Fresenlschlick(clamp(dot(half, view_dir), 0.0f, 1.0f), F0);


	vec3 nom = NDF * G * F;
	float denom = 4.0f * n_dot_v * n_dot_l + 1e-4;
	vec3 specular = nom / denom;

	// kS is equal to Fresnel
	vec3 ks = F;
	// for energy conservation, the diffuse and specular light can't
	// be above 1.0 (unless the surface emits light); to preserve this
	// relationship the diffuse component (kD) should equal 1.0 - kS.
	vec3 kd = vec3(1.0f) - ks;

	vec3 Lo = (kd * albedo / PI + specular) * m_dir_light.radiance * n_dot_l;


	// ambient lighting (note that the next IBL tutorial will replace 
	// this ambient lighting with environment lighting).
	vec3 ambient = vec3(0.03f) * albedo * occlusion;
	vec3 color = ambient + Lo;
	//color = clamp(color, vec3(0.0f), vec3(1.0f));
	color = clamp(color, vec3(0.0f), vec3(1.0f));

	// gamma correct
	color = pow(color, vec3(1.0 / 2.2));

	frag_color = vec4(color, 1.0f);

	return true;
}


