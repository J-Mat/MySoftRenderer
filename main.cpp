#include <vector>
#include <cmath>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const int width  = 800;
const int height = 800;
const int depth = 255;

Model* model = NULL;
int* zbuffer = NULL;
Vec3f camera(0, 0, 3);

Vec3f light_dir = Vec3f(0, -1, -1).normalize();
Vec3f eye(2, 1, 3);
Vec3f center(0, 0, 1);


//4d-->3d
//除以最后一个分量。（当最后一个分量为0，表示向量）
//不为0，表示坐标
Vec3f m2v(Matrix m) 
{
	return Vec3f(m[0][0] / m[3][0], m[1][0] / m[3][0], m[2][0] / m[3][0]);
}

//3d-->4d
//添加一个1表示坐标
Matrix v2m(Vec3f v) 
{
	Matrix m(4, 1);
	m[0][0] = v.x;
	m[1][0] = v.y;
	m[2][0] = v.z;
	m[3][0] = 1.f;
	return m;
}


Matrix Viewport(int x, int y, int w, int h) 
{
	Matrix m = Matrix::identity(4);
	//第4列表示平移信息
	m[0][3] = x + w / 2.f;
	m[1][3] = y + h / 2.f;
	m[2][3] = depth / 2.f;
	//对角线表示缩放信息
	m[0][0] = w / 2.f;
	m[1][1] = h / 2.f;
	m[2][2] = depth / 2.f;
	return m;
}

Matrix Lookat(Vec3f eye, Vec3f center, Vec3f up)
{
	Vec3f forward = (eye - center).normalize();
	Vec3f right = (up ^ forward).normalize();
	up = (forward ^ right).normalize();
	
	Matrix rotation = Matrix::identity(4);
	Matrix translation = Matrix::identity(4);
	

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
	
	Matrix res = rotation * translation;
	return res;
}

void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color) 
{
	bool steep = false;
	if (std::abs(x0 - x1) < std::abs(y0 - y1)) 
	{
		std::swap(x0, y0);
		std::swap(x1, y1);
		steep = true;
	}
	if (x0 > x1) {
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

void GetBoundingBox(Vec2i& min_box, Vec2i& max_box, const std::vector<Vec3i>& pts)
{
	for (Vec3i pt : pts)
	{
		min_box.x = std::min(min_box.x, pt.x);
		min_box.y = std::min(min_box.y, pt.y);
		
		max_box.x = std::max(max_box.x, pt.x);
		max_box.y = std::max(max_box.y, pt.y);
	}
}

Vec3f GetBarycentric(const Vec3f& A, const Vec3f& B, const Vec3f& C, const Vec3f& P)
{
	Vec3f x_vec = { B.x - A.x, C.x - A.x, A.x - P.x };
	Vec3f y_vec = { B.y - A.y, C.y - A.y, A.y - P.y };
	Vec3f s = x_vec ^ y_vec;
	if (std::abs(s.z) > 1e-8)
	{
		s = { s.x / s.z, s.y / s.z, 1.0f };
		return  { 1.0f - (s.x + s.y), s.x, s.y };
	}
	return { -1, 1, 1 };
}

int GetImageIndex(int x, int y, TGAImage& image)
{
	return y * image.get_width() + x;
}


void DrawTriangle(const std::vector<Vec3i>& pts, const std::vector<Vec2i>& uv, const std::vector<float>& distance, const std::vector<float>& intensity,  int* zbuffer, TGAImage& image)
{
	Vec2i min_box = { image.get_width() - 1, image.get_height() - 1};
	Vec2i max_box = {0, 0};
	GetBoundingBox(min_box, max_box, pts);
	for (int x = min_box.x; x <= max_box.x; ++x)
	{
		for (int y = min_box.y; y <= max_box.y; ++y)
		{
			Vec3i P = { x, y, 0 };
			Vec3f barycentric_coord = GetBarycentric(pts[0], pts[1], pts[2], P);
			if (barycentric_coord.x < 0.0f || barycentric_coord.y < 0.0f || barycentric_coord.z < 0.0f)
			{
				continue;
			}
			P.z = pts[0].z * barycentric_coord.x + pts[1].z * barycentric_coord.y + pts[2].z * barycentric_coord.z;
			int index = GetImageIndex(x, y, image);
			if (index >= 0 && zbuffer[index] < int(P.z))
			{
				Vec2i uv_pos = uv[0] * barycentric_coord.x + uv[1] * barycentric_coord.y + uv[2] * barycentric_coord.z;
				float dis_iter = distance[0] * barycentric_coord.x + distance[1] * barycentric_coord.y + distance[2] * barycentric_coord.z;
				float itensity_iter = intensity[0] * barycentric_coord.x + intensity[1] * barycentric_coord.y + intensity[2] * barycentric_coord.z;
				itensity_iter = std::min(1.f, std::abs(itensity_iter) + 0.01f);
				zbuffer[index] = int(P.z);
				TGAColor color = model->diffuse(uv_pos);
				if (itensity_iter > 0.0f)
				image.set(P.x, P.y, TGAColor(color.r * itensity_iter, color.g * itensity_iter, color.b * itensity_iter, 255));
				//image.set(P.x, P.y, TGAColor(color.r, color.g, color.b, 255));
			}
		}
	}
}
/*
void triangle(Vec3f* pts, float* zbuffer, TGAImage& image, TGAColor color)
{
	Vec3i t0 = { pts[0].x, pts[0].y, pts[0].z };
	Vec3i t1 = { pts[1].x, pts[1].y, pts[1].z };
	Vec3i t2 = { pts[2].x, pts[2].y, pts[2].z };

	if (t0.y > t1.y) std::swap(t0, t1);
	if (t0.y > t2.y) std::swap(t0, t2);
	if (t1.y > t2.y) std::swap(t1, t2);
	
	int total_height = t2.y - t0.y;
	for (int i = 0; i < total_height; ++i)
	{
		bool second_half = i > t1.y - t0.y || t1.y == t0.y;
		int segment_height = second_half ? t2.y - t1.y : t1.y - t0.y;
		float alpha = (float)i / total_height;	
		float beta = second_half ? (float)(i - (t1.y - t0.y)) / segment_height : beta = (float)i / segment_height;
		Vec3i A = t0 + (t2 - t0) * alpha;
		Vec3i B = second_half ? t1 + (t2 - t1) * beta : t0 + (t1 - t0) * beta;
		if (A.x > B.x) std::swap(A, B);
		for (int j = A.x; j <= B.x; ++j)
		{
			int index = j * width + (t0.y + i);
			if (zbuffer[index] < pts)
			image.set(j, t0.y + i, color);
		}
	}	
}
*/

Vec3f world2screen(Vec3f v) 
{
    return Vec3f(int((v.x+1.)*width/2.+.5), int((v.y+1.)*height/2.+.5), v.z);
}


int main(int argc, char** argv) {
	zbuffer = new int[width * height];
	for (int i = 0; i < width * height; ++i)
	{
		zbuffer[i] = -std::numeric_limits<int>::min();
	}
	/*
	std::vector<Vec3f> pts = {Vec3f(0,0,0), Vec3f(100, 0, 0), Vec3f(100, 100, 0)};
	DrawTriangle(pts, zbuffer, image, red);
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");

	return 0;
	*/
	
	if (2 == argc) {
		model = new Model(argv[1]);
	}
	else {
		model = new Model("../obj/african_head/african_head.obj");
	}

	
	Matrix modelview = Lookat(eye, center, Vec3f(0, 1, 0));
	Matrix projection = Matrix::identity(4);
	projection[3][2] = -1.0f / (eye - center).norm();
	Matrix viewport = Viewport(width / 8, height / 8, width * 3 / 4, height * 3 / 4);

	{
		TGAImage image(width, height, TGAImage::RGB);
		for (int i = 0; i < model->nfaces(); i++) {
			std::vector<int> face = model->face(i);
			std::vector<Vec3i> screen_coords(3);
			std::vector<float> distance(3);
			std::vector<float> intensity(3);
 			Vec3f world_coords[3];
			for (int j = 0; j < 3; j++) {
				Vec3f v = model->vert(face[j]);
				Matrix world_matrix = modelview * Matrix(v);
				screen_coords[j] = m2v(viewport * projection * world_matrix);
				Vec3f world_pos = Vec3f(world_matrix);
				distance[j] = (world_pos - eye).norm();
				intensity[j] = model->norm(i, j) * light_dir;
				world_coords[j] = v;
			}
			Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
			n.normalize();
			std::vector<Vec2i> uv(3);
			for (int j = 0; j < 3; ++j) {
				uv[j] = model->uv(i, j);
			}
			DrawTriangle(screen_coords, uv, distance, intensity, zbuffer, image);
		}
		image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
		image.write_tga_file("output.tga");
	}

	{
		TGAImage zbimage(width, height, TGAImage::GRAYSCALE);
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++) 
			{
				zbimage.set(i, j, TGAColor(zbuffer[i + j * width], 1));
			}
		}
		zbimage.flip_vertically();
		zbimage.write_tga_file("zbuffer.tga");
	}
	delete model;
	return 0;
}

