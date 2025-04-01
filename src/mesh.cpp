#include "noworry/mesh.h"

Mesh::Mesh(WGPUDevice device,
    Vertex* vertices, std::size_t vertex_count,
    std::uint16_t* indices, std::size_t tri_count)
{
    this->vertex_count = vertex_count;
    this->tri_count = tri_count;
    const std::uint64_t vbuffer_size = vertex_count * sizeof(Vertex);
    const std::uint64_t ibuffer_size = 3 * tri_count * sizeof(std::uint16_t);

    WGPUQueue queue = wgpuDeviceGetQueue(device);
    WGPUBufferDescriptor vbuffer_desc = { 0 };
    vbuffer_desc.nextInChain = nullptr;
    vbuffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Storage;
    vbuffer_desc.size = vbuffer_size;
    vbuffer_desc.mappedAtCreation = false;
    vertex_buffer = wgpuDeviceCreateBuffer(device, &vbuffer_desc);
    wgpuQueueWriteBuffer(queue, vertex_buffer, 0, vertices, vbuffer_size);

    WGPUBufferDescriptor ibuffer_desc = { 0 };
    ibuffer_desc.nextInChain = nullptr;
    ibuffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index;
    ibuffer_desc.size = ibuffer_size;
    ibuffer_desc.mappedAtCreation = false;
    index_buffer = wgpuDeviceCreateBuffer(device, &ibuffer_desc);
    wgpuQueueWriteBuffer(queue, index_buffer, 0, indices, ibuffer_size);
}

Mesh::~Mesh()
{
    wgpuBufferRelease(vertex_buffer);
    wgpuBufferRelease(index_buffer);
}
