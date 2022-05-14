#pragma once 
#include "core.h"
#include "shader/shader.h"


class RenderCommand
{
public:
	RenderCommand(std::shared_ptr<Mesh> mesh, std::shared_ptr<IShader> shader);
	std::shared_ptr<IShader> GetAttachShader() { return m_shader; }
	void Commit();
private:
	std::shared_ptr<Mesh> m_mesh;
	std::shared_ptr<IShader> m_shader;
};