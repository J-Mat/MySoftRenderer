#pragma once

#include "math_ext.h"
#include "tgaimage.h"
#include <vector>
#include <string>
using namespace Math;

typedef struct Cubemap
{
	static std::vector<char*>& GetFaceName()
	{
		static std::vector<char*> faces_name ={ "px", "nx", "py", "ny", "pz", "nz" };
		return faces_name;
	}
	TGAImage* faces[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
};

namespace Utils
{
	vec3 ConvertPixel2Color(TGAColor color);
	TGAColor ConvertColor2Pixel(vec3 color);
	vec3 TextureSample(vec2 texcorrd, TGAImage* image);
	void CalculateCubeMapUV(vec3 direction, vec2& texcoord, int& face_index);
	vec3 CubemapSample(vec3 direction, Cubemap* cubemap);
	void LoadTextureFromFile(std::string& file_name, TGAImage* image);
	void LoadCubmapFromFiles(std::vector<std::string>& paths, Cubemap* cubemap);
	vec3 Fresenlschlick(float h_dot_v, const vec3& f0);
	vec3 Fresenlschlick_Roughness(float h_dot_v, vec3& f0, float roughness);
	float DistributeGGX(vec3 n, vec3 h, float roughness);
	float GeometrySchlickGGX(float n_dot_v, float k);
	float GeometrySmith(vec3 n, vec3 v, vec3 l, float k);
}