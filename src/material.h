#ifndef MATERIAL_H
#define MATERIAL_H

#include "pipeline.h"
#include "texture.h"
#include <webgpu/webgpu.h>

class Material
{
public:
    Material(const Pipeline&, const Texture&, WGPUSampler sampler);

    WGPUBindGroup bind_group() { return m_bind_group; }
private:
    const Texture* m_texture;
    WGPUBindGroup m_bind_group;
};

#endif
