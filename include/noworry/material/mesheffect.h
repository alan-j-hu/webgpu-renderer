#ifndef MESH_EFFECT_H
#define MESH_EFFECT_H

#include <vector>
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

struct Model;
struct UniformLayout;

class MeshEffect
{
public:
    MeshEffect(WGPUDevice device, UniformLayout&);
    MeshEffect(const MeshEffect&) = delete;
    MeshEffect& operator=(const MeshEffect&) = delete;
    virtual ~MeshEffect();

    WGPUShaderModule vertex_shader()
    {
        return m_vertex_shader;
    }

    virtual WGPUShaderModule fragment_shader() = 0;
    virtual WGPUBindGroupLayout material_layout() = 0;
    WGPUBindGroupLayout model_layout() { return m_model_layout; }

    WGPUBindGroup create_model_group(
        WGPUDevice,
        WGPUBuffer transform,
        WGPUBuffer vertices,
        int tri_count);

    void enqueue(Model& model);

    void draw(WGPURenderPassEncoder encoder);

    virtual WGPUPrimitiveTopology topology() = 0;
    WGPUPipelineLayout pipeline_layout();
    WGPURenderPipeline pipeline();

private:
    WGPUDevice m_device;
    WGPUShaderModule m_vertex_shader;
    WGPUBindGroupLayout m_model_layout;
    UniformLayout* m_ul;

    WGPUPipelineLayout m_pipeline_layout;
    WGPURenderPipeline m_pipeline;
    std::vector<Model*> m_queue;

    void create_pipeline();
};

#endif
