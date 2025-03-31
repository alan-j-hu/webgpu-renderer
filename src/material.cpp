#include "noworry/material.h"

Material::Material(
    int id,
    WGPUDevice device,
    Pipeline& pipeline,
    const Texture& texture,
    WGPUSampler sampler)
{
    m_id = id;
    m_pipeline = &pipeline;
    m_texture = &texture;
    m_bind_group = pipeline
        .effect()
        .create_material_group(device, texture.view(), sampler);
}

