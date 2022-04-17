#include "utils.h"
#include "math_ext.h"
#include "debug.h"

vec3 Utils::ConvertPixel2Color(TGAColor color)
{
	vec3 res;
	for (int i = 0; i < 3; i++)
	{
		res[2 - i] = (float)color[i] / 255.f;
	}
	return res;
}

TGAColor Utils::ConvertColor2Pixel(vec3 color)
{
	unsigned char  r = Math::min(color.x * 255.0f, 255.0f);
	unsigned char  g = Math::min(color.y * 255.0f, 255.0f);
	unsigned char  b = Math::min(color.z * 255.0f, 255.0f);
	return { r, g, b };
}

vec3 Utils::TextureSample(vec2 texcorrd, TGAImage* image)
{
	texcorrd.x = fmod(texcorrd.x, 1.0f);
	texcorrd.y = fmod(texcorrd.y, 1.0f);
	
	ivec2 texture_pixel = { texcorrd.x * image->get_width(), texcorrd.y * image->get_height() };
	TGAColor color = image->get(texture_pixel.x, texture_pixel.y);	
	return ConvertPixel2Color(color);
}

void Utils::CalculateCubeMapUV(vec3 direction, vec2& texcoord, int& face_index)
{
	float ma = 0, sc = 0, tc = 0;
	float abs_x = fabs(direction[0]), abs_y = fabs(direction[1]), abs_z = fabs(direction[2]);

	if (abs_x > abs_y && abs_x > abs_z)			/* major axis -> x */
	{
		ma = abs_x;
		if (direction.x > 0)					/* positive x */
		{
			face_index = 0;
			sc = +direction.z;
			tc = +direction.y;
		}
		else									/* negative x */
		{
			face_index = 1;
			sc = -direction.z;
			tc = +direction.y;
		}
	}
	else if (abs_y > abs_z)						/* major axis -> y */
	{
		ma = abs_y;
		if (direction.y > 0)					/* positive y */
		{
			face_index = 2;
			sc = +direction. x;
			tc = +direction.z;
		}
		else									/* negative y */
		{
			face_index = 3;
			sc = +direction.x;
			tc = -direction.z;
		}
	}
	else										/* major axis -> z */
	{
		ma = abs_z;
		if (direction.z > 0)					/* positive z */
		{
			face_index = 4;
			sc = -direction.x;
			tc = +direction.y;
		}
		else									/* negative z */
		{
			face_index = 5;
			sc = +direction.x;
			tc = +direction.y;
		}
	}

	//uv[0] = (sc / ma + 1.0f) / 2.0f;
	//uv[1] = (tc / ma + 1.0f) / 2.0f;


	texcoord = {
		Math::Remap<float>(sc / ma, -1.0f, 1.0f, 0.0f, 1.0), 
		Math::Remap<float>(tc / ma, -1.0f, 1.0f, 0.0f, 1.0) 
	};
}

glm::vec3 Utils::CubemapSample(vec3 direction, Cubemap* cubemap)
{	
	vec2 texcoord;
	int face_index = -1;
	CalculateCubeMapUV(direction, texcoord, face_index);
	assert(face_index != -1);
	return TextureSample(texcoord, cubemap->faces[face_index]);
}

void Utils::LoadTextureFromFile(std::string& file_name, TGAImage* image)
{
	if (!image->read_tga_file(file_name.c_str()))
	{
		DEBUG_INFO("xxoo----------------");
	}
	image->flip_vertically();
}

void Utils::LoadCubmapFromFiles(std::vector<std::string>& paths, Cubemap* cubemap)
{
	for (int i = 0; i < 6; ++i)
	{
		cubemap->faces[i] = new TGAImage();
		LoadTextureFromFile(paths[i], cubemap->faces[i]);
	}
}

vec3 Utils::Fresenlschlick(float h_dot_v, const vec3& f0)
{
	return f0 + (vec3(1.0f) - f0) * pow(1.0f - h_dot_v, 5.0f);
}

vec3 Utils::Fresenlschlick_Roughness(float h_dot_v, vec3& f0, float roughness)
{
	float r1 = 1.0f - roughness < f0[0];
	if (r1 < f0[0])
	{
		r1 = f0[0];
	}
	return f0 + (vec3(r1, r1, r1) - f0) * pow(1 - h_dot_v, 5.0f);
}

float Utils::DistributeGGX(vec3 n, vec3 h, float roughness)
{
	float a2 = roughness * roughness;
	float n_dot_h = Math::max(dot(n, h), 0.0f);

	float nom = a2;
	float denom = n_dot_h * n_dot_h * (a2 - 1.0f) + 1.0f;
	denom = Math::pi<float>() * denom * denom;

	return nom / denom;
}


float Utils::GeometrySchlickGGX(float n_dot_v, float k)
{
	float nom = n_dot_v;
	float denom = n_dot_v * (1.0f - k) + k;
	return nom / denom;
}

float Utils::GeometrySmith(vec3 n, vec3 v, vec3 l, float k)
{
	float n_dot_v = Math::max(dot(n, v), 0.0f);
	float n_dot_l = Math::max(dot(n, l), 0.0f);
	float ggx1 = GeometrySchlickGGX(n_dot_v, k);
	float ggx2 = GeometrySchlickGGX(n_dot_l, k);

	return ggx1 * ggx2;
}
