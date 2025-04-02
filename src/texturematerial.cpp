#include "noworry/texturematerial.h"

TextureMaterial::TextureMaterial(
    int id,
    WGPUDevice device,
    TextureMeshPipeline& pipeline,
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

