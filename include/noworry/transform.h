#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "layout.h"
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

struct Renderer;

/// A 3D transformation mapped to GPU memory.
class Transform
{
public:
    Transform(Renderer&);
    ~Transform();

    WGPUBindGroup bind_group(Renderer& renderer);

    const glm::vec3& translation() { return m_translation; }
    void set_translation(const glm::vec3& v) { m_translation = v; }

private:
    ModelUniforms m_model;
    WGPUBuffer m_buffer;
    WGPUBindGroup m_bind_group;
    glm::vec3 m_translation;
    float m_yaw;
    glm::vec3 m_scale;

    void update_matrix(Renderer& renderer);
};

#endif
