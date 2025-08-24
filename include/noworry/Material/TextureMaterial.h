#ifndef TEXTUREMATERIAL_H
#define TEXTUREMATERIAL_H

#include "Material.h"
#include "TextureEffect.h"
#include "../texture.h"
#include <webgpu/webgpu.h>

/// Material that maps a single texture onto the face.
class TextureMaterial : public Material
{
public:
    TextureMaterial(
        int id,
        WGPUDevice device,
        TextureEffect&,
        std::shared_ptr<const Texture>,
        WGPUSampler sampler);

    int id() { return m_id; }
    const Texture& texture() { return *m_texture; }
    virtual WGPUBindGroup bind_group() const override { return m_bind_group; }
    virtual TextureEffect& effect() const override { return *m_effect; }

private:
    int m_id;
    TextureEffect* m_effect;
    std::shared_ptr<const Texture> m_texture;
    WGPUBindGroup m_bind_group;
};

#endif
