#include "shader.h"
#include "pipeline.h"
#include "debug.h"
#include "utils.h"

void Shader_HelloTriangle::VertexShader(int vertex_idx)
{
}

bool Shader_HelloTriangle::FragmentShader(float alpha, float beta, float gamma)
{
	frag_color = GET_BA_VALUE(Color, GetAttribute().colors);
	return true;
}