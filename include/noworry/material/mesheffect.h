#ifndef MESH_EFFECT_H
#define MESH_EFFECT_H

#include <vector>
#include <webgpu/webgpu.h>

struct Renderer;
struct RenderObject;
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

    void enqueue(RenderObject& model);

    void draw(WGPURenderPassEncoder encoder);

    virtual WGPUPrimitiveTopology topology() = 0;
    WGPUPipelineLayout pipeline_layout();
    WGPURenderPipeline pipeline();

private:
    WGPUDevice m_device;
    WGPUShaderModule m_vertex_shader;
    UniformLayout* m_ul;

    WGPUPipelineLayout m_pipeline_layout;
    WGPURenderPipeline m_pipeline;
    std::vector<RenderObject*> m_queue;

    void create_pipeline();
};

#endif
