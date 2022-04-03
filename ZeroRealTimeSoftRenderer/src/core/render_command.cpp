#include "render_command.h"
#include "pipeline.h"

RenderCommand::RenderCommand(std::shared_ptr<Mesh> mesh, std::shared_ptr<IShader> shader):
	m_mesh(mesh),
	m_shader(shader)
{
	
}

void RenderCommand::Commit()
{
	Pipeline::BindShader(m_shader);
	Pipeline::BindVAO(m_mesh);
	for (int face_idx = 0; face_idx < m_mesh->GetFaceSize(); ++face_idx)
	{	
		Pipeline::InitShaderAttribute(face_idx);
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
}


