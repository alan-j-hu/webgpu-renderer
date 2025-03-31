#ifndef MATERIAL_H
#define MATERIAL_H

#include "pipeline.h"
#include "texture.h"
#include <webgpu/webgpu.h>

class Material
{
public:
    Material(
        int id,
        WGPUDevice device,
        Pipeline&,
        const Texture&,
        WGPUSampler sampler);

    int id() { return m_id; }
    WGPUBindGroup bind_group() { return m_bind_group; }
    Pipeline& pipeline() { return *m_pipeline; }
private:
    int m_id;
    Pipeline* m_pipeline;
    const Texture* m_texture;
    WGPUBindGroup m_bind_group;
};

#endif
