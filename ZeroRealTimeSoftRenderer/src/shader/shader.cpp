#include "shader.h"
#include "pipeline.h"

#define GET_BA_VALUE(type, values)  GetBarycentricValue<type>(values, alpha, beta, gamma)

void IShader::NDC2ScreenCoord()
{
	for (int i = 0; i < 3; ++i)
	{
		m_attribute.screen_coord[i] = Pipeline::GetSreenCoord(m_attribute.ndc_coord[i]);
	}
}

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
	m_attribute.ndc_coord[vertex_idx] = m_uniform.perspect_mat * m_uniform.view_mat * vec4(m_attribute.pos[vertex_idx], 1.0);
}

bool Shader_Model::FragmentShader(float alpha, float beta, float gamma)
{
	return true;
}

