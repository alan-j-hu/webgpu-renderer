#ifndef MESH_H
#define MESH_H

#include "material/material.h"
#include "material/mesheffect.h"
#include <cstddef>
#include <webgpu/webgpu.h>

struct Mesh
{
public:
    Mesh(WGPUDevice device,
         Vertex* vertices, std::size_t vertex_count,
         std::uint16_t* indices, std::size_t index_count);
    Mesh(const Mesh&) = delete;
    Mesh(Mesh&& other);
    Mesh& operator=(const Mesh&) = delete;
    Mesh& operator=(Mesh&& other);

    ~Mesh();

    WGPUBuffer vertex_buffer() const { return m_vertex_buffer; }
    WGPUBuffer index_buffer() const { return m_index_buffer; }
    std::size_t vertex_count() const { return m_vertex_count; }
    std::size_t index_count() const { return m_index_count; }

private:
    WGPUBuffer m_vertex_buffer;
    WGPUBuffer m_index_buffer;
    std::size_t m_vertex_count;
    std::size_t m_index_count;
};

#endif
