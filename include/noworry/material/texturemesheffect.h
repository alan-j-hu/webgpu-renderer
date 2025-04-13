#ifndef TEXTUREMESHEFFECT_H
#define TEXTUREMESHEFFECT_H

#include "mesheffect.h"

class UniformLayout;

class TextureMeshEffect : public MeshEffect
{
public:
    TextureMeshEffect(WGPUDevice device, UniformLayout& ul);
    TextureMeshEffect(const TextureMeshEffect&) = delete;
    TextureMeshEffect& operator=(const TextureMeshEffect&) = delete;
    virtual ~TextureMeshEffect();

    virtual WGPUShaderModule vertex_shader() override
    {
        return m_vertex_shader;
    }

    virtual WGPUShaderModule fragment_shader() override
    {
        return m_fragment_shader;
    }

    virtual WGPUPipelineLayout pipeline_layout() override
    {
        return m_pipeline_layout;
    }

    virtual WGPUBindGroupLayout material_layout() override
    {
        return m_material_layout;
    }

    virtual WGPUBindGroup create_material_group(
        WGPUDevice, WGPUTextureView, WGPUSampler);
private:

    WGPUShaderModule m_vertex_shader;
    WGPUShaderModule m_fragment_shader;
    WGPUBindGroupLayout m_material_layout;
    WGPUPipelineLayout m_pipeline_layout;
};

#endif
