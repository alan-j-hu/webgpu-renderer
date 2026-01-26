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

    int id() const;
    const Texture& texture();
    virtual WGPUBindGroup bind_group() const override;
    virtual TextureEffect& effect() const override;
    virtual std::optional<const Texture*> diffuse_map() const override;

private:
    int m_id;
    TextureEffect* m_effect;
    std::shared_ptr<const Texture> m_texture;
    WGPUBindGroup m_bind_group;
};

#endif
