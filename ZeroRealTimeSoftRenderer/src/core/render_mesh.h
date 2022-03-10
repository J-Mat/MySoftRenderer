#pragma once 
#include "core.h"
#include "shader/shader.h"


class RenderMesh
{
public:
	RenderMesh(std::shared_ptr<Mesh> mesh, std::shared_ptr<IShader> shader);
	void Draw();
private:
	std::shared_ptr<Mesh> m_mesh;
	std::shared_ptr<IShader> m_shader;
};