#include "shader.h"

ivec2 IShader::GetSreenCoord(std::shared_ptr<FrameBuffer> buffer, vec4 ndc_coord)
{
	return { (ndc_coord.y / 2.0 + 0.5) * buffer->GetHeight(),
			 (ndc_coord.x / 2.0 + 0.5) * buffer->GetWidth()};
}

void Shader_HelloTriangle::vertex_shader(int face_idx, int vertex_idx)
{
	m_attribute.screen_coord[vertex_idx] = GetSreenCoord(m_color_framebuffer, m_attribute.vertexes[vertex_idx]);
}

bool Shader_HelloTriangle::fragment_shader(float alpha, float beta, float gamma)
{
	frag_color = GetBarycentricValue<Color>(m_attribute.colors, alpha, beta, gamma);
	return true;
}
