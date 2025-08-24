#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "Pipeline/MeshVertexShader.h"
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

struct Renderer;

/// A 3D transformation mapped to GPU memory.
class Transform
{
public:
    Transform();

    const glm::vec3& translation() { return m_translation; }
    void set_translation(const glm::vec3& v) { m_translation = v; }

    glm::mat4 matrix() const;

private:
    glm::vec3 m_translation;
    float m_yaw;
    glm::vec3 m_scale;
};

#endif
