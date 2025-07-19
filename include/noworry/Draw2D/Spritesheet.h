#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include "SpritePipeline.h"
#include "../texture.h"
#include <webgpu/webgpu.h>

class Spritesheet
{
public:
    Spritesheet(WGPUDevice device,
                const SpritePipeline&,
                const Texture&,
                WGPUSampler);
    Spritesheet(const Spritesheet&) = delete;
    Spritesheet(Spritesheet&&);

    Spritesheet& operator=(const Spritesheet&) = delete;
    Spritesheet& operator=(Spritesheet&&);

    virtual ~Spritesheet();

    WGPUBindGroup bind_group() const { return m_bind_group; }

private:
    const Texture* m_texture;
    WGPUBindGroup m_bind_group;

    void cleanup();
    void move(Spritesheet&&);
};

#endif
