#include "noworry/camera/orthographiccamera.h"
#include "noworry/layout.h"
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
}

void OrthographicCamera::update_matrix(CameraData* data)
{
    glm::mat4 proj = glm::orthoRH_ZO(
      m_left, m_right, m_bottom, m_top, m_near, m_far);
    glm::mat4 view = glm::lookAtRH(position(), target(), up());
    data->viewproj = proj * view;
}
