#include "noworry/camera/abstractcamera.h"
#include <cmath>
#include <glm/geometric.hpp>
#include <glm/vector_relational.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/ext/matrix_transform.hpp>

void AbstractCamera::set_position(const glm::vec3& position)
{
    m_position = position;
    update_lookat();
}

void AbstractCamera::set_target(const glm::vec3& target)
{
    m_target = target;
    update_lookat();
}


glm::vec3 AbstractCamera::position()
{
    return m_position;
}

glm::vec3 AbstractCamera::target()
{
    return m_target;
}

glm::vec3 AbstractCamera::up()
{
    return m_up;
}

void AbstractCamera::update_lookat()
{
    glm::vec3 lookat = m_target - m_position;

    // Represent rotation between FORWARD and lookat vector
    // as axis and angle
    glm::vec3 axis = glm::cross(FORWARD, lookat);
    if (glm::length(axis) < EPSILON) {
        if (glm::all(glm::epsilonEqual(FORWARD, -lookat, EPSILON))) {
            m_up = -m_up;
        }
        return;
    }

    float angle = std::acos(
      glm::dot(FORWARD, lookat)
      / glm::length(FORWARD)
      / glm::length(lookat));

    glm::mat4 rotation = glm::rotate(glm::identity<glm::mat4>(), angle, axis);

    // Apply the same rotation to UP to get the new up vector
    glm::vec4 up = rotation * glm::vec4(UP.x, UP.y, UP.z, 0);
    m_up = glm::vec3(up.x, up.y, up.z);
}
