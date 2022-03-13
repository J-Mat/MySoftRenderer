#pragma once
#include "mesh.h"
#include "framebuffer.h"
using namespace Math;

//  LearnOpenGL 6.multiple_lights
struct DirLight
{
	// vec3 position; // no longer necessary when using directional lights.
	vec3 direction = { -0.2f, -1.0f, -0.3f };
	vec3 ambient = { 0.4f, 0.4f, 0.4f };
	vec3 diffuse = { 0.3f, 0.3f, 0.3f };
	vec3 specular = { 1.0f, 1.0f, 1.0f } ;
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
	vec3 view_pos;
};

struct Attribute
{
	vec3 pos[3];
	vec4 ndc_coord[3]; // NDC
	vec3 world_pos[3];
	vec2 screen_coord[3];
	vec2 texcoord[3];
	Color colors[3];
	vec3 normals[3];
};


class IShader
{
public:
	int m_cur_face_idx;
	Uniform m_uniform;
	Attribute m_attribute;
	Color frag_color;
	virtual void VertexShader(int nvertex) {}
	virtual bool FragmentShader(float alpha, float beta, float gamma) { return true; }	
};

class Shader_HelloTriangle : public IShader
{
public:
	virtual void VertexShader(int vetex_idx);
	virtual bool FragmentShader(float alpha, float beta, float gamma);
};

class Shader_BaseLight : public IShader
{
public:
	DirLight m_dir_light;
	
public:
	virtual void VertexShader(int vetex_idx);
	virtual bool FragmentShader(float alpha, float beta, float gamma);
};



