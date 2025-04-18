#include "noworry/scene/camera.h"
#include "noworry/material/uniformlayout.h"
#include <cstring>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

Camera::Camera()
{
    m_position = glm::vec3(0.0f, 0.0f, -1.0f);
    m_target = glm::vec3(0.0f, 0.0f, 0.0f);
    m_up = glm::vec3(0.0f, 1.0f, 0.0f);
}

void Camera::set_position(const glm::vec3& position)
{
    m_position = position;
}

void Camera::set_target(const glm::vec3& target)
{
    m_target = target;
}

void Camera::update_matrix(CameraData* data)
{
    glm::mat4 proj = glm::perspectiveLH_ZO(
        glm::pi<float>() * 0.5f,
        4.0f / 3.0f,
        0.1f,
        100.f);
    glm::mat4 view = glm::lookAtLH(m_position, m_target, m_up);
    data->viewproj = proj * view;
}
