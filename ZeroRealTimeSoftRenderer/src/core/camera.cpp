#include "camera.h"
#include "input.h"


Camera::Camera(CameraSettings& settings) :
	m_settings(settings),
	m_project_mat(mat4(1.0f)),
	m_view_mat(mat4(1.0f))
{
}

void Camera::UpdateCamera(float delta_time)
{
	ProcessKeyboard(delta_time);
	ProcessMouseMovement(delta_time);

	UpdateMat();
}

void Camera::Init(vec3 eye, vec3 lookat)
{
	m_eye = eye;
	m_lookat = lookat;

	m_forward = normalize(m_lookat - m_eye);
	
	UpdateMat();
}


void Camera::UpdateMat()
{
	m_right = normalize(Math::cross(m_forward, m_settings.world_up));
	m_up = normalize(Math::cross(m_right, m_forward));
	
	m_lookat = m_eye + m_forward;
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
	m_project_view_mat = m_project_mat * m_view_mat;
}


void Camera::ProcessKeyboard(float delta_time)
{
	float velocity = m_settings.speed * delta_time;
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
	vec2 mouse_offset = Input::GetMouseOffset() * m_settings.mouse_sensivity;

	// 加 0 是对向量操作，  加 1 是对点操作
	mat4 yaw_mat = mat4(1.0f);
	yaw_mat = Math::rotate(yaw_mat, Math::radians(mouse_offset.x), glm::vec3(0.0f, 1.0f, 0.0f));
	m_forward = yaw_mat * vec4(m_forward, 0.0f);

	mat4 pitch_mat = mat4(1.0f);
	pitch_mat = Math::rotate(pitch_mat, Math::radians(mouse_offset.y), m_right);
	m_forward = pitch_mat * vec4(m_forward, 0.0f);
	// 对picth的角度进行限制
	m_forward.y = Math::clamp(m_forward.y, -0.9f, +0.9f);
	m_forward = Math::normalize(m_forward);
}
