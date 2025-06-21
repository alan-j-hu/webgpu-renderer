#ifndef TEXTUREEFFECT_H
#define TEXTUREEFFECT_H

#include "Effect.h"

/// An effect that displays a texture.
class TextureEffect : public Effect
{
public:
    TextureEffect(WGPUDevice device);
    TextureEffect(const TextureEffect&) = delete;
    TextureEffect& operator=(const TextureEffect&) = delete;
    virtual ~TextureEffect();

    virtual WGPUShaderModule fragment_shader() override
    {
        return m_fragment_shader;
    }

    virtual WGPUBindGroupLayout layout() override
    {
        return m_material_layout;
    }

    WGPUBindGroup create_material_group(
        WGPUDevice, WGPUTextureView, WGPUSampler);

private:
    WGPUShaderModule m_fragment_shader;
    WGPUBindGroupLayout m_material_layout;
};

#endif
