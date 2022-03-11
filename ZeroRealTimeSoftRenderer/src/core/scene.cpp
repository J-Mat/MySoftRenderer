#include "scene.h"
#include "shader/shader.h"
#include "pipeline.h"



void Scene_HelloTriangle::GenerateScene(std::shared_ptr<ColorBuffer> color_buffer, std::shared_ptr<ZBuffer> z_buffer)
{
	m_shader = std::make_shared<Shader_HelloTriangle>();
	Pipeline::BindShader(m_shader);
	Pipeline::BindColorBuffer(color_buffer);
	Pipeline::BindZBuffer(z_buffer);
	vec4 ndc_coord[3] = { {0.0, 0.5, 0.0, 1.0}, {-0.5, -0.5, 0.0, 1.0}, {0.5, -0.5, 0, 1.0} };
	m_shader->m_attribute.ndc_coord[0] = ndc_coord[0];
	m_shader->m_attribute.ndc_coord[1] = ndc_coord[1];
	m_shader->m_attribute.ndc_coord[2] = ndc_coord[2];

	Color colors[3] = {{1.0f, 0.0f, 0.0f, 1.0}, 
						  {0.0f, 1.0f, 0.0f, 1.0},
						  {0.0f, 0.0f, 1.0f, 1.0}};
	m_shader->m_attribute.colors[0] = colors[0];
	m_shader->m_attribute.colors[1] = colors[1];
	m_shader->m_attribute.colors[2] = colors[2];
}

void Scene_HelloTriangle::Render(float delta_time)
{
	m_shader->VertexShader(0);
	m_shader->VertexShader(1);
	m_shader->VertexShader(2);
	Pipeline::NDC2ScreenCoord();
	Pipeline::RunFragmentStage();
}



void Scene_Model::GenerateScene(std::shared_ptr<ColorBuffer> color_buffer, std::shared_ptr<ZBuffer> z_buffer)
{
	Pipeline::BindColorBuffer(color_buffer);
	Pipeline::BindZBuffer(z_buffer);
	// 相机
	const vec3 eye(0, 0, 5);
	const vec3 target(0, 0, 0);
	CameraSettings settings;
	m_main_camera = std::make_shared<Camera>(settings);
	m_main_camera->Init(eye, target);

	/*
	// 模型
	std::vector<char*> mesh_names =
	{
		"../res/gun/Cerberus.obj"
	};
	
	for (char* name : mesh_names)
	{
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(name);
		std::shared_ptr<IShader> shader = std::make_shared<Shader_Model>();
		std::shared_ptr<RenderCommand> command = std::make_shared<RenderCommand>(mesh, shader);
		m_render_commands.push_back(command);
	}	
	*/
	std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>("xxoo---");
	std::shared_ptr<IShader> shader = std::make_shared<Shader_Model>();
	std::shared_ptr<RenderCommand> command = std::make_shared<RenderCommand>(mesh, shader);
	m_render_commands.push_back(command);
}

void Scene_Model::Render(float delta_time)
{
	m_main_camera->UpdateCamera(delta_time);
	for (auto command : m_render_commands)
	{
		auto shader = command->GetAttachShader();
		shader->m_uniform.model_mat = mat4(1.0f);
		shader->m_uniform.view_mat = m_main_camera->GetViewMat();
		shader->m_uniform.perspect_mat = m_main_camera->GetProjectViewMat();
		command->Commit();
	}
}
