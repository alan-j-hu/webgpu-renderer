#include "noworry/camera/abstractcamera.h"
#include "noworry/Pipeline/MeshVertexShader.h"
#include <cmath>
#include <glm/geometric.hpp>
#include <glm/vector_relational.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

void AbstractCamera::init()
{
    update_lookat();
    update_viewproj();
}

void AbstractCamera::set_position(const glm::vec3& position)
{
    m_position = position;
    update_lookat();
    update_viewproj();
}

void AbstractCamera::set_target(const glm::vec3& target)
{
    m_target = target;
    update_lookat();
    update_viewproj();
}

glm::vec3 AbstractCamera::position() const
{
    return m_position;
}

glm::vec3 AbstractCamera::target() const
{
    return m_target;
}

glm::vec3 AbstractCamera::up() const
{
    return m_up;
}

void AbstractCamera::update_matrix(CameraData* data)
{
    data->viewproj = m_viewproj;
}

glm::vec3 AbstractCamera::project(const glm::vec3& pos) const
{
    glm::vec4 ndc = m_viewproj * glm::vec4(pos, 1);
    ndc /= ndc.w;
    return ndc;
}

glm::vec3 AbstractCamera::unproject(const glm::vec3& ndc) const
{
    glm::vec4 world = m_inv_viewproj * glm::vec4(ndc, 1);
    world /= world.w;
    return world;
}

void AbstractCamera::update_viewproj()
{
    glm::mat4 view = glm::lookAtRH(position(), target(), up());
    m_viewproj = proj() * view;
    m_inv_viewproj = inverse(m_viewproj);
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
