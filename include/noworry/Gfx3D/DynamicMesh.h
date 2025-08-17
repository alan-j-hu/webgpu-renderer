#ifndef DYNAMIC_MESH_H
#define DYNAMIC_MESH_H

#include "Mesh.h"
#include "../Pipeline/MeshVertexShader.h"
#include <cstddef>
#include <vector>

class DynamicMesh : public Mesh
{
public:
    DynamicMesh();
    DynamicMesh(DynamicMesh&& other);
    DynamicMesh& operator=(DynamicMesh&& other);

    virtual ~DynamicMesh();

    virtual WGPUBuffer vertex_buffer() const override
    { return m_vertex_buffer; }

    virtual WGPUBuffer index_buffer() const override
    { return m_index_buffer; }

    virtual std::size_t vertex_count() const override
    { return m_vertices.size(); }

    virtual std::size_t index_count() const override
    { return m_indices.size(); }

    virtual WGPUPrimitiveTopology topology() const override
    { return m_topology; }

    void clear();

    void append(const Vertex*,
                std::size_t,
                const std::uint16_t*,
                std::size_t);

    void flush(WGPUDevice device);

private:
    std::vector<Vertex> m_vertices;
    std::vector<std::uint16_t> m_indices;

    WGPUBuffer m_vertex_buffer;
    WGPUBuffer m_index_buffer;

    WGPUPrimitiveTopology m_topology;

    void resize_vertex(WGPUDevice device);
    void resize_index(WGPUDevice device);
    void queue_write(WGPUDevice device);
};

#endif
