#include "mesh.h"

#include <io.h> 
#include <fstream>
#include <sstream>

#include "shader/shader.h"

Mesh::Mesh(const char *filename, int is_skybox, int is_from_mmd)
	: is_skybox(is_skybox), is_from_mmd(is_from_mmd)
{
	std::ifstream in;
	in.open(filename, std::ifstream::in);
	if (in.fail())
	{
		printf("load model failed\n");
		m_positons.push_back(vec3(-1.0f, 0.0f, 0.0f));
		m_positons.push_back(vec3(1.0f, 0.0f, 0.0f));
		m_positons.push_back(vec3(0.0f, 1.0f, 0.0f));
		m_normals.push_back(vec3(0.0f, 1.0f, 0.0f));
		std::vector<int> n0 = { 0, 0, 0,      1, 0, 0,   2, 0, 0};
		m_faces.push_back(n0);
		return;
	}

	std::string line;
	while (!in.eof()) {
		std::getline(in, line);
		std::istringstream iss(line.c_str());
		char trash;
		if (!line.compare(0, 2, "v ")) 
		{
			iss >> trash;
			vec3 v;
			for (int i = 0; i < 3; i++)
				iss >> v[i];
		
			m_positons.push_back(v);
		}
		else if (!line.compare(0, 3, "vn ")) 
		{
			iss >> trash >> trash;
			vec3 n;
			for (int i = 0; i < 3; i++) 
				iss >> n[i];

			m_normals.push_back(n);
		}
		else if (!line.compare(0, 3, "vt ")) 
		{
			iss >> trash >> trash;
			vec2 uv;
			for (int i = 0; i < 2; i++) 
				iss >> uv[i];

			if (is_from_mmd)
				uv[1] = 1 - uv[1];
			m_texcoords.push_back(uv);
		}
		else if (!line.compare(0, 2, "f ")) 
		{
			std::vector<int> f;
			int tmp[3];
			iss >> trash;
			while (iss >> tmp[0] >> trash >> tmp[1] >> trash >> tmp[2]) 
			{
				for (int i = 0; i < 3; i++) 
					tmp[i]--; // in wavefront obj all indices start at 1, not zero

				f.push_back(tmp[0]); f.push_back(tmp[1]); f.push_back(tmp[2]);
			}
			m_faces.push_back(f);
		}
	}
	std::cerr << "# v# " << m_positons.size() << " f# " << m_faces.size() << " vt# " << m_texcoords.size() << " vn# " << m_normals.size() << std::endl;

	CreateMap(filename);

	m_environment_map = nullptr;
	if (is_skybox)
	{
		m_environment_map = new Cubemap();
		LoadCubemap(filename);
	}
}

Mesh::~Mesh() 
{
	if (m_diffuse_map) delete m_diffuse_map; m_diffuse_map = nullptr;
	if (m_normal_map) delete m_normal_map; m_normal_map = nullptr;
	if (m_specular_map) delete m_specular_map; m_specular_map = nullptr;
	if (m_roughness_map) delete m_roughness_map; m_roughness_map = nullptr;
	if (m_metalness_map) delete m_metalness_map; m_metalness_map = nullptr;
	if (m_occlusion_map) delete m_occlusion_map; m_occlusion_map = nullptr;
	if (m_emision_map) delete m_emision_map; m_emision_map = nullptr;

	if (m_environment_map)
	{
		for (int i = 0; i < 6; i++)
			delete m_environment_map->faces[i];
		delete m_environment_map;
	}
}

void Mesh::CreateMap(const char *filename)
{
	m_diffuse_map		= nullptr;
	m_normal_map		= nullptr;
	m_specular_map		= nullptr;
	m_roughness_map	= nullptr;
	m_metalness_map	= nullptr;
	m_occlusion_map	= nullptr;
	m_emision_map		= nullptr;

	std::string texfile(filename);
	size_t dot = texfile.find_last_of(".");

	texfile = texfile.substr(0, dot) + std::string("_diffuse.tga");
	if (_access(texfile.data(), 0) != -1)
	{
		m_diffuse_map = new TGAImage();
		LoadTexture(filename, "_diffuse.tga", m_diffuse_map);
	}

	texfile = texfile.substr(0, dot) + std::string("_normal.tga");
	if (_access(texfile.data(), 0) != -1)
	{
		m_normal_map = new TGAImage();
		LoadTexture(filename, "_normal.tga", m_normal_map);
	}

	texfile = texfile.substr(0, dot) + std::string("_spec.tga");
	if (_access(texfile.data(), 0) != -1)
	{
		m_specular_map = new TGAImage();
		LoadTexture(filename, "_spec.tga", m_specular_map);
	}

	texfile = texfile.substr(0, dot) + std::string("_roughness.tga");
	if (_access(texfile.data(), 0) != -1)
	{
		m_roughness_map = new TGAImage();
		LoadTexture(filename, "_roughness.tga", m_roughness_map);
	}

	texfile = texfile.substr(0, dot) + std::string("_metalness.tga");
	if (_access(texfile.data(), 0) != -1)
	{
		m_metalness_map = new TGAImage();
		LoadTexture(filename, "_metalness.tga", m_metalness_map);
	}

	texfile = texfile.substr(0, dot) + std::string("_emission.tga");
	if (_access(texfile.data(), 0) != -1)
	{
		m_emision_map = new TGAImage();
		LoadTexture(filename, "_emission.tga", m_emision_map);
	}

	texfile = texfile.substr(0, dot) + std::string("_occlusion.tga");
	if (_access(texfile.data(), 0) != -1)
	{
		m_occlusion_map = new TGAImage();
		LoadTexture(filename, "_occlusion.tga", m_metalness_map);
	}
}

void Mesh::LoadCubemap(const char *filename)
{
	m_environment_map->faces[0] = new TGAImage();
	LoadTexture(filename, "_right.tga", m_environment_map->faces[0]);
	m_environment_map->faces[1] = new TGAImage();
	LoadTexture(filename, "_left.tga", m_environment_map->faces[1]);
	m_environment_map->faces[2] = new TGAImage();
	LoadTexture(filename, "_top.tga", m_environment_map->faces[2]);
	m_environment_map->faces[3] = new TGAImage();
	LoadTexture(filename, "_bottom.tga", m_environment_map->faces[3]);
	m_environment_map->faces[4] = new TGAImage();
	LoadTexture(filename, "_back.tga", m_environment_map->faces[4]);
	m_environment_map->faces[5] = new TGAImage();
	LoadTexture(filename, "_front.tga", m_environment_map->faces[5]);
}

int Mesh::GetVertexSize() 
{
	return (int)m_positons.size();
}

int Mesh::GetFaceSize() 
{
	return (int)m_faces.size();
}

std::vector<int> Mesh::Face(int idx) 
{
	std::vector<int> face;
	for (int i = 0; i < 3; i++) 
		face.push_back(m_faces[idx][i*3]);
	return face;
}

vec3 Mesh::Position(int i) 
{
	return m_positons[i];
}

vec3 Mesh::Position(int iface, int nthvert) 
{
	return m_positons[m_faces[iface][nthvert*3]];
}

vec2 Mesh::Texcoord(int iface, int nthvert) 
{
	return m_texcoords[m_faces[iface][nthvert * 3+1]];
}

vec3 Mesh::Normal(int iface, int nthvert) 
{
	int idx = m_faces[iface][nthvert * 3+2];
	return glm::normalize(m_normals[idx]);
}

vec3 Mesh::FaceNormal(int iface)
{
	vec3 p0 = Position(iface, 0);
	vec3 p1 = Position(iface, 1);
	vec3 p2 = Position(iface, 2);
	vec3 edg_0 = normalize(p1 - p0);
	vec3 edg_1 = normalize(p2 - p0);
	return cross(edg_0, edg_1);
}

void Mesh::LoadTexture(std::string filename, const char *suffix, TGAImage &img) 
{
	std::string texfile(filename);
	size_t dot = texfile.find_last_of(".");
	if (dot != std::string::npos) 
	{
		texfile = texfile.substr(0, dot) + std::string(suffix);
		img.read_tga_file(texfile.c_str());
		img.flip_vertically();
	}
}

void Mesh::LoadTexture(std::string filename, const char *suffix, TGAImage *img) 
{
	std::string texfile(filename);
	size_t dot = texfile.find_last_of(".");
	if (dot != std::string::npos) {
		texfile = texfile.substr(0, dot) + std::string(suffix);
		img->read_tga_file(texfile.c_str());
		img->flip_vertically();
	}
}

vec3 Mesh::Diffuse(vec2 uv) 
{
	uv[0] = fmod(uv[0], 1);
	uv[1] = fmod(uv[1], 1);
	int uv0 = uv[0] * m_diffuse_map->get_width();
	int uv1 = uv[1] * m_diffuse_map->get_height();
	TGAColor c = m_diffuse_map->get(uv0, uv1);
	vec3 res;
	for (int i = 0; i < 3; i++)
		res[2 - i] = (float)c[i] / 255.f;
	return res;
}

vec3 Mesh::Normal(vec2 uv) 
{
	uv[0] = fmod(uv[0], 1);
	uv[1] = fmod(uv[1], 1);
	int uv0 = uv[0] * m_normal_map->get_width();
	int uv1 = uv[1] * m_normal_map->get_height();
	TGAColor c = m_normal_map->get(uv0, uv1);
	vec3 res;
	for (int i = 0; i < 3; i++)
		res[2 - i] = (float)c[i] / 255.f*2.f - 1.f; //because the normap_map coordinate is -1 ~ +1
	return res;
}

float Mesh::Roughness(vec2 uv) 
{
	uv[0] = fmod(uv[0], 1);
	uv[1] = fmod(uv[1], 1);
	int uv0 = uv[0] * m_roughness_map->get_width();
	int uv1 = uv[1] * m_roughness_map->get_height();
	return m_roughness_map->get(uv0, uv1)[0] / 255.f;
}

float Mesh::Metalness(vec2 uv) 
{
	uv[0] = fmod(uv[0], 1);
	uv[1] = fmod(uv[1], 1);
	int uv0 = uv[0] * m_metalness_map->get_width();
	int uv1 = uv[1] * m_metalness_map->get_height();
	return m_metalness_map->get(uv0, uv1)[0] / 255.f;
}

float Mesh::Specular(vec2 uv) 
{
	int uv0 = uv[0] * m_specular_map->get_width();
	int uv1 = uv[1] * m_specular_map->get_height();
	return m_specular_map->get(uv0, uv1)[0] / 1.f;
}

float Mesh::Occlusion(vec2 uv) 
{
	if (!m_occlusion_map)
		return 1;
	uv[0] = fmod(uv[0], 1);
	uv[1] = fmod(uv[1], 1);
	int uv0 = uv[0] * m_occlusion_map->get_width();
	int uv1 = uv[1] * m_occlusion_map->get_height();
	return m_occlusion_map->get(uv0, uv1)[0] / 255.f;
}

vec3 Mesh::Emission(vec2 uv)
{
	if (!m_emision_map)
		return vec3(0.0f, 0.0f, 0.0f);
	uv[0] = fmod(uv[0], 1);
	uv[1] = fmod(uv[1], 1);
	int uv0 = uv[0] * m_emision_map->get_width();
	int uv1 = uv[1] * m_emision_map->get_height();
	TGAColor c = m_emision_map->get(uv0, uv1);
	vec3 res;
	for (int i = 0; i < 3; i++)
		res[2 - i] = (float)c[i] / 255.f;
	return res;
}


