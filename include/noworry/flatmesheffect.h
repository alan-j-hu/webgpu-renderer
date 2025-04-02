#ifndef FLATMESHEFFECT_H
#define FLATMESHEFFECT_H

#include "noworry/mesheffect.h"

class FlatMeshEffect : public MeshEffect
{
public:
    FlatMeshEffect(WGPUDevice device);
    FlatMeshEffect(const FlatMeshEffect&) = delete;
    FlatMeshEffect& operator=(const FlatMeshEffect&) = delete;
    virtual ~FlatMeshEffect();

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

    WGPUBindGroup create_material_group(WGPUDevice, WGPUBuffer);
private:

    WGPUShaderModule m_vertex_shader;
    WGPUShaderModule m_fragment_shader;
    WGPUBindGroupLayout m_material_layout;
    WGPUPipelineLayout m_pipeline_layout;
};

#endif
