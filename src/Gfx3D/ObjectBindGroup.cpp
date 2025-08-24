#include "noworry/Gfx3D/ObjectBindGroup.h"

#include "noworry/renderer.h"
#include "noworry/transform.h"
#include <utility>

ObjectBindGroup::ObjectBindGroup(Renderer& renderer)
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

ObjectBindGroup::ObjectBindGroup(ObjectBindGroup&& other)
{
    m_buffer = nullptr;
    m_bind_group = nullptr;
    *this = std::move(other);
}

ObjectBindGroup& ObjectBindGroup::operator=(ObjectBindGroup&& other)
{
    std::swap(m_buffer, other.m_buffer);
    std::swap(m_bind_group, other.m_bind_group);

    return *this;
}

ObjectBindGroup::~ObjectBindGroup()
{
    if (m_buffer != nullptr) {
        wgpuBufferRelease(m_buffer);
    }
    if (m_bind_group != nullptr) {
        wgpuBindGroupRelease(m_bind_group);
    }
}

void ObjectBindGroup::copy(Renderer& renderer, const glm::mat4& transform)
{
    WGPUQueue queue = wgpuDeviceGetQueue(renderer.device());
    wgpuQueueWriteBuffer(queue, m_buffer, 0, &transform, sizeof(float[16]));
}

ObjectBindGroupPool::ObjectBindGroupPool(Renderer& renderer)
{
    m_renderer = &renderer;
    m_index = 0;
}

ObjectBindGroup* ObjectBindGroupPool::alloc()
{
    if (m_index >= m_pool.size()) {
        m_pool.push_back(std::make_unique<ObjectBindGroup>(*m_renderer));
    }

    return m_pool[m_index++].get();
}

void ObjectBindGroupPool::reset()
{
    m_index = 0;
}
