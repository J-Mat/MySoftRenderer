#include "shader.h"
#include "pipeline.h"
#include "debug.h"
#include "utils.h"


void Shader_Skybox::VertexShader(int vertex_idx)
{
	const mat4& model = GetUniform().model_mat;
	const mat4& view = GetUniform().view_mat;
	const mat4& projection = GetUniform().project_mat;

	const vec3& pos = GetClipAttribute().pos[vertex_idx];
	const vec3& normal = GetClipAttribute().normals[vertex_idx];

	// 世界坐标不变
	GetClipAttribute().world_pos[vertex_idx] = pos;
	GetClipAttribute().normals[vertex_idx] = mat3(transpose(inverse(model))) * normal;
	// 相机和天空盒子永远保持相对位置
	mat4 rot_view = mat4(mat3(view));
	vec4 gl_pos = projection * rot_view * vec4(pos, 1.0);

	// 天空盒的深度一直是最远的
	//xyww trick here that ensures the depth value of the rendered cube fragments always end up at 1.0, the maximum depth value
	GetClipAttribute().ndc_coord[vertex_idx] = { gl_pos.x, gl_pos.y, gl_pos.w, gl_pos.w };
}

bool Shader_Skybox::FragmentShader(float alpha, float beta, float gamma)
{
	vec3 direction = GET_BA_VALUE(vec3, GetAttribute().world_pos);
	vec3 color = Utils::CubemapSample(direction, Pipeline::GetBindVAO()->m_environment_map);
	//color = { 1.0f, 0.0f, 0.0f };
	frag_color = vec4(color, 1.0f);
	return true;
}