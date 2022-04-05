#include "utils.h"
#include "shader/shader.h"

vec3 Utils::ConvertPixel2Color(TGAColor color)
{
	vec3 res;
	for (int i = 0; i < 3; i++)
	{
		res[2 - i] = (float)color[i] / 255.f;
	}
	return res;
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

vec3 Utils::CubemapSample(vec3 dirction, Cubemap* cubemap)
{	
	vec2 texcoord;
	int face_index = -1;
	CalculateCubeMapUV(dirction, texcoord, face_index);
	assert(face_index != -1);
	return TextureSample(texcoord, cubemap->faces[face_index]);
}
