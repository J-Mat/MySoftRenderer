#include <vector>
#include <iostream>
#include <algorithm>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"
#include "our_gl.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const int width  = 800;
const int height = 800;
const int depth = 255;

Model* model = NULL;
int* zbuffer = NULL;
Vec3f camera(0, 0, 3);

Vec3f light_dir(0, 1, 1);
Vec3f eye(1, 0.5, 1.5);
Vec3f center(0, 0, 0);
Vec3f up(0, 1, 0);


struct GourandShader : public IShader
{
	GourandShader() = default;
	Vec3f varying_intensity;
	mat<2, 3, float> varying_uv;
	virtual Vec4f vertex(int iface, int nthvert)
	{
		Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert));
		varying_uv.set_col(nthvert, model->uv(iface, nthvert));

		mat<4, 4, float> uniform_M = g_projection * g_model_view;
		mat<4, 4, float> uniform_MIT = g_model_view.invert_transpose();
		gl_Vertex = g_viewport * uniform_M * gl_Vertex;
		
		Vec3f normal = proj<3>(embed<4>(model->normal(iface, nthvert))).normalize();
		varying_intensity[nthvert] = std::max(0.f, model->normal(iface, nthvert) * light_dir); // get diffuse lighting intensity
		return gl_Vertex;
	}

	virtual bool fragment(Vec3f bar, TGAColor& color)
	{
		Vec2f uv = varying_uv * bar;
		color = model->diffuse(uv);
		float intensity = varying_intensity * bar;
		color = color * intensity;
		return false;
	}
};


struct PhongShader : public IShader {
	mat<2, 3, float> varying_uv;  // same as above
	mat<4, 4, float> uniform_M = g_projection * g_model_view;
	mat<4, 4, float> uniform_MIT = g_model_view.invert_transpose();
	virtual Vec4f vertex(int iface, int nthvert) {
		varying_uv.set_col(nthvert, model->uv(iface, nthvert));
		Vec4f gl_Vertex = embed<4>(model->vert(iface, nthvert)); // read the vertex from .obj file
		return g_viewport * g_projection * g_model_view * gl_Vertex; // transform it to screen coordinates
	}
	virtual bool fragment(Vec3f bar, TGAColor& color) {
		Vec2f uv = varying_uv * bar;
		Vec3f n = proj<3>(uniform_MIT * embed<4>(model->normal(uv))).normalize();
		Vec3f l = proj<3>(uniform_M * embed<4>(light_dir)).normalize();
		Vec3f r = (n * (n * l * 2.f) - l).normalize();   // reflected light
		float spec = pow(std::max(r.z, 0.0f), model->specular(uv));
		float diff = std::max(0.f, n * l);
		TGAColor c = model->diffuse(uv);
		color = c;
		for (int i = 0; i < 3; i++) color[i] = std::min<float>(5 + c[i] * (diff + .6 * spec), 255);
		return false;
	}
};

int main(int argc, char** argv) 
{
	if (2 == argc) {
		model = new Model(argv[1]);
	}
	else {
		model = new Model("../obj/african_head/african_head.obj");
	}
	//初始化变换矩阵，投影矩阵，视角矩阵
	Lookat(eye, center, up);
	Projection(-1.f / (eye - center).norm());
	Viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);
	light_dir.normalize();
	//初始化image和zbuffer
	TGAImage image(width, height, TGAImage::RGB);
	TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);
		
	PhongShader shader;
	
	for (int i = 0; i < model->nfaces(); ++i)
	{
		std::vector<Vec4f> screen_coords; 
		screen_coords.resize(3);
		for (int j = 0; j < 3; ++j)
		{	
			screen_coords[j] = shader.vertex(i, j);
		}
		DrawTriangle(screen_coords, shader, image, zbuffer);
	}
	image.flip_vertically();
	zbuffer.flip_vertically();
	image.write_tga_file("output_phone.tga");
	zbuffer.write_tga_file("zbuffer.tga");

	delete model;
	return 0;
}

