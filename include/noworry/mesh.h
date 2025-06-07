#ifndef MESH_H
#define MESH_H

#include "layout.h"
#include "material/material.h"
#include <cstddef>
#include <webgpu/webgpu.h>

/// A mesh consists of a vertex buffer and an index buffer in GPU memory.
struct Mesh
{
public:
    /// @param device              - the device
    /// @param vertices            - the vertex buffer
    /// @param vertex_count        - the number of vertices
    /// @param indices             - the index buffer
    /// @param padded_index_count  - the padded length of the index buffer
    /// @param logical_index_count - the number of indices used by the mesh
    Mesh(WGPUDevice device,
         Vertex* vertices, std::size_t vertex_count,
         const std::uint16_t* indices, std::size_t padded_index_count,
         std::size_t logical_index_count);
    Mesh(const Mesh&) = delete;
    Mesh(Mesh&& other);
    Mesh& operator=(const Mesh&) = delete;
    Mesh& operator=(Mesh&& other);

    ~Mesh();

    WGPUBuffer vertex_buffer() const { return m_vertex_buffer; }
    WGPUBuffer index_buffer() const { return m_index_buffer; }
    std::size_t vertex_count() const { return m_vertex_count; }
    std::size_t index_count() const { return m_logical_index_count; }

private:
    WGPUBuffer m_vertex_buffer;
    WGPUBuffer m_index_buffer;
    std::size_t m_vertex_count;
    std::size_t m_padded_index_count;
    std::size_t m_logical_index_count;
};

#endif
