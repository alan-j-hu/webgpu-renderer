#ifndef FLATMESHEFFECT_H
#define FLATMESHEFFECT_H

#include "mesheffect.h"

class UniformLayout;

class FlatMeshEffect : public MeshEffect
{
public:
    FlatMeshEffect(WGPUDevice device, UniformLayout&);
    FlatMeshEffect(const FlatMeshEffect&) = delete;
    FlatMeshEffect& operator=(const FlatMeshEffect&) = delete;
    virtual ~FlatMeshEffect();

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
    WGPUShaderModule m_fragment_shader;
    WGPUBindGroupLayout m_material_layout;
    WGPUPipelineLayout m_pipeline_layout;
};

#endif
