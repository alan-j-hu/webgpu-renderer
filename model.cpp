#include "model.h"
#include "mesh.h"
#include <cstring>
#include <glm/ext/matrix_transform.hpp>

Model::Model(WGPUDevice device, Pipeline& pipeline, const Mesh& mesh)
    : m_mesh(mesh)
{
    m_model.transform = glm::identity<glm::mat4>();

    WGPUBufferDescriptor buffer_desc = { 0 };
    buffer_desc.nextInChain = nullptr;
    buffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
    buffer_desc.size = sizeof(Model);
    buffer_desc.mappedAtCreation = false;
    m_buffer = wgpuDeviceCreateBuffer(device, &buffer_desc);

    m_bind_group = pipeline.create_model_group(m_buffer);
}

Model::~Model()
{
    wgpuBindGroupRelease(m_bind_group);
    wgpuBufferRelease(m_buffer);
}

void Model::copy_to_gpu(WGPUDevice device)
{
    WGPUQueue queue = wgpuDeviceGetQueue(device);
    wgpuQueueWriteBuffer(
        queue, m_buffer, 0, &m_model.transform, sizeof(float[4][4]));
}
