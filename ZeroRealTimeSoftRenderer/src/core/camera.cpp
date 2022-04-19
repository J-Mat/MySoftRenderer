#include "camera.h"
#include "input.h"
#include "iostream"
#include "debug.h"


Camera::Camera(CameraSettings& settings) :
	m_settings(settings),
	m_project_mat(mat4(1.0f)),
	m_view_mat(mat4(1.0f))
{
}

void Camera::UpdateCamera(float delta_time)
{
	ProcessKeyboard(delta_time);
	if (Input::IsLeftButtonDown())
	{
		ProcessMouseMovement(delta_time);
	}

	UpdateMat();
}

void Camera::OnLeftBtnDown()
{
}

void Camera::OnLeftBtnUp()
{
}

void Camera::Init(vec3 eye, vec3 lookat)
{
	m_eye = eye;
	m_lookat = lookat;

	m_dir = normalize(m_eye - m_lookat);
	m_forward = -m_dir;
	m_radius = length(m_lookat - m_eye);
	
	m_phi = (float)atan2(m_dir.x, m_dir.z);
	m_theta = (float)acos(m_dir.y);
	UpdateMat();
}


void Camera::BindInput()
{
	Input::OnLeftBtnDownFunc = std::bind(&Camera::OnLeftBtnDown, this);
	Input::OnLeftBtnUpFunc = std::bind(&Camera::OnLeftBtnUp, this);
}

void Camera::UpdateMat()
{
	/*
	m_right = normalize(Math::cross(m_forward, m_settings.world_up));
	m_up = normalize(Math::cross(m_right, m_forward));
	*/
	
	m_view_mat = Math::GetLookAtMat(m_eye, m_lookat, m_settings.world_up);
	switch (m_settings.camera_type)
	{
	case CameraType::CT_ORI:	
	case CameraType::CT_PERSPECT:	
		m_project_mat = GetPerspectMat(m_settings.fovy, m_settings.aspect, -m_settings.near, -m_settings.far);
		break;
	default:
		break;
	}
}


void Camera::ProcessKeyboard(float delta_time)
{
	float velocity = m_settings.speed * delta_time;
	if (Input::IsKeyPressed('W'))
	{
		m_eye += m_forward * velocity;
		m_radius = length(m_eye - m_lookat);

	
		std::cout << m_radius << std::endl;
	}
	if (Input::IsKeyPressed('S'))
	{
		m_eye -= m_forward * velocity;
		m_radius = length(m_eye - m_lookat);
	}

	/*
	if (Input::IsKeyPressed('A'))
		m_eye -= m_right * velocity;
	if (Input::IsKeyPressed('D'))
		m_eye += m_right * velocity;
	if (Input::IsKeyPressed('Q'))
		m_eye += m_settings.world_up * velocity;
	if (Input::IsKeyPressed('E'))
		m_eye -= m_settings.world_up * velocity;
	*/
}


void Camera::ProcessMouseMovement(float delta_time)
{
	vec2 mouse_offset = Input::GetMouseOffset() * m_settings.mouse_sensivity * delta_time;

	m_phi -= mouse_offset.x;
	m_theta -= mouse_offset.y * 0.5;
	
	m_theta = Math::max(EPSILON, m_theta);
	m_theta = Math::min(PI - EPSILON, m_theta);
	
	vec3 dir = { sin(m_theta) * sin(m_phi), 
				cos(m_theta), 
				sin(m_theta) * cos(m_phi) };
	m_eye = m_lookat + dir * m_radius;

	m_dir = normalize(m_eye - m_lookat);
	m_forward = -m_dir;
}
