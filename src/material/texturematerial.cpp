#include "noworry/material/texturematerial.h"

TextureMaterial::TextureMaterial(
    int id,
    WGPUDevice device,
    TextureMeshEffect& effect,
    const Texture& texture,
    WGPUSampler sampler)
{
    m_id = id;
    m_effect = &effect;
    m_texture = &texture;
    m_bind_group = effect
        .create_material_group(device, texture.view(), sampler);
}

