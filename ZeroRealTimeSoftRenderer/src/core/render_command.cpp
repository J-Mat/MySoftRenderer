#include "render_command.h"
#include "pipeline.h"
#include "debug.h"

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

		/*	其次空间裁剪，裁剪成多个三角形
				 *	  
				 *   *      
		-----------------------------------------------
		|screen  *	      *							  |
		|        *            *                       |
		|        *                *                   |
		|        *                   *                |
		|        *                      *             |
		|        *                         *          |
		|        *                            *       |
		------------------------------------------------       
		         *                                 *
				 *                                    * 
				 * *************************************  triangle
		*/


		Pipeline::HomoClipping();
		int vertex_num = Pipeline::GetBindShader()->vertex_num;
		for (int i = 0; i < vertex_num - 2; ++i)
		{
			// 三角形顶点 0， i+1, i+2,裁剪之后一定是多个三角形组合的凸多边形（扇形）
			Pipeline::CommitAttribute(0, i + 1, i + 2);
			Pipeline::RunFragmentStage();
		}
	}
}


