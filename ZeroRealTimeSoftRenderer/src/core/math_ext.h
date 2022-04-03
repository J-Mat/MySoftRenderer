#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
/*
* 做一个math的中间层，手撸投影矩阵变换，达到学习的目的（其实glm里啥都有）
*/
#define EPSILON 1e-4f
namespace Math
{
	using namespace glm;
	using Color = vec4;
	// http://www.songho.ca/opengl/gl_camera.html
	mat4 GetLookAtMat(vec3 eye, vec3 target, vec3 up);
/*
 * fovy: the field of view angle in the y direction, in degrees
 * aspect: the aspect ratio, defined as width divided by height
 * near, far: the coordinates for the near and far clipping planes
 *
 * 1/(aspect*tan(fovy/2))              0             0           0
 *                      0  1/tan(fovy/2)             0           0
 *                      0              0  -(f+n)/(f-n)  -2fn/(f-n)
 *                      0              0            -1           0
 *
 * this is the same as
 *     float half_h = near * (float)tan(fovy / 2);
 *     float half_w = half_h * aspect;
 *     mat4_frustum(-half_w, half_w, -half_h, half_h, near, far);
 *
 * see http://www.songho.ca/opengl/gl_projectionmatrix.html
 *
 * note: my implementation is based on right-handed system, so it is a little different
 */
	mat4 GetPerspectMat(float fovy, float aspect, float near, float far);
	
	vec2 GetSreenCoord(int screen_width, int screen_height, vec3 ndc_coord);



	template <typename T>
	T Remap(T value, T start1, T stop1, T start2, T stop2)
	{
		return start2 + (value - start1) * (stop2 - start2) / (stop1 - start1);
	}

	// 通过UV算出TBN空间，
	//  https://learnopengl.com/Advanced-Lighting/Normal-Mapping
	//  https://zhuanlan.zhihu.com/p/144357517
	void GetTBN(vec3& T, vec3& B, vec3& N, vec3 & normal, vec2* texcoords, vec3* positions);	
};


