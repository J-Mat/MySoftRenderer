#pragma once
#include "mesh.h"
#include "framebuffer.h"
using namespace Math;

//  LearnOpenGL 6.multiple_lights
struct DirLight
{
	// vec3 position; // no longer necessary when using directional lights.
	vec3 direction = { -1.0f, -1.0f, -1.0f };
	vec3 ambient = { 0.4f, 0.4f, 0.4f };
	vec3 diffuse = { 0.03f, 0.03f, 0.03f };
	vec3 specular = { 1.0f, 1.0f, 1.0f } ;
	
	vec3 radiance = { 1.0f, 1.0f, 1.0f };
};

/*
enum CubeFace
{
	W_PLANE,
	X_RIGHT,
	X_LEFT,
	Y_TOP,
	Y_BOTTOM,
	Z_NEAR,
	Z_FAR
};
*/



typedef struct iblmap
{
	int mip_levels;
	Cubemap* irradiance_map;
	Cubemap* prefilter_maps[15];
	TGAImage* brdf_lut;
} iblmap_t;

struct Uniform
{
	mat4 model_mat;
	mat4 view_mat;
	mat4 project_mat;
	vec3 view_pos;
};

#define MAX_VERTEXES 10

struct Attribute
{
	vec3 pos[3];
	vec4 ndc_coord[3];
	vec3 world_pos[3];
	vec2 screen_coord[3];
	vec2 texcoord[3];
	Color colors[3];
	vec3 normals[3];
};


struct ClipAttribute
{
	vec3 pos[MAX_VERTEXES];
	vec4 ndc_coord[MAX_VERTEXES]; 
	vec3 world_pos[MAX_VERTEXES];
	vec2 screen_coord[MAX_VERTEXES];
	vec2 texcoord[MAX_VERTEXES];
	Color colors[MAX_VERTEXES];
	vec3 normals[MAX_VERTEXES];
};

#define TEXTURE(Type, texcoord)  Pipeline::GetBindVAO()->Type(texcoord)

class IShader
{
private:
	Attribute m_attribute;
	ClipAttribute m_clip_attribute[2];
	Uniform m_uniform;
public:
	int cur_attr_idx = 0;
	int m_cur_face_idx;
	int vertex_num = 3;
	Color frag_color;
	DirLight m_dir_light;
	std::shared_ptr<Cubemap> m_cubemap;
	virtual void VertexShader(int nvertex) {}
	virtual bool FragmentShader(float alpha, float beta, float gamma) { return true; }	

	virtual void ResetAttribute() { cur_attr_idx = 0; }
	virtual Attribute& GetAttribute() { return m_attribute; }
	virtual ClipAttribute& GetClipAttribute() { return m_clip_attribute[cur_attr_idx]; }
	virtual ClipAttribute& GetClipAttributeByIndex(int cur_idx) { return m_clip_attribute[cur_idx]; }
	virtual Uniform& GetUniform() { return m_uniform; }
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
	virtual void VertexShader(int vetex_idx);
	virtual bool FragmentShader(float alpha, float beta, float gamma);
};


// 由浅入深学习PBR的原理和实现，最强入门总结，没有之一  https://www.cnblogs.com/timlly/p/10631718.html?from=timeline&isappinstalled=0  
// 
class Shader_PBR : public IShader
{
public:
	virtual void VertexShader(int vetex_idx);
	virtual bool FragmentShader(float alpha, float beta, float gamma);
};

class Shader_Skybox : public IShader
{
public:
	virtual void VertexShader(int vetex_idx);
	virtual bool FragmentShader(float alpha, float beta, float gamma);
};

