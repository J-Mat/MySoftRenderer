#include "camera.h"
#include "input.h"
Camera::Camera(CameraType camera_type, vec3 eye, vec3 lookat, vec3 up, float speed /*= SPEED*/, float mouse_sensitivity /*= SENSITIVITY*/) :
	m_type(camera_type),
	m_eye(eye),
	m_lookat(lookat),
	m_world_up(up),
	m_speed(speed),
	m_mouse_sensitivity(mouse_sensitivity)
	
{	
	m_forward = normalize(m_lookat - m_eye);
	m_right = normalize(Math::cross(m_forward, m_up));
	m_up = normalize(Math::cross(m_right, m_forward));
	
	
}


void Camera::UpdateCamera(float delta_time)
{
	ProcessKeyboard(delta_time);
	ProcessMouseMovement(delta_time);
}

void Camera::ProcessKeyboard(float delta_time)
{
	float velocity = m_speed * delta_time;
	if (Input::IsKeyPressed('W'))
		m_eye += m_forward * velocity;
	if (Input::IsKeyPressed('A'))
		m_eye -= m_right * velocity;
	if (Input::IsKeyPressed('S'))
		m_eye -=  m_forward * velocity;
	if (Input::IsKeyPressed('D'))
		m_eye +=  m_right * velocity;
}

void Camera::ProcessMouseMovement(float delta_time)
{	
	vec2 mouse_offset = Input::GetMouseOffset() * m_mouse_sensitivity;
	
	m_pitch += mouse_offset.x;
	m_yaw += mouse_offset.y;
	
	m_pitch = Math::clamp(m_pitch, -89.0f, 89.0f);


	m_forward.x = cos(Math::radians(m_yaw)) * cos(glm::radians(m_pitch));
	m_forward.y = sin(Math::radians(m_pitch));
	m_forward.z = sin(Math::radians(m_yaw)) * cos(glm::radians(m_pitch));
}

