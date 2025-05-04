#include "noworry/material/texturematerial.h"
#include <utility>

TextureMaterial::TextureMaterial(
    int id,
    WGPUDevice device,
    TextureMeshEffect& effect,
    std::shared_ptr<const Texture> texture,
    WGPUSampler sampler)
{
    m_id = id;
    m_effect = &effect;
    m_texture = std::move(texture);
    m_bind_group = effect
        .create_material_group(device, m_texture->view(), sampler);
}

