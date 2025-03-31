#include "noworry/mesh.h"

Mesh::Mesh(WGPUDevice device, Vertex* vertices, std::size_t count)
    : tri_count(count / 3)
{
    WGPUBufferDescriptor buffer_desc = { 0 };
    buffer_desc.nextInChain = nullptr;
    buffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Storage;
    buffer_desc.size = count * sizeof(Vertex);
    buffer_desc.mappedAtCreation = false;

    vertex_buffer = wgpuDeviceCreateBuffer(device, &buffer_desc);

    WGPUQueue queue = wgpuDeviceGetQueue(device);
    wgpuQueueWriteBuffer(
        queue, vertex_buffer, 0, vertices, sizeof(Vertex) * count);
}

Mesh::~Mesh()
{
    wgpuBufferRelease(vertex_buffer);
}
