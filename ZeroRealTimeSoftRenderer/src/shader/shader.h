#pragma once
#include "mesh.h"
#include "framebuffer.h"
using namespace Math;

struct light
{
	vec3 pos;
	vec3 intensity;
};

typedef struct cubemap
{
	TGAImage* m_faces[6];
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
};

struct Attribute
{
	vec3 pos[3];
	vec4 ndc_coord[3]; // NDC
	ivec2 screen_coord[3];
	Color colors[3];
	vec3 normals[3];
};

class IShader
{
public:
	Uniform m_uniform;
	Attribute m_attribute;
	Color frag_color;
	virtual void VertexShader(int nvertex) {}
	virtual void NDC2ScreenCoord();
	virtual bool FragmentShader(float alpha, float beta, float gamma) { return true; }
	
	template <typename T>
	static T GetBarycentricValue(const T* values, float alpha, float beta, float gamma)
	{
		return values[0] * alpha + values[1] * beta + values[2] * gamma;
	}
};

class Shader_HelloTriangle : public IShader
{
public:
	virtual void VertexShader(int vetex_idx);
	virtual bool FragmentShader(float alpha, float beta, float gamma);
};

class Shader_Model : public IShader
{
public:
	virtual void VertexShader(int vetex_idx);
	virtual bool FragmentShader(float alpha, float beta, float gamma);
};



