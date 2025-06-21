#include "noworry/camera/orthographiccamera.h"
#include "noworry/Pipeline/MeshVertexShader.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

OrthographicCamera::OrthographicCamera()
    : m_left(-1),
      m_right(17),
      m_bottom(-1),
      m_top(17),
      m_near(0),
      m_far(100)
{
    init();
}

glm::mat4 OrthographicCamera::proj() const
{
    return glm::orthoRH_ZO(m_left, m_right, m_bottom, m_top, m_near, m_far);
}
