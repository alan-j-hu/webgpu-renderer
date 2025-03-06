#ifndef CAMERA_H
#define CAMERA_H

#include "pipeline.h"
#include <glm/vec3.hpp>

/* The camera uses a left-handed coordinate system with Y-up. */
class Camera
{
public:
    Camera(WGPUDevice device, Pipeline& pipeline);
    virtual ~Camera();

    void set_position(const glm::vec3& position);
    void set_target(const glm::vec3& target);

    void copy_to_gpu(WGPUDevice device);

    CameraData m_camera;
    WGPUBindGroup m_bind_group;
    WGPUBuffer m_buffer;

private:
    glm::vec3 m_position;
    glm::vec3 m_target;
    glm::vec3 m_up;

    void update_matrix();
};

#endif
