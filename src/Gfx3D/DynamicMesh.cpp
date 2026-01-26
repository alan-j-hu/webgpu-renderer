#include "noworry/Gfx3D/DynamicMesh.h"
#include <ranges>
#include <utility>

DynamicMesh::DynamicMesh()
    : m_vertex_buffer {nullptr},
      m_index_buffer {nullptr},
      m_topology {WGPUPrimitiveTopology_TriangleList}
{
}

DynamicMesh::DynamicMesh(std::shared_ptr<Material> material)
    : m_vertex_buffer {nullptr},
      m_index_buffer {nullptr},
      m_topology {WGPUPrimitiveTopology_TriangleList},
      m_model_part(std::move(material))
{
}

DynamicMesh::DynamicMesh(DynamicMesh&& other)
    : m_model_part(std::move(other.m_model_part)),
      m_vertex_buffer {nullptr},
      m_index_buffer {nullptr},
      m_topology {WGPUPrimitiveTopology_TriangleList}
{
}

DynamicMesh& DynamicMesh::operator=(DynamicMesh&& other)
{
    if (m_vertex_buffer != nullptr) {
        wgpuBufferRelease(m_vertex_buffer);
    }
    if (m_index_buffer != nullptr) {
        wgpuBufferRelease(m_index_buffer);
    }

    m_model_part = std::move(other.m_model_part);
    m_vertex_buffer = std::exchange(other.m_vertex_buffer, nullptr);
    m_index_buffer = std::exchange(other.m_index_buffer, nullptr);
    m_topology = other.m_topology;

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

WGPUBuffer DynamicMesh::vertex_buffer() const
{
    return m_vertex_buffer;
}

WGPUBuffer DynamicMesh::index_buffer() const
{
    return m_index_buffer;
}

std::size_t DynamicMesh::vertex_count() const
{
    return m_model_part.vertices().size();
}

std::size_t DynamicMesh::index_count() const
{
    return m_model_part.indices().size();
}

WGPUPrimitiveTopology DynamicMesh::topology() const
{
    return m_topology;
}

const ModelData::Part& DynamicMesh::mesh_data() const
{
    return m_model_part;
}

void DynamicMesh::clear()
{
    m_model_part.clear();
}

void DynamicMesh::append(std::span<const Vertex> vertices,
                         std::span<const std::uint16_t> indices)
{
    m_model_part.append(vertices, indices);
}

void DynamicMesh::flush(WGPUDevice device)
{
    resize_vertex_buffer(device);
    resize_index_buffer(device);
    queue_write(device);
}

void DynamicMesh::resize_vertex_buffer(WGPUDevice device)
{
    std::size_t byte_size = sizeof(Vertex) * m_model_part.vertices().size();
    if (m_vertex_buffer != nullptr) {
        if (wgpuBufferGetSize(m_vertex_buffer) >= byte_size) {
            return;
        }

        wgpuBufferRelease(m_vertex_buffer);
    }

    std::size_t new_size = 2 * byte_size;

    WGPUBufferDescriptor vbuffer_desc = { 0 };
    vbuffer_desc.nextInChain = nullptr;
    vbuffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex;
    vbuffer_desc.size = new_size;
    vbuffer_desc.mappedAtCreation = false;
    m_vertex_buffer = wgpuDeviceCreateBuffer(device, &vbuffer_desc);
}

void DynamicMesh::resize_index_buffer(WGPUDevice device)
{
    std::size_t byte_size =
        sizeof(std::uint16_t) * m_model_part.indices_with_padding().size();
    if (m_index_buffer != nullptr) {
        if (wgpuBufferGetSize(m_index_buffer) >= byte_size) {
            return;
        }

        wgpuBufferRelease(m_index_buffer);
    }

    std::size_t new_size = 2 * byte_size;

    WGPUBufferDescriptor ibuffer_desc = { 0 };
    ibuffer_desc.nextInChain = nullptr;
    ibuffer_desc.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index;
    ibuffer_desc.size = new_size;
    ibuffer_desc.mappedAtCreation = false;
    m_index_buffer = wgpuDeviceCreateBuffer(device, &ibuffer_desc);
}

void DynamicMesh::queue_write(WGPUDevice device)
{
    WGPUQueue queue = wgpuDeviceGetQueue(device);
    auto vertices = m_model_part.vertices();
    auto indices = m_model_part.indices_with_padding();

    wgpuQueueWriteBuffer(
        queue,
        m_vertex_buffer,
        0,
        vertices.data(),
        sizeof(Vertex) * vertices.size());
    wgpuQueueWriteBuffer(
        queue,
        m_index_buffer,
        0,
        indices.data(),
        sizeof(std::uint16_t) * indices.size());
}
