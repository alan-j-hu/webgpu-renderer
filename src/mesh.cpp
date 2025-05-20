#include "noworry/mesh.h"

Mesh::Mesh(WGPUDevice device,
    Vertex* vertices, std::size_t vertex_count,
    const std::uint16_t* indices, std::size_t index_count)
{
    m_vertex_count = vertex_count;
    m_index_count = index_count;
    const std::uint64_t vbuffer_size = vertex_count * sizeof(Vertex);
    const std::uint64_t ibuffer_size = index_count * sizeof(std::uint16_t);

    WGPUQueue queue = wgpuDeviceGetQueue(device);
    WGPUBufferDescriptor vbuffer_desc = { 0 };
    vbuffer_desc.nextInChain = nullptr;
    vbuffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Storage;
    vbuffer_desc.size = vbuffer_size;
    vbuffer_desc.mappedAtCreation = false;
    m_vertex_buffer = wgpuDeviceCreateBuffer(device, &vbuffer_desc);
    wgpuQueueWriteBuffer(queue, m_vertex_buffer, 0, vertices, vbuffer_size);

    WGPUBufferDescriptor ibuffer_desc = { 0 };
    ibuffer_desc.nextInChain = nullptr;
    ibuffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index;
    ibuffer_desc.size = ibuffer_size;
    ibuffer_desc.mappedAtCreation = false;
    m_index_buffer = wgpuDeviceCreateBuffer(device, &ibuffer_desc);
    wgpuQueueWriteBuffer(queue, m_index_buffer, 0, indices, ibuffer_size);
}

Mesh::Mesh(Mesh&& other)
{
    m_vertex_count = other.m_vertex_count;
    m_index_count = other.m_index_count;
    m_vertex_buffer = other.m_vertex_buffer;
    m_index_buffer = other.m_index_buffer;

    other.m_vertex_count = 0;
    other.m_index_count = 0;
    other.m_vertex_buffer = nullptr;
    other.m_index_buffer = nullptr;
}

Mesh& Mesh::operator=(Mesh&& other)
{
    wgpuBufferRelease(m_vertex_buffer);
    wgpuBufferRelease(m_index_buffer);

    m_vertex_count = other.m_vertex_count;
    m_index_count = other.m_index_count;
    m_vertex_buffer = other.m_vertex_buffer;
    m_index_buffer = other.m_index_buffer;

    other.m_vertex_count = 0;
    other.m_index_count = 0;
    other.m_vertex_buffer = nullptr;
    other.m_index_buffer = nullptr;

    return *this;
}

Mesh::~Mesh()
{
    wgpuBufferRelease(m_vertex_buffer);
    wgpuBufferRelease(m_index_buffer);
}
