#pragma once
#include "math_ext.h"

/*
* 做一个math的中间层，手撸投影矩阵变换，达到学习的目的（其实glm里啥都有）
*/
namespace Math
{
	// http://www.songho.ca/opengl/gl_camera.html
	mat4 GetLookAtMat(vec3 eye, vec3 target, vec3 up)
	{
		mat4 m = mat4(1.0f);
		vec3 forward = glm::normalize(eye - target);
		vec3 left = glm::normalize(glm::cross(up, forward));
		up = glm::normalize(glm::cross(forward, left));
		
		m[0][0] = left[0];
		m[0][1] = left[1];
		m[0][2] = left[2];
		
		m[1][0] = up[0];
		m[1][1] = up[1];
		m[1][2] = up[2];
		
		m[2][0] = forward[0];
		m[2][1] = forward[1];
		m[2][2] = forward[2];

		m[3][0] = -glm::dot(left,eye);
		m[3][1] = -glm::dot(up, eye);
		m[3][2] = -glm::dot(forward, eye);

		return m;
	}
	/*
	 * left, right: the coordinates for the left and right clipping planes
	 * bottom, top: the coordinates for the bottom and top clipping planes
	 * near, far: the coordinates for the near and far clipping planes
	 *
	 * 2/(r-l)        0         0  -(r+l)/(r-l)
	 *       0  2/(t-b)         0  -(t+b)/(t-b)
	 *       0        0   2/(n-f)  -(f+n)/(n-f)
	 *       0        0         0             1
	 * see http://docs.gl/gl2/glOrtho
	 *
	 * note: opgenl map the near plane to -1, far plane to 1,
	 *       but i map the near plane to 1, far plane to -1.
	 *       And if near and far is positive it means the plane is behind viewer.
	 */
	mat4 GetOrthoMat(float left, float right, float bottom, float top, float near, float far)
	{
		float x_range = right - left;
		float y_range = top - bottom;
		float z_range = near - far;
		mat4 m = mat4(1.0);
		m[0][0] = 2 / x_range;
		m[1][1] = 2 / y_range;
		m[2][2] = 2 / z_range;
		
		m[0][3] = -(left + right) / x_range;
		m[1][3] = -(bottom + top) / y_range;
		m[2][3] = -(near + far) / z_range;
		
		return m;
	}

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
	mat4 GetPerspectMat(float fovy, float aspect, float near, float far)
	{
		mat4 m = mat4(1.0);
		fovy = glm::radians(fovy);
		float t = std::abs(near) * std::tan(fovy / 2);
		float r = aspect * t;
		
		m[0][0] = near / r;
		m[1][1] = near / t;
		m[2][2] = (near + far) / (near - far);
		m[3][2] = -2 * near * far / (far - near);
		m[2][3] = -1;
		m[3][3] = 0;
		
		return m;
	}
	
	
	void GetTBN(vec3& T, vec3& B, vec3& N, vec3 & normal, vec2* texcoords, vec3* positions)
	{
		float x1 = texcoords[1][0] - texcoords[0][0];
		float y1 = texcoords[1][1] - texcoords[0][1];
		
		float x2 = texcoords[2][0] - texcoords[0][0];
		float y2 = texcoords[2][1] - texcoords[0][1];
		
		float det = 1.0f / (x1 * y2 - x2 * y1);
		
		vec3 e1 = positions[1] - positions[0];
		vec3 e2 = positions[2] - positions[0];
		
		vec3 t = e1 * y2 - e2 * y1;
		vec3 b = e1 * (-x2) + e2 * x1;
		
		t *= det;
		b *= det;
		
		// 斯密特正交
		//https://www.matongxue.com/madocs/789/
		N = normalize(normal);
		T = normalize(t - dot(t, normal) * normal);
		B = normalize(b - dot(b, normal) * normal - dot(b, t) * t);
	}
};


