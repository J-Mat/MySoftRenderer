#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
/*
* 做一个math的中间层，手撸投影矩阵变换，达到学习的目的（其实glm里啥都有）
*/
namespace Math
{
	using namespace glm;
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
};


