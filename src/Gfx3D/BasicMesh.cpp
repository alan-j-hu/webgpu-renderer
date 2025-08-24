#include "noworry/Gfx3D/BasicMesh.h"
#include <utility>

BasicMesh::BasicMesh(WGPUDevice device,
                     WGPUPrimitiveTopology topology,
                     std::span<const Vertex> vertices,
                     std::span<const std::uint16_t> indices,
                     std::size_t logical_index_count)
{
    m_topology = topology;
    m_vertex_count = vertices.size();
    m_padded_index_count = indices.size();
    m_logical_index_count = logical_index_count;
    const std::uint64_t vbuffer_size = m_vertex_count * sizeof(Vertex);
    const std::uint64_t ibuffer_size =
        m_padded_index_count * sizeof(std::uint16_t);

    WGPUQueue queue = wgpuDeviceGetQueue(device);
    WGPUBufferDescriptor vbuffer_desc = { 0 };
    vbuffer_desc.nextInChain = nullptr;
    vbuffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex;
    vbuffer_desc.size = vbuffer_size;
    vbuffer_desc.mappedAtCreation = false;
    m_vertex_buffer = wgpuDeviceCreateBuffer(device, &vbuffer_desc);
    wgpuQueueWriteBuffer(queue, m_vertex_buffer, 0,
                         vertices.data(), vbuffer_size);

    WGPUBufferDescriptor ibuffer_desc = { 0 };
    ibuffer_desc.nextInChain = nullptr;
    ibuffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index;
    ibuffer_desc.size = ibuffer_size;
    ibuffer_desc.mappedAtCreation = false;
    m_index_buffer = wgpuDeviceCreateBuffer(device, &ibuffer_desc);
    wgpuQueueWriteBuffer(queue, m_index_buffer, 0,
                         indices.data(), ibuffer_size);
}

BasicMesh::BasicMesh(BasicMesh&& other)
{
    m_vertex_count = 0;
    m_padded_index_count = 0;
    m_logical_index_count = 0;
    m_vertex_buffer = nullptr;
    m_index_buffer = nullptr;

    *this = std::move(other);
}

BasicMesh& BasicMesh::operator=(BasicMesh&& other)
{
    std::swap(m_topology, other.m_topology);
    std::swap(m_vertex_count, other.m_vertex_count);
    std::swap(m_padded_index_count, other.m_padded_index_count);
    std::swap(m_logical_index_count, other.m_logical_index_count);
    std::swap(m_vertex_buffer, other.m_vertex_buffer);
    std::swap(m_index_buffer, other.m_index_buffer);
    return *this;
}

BasicMesh::~BasicMesh()
{
    if (m_vertex_buffer != nullptr) {
        wgpuBufferRelease(m_vertex_buffer);
    }
    if (m_index_buffer != nullptr) {
        wgpuBufferRelease(m_index_buffer);
    }
}
