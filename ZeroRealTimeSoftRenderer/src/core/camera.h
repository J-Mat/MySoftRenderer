#pragma once

#include "math_ext.h"
using namespace Math;

enum CameraType
{
	CT_ORI,
	CT_PERSPECT,
};

const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;

struct CameraSettings
{
	float fovy;
	float aspect;
	vec3 world_up;
	CameraType camera_type;
};

class Camera
{
public:
	Camera(CameraSettings& settings, float speed = SPEED, float m_mouse_sensitivity = SENSITIVITY);
	~Camera() = default;
	
	void UpdateCamera(float delta_time);
private:
	CameraSettings m_settings;
	vec3 m_eye;
	vec3 m_lookat;
	vec3 m_forward;	
	vec3 m_right;
	vec3 m_up;
	vec3 m_world_up;
	float m_speed;
	float m_mouse_sensitivity;
	mat4 m_view_mat;
	mat4 m_project_mat;
private:
	void ProcessKeyboard(float delta_time);
	void ProcessMouseMovement(float delta_time);
	mat4 GetMVPMat();
public:
	mat4 GetViewMat() { return m_view_mat; }
	mat4 GetProjectMat() { return m_project_mat; }
	mat4 GetProjectViewMat() { return m_project_mat * m_view_mat; }
};
