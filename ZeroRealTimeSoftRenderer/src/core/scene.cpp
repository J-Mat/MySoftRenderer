#include "scene.h"
#include "shader/shader.h"
#include "pipeline.h"
#include "debug.h"



void Scene_HelloTriangle::GenerateScene(std::shared_ptr<ColorBuffer> color_buffer, std::shared_ptr<ZBuffer> z_buffer)
{
	m_shader = std::make_shared<Shader_HelloTriangle>();
	m_shader->ResetAttribute();
	Pipeline::BindShader(m_shader);
	Pipeline::BindColorBuffer(color_buffer);
	Pipeline::BindZBuffer(z_buffer);
	vec4 ndc_coord[3] = { {0.0, 0.5, 0.0, 1.0}, {-0.5, -0.5, 0.0, 1.0}, {2.0, -0.5, 0, 1.0} };
	m_shader->GetClipAttribute().ndc_coord[0] = ndc_coord[0];
	m_shader->GetClipAttribute().ndc_coord[1] = ndc_coord[1];
	m_shader->GetClipAttribute().ndc_coord[2] = ndc_coord[2];

	Color colors[3] = {{1.0f, 0.0f, 0.0f, 1.0}, 
						  {0.0f, 1.0f, 0.0f, 1.0},
						  {0.0f, 0.0f, 1.0f, 1.0}};
	m_shader->GetClipAttribute().colors[0] = colors[0];
	m_shader->GetClipAttribute().colors[1] = colors[1];
	m_shader->GetClipAttribute().colors[2] = colors[2];
}

void Scene_HelloTriangle::Render(float delta_time)
{
	m_shader->ResetAttribute();
	static vec4 ndc_coord[3] = { {0.0, 0.5, 0.0, 1.0}, {-0.5, -0.5, 0.0, 1.0}, {2, -0.5, 0, 1.0} };
	m_shader->GetClipAttribute().ndc_coord[0] = ndc_coord[0];
	m_shader->GetClipAttribute().ndc_coord[1] = ndc_coord[1];
	m_shader->GetClipAttribute().ndc_coord[2] = ndc_coord[2];

	static Color colors[3] = {{1.0f, 0.0f, 0.0f, 1.0}, 
						  {0.0f, 1.0f, 0.0f, 1.0},
						  {0.0f, 0.0f, 1.0f, 1.0}};
	m_shader->GetClipAttribute().colors[0] = colors[0];
	m_shader->GetClipAttribute().colors[1] = colors[1];
	m_shader->GetClipAttribute().colors[2] = colors[2];

	Pipeline::RunVertexStage();
	Pipeline::GetBindShader()->vertex_num = 3;
	//test
	Pipeline::HomoClipping();
	int vertex_num = Pipeline::GetBindShader()->vertex_num;
	for (int i = 0; i < vertex_num - 2; ++i)
	{
		Pipeline::CommitAttribute(0, i + 1, i + 2);
		Pipeline::RunFragmentStage();
	}
}



void Scene_Model::GenerateScene(std::shared_ptr<ColorBuffer> color_buffer, std::shared_ptr<ZBuffer> z_buffer)
{
	Pipeline::BindColorBuffer(color_buffer);
	Pipeline::BindZBuffer(z_buffer);
	// 相机
	const vec3 eye(0, 0, -3);
	const vec3 target(0, 0, 0);
	CameraSettings settings;
	m_main_camera = std::make_shared<Camera>(settings);
	m_main_camera->Init(eye, target);
	m_main_camera->BindInput();

	std::vector<char*> mesh_names =
	{
		"../res/helmet/helmet.obj",
	};
	
	for (char* name : mesh_names)
	{
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>(name);
		std::shared_ptr<IShader> shader = std::make_shared<Shader_BaseLight>();
		std::shared_ptr<RenderCommand> command = std::make_shared<RenderCommand>(mesh, shader);
		m_render_commands.push_back(command);
	}	
}

void Scene_Model::Render(float delta_time)
{
	m_main_camera->UpdateCamera(delta_time);
	for (auto command : m_render_commands)
	{
		auto shader = command->GetAttachShader();
		shader->GetUniform().model_mat = mat4(1.0f);
		shader->GetUniform().view_mat = m_main_camera->GetViewMat();
		shader->GetUniform().project_mat = m_main_camera->GetProjectMat();
		shader->GetUniform().view_pos = m_main_camera->GetViewPos();
		command->Commit();
	}
}

void Scene_Skybox::GenerateScene(std::shared_ptr<ColorBuffer> color_buffer, std::shared_ptr<ZBuffer> z_buffer)
{
	Pipeline::BindColorBuffer(color_buffer);
	Pipeline::BindZBuffer(z_buffer);
	// 相机
	const vec3 eye(0, 0, 4);
	const vec3 target(0, 0, 1);
	CameraSettings settings;
	m_main_camera = std::make_shared<Camera>(settings);
	m_main_camera->Init(eye, target);
	m_main_camera->BindInput();


	std::vector<char*> mesh_names =
	{
		"../res/gun/Cerberus.obj",
		"../res/skybox4/box.obj",
	};

	std::shared_ptr<Mesh> meshes[2];
	std::shared_ptr<IShader> shaders[2];
	

	meshes[0] = std::make_shared<Mesh>(mesh_names[0]);
	shaders[0] = std::make_shared<Shader_PBR>();
	
	meshes[1] = std::make_shared<Mesh>(mesh_names[1], true);
	shaders[1] = std::make_shared<Shader_Skybox>();
	
	for (int i = 0; i < 2; ++i)
	{
		std::shared_ptr<RenderCommand> command = std::make_shared<RenderCommand>(meshes[i], shaders[i]);
		m_render_commands.push_back(command);
	}	

}

void Scene_Skybox::Render(float delta_time)
{
	m_main_camera->UpdateCamera(delta_time);
	for (auto command : m_render_commands)
	{
		auto shader = command->GetAttachShader();
		shader->GetUniform().model_mat = mat4(1.0f);
		shader->GetUniform().view_mat = m_main_camera->GetViewMat();
		shader->GetUniform().project_mat = m_main_camera->GetProjectMat();
		shader->GetUniform().view_pos = m_main_camera->GetViewPos();
		command->Commit();
	}
}
