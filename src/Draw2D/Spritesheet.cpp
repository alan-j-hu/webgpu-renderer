#include "noworry/Draw2D/Spritesheet.h"
#include <utility>

Spritesheet::Spritesheet(WGPUDevice device,
                         const SpritePipeline& pipeline,
                         const Texture& texture,
                         WGPUSampler sampler)
    : m_texture(&texture)
{
    m_bind_group = pipeline.create_bind_group(device, texture.view(), sampler);
}

Spritesheet::Spritesheet(Spritesheet&& other)
{
    *this = std::move(other);
}

Spritesheet& Spritesheet::operator=(Spritesheet&& other)
{
    m_texture = std::exchange(other.m_texture, {});
    m_bind_group = std::exchange(other.m_bind_group, {});

    return *this;
}

Spritesheet::~Spritesheet()
{
    if (m_bind_group != nullptr) {
        wgpuBindGroupRelease(m_bind_group);
    }
}
