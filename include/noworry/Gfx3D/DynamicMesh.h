#ifndef DYNAMIC_MESH_H
#define DYNAMIC_MESH_H

#include "Mesh.h"
#include "ModelData.h"
#include "../Pipeline/MeshVertexShader.h"
#include <cstddef>
#include <span>
#include <vector>

class DynamicMesh : public Mesh
{
public:
    DynamicMesh();
    DynamicMesh(DynamicMesh&& other);
    DynamicMesh& operator=(DynamicMesh&& other);

    virtual ~DynamicMesh();

    WGPUBuffer vertex_buffer() const override;

    virtual WGPUBuffer index_buffer() const override;

    virtual std::size_t vertex_count() const override;

    virtual std::size_t index_count() const override;

    virtual WGPUPrimitiveTopology topology() const override;

    void clear();

    void append(std::span<const Vertex>, std::span<const std::uint16_t>);

    void flush(WGPUDevice device);

private:
    ModelData::Part m_model_part;
    WGPUBuffer m_vertex_buffer;
    WGPUBuffer m_index_buffer;
    WGPUPrimitiveTopology m_topology;

    void resize_vertex_buffer(WGPUDevice device);
    void resize_index_buffer(WGPUDevice device);
    void queue_write(WGPUDevice device);
};

#endif
