#include <iostream>
#include "tgaimage.h"
#include <string>
#include "math_ext.h"
#include <math.h>
#include "mesh.h"
#include <thread>
#include "debug.h"
using namespace std;
using namespace Math;

std::shared_ptr<Mesh> g_skybox_mesh;
const int g_image_size = 512;
const int g_face_num = 6;

enum Face
{
	F_PX,
	F_NX,
	F_PY,
	F_NY,
	F_PZ,
	F_NZ
};

float SchlickGGX_geometry(float n_dot_v, float roughness)
{
	float r = (1 + roughness);
	float k = r * r / 8.0;
	k = roughness * roughness / 2.0f;
	return n_dot_v / (n_dot_v * (1 - k) + k);
}

float Geometry_Smith(float n_dot_v, float n_dot_l, float roughness)
{
	float g1 = SchlickGGX_geometry(n_dot_v, roughness);
	float g2 = SchlickGGX_geometry(n_dot_l, roughness);

	return g1 * g2;
}


float RadicalInverse_VdC(unsigned int bits) {
	bits = (bits << 16u) | (bits >> 16u);
	bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
	bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
	bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
	bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
	return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley2d(int i, int N) {
	return vec2(float(i) / float(N), RadicalInverse_VdC(i));
}
vec3 GetCubeMapDir(Face face, int x, int y, int image_size)
{
	float x_step = float(x) / (image_size - 1);
	float y_step = float(y) / (image_size - 1);
	
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
	cout << face << endl;
	for (int i = 0; i < g_image_size; ++i)
	{
		for (int j = 0; j < g_image_size; ++j)
		{
			vec3 dir = GetCubeMapDir(face, i, j, g_image_size);
			// 随便两个垂直于dir的向量，形成一个三维空间
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
					sample_dir = sample_dir.x * right + sample_dir.y * up + sample_dir.z * dir; //转到对应的空间
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
}

void IrradianceMap()
{
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

	std::thread tt;
	for (int face_id = 0; face_id < 6; ++face_id)
	{
		images[face_id].flip_vertically();
		images[face_id].write_tga_file(paths[face_id]);
	}
	cout << "IrradianceMap" << endl;
}

vec3 ImportanceSampleGGX(vec2 xi, vec3 normal, float roughness)
{
	float a = roughness * roughness;
	
	float phi = 2.0 * PI * xi.x;
	float cosTheta = sqrt((1.0 - xi.y) / (1.0 + (a * a - 1.0) * xi.y));
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

	// from spherical coordinates to cartesian coordinates
	vec3 h;
	h.x = cos(phi) * sinTheta;
	h.y = cosTheta;
	h.z = sin(phi) * sinTheta;


	vec3 up = Math::abs(normal.y) < 0.999f ? vec3(0.0f, 1.0f, 0.0f) : vec3(0.0f, 0.0f, 1.0f);
	vec3 tangent = normalize(cross(normal, up));
	vec3 bitangent = cross(normal, tangent);
	
	vec3 sample_vec = tangent * h.x + normal * h.y + bitangent * h.z;

	return sample_vec;
}


void GeneratePreFilterMap(Face face_id, int mip_level, int image_size, TGAImage& image, float roughness)
{
	for (int i = 0; i < image_size; ++i)
	{
		for (int j = 0; j < image_size; ++j)
		{
			vec3 normal = GetCubeMapDir(face_id, i, j, image_size);
				
			vec3 r = normal;
			vec3 v = r;    // 假设，近似
				
			vec3 prefilter_color(0, 0, 0);
			float total_weight = 0.0f;
			float num_samples = 1024;
			for (int i = 0; i < num_samples; ++i)
			{
				vec2 xi = Hammersley2d(i, num_samples);
				vec3 h = ImportanceSampleGGX(xi, normal, roughness);
				vec3 l = normalize(2.0f * dot(v, h) * h - v);
				
				vec3 radiance = Utils::CubemapSample(l, g_skybox_mesh->m_environment_map);
				float n_dot_l = fmax(dot(normal, l), 0.9f);
				if (n_dot_l > 0.0f)
				{
					prefilter_color += radiance * n_dot_l;
					total_weight += n_dot_l;
				}
			}
			prefilter_color = prefilter_color / total_weight;
			TGAColor pixel = Utils::ConvertColor2Pixel(prefilter_color);
			image.set(i, j, pixel);
		}
	}
}


void PreFilterMiplevel()
{
	const char* faces[6] = { "px", "nx", "py", "ny", "pz", "nz" };
	char paths[6][256];
	TGAImage images[6];
	
	int factor = 1;
	for (int mipmap_level = 0; mipmap_level < 10; ++mipmap_level)
	{
		int image_mipmap_size = g_image_size / factor;
		float roughness = float(mipmap_level) / 10;
		factor *= 2;
		image_mipmap_size = std::max(64, image_mipmap_size);
		for (int i = 0; i < 6; ++i)
		{
			sprintf(paths[i], "%s/m%d_%s.tga", "../res/temp", mipmap_level, faces[i]);
			images[i] = TGAImage(image_mipmap_size, image_mipmap_size, TGAImage::RGB);
		}

		std::thread threads[g_face_num];
		for (int face_id = 0; face_id < g_face_num; ++face_id)
		{
			threads[face_id] = std::thread(GeneratePreFilterMap, Face(face_id), mipmap_level, image_mipmap_size, std::ref(images[face_id]), roughness);
		}
		

		for (int face_id = 0; face_id < g_face_num; ++face_id)
		{
			threads[face_id].join();
		}

		for (int face_id = 0; face_id < g_face_num; ++face_id)
		{
			images[face_id].flip_vertically();
			images[face_id].write_tga_file(paths[face_id]);
		}
	}

	cout << "PreFilterMiplevel" << endl;
}

vec3 IntegrateBRDF(float n_dot_v, float roughness)
{
	vec3 n = vec3(0.0f, 1.0f, 0.0f);
	// 各向同性， 随意取一个v
	vec3 v = {sqrt(1.0f - n_dot_v * n_dot_v), n_dot_v, 0.0f};
	
	float A = 0.0f, B = 0.0f;
	
	const int sample_count = 1024;
	
	for (int i = 0; i < sample_count;++i)
	{ 
		// generates a sample vector that's biased towards the
		// preferred alignment direction (importance sampling).
		vec2 xi = Hammersley2d(i, sample_count);
		vec3 h = ImportanceSampleGGX(xi, n, roughness);
		vec3 l = normalize(2.0f * dot(v, h) * h - v);
		
		float n_dot_l = fmax(l.y, 0.0f);
		float n_dot_v = fmax(v.y, 0.0f);
		float n_dot_h = fmax(h.y, 0.0f);
		float v_dot_h = fmax(dot(v, h), 0.0f);
		
		if (n_dot_l > 0.0f)
		{
			float G = Geometry_Smith(n_dot_v, n_dot_l, roughness);
			float G_Vis = (G * v_dot_h) / (n_dot_h * n_dot_v);
			float Fc = pow(1.0 - v_dot_h, 5.0);

			A += (1.0 - Fc) * G_Vis;
			B += Fc * G_Vis;
		}
	}
	
	return vec3(A, B, 0.0f) / float(sample_count);

}

void BRDF_LUT()
{
	const int image_size = 256;
	TGAImage image = TGAImage(image_size, image_size, TGAImage::RGB);;
	for (int i = 0; i < image_size; ++i)
	{
		for (int j = 0; j < image_size; ++j)
		{
			float n_dot_v = i == 0 ? 0.002f : float(i) / image_size;
			float roughness = float(j) / image_size;
			vec3 color = IntegrateBRDF(n_dot_v, roughness);

			TGAColor pixel = Utils::ConvertColor2Pixel(color);
			image.set(i, j, pixel);
		}
	}
	

	char path[256];
	sprintf(path, "%s/budf_lut.tga", "../res/temp");

	image.flip_vertically();
	image.write_tga_file(path);

	cout << "BRDF_LUT" << endl;
}

// 这里是整个项目最难理解的一部分，为了达到更好的渲染效果，上IBL
// 上强度之前，要先了解蒙特卡洛积分原理


// radiance irradiance BRDF基本概念 games101 P14 P15 讲的非常好，多看几遍就理解了
// Cook-Torrance BRDF
// https://zhuanlan.zhihu.com/p/152226698
// 
// 
// 根据图形学第一定律：如果它看起来是对的，那它就是对的
// // 这里用到了一个很重要积分相似公式 Spilt Sum Aprroximation（https://zhuanlan.zhihu.com/p/429965810）
// 参考：
// games202 P5
// 
// IBL原理 公式推导 
// 最后整上以下全局光照的总结
// https://zhuanlan.zhihu.com/p/261005894
// https://zhuanlan.zhihu.com/p/66518450
int main()
{
	g_skybox_mesh = std::make_shared<Mesh>("../res/skybox4/box.obj", true);

	IrradianceMap();
	PreFilterMiplevel();
	BRDF_LUT();

	cout << "Finish!!" << endl;
	return 0;
}