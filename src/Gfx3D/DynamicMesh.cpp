#include "noworry/Gfx3D/DynamicMesh.h"
#include <ranges>
#include <utility>

DynamicMesh::DynamicMesh()
    : m_vertex_buffer {nullptr},
      m_index_buffer {nullptr},
      m_topology {WGPUPrimitiveTopology_TriangleList}
{
}

DynamicMesh::DynamicMesh(DynamicMesh&& other)
    : m_vertex_buffer {nullptr},
      m_index_buffer {nullptr},
      m_topology {WGPUPrimitiveTopology_TriangleList}
{
    *this = std::move(other);
}

DynamicMesh& DynamicMesh::operator=(DynamicMesh&& other)
{
    std::swap(m_vertices, other.m_vertices);
    std::swap(m_indices, other.m_indices);
    std::swap(m_vertex_buffer, other.m_vertex_buffer);
    std::swap(m_index_buffer, other.m_index_buffer);
    std::swap(m_topology, other.m_topology);

    return *this;
}

DynamicMesh::~DynamicMesh()
{
    if (m_vertex_buffer != nullptr) {
        wgpuBufferRelease(m_vertex_buffer);
    }
    if (m_index_buffer != nullptr) {
        wgpuBufferRelease(m_index_buffer);
    }
}

void DynamicMesh::clear()
{
    m_vertices.clear();
    m_indices.clear();
}

void DynamicMesh::append(std::span<const Vertex> vertices,
                         std::span<const std::uint16_t> indices)
{
    std::uint16_t index_offset = m_vertices.size();
    m_vertices.insert(m_vertices.end(),
                      vertices.begin(), vertices.end());

    for (auto index : indices) {
        m_indices.push_back(index + index_offset);
    }
}

void DynamicMesh::flush(WGPUDevice device)
{
    resize_vertex(device);
    resize_index(device);
    queue_write(device);
}

void DynamicMesh::resize_vertex(WGPUDevice device)
{
    if (m_vertex_buffer != nullptr) {
        std::size_t vertex_bytesize = sizeof(Vertex) * m_vertices.size();
        if (wgpuBufferGetSize(m_vertex_buffer) >= vertex_bytesize) {
            return;
        }

        wgpuBufferRelease(m_vertex_buffer);
    }

    std::size_t vertex_capacity = m_vertices.capacity();

    WGPUBufferDescriptor vbuffer_desc = { 0 };
    vbuffer_desc.nextInChain = nullptr;
    vbuffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex;
    vbuffer_desc.size = sizeof(Vertex) * vertex_capacity;
    vbuffer_desc.mappedAtCreation = false;
    m_vertex_buffer = wgpuDeviceCreateBuffer(device, &vbuffer_desc);
}

void DynamicMesh::resize_index(WGPUDevice device)
{
    if (m_index_buffer != nullptr) {
        std::size_t index_bytesize = sizeof(std::uint16_t) * m_indices.size();
        if (wgpuBufferGetSize(m_index_buffer) >= index_bytesize) {
            return;
        }

        wgpuBufferRelease(m_index_buffer);
    }

    std::size_t index_capacity = m_indices.capacity();
    if (index_capacity % 2 == 1) {
        index_capacity *= 2;
        m_indices.reserve(index_capacity);
    }

    WGPUBufferDescriptor ibuffer_desc = { 0 };
    ibuffer_desc.nextInChain = nullptr;
    ibuffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index;
    ibuffer_desc.size = sizeof(std::uint16_t) * index_capacity;
    ibuffer_desc.mappedAtCreation = false;
    m_index_buffer = wgpuDeviceCreateBuffer(device, &ibuffer_desc);
}

void DynamicMesh::queue_write(WGPUDevice device)
{
    WGPUQueue queue = wgpuDeviceGetQueue(device);
    wgpuQueueWriteBuffer(queue,
                         m_vertex_buffer,
                         0,
                         m_vertices.data(),
                         sizeof(Vertex) * m_vertices.size());
    wgpuQueueWriteBuffer(queue,
                         m_index_buffer,
                         0,
                         m_indices.data(),
                         sizeof(std::uint16_t) * m_indices.size());
}
