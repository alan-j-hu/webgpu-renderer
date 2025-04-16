#include "noworry/material/flatmaterial.h"
#include <glm/vec3.hpp>

FlatMaterial::FlatMaterial(
    int id,
    WGPUDevice device,
    FlatMeshEffect& effect,
    float r, float g, float b)
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

