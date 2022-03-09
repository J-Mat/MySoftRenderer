#pragma once

#include "core.h"
#include "camera.h"
#include "mesh.h"
#include "shader/shader.h"
using namespace  Math;


class Scene
{
public:
	Scene() = default;
	~Scene() = default;
	
	virtual void GenerateScene(
		std::shared_ptr<ColorBuffer> color_buffer,
		std::shared_ptr<ZBuffer> m_z_buffer) = 0;
	virtual void Render(float delta_time) = 0;

protected:
	std::shared_ptr<Camera>  m_main_camera;
	std::vector<std::shared_ptr<Mesh>> m_meshes;
};

class Scene_HelloTriangle : public Scene 
{
public:
	virtual void GenerateScene(
		std::shared_ptr<ColorBuffer> color_buffer,
		std::shared_ptr<ZBuffer> m_z_buffer);
	virtual void Render(float delta_time);
private:
	std::shared_ptr<Shader_HelloTriangle> m_shader;
};

class Scene_Model : public Scene
{
	virtual void GenerateScene(
		std::shared_ptr<ColorBuffer> color_buffer,
		std::shared_ptr<ZBuffer> m_z_buffer);
	virtual void Render(float delta_time);
private:
	std::shared_ptr<Shader_HelloTriangle> m_shader;
};