#include "noworry/Draw3D/ModelGroup.h"

#include "noworry/renderer.h"
#include "noworry/transform.h"

ModelGroup::ModelGroup(Renderer& renderer)
{
    WGPUBufferDescriptor buffer_desc = { 0 };
    buffer_desc.nextInChain = nullptr;
    buffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
    buffer_desc.size = sizeof(ModelUniforms);
    buffer_desc.mappedAtCreation = false;
    m_buffer = wgpuDeviceCreateBuffer(renderer.device(), &buffer_desc);

    m_bind_group =
        renderer.mesh_vertex_shader().create_model_group(renderer.device(),
                                                         m_buffer);
}

ModelGroup::ModelGroup(ModelGroup&& other)
{
    m_buffer = other.m_buffer;
    m_bind_group = other.m_bind_group;
    other.m_buffer = nullptr;
    other.m_bind_group = nullptr;
}

ModelGroup& ModelGroup::operator=(ModelGroup&& other)
{
    if (m_buffer != nullptr) {
        wgpuBufferRelease(m_buffer);
    }
    if (m_bind_group != nullptr) {
        wgpuBindGroupRelease(m_bind_group);
    }

    m_buffer = other.m_buffer;
    m_bind_group = other.m_bind_group;
    other.m_buffer = nullptr;
    other.m_bind_group = nullptr;

    return *this;
}

ModelGroup::~ModelGroup()
{
    if (m_buffer != nullptr) {
        wgpuBufferRelease(m_buffer);
    }
    if (m_bind_group != nullptr) {
        wgpuBindGroupRelease(m_bind_group);
    }
}

void ModelGroup::copy(Renderer& renderer, Transform& transform)
{
    ModelUniforms uniforms {};
    transform.update_matrix(uniforms);

    WGPUQueue queue = wgpuDeviceGetQueue(renderer.device());
    wgpuQueueWriteBuffer(queue, m_buffer, 0, &uniforms, sizeof(float[16]));
}
