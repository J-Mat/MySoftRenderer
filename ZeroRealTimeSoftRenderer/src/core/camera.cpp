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

	// �� 0 �Ƕ�����������  �� 1 �ǶԵ����
	mat4 yaw_mat = mat4(1.0f);
	yaw_mat = Math::rotate(yaw_mat, Math::radians(mouse_offset.x), glm::vec3(0.0f, 1.0f, 0.0f));
	m_forward = yaw_mat * vec4(m_forward, 0.0f);

	mat4 pitch_mat = mat4(1.0f);
	pitch_mat = Math::rotate(pitch_mat, Math::radians(mouse_offset.y), m_right);
	m_forward = pitch_mat * vec4(m_forward, 0.0f);
	// ��picth�ĽǶȽ�������
	m_forward.y = Math::clamp(m_forward.y, -0.9f, +0.9f);
	m_forward = Math::normalize(m_forward);
}
