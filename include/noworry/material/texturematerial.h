#ifndef TEXTUREMATERIAL_H
#define TEXTUREMATERIAL_H

#include "material.h"
#include "texturemesheffect.h"
#include "../texture.h"
#include <webgpu/webgpu.h>

/// Material that maps a single texture onto the face.
class TextureMaterial : public Material
{
public:
    TextureMaterial(
        int id,
        WGPUDevice device,
        TextureMeshEffect&,
        std::shared_ptr<const Texture>,
        WGPUSampler sampler);

    int id() { return m_id; }
    const Texture& texture() { return *m_texture; }
    virtual WGPUBindGroup bind_group() override { return m_bind_group; }
    virtual TextureMeshEffect& effect() override { return *m_effect; }

private:
    int m_id;
    TextureMeshEffect* m_effect;
    std::shared_ptr<const Texture> m_texture;
    WGPUBindGroup m_bind_group;
};

#endif
