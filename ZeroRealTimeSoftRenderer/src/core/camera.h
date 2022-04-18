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
	CameraSettings() = default;
	float fovy = 60.0f;
	float aspect = 4.0f / 3.0f;
	vec3 world_up = {0.0f, 1.0f, 0.0f};
	float speed = SPEED;
	float mouse_sensivity = SENSITIVITY;
	CameraType camera_type =  CameraType::CT_PERSPECT;
	
	float near = 0.1f;
	float far = 10000.0f;
	float left = 0.0f;
	float right = 0.0f;
	float bottom = 0.0f;
	float top = 0.0f;
};

class Camera
{
public:
	Camera(CameraSettings& settings);
	~Camera() = default;
	void Init(vec3 eye, vec3 lookat);
	void BindInput();
	void UpdateCamera(float delta_time);

	void OnLeftBtnDown();
	void OnLeftBtnUp();
private:
	CameraSettings m_settings;
	vec3 m_eye;
	vec3 m_lookat;
	vec3 m_forward;	
	float  m_radius;
	vec3 m_beg_forward;
	vec3 m_end_forward;
	vec3 m_right;
	vec3 m_up;
	mat4 m_view_mat;
	mat4 m_project_mat;
	float m_yaw;
	float m_pitch;
private:
	void UpdateMat();
	void ProcessKeyboard(float delta_time);
	void ProcessMouseMovement(float delta_time);
public:
	vec3 GetViewPos() { return m_eye; }
	mat4 GetViewMat() { return m_view_mat; }
	mat4 GetProjectMat() { return m_project_mat; }
};
