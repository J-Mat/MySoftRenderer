#pragma once
#include "core.h"
using namespace Math;

typedef struct cubemap cubemap_t; // forward declaration

class Mesh {
private:
	std::vector<vec3> m_positons;
	std::vector<std::vector<int> > m_faces; // attention, this Vec3i means vertex/uv/normal
	std::vector<vec3> m_normals;
	std::vector<vec2> m_texcoords;


	void LoadCubemap(const char *filename);
	void CreateMap(const char *filename);
	void LoadTexture(std::string filename, const char *suffix, TGAImage &img);
	void LoadTexture(std::string filename, const char *suffix, TGAImage *img);
public:
	Mesh(const char *filename, int is_skybox = 0, int is_from_mmd = 0);
	~Mesh();
	
	//skybox
	cubemap_t *m_environment_map;
	int is_skybox;

	//map
	int is_from_mmd;
	TGAImage *m_diffuse_map;
	TGAImage *m_normal_map;
	TGAImage *m_specular_map;
	TGAImage *m_roughness_map;
	TGAImage *m_metalness_map;
	TGAImage *m_occlusion_map;
	TGAImage *m_emision_map;

	int GetVertexSize();
	int GetFaceSize();
	vec3 Normal(int iface, int nthvert);
	vec3 Normal(vec2 uv);
	vec3 Position(int i);
	vec3 Position(int iface, int nthvert);

	vec2 Texcoord(int iface, int nthvert);
	vec3 Diffuse(vec2 uv);
	float Roughness(vec2 uv);
	float Metalness(vec2 uv);
	vec3 Emission(vec2 uv);
	float Occlusion(vec2 uv);
	float Specular(vec2 uv);

	std::vector<int> Face(int idx);
};

