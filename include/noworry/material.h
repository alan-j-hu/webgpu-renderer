#ifndef MATERIAL_H
#define MATERIAL_H

#include "effect.h"
#include "texture.h"
#include <webgpu/webgpu.h>

class Material
{
public:
    Material(WGPUDevice device, const Effect&, const Texture&, WGPUSampler sampler);

    WGPUBindGroup bind_group() { return m_bind_group; }
private:
    const Texture* m_texture;
    WGPUBindGroup m_bind_group;
};

#endif
