#pragma once 
#include "core.h"
#include "shader/shader.h"
#include "mesh.h"

using namespace Math;

using VAO = Mesh;

class Pipeline
{
public: 
	static void BindColorBuffer(std::shared_ptr<ColorBuffer> buffer) { s_color_buffer = buffer; }
	static void BindZBuffer(std::shared_ptr<ZBuffer> buffer) { s_zbuffer = buffer; }
	static void BindShader(std::shared_ptr<IShader> shader) { s_shader = shader; }

	static ivec2 GetSreenCoord(vec4 ndc_coord);
	static vec3 GetBarycentric(const vec2& A, const vec2& B, const vec2& C, const vec2& P);
	static void GetBoundingBox(ivec2& min_box, ivec2& max_box, vec4* pts);
	static void DrawTriangle(vec4* pts);
	static void DrawTriangles(VAO* vao);
	
private:
	static std::shared_ptr<ColorBuffer> s_color_buffer;
	static std::shared_ptr<ZBuffer> s_zbuffer;
	static std::shared_ptr<IShader> s_shader;
};

