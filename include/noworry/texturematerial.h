#ifndef TEXTUREMATERIAL_H
#define TEXTUREMATERIAL_H

#include "material.h"
#include "texture.h"
#include "texturemeshpipeline.h"
#include <webgpu/webgpu.h>

class TextureMaterial : public Material
{
public:
    TextureMaterial(
        int id,
        WGPUDevice device,
        TextureMeshPipeline&,
        const Texture&,
        WGPUSampler sampler);

    int id() { return m_id; }
    virtual WGPUBindGroup bind_group() override { return m_bind_group; }
    virtual TextureMeshPipeline& pipeline() override { return *m_pipeline; }
private:
    int m_id;
    TextureMeshPipeline* m_pipeline;
    const Texture* m_texture;
    WGPUBindGroup m_bind_group;
};

#endif
