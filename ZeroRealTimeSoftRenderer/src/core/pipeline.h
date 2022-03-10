#pragma once 
#include "shader/shader.h"
#include "mesh.h"
#include "math_ext.h"

using namespace Math;

using VAO = Mesh;

class Pipeline
{
public: 
	static void BindColorBuffer(std::shared_ptr<ColorBuffer> buffer) { s_color_buffer = buffer; }
	static void BindZBuffer(std::shared_ptr<ZBuffer> buffer) { s_zbuffer = buffer; }
	static void BindShader(std::shared_ptr<IShader> shader) { s_shader = shader; }
	static void BindVAO(std::shared_ptr<VAO> vao) { s_vao = vao; }

	static void InitShaderAttribute(int face_idx);
	static void RunVertexStage();
	static void NDC2ScreenCoord();
	static ivec2 GetSreenCoord(vec4 ndc_coord);
	static vec3 GetBarycentric(const vec2& A, const vec2& B, const vec2& C, const vec2& P);
	static void GetBoundingBox(ivec2& min_box, ivec2& max_box);
	static void RunFragmentStage(); // »­Èý½ÇÐÎ
private:
	static std::shared_ptr<ColorBuffer> s_color_buffer;
	static std::shared_ptr<ZBuffer> s_zbuffer;
	static std::shared_ptr<IShader> s_shader;
	static std::shared_ptr<VAO> s_vao;
};

