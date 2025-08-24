#include "noworry/transform.h"
#include "noworry/renderer.h"
#include <glm/ext/matrix_transform.hpp>

Transform::Transform()
{
    m_translation = glm::vec3(0.0f, 0.0f, 0.0f);
    m_yaw = 0.0f;
    m_scale = glm::vec3(1.0f, 1.0f, 1.0f);
}

glm::mat4 Transform::matrix() const
{
    glm::mat4 transform = glm::mat4(1);
    // Unintuitively, the matrix argument is the *left* mtrix (i.e. the
    // the *next* step in the transformation. Therefore, the code is
    // written in the opposite order than the transformations.
    transform = glm::translate(transform, m_translation);
    transform = glm::rotate(transform, m_yaw, glm::vec3(0, 1, 0));
    transform = glm::scale(transform, m_scale);
    return transform;
}
