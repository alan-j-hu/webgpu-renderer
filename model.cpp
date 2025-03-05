#include "model.h"
#include <cstring>

ModelBinding::ModelBinding(WGPUDevice device, Pipeline pipeline)
{
    std::memset(m_model.transform, 0, sizeof(float[4][4]));
    m_model.transform[0][0] = 0.5;
    m_model.transform[1][1] = 0.5;
    m_model.transform[2][2] = 1;
    m_model.transform[3][3] = 1;

    WGPUBufferDescriptor buffer_desc = { 0 };
    buffer_desc.nextInChain = nullptr;
    buffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
    buffer_desc.size = sizeof(Model);
    buffer_desc.mappedAtCreation = false;

    m_buffer = wgpuDeviceCreateBuffer(device, &buffer_desc);

    m_bind_group = pipeline.create_model_group(m_buffer);
}

void ModelBinding::copy_to_gpu(WGPUDevice device)
{
    WGPUQueue queue = wgpuDeviceGetQueue(device);
    wgpuQueueWriteBuffer(
        queue, m_buffer, 0, &m_model.transform, sizeof(float[4][4]));
}
