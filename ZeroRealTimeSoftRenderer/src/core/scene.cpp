#include "scene.h"
#include "shader/shader.h"
#include "pipeline.h"



void Scene_HelloTriangle::GenerateScene(std::shared_ptr<ColorBuffer> color_buffer, std::shared_ptr<ZBuffer> z_buffer)
{
	m_shader = std::make_shared<Shader_HelloTriangle>();
	m_shader->m_color_framebuffer = color_buffer;
	m_shader->m_z_framebuffer = z_buffer;
	vec4 vertexes[3] = { {0.0, 0.5, 0.0, 1.0}, {-0.5, -0.5, 0.0, 1.0}, {0.5, -0.5, 0, 1.0} };
	m_shader->m_attribute.vertexes[0] = vertexes[0];
	m_shader->m_attribute.vertexes[1] = vertexes[1];
	m_shader->m_attribute.vertexes[2] = vertexes[2];

	Color colors[3] = {{1.0f, 0.0f, 0.0f, 1.0}, 
						  {0.0f, 1.0f, 0.0f, 1.0},
						  {0.0f, 0.0f, 1.0f, 1.0}};
	m_shader->m_attribute.colors[0] = colors[0];
	m_shader->m_attribute.colors[1] = colors[1];
	m_shader->m_attribute.colors[2] = colors[2];
}

void Scene_HelloTriangle::Render(float delta_time)
{
	m_shader->VertexShader(0, 0);
	m_shader->VertexShader(0, 1);
	m_shader->VertexShader(0, 2);
	auto* test = m_shader->m_attribute.screen_coord;
	vec4 pts[3];
	for (int i = 0; i < 3; ++i)
	{
		pts[i] = { test[i].x, test[i].y, 0.0, 1.0 };
	}
	Pipeline::DrawTriangle(m_shader, m_shader->m_color_framebuffer, pts);
}



void Scene_Model::GenerateScene(std::shared_ptr<ColorBuffer> color_buffer, std::shared_ptr<ZBuffer> m_z_buffer)
{
	const vec3 eye(0, 1, 5);
	const vec3 up(0, 1, 0);
	const vec3 target(0, 1, 0);
	m_main_camera = std::make_shared<Camera>(CameraType::CT_PERSPECT, eye, target, up);
}

void Scene_Model::Render(float delta_time)
{

}
