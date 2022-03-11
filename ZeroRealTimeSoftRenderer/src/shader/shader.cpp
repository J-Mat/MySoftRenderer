#include "shader.h"
#include "pipeline.h"
#include "debug.h"

#define GET_BA_VALUE(type, values)  GetBarycentricValue<type>(values, alpha, beta, gamma)


void Shader_HelloTriangle::VertexShader(int vertex_idx)
{
}

bool Shader_HelloTriangle::FragmentShader(float alpha, float beta, float gamma)
{
	frag_color = GET_BA_VALUE(Color, m_attribute.colors);
	return true;
}

void Shader_Model::VertexShader(int vertex_idx)
{	
	// MVP
	vec4 view_pos = m_uniform.view_mat * vec4(m_attribute.pos[vertex_idx], 1.0);
	DEBUG_INFO("---------------------------------------%d\n", vertex_idx);
	DEBUG_INFO("view_pos ");
	DEBUG_POS4(view_pos);
	vec4 project_pos = m_uniform.perspect_mat * view_pos;
	project_pos /= project_pos.w;
	DEBUG_INFO("proj_pos: ");
	DEBUG_POS4(project_pos);
	DEBUG_INFO("---------------------------------------\n");
	m_attribute.ndc_coord[vertex_idx] = m_uniform.perspect_mat * m_uniform.view_mat * vec4(m_attribute.pos[vertex_idx], 1.0);	
}

bool Shader_Model::FragmentShader(float alpha, float beta, float gamma)
{
	frag_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	return true;
}

