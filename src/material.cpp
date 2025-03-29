#include "noworry/material.h"

Material::Material(const Pipeline& pipeline,
                   const Texture& texture,
                   WGPUSampler sampler)
{
    m_texture = &texture;
    m_bind_group =
        pipeline.create_material_group(texture.view(), sampler);
}

