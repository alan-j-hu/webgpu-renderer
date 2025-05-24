#include "noworry/camera/orthographiccamera.h"
#include "noworry/material/uniformlayout.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

OrthographicCamera::OrthographicCamera()
    : m_left(0),
      m_right(16),
      m_bottom(0),
      m_top(16),
      m_near(0),
      m_far(100)
{
    m_position = glm::vec3(0.0f, 0.0f, -1.0f);
    m_target = glm::vec3(0.0f, 0.0f, 0.0f);
    m_up = glm::vec3(0.0f, 0.0f, 1.0f);
}

void OrthographicCamera::set_position(const glm::vec3& position)
{
    m_position = position;
}

void OrthographicCamera::set_target(const glm::vec3& target)
{
    m_target = target;
}

void OrthographicCamera::update_matrix(CameraData* data)
{
    glm::mat4 proj = glm::orthoRH_ZO(
      m_left, m_right, m_bottom, m_top, m_near, m_far);
    glm::mat4 view = glm::lookAtRH(m_position, m_target, m_up);
    data->viewproj = proj * view;
}
