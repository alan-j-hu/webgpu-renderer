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
    m_texture = other.m_texture;
    m_bind_group = other.m_bind_group;

    other.m_texture = nullptr;
    other.m_bind_group = nullptr;
}

Spritesheet& Spritesheet::operator=(Spritesheet&& other)
{
    cleanup();
    move(std::move(other));
    return *this;
}

Spritesheet::~Spritesheet()
{
    cleanup();
}

void Spritesheet::cleanup()
{
    if (m_bind_group != nullptr) {
        wgpuBindGroupRelease(m_bind_group);
    }
}

void Spritesheet::move(Spritesheet&& other)
{
    m_texture = other.m_texture;
    m_bind_group = other.m_bind_group;

    other.m_texture = nullptr;
    other.m_bind_group = nullptr;
}
