#pragma once
#include "model.h"
#include "framebuffer.h"
using namespace Math;

struct light
{
	vec3 pos;
	vec3 intensity;
};

typedef struct cubemap
{
	TGAImage* faces[6];
}cubemap_t;

typedef struct iblmap
{
	int mip_levels;
	cubemap_t* irradiance_map;
	cubemap_t* prefilter_maps[15];
	TGAImage* brdf_lut;
} iblmap_t;

struct Uniform
{
	mat4 model_mat;
	mat4 view_mat;
	mat4 perspect_mat;
	mat4 mvp_mat;
};

struct Attribute
{
	vec4 vertexes[3]; // NDC
	ivec2 screen_coord[3];
	Color colors[3];
	vec3 normals[3];
};

class IShader
{
public:
	static ivec2 GetSreenCoord(std::shared_ptr<FrameBuffer> buffer, vec4 ndc_coord);
	Uniform m_uniform;
	Attribute m_attribute;
	Color frag_color;
	std::shared_ptr<Model> m_attach_model;
	std::shared_ptr<ColorBuffer> m_color_framebuffer;
	std::shared_ptr<ZBuffer> m_z_framebuffer;
	virtual void vertex_shader(int nfaces, int nvertex) {}
	virtual bool fragment_shader(float alpha, float beta, float gamma) { return true; }
	
	template <typename T>
	static T GetBarycentricValue(const T* values, float alpha, float beta, float gamma)
	{
		return values[0] * alpha + values[1] * beta + values[2] * gamma;
	}
};

class Shader_HelloTriangle : public IShader
{
public:
	Shader_HelloTriangle() = default;
	virtual void vertex_shader(int face_idx, int vetex_idx);
	virtual bool fragment_shader(float alpha, float beta, float gamma);
};



