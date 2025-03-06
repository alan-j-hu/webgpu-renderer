#include "camera.h"
#include <cstring>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

Camera::Camera(WGPUDevice device, Pipeline& pipeline)
{
    m_position = glm::vec3(0.0f, 0.0f, 1.0f);
    m_target = glm::vec3(0.0f, 0.0f, 0.0f);
    m_up = glm::vec3(0.0f, 1.0f, 0.0f);

    m_camera.viewproj = glm::identity<glm::mat4>();

    WGPUBufferDescriptor buffer_desc = { 0 };
    buffer_desc.nextInChain = nullptr;
    buffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
    buffer_desc.size = sizeof(Camera);
    buffer_desc.mappedAtCreation = false;
    m_buffer = wgpuDeviceCreateBuffer(device, &buffer_desc);

    m_bind_group = pipeline.create_camera_group(m_buffer);
}

Camera::~Camera()
{
    wgpuBindGroupRelease(m_bind_group);
    wgpuBufferRelease(m_buffer);
}

void Camera::copy_to_gpu(WGPUDevice device)
{
    update_matrix();
    WGPUQueue queue = wgpuDeviceGetQueue(device);
    wgpuQueueWriteBuffer(
        queue, m_buffer, 0, &m_camera.viewproj, sizeof(float[4][4]));
}

void Camera::update_matrix()
{
    glm::mat4 proj = glm::perspective(
        glm::pi<float>() * 0.5f,
        4.0f / 3.0f,
        0.1f,
        100.f);
    glm::mat4 view = glm::lookAtLH(m_position, m_target, m_up);
    m_camera.viewproj = proj * view;
}
