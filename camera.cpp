#include "camera.h"
#include <cstring>
#include <glm/ext/matrix_transform.hpp>

Camera::Camera(WGPUDevice device, Pipeline& pipeline)
{
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
    WGPUQueue queue = wgpuDeviceGetQueue(device);
    wgpuQueueWriteBuffer(
        queue, m_buffer, 0, &m_camera.viewproj, sizeof(float[4][4]));
}
