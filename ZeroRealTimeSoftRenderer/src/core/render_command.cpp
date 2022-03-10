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
	Pipeline::BindVAO(m_mesh);
	for (int face_idx = 0; face_idx < m_mesh->GetFaceSize(); ++face_idx)
	{
		Pipeline::InitShaderAttribute(face_idx);
		Pipeline::RunVertexStage();
		Pipeline::NDC2ScreenCoord();
		Pipeline::RunFragmentStage();
	}
}


