#include "camera.h"
#include <cstring>

CameraBinding::CameraBinding(WGPUDevice device, Pipeline pipeline)
{
    std::memset(m_camera.viewproj, 0, sizeof(float[4][4]));
    m_camera.viewproj[0][0] = 2;
    m_camera.viewproj[1][1] = 0.5;
    m_camera.viewproj[2][2] = 1;
    m_camera.viewproj[3][3] = 1;

    WGPUBufferDescriptor buffer_desc = { 0 };
    buffer_desc.nextInChain = nullptr;
    buffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
    buffer_desc.size = sizeof(Camera);
    buffer_desc.mappedAtCreation = false;

    m_buffer =
        wgpuDeviceCreateBuffer(device, &buffer_desc);

    m_bind_group = pipeline.create_camera_group(m_buffer);
}

void CameraBinding::copy_to_gpu(WGPUDevice device)
{
    WGPUQueue queue = wgpuDeviceGetQueue(device);
    wgpuQueueWriteBuffer(
        queue, m_buffer, 0, &m_camera.viewproj, sizeof(float[4][4]));
}
