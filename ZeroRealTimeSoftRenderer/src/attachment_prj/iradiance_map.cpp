#include <iostream>
#include "tgaimage.h"
#include <string>
#include "math_ext.h"
#include <math.h>
#include "mesh.h"
#include <thread>
using namespace std;
using namespace Math;

std::shared_ptr<Mesh> g_skybox_mesh;
const int g_image_size = 256;

enum Face
{
	F_PX,
	F_NX,
	F_PY,
	F_NY,
	F_PZ,
	F_NZ
};

vec3 GetCubeMapDir(Face face, int x, int y)
{
	float x_step = float(x) / g_image_size;
	float y_step = float(y) / g_image_size;
	
	float u, v, t;
	switch (face)
	{
	case F_PX:   //positive x (right face)
		u = 0.5f;
		v = -0.5f + y_step;
		t = -0.5f + x_step;
		break;
	case F_NX:   //negative x (left face)		
		u = -0.5f;
		v = -0.5f + y_step;
		t = 0.5f - x_step;
		break;
	case F_PY:   //positive y (top face)
		u = -0.5f + x_step;
		v = 0.5f;
		t = -0.5f + y_step;
		break;
	case F_NY:   //negative y (bottom face)
		u = -0.5f + x_step;
		v = -0.5f;
		t = 0.5f - y_step;
		break;
	case F_PZ:   //positive z (back face)
		u = 0.5f - x_step;
		v = -0.5f + y_step;
		t = 0.5f;
		break;
	case F_NZ:   //negative z (front face)
		u = -0.5f + x_step;
		v = -0.5f + y_step;
		t = -0.5f;
		break;
	default:
		break;
	}
	vec3 dir = { u, v, t };
	return normalize(dir);
}
void GenerateIrradianceMap(TGAImage& image, Face face)
{
	for (int i = 0; i < g_image_size; ++i)
	{
		for (int j = 0; j < g_image_size; ++j)
		{
			vec3 dir = GetCubeMapDir(face, i, j);
			// ���������ֱ��dir���������γ�һ����ά�ռ�
			vec3 up = Math::abs(dir.y) < 0.999f ? vec3(0.0f, 1.0f, 0.0f) : vec3(0.0f, 0.0f, 1.0f);
			vec3 right = normalize(cross(dir, up));
			up = normalize(cross(right, dir));
			vec3 irradiance = vec3(0, 0, 0);
			float delta = 0.025f;
			int cnt = 0;
			for (float phi = 0.0f; phi < TWO_PI; phi += delta)
			{
				for (float theta = 0.0f; theta < HALF_PI; theta += delta)
				{
					vec3 sample_dir = { sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta) };
					sample_dir = sample_dir.x * right + sample_dir.y * up + sample_dir.z * dir; //ת����Ӧ�Ŀռ�
					normalize(sample_dir);
					vec3 color = Utils::CubemapSample(sample_dir, g_skybox_mesh->m_environment_map);
					irradiance += color * sin(theta) * cos(theta);
					++cnt;
				}
			}

			irradiance = PI * irradiance / float(cnt);
			TGAColor pixel = Utils::ConvertColor2Pixel(irradiance);
			image.set(i, j, pixel);
		}
	}
	cout << face << endl;
}

// ������������Ŀ��������һ���֣�Ϊ�˴ﵽ���õ���ȾЧ������IBL
// ��ǿ��֮ǰ��Ҫ���˽����ؿ������ԭ��


// radiance irradiance BRDF�������� games101 P14 P15 ���ķǳ��ã��࿴����������
// Cook-Torrance BRDF
// https://zhuanlan.zhihu.com/p/152226698
// 
// 
// ����ͼ��ѧ��һ���ɣ�������������ǶԵģ��������ǶԵ�
// // �����õ���һ������Ҫ�������ƹ�ʽ Spilt Sum Aprroximation��https://zhuanlan.zhihu.com/p/429965810��
// �ο���
// games202 P5
// 
// IBLԭ�� ��ʽ�Ƶ� 
// �����������ȫ�ֹ��յ��ܽ�
// https://zhuanlan.zhihu.com/p/261005894
// https://zhuanlan.zhihu.com/p/66518450
int main()
{
	g_skybox_mesh = std::make_shared<Mesh>("../res/skybox4/box.obj", true);

	const char* faces[6] = { "px", "nx", "py", "ny", "pz", "nz" };
	char paths[6][256];
	std::thread threads[6];
	TGAImage images[6];
	for (int face_id = 0; face_id < 6; ++face_id)
	{
		sprintf(paths[face_id], "%s/i_%s.tga", "../res/temp", faces[face_id]);
		images[face_id] = TGAImage(g_image_size, g_image_size, TGAImage::RGB);
		
		threads[face_id] = std::thread(GenerateIrradianceMap, std::ref(images[face_id]), Face(face_id));
	}
	
	for (int face_id = 0; face_id < 6; ++face_id)
	{
		threads[face_id].join();
	}

	for (int face_id = 0; face_id < 6; ++face_id)
	{
		images[face_id].flip_vertically();
		images[face_id].write_tga_file(paths[face_id]);
	}
	
	cout << "Finish!!" << endl;
	return 0;
}