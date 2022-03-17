#include "utils.h"

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

}

vec3 Utils::CubemapSample(vec3 dirction, Cubemap* cubemap)
{	
	vec2 texcoord;
	//int face_index 
	return vec3();
}
