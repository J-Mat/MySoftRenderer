#pragma once 
#include "shader/shader.h"
#include "mesh.h"
#include "math_ext.h"

using namespace Math;

using VAO = Mesh;

#define GET_BA_VALUE(type, values)  Pipeline::GetBarycentricValue<type>(values, alpha, beta, gamma)

enum ClipPlane
{
	CP_Left, 
	CP_Right,
	CP_Top,
	CP_Down,
	CP_Front, 
	CP_Back
};

class Pipeline
{
public: 
	static void BindColorBuffer(std::shared_ptr<ColorBuffer> buffer) { s_color_buffer = buffer; }
	static void BindZBuffer(std::shared_ptr<ZBuffer> buffer) { s_zbuffer = buffer; }
	static void BindShader(std::shared_ptr<IShader> shader) { s_shader = shader; }
	static void BindVAO(std::shared_ptr<VAO> vao) { s_vao = vao; }
	static std::shared_ptr<VAO> GetBindVAO() { return s_vao; }
	static std::shared_ptr<IShader> GetBindShader() { return  s_shader; }
	
	// 齐次空间裁剪
	// https://fabiensanglard.net/polygon_codec/clippingdocument/Clipping.pdf
	static bool IsInsidePlane(ClipPlane plane, vec4 ndc_vertex);
	static int  ClipThePlane(ClipPlane plane);
	static void HomoClipping();

	static void InitShaderAttribute(int face_idx);
	static void CommitAttribute(int start_idx);
	static void RunVertexStage();
	static bool VisibleClip();
	static ivec2 GetSreenCoord(vec4 ndc_coord);
	static vec3 GetBarycentric(const vec2& A, const vec2& B, const vec2& C, const vec2& P);
	static void GetBoundingBox(ivec2& min_box, ivec2& max_box);
	static void RunFragmentStage(); // 画三角形

	template <typename T> static T GetBarycentricValue(const T* values, float alpha, float beta, float gamma)
	{
		return values[0] * alpha + values[1] * beta + values[2] * gamma;
	}
private:
	static std::shared_ptr<ColorBuffer> s_color_buffer;
	static std::shared_ptr<ZBuffer> s_zbuffer;
	static std::shared_ptr<IShader> s_shader;
	static std::shared_ptr<VAO> s_vao;
};

