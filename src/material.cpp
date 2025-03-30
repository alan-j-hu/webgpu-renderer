#include "noworry/material.h"

Material::Material(WGPUDevice device, const Effect& effect,
                   const Texture& texture,
                   WGPUSampler sampler)
{
    m_texture = &texture;
    m_bind_group =
        effect.create_material_group(device, texture.view(), sampler);
}

