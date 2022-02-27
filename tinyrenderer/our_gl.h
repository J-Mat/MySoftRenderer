#ifndef __OUR_GL_H__
#define __OUR_GL_H__

#include "tgaimage.h"
#include "geometry.h"

extern Matrix g_model_view;
extern Matrix g_viewport;
extern Matrix g_projection;

void Viewport(int x, int y, int w, int h);
void Projection(float coeff = 0.f); // coeff = -1/c
void Lookat(Vec3f eye, Vec3f center, Vec3f up);



struct IShader
{
	virtual ~IShader() {};
	virtual Vec4f vertex(int iface, int nthvert) = 0;
	virtual bool fragment(Vec3f bar, TGAColor& color) = 0;
};


Vec3f GetBarycentric(const Vec2f& A, const Vec2f& B, const Vec2f& C, const Vec2f& P);
void GetBoundingBox(Vec2i& min_box, Vec2i& max_box, const std::vector<Vec3i>& pts);
void DrawTriangle(std::vector<Vec4f>& pts, IShader& shader, TGAImage& image, TGAImage& zbuffer);
void DrawLine(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color);

#endif //__OUR_GL_H__
