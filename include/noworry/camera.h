#ifndef CAMERA_H
#define CAMERA_H

#include "effect.h"
#include <glm/vec3.hpp>

/* The camera uses a left-handed coordinate system with Y-up. */
class Camera
{
public:
    Camera(WGPUDevice device, Effect& effect);
    virtual ~Camera();

    WGPUBindGroup bind_group() { return m_bind_group; }

    void set_position(const glm::vec3& position);
    void set_target(const glm::vec3& target);

    void copy_to_gpu(WGPUDevice device);

private:
    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_up;
    CameraData m_camera;
    WGPUBuffer m_buffer;
    WGPUBindGroup m_bind_group;

    void update_matrix();
};

#endif
