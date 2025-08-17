#ifndef BASIC_MESH_H
#define BASIC_MESH_H

#include "Mesh.h"
#include "../Pipeline/MeshVertexShader.h"
#include <cstddef>
#include <webgpu/webgpu.h>

/// A mesh consists of a vertex buffer and an index buffer in GPU memory.
class BasicMesh : public Mesh
{
public:
    /// @param device              - the device
    /// @param topology            - the interpretation of the vertex order
    /// @param vertices            - the vertex buffer
    /// @param vertex_count        - the number of vertices
    /// @param indices             - the index buffer
    /// @param padded_index_count  - the padded length of the index buffer
    /// @param logical_index_count - the number of indices used by the mesh
    BasicMesh(WGPUDevice device,
              WGPUPrimitiveTopology topology,
              Vertex* vertices, std::size_t vertex_count,
              const std::uint16_t* indices, std::size_t padded_index_count,
              std::size_t logical_index_count);
    BasicMesh(const BasicMesh&) = delete;
    BasicMesh(BasicMesh&& other);
    BasicMesh& operator=(const BasicMesh&) = delete;
    BasicMesh& operator=(BasicMesh&& other);

    virtual ~BasicMesh();

    virtual WGPUBuffer vertex_buffer() const override
    { return m_vertex_buffer; }

    virtual WGPUBuffer index_buffer() const override
    { return m_index_buffer; }

    virtual std::size_t vertex_count() const override
    { return m_vertex_count; }

    virtual std::size_t index_count() const override
    { return m_logical_index_count; }

    virtual WGPUPrimitiveTopology topology() const override
    { return m_topology; }

private:
    WGPUBuffer m_vertex_buffer;
    WGPUBuffer m_index_buffer;
    std::size_t m_vertex_count;
    std::size_t m_padded_index_count;
    std::size_t m_logical_index_count;
    WGPUPrimitiveTopology m_topology;
};

#endif
