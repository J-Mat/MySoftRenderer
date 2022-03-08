#pragma once
#include "core.h"
using namespace Math;

enum CameraType
{
	CT_ORI,
	CT_PERSPECT,
};

const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;

class Camera
{
public:
	Camera(CameraType camera_type, vec3 eye, vec3 lookat, vec3 up, float speed = SPEED, float m_mouse_sensitivity = SENSITIVITY);
	~Camera() = default;
	
	void UpdateCamera(float delta_time);
private:
	CameraType m_type;
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
};
