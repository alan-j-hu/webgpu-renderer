#include "noworry/Material/TextureMaterial.h"
#include <utility>

TextureMaterial::TextureMaterial(
    int id,
    WGPUDevice device,
    TextureEffect& effect,
    std::shared_ptr<const Texture> texture,
    WGPUSampler sampler)
{
    m_id = id;
    m_effect = &effect;
    m_texture = std::move(texture);
    m_bind_group = effect
        .create_material_group(device, m_texture->view(), sampler);
}


int TextureMaterial::id() const
{
    return m_id;
}

const Texture& TextureMaterial::texture()
{
    return *m_texture;
}

WGPUBindGroup TextureMaterial::bind_group() const
{
    return m_bind_group;
}

TextureEffect& TextureMaterial::effect() const
{
    return *m_effect;
}

std::optional<const Texture*> TextureMaterial::diffuse_map() const
{
    return m_texture.get();
}
