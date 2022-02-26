#include <cmath>
#include <limits>
#include <cstdlib>
#include "our_gl.h"
#include <algorithm>
Matrix g_model_view;
Matrix g_viewport;
Matrix g_projection;



void Viewport(int x, int y, int w, int h)
{
	g_viewport = Matrix::identity();
	g_viewport[0][3] = x + w / 2.f;
	g_viewport[1][3] = y + h / 2.f;
	g_viewport[2][3] = 255.f / 2.f;
	g_viewport[0][0] = w / 2.f;
	g_viewport[1][1] = h / 2.f;
	g_viewport[2][2] = 255.f / 2.f;
}

void Projection(float coeff /*= 0.f*/)
{
	g_projection = Matrix::identity();
	g_projection[3][2] = coeff;
}

void Lookat(Vec3f eye, Vec3f center, Vec3f up)
{
	Vec3f forward = (eye - center).normalize();
	Vec3f right = cross(up , forward).normalize();
	up = cross(forward,right).normalize();

	Matrix rotation = Matrix::identity();
	Matrix translation = Matrix::identity();


	for (int i = 0; i < 3; ++i)
	{
		rotation[i][3] = -center[i];
	}

	for (int i = 0; i < 3; ++i)
	{
		rotation[0][i] = right[i];
		rotation[1][i] = up[i];
		rotation[2][i] = forward[i];
	}

	g_model_view = rotation * translation;
}

void GetBoundingBox(Vec2i& min_box, Vec2i& max_box, const std::vector<Vec4f>& pts)
{
	for (Vec4f pt : pts)
	{
		min_box.x = std::min(min_box.x, int(pt[0] / pt[3]));
		min_box.y = std::min(min_box.y, int(pt[1] / pt[3]));

		max_box.x = std::max(max_box.x, int(pt[0] / pt[3]));
		max_box.y = std::max(max_box.y, int(pt[1] / pt[3]));
	}
}

Vec3f GetBarycentric(const Vec2f& A, const Vec2f& B, const Vec2f& C, const Vec2f& P)
{
	Vec3f x_vec = { B.x - A.x, C.x - A.x, A.x - P.x };
	Vec3f y_vec = { B.y - A.y, C.y - A.y, A.y - P.y };
	Vec3f s = cross(x_vec, y_vec);
	if (std::abs(s.z) > 1e-8)
	{
		s = { s.x / s.z, s.y / s.z, 1.0f };
		return  { 1.0f - (s.x + s.y), s.x, s.y };
	}
	return { -1, 1, 1 };
}


void DrawTriangle(std::vector<Vec4f>& pts, IShader& shader, TGAImage& image, TGAImage& zbuffer)
{
	Vec2i min_box = { image.get_width() - 1, image.get_height() - 1 };
	Vec2i max_box = { 0, 0 };
	GetBoundingBox(min_box, max_box, pts);
	for (int x = min_box.x; x <= max_box.x; ++x)
	{
		for (int y = min_box.y; y <= max_box.y; ++y)
		{
			Vec2i P = { x, y};
			Vec3f barycentric_coord = GetBarycentric(proj<2>(pts[0] / pts[0][3]), proj<2>(pts[1] / pts[1][3]), proj<2>(pts[2] / pts[2][3]), proj<2>(P));
			if (barycentric_coord.x < 0.0f || barycentric_coord.y < 0.0f || barycentric_coord.z < 0.0f)
			{
				continue;
			}
			float z_P = (pts[0][2] / pts[0][3]) * barycentric_coord.x + (pts[0][2] / pts[1][3]) * barycentric_coord.y + (pts[0][2] / pts[2][3]) * barycentric_coord.z;
			int frag_depth = std::max(0, std::min(255, int(z_P + .5)));
			if (zbuffer.get(P.x, P.y)[0] > frag_depth)
			{
				continue;
			}
			TGAColor color;
			bool discard = shader.fragment(barycentric_coord, color);
			if (!discard)
			{
				zbuffer.set(P.x, P.y, TGAColor(frag_depth));
				image.set(P.x, P.y, color);
			}
			
		}
	}
}

void DrawLine(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
{
	bool steep = false;
	if (std::abs(x0 - x1) < std::abs(y0 - y1))
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if (x0 > x1) 
	{
		std::swap(x0, x1);
		std::swap(y0, y1);
	}
	int dx = x1 - x0;
	int dy = y1 - y0;
	int derror2 = std::abs(dy) * 2;
	int error2 = 0;
	int y = y0;
	for (int x = x0; x <= x1; x++)
	{
		if (steep)
		{
			image.set(y, x, color);
		}
		else
		{
			image.set(x, y, color);
		}
		error2 += derror2;
		if (error2 > dx)
		{
			y += (y1 > y0 ? 1 : -1);
			error2 -= dx * 2;
		}
	}
}
