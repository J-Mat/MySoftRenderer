#include "tgaimage.h"
#include "core.h"
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

class IShader
{
public:
	Uniform m_uniform;
	std::shared_ptr<Model> m_attach_model;
	virtual void vertex_shader(int nfaces, int nvertex) {}
	virtual vec3 fragment_shader(float alpha, float beta, float gamma) { return vec3(1, 1, 1); }
};


