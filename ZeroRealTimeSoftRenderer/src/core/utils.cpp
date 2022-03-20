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
	float ma = 0;
	float sc = 0, tc = 0;
	vec3 abs_dir = { fabs(direction.x), fabs(direction.y), fabs(direction.z) };

	if (abs_dir.x > abs_dir.y && abs_dir.x > abs_dir.z)	
	{
		ma = abs_dir.x;
		if (direction.x < .0f)				
		{
			face_index = CubeFace::CF_Right;
			sc = +direction.z;
			tc = +direction.y;
		}
		else									
		{
			face_index = CubeFace::CF_Left;
			sc = -direction.z;
			tc = +direction.y;
		}
	}
	else if (abs_dir.y > abs_dir.x && abs_dir.y > abs_dir.z)	
	{
		ma = abs_dir.y;
		if (direction.y > .0f)					
		{
			face_index = CubeFace::CF_Top;
			sc = +direction.x;
			tc = -direction.z;
		}
		else									
		{
			face_index = CubeFace::CF_Down;
			sc = +direction.x;
			tc = +direction.z;
		}
	}
	else		
	{
		ma = abs_dir.z;
		if (direction.z > .0f)				
		{
			face_index = CubeFace::CF_Back;
			sc = +direction.x;
			tc = +direction.y;
		}
		else							
		{
			face_index = CubeFace::CF_Front;
			sc = -direction.x;
			tc = +direction.y;
		}
	}

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
