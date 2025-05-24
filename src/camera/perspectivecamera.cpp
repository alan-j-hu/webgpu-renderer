#include "noworry/camera/perspectivecamera.h"
#include "noworry/material/uniformlayout.h"
#include <cstring>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

PerspectiveCamera::PerspectiveCamera()
{
    m_position = glm::vec3(0.0f, 0.0f, -1.0f);
    m_target = glm::vec3(0.0f, 0.0f, 0.0f);
    m_up = glm::vec3(0.0f, 0.0f, 1.0f);
}

void PerspectiveCamera::set_position(const glm::vec3& position)
{
    m_position = position;
}

void PerspectiveCamera::set_target(const glm::vec3& target)
{
    m_target = target;
}

void PerspectiveCamera::update_matrix(CameraData* data)
{
    glm::mat4 proj = glm::perspectiveRH_ZO(
        glm::pi<float>() * 0.5f,
        4.0f / 3.0f,
        0.1f,
        100.f);
    glm::mat4 view = glm::lookAtRH(m_position, m_target, m_up);
    data->viewproj = proj * view;
}
