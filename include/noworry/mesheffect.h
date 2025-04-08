#ifndef MESH_EFFECT_H
#define MESH_EFFECT_H

#include <glm/mat4x4.hpp>
#include <webgpu/webgpu.h>

struct Vertex
{
    float x, y, z;
    float u, v;
};

struct ModelData
{
    glm::mat4 transform;
};

class MeshEffect
{
public:
    MeshEffect(WGPUDevice device);
    MeshEffect(const MeshEffect&) = delete;
    MeshEffect& operator=(const MeshEffect&) = delete;
    virtual ~MeshEffect();

    virtual WGPUShaderModule vertex_shader() = 0;
    virtual WGPUShaderModule fragment_shader() = 0;
    virtual WGPUPipelineLayout pipeline_layout() = 0;

    virtual WGPUBindGroupLayout material_layout() = 0;
    WGPUBindGroupLayout model_layout() { return m_model_layout; }

    WGPUBindGroup create_model_group(
        WGPUDevice,
        WGPUBuffer transform,
        WGPUBuffer vertices,
        int tri_count);
private:

    WGPUBindGroupLayout m_model_layout;
};

#endif
