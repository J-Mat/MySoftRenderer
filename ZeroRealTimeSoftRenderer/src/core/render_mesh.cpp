#include "render_mesh.h"
#include "pipeline.h"

RenderMesh::RenderMesh(std::shared_ptr<Mesh> mesh, std::shared_ptr<IShader> shader):
	m_mesh(mesh),
	m_shader(shader)
{
	
}

void RenderMesh::Draw()
{
	Pipeline::BindShader(m_shader);
}


