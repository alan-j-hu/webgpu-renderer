#include "noworry/Material/FlatMaterial.h"
#include <glm/vec3.hpp>

FlatMaterial::FlatMaterial(
    int id,
    WGPUDevice device,
    FlatEffect& effect,
    float r, float g, float b)
    : m_color(r, g, b, 1)
{
    WGPUBufferDescriptor buffer_desc = { 0 };
    buffer_desc.nextInChain = nullptr;
    buffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform;
    buffer_desc.size = sizeof(glm::vec3);
    buffer_desc.mappedAtCreation = false;
    m_buffer = wgpuDeviceCreateBuffer(device, &buffer_desc);

    glm::vec3 color(r, g, b);
    WGPUQueue queue = wgpuDeviceGetQueue(device);
    wgpuQueueWriteBuffer(
        queue, m_buffer, 0, &color, sizeof(glm::vec3));

    m_id = id;
    m_effect = &effect;
    m_bind_group = effect
        .create_material_group(device, m_buffer);
}

int FlatMaterial::id() const
{
    return m_id;
}

WGPUBindGroup FlatMaterial::bind_group() const
{
    return m_bind_group;
}

FlatEffect& FlatMaterial::effect() const
{
    return *m_effect;
}

glm::vec4 FlatMaterial::diffuse_factor() const
{
    return m_color;
}
