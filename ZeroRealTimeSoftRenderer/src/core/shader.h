#pragma once
#include "core.h"
using namespace glm;

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