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

    virtual WGPUShaderModule fragment_shader() override
    {
        return m_fragment_shader;
    }

    virtual WGPUBindGroupLayout material_layout() override
    {
        return m_material_layout;
    }

    virtual WGPUPrimitiveTopology topology() override
    {
        return WGPUPrimitiveTopology_TriangleList;
    }

    WGPUBindGroup create_material_group(
        WGPUDevice, WGPUTextureView, WGPUSampler);

private:
    WGPUShaderModule m_fragment_shader;
    WGPUBindGroupLayout m_material_layout;
};

#endif
